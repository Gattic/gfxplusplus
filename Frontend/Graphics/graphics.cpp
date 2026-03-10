// Copyright 2026 Robert Carneiro, Derek Meer, Matthew Tabak, Eric Lujan
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include "graphics.h"
#include "GfxRenderer.h"
#ifdef GFX_HAVE_OPENGL
#include "../GUI/Text/GLTextRenderer.h"
#endif
#include "../GItems/GItem.h"
#include "../GItems/GLayout.h"
#include "../GItems/GPanel.h"
#include "../GItems/RUColors.h"
#include "../GItems/RUComponent.h"
#include "../GUI/Text/GFont.h"
//#include "../GUI/Text/RULabel.h"
#ifdef GFX_HAVE_OPENGL
#include <GLFW/glfw3.h>
#endif
#include <string.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

// Monotonic-ish time in milliseconds for both backends.
// (C++98: helper free function)
static int32_t gfxpp_now_ms(const gfxpp* g)
{
#ifdef GFX_HAVE_OPENGL
	if (g && g->getRenderBackend() == gfxpp::RENDER_BACKEND_OPENGL)
		return (int32_t)(glfwGetTime() * 1000.0);
#endif
#ifdef GFX_HAVE_SDL2
	return (int32_t)SDL_GetTicks();
#else
#ifdef GFX_HAVE_OPENGL
	return (int32_t)(glfwGetTime() * 1000.0);
#else
	(void)g;
	return 0;
#endif
#endif
}

// Higher-resolution time in microseconds for profiling/logging.
static int64_t gfxpp_now_us(const gfxpp* g)
{
#ifdef GFX_HAVE_OPENGL
	if (g && g->getRenderBackend() == gfxpp::RENDER_BACKEND_OPENGL)
		return (int64_t)(glfwGetTime() * 1000000.0);
#endif
	// Fallback: wall-clock microseconds.
#ifdef _WIN32
	LARGE_INTEGER freq, cnt;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&cnt);
	return (int64_t)(cnt.QuadPart * 1000000LL / freq.QuadPart);
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (int64_t)tv.tv_sec * 1000000LL + (int64_t)tv.tv_usec;
#endif
}

static bool gfxpp_ui_profile_enabled()
{
	const char* v = getenv("GFXPP_UI_PROFILE");
	return (v && v[0] != '\0' && v[0] != '0');
}

static void gfxpp_collect_dirty_items(GItem* item, std::vector<GItem*>& out)
{
	if (!item)
		return;
	if (item->getDrawUpdateRequired())
		out.push_back(item);
	const std::vector<GItem*>& kids = item->getItems();
	for (size_t i = 0; i < kids.size(); ++i)
		gfxpp_collect_dirty_items(kids[i], out);
}

// GLFW callbacks are now in GlfwEventBridge.cpp

// Backend selection: change this variable to switch rendering backends
static gfxpp::RenderBackend gRenderBackend =
#ifdef GFX_HAVE_OPENGL
	gfxpp::RENDER_BACKEND_OPENGL;
#else
	gfxpp::RENDER_BACKEND_SDL2;
#endif

#ifdef GFX_HAVE_OPENGL
// GLFW is a process-global singleton. gfxpp may be constructed multiple times in
// the same process, so keep a tiny refcount to avoid double-terminating GLFW.
//
// Additionally, on some Wayland stacks (notably in combination with NVIDIA),
// calling glfwTerminate() can crash inside libwayland-client during
// wl_display_disconnect(). We therefore skip glfwTerminate() when GLFW is using
// the Wayland platform; the OS will reclaim resources at process exit.
static int g_glfw_refcount = 0;
static bool g_glfw_inited = false;

static bool gfxpp_env_true(const char* v)
{
	return (v && v[0] != '\0' && v[0] != '0');
}

static bool gfxpp_is_wayland_session()
{
	// Best-effort detection for older GLFW (< 3.4) where glfwGetPlatform() is unavailable.
	// If this returns true, we avoid glfwTerminate() to sidestep known Wayland/NVIDIA teardown issues.
	const char* wd = getenv("WAYLAND_DISPLAY");
	if (wd && wd[0] != '\0')
		return true;
	const char* st = getenv("XDG_SESSION_TYPE");
	if (st && strcmp(st, "wayland") == 0)
		return true;
	return false;
}

static bool gfxpp_should_terminate_glfw()
{
	// Allow override:
	// - GFXPP_GLFW_NO_TERMINATE=1 forces skipping termination
	// - GFXPP_GLFW_FORCE_TERMINATE=1 forces termination (useful for leak checks)
	if (gfxpp_env_true(getenv("GFXPP_GLFW_NO_TERMINATE")))
		return false;
	if (gfxpp_env_true(getenv("GFXPP_GLFW_FORCE_TERMINATE")))
		return true;

#if (GLFW_VERSION_MAJOR > 3) || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 4)
	if (glfwGetPlatform() == GLFW_PLATFORM_WAYLAND)
		return false;
#endif
	if (gfxpp_is_wayland_session())
		return false;
	return true;
}

static bool gfxpp_glfw_acquire()
{
	if (g_glfw_refcount == 0)
	{
#if (GLFW_VERSION_MAJOR > 3) || (GLFW_VERSION_MAJOR == 3 && GLFW_VERSION_MINOR >= 4)
		// If we previously skipped glfwTerminate() (Wayland teardown workaround),
		// GLFW is still initialized; don't attempt to re-init.
		if (!g_glfw_inited)
		{
			// Allow forcing platform at runtime (useful as a workaround for driver/Wayland issues).
			// Values: "x11" or "wayland"
			const char* p = getenv("GFXPP_GLFW_PLATFORM");
			if (p && p[0] != '\0')
			{
				if (strcmp(p, "x11") == 0)
					glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
				else if (strcmp(p, "wayland") == 0)
					glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
			}
		}
#endif

		if (!g_glfw_inited)
		{
			if (!glfwInit())
				return false;
			g_glfw_inited = true;
		}
	}
	++g_glfw_refcount;
	return true;
}

static void gfxpp_glfw_release()
{
	if (g_glfw_refcount <= 0)
		return;

	--g_glfw_refcount;
	if (g_glfw_refcount != 0)
		return;

	// Workaround: avoid known Wayland/NVIDIA teardown hangs/crashes.
	// If we skip termination, keep g_glfw_inited=true so we won't re-init later.
	if (!gfxpp_should_terminate_glfw())
		return;
	glfwTerminate();
	g_glfw_inited = false;
}
#endif

// Define non-integral static class member for broad standards compatibility
// 60fps improves perceived input responsiveness (hover, caret, drag).
const float gfxpp::MAX_FRAMES_PER_SECOND = 60.0f;

void gfxpp::initFonts(GfxNativeRenderer* sdlRenderer)
{
	fontManager.initSDL(sdlRenderer);
}

void gfxpp::initMembers()
{
	errorFlag = 0;
	running = false;
	width = 800;
	height = 600;

	frames = 0;
	fps = 0;
	rotate = false;
	move = false;
	now = 0;
	then = 0;

	input.reset();

	#ifdef GFX_HAVE_SDL2
	window = NULL;
	systemCursor = NULL;
	context = NULL;
	renderer = NULL;
	#endif
	glfwWindow = NULL;
	glfwInitialized = false;
#ifdef GFX_HAVE_OPENGL
	glfwBridge = NULL;
#endif
	renderBackend = gRenderBackend;
	ttfReady = false;
	finalized = false;
	ownsWindow = true;

	nextItemID = 1;
	focusedItem = NULL;
	focusedPanel = NULL;
}

gfxpp::gfxpp()
{
	initMembers();
	errorFlag = initHelper(false, "gfxplusplus", true);
}

gfxpp::gfxpp(shmea::GString newTitle, int newRenderStatus, bool fullScreenMode, bool compatMode, int newWidth, int newHeight)
{
	initMembers();
	width = newWidth;
	height = newHeight;
	errorFlag = initHelper(fullScreenMode, newTitle, compatMode);
}

// Construct with external GLFW window/context
#ifdef GFX_HAVE_OPENGL
gfxpp::gfxpp(GLFWwindow* externalWindow, int newWidth, int newHeight)
{
	initMembers();
	ownsWindow = false;
	width = newWidth;
	height = newHeight;
	glfwWindow = externalWindow;

	// Skip creating window; set backend to OpenGL and init
	if (!glfwWindow)
	{
		errorFlag = -2;
		return;
	}
	// When external window is provided, force OpenGL backend
	renderBackend = RENDER_BACKEND_OPENGL;
	// Minimal init
	int err = init2D(true);
	if (err < 0) { errorFlag = err; return; }
	this->draw = shmea::GPointer<GfxRenderer>(GfxRenderer::createOpenGL(glfwWindow));

	// Hook GLFW callbacks via bridge
	glfwBridge = new GlfwEventBridge();
	glfwBridge->attach(glfwWindow, width, height);
	then = (int32_t)(glfwGetTime() * 1000.0);
	errorFlag = 0;
}
#endif

// Construct with external SDL window/renderer
#ifdef GFX_HAVE_SDL2
gfxpp::gfxpp(SDL_Window* externalWindow, SDL_Renderer* externalRenderer, int newWidth, int newHeight)
{
	initMembers();
	ownsWindow = false;
	width = newWidth;
	height = newHeight;
	renderer = externalRenderer;

	if (!externalWindow || !externalRenderer)
	{
		errorFlag = -2;
		return;
	}
	window = externalWindow;
	renderBackend = RENDER_BACKEND_SDL2;
	int err = init2D(true);
	if (err < 0) { errorFlag = err; return; }
	this->draw = shmea::GPointer<GfxRenderer>(GfxRenderer::createSDL(renderer, window));
	then = SDL_GetTicks();
	errorFlag = 0;
}
#endif

int gfxpp::getErrorFlag() const
{
	return errorFlag;
}

#ifdef GFX_HAVE_SDL2
SDL_Renderer* gfxpp::getRenderer()
{
	return renderer;
}
#endif

int gfxpp::initHelper(bool fullscreenMode, shmea::GString title, bool compatMode)
{
	// Member variables already initialized by initMembers() in each constructor.

	if (renderBackend == RENDER_BACKEND_OPENGL)
	{
#ifdef GFX_HAVE_OPENGL
		// GLFW path: initialize and create OpenGL window/context
		if (!glfwInitialized)
		{
			if (!gfxpp_glfw_acquire())
			{
				printf("[GFX] GLFW init failed\n");
				finish();
				return -1;
			}
			glfwInitialized = true;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, fullscreenMode ? GLFW_FALSE : GLFW_TRUE);

		GLFWmonitor* monitor = NULL;
		if (fullscreenMode)
		{
			// In fullscreen, use monitor's video mode for the window, but keep logical size
			// unchanged so UI scales like SDL_RenderSetLogicalSize
			monitor = glfwGetPrimaryMonitor();
		}

		glfwWindow = glfwCreateWindow(width, height, title.c_str(), monitor, NULL);
		if (!glfwWindow)
		{
			printf("[GFX] GLFW window creation failed\n");
			finish();
			return -2;
		}

		glfwMakeContextCurrent(glfwWindow);
		glfwSwapInterval(1);

		// Hook GLFW callbacks via bridge
		glfwBridge = new GlfwEventBridge();
		glfwBridge->attach(glfwWindow, width, height);

		// Set initial time base
		then = (int32_t)(glfwGetTime() * 1000.0);
#else
		return -1;
#endif
	}
	else
	{
		// SDL2 path
#ifdef GFX_HAVE_SDL2
		int sdlStatus = SDL_Init(SDL_INIT_VIDEO);
		if (sdlStatus < 0)
		{
			printf("[GFX] Initialization error: %s\n", SDL_GetError());
			finish();
			return -1;
		}

		//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

		// Create a new window
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		Uint32 windowFlags = 0;
		if (fullscreenMode)
			windowFlags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
		else
			windowFlags |= SDL_WINDOW_RESIZABLE;

		window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
					 getWidth(), getHeight(), windowFlags);

		if (fullscreenMode)
		{
			// get the width and height
			SDL_DisplayMode DM;
			int errorNo = SDL_GetDesktopDisplayMode(0, &DM);
			if (errorNo < 0)
				return -5;
			width = DM.w;
			height = DM.h;
		}

		if (!window)
		{
			printf("[GFX] Window error: %s\n", SDL_GetError());
			finish();
			return -2;
		}

		then = SDL_GetTicks();
#else
		printf("[GFX] SDL2 backend requested but not compiled.\n");
		return -1;
#endif
	}

	int errorNo = init2D(compatMode);
	if (errorNo < 0)
	    return errorNo;

	// Create drawing helper based on backend
	if (renderBackend == RENDER_BACKEND_OPENGL)
	{
#ifdef GFX_HAVE_OPENGL
		if (glfwWindow)
			this->draw = shmea::GPointer<GfxRenderer>(GfxRenderer::createOpenGL(glfwWindow));
		else
		{
#ifdef GFX_HAVE_SDL2
			this->draw = shmea::GPointer<GfxRenderer>(GfxRenderer::createOpenGL(window, context));
#endif
		}
#endif
	}
	else
	{
#ifdef GFX_HAVE_SDL2
		this->draw = shmea::GPointer<GfxRenderer>(GfxRenderer::createSDL(renderer, window));
#endif
	}

	// Set the SDL system cursor (SDL path only)
#ifdef GFX_HAVE_SDL2
	if (renderBackend == RENDER_BACKEND_SDL2)
		systemCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
#endif

	// ALl good with initialization, return success
	return 0;
}

int gfxpp::init2D(bool compatMode)
{
	// If backend is OpenGL, delegate to OpenGL initialization
	if (renderBackend == RENDER_BACKEND_OPENGL)
		return initOpenGL();

	#ifdef GFX_HAVE_SDL2
	// Create a new renderer; -1 loads the default video driver we need
	if (!renderer)
	{
		if(compatMode)
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE);
		else
			renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	}
	if (!renderer)
	{
		printf("[GFX] Renderer error: %s\n", SDL_GetError());
		finish();
		return -3;
	}
	else
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	// Init ttf (SDL2-only path)
	#ifdef GFX_HAVE_SDL2
	if (renderBackend == RENDER_BACKEND_SDL2)
	{
		if (!ttfReady && TTF_Init() == -1)
		{
			printf("[GFX] TTF Init error: %s\n", SDL_GetError());
			finish();
			return -4;
		}
		ttfReady = true;
	}
	#endif

	initFonts(renderer);

	// Load support for the PNG, TIF, and JPG image formats
	#ifdef GFX_HAVE_SDL2
	int flags = IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_JPG;
	int imgInitStatus = IMG_Init(flags);
	if ((imgInitStatus & flags) != flags)
	{
		printf("[GFX] Image Init error: %s\n", IMG_GetError());
		return -5;
	}
	#endif

	return 0;
	#else
	(void)compatMode;
	return -1;
	#endif
}

int gfxpp::initOpenGL()
{
	// If GLFW path, context is already current
#ifdef GFX_HAVE_OPENGL
	if (glfwWindow)
	{
		glfwMakeContextCurrent(glfwWindow);
	}
	else
	{
		// Create the GL context via SDL if available
		#ifdef GFX_HAVE_SDL2
		context = SDL_GL_CreateContext(window);
		if (!context)
		{
			printf("[GFX] OpenGL Context error: %s\n", SDL_GetError());
			finish();
			return -3;
		}
		// Enable VSync if possible
		SDL_GL_SetSwapInterval(1);
		#else
		return -3;
		#endif
	}

	// Basic viewport and fixed-function 2D setup (top-left origin, pixel coords)
	glViewport(0, 0, getWidth(), getHeight());
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Set orthographic projection matching window size
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// left, right, bottom, top, near, far (y flipped so origin is top-left)
	glOrtho(0.0, (GLdouble)getWidth(), (GLdouble)getHeight(), 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Default clear color transparent black
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Initialize font wrappers for GL path (no SDL_ttf needed here)
	fontManager.initOpenGL();

	return 0;
#else
	return -1;
#endif
}

bool gfxpp::updateKeyState(int eventType, GfxKeycode key)
{
	return input.handleKeyEvent(eventType, key);
}

void gfxpp::run()
{
	// the display loop
	display();
	finish();
}

void gfxpp::display()
{
	running = true;
	frames = 0;
	fps = 0;
	rotate = false;
	move = false;
	now = 0;
	#ifdef GFX_HAVE_OPENGL
	if (renderBackend == RENDER_BACKEND_OPENGL)
		then = (int32_t)(glfwGetTime() * 1000.0);
	else
	#endif
	{
	#ifdef GFX_HAVE_SDL2
		then = SDL_GetTicks();
	#else
		#ifdef GFX_HAVE_OPENGL
		then = (int32_t)(glfwGetTime() * 1000.0);
		#else
		then = 0;
		#endif
	#endif
	}

	input.reset();

	// Pre-layout warmup: stabilize auto-sized components (labels, buttons, textboxes)
	// so their final sizes are known before the first visible frame is presented.
	// We run a small number of passes: layout -> background/helpers -> layout.
	if (focusedPanel && this->draw)
	{
		std::pair<int,int> zero(0,0);
		focusedPanel->calculateSubItemPositions(zero);
		// Trigger components (e.g., RUTextComponent) to compute auto sizes during their helpers
		focusedPanel->updateBackgroundHelper(this);
		// Recompute layout once more using final measured sizes
		focusedPanel->calculateSubItemPositions(zero);
	}

	// draw/event loop
	int32_t fpsWindowStartMs = gfxpp_now_ms(this);
	int32_t fpsWindowFrames = 0;
	int lastLogicalW = -1;
	int lastLogicalH = -1;

	// Optional perf profiling (enable with env var: GFXPP_UI_PROFILE=1)
	const bool uiProfile = gfxpp_ui_profile_enabled();
	int64_t profWindowStartUs = gfxpp_now_us(this);
	int64_t profEventDispatchUs = 0;
	int64_t profEventPumpUs = 0;
	int64_t profRenderUs = 0;
	int64_t profPresentUs = 0;
	int64_t profMaxEventDispatchUs = 0;
	int64_t profMaxRenderUs = 0;
	int64_t profMaxPresentUs = 0;
	uint32_t profEvents = 0;
	uint32_t profMotionEvents = 0;
	uint32_t profKeyEvents = 0;

	while (running)
	{
		++frames;
		++fpsWindowFrames;
		const int32_t frameStartMs = gfxpp_now_ms(this);
		const int64_t frameStartUs = uiProfile ? gfxpp_now_us(this) : 0;

		//=================EVENTS=================
		#ifdef GFX_HAVE_OPENGL
		if (renderBackend == RENDER_BACKEND_OPENGL && glfwWindow)
			glfwPollEvents();
		if (renderBackend == RENDER_BACKEND_OPENGL && glfwBridge)
		{
			const int64_t pumpStartUs = uiProfile ? gfxpp_now_us(this) : 0;
			if (glfwBridge->isCloseRequested())
				running = false;
			std::vector<GfxEvent> glfwEvents;
			glfwBridge->drainEvents(glfwEvents);
			for (size_t qi = 0; qi < glfwEvents.size(); ++qi)
			{
				GfxEvent e2 = glfwEvents[qi];
				if (e2.type == SDL_MOUSEMOTION) { input.mouseX = e2.motion.x; input.mouseY = e2.motion.y; }
				else if (e2.type == SDL_MOUSEBUTTONDOWN || e2.type == SDL_MOUSEBUTTONUP) { input.mouseX = e2.button.x; input.mouseY = e2.button.y; }
				if (e2.type == SDL_KEYUP || e2.type == SDL_KEYDOWN)
				{
					if (updateKeyState(e2.type, e2.key.keysym.sym))
						running = false;
				}
				if (e2.type == SDL_QUIT) running = false;
				if (focusedPanel)
				{
					const int64_t dispatchStartUs = uiProfile ? gfxpp_now_us(this) : 0;
					focusedPanel->processSubItemEvents(this, NULL, NULL, e2, input.mouseX, input.mouseY);
					if (uiProfile)
					{
						const int64_t dt = gfxpp_now_us(this) - dispatchStartUs;
						profEventDispatchUs += dt;
						if (dt > profMaxEventDispatchUs) profMaxEventDispatchUs = dt;
					}
				}
				for (size_t li = 0; li < listeners.size(); ++li)
					listeners[li].fn(e2, listeners[li].userData);

				if (uiProfile)
				{
					++profEvents;
					if (e2.type == SDL_MOUSEMOTION) ++profMotionEvents;
					else if (e2.type == SDL_KEYDOWN || e2.type == SDL_KEYUP) ++profKeyEvents;
				}
			}
			if (uiProfile) profEventPumpUs += (gfxpp_now_us(this) - pumpStartUs);
		}
		#endif

		#ifdef GFX_HAVE_SDL2
		if (renderBackend == RENDER_BACKEND_SDL2)
		{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			const int64_t pumpStartUs = uiProfile ? gfxpp_now_us(this) : 0;
			if (event.type == SDL_QUIT)
				running = false;

			if ((event.type == SDL_KEYUP) || (event.type == SDL_KEYDOWN))
			{
				if (updateKeyState(event.type, event.key.keysym.sym))
					running = false;
			}

			if ((event.type == SDL_MOUSEBUTTONDOWN) || (event.type == SDL_MOUSEBUTTONUP) || (event.type == SDL_MOUSEMOTION))
			{
				input.mouseX = event.button.x;
				input.mouseY = event.button.y;
			}

			if (focusedPanel)
			{
				const int64_t dispatchStartUs = uiProfile ? gfxpp_now_us(this) : 0;
				focusedPanel->processSubItemEvents(this, NULL, NULL, event, input.mouseX, input.mouseY);
				if (uiProfile)
				{
					const int64_t dt = gfxpp_now_us(this) - dispatchStartUs;
					profEventDispatchUs += dt;
					if (dt > profMaxEventDispatchUs) profMaxEventDispatchUs = dt;
				}
			}

			for (size_t li = 0; li < listeners.size(); ++li)
				listeners[li].fn(event, listeners[li].userData);

			if (uiProfile)
			{
				++profEvents;
				if (event.type == SDL_MOUSEMOTION) ++profMotionEvents;
				else if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) ++profKeyEvents;
				profEventPumpUs += (gfxpp_now_us(this) - pumpStartUs);
			}
		}
		} // if (renderBackend == RENDER_BACKEND_SDL2)
		#endif

		// If a quit was requested (window close, ESC, etc.), stop immediately.
		// Some platforms/drivers can hang if we continue to render/present after the
		// window has begun tearing down.
		#ifdef GFX_HAVE_OPENGL
		if (renderBackend == RENDER_BACKEND_OPENGL && glfwWindow)
		{
			// Backup: in case a close callback is not installed/triggered, honor the
			// native close request directly.
			if (glfwWindowShouldClose(glfwWindow))
				running = false;
		}
		#endif
		if (!running)
			break;

		//=================Render=================
		const int64_t renderStartUs = uiProfile ? gfxpp_now_us(this) : 0;
		if (this->draw)
		{
			// Ensure viewport/projection up-to-date before drawing; reset modelview
			const int w = getWidth();
			const int h = getHeight();
			if (w != lastLogicalW || h != lastLogicalH)
			{
				this->draw->setLogicalSize(w, h);
				lastLogicalW = w;
				lastLogicalH = h;
			}
		#ifdef GFX_HAVE_OPENGL
			if (renderBackend == RENDER_BACKEND_OPENGL)
			{
				glMatrixMode(GL_MODELVIEW);
				glLoadIdentity();
			}
		#endif

			// Debug: render loop heartbeat (every ~60 frames)
			/*if ((frames % 60) == 0)
				printf("[GFX] frame=%d backend=%s size=%dx%d focusedPanel=%s guiCount=%zu\n",
					frames,
					(renderBackend == RENDER_BACKEND_OPENGL ? "OpenGL" : "SDL2"),
					getWidth(), getHeight(),
					(focusedPanel ? "yes" : "no"), guiElements.size());*/

			// Clear every frame
		#ifdef GFX_HAVE_OPENGL
			if (renderBackend == RENDER_BACKEND_OPENGL)
			{
				this->draw->setDrawColor(RUColors::DEFAULT_COLOR_BACKGROUND.r,
					RUColors::DEFAULT_COLOR_BACKGROUND.g,
					RUColors::DEFAULT_COLOR_BACKGROUND.b,
					RUColors::DEFAULT_COLOR_BACKGROUND.a);
			}
			else
			{
				this->draw->setDrawColor(0, 0, 0, 0);
			}
		#else
			this->draw->setDrawColor(0, 0, 0, 0);
		#endif
			this->draw->clear();

			if (focusedPanel)
				focusedPanel->updateBackgroundHelper(this);
			else
			{
				for (unsigned int i = 0; i < guiElements.size(); ++i)
				{
					GItem* cItem = guiElements[i];
					if (cItem)
						cItem->updateBackgroundHelper(this);
				}
			}
		}
		if (uiProfile)
		{
			const int64_t dt = gfxpp_now_us(this) - renderStartUs;
			profRenderUs += dt;
			if (dt > profMaxRenderUs) profMaxRenderUs = dt;
		}

		// Present every frame so interactive elements (caret) update reliably
		if (this->draw)
		{
			const int64_t presentStartUs = uiProfile ? gfxpp_now_us(this) : 0;
			this->draw->present();
			if (uiProfile)
			{
				const int64_t dt = gfxpp_now_us(this) - presentStartUs;
				profPresentUs += dt;
				if (dt > profMaxPresentUs) profMaxPresentUs = dt;
			}
		}

		// Periodic UI perf log (once per second)
		if (uiProfile)
		{
			const int64_t nowUs = gfxpp_now_us(this);
			const int64_t windowUs = nowUs - profWindowStartUs;
			if (windowUs >= 1000000LL)
			{
				std::vector<GItem*> dirty;
				if (focusedPanel)
					gfxpp_collect_dirty_items(focusedPanel, dirty);

				const double windowMs = (double)windowUs / 1000.0;
				const double avgEventDispatchUs = (profEvents > 0) ? ((double)profEventDispatchUs / (double)profEvents) : 0.0;

				printf("[UI PERF] %.0fms window fps=%.1f events=%u (motion=%u key=%u) "
					   "pump=%.1fms dispatch=%.1fms(avg=%.1fus max=%.1fus) "
					   "render=%.1fms(max=%.1fus) present=%.1fms(max=%.1fus) dirty=%zu\n",
					   windowMs, fps,
					   (unsigned int)profEvents, (unsigned int)profMotionEvents, (unsigned int)profKeyEvents,
					   (double)profEventPumpUs / 1000.0,
					   (double)profEventDispatchUs / 1000.0,
					   avgEventDispatchUs, (double)profMaxEventDispatchUs,
					   (double)profRenderUs / 1000.0, (double)profMaxRenderUs,
					   (double)profPresentUs / 1000.0, (double)profMaxPresentUs,
					   dirty.size());

				// Print a few dirty items to spot what's constantly re-rendering.
				const size_t maxPrint = 8;
				for (size_t i = 0; i < dirty.size() && i < maxPrint; ++i)
				{
					GItem* it = dirty[i];
					printf("  dirty[%u]: type=%s name=%s id=%d w=%d h=%d\n",
						   (unsigned int)i,
						   it->getType().c_str(),
						   it->getName().c_str(),
						   it->getID(),
						   it->getWidth(), it->getHeight());
				}

				// Reset window counters
				profWindowStartUs = nowUs;
				profEventDispatchUs = 0;
				profEventPumpUs = 0;
				profRenderUs = 0;
				profPresentUs = 0;
				profMaxEventDispatchUs = 0;
				profMaxRenderUs = 0;
				profMaxPresentUs = 0;
				profEvents = 0;
				profMotionEvents = 0;
				profKeyEvents = 0;
			}
		}

		// Frame pacing: cap based on actual frame duration (not average FPS).
		// The previous code used average FPS since startup, which could cause huge sleeps
		// early on and make input feel unresponsive.
		int32_t frameEndMs = gfxpp_now_ms(this);
		int32_t frameMs = frameEndMs - frameStartMs;
		if (frameMs < 1) frameMs = 1;

		if (MAX_FRAMES_PER_SECOND > 0.0f)
		{
			int targetMs = (int)(1000.0f / MAX_FRAMES_PER_SECOND);
			if (targetMs < 1) targetMs = 1;
			if (frameMs < targetMs)
			{
				int sleepMs = targetMs - frameMs;
#ifdef GFX_HAVE_SDL2
				if (renderBackend == RENDER_BACKEND_SDL2)
					SDL_Delay((Uint32)sleepMs);
				else
#endif
#ifdef _WIN32
					Sleep((DWORD)sleepMs);
#else
					usleep((useconds_t)(sleepMs * 1000));
#endif

				frameEndMs = gfxpp_now_ms(this);
				frameMs = frameEndMs - frameStartMs;
				if (frameMs < 1) frameMs = 1;
			}
		}

		// Update FPS a few times per second (stable and cheap).
		if ((frameEndMs - fpsWindowStartMs) >= 500)
		{
			const int32_t windowMs = frameEndMs - fpsWindowStartMs;
			if (windowMs > 0)
				fps = ((float)fpsWindowFrames * 1000.0f) / (float)windowMs;
			fpsWindowStartMs = frameEndMs;
			fpsWindowFrames = 0;
		}
	}
}

// UPDATE FUNCTION
void gfxpp::clean2D()
{
	// clean up the componenets
	focusedItem = NULL;
	// Also clear the focusedPanel to avoid dereferencing a freed panel
	focusedPanel = NULL;

	// gui
	for (unsigned int i = 0; i < guiElements.size(); ++i)
	{
		GItem* cItem = guiElements[i];
		if (cItem)
			delete cItem;
	}
	guiElements.clear();

	#ifdef GFX_HAVE_SDL2
	if (renderer)
	{
		//SDL_DestroyRenderer(renderer); // TODO: CRASHES
		renderer = NULL;
	}
	#endif

	fontManager.cleanup(renderBackend == RENDER_BACKEND_OPENGL);
}

void gfxpp::finish()
{
	if (finalized)
		return;
	running = false;
	// Ensure no stale UI pointers remain before cleaning up UI elements
	focusedPanel = NULL;
	clean2D();

	this->draw.reset();

#ifdef GFX_HAVE_OPENGL
	if (renderBackend == RENDER_BACKEND_OPENGL)
	{
		// Destroy only if we own the GLFW lifecycle (created internally)
		if (glfwBridge)
		{
			glfwBridge->detach(glfwWindow);
			delete glfwBridge;
			glfwBridge = NULL;
		}
		if (glfwWindow && glfwInitialized && ownsWindow)
		{
			// Clear current context before destroy to avoid dangling current context.
			glfwMakeContextCurrent(NULL);
			glfwDestroyWindow(glfwWindow);
			glfwWindow = NULL;
		}
		if (glfwInitialized)
		{
			gfxpp_glfw_release();
			glfwInitialized = false;
		}
		#ifdef GFX_HAVE_SDL2
		if (context)
		{
			SDL_GL_DeleteContext(context);
			context = NULL;
		}
		#endif
	}
#endif

	// Destroy SDL window only if we created it
	#ifdef GFX_HAVE_SDL2
	if (window && (renderBackend == RENDER_BACKEND_SDL2) && ownsWindow)
	{
		SDL_DestroyWindow(window);
		window = NULL;
	}
	#endif

#ifdef GFX_HAVE_SDL2
	if (systemCursor)
	{
		SDL_FreeCursor(systemCursor);
		systemCursor = NULL;
	}
	if (ttfReady)
		TTF_Quit();
	ttfReady = false;
	IMG_Quit();
#endif
	#ifdef GFX_HAVE_SDL2
	if (renderBackend == RENDER_BACKEND_SDL2)
		SDL_Quit();
	#endif
	finalized = true;
}

gfxpp::~gfxpp()
{
	finish();
}

bool gfxpp::getRunning() const
{
	return running;
}

unsigned int gfxpp::RGBfromHue(double hue, int8_t* r, int8_t* g, int8_t* b)
{
	int h = int(hue * 256 * 6);
	int x = h % 0x100;

	(*r) = 0;
	(*g) = 0;
	(*b) = 0;
	switch (h / 256)
	{
	case 0:
		(*r) = 0xFF;
		(*g) = x;
		break;
	case 1:
		(*g) = 0xFF;
		(*r) = 0xFF - x;
		break;
	case 2:
		(*g) = 0xFF;
		(*b) = x;
		break;
	case 3:
		(*b) = 0xFF;
		(*g) = 0xFF - x;
		break;
	case 4:
		(*b) = 0xFF;
		(*r) = x;
		break;
	case 5:
		(*r) = 0xFF;
		(*b) = 0xFF - x;
		break;
	}

	return (*r) + ((*g) << 8) + ((*b) << 16);
}

GfxCursor* gfxpp::getSystemCursor()
{
#ifdef GFX_HAVE_SDL2
	return systemCursor;
#else
	return NULL;
#endif
}

void gfxpp::setCursor(GfxCursor* newCursor)
{
#ifdef GFX_HAVE_SDL2
	if (newCursor)
		SDL_SetCursor(newCursor);
#else
	(void)newCursor;
#endif
}

void gfxpp::addItem(GItem* newItem)
{
	if (!newItem)
		return;

	newItem->setID(nextItemID++);
	guiElements.push_back(newItem);
}

// Unique ID for each item, but names may be the same.
void gfxpp::removeItem(int itemID)
{
	if (!itemID)
		return;

	for (unsigned int i = 0; i < guiElements.size(); ++i)
	{
		if (guiElements[i]->getID() == itemID)
		{
			GItem* removed = guiElements[i];
			if (focusedItem == removed)
				focusedItem = NULL;
			guiElements.erase(guiElements.begin() + i);
			delete removed;
			break;
		}
	}
}

// Unique ID for each item, but names may be the same.
GItem* gfxpp::getItemByID(int itemID)
{
	for (unsigned int i = 0; i < guiElements.size(); ++i)
	{
		if (guiElements[i]->getID() == itemID)
			return guiElements[i];
	}
	return NULL;
}

void gfxpp::setFocus(GItem* newFocusedItem)
{
	if (!newFocusedItem)
		return;

	if (focusedItem)
	{
		focusedItem->unsetFocus();
		if (focusedPanel)
			focusedItem->triggerLoseFocusEvent(focusedPanel);
	}

	focusedItem = newFocusedItem;
	focusedItem->setFocus();
}

int gfxpp::getWidth() const
{
	return width;
}

int gfxpp::getHeight() const
{
	return height;
}

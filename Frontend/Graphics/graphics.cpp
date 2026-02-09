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
#include <unistd.h>
#include <string.h>
#include <sys/time.h>

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
	struct timeval tv;
	gettimeofday(&tv, NULL);
	return (int64_t)tv.tv_sec * 1000000LL + (int64_t)tv.tv_usec;
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
	std::vector<GItem*> kids = item->getItems();
	for (size_t i = 0; i < kids.size(); ++i)
		gfxpp_collect_dirty_items(kids[i], out);
}

// GLFW -> SDL event translation helpers (C++98: use free functions)
#ifdef GFX_HAVE_OPENGL
void glfw_window_close_cb(GLFWwindow* w)
{
	gfxpp* self = reinterpret_cast<gfxpp*>(glfwGetWindowUserPointer(w));
	if (!self) return;
	// Always stop the render loop immediately on close.
	// Some host programs may not pump our synthetic event queue, so relying on
	// an enqueued SDL_QUIT alone can cause a "hang on close" (run() never returns).
	self->running = false;
#ifdef GFX_HAVE_SDL2
	// Still enqueue SDL_QUIT so listener callbacks get consistent semantics.
	GfxEvent e;
	memset(&e, 0, sizeof(GfxEvent));
	e.type = SDL_QUIT;
	self->glfwEventQueue.push_back(e);
#endif
}

void glfw_key_cb(GLFWwindow* w, int key, int scancode, int action, int mods)
{
	gfxpp* self = reinterpret_cast<gfxpp*>(glfwGetWindowUserPointer(w));
	if (!self) return;
	GfxEvent e; memset(&e, 0, sizeof(GfxEvent));
	if (action == GLFW_PRESS || action == GLFW_REPEAT) e.type = SDL_KEYDOWN; else if (action == GLFW_RELEASE) e.type = SDL_KEYUP; else return;
	int sdlk = 0;
	switch (key)
	{
		// Modifiers and controls
		case GLFW_KEY_LEFT_CONTROL: sdlk = SDLK_LCTRL; break;
		case GLFW_KEY_RIGHT_CONTROL: sdlk = SDLK_RCTRL; break;
		case GLFW_KEY_LEFT_ALT: sdlk = SDLK_LALT; break;
		case GLFW_KEY_RIGHT_ALT: sdlk = SDLK_RALT; break;
		case GLFW_KEY_SPACE: sdlk = SDLK_SPACE; break;
		// Letters
		case GLFW_KEY_A: sdlk = SDLK_a; break;
		case GLFW_KEY_B: sdlk = SDLK_b; break;
		case GLFW_KEY_C: sdlk = SDLK_c; break;
		case GLFW_KEY_D: sdlk = SDLK_d; break;
		case GLFW_KEY_E: sdlk = SDLK_e; break;
		case GLFW_KEY_F: sdlk = SDLK_f; break;
		case GLFW_KEY_G: sdlk = SDLK_g; break;
		case GLFW_KEY_H: sdlk = SDLK_h; break;
		case GLFW_KEY_I: sdlk = SDLK_i; break;
		case GLFW_KEY_J: sdlk = SDLK_j; break;
		case GLFW_KEY_K: sdlk = SDLK_k; break;
		case GLFW_KEY_L: sdlk = SDLK_l; break;
		case GLFW_KEY_M: sdlk = SDLK_m; break;
		case GLFW_KEY_N: sdlk = SDLK_n; break;
		case GLFW_KEY_O: sdlk = SDLK_o; break;
		case GLFW_KEY_P: sdlk = SDLK_p; break;
		case GLFW_KEY_Q: sdlk = SDLK_q; break;
		case GLFW_KEY_R: sdlk = SDLK_r; break;
		case GLFW_KEY_S: sdlk = SDLK_s; break;
		case GLFW_KEY_T: sdlk = SDLK_t; break;
		case GLFW_KEY_U: sdlk = SDLK_u; break;
		case GLFW_KEY_V: sdlk = SDLK_v; break;
		case GLFW_KEY_W: sdlk = SDLK_w; break;
		case GLFW_KEY_X: sdlk = SDLK_x; break;
		case GLFW_KEY_Y: sdlk = SDLK_y; break;
		case GLFW_KEY_Z: sdlk = SDLK_z; break;
		// Numbers
		case GLFW_KEY_0: sdlk = SDLK_0; break;
		case GLFW_KEY_1: sdlk = SDLK_1; break;
		case GLFW_KEY_2: sdlk = SDLK_2; break;
		case GLFW_KEY_3: sdlk = SDLK_3; break;
		case GLFW_KEY_4: sdlk = SDLK_4; break;
		case GLFW_KEY_5: sdlk = SDLK_5; break;
		case GLFW_KEY_6: sdlk = SDLK_6; break;
		case GLFW_KEY_7: sdlk = SDLK_7; break;
		case GLFW_KEY_8: sdlk = SDLK_8; break;
		case GLFW_KEY_9: sdlk = SDLK_9; break;
		// Navigation
		case GLFW_KEY_UP: sdlk = SDLK_UP; break;
		case GLFW_KEY_DOWN: sdlk = SDLK_DOWN; break;
		case GLFW_KEY_LEFT: sdlk = SDLK_LEFT; break;
		case GLFW_KEY_RIGHT: sdlk = SDLK_RIGHT; break;
		case GLFW_KEY_HOME: sdlk = SDLK_HOME; break;
		case GLFW_KEY_END: sdlk = SDLK_END; break;
		case GLFW_KEY_BACKSPACE: sdlk = SDLK_BACKSPACE; break;
		case GLFW_KEY_DELETE: sdlk = SDLK_DELETE; break;
		case GLFW_KEY_ESCAPE: sdlk = SDLK_ESCAPE; break;
		// Symbols (common US layout)
		case GLFW_KEY_MINUS: sdlk = SDLK_MINUS; break;
		case GLFW_KEY_EQUAL: sdlk = SDLK_EQUALS; break;
		case GLFW_KEY_LEFT_BRACKET: sdlk = SDLK_LEFTBRACKET; break;
		case GLFW_KEY_RIGHT_BRACKET: sdlk = SDLK_RIGHTBRACKET; break;
		case GLFW_KEY_BACKSLASH: sdlk = SDLK_BACKSLASH; break;
		case GLFW_KEY_SEMICOLON: sdlk = SDLK_SEMICOLON; break;
		case GLFW_KEY_APOSTROPHE: sdlk = SDLK_QUOTE; break;
		case GLFW_KEY_GRAVE_ACCENT: sdlk = SDLK_BACKQUOTE; break;
		case GLFW_KEY_COMMA: sdlk = SDLK_COMMA; break;
		case GLFW_KEY_PERIOD: sdlk = SDLK_PERIOD; break;
		case GLFW_KEY_SLASH: sdlk = SDLK_SLASH; break;
		case GLFW_KEY_TAB: sdlk = SDLK_TAB; break;
		default: return;
	}
	e.key.keysym.sym = (SDL_Keycode)sdlk;
	// Modifiers
	Uint16 keymods = 0;
	if (mods & GLFW_MOD_SHIFT) keymods |= KMOD_SHIFT;
	if (mods & GLFW_MOD_CONTROL) keymods |= KMOD_CTRL;
	if (mods & GLFW_MOD_ALT) keymods |= KMOD_ALT;
#ifdef GLFW_MOD_CAPS_LOCK
	if (mods & GLFW_MOD_CAPS_LOCK) keymods |= KMOD_CAPS;
#endif
	e.key.keysym.mod = keymods;
	self->glfwEventQueue.push_back(e);
}

void glfw_mouse_button_cb(GLFWwindow* w, int button, int action, int mods)
{
	gfxpp* self = reinterpret_cast<gfxpp*>(glfwGetWindowUserPointer(w));
	if (!self) return;
	double cx=0, cy=0; glfwGetCursorPos(w, &cx, &cy);
	int winW=0, winH=0; glfwGetWindowSize(w, &winW, &winH);
	if (winW <= 0) winW = 1;
	if (winH <= 0) winH = 1;
	double sx = (double)self->getWidth() / (double)winW;
	double sy = (double)self->getHeight() / (double)winH;
	GfxEvent e; memset(&e, 0, sizeof(GfxEvent));
	if (action == GLFW_PRESS) e.type = SDL_MOUSEBUTTONDOWN; else if (action == GLFW_RELEASE) e.type = SDL_MOUSEBUTTONUP; else return;
	e.button.x = (int)(cx * sx); e.button.y = (int)(cy * sy);
	switch (button)
	{
		case GLFW_MOUSE_BUTTON_LEFT: e.button.button = SDL_BUTTON_LEFT; break;
		case GLFW_MOUSE_BUTTON_RIGHT: e.button.button = SDL_BUTTON_RIGHT; break;
		case GLFW_MOUSE_BUTTON_MIDDLE: e.button.button = SDL_BUTTON_MIDDLE; break;
		default: e.button.button = 0; break;
	}
	self->glfwEventQueue.push_back(e);
}

void glfw_cursor_pos_cb(GLFWwindow* w, double x, double y)
{
	gfxpp* self = reinterpret_cast<gfxpp*>(glfwGetWindowUserPointer(w));
	if (!self) return;
	int winW=0, winH=0; glfwGetWindowSize(w, &winW, &winH);
	if (winW <= 0) winW = 1;
	if (winH <= 0) winH = 1;
	double sx = (double)self->getWidth() / (double)winW;
	double sy = (double)self->getHeight() / (double)winH;
	GfxEvent e; memset(&e, 0, sizeof(GfxEvent));
	e.type = SDL_MOUSEMOTION; e.motion.x = (int)(x * sx); e.motion.y = (int)(y * sy);
	// Coalesce high-frequency mouse motion events to avoid queue blowups.
	if (!self->glfwEventQueue.empty() && self->glfwEventQueue.back().type == SDL_MOUSEMOTION)
		self->glfwEventQueue.back() = e;
	else
		self->glfwEventQueue.push_back(e);
}

void glfw_scroll_cb(GLFWwindow* w, double xoffset, double yoffset)
{
	gfxpp* self = reinterpret_cast<gfxpp*>(glfwGetWindowUserPointer(w));
	if (!self) return;
	GfxEvent e; memset(&e, 0, sizeof(GfxEvent));
	e.type = SDL_MOUSEWHEEL;
	// Cast offsets to integer steps consistent with SDL semantics
	e.wheel.x = (int)(xoffset);
	e.wheel.y = (int)(yoffset);
	e.wheel.direction = SDL_MOUSEWHEEL_NORMAL;
	self->glfwEventQueue.push_back(e);
}

void glfw_framebuffer_size_cb(GLFWwindow* w, int ww, int hh)
{
	gfxpp* self = reinterpret_cast<gfxpp*>(glfwGetWindowUserPointer(w));
	if (!self) return;
	(void)w; (void)ww; (void)hh; // Keep logical size; viewport is adjusted via setLogicalSize
}
#endif

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

gfxpp::gfxpp()
{
	#ifdef GFX_HAVE_SDL2
	systemCursor = NULL;
	#endif
	width = 800;
	height = 600;

	#ifdef GFX_HAVE_SDL2
	context = NULL;
	renderer = NULL;
	#endif
	glfwWindow = NULL;
	glfwInitialized = false;
	renderBackend = gRenderBackend;
	this->draw = NULL;
	ttfReady = false;
	finalized = false;
#ifdef GFX_HAVE_OPENGL
	glTextCache.clear();
#endif

	errorFlag = initHelper(false, "gfxplusplus", true);
}

gfxpp::gfxpp(shmea::GString newTitle, int newRenderStatus, bool fullScreenMode, bool compatMode, int newWidth, int newHeight)
{
	#ifdef GFX_HAVE_SDL2
	systemCursor = NULL;
	#endif
	width = newWidth;
	height = newHeight;

	#ifdef GFX_HAVE_SDL2
	context = NULL;
	renderer = NULL;
	#endif
	glfwWindow = NULL;
	glfwInitialized = false;
	renderBackend = gRenderBackend;
	this->draw = NULL;
	ttfReady = false;
	finalized = false;
#ifdef GFX_HAVE_OPENGL
	glTextCache.clear();
#endif

	errorFlag = initHelper(fullScreenMode, newTitle, compatMode);
}

// Construct with external GLFW window/context
#ifdef GFX_HAVE_OPENGL
gfxpp::gfxpp(GLFWwindow* externalWindow, int newWidth, int newHeight)
{
	#ifdef GFX_HAVE_SDL2
	systemCursor = NULL;
	#endif
	width = newWidth;
	height = newHeight;

	#ifdef GFX_HAVE_SDL2
	context = NULL;
	renderer = NULL;
	#endif
	glfwWindow = externalWindow;
	glfwInitialized = false; // not owned
	renderBackend = gRenderBackend;
	this->draw = NULL;
	ttfReady = false;
	finalized = false;
	glTextCache.clear();

	// Initialize runtime state
	running = false;
	hunterZolomon = 1.0f;
	frames = 0;
	rotate = false;
	move = false;
	now = 0;
	then = 0;
	mouseX = 0;
	mouseY = 0;
	CTRLPressed = false;
	ALTPressed = false;
	spacePressed = false;
	fPressed = false;
	uPressed = false;
	qPressed = false;
	gPressed = false;
	rPressed = false;
	lPressed = false;
	upPressed = false;
	downPressed = false;
	leftPressed = false;
	rightPressed = false;
	#ifdef GFX_HAVE_SDL2
	window = NULL;
	#endif
	focusedItem = NULL;
	focusedPanel = NULL;
	fps = 0;

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
	this->draw = GfxRenderer::createOpenGL(glfwWindow);

	// Hook GLFW callbacks for translating events to our queue
	glfwSetWindowUserPointer(glfwWindow, this);
	glfwSetWindowCloseCallback(glfwWindow, glfw_window_close_cb);
	glfwSetKeyCallback(glfwWindow, glfw_key_cb);
	glfwSetMouseButtonCallback(glfwWindow, glfw_mouse_button_cb);
	glfwSetCursorPosCallback(glfwWindow, glfw_cursor_pos_cb);
	glfwSetScrollCallback(glfwWindow, glfw_scroll_cb);
	glfwSetFramebufferSizeCallback(glfwWindow, glfw_framebuffer_size_cb);
	then = (int32_t)(glfwGetTime() * 1000.0);
	errorFlag = 0;
}
#endif

// Construct with external SDL window/renderer
#ifdef GFX_HAVE_SDL2
gfxpp::gfxpp(SDL_Window* externalWindow, SDL_Renderer* externalRenderer, int newWidth, int newHeight)
{
	systemCursor = NULL;
	width = newWidth;
	height = newHeight;

	context = NULL;
	renderer = externalRenderer;
	glfwWindow = NULL;
	glfwInitialized = false;
	renderBackend = gRenderBackend;
	this->draw = NULL;
	ttfReady = false;
	finalized = false;
#ifdef GFX_HAVE_OPENGL
	glTextCache.clear();
#endif

	// Initialize runtime state
	running = false;
	hunterZolomon = 1.0f;
	frames = 0;
	rotate = false;
	move = false;
	now = 0;
	then = 0;
	mouseX = 0;
	mouseY = 0;
	CTRLPressed = false;
	ALTPressed = false;
	spacePressed = false;
	fPressed = false;
	uPressed = false;
	qPressed = false;
	gPressed = false;
	rPressed = false;
	lPressed = false;
	upPressed = false;
	downPressed = false;
	leftPressed = false;
	rightPressed = false;
	focusedItem = NULL;
	focusedPanel = NULL;
	fps = 0;

	if (!externalWindow || !externalRenderer)
	{
		errorFlag = -2;
		return;
	}
	window = externalWindow;
	renderBackend = RENDER_BACKEND_SDL2;
	int err = init2D(true);
	if (err < 0) { errorFlag = err; return; }
	this->draw = GfxRenderer::createSDL(renderer, window);
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
	running = false;
	hunterZolomon = 1.0f;

	frames = 0;
	rotate = false;
	move = false;
	now = 0;
	// initialize time base later after backend is initialized
	then = 0;

	// for mouse
	mouseX = 0;
	mouseY = 0;

	// for key presses
	CTRLPressed = false;
	ALTPressed = false;
	spacePressed = false;
	fPressed = false;
	uPressed = false;
	qPressed = false;
	gPressed = false;
	rPressed = false;
	lPressed = false;
	upPressed = false;
	downPressed = false;
	leftPressed = false;
	rightPressed = false;

	#ifdef GFX_HAVE_SDL2
	#ifdef GFX_HAVE_SDL2
	window = NULL;
	renderer = NULL;
	#endif
	#endif

	focusedItem = NULL;
	focusedPanel = NULL;

	fps = 0;

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

		// Set user pointer and callbacks to translate events
		glfwSetWindowUserPointer(glfwWindow, this);
		glfwSetWindowCloseCallback(glfwWindow, glfw_window_close_cb);
		glfwSetKeyCallback(glfwWindow, glfw_key_cb);
		glfwSetMouseButtonCallback(glfwWindow, glfw_mouse_button_cb);
		glfwSetCursorPosCallback(glfwWindow, glfw_cursor_pos_cb);
		glfwSetScrollCallback(glfwWindow, glfw_scroll_cb);
		glfwSetFramebufferSizeCallback(glfwWindow, glfw_framebuffer_size_cb);

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
			this->draw = GfxRenderer::createOpenGL(glfwWindow);
		else
		{
#ifdef GFX_HAVE_SDL2
			this->draw = GfxRenderer::createOpenGL(window, context);
#else
			this->draw = NULL;
#endif
		}
#endif
	}
	else
	{
#ifdef GFX_HAVE_SDL2
		this->draw = GfxRenderer::createSDL(renderer, window);
#else
		this->draw = NULL;
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

	cFont = new GFont(renderer);
	graphicsFonts.insert(std::pair<int, GFont*>(0, cFont));

	GFont* fontGreen = new GFont(renderer);
	fontGreen->setTextColor(RUColors::TEXT_COLOR_GREEN);
	graphicsFonts.insert(std::pair<int, GFont*>(1, fontGreen));

	GFont* fontRed = new GFont(renderer);
	fontRed->setTextColor(RUColors::TEXT_COLOR_RED);
	graphicsFonts.insert(std::pair<int, GFont*>(2, fontRed));

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
	cFont = new GFont();
	graphicsFonts.insert(std::pair<int, GFont*>(0, cFont));

	GFont* fontGreen = new GFont();
	fontGreen->setTextColor(RUColors::TEXT_COLOR_GREEN);
	graphicsFonts.insert(std::pair<int, GFont*>(1, fontGreen));

	GFont* fontRed = new GFont();
	fontRed->setTextColor(RUColors::TEXT_COLOR_RED);
	graphicsFonts.insert(std::pair<int, GFont*>(2, fontRed));

	return 0;
#else
	return -1;
#endif
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

	// for mouse
	mouseX = 0;
	mouseY = 0;

	// for key presses
	CTRLPressed = false;
	ALTPressed = false;
	spacePressed = false;
	fPressed = false;
	uPressed = false;
	qPressed = false;
	gPressed = false;
	rPressed = false;
	lPressed = false;
	upPressed = false;
	downPressed = false;
	leftPressed = false;
	rightPressed = false;

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
		if (renderBackend == RENDER_BACKEND_OPENGL && glfwWindow)
		{
			const int64_t pumpStartUs = uiProfile ? gfxpp_now_us(this) : 0;
			// Pump synthetic events immediately without requiring SDL loop
			// Avoid O(n^2) erase-at-begin by iterating and clearing once.
			for (size_t qi = 0; qi < glfwEventQueue.size(); ++qi)
			{
				GfxEvent e2 = glfwEventQueue[qi];
				// update mouse position for motion or button events
				if (e2.type == SDL_MOUSEMOTION) { mouseX = e2.motion.x; mouseY = e2.motion.y; }
				else if (e2.type == SDL_MOUSEBUTTONDOWN || e2.type == SDL_MOUSEBUTTONUP) { mouseX = e2.button.x; mouseY = e2.button.y; }
				// handle key events similar to SDL path
				if (e2.type == SDL_KEYUP || e2.type == SDL_KEYDOWN)
				{
					SDL_Keycode keyPressed = e2.key.keysym.sym;
					if (keyPressed == SDLK_LCTRL || keyPressed == SDLK_RCTRL)
					{
						if (e2.type == SDL_KEYUP) CTRLPressed = false; else CTRLPressed = true;
					}
					else if (keyPressed == SDLK_LALT || keyPressed == SDLK_RALT)
					{
						if (e2.type == SDL_KEYUP) ALTPressed = false; else ALTPressed = true;
					}
					else if (keyPressed == SDLK_SPACE)
					{
						if (e2.type == SDL_KEYUP) spacePressed = false; else spacePressed = true;
					}
					else if (keyPressed == SDLK_f)
					{
						if (e2.type == SDL_KEYUP) fPressed = false; else fPressed = true;
					}
					else if (keyPressed == SDLK_u)
					{
						if (e2.type == SDL_KEYUP) uPressed = false; else uPressed = true;
					}
					else if (keyPressed == SDLK_q)
					{
						if (e2.type == SDL_KEYUP) qPressed = false; else qPressed = true;
					}
					else if (keyPressed == SDLK_g)
					{
						if (e2.type == SDL_KEYUP) gPressed = false; else gPressed = true;
					}
					else if (keyPressed == SDLK_r)
					{
						if (e2.type == SDL_KEYUP) rPressed = false; else rPressed = true;
					}
					else if (keyPressed == SDLK_l)
					{
						if (e2.type == SDL_KEYUP) lPressed = false; else lPressed = true;
					}
					else if (keyPressed == SDLK_UP)
					{
						if (e2.type == SDL_KEYUP) upPressed = false; else upPressed = true;
					}
					else if (keyPressed == SDLK_DOWN)
					{
						if (e2.type == SDL_KEYUP) downPressed = false; else downPressed = true;
					}
					else if (keyPressed == SDLK_LEFT)
					{
						if (e2.type == SDL_KEYUP) leftPressed = false; else leftPressed = true;
					}
					else if (keyPressed == SDLK_RIGHT)
					{
						if (e2.type == SDL_KEYUP) rightPressed = false; else rightPressed = true;
					}

					if (CTRLPressed)
					{
						if (qPressed) running = false;
						if (gPressed) running = false;
						if (lPressed) system("clear");
					}

					if (keyPressed == SDLK_ESCAPE)
						running = false;
				}
				if (e2.type == SDL_QUIT) running = false;
				if (focusedPanel)
				{
					const int64_t dispatchStartUs = uiProfile ? gfxpp_now_us(this) : 0;
					focusedPanel->processSubItemEvents(this, NULL, NULL, e2, mouseX, mouseY);
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
			glfwEventQueue.clear();
			if (uiProfile) profEventPumpUs += (gfxpp_now_us(this) - pumpStartUs);
		}
		#endif

		#ifdef GFX_HAVE_SDL2
		SDL_Event event;
		#ifdef GFX_HAVE_OPENGL
		// already handled above for GLFW path
		#endif
		while (SDL_PollEvent(&event))
		{
			const int64_t pumpStartUs = uiProfile ? gfxpp_now_us(this) : 0;
			if (event.type == SDL_QUIT)
				running = false;

			SDL_Keycode keyPressed = 0x00;
			if ((event.type == SDL_KEYUP) || (event.type == SDL_KEYDOWN))
			{
				keyPressed = event.key.keysym.sym;
				if ((keyPressed == SDLK_LCTRL) || (keyPressed == SDLK_RCTRL))
				{
					if (event.type == SDL_KEYUP)
						CTRLPressed = false;
					else if (event.type == SDL_KEYDOWN)
						CTRLPressed = true;
				}
				else if ((keyPressed == SDLK_LALT) || (keyPressed == SDLK_RALT))
				{
					if (event.type == SDL_KEYUP)
						ALTPressed = false;
					else if (event.type == SDL_KEYDOWN)
						ALTPressed = true;
				}
				else if (keyPressed == SDLK_SPACE)
				{
					if (event.type == SDL_KEYUP)
						spacePressed = false;
					else if (event.type == SDL_KEYDOWN)
						spacePressed = true;
				}
				else if (keyPressed == SDLK_f)
				{
					if (event.type == SDL_KEYUP)
						fPressed = false;
					else if (event.type == SDL_KEYDOWN)
						fPressed = true;
				}
				else if (keyPressed == SDLK_u)
				{
					if (event.type == SDL_KEYUP)
						uPressed = false;
					else if (event.type == SDL_KEYDOWN)
						uPressed = true;
				}
				else if (keyPressed == SDLK_q)
				{
					if (event.type == SDL_KEYUP)
						qPressed = false;
					else if (event.type == SDL_KEYDOWN)
						qPressed = true;
				}
				else if (keyPressed == SDLK_g)
				{
					if (event.type == SDL_KEYUP)
						gPressed = false;
					else if (event.type == SDL_KEYDOWN)
						gPressed = true;
				}
				else if (keyPressed == SDLK_r)
				{
					if (event.type == SDL_KEYUP)
						rPressed = false;
					else if (event.type == SDL_KEYDOWN)
						rPressed = true;
				}
				else if (keyPressed == SDLK_l)
				{
					if (event.type == SDL_KEYUP)
						lPressed = false;
					else if (event.type == SDL_KEYDOWN)
						lPressed = true;
				}
				else if (keyPressed == SDLK_UP)
				{
					if (event.type == SDL_KEYUP)
						upPressed = false;
					else if (event.type == SDL_KEYDOWN)
						upPressed = true;
				}
				else if (keyPressed == SDLK_DOWN)
				{
					if (event.type == SDL_KEYUP)
						downPressed = false;
					else if (event.type == SDL_KEYDOWN)
						downPressed = true;
				}
				else if (keyPressed == SDLK_LEFT)
				{
					if (event.type == SDL_KEYUP)
						leftPressed = false;
					else if (event.type == SDL_KEYDOWN)
						leftPressed = true;
				}
				else if (keyPressed == SDLK_RIGHT)
				{
					if (event.type == SDL_KEYUP)
						rightPressed = false;
					else if (event.type == SDL_KEYDOWN)
						rightPressed = true;
				}

				if (CTRLPressed)
				{
					if (qPressed)
						running = false;
					if (gPressed)
						running = false;
					if (lPressed)
						system("clear");
				}

				if (keyPressed == SDLK_ESCAPE)
					running = false;
			}

			if ((event.type == SDL_MOUSEBUTTONDOWN) || (event.type == SDL_MOUSEBUTTONUP) || (event.type == SDL_MOUSEMOTION))
			{
				mouseX = event.button.x;
				mouseY = event.button.y;
			}

			if (focusedPanel)
			{
				const int64_t dispatchStartUs = uiProfile ? gfxpp_now_us(this) : 0;
				focusedPanel->processSubItemEvents(this, NULL, NULL, event, mouseX, mouseY);
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
					usleep((useconds_t)(sleepMs * 1000));

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

	if (cFont)
		delete cFont;
	cFont = NULL;

	// Iterate GFonts in graphicsFonts
	/*std::map<int, GFont*>::iterator itr = graphicsFonts.begin();
	for (; itr != graphicsFonts.end(); ++itr)
	{
		if (itr->second)
			delete itr->second;
		itr->second = NULL;
	}*/

	// Free GL text cache (OpenGL backend only)
#ifdef GFX_HAVE_OPENGL
	if (renderBackend == RENDER_BACKEND_OPENGL)
	{
		for (std::map<std::string, GLTextRenderer*>::iterator it = glTextCache.begin(); it != glTextCache.end(); ++it)
		{
			delete it->second;
		}
		//glTextCache.clear();
	}
#endif
}

void gfxpp::finish()
{
	if (finalized)
		return;
	running = false;
	// Ensure no stale UI pointers remain before cleaning up UI elements
	focusedPanel = NULL;
	clean2D();

	if (this->draw)
	{
		delete this->draw;
		this->draw = NULL;
	}

#ifdef GFX_HAVE_OPENGL
	if (renderBackend == RENDER_BACKEND_OPENGL)
	{
		// Destroy only if we own the GLFW lifecycle (created internally)
		if (glfwWindow && glfwInitialized)
		{
			// Detach callbacks/user pointer to avoid any late callback using freed gfxpp.
			glfwSetWindowUserPointer(glfwWindow, NULL);
			glfwSetWindowCloseCallback(glfwWindow, NULL);
			glfwSetKeyCallback(glfwWindow, NULL);
			glfwSetMouseButtonCallback(glfwWindow, NULL);
			glfwSetCursorPosCallback(glfwWindow, NULL);
			glfwSetScrollCallback(glfwWindow, NULL);
			glfwSetFramebufferSizeCallback(glfwWindow, NULL);

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
	if (window && (renderBackend == RENDER_BACKEND_SDL2))
	{
		SDL_DestroyWindow(window);
		window = NULL;
	}
	#endif

#ifdef GFX_HAVE_SDL2
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

#ifdef GFX_HAVE_OPENGL
GLTextRenderer* gfxpp::getGLText(const std::string& fontPath, int pixelHeight)
{
	if (renderBackend != RENDER_BACKEND_OPENGL)
		return NULL;

	char keyBuf[1024];
	keyBuf[0] = '\0';
	// Key by path + size for reuse
	snprintf(keyBuf, sizeof(keyBuf)-1, "%s|%d", fontPath.c_str(), pixelHeight);
	std::string key(keyBuf);

	std::map<std::string, GLTextRenderer*>::iterator it = glTextCache.find(key);
	if (it != glTextCache.end())
		return it->second;

	GLTextRenderer* renderer = new GLTextRenderer();
	if (!renderer->init(fontPath, pixelHeight))
	{
		delete renderer;
		return NULL;
	}
	glTextCache.insert(std::make_pair(key, renderer));
	return renderer;
}
#endif

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

void gfxpp::addGradient(int x, int y, int size)
{
#ifdef GFX_HAVE_SDL2
	// check the renderer
	if (!renderer)
	{
		printf("[GFX] Renderer error: %s\n", SDL_GetError());
		return;
	}

	/*for (int i = (-(size / 2)); i < size / 2; ++i)
	{
		for (int j = (-(size / 2)); j < size / 2; ++j)
		{
			// calculate the hue
			double hue = ((double)((i * i) + (j * j))) / ((double)(size * size));

			// get the color
			int8_t redMask = 0;
			int8_t greenMask = 0;
			int8_t blueMask = 0;
			unsigned int colorMask = RGBfromHue(hue, &redMask, &greenMask, &blueMask);

			// set the color and draw the point
			SDL_SetRenderDrawColor(renderer, redMask, greenMask, blueMask, SDL_ALPHA_OPAQUE);
			SDL_RenderDrawPoint(renderer, x + i, y + j);
		}
	}*/
#else
	(void)x; (void)y; (void)size;
#endif
}

void gfxpp::addItem(GItem* newItem)
{
	if (!newItem)
		return;

	const shmea::GString options = "0123456789";
	const int keyLength = 6;
	shmea::GString newItemID = "";
	int itemID = 0;

	do
	{
		newItemID = "";
		for (int i = 0; i < keyLength; ++i)
		{
			int newIndex = rand() % options.length();
			char newChar = options[newIndex];
			newItemID += newChar;
		}

		itemID = atoi(newItemID.c_str());

	} while (getItemByID(itemID)); // Generate new item id if already used

	// Assign same id to object of GItem and add it
	newItem->setID(itemID);
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
			guiElements.erase(guiElements.begin() + i);
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

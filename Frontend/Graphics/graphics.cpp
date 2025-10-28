// Copyright 2020 Robert Carneiro, Derek Meer, Matthew Tabak, Eric Lujan
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

// GLFW -> SDL event translation helpers (C++98: use free functions)
#ifdef GFX_HAVE_OPENGL
void glfw_window_close_cb(GLFWwindow* w)
{
	gfxpp* self = reinterpret_cast<gfxpp*>(glfwGetWindowUserPointer(w));
	if (!self) return;
	#ifdef GFX_HAVE_SDL2
	SDL_Event e; memset(&e, 0, sizeof(SDL_Event)); e.type = SDL_QUIT; self->glfwEventQueue.push_back(e);
	#else
	self->running = false;
	#endif
}

void glfw_key_cb(GLFWwindow* w, int key, int scancode, int action, int mods)
{
	gfxpp* self = reinterpret_cast<gfxpp*>(glfwGetWindowUserPointer(w));
	if (!self) return;
	SDL_Event e; memset(&e, 0, sizeof(SDL_Event));
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
	if (winW <= 0) winW = 1; if (winH <= 0) winH = 1;
	double sx = (double)self->getWidth() / (double)winW;
	double sy = (double)self->getHeight() / (double)winH;
	SDL_Event e; memset(&e, 0, sizeof(SDL_Event));
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
	if (winW <= 0) winW = 1; if (winH <= 0) winH = 1;
	double sx = (double)self->getWidth() / (double)winW;
	double sy = (double)self->getHeight() / (double)winH;
	SDL_Event e; memset(&e, 0, sizeof(SDL_Event));
	e.type = SDL_MOUSEMOTION; e.motion.x = (int)(x * sx); e.motion.y = (int)(y * sy);
	self->glfwEventQueue.push_back(e);
}

void glfw_scroll_cb(GLFWwindow* w, double xoffset, double yoffset)
{
	gfxpp* self = reinterpret_cast<gfxpp*>(glfwGetWindowUserPointer(w));
	if (!self) return;
	SDL_Event e; memset(&e, 0, sizeof(SDL_Event));
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

// Define non-integral static class member for broad standards compatibility
const float gfxpp::MAX_FRAMES_PER_SECOND = 30.0f;

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
			if (!glfwInit())
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
	while (running)
	{
		++frames;

		//=================EVENTS=================
		#ifdef GFX_HAVE_OPENGL
		if (renderBackend == RENDER_BACKEND_OPENGL && glfwWindow)
			glfwPollEvents();
		if (renderBackend == RENDER_BACKEND_OPENGL && glfwWindow)
		{
			// Pump synthetic events immediately without requiring SDL loop
			while (!glfwEventQueue.empty())
			{
				SDL_Event e2 = glfwEventQueue.front();
				glfwEventQueue.erase(glfwEventQueue.begin());
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
					focusedPanel->processSubItemEvents(this, NULL, NULL, e2, mouseX, mouseY);
				for (size_t li = 0; li < listeners.size(); ++li)
					listeners[li].fn(e2, listeners[li].userData);
			}
		}
		#endif

		#ifdef GFX_HAVE_SDL2
		SDL_Event event;
		#ifdef GFX_HAVE_OPENGL
		// already handled above for GLFW path
		#endif
		while (SDL_PollEvent(&event))
		{
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
				focusedPanel->processSubItemEvents(this, NULL, NULL, event, mouseX, mouseY);

			for (size_t li = 0; li < listeners.size(); ++li)
				listeners[li].fn(event, listeners[li].userData);
		}
		#endif

		//=================Render=================
		if (this->draw)
		{
			// Ensure viewport/projection up-to-date before drawing; reset modelview
			this->draw->setLogicalSize(getWidth(), getHeight());
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

		// fps
		#ifdef GFX_HAVE_OPENGL
		if (renderBackend == RENDER_BACKEND_OPENGL)
			now = (int32_t)(glfwGetTime() * 1000.0);
		else
		#endif
		{
		#ifdef GFX_HAVE_SDL2
			now = SDL_GetTicks();
		#else
			#ifdef GFX_HAVE_OPENGL
			now = (int32_t)(glfwGetTime() * 1000.0);
			#else
			now = 0;
			#endif
		#endif
		}
		float cFrames = ((float)frames * 1000.0f) / ((float)(now - then));
		/*if (running)
		{
			char fpsBuffer[26];
			bzero(&fpsBuffer, 26);
			sprintf(fpsBuffer, "%2.1f fps", cFrames);
		}*/

		// Cap the frame rate
		fps = cFrames;
		if ((cFrames - MAX_FRAMES_PER_SECOND > 0) && (cFrames > MAX_FRAMES_PER_SECOND))
		{
		#ifdef GFX_HAVE_OPENGL
			if (renderBackend == RENDER_BACKEND_OPENGL)
			{
				int ms = (int)((cFrames - MAX_FRAMES_PER_SECOND) * 10.0f);
				if (ms > 0) usleep(ms * 1000);
			}
			else
		#endif
			{
			#ifdef GFX_HAVE_SDL2
				SDL_Delay((Uint32)((cFrames - MAX_FRAMES_PER_SECOND) * 10.0f));
			#else
				int ms = (int)((cFrames - MAX_FRAMES_PER_SECOND) * 10.0f);
				if (ms > 0) usleep(ms * 1000);
			#endif
			}
		}

		// Present every frame so interactive elements (caret) update reliably
		if (this->draw)
		{
			this->draw->setLogicalSize(getWidth(), getHeight());
			this->draw->present();
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
			glfwDestroyWindow(glfwWindow);
			glfwWindow = NULL;
		}
		if (glfwInitialized)
		{
			glfwTerminate();
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

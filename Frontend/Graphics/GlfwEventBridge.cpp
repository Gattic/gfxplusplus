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
#ifdef GFX_HAVE_OPENGL

#include "GlfwEventBridge.h"
#include <GLFW/glfw3.h>
#include <string.h>

GlfwEventBridge::GlfwEventBridge()
{
	logicalW = 800;
	logicalH = 600;
	closeRequested = false;
}

void GlfwEventBridge::attach(GLFWwindow* window, int logicalWidth, int logicalHeight)
{
	logicalW = logicalWidth;
	logicalH = logicalHeight;
	closeRequested = false;
	glfwSetWindowUserPointer(window, this);
	glfwSetWindowCloseCallback(window, windowCloseCb);
	glfwSetKeyCallback(window, keyCb);
	glfwSetMouseButtonCallback(window, mouseButtonCb);
	glfwSetCursorPosCallback(window, cursorPosCb);
	glfwSetScrollCallback(window, scrollCb);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCb);
}

void GlfwEventBridge::detach(GLFWwindow* window)
{
	if (!window)
		return;
	glfwSetWindowUserPointer(window, NULL);
	glfwSetWindowCloseCallback(window, NULL);
	glfwSetKeyCallback(window, NULL);
	glfwSetMouseButtonCallback(window, NULL);
	glfwSetCursorPosCallback(window, NULL);
	glfwSetScrollCallback(window, NULL);
	glfwSetFramebufferSizeCallback(window, NULL);
}

void GlfwEventBridge::drainEvents(std::vector<GfxEvent>& out)
{
	out.swap(eventQueue);
	eventQueue.clear();
}

bool GlfwEventBridge::isCloseRequested() const
{
	return closeRequested;
}

void GlfwEventBridge::setLogicalSize(int w, int h)
{
	logicalW = w;
	logicalH = h;
}

// --- Static GLFW callbacks ---

void GlfwEventBridge::windowCloseCb(GLFWwindow* w)
{
	GlfwEventBridge* self = reinterpret_cast<GlfwEventBridge*>(glfwGetWindowUserPointer(w));
	if (!self) return;
	self->closeRequested = true;
#ifdef GFX_HAVE_SDL2
	GfxEvent e;
	memset(&e, 0, sizeof(GfxEvent));
	e.type = SDL_QUIT;
	self->eventQueue.push_back(e);
#endif
}

void GlfwEventBridge::keyCb(GLFWwindow* w, int key, int scancode, int action, int mods)
{
	GlfwEventBridge* self = reinterpret_cast<GlfwEventBridge*>(glfwGetWindowUserPointer(w));
	if (!self) return;
	(void)scancode;
	GfxEvent e; memset(&e, 0, sizeof(GfxEvent));
	if (action == GLFW_PRESS || action == GLFW_REPEAT) e.type = SDL_KEYDOWN; else if (action == GLFW_RELEASE) e.type = SDL_KEYUP; else return;
	int sdlk = 0;
	switch (key)
	{
		case GLFW_KEY_LEFT_CONTROL: sdlk = SDLK_LCTRL; break;
		case GLFW_KEY_RIGHT_CONTROL: sdlk = SDLK_RCTRL; break;
		case GLFW_KEY_LEFT_ALT: sdlk = SDLK_LALT; break;
		case GLFW_KEY_RIGHT_ALT: sdlk = SDLK_RALT; break;
		case GLFW_KEY_SPACE: sdlk = SDLK_SPACE; break;
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
		case GLFW_KEY_UP: sdlk = SDLK_UP; break;
		case GLFW_KEY_DOWN: sdlk = SDLK_DOWN; break;
		case GLFW_KEY_LEFT: sdlk = SDLK_LEFT; break;
		case GLFW_KEY_RIGHT: sdlk = SDLK_RIGHT; break;
		case GLFW_KEY_HOME: sdlk = SDLK_HOME; break;
		case GLFW_KEY_END: sdlk = SDLK_END; break;
		case GLFW_KEY_BACKSPACE: sdlk = SDLK_BACKSPACE; break;
		case GLFW_KEY_DELETE: sdlk = SDLK_DELETE; break;
		case GLFW_KEY_ESCAPE: sdlk = SDLK_ESCAPE; break;
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
	Uint16 keymods = 0;
	if (mods & GLFW_MOD_SHIFT) keymods |= KMOD_SHIFT;
	if (mods & GLFW_MOD_CONTROL) keymods |= KMOD_CTRL;
	if (mods & GLFW_MOD_ALT) keymods |= KMOD_ALT;
#ifdef GLFW_MOD_CAPS_LOCK
	if (mods & GLFW_MOD_CAPS_LOCK) keymods |= KMOD_CAPS;
#endif
	e.key.keysym.mod = keymods;
	self->eventQueue.push_back(e);
}

void GlfwEventBridge::mouseButtonCb(GLFWwindow* w, int button, int action, int mods)
{
	GlfwEventBridge* self = reinterpret_cast<GlfwEventBridge*>(glfwGetWindowUserPointer(w));
	if (!self) return;
	(void)mods;
	double cx=0, cy=0; glfwGetCursorPos(w, &cx, &cy);
	int winW=0, winH=0; glfwGetWindowSize(w, &winW, &winH);
	if (winW <= 0) winW = 1;
	if (winH <= 0) winH = 1;
	double sx = (double)self->logicalW / (double)winW;
	double sy = (double)self->logicalH / (double)winH;
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
	self->eventQueue.push_back(e);
}

void GlfwEventBridge::cursorPosCb(GLFWwindow* w, double x, double y)
{
	GlfwEventBridge* self = reinterpret_cast<GlfwEventBridge*>(glfwGetWindowUserPointer(w));
	if (!self) return;
	int winW=0, winH=0; glfwGetWindowSize(w, &winW, &winH);
	if (winW <= 0) winW = 1;
	if (winH <= 0) winH = 1;
	double sx = (double)self->logicalW / (double)winW;
	double sy = (double)self->logicalH / (double)winH;
	GfxEvent e; memset(&e, 0, sizeof(GfxEvent));
	e.type = SDL_MOUSEMOTION; e.motion.x = (int)(x * sx); e.motion.y = (int)(y * sy);
	if (!self->eventQueue.empty() && self->eventQueue.back().type == SDL_MOUSEMOTION)
		self->eventQueue.back() = e;
	else
		self->eventQueue.push_back(e);
}

void GlfwEventBridge::scrollCb(GLFWwindow* w, double xoffset, double yoffset)
{
	GlfwEventBridge* self = reinterpret_cast<GlfwEventBridge*>(glfwGetWindowUserPointer(w));
	if (!self) return;
	GfxEvent e; memset(&e, 0, sizeof(GfxEvent));
	e.type = SDL_MOUSEWHEEL;
	e.wheel.x = (int)(xoffset);
	e.wheel.y = (int)(yoffset);
	e.wheel.direction = SDL_MOUSEWHEEL_NORMAL;
	self->eventQueue.push_back(e);
}

void GlfwEventBridge::framebufferSizeCb(GLFWwindow* w, int ww, int hh)
{
	(void)w; (void)ww; (void)hh;
}

#endif // GFX_HAVE_OPENGL

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
#include "InputState.h"

InputState::InputState()
{
	reset();
}

void InputState::reset()
{
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
}

bool InputState::handleKeyEvent(int eventType, GfxKeycode key)
{
	bool pressed = (eventType == SDL_KEYDOWN);

	if (key == SDLK_LCTRL || key == SDLK_RCTRL)
		CTRLPressed = pressed;
	else if (key == SDLK_LALT || key == SDLK_RALT)
		ALTPressed = pressed;
	else if (key == SDLK_SPACE)
		spacePressed = pressed;
	else if (key == SDLK_f)
		fPressed = pressed;
	else if (key == SDLK_u)
		uPressed = pressed;
	else if (key == SDLK_q)
		qPressed = pressed;
	else if (key == SDLK_g)
		gPressed = pressed;
	else if (key == SDLK_r)
		rPressed = pressed;
	else if (key == SDLK_l)
		lPressed = pressed;
	else if (key == SDLK_UP)
		upPressed = pressed;
	else if (key == SDLK_DOWN)
		downPressed = pressed;
	else if (key == SDLK_LEFT)
		leftPressed = pressed;
	else if (key == SDLK_RIGHT)
		rightPressed = pressed;

	// Check for quit keys
	if (CTRLPressed && (qPressed || gPressed))
		return true;
	if (key == SDLK_ESCAPE)
		return true;

	return false;
}

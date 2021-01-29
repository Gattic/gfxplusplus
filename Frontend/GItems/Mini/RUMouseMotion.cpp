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
#include "RUMouseMotion.h"
#include "../../GFXUtilities/EventTracker.h"
#include "../../Graphics/graphics.h"
#include "../GItem.h"

RUMouseMotion::RUMouseMotion()
{
	// event listeners
	MouseMotionListener = GeneralListener();
	unhovered = false;
	cursor = SDL_SYSTEM_CURSOR_ARROW;
	customCursor = false;
}

RUMouseMotion::~RUMouseMotion()
{
	// event listeners
	MouseMotionListener = GeneralListener();
	unhovered = false;
	cursor = SDL_SYSTEM_CURSOR_ARROW;
	customCursor = false;
}

SDL_SystemCursor RUMouseMotion::getCursor() const
{
	return cursor;
}

void RUMouseMotion::setCursor(SDL_SystemCursor newCursor)
{
	cursor = newCursor;
	customCursor = true;
}

void RUMouseMotion::setMouseMotionListener(GeneralListener f)
{
	MouseMotionListener = f;
}

void RUMouseMotion::onMouseMotionHelper(gfxpp* cGfx, EventTracker* eventsStatus, GPanel* cPanel,
										int eventX, int eventY, bool overrideRange)
{
	if (!cGfx)
		return;

	if (!eventsStatus)
		return;

	if (!cPanel)
		return;

	if (!visible)
		return;

	// To close the dropdown
	if ((!overrideRange) && (!inRange(eventX, eventY)))
		return;

	// Dont want to override subcomps cursor
	if ((customCursor) && (!eventsStatus->hovered))
	{
		// Set the cursor
		SDL_Cursor* renderCursor = SDL_CreateSystemCursor(cursor);
		SDL_SetCursor(renderCursor);
	}

	// pass on the event
	unhovered = false;
	onMouseMotion(cGfx, cPanel, eventX - getX(), eventY - getY());

	// Call the callback we saved
	MouseMotionListener.call(eventX - getX(), eventY - getY());

	eventsStatus->hovered = true;
}

void RUMouseMotion::onMouseMotion(gfxpp* cGfx, GPanel* cPanel, int eventX, int eventY)
{
	// printf("RUMouseMotion: onMouseMotion(%d, %d);\n", eventX, eventY);
}

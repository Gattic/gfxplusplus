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
#include "RUMouseUp.h"
#include "../../GFXUtilities/EventTracker.h"
#include "../../Graphics/graphics.h"
#include "../GItem.h"

RUMouseUp::RUMouseUp()
{
	// event listeners
	MouseUpListener = GeneralListener();
}

RUMouseUp::~RUMouseUp()
{
	// event listeners
	MouseUpListener = GeneralListener();
}

void RUMouseUp::setMouseUpListener(GeneralListener f)
{
	MouseUpListener = f;
}

void RUMouseUp::onMouseUpHelper(gfxpp* cGfx, EventTracker* eventsStatus, GPanel* cPanel, int eventX,
								int eventY, bool overrideRange)
{
	if (!cGfx)
		return;

	if (!eventsStatus)
		return;

	if (!cPanel)
		return;

	if (!visible)
		return;

	if ((!overrideRange) && (!inRange(eventX, eventY)))
		return;

	// Render the GUI object
	drawUpdate = true;

	// pass on the event
	onMouseUp(cGfx, cPanel, eventX - getX(), eventY - getY());

	// Call the callback we saved
	MouseUpListener.call(eventX - getX(), eventY - getY());

	eventsStatus->upClicked = true;
}

void RUMouseUp::onMouseUp(gfxpp* cGfx, GPanel* cPanel, int eventX, int eventY)
{
	// printf("RUMouseUp: onMouseUp(%d, %d);\n", eventX, eventY);
}

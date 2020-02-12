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
#include "RUKeyUp.h"
#include "../../GFXUtilities/EventTracker.h"
#include "../../GUI/Text/RUTextComponent.h"
#include "../../Graphics/graphics.h"
#include "../GItem.h"

RUKeyUp::RUKeyUp()
{
	// event listeners
	KeyUpListener = 0;
}

RUKeyUp::~RUKeyUp()
{
	// event listeners
	KeyUpListener = 0;
}

void RUKeyUp::setKeyUpListener(void (GPanel::*f)(const std::string&))
{
	KeyUpListener = f;
}

void RUKeyUp::onKeyUpHelper(EventTracker* eventsStatus, GPanel* cPanel, SDL_Keycode keyPressed,
							Uint16 keyModPressed)
{
	if (!eventsStatus)
		return;

	if (!cPanel)
		return;

	if (!visible)
		return;

	GItem* cItem = dynamic_cast<GItem*>(this);
	if (!(Graphics::focusedItem == cItem))
		return;

	// Render the GUI object
	drawUpdate = true;

	// pass on the event
	onKeyUp(cPanel, keyPressed, keyModPressed);
}

void RUKeyUp::onKeyUp(GPanel* cPanel, SDL_Keycode keyPressed, Uint16 keyModPressed)
{
	// printf("RUKeyUp: ");
}

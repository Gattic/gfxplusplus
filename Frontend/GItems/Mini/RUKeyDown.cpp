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
#include "RUKeyDown.h"
#include "../../GFXUtilities/EventTracker.h"
#include "../../GUI/Text/RUTextComponent.h"
#include "../../Graphics/graphics.h"
#include "../GItem.h"

RUKeyDown::RUKeyDown()
{
	// event listeners
	KeyDownListener = 0;
}

RUKeyDown::~RUKeyDown()
{
	// event listeners
	KeyDownListener = 0;
}

void RUKeyDown::setKeyDownListener(void (GPanel::*f)(const std::string&))
{
	KeyDownListener = f;
}

void RUKeyDown::onKeyDownHelper(EventTracker* eventsStatus, GPanel* cPanel, SDL_Keycode keyPressed,
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

	if (!((cItem->getType() == "RUTextbox") && (!Graphics::CTRLPressed) && (!Graphics::ALTPressed)))
		return;

	// Render the GUI object
	drawUpdate = true;

	// pass on the event
	onKeyDown(cPanel, keyPressed, keyModPressed);

	if (KeyDownListener != 0)
		(cPanel->*KeyDownListener)(cItem->getName());

	if ((Graphics::focusedItem->getType() == "RUTextbox") && (!Graphics::CTRLPressed) &&
		(!Graphics::ALTPressed))
	{
		((RUTextComponent*)(Graphics::focusedItem))
			->onKeyHelper(Graphics::focusedPanel, keyPressed, keyModPressed);
		eventsStatus->keyReleased = true;
	}

	eventsStatus->keyPressed = true;
}

void RUKeyDown::onKeyDown(GPanel* cPanel, SDL_Keycode keyPressed, Uint16 keyModPressed)
{
	// printf("RUKeyDown: ");
}

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
#include "RUButton.h"
#include "../../GItems/RUColors.h"
#include "../../Graphics/graphics.h"

RUButton::RUButton()
{
	buttonColor = "blue";
	setBGColor(RUColors::DEFAULT_BUTTON_BLUE);
	//setBorderColor(RUColors::DEFAULT_BUTTON_BORDER_BLUE);
	//toggleBorder(true);
	setCursor(SDL_SYSTEM_CURSOR_HAND);
}

RUButton::RUButton(shmea::GString buttonColor_)
{
	buttonColor = buttonColor_;

	if (buttonColor == "red")
	{
		setBGColor(RUColors::DEFAULT_BUTTON_RED);
		//setBorderColor(RUColors::DEFAULT_BUTTON_BORDER_RED);
	}
	else if (buttonColor == "green")
	{
		setBGColor(RUColors::DEFAULT_BUTTON_GREEN);
		//setBorderColor(RUColors::DEFAULT_BUTTON_BORDER_GREEN);
	}
	else if (buttonColor == "blue")
	{
		setBGColor(RUColors::DEFAULT_BUTTON_BLUE);
		//setBorderColor(RUColors::DEFAULT_BUTTON_BORDER_BLUE);
	}
	else
	{
		// Default to blue
		setBGColor(RUColors::DEFAULT_BUTTON_BLUE);
		//setBorderColor(RUColors::DEFAULT_BUTTON_BORDER_BLUE);
		printf("Button color not recognized. Defaulting to blue.");
	}

	//toggleBorder(true);
	setCursor(SDL_SYSTEM_CURSOR_HAND);
}

RUButton::~RUButton()
{
	//
}

void RUButton::updateBackground(gfxpp* cGfx)
{
	drawText(cGfx);
}

void RUButton::hover(gfxpp* cGfx)
{
	if (buttonColor == "red")
	{
		setBGColor(RUColors::DEFAULT_BUTTON_HOVER_RED);
		// setTextColor(RUColors::DEFAULT_BUTTON_COLOR_TEXT_HIGHLIGHT_RED);
	}
	else if (buttonColor == "green")
	{
		setBGColor(RUColors::DEFAULT_BUTTON_HOVER_GREEN);
		// setTextColor(RUColors::DEFAULT_BUTTON_COLOR_TEXT_HIGHLIGHT_GREEN);
	}
	else if (buttonColor == "blue")
	{
		setBGColor(RUColors::DEFAULT_BUTTON_HOVER_BLUE);
		// setTextColor(RUColors::DEFAULT_BUTTON_COLOR_TEXT_HIGHLIGHT_BLUE);
	}
	else
	{
		// Default to blue
		setBGColor(RUColors::DEFAULT_BUTTON_HOVER_BLUE);
		// setTextColor(RUColors::DEFAULT_BUTTON_COLOR_TEXT_HIGHLIGHT_BLUE);
	}
	drawUpdate = true;
}

void RUButton::unhover(gfxpp* cGfx)
{
	if (buttonColor == "red")
	{
		setBGColor(RUColors::DEFAULT_BUTTON_RED);
		// setTextColor(RUColors::DEFAULT_BUTTON_COLOR_TEXT_RED);
	}
	else if (buttonColor == "green")
	{
		setBGColor(RUColors::DEFAULT_BUTTON_GREEN);
		// setTextColor(RUColors::DEFAULT_BUTTON_COLOR_TEXT_GREEN);
	}
	else if (buttonColor == "blue")
	{
		setBGColor(RUColors::DEFAULT_BUTTON_BLUE);
		// setTextColor(RUColors::DEFAULT_BUTTON_COLOR_TEXT_BLUE);
	}
	else
	{
		// Default to blue
		setBGColor(RUColors::DEFAULT_BUTTON_BLUE);
		// setTextColor(RUColors::DEFAULT_BUTTON_COLOR_TEXT_BLUE);
	}
	drawUpdate = true;
}

shmea::GString RUButton::getType() const
{
	return "RUButton";
}

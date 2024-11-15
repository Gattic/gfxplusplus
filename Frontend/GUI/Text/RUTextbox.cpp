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
#include "RUTextbox.h"
#include "../../GItems/RUColors.h"
#include "../../Graphics/graphics.h"

RUTextbox::RUTextbox()
{
	readOnly = false;
	staticBorder = false;
	setBGColor(RUColors::DEFAULT_COLOR_BACKGROUND);
	// Draw a bg image instead of color?
	setCursor(SDL_SYSTEM_CURSOR_IBEAM);
}

RUTextbox::~RUTextbox()
{
	//
}

void RUTextbox::updateBackground(gfxpp* cGfx)
{
	drawText(cGfx);
}

shmea::GString RUTextbox::getType() const
{
	return "RUTextbox";
}

void RUTextbox::hover(gfxpp* cGfx)
{
	// setBorderColor(RUColors::DEFAULT_COMPONENT_HIGHLIGHT);
	toggleBorder(true);
}

void RUTextbox::unhover(gfxpp* cGfx)
{
	if (!isFocused() && !staticBorder)
	{
		toggleBorder(false);
	}
}

void RUTextbox::setStaticBorder(bool staticBorder_)
{
	staticBorder = staticBorder_;
}

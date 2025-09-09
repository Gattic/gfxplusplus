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
#include "RUBorderComponent.h"
#include "../../Graphics/graphics.h"
#include "../../Graphics/GfxRenderer.h"
#include "../GItem.h"
#include "../RUColors.h"

RUBorderComponent::RUBorderComponent()
{
	borderWidth = DEFAULT_BORDER_WIDTH;
	borderEnabled = false;
	setBorderColor(RUColors::DEFAULT_COLOR_BORDER);
}

RUBorderComponent::RUBorderComponent(GfxColor newBorderColor)
{
	borderWidth = DEFAULT_BORDER_WIDTH;
	borderEnabled = false;
	setBorderColor(newBorderColor);
}

RUBorderComponent::RUBorderComponent(int newWidth, GfxColor newBorderColor)
{
	borderWidth = newWidth;
	borderEnabled = false;
	setBorderColor(newBorderColor);
}

RUBorderComponent::~RUBorderComponent()
{
	borderWidth = 0;
	borderEnabled = false;
	setBorderColor(RUColors::DEFAULT_COLOR_BORDER);
}

bool RUBorderComponent::getBorderEnabled() const
{
	return borderEnabled;
}

GfxColor RUBorderComponent::getBorderColor() const
{
	return borderColor;
}

int RUBorderComponent::getBorderWidth() const
{
	return borderWidth;
}

void RUBorderComponent::toggleBorder(bool newEnabled)
{
	borderEnabled = newEnabled;
}

void RUBorderComponent::setBorderColor(GfxColor newBorderColor)
{
	borderColor = newBorderColor;
}

void RUBorderComponent::setBorderWidth(int newWidth)
{
	borderWidth = newWidth;
}

void RUBorderComponent::updateBorderBackground(gfxpp* cGfx)
{
	if (!borderEnabled)
		return;

	if (borderWidth <= 0)
		return;

	if (!((getWidth() > 0) && (getHeight() > 0)))
		return;

	GfxRect borderRect;
	borderRect.x = 0;
	borderRect.y = 0;
	borderRect.w = getWidth();
	borderRect.h = getHeight();

	if (cGfx->getDraw())
	{
		cGfx->getDraw()->setDrawColor(borderColor.r, borderColor.g, borderColor.b,
					   borderColor.a);
		cGfx->getDraw()->drawRect(&borderRect);
	}
}

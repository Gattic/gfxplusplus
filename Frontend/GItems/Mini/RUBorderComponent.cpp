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
#include "../GItem.h"
#include "../RUColors.h"

RUBorderComponent::RUBorderComponent()
{
	borderEnabled = false;
	borderWidth = 1;
	setBorderColor(RUColors::DEFAULT_COLOR_BORDER);
}

RUBorderComponent::RUBorderComponent(int newWidth)
{
	borderEnabled = true;
	borderWidth = newWidth;
	setBorderColor(RUColors::DEFAULT_COLOR_BORDER);
}

RUBorderComponent::RUBorderComponent(SDL_Color newBorderColor)
{
	borderEnabled = true;
	borderWidth = 1;
	setBorderColor(newBorderColor);
}

RUBorderComponent::RUBorderComponent(int newWidth, SDL_Color newBorderColor)
{
	borderEnabled = true;
	borderWidth = newWidth;
	setBorderColor(newBorderColor);
}

RUBorderComponent::~RUBorderComponent()
{
	borderEnabled = false;
}

bool RUBorderComponent::getBorderEnabled() const
{
	return borderEnabled;
}

SDL_Color RUBorderComponent::getBorderColor() const
{
	return borderColor;
}

int RUBorderComponent::getBorderWidth() const
{
	return borderWidth;
}

void RUBorderComponent::toggleBorder(bool newBorder)
{
	borderEnabled = newBorder;
	drawUpdate = true;
}

void RUBorderComponent::setBorderColor(SDL_Color newBorderColor)
{
	borderColor = newBorderColor;
}

void RUBorderComponent::setBorderWidth(int newBorderWidth)
{
	borderWidth = newBorderWidth;
}

void RUBorderComponent::updateBorderBackground(gfxpp* cGfx)
{
	if (!borderEnabled)
		return;

	if (!((getWidth() > 0) && (getHeight() > 0)))
		return;

	for (int i = 0; i < borderWidth; ++i)
	{
		// draw the border
		SDL_Rect borderRect;
		borderRect.x = i;
		borderRect.y = i;
		borderRect.w = getWidth() - (borderWidth - 1);
		borderRect.h = getHeight() - (borderWidth - 1);

		SDL_SetRenderDrawColor(cGfx->getRenderer(), borderColor.r, borderColor.g, borderColor.b,
							   borderColor.a);
		SDL_RenderDrawRect(cGfx->getRenderer(), &borderRect);
	}
}

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
#include "RUProgressBar.h"
#include "../GItems/RUColors.h"
#include "../Graphics/graphics.h"
#include "../Graphics/GfxRenderer.h"

RUProgressBar::RUProgressBar()
{
	barColor = RUColors::DEFAULT_COLOR_BAR;
	value = 0;
	maxValue = 100;
}

RUProgressBar::~RUProgressBar()
{
	value = 0;
}

int RUProgressBar::getValue() const
{
	return value;
}

void RUProgressBar::setValue(int newValue)
{
	value = newValue;
	if (value < 0)
		value = 0;
	if (value > maxValue)
		value = maxValue;
}

void RUProgressBar::updateBackground(gfxpp* cGfx)
{
	int barWidth = (maxValue == 0) ? 0 : (width * value) / maxValue;
	GfxRect barRect;
	barRect.x = 0;
	barRect.y = 0;
	barRect.w = barWidth;
	barRect.h = height;

	if (cGfx->getDraw())
	{
		cGfx->getDraw()->setDrawColor(barColor.r, barColor.g, barColor.b,
					 GFX_ALPHA_OPAQUE);
		cGfx->getDraw()->fillRect(&barRect);
	}
}

shmea::GString RUProgressBar::getType() const
{
	return "RUProgressBar";
}

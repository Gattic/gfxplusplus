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
#include "Graphable.h"
#include "RUGraph.h"
#include "../GFXUtilities/ActionBubble.h"
#include "../Graphics/GfxRenderer.h"

template <>
void Graphable<ActionBubble>::computeAxisRanges(bool additionOptimization)
{
	// We use the candle calculations
	parent->requireDrawUpdate();
}

template <>
void Graphable<ActionBubble>::draw(gfxpp* cGfx)
{
	if (!parent)
		return;

	for (unsigned int i = 0; i < points.size(); ++i)
	{
		ActionBubble* cBubble = points[i];
		if (!cBubble)
			continue;

		if(cBubble->getActionType() == ActionBubble::ACTION_BUY)
		{
			if (cGfx->getDraw())
				cGfx->getDraw()->setDrawColor(RUColors::COLOR_WHITE.r, RUColors::COLOR_WHITE.g, RUColors::COLOR_WHITE.b, RUColors::COLOR_WHITE.a);
		}
		else if(cBubble->getActionType() == ActionBubble::ACTION_SELL)
		{
			if (cGfx->getDraw())
				cGfx->getDraw()->setDrawColor(RUColors::COLOR_BLACK.r, RUColors::COLOR_BLACK.g, RUColors::COLOR_BLACK.b, RUColors::COLOR_BLACK.a);
		}
		else
		{
			if (cGfx->getDraw())
				cGfx->getDraw()->setDrawColor(getColor().r, getColor().g, getColor().b, getColor().a);
			continue;
		}

		const Point2* f = cBubble->getFocalPoint();
		if (!f) continue;
		int xVal = static_cast<int>(f->getX());
		int yVal = static_cast<int>(f->getY());
		if (cGfx->getDraw())
			cGfx->getDraw()->drawPoint(parent->getX() + xVal, parent->getY() + yVal);
	}
}

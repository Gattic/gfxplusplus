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

	if (!parent->getGraphables()["candles"])
		return;

	Graphable<Candle>* candlePlotter = parent->getGraphables()["candles"]->get(Candle());
	if(!candlePlotter)
		return;

	if((candlePlotter->size() == 0) || (candlePlotter->normalizedPoints.size() == 0))
		return;

	float candleWidth = ((float)parent->getWidth()) / candlePlotter->size();
	float yRange = getYMax() - getYMin();
	float pointYGap = ((float)parent->getHeight()) / yRange;

	for (unsigned int i = 0; i < points.size(); ++i)
	{
		const ActionBubble* cBubble = points[i];
		if (!cBubble)
			continue;

		if(cBubble->getActionType() == ActionBubble::ACTION_BUY)
			SDL_SetRenderDrawColor(cGfx->getRenderer(), RUColors::COLOR_WHITE.r, RUColors::COLOR_WHITE.g, RUColors::COLOR_WHITE.b, RUColors::COLOR_WHITE.a);
		else if(cBubble->getActionType() == ActionBubble::ACTION_SELL)
			SDL_SetRenderDrawColor(cGfx->getRenderer(), RUColors::COLOR_BLACK.r, RUColors::COLOR_BLACK.g, RUColors::COLOR_BLACK.b, RUColors::COLOR_BLACK.a);
		else
		{
			SDL_SetRenderDrawColor(cGfx->getRenderer(), getColor().r, getColor().g, getColor().b, getColor().a);
			continue;
		}

		const Point2* cFocalPoint = cBubble->getFocalPoint();
		if (!cFocalPoint)
			continue;

		double radius = candleWidth / 2.0f;
		if (radius <= 0)
			continue;
	
		for (int drawX = -radius; drawX < radius; ++drawX)
		{
			unsigned int xIndex = cFocalPoint->getX();
			if(xIndex >= candlePlotter->normalizedPoints.size())
				continue;

			int xVal = candlePlotter->normalizedPoints[xIndex]->getX() + drawX - radius; // from candle X

			std::map<int, int> newMap;
			for (int drawY = -radius; drawY < radius; ++drawY)
			{
				double distance = sqrt(pow(((double)drawX), 2.0f) + pow(((double)drawY), 2.0f));
				if (distance > radius)
					continue;

				float yVal = (cFocalPoint->getY() - getYMin()) * pointYGap;
				if(cBubble->getActionType() == ActionBubble::ACTION_BUY)
					yVal = parent->getHeight() - yVal - (candleWidth*3.0f);
				else if(cBubble->getActionType() == ActionBubble::ACTION_SELL)
					yVal = parent->getHeight() - yVal + (candleWidth*3.0f);
				yVal += drawY;
	
				// set the color and draw the point
				SDL_RenderDrawPoint(cGfx->getRenderer(), parent->getX() + xVal,
									parent->getY() + yVal);
			}
		}
	}
}

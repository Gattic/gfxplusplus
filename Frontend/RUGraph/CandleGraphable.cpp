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

template <>
void Graphable<Candle>::computeAxisRanges(gfxpp* cGfx, bool fillOptimization)
{
	if (!cGfx)
		return;

	if (!parent)
		return;

	if (points.empty())
		return;

	y_max = points[0]->getHigh();
	y_min = points[0]->getLow();

	for (unsigned int i = 1; i < points.size(); ++i)
	{
		Candle* c = points[i];
		float y_high = c->getHigh();
		float y_low = c->getLow();
		//float open_pt = c->getOpen();
		//float close_pt = c->getClose();

		if (y_high > y_max)
			y_max = y_high;

		else if (y_low < y_min)
			y_min = y_low;
	}
}

template <>
void Graphable<Candle>::draw(gfxpp* cGfx)
{
	SDL_SetRenderDrawColor(cGfx->getRenderer(), getColor().r, getColor().g, getColor().b, getColor().a);

	float xRange = (float)points.size();
	float yRange = y_max - y_min;

	printf("xRange, yRange (%f,%f)\n", xRange, yRange);

	// Scales coordinates based on graph size and data range.
	float pointXGap = ((float)parent->getWidth()) / xRange;
	float pointYGap = ((float)parent->getHeight()) / yRange;

	printf("PointXGap, PointYGap (%f,%f)\n", pointXGap, pointYGap);
	printf("Total Candles: %d\n", points.size());

	Candle* cCandle = NULL;
	Candle* prevCandle = NULL;

	for (unsigned int i = 0; i < points.size(); ++i)
	{
		float newXValue = i * pointXGap;
		float newOpenValue = (points[i]->getOpen() - y_min) * pointYGap;
		float newCloseValue = (points[i]->getClose() - y_min) * pointYGap;
		float newHighValue = (points[i]->getHigh() - y_min) * pointYGap;
		float newLowValue = (points[i]->getLow() - y_min) * pointYGap;

		// add next point to the background
		cCandle = new Candle(parent->getAxisOriginY() + parent->getHeight() - newOpenValue,
							parent->getAxisOriginY() + parent->getHeight() - newCloseValue,
							parent->getAxisOriginY() + parent->getHeight() - newHighValue,
							parent->getAxisOriginY() + parent->getHeight() - newLowValue);

		// Wick
		// Just above and below the real body are the "wicks" or "shadows." 
		// The wicks show the high and low prices of that day's trading.
		// TODO: Maybe add second line with +1 x offset if too thin to see.
		SDL_RenderDrawLine(cGfx->getRenderer(), 
						   parent->getAxisOriginX() + newXValue, cCandle->getHigh(), 
						   parent->getAxisOriginX() + newXValue, cCandle->getLow());

		// TODO
		// Draw a rectangle for the real body representing the price range between open and close
		//SDL_Rect bgRect;
		//bgRect.x = newXValue - 2; //TODO: Make global variable / 2
		//bgRect.w = 4; //TODO: Make global variable

		// If the close is higher than the open, make the real body green.
		if (points[i]->getClose() > points[i]->getOpen())
		{
			//TODO: Make the rectangle green RUColors::DEFAULT_BUTTON_BORDER_GREEN
			float bgRectHeight = points[i]->getClose() - points[i]->getOpen();
			//bgRect.y = cCandle->getY() - bgRectHeight;
			//bgRect.h = bgRectHeight; // TODO
		}
		// If the close is lower than the open, make the real body red.
		else if (points[i]->getClose() < points[i]->getOpen())
		{
			//TODO: Make the rectangle red RUColors::DEFAULT_BUTTON_BORDER_RED
			float bgRectHeight = points[i]->getOpen() - points[i]->getClose();
			//bgRect.y = 0; // TODO
			//bgRect.h = bgRectHeight; // TODO
		}
		else
		{
			// When open and close are the same, show no real body.
		}

		//SDL_RenderFillRect(cGfx->getRenderer(), &bgRect);

		// save the previous point for later
		if (prevCandle)
			delete prevCandle;
		prevCandle = cCandle;
	}

	if (cCandle)
		delete cCandle;
}

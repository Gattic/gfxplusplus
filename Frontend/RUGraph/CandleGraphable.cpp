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

	yMax = points[0]->getHigh();
	yMin = points[0]->getLow();
	//printf("yMin, yMax (%f,%f)\n", yMin, yMax);

	for (unsigned int i = 1; i < points.size(); ++i)
	{
		Candle* c = points[i];
		float y_high = c->getHigh();
		float y_low = c->getLow();
		//printf("y_low, y_high (%f,%f)\n", y_low, y_high);
		//float open_pt = c->getOpen();
		//float close_pt = c->getClose();

		if (y_high > yMax)
			yMax = y_high;

		else if (y_low < yMin)
			yMin = y_low;
	}
}

template <>
void Graphable<Candle>::draw(gfxpp* cGfx)
{

	float xRange = (float)points.size();
	float yRange = yMax - yMin;

	//printf("yMin, yMax (%f,%f)\n", yMin, yMax);
	//printf("xRange, yRange (%f,%f)\n", xRange, yRange);

	// Scales coordinates based on graph size and data range.
	float pointXGap = ((float)parent->getWidth()) / xRange;
	float pointYGap = ((float)parent->getHeight()) / yRange;

	//printf("PointXGap, PointYGap (%f,%f)\n", pointXGap, pointYGap);
	//printf("Total Candles: %d\n", points.size());

	Candle* cCandle = NULL;
	Candle* prevCandle = NULL;

	for (unsigned int i = 0; i < points.size(); ++i)
	{
		float newXValue = i * pointXGap;
		float newOpenValue = (points[i]->getOpen() - yMin) * pointYGap;
		float newCloseValue = (points[i]->getClose() - yMin) * pointYGap;
		float newHighValue = (points[i]->getHigh() - yMin) * pointYGap;
		float newLowValue = (points[i]->getLow() - yMin) * pointYGap;
		//printf("RAW-CANDLE: %f:%f:%f:%f\n", points[i]->getOpen(), points[i]->getClose(), points[i]->getHigh(), points[i]->getLow());
		//printf("NRM-CANDLE: %f:%f:%f:%f\n", newOpenValue, newCloseValue, newHighValue, newLowValue);
		//printf("++++++++++++++++++++++++++++++++++++++++++++++++++++\n");

		// add next point to the background
		cCandle = new Candle(parent->getAxisOriginY() + parent->getHeight() - newOpenValue,
							parent->getAxisOriginY() + parent->getHeight() - newCloseValue,
							parent->getAxisOriginY() + parent->getHeight() - newHighValue,
							parent->getAxisOriginY() + parent->getHeight() - newLowValue);

		// Wick / Shadow
		// Just above and below the real body are the "wicks" or "shadows." 
		// The wicks show the high and low prices of that day's trading.
		SDL_SetRenderDrawColor(cGfx->getRenderer(), getColor().r, getColor().g, getColor().b, getColor().a);

		if (i == 0)
		{
			// First wick would be drawn at x = 0, so need to center line in bar
			SDL_RenderDrawLine(cGfx->getRenderer(), 
						   parent->getAxisOriginX() + newXValue + (pointXGap / 4), cCandle->getHigh(), 
						   parent->getAxisOriginX() + newXValue + (pointXGap / 4), cCandle->getLow());
		}
		else
		{
			SDL_RenderDrawLine(cGfx->getRenderer(), 
				   parent->getAxisOriginX() + newXValue, cCandle->getHigh(), 
				   parent->getAxisOriginX() + newXValue, cCandle->getLow());
		}

		// Draw a rectangle for the real body representing the price range between open and close
		SDL_Rect bgRect;
		bgRect.x = parent->getAxisOriginX() + newXValue - (pointXGap / 2);
		bgRect.w = pointXGap;

		// If the close is higher than the open, make the real body green.
		if (points[i]->getClose() > points[i]->getOpen())
		{
			SDL_SetRenderDrawColor(cGfx->getRenderer(), 0, 209, 0, 255);

			float bgRectHeight = cCandle->getClose() - cCandle->getOpen();
			bgRect.h = bgRectHeight; // Rendered down, not up
			bgRect.y = cCandle->getClose();
		}
		// If the close is lower than the open, make the real body red.
		else if (points[i]->getClose() < points[i]->getOpen())
		{
			SDL_SetRenderDrawColor(cGfx->getRenderer(), 255, 0, 0, 255);

			float bgRectHeight = cCandle->getOpen() - cCandle->getClose();
			bgRect.h = bgRectHeight; // Rendered down, not up
			bgRect.y = cCandle->getOpen();
		}
		else
		{
			// When open and close are the same, show no real body.
			bgRect.y = cCandle->getOpen();
			bgRect.h = 0;
		}

		SDL_RenderFillRect(cGfx->getRenderer(), &bgRect);

		// save the previous point for later
		if (prevCandle)
			delete prevCandle;
		prevCandle = cCandle;
	}

	if (cCandle)
		delete cCandle;
}

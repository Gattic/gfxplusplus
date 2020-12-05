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
void Graphable<Candle>::computeAxisRanges(gfxpp* cGfx, bool additionOptimization)
{
	if (!cGfx)
		return;

	if (!parent)
		return;

	if (!cGfx->getRenderer())
		return;

	if (points.empty())
		return;

	redoRange = !additionOptimization;
	if(additionOptimization)
	{
		// Is the latest y not within the current range?
		float cY1 = points[points.size()-1]->getHigh();
		float cY2 = points[points.size()-1]->getLow();
		if(!((cY2 >= yMin) && (cY1 <= yMax)))
			redoRange = true;
	}

	if(redoRange)
	{
		float y_max = points[0]->getHigh();
		float y_min = points[0]->getLow();
		//printf("y_min, y_max (%f,%f)\n", y_min, y_max);

		for (unsigned int i = 1; i < points.size(); ++i)
		{
			Candle* c = points[i];
			float y_high = c->getHigh();
			float y_low = c->getLow();
			//printf("y_low, y_high (%f,%f)\n", y_low, y_high);
			//float open_pt = c->getOpen();
			//float close_pt = c->getClose();

			if (y_high > y_max)
				y_max = y_high;

			else if (y_low < y_min)
				y_min = y_low;
		}

		yMin = y_min;
		yMax = y_max;
	}

	//==============================================Normalize the points==============================================

	float xRange = (float)points.size();
	float yRange = yMax - yMin;

	// Scales coordinates based on graph size and data range.
	float pointXGap = ((float)parent->getWidth()) / xRange;
	float pointYGap = ((float)parent->getHeight()) / yRange;

	unsigned int i = 0;
	if(redoRange)
		normalizedPoints.clear();
	else
		i = points.size()-1;

	for (; i < points.size(); ++i)
	{

		// First point would be drawn at x = 0, so we need to add (pointXGap / 2)
		// so that the bar can be drawn left and right.
		float newXValue = i * pointXGap + (pointXGap / 2);
		float newOpenValue = (points[i]->getOpen() - yMin) * pointYGap;
		float newCloseValue = (points[i]->getClose() - yMin) * pointYGap;
		float newHighValue = (points[i]->getHigh() - yMin) * pointYGap;
		float newLowValue = (points[i]->getLow() - yMin) * pointYGap;

		Candle* newCandle = new Candle(parent->getAxisOriginY() + parent->getHeight() - newOpenValue,
							parent->getAxisOriginY() + parent->getHeight() - newCloseValue,
							parent->getAxisOriginY() + parent->getHeight() - newHighValue,
							parent->getAxisOriginY() + parent->getHeight() - newLowValue);

		normalizedPoints.push_back(newCandle);
	}
}

template <>
void Graphable<Candle>::draw(gfxpp* cGfx)
{
	float xRange = (float)normalizedPoints.size();
	float yRange = yMax - yMin;

	// Scales coordinates based on graph size and data range.
	float pointXGap = ((float)parent->getWidth()) / xRange;
	float pointYGap = ((float)parent->getHeight()) / yRange;

	//printf("PointXGap, PointYGap (%f,%f)\n", pointXGap, pointYGap);
	//printf("Total Candles: %d\n", normalizedPoints.size());

	Candle* cCandle = NULL;
	Candle* prevCandle = NULL;

	for (unsigned int i = 0; i < normalizedPoints.size(); ++i)//TODO: REPLACE THIS LOOP
	{
		float newXValue = i * pointXGap + (pointXGap / 2);

		// Add next point to the background
		cCandle = normalizedPoints[i];
		if(!cCandle)
			continue;

		// Wick / Shadow
		// Just above and below the real body are the "wicks" or "shadows." 
		// The wicks show the high and low prices of that day's trading.
		SDL_SetRenderDrawColor(cGfx->getRenderer(),
			getColor().r, getColor().g, getColor().b, getColor().a);

		SDL_RenderDrawLine(cGfx->getRenderer(), 
				   parent->getAxisOriginX() + newXValue, cCandle->getHigh(), 
				   parent->getAxisOriginX() + newXValue, cCandle->getLow());

		// Draw a rectangle for the real body representing the price range between open and close
		SDL_Rect bgRect;
		bgRect.x = parent->getAxisOriginX() + newXValue - (pointXGap / 2);
		bgRect.w = pointXGap;

		// If the close is higher than the open, make the real body green.
		if (normalizedPoints[i]->getClose() > normalizedPoints[i]->getOpen())
		{
			SDL_SetRenderDrawColor(cGfx->getRenderer(), 0, 209, 0, 255);

			float bgRectHeight = cCandle->getClose() - cCandle->getOpen();
			bgRect.h = bgRectHeight; // Rendered down, not up
			bgRect.y = cCandle->getClose();
		}
		// If the close is lower than the open, make the real body red.
		else if (normalizedPoints[i]->getClose() < normalizedPoints[i]->getOpen())
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

		if (i == normalizedPoints.size() - 1)
		{
			// Draw horizontal line in candle graph for the last close price.
			SDL_SetRenderDrawColor(cGfx->getRenderer(), 209, 0, 118, 255);

			SDL_RenderDrawLine(cGfx->getRenderer(), 
				   parent->getAxisOriginX(), cCandle->getClose(), 
				   parent->getAxisOriginX() + newXValue, cCandle->getClose());
		}


		prevCandle = cCandle;
	}
}

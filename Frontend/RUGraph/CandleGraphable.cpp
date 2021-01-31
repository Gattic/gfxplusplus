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
#include "RUCandleGraph.h"

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

	redoRange = true;
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

	// Set the pixels array
	int width = parent->getWidth();
	int height = parent->getHeight();
	if((oldWidth*oldHeight != width*height) || (!pixels))
	{
		pixels = (unsigned int*)malloc(width * height * sizeof(*pixels));
	}

	oldWidth = width;
	oldHeight = height;

	//==============================================Normalize the points==============================================

	unsigned int agg = ((RUCandleGraph*)parent)->getAggregate();
	float xRange = (float)points.size() / (float)agg;
	float yRange = yMax - yMin;

	// Scales coordinates based on graph size and data range.
	float pointXGap = ((float)parent->getWidth()) / xRange;
	float pointYGap = ((float)parent->getHeight()) / yRange;
	if(isinf(pointXGap))
	{
		normalizedPoints.clear();
		return;
	}

	unsigned int i = 0;
	if(redoRange)
		normalizedPoints.clear();
	else
		i = points.size()-1;

		// Aggregate helpers
	unsigned int aggCounter = 0;
	float aggOpenValue = 0.0f;
	float aggCloseValue = 0.0f;
	float aggHighValue = 0.0f;
	float aggLowValue = 0.0f;

	for (; i < points.size(); ++i)
	{

		// First point would be drawn at x = 0, so we need to add (pointXGap / 2)
		// so that the bar can be drawn left and right.
		float newOpenValue = (points[i]->getOpen() - yMin) * pointYGap;
		float newCloseValue = (points[i]->getClose() - yMin) * pointYGap;
		float newHighValue = (points[i]->getHigh() - yMin) * pointYGap;
		float newLowValue = (points[i]->getLow() - yMin) * pointYGap;

		// Time to Aggreagate
		++aggCounter;
		if(aggCounter == 1)
			aggOpenValue=newOpenValue;
		if(aggCounter >= agg)
			aggCloseValue=newCloseValue;
		if(newHighValue > aggHighValue)
			aggHighValue=newHighValue;
		if(newLowValue < aggLowValue)
			aggLowValue=newLowValue;

		// Are we done aggregating data yet?
		if(aggCounter < agg)
			continue;

		// Our aggregated candle
		float newXValue = ((i/agg) * pointXGap) + (pointXGap / 2);
		Candle* newCandle = new Candle(parent->getAxisOriginX() + newXValue,
							parent->getAxisOriginY() + parent->getHeight() - aggOpenValue,
							parent->getAxisOriginY() + parent->getHeight() - aggCloseValue,
							parent->getAxisOriginY() + parent->getHeight() - aggHighValue,
							parent->getAxisOriginY() + parent->getHeight() - aggLowValue);

		// The draw container
		normalizedPoints.push_back(newCandle);

		// Reset the agg helpers
		aggCounter = 0;
		aggOpenValue = 0.0f;
		aggCloseValue = 0.0f;
		aggHighValue = 0.0f;
		aggLowValue = 0.0f;
	}
}

template <>
void Graphable<Candle>::draw(gfxpp* cGfx)
{
	int width = parent->getWidth();
	int height = parent->getHeight();

	float xRange = (float)normalizedPoints.size();
	float yRange = yMax - yMin;

	// Scales coordinates based on graph size and data range.
	float pointXGap = ((float)parent->getWidth()) / xRange;
	float pointYGap = ((float)parent->getHeight()) / yRange;
	if(isinf(pointXGap))
		return;

	//printf("PointXGap, PointYGap (%f,%f)\n", pointXGap, pointYGap);
	//printf("Total Candles: %d\n", normalizedPoints.size());

	unsigned int cBGColor = parent->getBGColor().r;
	cBGColor = cBGColor*0x100 + parent->getBGColor().g;
	cBGColor = cBGColor*0x100 + parent->getBGColor().b;
	cBGColor = cBGColor*0x100 + parent->getBGColor().a;

	unsigned int wickColor = getColor().r;
	wickColor = wickColor*0x100 + getColor().g;
	wickColor = wickColor*0x100 + getColor().b;
	wickColor = wickColor*0x100 + getColor().a;

	unsigned int lineColor = 0xD10076FF;
	unsigned int candleGreenColor = 0x00D100FF;
	unsigned int candleRedColor = 0xFF0000FF;

	// The background color
	for (int y = 0; y < height; ++y)
	{
		unsigned int* row = pixels + y * width;
		for (int x = 0; x < width; ++x)
		{
			row[x] = cBGColor;
		}
	}

	Candle* lastCandle = NULL;
	for (unsigned int i = 0; i < normalizedPoints.size(); ++i)
	{
		// Add next point to the background
		Candle* cCandle = normalizedPoints[i];
		if(!cCandle)
			continue;

		// Wick / Shadow
		// Just above and below the real body are the "wicks" or "shadows." 
		// The wicks show the high and low prices of that day's trading.
		/*int wickX = ((int)cCandle->getX());
		if((wickX < width) && (wickX >= 0))
		{
			for(int wickY = cCandle->getLow(); wickY < cCandle->getHigh(); ++i)
			{
				if((wickY < width) && (wickY >= 0))
				{
					printf("wick: (%d,%d)\n", wickX, wickY);
					//break;
					unsigned int* row = pixels + ((wickY) * width);
					row[wickX] = wickColor;
				}
			}
		}*/

		// Draw a rectangle for the real body representing the price range between open and close
		//printf("candleX[%d]: (%f,%f)\n", i,
			//cCandle->getX() - (pointXGap / 2), cCandle->getX() + (pointXGap / 2));
		for(int cX = cCandle->getX(); cX < cCandle->getX() + pointXGap; ++cX)
		{
			if((cX >= width) || (cX < 0))
				continue;

			int yStart = -1;
			int cY = -1;
			int bodyMaxY = -1;
			unsigned int cColor = 0;

			if (normalizedPoints[i]->getClose() > normalizedPoints[i]->getOpen())
			{
				// If the close is higher than the open, make the real body green.
				cY = cCandle->getClose(); // Rendered down, not up
				yStart = cY;
				bodyMaxY = cCandle->getClose() - cCandle->getOpen();
				cColor = candleGreenColor;
			}
			else if (normalizedPoints[i]->getClose() < normalizedPoints[i]->getOpen())
			{
				// If the close is lower than the open, make the real body red.
				cY = cCandle->getOpen(); // Rendered down, not up
				yStart = cY;
				bodyMaxY = cCandle->getOpen() - cCandle->getClose();
				cColor = candleRedColor;
			}
			else
				continue;

			for(;cY < yStart + bodyMaxY; ++cY)
			{
				if((cY < 0) || (cY >= height))
					continue;

				if(bodyMaxY < 0)
					continue;

				unsigned int* row = pixels + ((cY) * width);
				row[cX] = cColor;
			}
		}

		lastCandle = cCandle;
	}

	// Draw horizontal line in candle graph for the last close price.
	if(lastCandle)
	{
		int cY = parent->getAxisOriginY() + lastCandle->getClose();
		if((cY < width) && (cY >= 0))
		{
			for(int cX = parent->getAxisOriginX(); cX < width; ++ cX)
			{
				unsigned int* row = pixels + ((cY) * width);
				row[cX] = lineColor;
			}
		}
	}

	// Update the texture with the new pixel data
	SDL_UpdateTexture(parent->getBackground(), NULL, pixels, width * sizeof(*pixels));
}

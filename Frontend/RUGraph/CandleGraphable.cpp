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

template <>
void Graphable<Candle>::computeAxisRanges(bool additionOptimization)
{
	if (!parent)
		return;

	if (points.empty())
		return;

	redoRange = !additionOptimization;
	if(additionOptimization)
	{
		// Is the latest y not within the current range?
		float cY1 = points[points.size()-1]->getHigh();
		float cY2 = points[points.size()-1]->getLow();
		if(!((cY2 >= getYMin()) && (cY1 <= getYMax())))
			redoRange = true;
	}

	redoRange = true; // TODO: force this?
	float vscale = parent->getVScale();
	if(redoRange)
	{
		float x_max = points[0]->getX();
		float x_min = x_max;
		float local_x_max = x_max;
		float local_x_min = x_min;

		float y_max = parent->getYMax();
		float y_min = parent->getYMin();
		float local_y_max = points[0]->getHigh();
		float local_y_min = points[0]->getLow();

		for (unsigned int i = 1; i < points.size(); ++i)
		{
			Candle* c = points[i];
			float x_pt = c->getX();
			float y_high = c->getHigh();
			float y_low = c->getLow();

			if (x_pt > local_x_max)
				local_x_max = x_pt;
			else if (x_pt < local_x_min)
				local_x_min = x_pt;
	//printf("Candle-PRE[%s]: %f:%f\n", parent->getName().c_str(), local_x_max, local_x_min);

			if (y_high > local_y_max)
				local_y_max = y_high;
			else if (y_low < local_y_min)
				local_y_min = y_low;

			if (y_high > y_max)
				y_max = y_high;
			else if (y_low < y_min)
				y_min = y_low;
		}

		setLocalXMin(local_x_min);
		setLocalXMax(local_x_max);
		setLocalYMin(local_y_min);
		setLocalYMax(local_y_max * vscale);

		if(y_min < parent->getYMin())
			parent->setYMin(y_min);
		if(y_max > parent->getYMax())
			parent->setYMax(y_max * vscale);
	}

	//printf("Candle-PRE[%s]: %f:%f\n", parent->getName().c_str(), getLocalXMax(), getLocalXMin());

	//==============================================Normalize the points==============================================

	unsigned int agg = parent->getAggregate();
	float xRange = (float)points.size() / (float)agg;
	float yRange = getYMax() - getYMin();
	if(points.size() % agg)
		++xRange;

	// Scales coordinates based on graph size and data range.
	float pointXGap = ((float)parent->getWidth()) / xRange;
	float pointYGap = ((float)parent->getHeight()) / yRange;
	if(isinf(pointXGap))
	{
		normalizedPoints.clear();
		return;
	}

	// Size up the normalized points vec
	while(normalizedPoints.size() < xRange)
	{
		Candle* newCandle = new Candle();
		normalizedPoints.push_back(newCandle);
	}

	while(normalizedPoints.size() > xRange)
	{
		normalizedPoints.erase(normalizedPoints.begin()+normalizedPoints.size()-1);
	}

	//Get a head start?
	unsigned int i = 0;
	if(!redoRange)
		i = points.size()-1;

	// Aggregate helpers
	unsigned int aggCounter = 0;
	float aggOpenValue = 0.0f;
	float aggCloseValue = 0.0f;
	float aggHighValue = 0.0f;
	float aggLowValue = 0.0f;

	unsigned int normalCounter = 0;
	for (; i < points.size(); ++i)
	{
		// First point would be drawn at x = 0, so we need to add (pointXGap / 2)
		// so that the bar can be drawn left and right.
		float newOpenValue = (points[i]->getOpen() - getYMin()) * pointYGap;
		float newCloseValue = (points[i]->getClose() - getYMin()) * pointYGap;
		float newHighValue = (points[i]->getHigh() - getYMin()) * pointYGap;
		float newLowValue = (points[i]->getLow() - getYMin()) * pointYGap;

		// Time to Aggreagate
		++aggCounter;
		aggCloseValue=newCloseValue;
		if(aggCounter == 1)
		{
			aggOpenValue=newOpenValue;
			aggHighValue=newHighValue;
			aggLowValue=newLowValue;
		}

		// Set high and low of aggregate
		if(newHighValue > aggHighValue)
			aggHighValue=newHighValue;
		if(newLowValue < aggLowValue)
			aggLowValue=newLowValue;

		// Are we done aggregating data yet?
		if((aggCounter < agg) && (i < points.size()-1))
			continue;

		// Our aggregated candle
		//float newXValue = ((i/agg) * pointXGap);
		//float newXValue = i * pointXGap + (pointXGap / 2);
		float newXValue = ((i/agg) * pointXGap)+ (pointXGap / 2);
		normalizedPoints[normalCounter]->setX(parent->getAxisOriginX() + newXValue);
		normalizedPoints[normalCounter]->setOpen(parent->getAxisOriginY() + parent->getHeight() - aggOpenValue);
		normalizedPoints[normalCounter]->setClose(parent->getAxisOriginY() + parent->getHeight() - aggCloseValue);
		normalizedPoints[normalCounter]->setHigh(parent->getAxisOriginY() + parent->getHeight() - aggHighValue);
		normalizedPoints[normalCounter]->setLow(parent->getAxisOriginY() + parent->getHeight() - aggLowValue);

		// The draw container
		++normalCounter;

		// Reset the agg helpers
		aggCounter = 0;
		aggOpenValue = 0.0f;
		aggCloseValue = 0.0f;
		aggHighValue = 0.0f;
		aggLowValue = 0.0f;
	}

	parent->requireDrawUpdate();
}

template <>
void Graphable<Candle>::draw(gfxpp* cGfx)
{
	float xRange = (float)normalizedPoints.size();
	//float xRange = getXMax() - getXMin();
	float yRange = getYMax() - getYMin();
	//printf("Candle[%s]: xRange: %f\n", parent->getName().c_str(),  xRange);

	// Scales coordinates based on graph size and data range.
	unsigned int agg = parent->getAggregate();
	float pointXGap = ((float)parent->getWidth()) / xRange;
	float pointYGap = ((float)parent->getHeight()) / yRange;

	if(isinf(pointXGap))
		return;

	// Dont draw with bad data
	if(! ((xRange == points.size()/agg) || (xRange == (points.size()/agg)+1)) )
		return;

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

	// Time to draw each candle
	Candle* cCandle = NULL;
	for (unsigned int i = 0; i < normalizedPoints.size(); ++i)
	{
		// Add next point to the background
		float cX = i * pointXGap + (pointXGap / 2);
		cCandle = normalizedPoints[i];
		if(!cCandle)
			continue;

		//float cX = cCandle->getX();

		// Wick / Shadow
		// Just above and below the real body are the "wicks" or "shadows." 
		// The wicks show the high and low prices of that day's trading.
		SDL_SetRenderDrawColor(cGfx->getRenderer(),
			getColor().r, getColor().g, getColor().b, getColor().a);

		SDL_RenderDrawLine(cGfx->getRenderer(), 
				   parent->getAxisOriginX() + cX, cCandle->getHigh(), 
				   parent->getAxisOriginX() + cX, cCandle->getLow());

		// Draw a rectangle for the real body representing the price range between open and close
		SDL_Rect bgRect;
		bgRect.x = parent->getAxisOriginX() + cX - (pointXGap / 2);
		bgRect.w = pointXGap;

		// Rendered down, not up, this block will probably not make sense
		// If the close is higher than the open, make the real body green.
		//printf("candle(%f, %f, %f, %f)\n", cCandle->getOpen(), cCandle->getHigh(), cCandle->getLow(), cCandle->getClose());
		if (normalizedPoints[i]->getClose() > normalizedPoints[i]->getOpen())
		{
			SDL_SetRenderDrawColor(cGfx->getRenderer(), 255, 0, 0, 255);

			float bgRectHeight = cCandle->getClose() - cCandle->getOpen();
			bgRect.h = bgRectHeight;
			bgRect.y = cCandle->getOpen();
		}
		// If the close is lower than the open, make the real body red.
		else if (normalizedPoints[i]->getClose() < normalizedPoints[i]->getOpen())
		{
			SDL_SetRenderDrawColor(cGfx->getRenderer(), 0, 209, 0, 255);

			float bgRectHeight = cCandle->getOpen() - cCandle->getClose();
			bgRect.h = bgRectHeight;
			bgRect.y = cCandle->getClose();
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
				   parent->getAxisOriginX() + cX, cCandle->getClose());
		}
	}
}

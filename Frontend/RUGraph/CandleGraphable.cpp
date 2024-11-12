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
		float yHigh = points[points.size()-1]->getHigh();
		float yLow = points[points.size()-1]->getLow();
		if((yLow < getYMin()) || (yHigh > getYMax()))
			redoRange = true;
	}

	//redoRange = true;
	unsigned int i = 0;
	//if(redoRange)
		//i = points.size()-1;
	for (; i < points.size(); ++i)
	{
		Candle* c = points[points.size()-1];
		float x_pt = c->getX();
		float y_high = c->getHigh();
		float y_low = c->getLow();

		setXMax(x_pt);
		setXMin(x_pt);
		setYMax(y_high);
		setYMin(y_low);
	}

	// Set the parents
	parent->setXMin(getXMin());
	parent->setXMax(getXMax());
	parent->setYMin(getYMin());
	parent->setYMax(getYMax());

	//printf("Candle-PRE[%s]: %f:%f\n", parent->getName().c_str(), getXMax(), getXMin());

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
	i = 0;
	//if(!redoRange)
		//i = points.size()-1;

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
		normalizedPoints[normalCounter]->setOpen(parent->getAxisOriginY() + (float)parent->getHeight() - aggOpenValue);
		normalizedPoints[normalCounter]->setClose(parent->getAxisOriginY() + (float)parent->getHeight() - aggCloseValue);
		normalizedPoints[normalCounter]->setHigh(parent->getAxisOriginY() + (float)parent->getHeight() - aggHighValue);
		normalizedPoints[normalCounter]->setLow(parent->getAxisOriginY() + (float)parent->getHeight() - aggLowValue);

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
	if (!parent)
		return;

	float xRange = static_cast<float>(normalizedPoints.size());
	float yRange = getYMax() - getYMin();

	if (isinf(xRange * yRange))
		return;

	unsigned int agg = parent->getAggregate();
	float parentWidth = static_cast<float>(parent->getWidth());
	float parentHeight = static_cast<float>(parent->getHeight());
	float pointXGap = parentWidth / xRange;
	float pointYGap = parentHeight / yRange;

	if (!(((xRange == points.size() / agg) || (xRange == (points.size() / agg) + 1))))
		return;

	unsigned int cBGColor = (parent->getBGColor().r << 24) | (parent->getBGColor().g << 16) |
		(parent->getBGColor().b << 8) | parent->getBGColor().a;

	unsigned int wickColor = 0xE0E0E0FF; // Light color for the wick
	unsigned int candleGreenColor = 0x00D100FF; // Green color for the candle
	unsigned int candleRedColor = 0xFF0000FF; // Red color for the candle

	std::vector<SDL_Point> pointsToDraw;  // Store the points to be drawn
	std::vector<SDL_Rect> rectsToFill;   // Store the rectangles to be filled
	std::vector<unsigned int> colors;     // Store the colors for the rectangles
	std::vector<unsigned int> outlineColors; // Store the outline colors for the rectangles

	Candle* cCandle = NULL; // Using NULL instead of nullptr
	for (unsigned int i = 0; i < normalizedPoints.size(); ++i)
	{
		float cX = i * pointXGap + (pointXGap / 2);
		cCandle = normalizedPoints[i];
		if (!cCandle)
			continue;

		// Wick / Shadow
		SDL_Point highPoint = { static_cast<int>(parent->getAxisOriginX() + cX), static_cast<int>(cCandle->getHigh()) };
		SDL_Point lowPoint = { static_cast<int>(parent->getAxisOriginX() + cX), static_cast<int>(cCandle->getLow()) };
		pointsToDraw.push_back(highPoint);
		pointsToDraw.push_back(lowPoint);

		// Draw a rectangle for the real body representing the price range between open and close
		SDL_Rect bgRect;
		bgRect.x = static_cast<int>(parent->getAxisOriginX() + cX - (pointXGap / 2));
		bgRect.w = static_cast<int>(pointXGap);

		if (cCandle->getClose() > cCandle->getOpen())
		{
			float bgRectHeight = cCandle->getClose() - cCandle->getOpen();
			bgRect.h = static_cast<int>(bgRectHeight);
			bgRect.y = static_cast<int>(cCandle->getOpen());

			rectsToFill.push_back(bgRect);
			colors.push_back(candleGreenColor);
			outlineColors.push_back(0x00FF00FF | (static_cast<unsigned int>(cCandle->getHigh()) << 24)); // Green outline with dynamic alpha based on high value
		}
		else if (cCandle->getClose() < cCandle->getOpen())
		{
			float bgRectHeight = cCandle->getOpen() - cCandle->getClose();
			bgRect.h = static_cast<int>(bgRectHeight);
			bgRect.y = static_cast<int>(cCandle->getClose());

			rectsToFill.push_back(bgRect);
			colors.push_back(candleRedColor);
			outlineColors.push_back(0xFF0000FF | (static_cast<unsigned int>(cCandle->getLow()) << 24)); // Red outline with dynamic alpha based on low value
		}
		else
		{
			// When open and close are the same, show no real body.
			bgRect.y = static_cast<int>(cCandle->getOpen());
			bgRect.h = 0;
		}

		if (i == normalizedPoints.size() - 1)
		{
			// Draw horizontal line in candle graph for the last close price.
			SDL_Point startPoint = { parent->getAxisOriginX(), static_cast<int>(cCandle->getClose()) };
			SDL_Point endPoint = { static_cast<int>(parent->getAxisOriginX() + cX), static_cast<int>(cCandle->getClose()) };

			pointsToDraw.push_back(startPoint);
			pointsToDraw.push_back(endPoint);
		}
	}

	// Set the render color for the wick
	SDL_SetRenderDrawColor(cGfx->getRenderer(), (wickColor >> 24) & 0xFF, (wickColor >> 16) & 0xFF,
		(wickColor >> 8) & 0xFF, wickColor & 0xFF);

	// Render all the points at once
	SDL_RenderDrawPoints(cGfx->getRenderer(), &pointsToDraw[0], pointsToDraw.size());

	// Render the filled rectangles with outlines
	for (size_t i = 0; i < rectsToFill.size(); ++i)
	{
		unsigned int color = colors[i];
		unsigned int outlineColor = outlineColors[i];
		SDL_SetRenderDrawColor(cGfx->getRenderer(), (color >> 24) & 0xFF, (color >> 16) & 0xFF,
			(color >> 8) & 0xFF, color & 0xFF);

		SDL_RenderFillRect(cGfx->getRenderer(), &rectsToFill[i]);

		// Render the outline
		SDL_SetRenderDrawColor(cGfx->getRenderer(), (outlineColor >> 24) & 0xFF, (outlineColor >> 16) & 0xFF,
			(outlineColor >> 8) & 0xFF, outlineColor & 0xFF);

		SDL_RenderDrawRect(cGfx->getRenderer(), &rectsToFill[i]);
	}
}


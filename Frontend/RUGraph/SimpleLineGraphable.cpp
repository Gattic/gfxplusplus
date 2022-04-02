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
#include "../GFXUtilities/SimpleLine.h"
#include "RUGraph.h"

//TODO: Separate Scalar1D from Point2D and implement this add fncality

template <>
void Graphable<SimpleLine>::computeAxisRanges(bool additionOptimization)
{
	if (!parent)
		return;

	if (points.empty())
		return;

	redoRange = !additionOptimization;
	if(additionOptimization)
	{
		// Is the latest y not within the current range?
		float cY = points[points.size()-1]->getY();
		if(!((cY >= getYMin()) && (cY <= getYMax())))
			redoRange = true;
	}

	redoRange = true;
	float vscale = parent->getVScale();
	if(redoRange)
	{
		float y_max = parent->getYMax();
		float y_min = parent->getYMin();
		float local_y_max = points[0]->getY();
		float local_y_min = local_y_max;

		for (unsigned int i = 1; i < points.size(); ++i)
		{
			SimpleLine* pt = points[i];
			float y_pt = pt->getY();

			if (y_pt > local_y_max)
				local_y_max = y_pt;
			else if (y_pt < local_y_min)
				local_y_min = y_pt;

			if (y_pt > y_max)
				y_max = y_pt;
			else if (y_pt < y_min)
				y_min = y_pt;
		}

		setLocalYMin(local_y_min);
		setLocalYMax(local_y_max * vscale);

		if(y_min < parent->getYMin())
			parent->setYMin(y_min);
		if(y_max > parent->getYMax())
			parent->setYMax(y_max * vscale);
	}

	//==============================================Normalize the points==============================================

	unsigned int agg = parent->getAggregate();
	float xRange = (float)points.size() / (float)agg;
	float yRange = getYMax() - getYMin();
	if(points.size() % agg)
		++xRange;

	//printf("Line[%s]: xRange;pointXGap: %f; %f\n", parent->getName().c_str(),  xRange, pointXGap);
	float pointYGap = ((float)parent->getHeight()) / yRange;

	// Size up the normalized points vec
	while(normalizedPoints.size() < xRange)
	{
		SimpleLine* newPoint = new SimpleLine();
		normalizedPoints.push_back(newPoint);
	}

	while(normalizedPoints.size() > xRange)
	{
		normalizedPoints.erase(normalizedPoints.begin()+normalizedPoints.size()-1);
	}

	unsigned int i = 0;
	if(!redoRange)
		i = points.size()-1;

	// Aggregate helpers
	unsigned int aggCounter = 0;
	float aggValue = 0.0f;

	unsigned int normalCounter = 0;
	for (; i < points.size(); ++i)
	{
		float newYValue = (points[i]->getY() - getYMin()) * pointYGap;

		// Time to Aggreagate
		++aggCounter;
		aggValue=newYValue;

		// Are we done aggregating data yet?
		if((aggCounter < agg) && (i < points.size()-1))
			continue;

		// Our aggregated points
		normalizedPoints[normalCounter]->setY(parent->getAxisOriginY() + parent->getHeight() - aggValue);

		// The draw container
		++normalCounter;

		// Reset the agg helpers
		aggCounter = 0;
		aggValue = 0.0f;
	}

	parent->requireDrawUpdate();
}

template <>
void Graphable<SimpleLine>::draw(gfxpp* cGfx)
{
	SDL_SetRenderDrawColor(cGfx->getRenderer(), getColor().r, getColor().g, getColor().b,
						   getColor().a);

	float yRange = getYMax() - getYMin();

	//printf("norm-size: %lu\n", normalizedPoints.size());
	for (unsigned int i = 0; i < normalizedPoints.size(); ++i)
	{
		// add it to the background
		SimpleLine* cPoint = normalizedPoints[i];

		SDL_RenderDrawLine(cGfx->getRenderer(), parent->getAxisOriginX(), cPoint->getY() - 1,
								   parent->getAxisOriginX() + parent->getWidth(), cPoint->getY() - 1);
		SDL_RenderDrawLine(cGfx->getRenderer(), parent->getAxisOriginX(), cPoint->getY(),
								   parent->getAxisOriginX() + parent->getWidth(), cPoint->getY());
		SDL_RenderDrawLine(cGfx->getRenderer(), parent->getAxisOriginX(), cPoint->getY() + 1,
								   parent->getAxisOriginX() + parent->getWidth(), cPoint->getY() + 1);
	}
}

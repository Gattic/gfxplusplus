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

//TODO: Separate Scalar1D from Point2D and implement this add fncality

template <>
void Graphable<Point2>::computeAxisRanges(bool additionOptimization)
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
		if((cY < getYMin()) || (cY > getYMax()))
			redoRange = true;
	}

	//redoRange = true;
	unsigned int i = 0;
	//if(redoRange)
		//i = points.size()-1;
	for (; i < points.size(); ++i)
	{
		Point2* pt = points[i];
		float x_pt = pt->getX();
		float y_pt = pt->getY();

		// Local X check
		if (x_pt > getLocalXMax())
			setLocalXMax(x_pt);
		else if (x_pt < getLocalXMin())
			setLocalXMin(x_pt);

		// Local Y check
		if (y_pt > getLocalYMax())
			setLocalYMax(y_pt);
		else if (y_pt < getLocalYMin())
			setLocalYMin(y_pt);
	}

	// Set the parents
	if(getLocalXMin() < parent->getXMin())
		parent->setXMin(getLocalXMin());
	if(getLocalXMax() > parent->getXMax())
		parent->setXMax(getLocalXMax());

	if(getLocalYMin() < parent->getYMin())
		parent->setYMin(getLocalYMin());
	if(getLocalYMax() > parent->getYMax())
		parent->setYMax(getLocalYMax());

	//==============================================Normalize the points==============================================

	unsigned int agg = parent->getAggregate();
	float xRange = (float)points.size() / (float)agg;
	float yRange = getYMax() - getYMin();
	if(points.size() % agg)
		++xRange;

	float pointXGap = ((float)parent->getWidth()) / xRange;
	//printf("Line[%s]: xRange;pointXGap: %f; %f\n", parent->getName().c_str(),  xRange, pointXGap);
	float pointYGap = ((float)parent->getHeight()) / yRange;
	if(isinf(pointXGap))
	{
		normalizedPoints.clear();
		return;
	}

	// Size up the normalized points vec
	while(normalizedPoints.size() < xRange)
	{
		Point2* newPoint = new Point2();
		normalizedPoints.push_back(newPoint);
	}

	while(normalizedPoints.size() > xRange)
	{
		normalizedPoints.erase(normalizedPoints.begin()+normalizedPoints.size()-1);
	}

	i = 0;
	//if(!redoRange)
		//i = points.size()-1;

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
		float newXValue = (normalCounter * pointXGap);
		//float newXValue = i * pointXGap;
		normalizedPoints[normalCounter]->setX(parent->getAxisOriginX() + newXValue);
		normalizedPoints[normalCounter]->setY(parent->getAxisOriginY() + (float)parent->getHeight() - aggValue);

		// The draw container
		++normalCounter;

		// Reset the agg helpers
		aggCounter = 0;
		aggValue = 0.0f;
	}

	parent->requireDrawUpdate();
}

template <>
void Graphable<Point2>::draw(gfxpp* cGfx)
{
	if (!parent)
		return;

	SDL_SetRenderDrawColor(cGfx->getRenderer(), getColor().r, getColor().g, getColor().b,
						   getColor().a);

	float vscale = parent->getVScale();
	//printf("norm-size: %lu\n", normalizedPoints.size());

	Point2* prevPoint = NULL;
	unsigned int i = 0;
	for (; i < normalizedPoints.size(); ++i)
	{
		// add it to the background
		Point2* cPoint = normalizedPoints[i];

		// draw a thick line from the previous to the current point
		if ((prevPoint) && (i > 0))
		{
			/*if(parent->getName() == "RoRGraph")
			{
				printf("Dim[%s]: %d:%d\n", parent->getName().c_str(), parent->getWidth(), parent->getWidth());
				printf("Range[%s]: %f:%f\n", parent->getName().c_str(), getYMin(), getYMax());
				printf("LocalRange[%s]: %f:%f\n", parent->getName().c_str(), getLocalYMin(), getLocalYMax());
				printf("p[%s][%u]:  (%f, %f); c(%f, %f)\n", parent->getName().c_str(), i, prevPoint->getX(), prevPoint->getY(), cPoint->getX(), cPoint->getY());
				printf("-----\n");
			}*/
			SDL_RenderDrawLine(cGfx->getRenderer(), prevPoint->getX(), prevPoint->getY() - 1,
							   cPoint->getX(), cPoint->getY() - 1);
			SDL_RenderDrawLine(cGfx->getRenderer(), prevPoint->getX(), prevPoint->getY(),
							   cPoint->getX(), cPoint->getY());
			SDL_RenderDrawLine(cGfx->getRenderer(), prevPoint->getX(), prevPoint->getY() + 1,
							   cPoint->getX(), cPoint->getY() + 1);
		}

		prevPoint = cPoint;
	}
}

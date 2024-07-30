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
#include "../GFXUtilities/point2.h"

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

		setXMax(x_pt);
		setXMin(x_pt);
		setYMax(y_pt);
		setYMin(y_pt);
	}

	// Set the parents
	parent->setXMin(getXMin());
	parent->setXMax(getXMax());
	parent->setYMin(getYMin());
	parent->setYMax(getYMax());

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

    SDL_Color color = getColor();
    SDL_SetRenderDrawColor(cGfx->getRenderer(), color.r, color.g, color.b, color.a);

    float vscale = parent->getVScale();

    if (normalizedPoints.empty())
        return;

    // Prepare the points array for SDL_RenderDrawLines
    std::vector<SDL_Point> points(normalizedPoints.size());

    for (unsigned int i = 0; i < normalizedPoints.size(); ++i)
    {
        Point2* cPoint = normalizedPoints[i];
        SDL_Point point;
        point.x = static_cast<int>(cPoint->getX());
        point.y = static_cast<int>(cPoint->getY());
        points[i] = point;
    }

    // Draw all the lines at once
    SDL_RenderDrawLines(cGfx->getRenderer(), &points[0], points.size());
}

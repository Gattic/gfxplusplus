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
#include "../GFXUtilities/Circle.h"

template <>
void Graphable<Circle>::computeAxisRanges(bool additionOptimization)
{
	if (!parent)
		return;

	if (points.empty())
		return;

	redoRange = !additionOptimization;
	if(additionOptimization)
	{
		// Is the latest y not within the current range?
		float cY = points[points.size()-1]->getCenter()->getY();
		if(!((cY >= getYMin()) && (cY <= getYMax())))
			redoRange = true;
	}

	redoRange = true;
	float vscale = parent->getVScale();
	if(redoRange)
	{
		const Point2* firstPoint = points[0]->getCenter();
		float x_max = firstPoint->getX();
		float x_min = x_max;
		float local_x_max = x_max;
		float local_x_min = x_min;

		float y_max = parent->getYMax();
		float y_min = parent->getYMin();
		float local_y_max = firstPoint->getY();
		float local_y_min = local_y_max;

		for (unsigned int i = 1; i < points.size(); ++i)
		{
			const Point2* pt = points[i]->getCenter();
			float x_pt = pt->getX();
			float y_pt = pt->getY();

			if (x_pt > local_x_max)
				local_x_max = x_pt;
			else if (x_pt < local_x_min)
				local_x_min = x_pt;

			if (y_pt > local_y_max)
				local_y_max = y_pt;
			else if (y_pt < local_y_min)
				local_y_min = y_pt;

			if (x_pt > x_max)
				x_max = x_pt;
			else if (x_pt < x_min)
				x_min = x_pt;

			if (y_pt > y_max)
				y_max = y_pt;
			else if (y_pt < y_min)
				y_min = y_pt;
		}

		setLocalXMin(local_x_min);
		setLocalXMax(local_x_max);
		setLocalYMin(local_y_min);
		setLocalYMax(local_y_max * vscale);

		if(x_min < parent->getXMin())
			parent->setXMin(x_min);
		if(x_max > parent->getXMax())
			parent->setXMax(x_max * vscale);

		if(y_min < parent->getYMin())
			parent->setYMin(y_min);
		if(y_max > parent->getYMax())
			parent->setYMax(y_max * vscale);
	}

	//==============================================Normalize the points==============================================

	float yRange = getYMax() - getYMin();

	// Size up the normalized points vec
	while(normalizedPoints.size() < points.size())
	{
		Circle* newPoint = new Circle();
		normalizedPoints.push_back(newPoint);
	}

	while(normalizedPoints.size() > points.size())
	{
		normalizedPoints.erase(normalizedPoints.begin()+normalizedPoints.size()-1);
	}

	unsigned int i = 0;
	if(!redoRange)
		i = points.size()-1;

	for (; i < points.size(); ++i)
	{
		float newXValue = ((points[i]->getCenter()->getX() - getXMin()) / (getXMax()-getXMin())) * ((float)parent->getWidth());
		//printf("bounds[%u]: %d:%d\n", i, parent->getWidth(), parent->getHeight());
		//printf("xRange[%u]: %f:%f\n", i, getXMin(), getXMax());
		//printf("unscaledXValue[%u]: %f\n", i, (points[i]->getCenter()->getX() - getXMin()));
		//printf("newXValue[%u]: %f\n", i, newXValue);
		float newYValue = ((points[i]->getCenter()->getY() - getYMin()) / (getYMax()-getYMin())) * ((float)parent->getHeight());
		//printf("yRange[%u]: %f:%f\n", i, getYMin(), getYMax());
		//printf("unscaledYValue[%u]: %f\n", i, (points[i]->getCenter()->getY() - getYMin()));
		//printf("newYValue[%u]: %f\n", i, newYValue);
		//printf("adjustedYValue[%u]: %f\n", i, (float)parent->getHeight()-newYValue);
		//printf("-----\n");

		normalizedPoints[i]->setRadius(points[i]->getRadius());
		normalizedPoints[i]->setCenter(new Point2(parent->getAxisOriginX() + newXValue,
							parent->getAxisOriginY() + (float)parent->getHeight() - newYValue));
	}

	parent->requireDrawUpdate();
}

template <>
void Graphable<Circle>::draw(gfxpp* cGfx)
{
	for (unsigned int i = 0; i < normalizedPoints.size(); ++i)
	{
		Circle* pt = normalizedPoints[i];

		if (pt->getRadius() <= 0)
			return;
	
		if (pt->getCenter() == NULL)
			return;
	
		SDL_SetRenderDrawColor(cGfx->getRenderer(), getColor().r, getColor().g, getColor().b, getColor().a);
	
		const Point2* cFocalPoint = pt->getCenter();
		double radius = pt->getRadius();
		for (int i = -radius; i < radius; ++i)
		{
			int xIndex = cFocalPoint->getX() + i;

			std::map<int, int> newMap;
			for (int j = -radius; j < radius; ++j)
			{
				int yIndex = cFocalPoint->getY() + j;

				// calculate the distance
				double distance = sqrt(pow(((double)i), 2.0f) + pow(((double)j), 2.0f));
				double hue =
					distance / sqrt(pow(((double)radius), 2.0f) + pow(((double)radius), 2.0f));
				if (distance > radius)
					continue;

				// get the color
				int8_t redMask = 0;
				int8_t greenMask = 0;
				int8_t blueMask = 0;
				unsigned int colorMask = gfxpp::RGBfromHue(hue, &redMask, &greenMask, &blueMask);
	
				// Set the color and draw the point
				SDL_SetRenderDrawColor(cGfx->getRenderer(), redMask, greenMask, blueMask, SDL_ALPHA_OPAQUE);
				SDL_RenderDrawPoint(cGfx->getRenderer(), xIndex, yIndex);
				//printf("DRAW-CIRCLE[%d][%d]: %08X\n", xIndex, yIndex, colorMask);
			}
		}
	}
}

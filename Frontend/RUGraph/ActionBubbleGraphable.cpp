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
#include "../GFXUtilities/ActionBubble.h"

template <>
void Graphable<ActionBubble>::computeAxisRanges(bool additionOptimization)
{
	if (!parent)
		return;

	if (points.empty())
		return;

	redoRange = !additionOptimization;
	if(additionOptimization)
	{
		// Is the latest y not within the current range?
		float cY = points[points.size()-1]->getFocalPoint()->getY();
		if(!((cY >= getYMin()) && (cY <= getYMax())))
			redoRange = true;
	}

	redoRange = true;
	float vscale = parent->getVScale();
	if(redoRange)
	{
		float x_max = points[0]->getFocalPoint()->getX();
		float x_min = x_max;
		float local_x_max = x_max;
		float local_x_min = x_min;

		float y_max = parent->getYMax();
		float y_min = parent->getYMin();
		float local_y_max = points[0]->getFocalPoint()->getY();
		float local_y_min = local_y_max;

		for (unsigned int i = 1; i < points.size(); ++i)
		{
			const Point2* pt = points[i]->getFocalPoint();
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

	float pointYGap = ((float)parent->getHeight()) / yRange;

	// Size up the normalized points vec
	while(normalizedPoints.size() < points.size())
	{
		ActionBubble* newPoint = new ActionBubble();
		normalizedPoints.push_back(newPoint);
	}

	while(normalizedPoints.size() > points.size())
	{
		normalizedPoints.erase(normalizedPoints.begin()+normalizedPoints.size()-1);
	}

	unsigned int i = 0;
	if(!redoRange)
		i = points.size()-1;

	// Raw cost and fp->Y are the same
	for (; i < points.size(); ++i)
	{
		float newXValue = ((points[i]->getFocalPoint()->getX() - getXMin()) / getXMax()) * parent->getWidth();
		float newYValue = (points[i]->getFocalPoint()->getY() - getYMin()) * pointYGap;

		normalizedPoints[i]->setRadius(points[i]->getRadius());
		normalizedPoints[i]->setCost(points[i]->getCost());
		normalizedPoints[i]->setQuantity(points[i]->getQuantity());
		normalizedPoints[i]->setActionType(points[i]->getActionType());
		normalizedPoints[i]->setFocalPoint(new Point2(parent->getAxisOriginX() + newXValue, parent->getAxisOriginY() + parent->getHeight() - newYValue));
	}

	parent->requireDrawUpdate();
}

template <>
void Graphable<ActionBubble>::draw(gfxpp* cGfx)
{
	//TODO: Use the action type to change the color for each point in the first loop
	SDL_SetRenderDrawColor(cGfx->getRenderer(), getColor().r, getColor().g, getColor().b, getColor().a);

	for (unsigned int i = 0; i < normalizedPoints.size(); ++i)
	{
		const ActionBubble* cBubble = normalizedPoints[i];
		if (!cBubble)
			continue;

		const Point2* cFocalPoint = cBubble->getFocalPoint();
		if (!cFocalPoint)
			continue;

		double radius = cBubble->getRadius();
		if (radius <= 0)
			continue;
	
		for (int i = -radius; i < radius; ++i)
		{
			int xIndex = cFocalPoint->getX() + i;

			std::map<int, int> newMap;
			for (int j = -radius; j < radius; ++j)
			{
				int yIndex = cFocalPoint->getY() + j;
	
				// set the color and draw the point
				SDL_RenderDrawPoint(cGfx->getRenderer(), parent->getX() + xIndex,
									parent->getY() + yIndex);
			}
		}
	}
}

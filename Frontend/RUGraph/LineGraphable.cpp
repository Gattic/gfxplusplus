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

//TODO: Separate Scalar1D from Point2D and implement this add fncality

template <>
void Graphable<Point2>::computeAxisRanges(gfxpp* cGfx, bool additionOptimization)
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
		float cY = points[points.size()-1]->getY();
		if(!((cY >= yMin) && (cY <= yMax)))
			redoRange = true;
	}

	if(redoRange)
	{
		float y_max = points[0]->getY();
		float y_min = y_max;

		for (unsigned int i = 1; i < points.size(); ++i)
		{
			Point2* pt = points[i];
			float y_pt = pt->getY(), x_pt = pt->getX();

			if (y_pt > y_max)
				y_max = y_pt;

			else if (y_pt < y_min)
				y_min = y_pt;
		}

		yMin = y_min;
		yMax = y_max;
	}

	//==============================================Normalize the points==============================================

	float xRange = (float)points.size();
	float yRange = yMax - yMin;

	float pointXGap = ((float)parent->getWidth()) / xRange;
	float pointYGap = ((float)parent->getHeight()) / yRange;

	unsigned int i = 0;
	if(redoRange)
		normalizedPoints.clear();
	else
		i = points.size()-1;

	for (; i < points.size(); ++i)
	{
		float newXValue = i * pointXGap;
		float newYValue = (points[i]->getY() - yMin) * pointYGap;
		normalizedPoints.push_back(new Point2(parent->getAxisOriginX() + newXValue,
							parent->getAxisOriginY() + parent->getHeight() - newYValue));
	}
}

template <>
void Graphable<Point2>::draw(gfxpp* cGfx)
{
	SDL_SetRenderDrawColor(cGfx->getRenderer(), getColor().r, getColor().g, getColor().b,
						   getColor().a);

	float xRange = (float)points.size(); // points per x axis
	float yRange = yMax - yMin;

	float pointXGap = ((float)parent->getWidth()) / xRange;
	float pointYGap = ((float)parent->getHeight()) / yRange;

	if((redoRange) || (normalizedPoints.size() < 2))
	{
		Point2* prevPoint = NULL;
		unsigned int i = 0;
		for (; i < normalizedPoints.size(); ++i)
		{
			// add it to the background
			Point2* cPoint = normalizedPoints[i];

			// draw a thick line from the previous to the current point
			if ((prevPoint) && (i > 0))
			{
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
	else
	{
		//
		SDL_Rect dRect;
		dRect.x = 0;
		dRect.y = 0;
		dRect.w = parent->getWidth() - pointXGap;
		dRect.h = parent->getHeight();
		SDL_RenderCopy(cGfx->getRenderer(), parent->getBackground(), NULL, &dRect);

		unsigned int i = normalizedPoints.size()-1;
		Point2* prevPoint = normalizedPoints[i-1];
		for (; i < normalizedPoints.size(); ++i)
		{
			// add it to the background
			Point2* cPoint = normalizedPoints[i];

			// draw a thick line from the previous to the current point
			if ((prevPoint) && (i > 0))
			{
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
}

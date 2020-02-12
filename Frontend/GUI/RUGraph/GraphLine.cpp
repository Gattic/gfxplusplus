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
#include "GraphLine.h"
#include "../../GFXUtilities/point2.h"
#include "../../GItems/RUColors.h"
#include "RUGraph.h"

GraphLine::GraphLine(RUGraph* newParent, SDL_Color newColor) : Graphable(newParent, newColor)
{
	// put code here to initialize private GraphLine variables
}

GraphLine::~GraphLine()
{
	// put code here to destroy private GraphLine variables
}

void GraphLine::draw(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawColor(renderer, getColor().r, getColor().g, getColor().b, getColor().a);

	float xRange = (float)points.size(); // points per x axis
	float yRange = y_max - y_min;

	float pointXGap = ((float)parent->getWidth()) / xRange;
	float pointYGap = ((float)parent->getHeight()) / yRange;

	Point2* cPoint = NULL;
	Point2* prevPoint = NULL;
	for (unsigned int i = 0; i < points.size(); ++i)
	{
		float newXValue = i * pointXGap;
		float newYValue = (points[i]->getY() - y_min) * pointYGap;
		// add it to the background
		cPoint = new Point2(parent->getAxisOriginX() + newXValue,
							parent->getAxisOriginY() + parent->getHeight() - newYValue);

		// draw a thick line from the previous to the current point
		if ((prevPoint) && (i > 0))
		{
			SDL_RenderDrawLine(renderer, prevPoint->getX(), prevPoint->getY() - 1, cPoint->getX(),
							   cPoint->getY() - 1);
			SDL_RenderDrawLine(renderer, prevPoint->getX(), prevPoint->getY(), cPoint->getX(),
							   cPoint->getY());
			SDL_RenderDrawLine(renderer, prevPoint->getX(), prevPoint->getY() + 1, cPoint->getX(),
							   cPoint->getY() + 1);
		}

		// save the previous point for later
		if (prevPoint)
			delete prevPoint;
		prevPoint = cPoint;
	}

	if (cPoint)
		delete cPoint;
}

std::string GraphLine::getType() const
{
	return "GraphLine";
}

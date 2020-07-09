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
void Graphable<Point2>::computeAxisRanges(gfxpp* cGfx, bool fillOptimization)
{
	if (!cGfx)
		return;

	if (points.empty())
		return;

	float y_max = points[0]->getY();
	float y_min = y_max;
	float x_max = points[0]->getX();
	float x_min = x_max;

	for (unsigned int i = 1; i < points.size(); ++i)
	{
		Point2* pt = points[i];
		float y_pt = pt->getY(), x_pt = pt->getX();
		if (y_pt > y_max)
			y_max = y_pt;

		else if (y_pt < y_min)
			y_min = y_pt;

		if (x_pt < x_min)
			x_min = x_pt;

		else if (x_pt > x_max)
			x_max = x_pt;
	}

	// Save the old values for an optimization later
	float oldxMin = xMin;
	float oldxMax = xMax;
	float oldyMin = yMin;
	float oldyMax = yMax;

	//==============================================BELOW HERE PUT INTO WRAPPER HELPER FNC==============================================

	int xRange = x_max - x_min;
	int yRange = y_max - y_min;

	// Canvas Size
	// If we need more room, extend by doubling
	int newWidth = canvasWidth;
	while (newWidth < xRange)
	{
		if (newWidth == -1)
			return;

		// Add an upper limit here to work on certain size images only
		// if (newWidth >= SOME_LARGE_NUMBER)
		// return;

		if (newWidth == 0)
			newWidth = 16;
		else
			newWidth *= 2;
	}

	// If we need more room, extend by doubling
	int newHeight = canvasHeight;
	while (newHeight < yRange)
	{
		if (newHeight == -1)
			return;

		// Add an upper limit here to work on certain size images only
		// if (newWidth >= SOME_LARGE_NUMBER)
		// return;

		if (newHeight == 0)
			newHeight = 16;
		else
			newHeight *= 2;
	}

	if(((canvasWidth < newWidth) || (canvasHeight < newHeight)) &&
		((newWidth > 0) && (newHeight > 0)))
	{
		printf("Canvas Texture(%d,%d)\n", canvasWidth, canvasHeight);
		printf("New Texture(%d,%d)\n", newWidth, newHeight);
		printf("=================================================\n");

		//SDL_Texture* newGraph = NULL;
		/*SDL_Texture* newGraph = SDL_CreateTexture(cGfx->getRenderer(),
			SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET, newWidth, newHeight);

		// Couldnt create a new texture
		if(!newGraph)
		{
			newGraph = NULL;
			printf("HOOPLA\n");
			return;
		}

		// Shrink the old texture to fit the new point and avoid redrawing
		//TODO COPY RECT HERE

		// Delete the old texture
		if (rawGraph)
			SDL_DestroyTexture(rawGraph);
		rawGraph = newGraph;*/ // Replace the old texture

		// Set the new raw graph size
		canvasWidth = newWidth;
		canvasHeight = newHeight;
	}

	// Canvas is big enough but is the drawing the same?
	/*if((xRange == 0.0f) || (yRange == 0.0f))
	{
		// Unset so do nothing
		drawType = OPT_NOTHING;
	}
	else if(prevSize == points.size() - 1)
	{
		// Time Series Assumption
	}
	else if((x_min < oldxMin) ||
		(x_max > oldxMax) ||
		(y_min < oldyMin) ||
		(y_max > oldyMax))
	{
		// Create a new texture
		printf("xMin(o,n)(%f,%f)\n", x_min, oldxMin);
		printf("xMax(o,n)(%f,%f)\n", x_max, oldxMax);
		printf("yMin(o,n)(%f,%f)\n", y_min, oldyMin);
		printf("yMax(o,n)(%f,%f)\n", y_max, oldyMax);
		printf("-------------------------------------------------\n");
	}*/

	//==============================================ABOVE HERE PUT INTO WRAPPER HELPER FNC==============================================

	prevSize = points.size();
	xMin = x_min;
	xMax = x_max;
	yMin = y_min;
	yMax = y_max;

	//==============================================SCRATCH SPACE BELOW==============================================
}


// This function is recommended for optimizations.
template <>
void Graphable<Point2>::add(gfxpp* cGfx, const Point2* newPoint)
{
	if (!newPoint)
		return;

	points.push_back(new Point2(points.size(), newPoint->getY()));
	computeAxisRanges(cGfx, true); // add point optimization
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

	Point2* cPoint = NULL;
	Point2* prevPoint = NULL;

	// Short circuit the draw
	// Time Series Assumption
	unsigned int i = 0;
	//if(prevSize == points.size() - 1)
	//	i = prevSize - 1;

	for (; i < points.size(); ++i)
	{
		float newXValue = i * pointXGap;
		float newYValue = (points[i]->getY() - yMin) * pointYGap;
		// add it to the background
		cPoint = new Point2(parent->getAxisOriginX() + newXValue,
							parent->getAxisOriginY() + parent->getHeight() - newYValue);

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

		// save the previous point for later
		if (prevPoint)
			delete prevPoint;
		prevPoint = cPoint;
	}

	if (cPoint)
		delete cPoint;
}

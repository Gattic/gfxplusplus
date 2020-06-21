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

template <>
void Graphable<Point2>::computeAxisRanges(gfxpp* cGfx, bool fillOptimization)
{
	if (!cGfx)
		return;

	if (points.empty())
		return;

	y_max = points[0]->getY();
	y_min = y_max;
	x_max = points[0]->getX();
	x_min = x_max;

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

	int calculatedWidth = x_max - x_min;
	int calculatedHeight = y_max - y_min;

	// If we need more room, extend by doubling
	int newWidth = drawWidth;
	while (newWidth < calculatedWidth)
	{
		if (newWidth == -1)
			return;

		// Add an upper limit here to work on certain size images only
		// if (newWidth >= SOMME_LARGE_NUMBER)
		// return;

		if (newWidth == 0)
			newWidth = 16;
		else
			newWidth *= 2;
	}

	// If we need more room, extend by doubling
	int newHeight = drawHeight;
	while (newHeight < calculatedHeight)
	{
		if (newHeight == -1)
			return;

		if (newHeight == 0)
			newHeight = 16;
		else
			newHeight *= 2;
	}

	/*printf("Calculated(%d,%d)\n", calculatedWidth, calculatedHeight);
	printf("New(%d,%d)\n", newWidth, newHeight);
	printf("Draw(%d,%d)\n", drawWidth, drawHeight);
	printf("---------------------------------------------------------------\n");*/
	drawWidth = newWidth;
	drawHeight = newHeight;

	// Do we need to recreate the texutre?
	/*if ((newWidth >= drawWidth) || (newHeight >= drawHeight))
	{
		// Delete the old texture
		if (rawGraph)
			free(rawGraph);

		// Set the new raw graph size
		drawWidth = newWidth;
		drawHeight = newHeight;

		// Create a new texture
		printf("New Texture(%d,%d)\n", drawWidth, drawHeight);
		//rawGraph = SDL_CreateTexture(cGfx->getRenderer(),
		//	SDL_PIXELFORMAT_RGBA8888,
		//	SDL_TEXTUREACCESS_TARGET, drawWidth, drawHeight);
	}*/





	// Redo the texture
	/*if (drawType == OPT_FULL)
	{
		// If there was a graph, then resize it
		if (rawGraph)
		{
			//Render to different target?
		}
		else
		{
			// Create a new texture
			rawGraph = SDL_CreateTexture(cGfx->getRenderer(),
				SDL_PIXELFORMAT_RGBA8888,
				SDL_TEXTUREACCESS_TARGET, x_max - x_min, y_max - y_min);
		}
	}
	else if (drawType == OPT_FILL)
	{
		// Create or resize the new texture
		// Set Render target to desitination texture
		// RenderCopy source texture
	}
	else if (drawType == OPT_NOTHING)
	{
		// Do nothing, just refresh
	}*/
}

template <>
void Graphable<Candle>::computeAxisRanges(gfxpp* cGfx, bool fillOptimization)
{
	//
}


//===========================START ADD FUNCTIONS===========================
// These function is recommended for optimizations.

template <>
void Graphable<Point2>::add(gfxpp* cGfx, const Point2* newPoint)
{
	if (!newPoint)
		return;

	//TODO: Separate Scalar1D from Point2D and implement this add fncality
	//points.push_back(new T(points.size(), newPoint->getY()));
	//drawType = OPT_FILL;
	//computeAxisRanges(cGfx, true); // add point optimization
}


//===========================END ADD FUNCTIONS===========================

//===========================START DRAW FUNCTIONS===========================

template <>
void Graphable<Point2>::draw(gfxpp* cGfx)
{
	SDL_SetRenderDrawColor(cGfx->getRenderer(), getColor().r, getColor().g, getColor().b,
						   getColor().a);

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

template <>
void Graphable<Candle>::draw(gfxpp* cGfx)
{
	SDL_SetRenderDrawColor(cGfx->getRenderer(), getColor().r, getColor().g, getColor().b, getColor().a);

	float xRange = (float)points.size() / 4; // points per x axis (4 values per candlestick)
	float yRange = y_max - y_min;

	float pointXGap = ((float)parent->getWidth()) / xRange;
	float pointYGap = ((float)parent->getHeight()) / yRange;

	Candle* cPoint = NULL;
	Candle* prevPoint = NULL;

	// points[i]->getY() = float rawOpen,
	// points[i+1]->getY() = float rawClose,
	// points[i+2]->getY() = float rawHigh,
	// points[i+3]->getY() = float rawLow

	//TODO: I COMMENTED THIS NEXT BLOCK OUT IN MY REFACTOR
	/*for (unsigned int i = 0; i < points.size(); i+4) // i+4 because each candlestick has four values
	{
		float newXValue = (i/4) * pointXGap; // divide by 4 to account for 4 values per candlestick
		float newYValue = (points[i]->getY() - y_min) * pointYGap;
		// add it to the background
		cPoint = new Point2(parent->getAxisOriginX() + newXValue,
							parent->getAxisOriginY() + parent->getHeight() - newYValue);
		cPoint = NULL;

		// draw a thick line from the previous to the current point
		if ((prevPoint) && (i > 0))
		{
			//SDL_RenderDrawLine(cGfx->getRenderer(), prevPoint->getX(), prevPoint->getY() - 1, cPoint->getX(), cPoint->getY() - 1);
			//SDL_RenderDrawLine(cGfx->getRenderer(), prevPoint->getX(), prevPoint->getY(), cPoint->getX(), cPoint->getY());
			//SDL_RenderDrawLine(cGfx->getRenderer(), prevPoint->getX(), prevPoint->getY() + 1, cPoint->getX(), cPoint->getY() + 1);

			// TODO: Create new container for rectangles. wxyz. Save in SDL_Rect.

			//SDL_Rect bgRect;
			//bgRect.x = 0;
			//bgRect.y = 0;
			//bgRect.w = 0;
			//bgRect.h = 0;

			// SDL_RenderFillRect(cGfx->getRenderer(), &bgRect);

		}

		// save the previous point for later
		if (prevPoint)
			delete prevPoint;
		prevPoint = cPoint;
	}

	if (cPoint)
		delete cPoint;*/


	// TODO:

	// SDL_RenderDrawRect for real body representing the price range between the open and close
	// of that day's trading. When the real body is colored red, it means the close
	// was lower than the open. If the real body is green, it means the close was higher than the
	// open.

	// Just above and below the real body are the "shadows" or "wicks." The wicks show the high
	// and low prices of that day's trading. Use SDL_RenderDrawLine. Example:
	// SDL_RenderDrawLine(cGfx->getRenderer(), prevPoint->getX(), prevPoint->getY() - 1, cPoint->getX(), cPoint->getY() - 1);

	// TODO: Update for Rectangle instead of Point
	// save the previous point for later
}

//===========================END DRAW FUNCTIONS===========================

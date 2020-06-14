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
#include "GraphCandlestick.h"
#include "../GFXUtilities/point2.h"
#include "../GItems/RUColors.h"
#include "../Graphics/graphics.h"
#include "RUGraph.h"

/*
Candlesticks are useful when trading as they show four price points
(open, close, high, and low) throughout the period of time the trader specifies.
The relationship between the days open, high, low, and close determines the look
of the daily candlestick.
*/

GraphCandlestick::GraphCandlestick(RUGraph* newParent, SDL_Color newColor)
	: Graphable(newParent, newColor)
{
	// put code here to initialize private GraphLine variables
}

GraphCandlestick::~GraphCandlestick()
{
	// put code here to destroy private GraphLine variables
}

void GraphCandlestick::draw(gfxpp* cGfx)
{
	SDL_SetRenderDrawColor(cGfx->getRenderer(), getColor().r, getColor().g, getColor().b, getColor().a);

	float xRange = (float)points.size() / 4; // points per x axis (4 values per candlestick)
	float yRange = y_max - y_min;

	float pointXGap = ((float)parent->getWidth()) / xRange;
	float pointYGap = ((float)parent->getHeight()) / yRange;

	Point2* cPoint = NULL;
	Point2* prevPoint = NULL;

	// points[i]->getY() = float rawOpen,
	// points[i+1]->getY() = float rawClose,
	// points[i+2]->getY() = float rawHigh,
	// points[i+3]->getY() = float rawLow
	for (unsigned int i = 0; i < points.size(); i+4) // i+4 because each candlestick has four values
	{
		float newXValue = (i/4) * pointXGap; // divide by 4 to account for 4 values per candlestick
		float newYValue = (points[i]->getY() - y_min) * pointYGap;
		// add it to the background
		cPoint = new Point2(parent->getAxisOriginX() + newXValue,
							parent->getAxisOriginY() + parent->getHeight() - newYValue);

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
		delete cPoint;


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

std::string GraphCandlestick::getType() const
{
	return "GraphCandlestick";
}
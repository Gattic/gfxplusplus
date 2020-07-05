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
void Graphable<Candle>::computeAxisRanges(gfxpp* cGfx, bool fillOptimization)
{
	if (!cGfx)
		return;

	if (points.empty())
		return;

	y_max = points[0]->getHigh();
	y_min = y_max;
	x_max = points.size();
	x_min = x_max;

	for (unsigned int i = 1; i < points.size(); ++i)
	{
		Candle* c = points[i];
		float y_pt = c->getHigh();

		if (y_pt > y_max)
			y_max = y_pt;

		else if (y_pt < y_min)
			y_min = y_pt;
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
		// if (newWidth >= SOME_LARGE_NUMBER)
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
}

template <>
void Graphable<Candle>::draw(gfxpp* cGfx)
{
	SDL_SetRenderDrawColor(cGfx->getRenderer(), getColor().r, getColor().g, getColor().b, getColor().a);

	float xRange = (float)points.size();
	float yRange = y_max - y_min;

	float pointXGap = ((float)parent->getWidth()) / xRange;
	float pointYGap = ((float)parent->getHeight()) / yRange;

	Point2* cPoint = NULL;
	Point2* prevPoint = NULL;

	for (unsigned int i = 0; i < points.size(); ++i)
	{
		float newXValue = i * pointXGap;
		float newYValue = (points[i]->getHigh() - y_min) * pointYGap;

		// add next point to the background
		cPoint = new Point2(parent->getAxisOriginX() + newXValue,
							parent->getAxisOriginY() + parent->getHeight() - newYValue);

		// Wick 
		// Just above and below the real body are the "wicks" or "shadows." 
		// The wicks show the high and low prices of that day's trading.
		// TODO: Maybe add second line with +1 x offset if too thin to see.
		SDL_RenderDrawLine(cGfx->getRenderer(), cPoint->getX(), points[i]->getHigh(), 
						   cPoint->getX(), points[i]->getLow());

		// TODO
		// Draw a rectangle for the real body representing the price range between open and close
		//SDL_Rect bgRect;
		//bgRect.x = cPoint->getX() - 2; //TODO: Make global variable / 2
		//bgRect.w = 4; //TODO: Make global variable

		// If the close is higher than the open, make the real body green.
		if (points[i]->getClose() > points[i]->getOpen())
		{
			//TODO: Make the rectangle green RUColors::DEFAULT_BUTTON_BORDER_GREEN
			float bgRectHeight = points[i]->getClose() - points[i]->getOpen();
			//bgRect.y = cPoint->getY() - bgRectHeight;
			//bgRect.h = bgRectHeight; // TODO
		}
		// If the close is lower than the open, make the real body red.
		else if (points[i]->getClose() < points[i]->getOpen())
		{
			//TODO: Make the rectangle red RUColors::DEFAULT_BUTTON_BORDER_RED
			float bgRectHeight = points[i]->getOpen() - points[i]->getClose();
			//bgRect.y = 0; // TODO
			//bgRect.h = bgRectHeight; // TODO
		}
		else
		{
			// When open and close are the same, show no real body.
		}

		//SDL_RenderFillRect(cGfx->getRenderer(), &bgRect);

		// save the previous point for later
		if (prevPoint)
			delete prevPoint;
		prevPoint = cPoint;
	}

	if (cPoint)
		delete cPoint;
}
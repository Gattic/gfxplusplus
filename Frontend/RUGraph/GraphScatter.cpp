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

#include "GraphScatter.h"
#include "../GFXUtilities/point2.h"
#include "../GItems/RUColors.h"
#include "../Graphics/graphics.h"
#include "RUGraph.h"

GraphScatter::GraphScatter(RUGraph* newParent, SDL_Color newColor, int pointSz)
	: Graphable(newParent, newColor)
{
	pointSize = pointSz;
}

GraphScatter::~GraphScatter()
{
	pointSize = 0;
}

void GraphScatter::draw(gfxpp* cGfx)
{
	SDL_SetRenderDrawColor(cGfx->getRenderer(), getColor().r, getColor().g, getColor().b,
						   getColor().a);

	// draw the line
	float xRange = (x_max - x_min) * 1.000001;
	float yRange = y_max - y_min;
	float pointXGap = ((float)parent->getWidth()) / xRange;
	float pointYGap = ((float)parent->getHeight()) / yRange;

	Point2* cPoint = NULL;
	Point2* prevPoint = NULL;
	for (unsigned int i = 0; i < points.size(); ++i)
	{
		float newXValue = (points[i]->getX() - x_min) * pointXGap;
		float newYValue = (points[i]->getY() - y_min) * pointYGap;

		// add it to the background
		cPoint = new Point2(parent->getAxisOriginX() + newXValue,
							parent->getAxisOriginY() + parent->getHeight() - newYValue);

		drawPointOutline(cGfx, cPoint->getX(), cPoint->getY());

		// save the previous point for later
		if (prevPoint)
			delete prevPoint;
		prevPoint = cPoint;
	}

	if (cPoint)
		delete cPoint;
}

void GraphScatter::drawPointOutline(gfxpp* cGfx, int cx, int cy, int r)
{
	if (r < 0 || cx < 0 || cy < 0)
		return;
	if (r == 0)
		r = pointSize / 2;
	int x = r - 1, y = 0, dx = 1, dy = 1, err = dx - (r << 1);
	while (x >= y)
	{
		// TODO: Draw a circlw instead
		SDL_RenderDrawPoint(cGfx->getRenderer(), cx + x, cy + y);
		SDL_RenderDrawPoint(cGfx->getRenderer(), cx + y, cy + x);
		SDL_RenderDrawPoint(cGfx->getRenderer(), cx - y, cy + x);
		SDL_RenderDrawPoint(cGfx->getRenderer(), cx - x, cy + y);
		SDL_RenderDrawPoint(cGfx->getRenderer(), cx - x, cy - y);
		SDL_RenderDrawPoint(cGfx->getRenderer(), cx - y, cy - x);
		SDL_RenderDrawPoint(cGfx->getRenderer(), cx + y, cy - x);
		SDL_RenderDrawPoint(cGfx->getRenderer(), cx + x, cy - y);

		if (err <= 0)
		{
			++y;
			err += dy;
			dy += 2;
		}
		if (err > 0)
		{
			--x;
			dx += 2;
			err += dx - (r << 1);
		}
	}
}

/*
 * @brief draw point
 * @details draws a filled point on the renderer at (x,y) with radius r using the Midpoint circle
 * algorithm.
 * @param cGfx the central graphics object
 * @param cx the desired central x-coordinate
 * @param cy the desired central y-coordinate
 * @param r the desired circle radius
 */
void GraphScatter::drawPoint(gfxpp* cGfx, int cx, int cy, int r)
{
	if (r == 0)
		r = pointSize / 2;
	for (int i = 1; i <= r; ++i)
		drawPointOutline(cGfx, cx, cy, i);
}

std::string GraphScatter::getType() const
{
	return "GraphScatter";
}

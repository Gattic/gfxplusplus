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
#include "../GFXUtilities/point2.h"
#include "../Graphics/graphics.h"
#include "Backend/Database/GList.h"
#include "RUGraph.h"

Graphable::Graphable(RUGraph* newParent, SDL_Color newColor)
{
	rawGraph = NULL;
	parent = newParent;
	setColor(newColor);
	drawType = OPT_FULL;
	drawWidth = 0;
	drawHeight = 0;

	x_max = 0.0f;
	x_min = 0.0f;
	y_max = 0.0f;
	y_min = 0.0f;
}

Graphable::~Graphable()
{
	rawGraph = NULL;
	drawType = OPT_NOTHING;
	drawWidth = -1;
	drawHeight = -1;

	// dangling parent pointer (get it?)
	parent = NULL;

	x_max = 0.0f;
	x_min = 0.0f;

	y_min = 0.0f;
	y_max = 0.0f;

	clear();
}

SDL_Color Graphable::getColor() const
{
	return lineColor;
}

void Graphable::setColor(SDL_Color newColor)
{
	lineColor = newColor;
}

void Graphable::computeAxisRanges(gfxpp* cGfx, bool fillOptimization)
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

	/*if(fillOptimization)
	{
	}



	// Redo the texture
	if (drawType == OPT_FULL)
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

// This function is recommended for optimizations.
void Graphable::addPoint(gfxpp* cGfx, const Point2* newPoint)
{
	if (!newPoint)
		return;

	points.push_back(new Point2(points.size(), newPoint->getY()));
	drawType = OPT_FILL;
	computeAxisRanges(cGfx, true); // add point optimization
}

void Graphable::set(gfxpp* cGfx, const std::vector<Point2*>& newPoints)
{
	if (newPoints.empty())
		return;

	points = newPoints;
	drawType = OPT_FULL;
	computeAxisRanges(cGfx);
}

void Graphable::set(gfxpp* cGfx, const shmea::GList& newLine)
{
	if (newLine.empty())
		return;

	points.clear();
	for (unsigned int i = 0; i < newLine.size(); ++i)
		points.push_back(new Point2(i, newLine.getFloat(i)));
	computeAxisRanges(cGfx);
	drawType = OPT_FULL;
}

void Graphable::clear()
{
	drawType = OPT_NOTHING;
	for (unsigned int i = 0; i < points.size(); ++i)
	{
		Point2* cPoint = points[i];
		if (!cPoint)
			continue;
		delete cPoint;
		points.erase(points.begin() + i);
	}

	points.clear();

	parent = NULL;
	x_max = 0.0f;
	x_min = 0.0f;
	y_max = 0.0f;
	y_min = 0.0f;
}

void Graphable::updateBackground(gfxpp* cGfx)
{
	//Does this do anything?
	//if (!parent || !parent->isVisible() || !(parent->getWidth() > 0 && parent->getHeight() > 0))
	//	return;

	// Set the render target to draw the cached texture
	// RenderTarget

	// draw the line
	draw(cGfx);
	drawType = OPT_FULL;
}

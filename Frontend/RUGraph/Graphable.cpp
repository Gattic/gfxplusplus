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
#include "../../include/Backend/Database/GList.h"
#include "../GFXUtilities/point2.h"
#include "RUGraph.h"

Graphable::Graphable(RUGraph* newParent, SDL_Color newColor)
{
	parent = newParent;
	setColor(newColor);

	x_max = 0.0f;
	x_min = 0.0f;
	y_max = 0.0f;
	y_min = 0.0f;
}

Graphable::~Graphable()
{
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

void Graphable::computeAxisRanges()
{
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
}

void Graphable::setPoints(const std::vector<Point2*>& newPoints)
{
	if (newPoints.empty())
		return;

	points = newPoints;
	computeAxisRanges();
}

void Graphable::setLine(const shmea::GList& newLine)
{
	if (newLine.empty())
		return;

	points.clear();
	for (unsigned int i = 0; i < newLine.size(); ++i)
		points.push_back(new Point2(i, newLine.getFloat(i)));
	computeAxisRanges();
}

void Graphable::clear()
{
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
	if (!parent || !parent->isVisible() || !(parent->getWidth() > 0 && parent->getHeight() > 0))
		return;

	// draw the line
	draw(cGfx);
}

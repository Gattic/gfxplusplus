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
#include "RULineGraph.h"
#include "../Graphics/graphics.h"
#include "../GFXUtilities/point2.h"
#include "Graphable.h"

RULineGraph::RULineGraph(int newWidth, int newHeight, int newQuadrants)
	: RUGraph(newWidth, newHeight, newQuadrants)
{
	//
}

RULineGraph::~RULineGraph()
{
	clear();
}

void RULineGraph::add(std::string label, const Point2* newPoint, SDL_Color lineColor, bool recompute)
{
	Point2* plotterPoint = new Point2(newPoint->getX(), newPoint->getY());

	if (lines.find(label) == lines.end())
	{
		std::vector<Point2*> newPointVec;
		newPointVec.push_back(plotterPoint);
		set(label, newPointVec, lineColor);
		return;
	}

	Graphable<Point2>* cPlotter = lines[label];
	cPlotter->add(plotterPoint, recompute);
	cPlotter->setColor(lineColor);

	// DON'T trigger the draw update here
}

void RULineGraph::set(const std::string& label, const std::vector<Point2*>& graphPoints, SDL_Color lineColor)
{
	Graphable<Point2>* newPlotter;
	if (lines.find(label) != lines.end())
	{
		newPlotter = lines[label];
		newPlotter->setColor(lineColor);
	}
	else
	{
		newPlotter = new Graphable<Point2>(this, lineColor);
		if (newPlotter)
			lines[label] = newPlotter;
	}

	newPlotter->set(graphPoints);

	// trigger the draw update
	drawUpdate = true;
}

void RULineGraph::updateBackground(gfxpp* cGfx)
{
	if(!cGfx)
		return;

	RUGraph::updateBackground(cGfx);

	// draw the lines
	std::map<std::string, Graphable<Point2>*>::iterator it;

	for (it = lines.begin(); it != lines.end(); ++it)
	{
		Graphable<Point2>* g = it->second;
		if (g)
			g->updateBackground(cGfx);
	}
}

void RULineGraph::update()
{
	// compute the lines
	std::map<std::string, Graphable<Point2>*>::iterator it;
	for (it = lines.begin(); it != lines.end(); ++it)
	{
		Graphable<Point2>* g = it->second;
		if (g)
			g->computeAxisRanges();
	}
}

void RULineGraph::clear(bool toggleDraw)
{
	std::map<std::string, Graphable<Point2>*>::iterator it;

	for (it = lines.begin(); it != lines.end(); ++it)
		delete it->second;
	lines.clear();

	xMin = FLT_MAX;
	xMax = FLT_MIN;
	yMin = FLT_MAX;
	yMax = FLT_MIN;

	if (toggleDraw)
		drawUpdate = true;
}

std::string RULineGraph::getType() const
{
	return "RULineGraph";
}

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
#include "RUScatterGraph.h"
#include "../GFXUtilities/point2.h"
#include "../GUI/Text/RULabel.h"
#include "../Graphics/graphics.h"
#include "Backend/Database/GList.h"
#include "Backend/Database/GTable.h"
#include "Backend/Database/GType.h"
#include "GraphScatter.h"
#include "Graphable.h"

RUScatterGraph::RUScatterGraph(int newWidth, int newHeight, int newQuadrants)
	: RUGraph(newWidth, newHeight, newQuadrants)
{
	//
}

RUScatterGraph::~RUScatterGraph()
{
	//
}

void RUScatterGraph::add(gfxpp* cGfx, const std::string& label, const Point2& newPoint)
{
	Point2* plotterPoint = new Point2(newPoint.getX(), newPoint.getY());

	if (lines.find(label) == lines.end())
		return;

	Graphable* cPlotter = lines[label];
	// cPlotter->add(cGfx, newPoint);//Uncomment this when the class becomes RULineGraph

	// trigger the draw update
	drawUpdate = true;
}

void RUScatterGraph::set(gfxpp* cGfx, const std::string& label, const std::vector<Point2*>& graphPoints,
				  SDL_Color lineColor)
{
	Graphable* newPlotter;
	if (lines.find(label) != lines.end())
		newPlotter = lines[label];
	else
	{
		newPlotter = new GraphScatter(this, lineColor);

		if (newPlotter)
			lines[label] = newPlotter;
	}

	newPlotter->set(cGfx, graphPoints);

	// trigger the draw update
	drawUpdate = true;
}

void RUScatterGraph::set(gfxpp* cGfx, const std::string& label, const shmea::GList& graphPoints,
				  SDL_Color lineColor)
{
	Graphable* newPlotter;
	if (lines.find(label) != lines.end())
		newPlotter = lines[label];
	else
	{
		newPlotter = new GraphScatter(this, lineColor);

		if (newPlotter)
			lines[label] = newPlotter;
	}

	newPlotter->set(cGfx, graphPoints);

	// trigger the draw update
	drawUpdate = true;
}

/*void RUScatterGraph::set(gfxpp* cGfx, const shmea::GTable& graphPoints)//old addScatterPoints
{
	// pairs of (expected, predicted)
	if (graphPoints.numberOfCols() % 2 == 1)
		return;

	// set line colors
	std::vector<SDL_Color> colorOpts;
	colorOpts.push_back(RUColors::DEFAULT_COLOR_LINE);
	colorOpts.push_back(RUColors::GRAPH_LINE_2);
	colorOpts.push_back(RUColors::GRAPH_LINE_3);
	colorOpts.push_back(RUColors::GRAPH_LINE_4);
	colorOpts.push_back(RUColors::GRAPH_LINE_5);

	// jump pairs
	for (unsigned int i = 0; i < graphPoints.numberOfCols(); i += 2)
	{
		shmea::GList xPoints = graphPoints.getCol(i);
		shmea::GList yPoints = graphPoints.getCol(i + 1);

		// build the line
		std::vector<Point2*> points;
		for (unsigned int p = 0; p < xPoints.size(); ++p)
			points.push_back(new Point2(xPoints.getDouble(p), yPoints.getDouble(p)));

		// set the line
		int pairIndex = i / 2;
		set(cGfx, shmea::GType::intTOstring(pairIndex), points, 1, colorOpts[pairIndex % 5]);
	}
}*/

std::string RUScatterGraph::getType() const
{
	return "RUScatterGraph";
}

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
#include "RUCandleGraph.h"
#include "../Graphics/graphics.h"
#include "../GFXUtilities/Candle.h"
#include "Graphable.h"

const std::string RUCandleGraph::CANDLE_LABEL = "candles";

RUCandleGraph::RUCandleGraph(int newWidth, int newHeight, int newQuadrants)
	: RUGraph(newWidth, newHeight, newQuadrants)
{
	//
}

RUCandleGraph::~RUCandleGraph()
{
	clear();
}

void RUCandleGraph::add(const Candle* newPoint, SDL_Color lineColor)
{
	Candle* plotterPoint = new Candle(newPoint->getX(), newPoint->getOpen(), newPoint->getClose(),
		newPoint->getHigh(), newPoint->getLow());

	if (candles.find(CANDLE_LABEL) == candles.end())
	{
		std::vector<Candle*> newPointVec;
		newPointVec.push_back(plotterPoint);
		set(newPointVec, lineColor);
		return;
	}

	Graphable<Candle>* cPlotter = candles[CANDLE_LABEL];
	cPlotter->add(plotterPoint, false);

	// DON'T trigger the draw update here
}

void RUCandleGraph::set(const std::vector<Candle*>& graphPoints, SDL_Color lineColor)
{
	Graphable<Candle>* newPlotter;
	if (candles.find(CANDLE_LABEL) != candles.end())
		newPlotter = candles[CANDLE_LABEL];
	else
	{
		newPlotter = new Graphable<Candle>(this, lineColor);

		if (newPlotter)
			candles[CANDLE_LABEL] = newPlotter;
	}

	newPlotter->set(graphPoints);

	// trigger the draw update
	drawUpdate = true;
}

void RUCandleGraph::addIndicator(std::string label, const Point2* newPoint, SDL_Color lineColor)
{
	Point2* plotterPoint = new Point2(newPoint->getX(), newPoint->getY());

	if (indicators.find(label) == indicators.end())
	{
		std::vector<Point2*> newPointVec;
		newPointVec.push_back(plotterPoint);
		setIndicator(label, newPointVec, lineColor);
		return;
	}

	Graphable<Point2>* cPlotter = indicators[label];
	cPlotter->add(plotterPoint, false);

	// DON'T trigger the draw update here
}

void RUCandleGraph::setIndicator(std::string label, const std::vector<Point2*>& graphPoints, SDL_Color lineColor)
{
	Graphable<Point2>* newPlotter;
	if (indicators.find(label) != indicators.end())
		newPlotter = indicators[label];
	else
	{
		newPlotter = new Graphable<Point2>(this, lineColor);
		if (newPlotter)
			indicators[label] = newPlotter;
	}

	newPlotter->set(graphPoints);

	// trigger the draw update
	drawUpdate = true;
}

void RUCandleGraph::updateBackground(gfxpp* cGfx)
{
	if(!cGfx)
		return;

	RUGraph::updateBackground(cGfx);

	// draw the candles
	std::map<std::string, Graphable<Candle>*>::iterator it;
	for (it = candles.begin(); it != candles.end(); ++it)
	{
		Graphable<Candle>* g = it->second;
		if (g)
			g->updateBackground(cGfx);
	}

	// draw the indicators
	std::map<std::string, Graphable<Point2>*>::iterator it2;
	for (it2 = indicators.begin(); it2 != indicators.end(); ++it2)
	{
		Graphable<Point2>* g = it2->second;
		if (g)
			g->updateBackground(cGfx);
	}
}

void RUCandleGraph::update()
{
	// compute the candles
	std::map<std::string, Graphable<Candle>*>::iterator it;
	for (it = candles.begin(); it != candles.end(); ++it)
	{
		Graphable<Candle>* g = it->second;
		if (g)
			g->computeAxisRanges();
	}

	// compute the indicators
	std::map<std::string, Graphable<Point2>*>::iterator it2;
	for (it2 = indicators.begin(); it2 != indicators.end(); ++it2)
	{
		Graphable<Point2>* g = it2->second;
		if (g)
			g->computeAxisRanges();
	}
}

void RUCandleGraph::clear(bool toggleDraw)
{
	std::map<std::string, Graphable<Candle>*>::iterator it;
	for (it = candles.begin(); it != candles.end(); ++it)
		delete it->second;
	candles.clear();

	std::map<std::string, Graphable<Point2>*>::iterator it2;
	for (it2 = indicators.begin(); it2 != indicators.end(); ++it2)
		delete it2->second;
	indicators.clear();

	if (toggleDraw)
		drawUpdate = true;
}

std::string RUCandleGraph::getType() const
{
	return "RUCandleGraph";
}

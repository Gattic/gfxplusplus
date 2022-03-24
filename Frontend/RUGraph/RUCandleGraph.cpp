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

const shmea::GString RUCandleGraph::CANDLE_LABEL = "candles";

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
	cPlotter->setColor(lineColor);

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

void RUCandleGraph::addMLine(shmea::GString label, const double newPoint, SDL_Color lineColor)
{
	Graphable<SimpleLine>* newPlotter = new Graphable<SimpleLine>(this, lineColor);
	if (newPlotter)
		mLines[label] = newPlotter;

	SimpleLine* cPoint = new SimpleLine(newPoint);
	newPlotter->add(cPoint);

	// DON'T trigger the draw update here

}

void RUCandleGraph::resetMLines()
{
	mLines.clear();	
}

void RUCandleGraph::setIndicatorPeriods(const std::vector<unsigned int>& newIndPers)
{
	indicatorPeriods = newIndPers;
}

void RUCandleGraph::addIndicator(shmea::GString label, const Point2* newPoint, SDL_Color lineColor)
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
	//cPlotter->setLocalXMode(true);
	cPlotter->add(plotterPoint, false);

	// Set the indicator color
	unsigned int indCounter = 0;
	std::map<shmea::GString, Graphable<Point2>*>::iterator it2;
	for (it2 = indicators.begin(); it2 != indicators.end(); ++it2)
	{
		// calculate the hue
		double hue = ((double)indCounter) / ((double)indicators.size());
		hue = 1.0f - hue;

		// get the color
		int8_t redMask = 0xBF;
		int8_t greenMask = 0xBF;
		int8_t blueMask = 0xFF * hue;

		// set the color and draw the point
		SDL_Color cColor;
		cColor.r = redMask,
		cColor.g = greenMask;
		cColor.b = blueMask;
		cColor.a = SDL_ALPHA_OPAQUE;
		Graphable<Point2>* g = it2->second;
		if (g)
		{
			g->setColor(cColor);
			++indCounter;
		}
	}

	// DON'T trigger the draw update here
}

void RUCandleGraph::setIndicator(shmea::GString label, const std::vector<Point2*>& graphPoints, SDL_Color lineColor)
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

	//newPlotter->setLocalXMode(true);
	newPlotter->set(graphPoints);

	// Set the indicator color
	unsigned int indCounter = 0;
	std::map<shmea::GString, Graphable<Point2>*>::iterator it2;
	for (it2 = indicators.begin(); it2 != indicators.end(); ++it2)
	{
		// calculate the hue
		double hue = ((double)indCounter) / ((double)indicators.size());
		hue = 1.0f - hue;

		// get the color
		int8_t redMask = 0xBF;
		int8_t greenMask = 0xBF;
		int8_t blueMask = 0xFF * hue;

		// set the color and draw the point
		SDL_Color cColor;
		cColor.r = redMask,
		cColor.g = greenMask;
		cColor.b = blueMask;
		cColor.a = SDL_ALPHA_OPAQUE;
		Graphable<Point2>* g = it2->second;
		if (g)
		{
			g->setColor(cColor);
			++indCounter;
		}
	}

	// trigger the draw update
	drawUpdate = true;
}

void RUCandleGraph::addTrade(shmea::GString label, const ActionBubble* newPoint, SDL_Color lineColor)
{
	ActionBubble* plotterPoint = new ActionBubble(*newPoint);

	if (trades.find(label) == trades.end())
	{
		std::vector<ActionBubble*> newPointVec;
		newPointVec.push_back(plotterPoint);
		setTrade(label, newPointVec, lineColor);
		return;
	}

	Graphable<ActionBubble>* cPlotter = trades[label];
	//cPlotter->setLocalXMode(true);
	cPlotter->add(plotterPoint, false);

	// DON'T trigger the draw update here
}

void RUCandleGraph::setTrade(shmea::GString label, const std::vector<ActionBubble*>& graphPoints, SDL_Color lineColor)
{
	Graphable<ActionBubble>* newPlotter;
	if (trades.find(label) != trades.end())
		newPlotter = trades[label];
	else
	{
		newPlotter = new Graphable<ActionBubble>(this, lineColor);
		if (newPlotter)
			trades[label] = newPlotter;
	}

	//newPlotter->setLocalXMode(true);
	newPlotter->set(graphPoints);

	// trigger the draw update
	drawUpdate = true;
}

Graphable<Candle>* RUCandleGraph::getCandleGraphable()
{
	if (candles.find(CANDLE_LABEL) == candles.end())
		return NULL;

	return candles[CANDLE_LABEL];
}

unsigned int RUCandleGraph::getCandleGraphableSize() const
{
	std::map<shmea::GString, Graphable<Candle>*>::const_iterator itr = candles.find(CANDLE_LABEL);
	if (itr == candles.end())
		return 0;

	return itr->second->size();
}

unsigned int RUCandleGraph::getCandleGraphableNormalizedSize() const
{
	std::map<shmea::GString, Graphable<Candle>*>::const_iterator itr = candles.find(CANDLE_LABEL);
	if (itr == candles.end())
		return 0;

	return itr->second->normalizedSize();
}

void RUCandleGraph::updateBackground(gfxpp* cGfx)
{
	if(!cGfx)
		return;

	RUGraph::updateBackground(cGfx);

	// draw the candles
	std::map<shmea::GString, Graphable<Candle>*>::iterator it;
	for (it = candles.begin(); it != candles.end(); ++it)
	{
		Graphable<Candle>* g = it->second;
		if (g)
			g->updateBackground(cGfx);
	}

	// draw the indicators
	std::map<shmea::GString, Graphable<Point2>*>::iterator it2;
	for (it2 = indicators.begin(); it2 != indicators.end(); ++it2)
	{
		Graphable<Point2>* g = it2->second;
		if (g)
			g->updateBackground(cGfx);
	}
	
	// draw the marketmaker lines
	std::map<shmea::GString, Graphable<SimpleLine>*>::iterator it3;
	for (it3 = mLines.begin(); it3 != mLines.end(); ++it3)
	{
		Graphable<SimpleLine>* g = it3->second;
		if (g)
			g->updateBackground(cGfx);
	}

	// draw the trades
	std::map<shmea::GString, Graphable<ActionBubble>*>::iterator it4;
	for (it4 = trades.begin(); it4 != trades.end(); ++it4)
	{
		Graphable<ActionBubble>* g = it4->second;
		if (g)
			g->updateBackground(cGfx);
	}
}

void RUCandleGraph::update()
{
	// compute the candles
	std::map<shmea::GString, Graphable<Candle>*>::iterator it;
	for (it = candles.begin(); it != candles.end(); ++it)
	{
		Graphable<Candle>* g = it->second;
		if (g)
			g->computeAxisRanges();
	}

	// compute the indicators
	std::map<shmea::GString, Graphable<Point2>*>::iterator it2;
	for (it2 = indicators.begin(); it2 != indicators.end(); ++it2)
	{
		Graphable<Point2>* g = it2->second;
		if (g)
			g->computeAxisRanges();
	}

	// compute the marketmaker lines 
	std::map<shmea::GString, Graphable<SimpleLine>*>::iterator it3;
	for (it3 = mLines.begin(); it3 != mLines.end(); ++it3)
	{
		Graphable<SimpleLine>* g = it3->second;
		if (g)
			g->computeAxisRanges();
	}

	// compute the trades
	std::map<shmea::GString, Graphable<ActionBubble>*>::iterator it4;
	for (it4 = trades.begin(); it4 != trades.end(); ++it4)
	{
		Graphable<ActionBubble>* g = it4->second;
		if (g)
			g->computeAxisRanges();
	}
}

void RUCandleGraph::clear(bool toggleDraw)
{
	std::map<shmea::GString, Graphable<Candle>*>::iterator it;
	for (it = candles.begin(); it != candles.end(); ++it)
		delete it->second;
	candles.clear();

	std::map<shmea::GString, Graphable<Point2>*>::iterator it2;
	for (it2 = indicators.begin(); it2 != indicators.end(); ++it2)
		delete it2->second;
	indicators.clear();
	indicatorPeriods.clear();

	std::map<shmea::GString, Graphable<ActionBubble>*>::iterator it3;
	for (it3 = trades.begin(); it3 != trades.end(); ++it3)
		delete it3->second;
	trades.clear();

	std::map<shmea::GString, Graphable<SimpleLine>*>::iterator it4;
	for (it4 = mLines.begin(); it4 != mLines.end(); ++it4)
		delete it4->second;
	mLines.clear();

	xMin = FLT_MAX;
	xMax = FLT_MIN;
	yMin = FLT_MAX;
	yMax = FLT_MIN;

	if (toggleDraw)
		drawUpdate = true;
}

shmea::GString RUCandleGraph::getType() const
{
	return "RUCandleGraph";
}

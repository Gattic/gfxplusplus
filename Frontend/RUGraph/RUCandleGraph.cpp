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

RUCandleGraph::RUCandleGraph(int newWidth, int newHeight, int newQuadrants)
	: RUGraph(newWidth, newHeight, newQuadrants)
{
	//
}

RUCandleGraph::~RUCandleGraph()
{
	//
}

//Dont worry about this fnc
void RUCandleGraph::add(gfxpp* cGfx, std::string label, const Candle* newPoint,
				  SDL_Color lineColor)
{
	if(!cGfx)
		return;

	Candle* plotterPoint = new Candle(newPoint->getOpen(), newPoint->getClose(),
		newPoint->getHigh(), newPoint->getLow());

	//if (candles.find(label) == candles.end())
		//return;
	if (candles.find(label) == candles.end())
	{
		std::vector<Candle*> newPointVec;
		newPointVec.push_back(plotterPoint);
		set(cGfx, label, newPointVec, lineColor);
		return;
	}

	Graphable<Candle>* cPlotter = candles[label];
	cPlotter->add(cGfx, plotterPoint);

	// trigger the draw update
	drawUpdate = true;
}

void RUCandleGraph::set(gfxpp* cGfx, const std::string& label, const std::vector<Candle*>& graphPoints,
				  SDL_Color lineColor)
{
	if(!cGfx)
		return;

	Graphable<Candle>* newPlotter;
	if (candles.find(label) != candles.end())
		newPlotter = candles[label];
	else
	{
		newPlotter = new Graphable<Candle>(this, lineColor);

		if (newPlotter)
			candles[label] = newPlotter;
	}

	newPlotter->set(cGfx, graphPoints);

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
}

void RUCandleGraph::clear(bool toggleDraw)
{
	std::map<std::string, Graphable<Candle>*>::iterator it;

	for (it = candles.begin(); it != candles.end(); ++it)
		delete it->second;
	candles.clear();

	if (toggleDraw)
		drawUpdate = true;
}

std::string RUCandleGraph::getType() const
{
	return "RUCandleGraph";
}

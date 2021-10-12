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
#ifndef _RUCANDLEGRAPH
#define _RUCNADLEGRAPH

#include "RUGraph.h"
#include "../GItems/RUColors.h"
#include "../GFXUtilities/Candle.h"
#include "Graphable.h"
#include <SDL2/SDL.h>
#include <map>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

class gfxpp;
//class Graphable<Point2>;
class Candle;

namespace shmea {
class GList;
}; // namespace shmea

class RUCandleGraph : public RUGraph
{
private:

	std::map<std::string, Graphable<Candle>*> candles;
	std::map<std::string, Graphable<Point2>*> indicators;

protected:
	// render
	virtual void updateBackground(gfxpp*);

public:

	const static std::string CANDLE_LABEL;

	// constructors & destructor
	RUCandleGraph(int, int, int = QUADRANTS_ONE);
	virtual ~RUCandleGraph();

	virtual std::string getType() const;

	//
	virtual void add(gfxpp*, const Candle*,
			 SDL_Color = RUColors::DEFAULT_COLOR_LINE);
	virtual void set(gfxpp*, const std::vector<Candle*>&,
			 SDL_Color = RUColors::DEFAULT_COLOR_LINE);

	//
	virtual void addIndicator(gfxpp*, std::string, const Point2*,
			 SDL_Color = RUColors::DEFAULT_COLOR_LINE);
	virtual void setIndicator(gfxpp*, std::string, const std::vector<Point2*>&,
			 SDL_Color = RUColors::DEFAULT_COLOR_LINE);

	void update(gfxpp*);
	void clear(bool = false);
};

#endif

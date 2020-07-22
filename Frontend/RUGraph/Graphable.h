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
#ifndef _GRAPHABLE_H
#define _GRAPHABLE_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include "../GFXUtilities/point2.h"
#include "../GFXUtilities/Candle.h"
#include "../Graphics/graphics.h"
#include "Backend/Database/GList.h"
#include "RUGraph.h"

template <class T>
class Graphable
{
private:
	RUGraph* parent;
	std::vector<T*> points;
	size_t prevSize;
	float xMin;
	float xMax;
	float yMin;
	float yMax;

	const static int OPT_NOTHING = 0;
	const static int OPT_FILL = 1;
	const static int OPT_FULL = 2;

	SDL_Texture* rawGraph;
	SDL_Color lineColor;

	// Our canvas we edit
	int drawType;
	int drawWidth;
	int drawHeight;

	int canvasWidth;
	int canvasHeight;

	void computeAxisRanges(gfxpp*, bool = false);

public:

	const static int TEXTURE_MAX_DIM = 16384;

	// constructors & destructor
	Graphable(RUGraph*, SDL_Color);
	virtual ~Graphable();

	// gets
	SDL_Color getColor() const;

	// sets
	void setColor(SDL_Color);
	void add(gfxpp*, const T*);
	void set(gfxpp*, const std::vector<T*>&);
	virtual void clear();

	// render
	virtual void updateBackground(gfxpp*);
	virtual void draw(gfxpp*);
};

template <class T>
Graphable<T>::Graphable(RUGraph* newParent, SDL_Color newColor)
{
	rawGraph = NULL;
	parent = newParent;
	setColor(newColor);
	drawType = OPT_NOTHING;
	canvasWidth = 0;
	canvasHeight = 0;
	drawWidth = 0;
	drawHeight = 0;

	prevSize = 0;
	xMin = 0.0f;
	xMax = 0.0f;
	yMin = 0.0f;
	yMax = 0.0f;
}

template <class T>
Graphable<T>::~Graphable()
{
	rawGraph = NULL;
	drawType = OPT_NOTHING;
	canvasWidth = 0;
	canvasHeight = 0;
	drawWidth = 0;
	drawHeight = 0;

	clear();
}

template <class T>
SDL_Color Graphable<T>::getColor() const
{
	return lineColor;
}

template <class T>
void Graphable<T>::set(gfxpp* cGfx, const std::vector<T*>& newPoints)
{
	if (newPoints.empty())
		return;

	points = newPoints;
	computeAxisRanges(cGfx);
}

template <class T>
void Graphable<T>::setColor(SDL_Color newColor)
{
	lineColor = newColor;
}

/*
 * @brief draw point
 * @details draws a filled point on the renderer at (x,y) with radius r using the Midpoint circle
 * algorithm. Scatter Helper fnc
 * @param cGfx the central graphics object
 * @param cx the desired central x-coordinate
 * @param cy the desired central y-coordinate
 * @param r the desired circle radius
 */
/*template <class T>
void Graphable<T>::drawPoint(gfxpp* cGfx, int cx, int cy, int r)//Turn these 3 points into a class?
{
	if (r == 0)
		r = pointSize / 2;
	for (int i = 1; i <= r; ++i)
		drawPointOutline(cGfx, cx, cy, i);
}*/

template <class T>
void Graphable<T>::clear()
{
	for (unsigned int i = 0; i < points.size(); ++i)
	{
		T* cPoint = points[i];
		if (!cPoint)
			continue;
		delete cPoint;
		points.erase(points.begin() + i);
	}

	points.clear();

	parent = NULL;
	xMin = 0.0f;
	xMax = 0.0f;
	yMin = 0.0f;
	yMax = 0.0f;
}

template <class T>
void Graphable<T>::updateBackground(gfxpp* cGfx)
{
	if(!cGfx)
		return;

	//if(!rawGraph)
	//	return;

	// Set the render target to draw the cached raw draw space
	//SDL_SetRenderTarget(cGfx->getRenderer(), rawGraph);
	//draw(cGfx);

	// draw the line
	SDL_SetRenderTarget(cGfx->getRenderer(), parent->getBackground());
	draw(cGfx);
}

#endif

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
#include <limits>
#include <cmath>
#include "../GFXUtilities/GraphablePoint.h"
#include "../GItems/RUColors.h"
#include "../Graphics/graphics.h"
#include "Backend/Database/standardizable.h"

class RUGraph;

template <class T>
class Graphable : public shmea::GStandardizable
{
private:
	RUGraph* parent;
	std::vector<T*> points;
	bool visible;
	SDL_Color lineColor;

public:

	std::vector<T*> normalizedPoints;

	const static int TEXTURE_MAX_DIM = 16384;

	// constructors & destructor
	Graphable();
	Graphable(RUGraph*);
	virtual ~Graphable();

	unsigned int size() const;
	unsigned int normalizedSize() const;
	bool isVisible() const;
	SDL_Color getColor() const;
	void setVisible(bool);
	void setColor(SDL_Color);
	void add(const T*, bool = true);
	void set(const std::vector<T*>&);
	virtual void clear();
	void updateBackground(gfxpp*);

	// render
	virtual void draw(gfxpp*);
	virtual void computeAxisRanges(bool = false);
};

template <class T>
Graphable<T>::Graphable(RUGraph* newParent)
{
	parent = newParent;
	visible = true;
}

template <class T>
Graphable<T>::Graphable()
{
	//
	parent = NULL;
}

template <class T>
Graphable<T>::~Graphable()
{
	clear();
}

template <class T>
unsigned int Graphable<T>::size() const
{
	return points.size();
}

template <class T>
unsigned int Graphable<T>::normalizedSize() const
{
	return normalizedPoints.size();
}

template <class T>
bool Graphable<T>::isVisible() const
{
	return visible;
}

template <class T>
SDL_Color Graphable<T>::getColor() const
{
	return lineColor;
}

template <class T>
void Graphable<T>::setVisible(bool newVisible)
{
	visible = newVisible;
}

template <class T>
void Graphable<T>::setColor(SDL_Color newColor)
{
	lineColor = newColor;
}

template <class T>
void Graphable<T>::set(const std::vector<T*>& newPoints)
{
	if (newPoints.empty())
		return;

	points = newPoints;
	computeAxisRanges();//sets redoRange automatically
}

// This function is recommended for TimeSeries optimizations.
template <class T>
void Graphable<T>::add(const T* newPoint, bool recompute)
{
	if (!newPoint)
		return;

	points.push_back(new T(*newPoint));
	if(recompute)
		computeAxisRanges(true);
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

	parent = NULL;
	points.clear();
	normalizedPoints.clear();
	visible = false;
	lineColor = RUColors::DEFAULT_COLOR_LINE;
}

/*template <class T>
void Graphable<T>::computeAxisRanges(bool additionOptimization)
{
	if (!parent)
		return;

	if (points.empty())
		return;

	redoRange = !additionOptimization;
	if(additionOptimization)
	{
		// Is the latest y not within the current range?
		float yMax = points[points.size()-1]->getYMax();
		float yMin = points[points.size()-1]->getYMin();
		if ((yMin < getYMin()) || (yMax > getYMax()))
			redoRange = true;
	}

	//redoRange = true;
	unsigned int i = 0;
	//if(redoRange)
		//i = points.size()-1;
	for (; i < points.size(); ++i)
	{
		T* c = points[points.size()-1];
		float x_pt = c->getX();
		float y_max = c->getYMax();
		float y_min = c->getYMin();

		setXMax(x_pt);
		setXMin(x_pt);
		setYMax(y_max);
		setYMin(y_min);
	}

	// Set the parents
	parent->setXMin(getXMin());
	parent->setXMax(getXMax());
	parent->setYMin(getYMin());
	parent->setYMax(getYMax());

	//printf("T-PRE[%s]: %f:%f\n", parent->getName().c_str(), getXMax(), getXMin());

	//==============================================Normalize the points==============================================

	unsigned int agg = parent->getAggregate();
	float xRange = (float)points.size() / (float)agg;
	float yRange = getYMax() - getYMin();
	if(points.size() % agg)
		++xRange;

	// Scales coordinates based on graph size and data range.
	float pointXGap = ((float)parent->getWidth()) / xRange;
	float pointYGap = ((float)parent->getHeight()) / yRange;
	if(isinf(pointXGap))
	{
		normalizedPoints.clear();
		return;
	}

	// Size up the normalized points vec
	while(normalizedPoints.size() < xRange)
	{
		T* newCandle = new T();
		normalizedPoints.push_back(newCandle);
	}

	while(normalizedPoints.size() > xRange)
	{
		normalizedPoints.erase(normalizedPoints.begin()+normalizedPoints.size()-1);
	}

	//Get a head start?
	i = 0;
	//if(!redoRange)
		//i = points.size()-1;

	// Aggregate helpers
	unsigned int aggCounter = 0;
	float aggOpenValue = 0.0f;
	float aggCloseValue = 0.0f;
	float aggHighValue = 0.0f;
	float aggLowValue = 0.0f;

	unsigned int normalCounter = 0;
	for (; i < points.size(); ++i)
	{
		// First point would be drawn at x = 0, so we need to add (pointXGap / 2)
		// so that the bar can be drawn left and right.
		float newOpenValue = (points[i]->getOpen() - getYMin()) * pointYGap;
		float newCloseValue = (points[i]->getClose() - getYMin()) * pointYGap;
		float newHighValue = (points[i]->getHigh() - getYMin()) * pointYGap;
		float newLowValue = (points[i]->getLow() - getYMin()) * pointYGap;

		// Time to Aggreagate
		++aggCounter;
		aggCloseValue=newCloseValue;
		if(aggCounter == 1)
		{
			aggOpenValue=newOpenValue;
			aggHighValue=newHighValue;
			aggLowValue=newLowValue;
		}

		// Set high and low of aggregate
		if(newHighValue > aggHighValue)
			aggHighValue=newHighValue;
		if(newLowValue < aggLowValue)
			aggLowValue=newLowValue;

		// Are we done aggregating data yet?
		if((aggCounter < agg) && (i < points.size()-1))
			continue;

		// Our aggregated T
		//float newXValue = ((i/agg) * pointXGap);
		//float newXValue = i * pointXGap + (pointXGap / 2);
		float newXValue = ((i/agg) * pointXGap)+ (pointXGap / 2);
		normalizedPoints[normalCounter]->setX(parent->getAxisOriginX() + newXValue);
		normalizedPoints[normalCounter]->setOpen(parent->getAxisOriginY() + (float)parent->getHeight() - aggOpenValue);
		normalizedPoints[normalCounter]->setClose(parent->getAxisOriginY() + (float)parent->getHeight() - aggCloseValue);
		normalizedPoints[normalCounter]->setHigh(parent->getAxisOriginY() + (float)parent->getHeight() - aggHighValue);
		normalizedPoints[normalCounter]->setLow(parent->getAxisOriginY() + (float)parent->getHeight() - aggLowValue);

		// The draw container
		++normalCounter;

		// Reset the agg helpers
		aggCounter = 0;
		aggOpenValue = 0.0f;
		aggCloseValue = 0.0f;
		aggHighValue = 0.0f;
		aggLowValue = 0.0f;
	}

	parent->requireDrawUpdate();
}*/

template <class T>
void Graphable<T>::updateBackground(gfxpp* cGfx)
{
	if(!cGfx)
		return;

	// draw the line
	draw(cGfx);
	//parent->requireDrawUpdate();

	redoRange = false;
}

template<typename T>
void Graphable<T>::computeAxisRanges(bool additionOptimization)
{
    if (!parent)
        return;

    if (points.empty())
        return;

    redoRange = !additionOptimization;
    if (additionOptimization)
    {
        // Check if the latest y is not within the current range
        float yMax = points.back()->getMaxY();
        float yMin = points.back()->getMinY();
        if ((yMin < getYMin()) || (yMax > getYMax()))
            redoRange = true;
    }

    unsigned int i = 0;
    if (redoRange)
    {
        for (; i < points.size(); ++i)
        {
            T* pt = points[i];
            float x_pt = pt->getX();
            float y_max = pt->getMaxY();
            float y_min = pt->getMinY();

            setXMax(x_pt);
            setXMin(x_pt);
            setYMax(y_max);
            setYMin(y_min);
        }
    }

    // Set the parent's range
    parent->setXMin(getXMin());
    parent->setXMax(getXMax());
    parent->setYMin(getYMin());
    parent->setYMax(getYMax());

	//==============================================Normalize the points==============================================

    unsigned int agg = parent->getAggregate();
    float xRange = static_cast<float>(points.size()) / static_cast<float>(agg);
    float yRange = getYMax() - getYMin();
    if (points.size() % agg)
        ++xRange;

    float pointXGap = static_cast<float>(parent->getWidth()) / xRange;
    float pointYGap = static_cast<float>(parent->getHeight()) / yRange;
    if (isinf(pointXGap))
    {
        normalizedPoints.clear();
        return;
    }

    // Size up the normalized points vector
    while (normalizedPoints.size() < xRange)
    {
        normalizedPoints.push_back(new T());
    }

    while (normalizedPoints.size() > xRange)
    {
        normalizedPoints.pop_back();
	//normalizedPoints.erase(normalizedPoints.begin()+normalizedPoints.size()-1);
    }

    i = 0;
    unsigned int aggCounter = 0;
    float aggValue = 0.0f;
    unsigned int normalCounter = 0;

    for (; i < points.size(); ++i)
    {
        float newYValue = (points[i]->getMaxY() - getYMin()) * pointYGap;

        ++aggCounter;
        aggValue = newYValue;

        if ((aggCounter < agg) && (i < points.size() - 1))
        {
            continue;
        }

        //float newXValue = normalCounter * pointXGap;
	float newXValue = ((i/agg) * pointXGap)+ (pointXGap / 2);
        normalizedPoints[normalCounter]->setX(parent->getAxisOriginX() + newXValue);
        normalizedPoints[normalCounter]->setY(parent->getAxisOriginY() + static_cast<float>(parent->getHeight()) - aggValue);

        ++normalCounter;
        aggCounter = 0;
        aggValue = 0.0f;
    }

    parent->requireDrawUpdate();
}

#endif

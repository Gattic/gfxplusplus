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
#include "GraphableAttr.h"
#include "RUGraph.h"

GraphableAttr::GraphableAttr()
{
	parent = NULL;
	setColor(RUColors::DEFAULT_COLOR_LINE);
	localXMode = false;
	localYMode = false;
	localXMin = FLT_MAX;
	localXMax = FLT_MIN;
	localYMin = FLT_MAX;
	localYMax = FLT_MIN;
	redoRange = true;
}

GraphableAttr::GraphableAttr(RUGraph* newParent, SDL_Color newColor)
{
	parent = newParent;
	setColor(newColor);
	localXMode = false;
	localYMode = false;
	localXMin = FLT_MAX;
	localXMax = FLT_MIN;
	localYMin = FLT_MAX;
	localYMax = FLT_MIN;
	redoRange = true;
}

GraphableAttr::~GraphableAttr()
{
	clear();
}

float GraphableAttr::getXMin() const
{
	if(!parent)
		return 0.0f;

	if(localXMode)
		return localXMin;

	return parent->getXMin();
}

float GraphableAttr::getXMax() const
{
	if(!parent)
		return 0.0f;

	if(localXMode)
		return localXMax;

	return parent->getXMax();
}

float GraphableAttr::getYMin() const
{
	if(!parent)
		return 0.0f;

	if(localYMode)
		return localYMin;

	return parent->getYMin();
}

float GraphableAttr::getYMax() const
{
	if(!parent)
		return 0.0f;

	if(localYMode)
		return localYMax;

	return parent->getYMax();
}

bool GraphableAttr::getLocalXMode() const
{
	return localXMode;
}

float GraphableAttr::getLocalXMin() const
{
	return localXMin;
}

float GraphableAttr::getLocalXMax() const
{
	return localXMax;
}

bool GraphableAttr::getLocalYMode() const
{
	return localYMode;
}

float GraphableAttr::getLocalYMin() const
{
	return localYMin;
}

float GraphableAttr::getLocalYMax() const
{
	return localYMax;
}

SDL_Color GraphableAttr::getColor() const
{
	return lineColor;
}

void GraphableAttr::setLocalXMode(bool newLocalXMode)
{
	localXMode = newLocalXMode;
}

void GraphableAttr::setLocalXMin(float newXMin)
{
	localXMin = newXMin;
}

void GraphableAttr::setLocalXMax(float newXMax)
{
	localXMax = newXMax;
}

void GraphableAttr::setLocalYMode(bool newLocalYMode)
{
	localYMode = newLocalYMode;
}

void GraphableAttr::setLocalYMin(float newYMin)
{
	localYMin = newYMin;
}

void GraphableAttr::setLocalYMax(float newYMax)
{
	localYMax = newYMax;
}

void GraphableAttr::setColor(SDL_Color newColor)
{
	lineColor = newColor;
}

void GraphableAttr::clear()
{
	parent = NULL;
	localXMode = false;
	localYMode = false;
	localXMin = FLT_MAX;
	localXMax = FLT_MIN;
	localYMin = FLT_MAX;
	localYMax = FLT_MIN;
	redoRange = true;
}

void GraphableAttr::updateBackground(gfxpp* cGfx)
{
	if(!cGfx)
		return;

	// draw the line
	draw(cGfx);
	//parent->requireDrawUpdate();

	redoRange = false;
}

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
	xMode = false; // is local
	yMode = false; // is local
}

GraphableAttr::GraphableAttr(RUGraph* newParent)
{
	parent = newParent;
	xMode = false; // is local
	yMode = false; // is local
}

GraphableAttr::~GraphableAttr()
{
	clear();
}

float GraphableAttr::getXMinModed() const
{
	if(!parent)
		return 0.0f;

	if(xMode)
		return getXMin();

	return parent->getXMin();
}

float GraphableAttr::getXMaxModed() const
{
	if(!parent)
		return 0.0f;

	if(xMode)
		return getXMax();

	return parent->getXMax();
}

float GraphableAttr::getYMinModed() const
{
	if(!parent)
		return 0.0f;

	if(yMode)
		return getYMin();

	return parent->getYMin();
}

float GraphableAttr::getYMaxModed() const
{
	if(!parent)
		return 0.0f;

	if(yMode)
		return getYMax();

	return parent->getYMax();
}

bool GraphableAttr::getXMode() const
{
	return xMode;
}

bool GraphableAttr::getYMode() const
{
	return yMode;
}

void GraphableAttr::setXMode(bool newLocalXMode)
{
	xMode = newLocalXMode;
}

void GraphableAttr::setYMode(bool newLocalYMode)
{
	yMode = newLocalYMode;
}

void GraphableAttr::clear()
{
	parent = NULL;
	xMode = false;
	yMode = false;
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

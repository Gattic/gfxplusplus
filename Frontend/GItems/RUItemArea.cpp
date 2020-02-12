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
#include "RUItemArea.h"

RUItemArea::RUItemArea()
{
	x = 0;
	y = 0;
	width = 0;
	height = 0;
	paddingX = 6; // pixels
	paddingY = 6; // pixels
	marginX = 0;  // pixels
	marginY = 0;  // pixels
	visible = false;
	drawUpdate = false;
}

RUItemArea::RUItemArea(int newX, int newY, int newWidth, int newHeight)
{
	x = newX;
	y = newY;
	width = newWidth;
	height = newHeight;
	paddingX = 6; // pixels
	paddingY = 6; // pixels
	marginX = 0;  // pixels
	marginY = 0;  // pixels
	visible = false;
	drawUpdate = false;
}

RUItemArea::~RUItemArea()
{
	//
}

int RUItemArea::getX() const
{
	return x;
}

int RUItemArea::getY() const
{
	return y;
}

int RUItemArea::getWidth() const
{
	return width;
}

int RUItemArea::getHeight() const
{
	return height;
}

int RUItemArea::getPaddingX() const
{
	return paddingX;
}

int RUItemArea::getPaddingY() const
{
	return paddingY;
}

int RUItemArea::getMarginX() const
{
	return marginX;
}

int RUItemArea::getMarginY() const
{
	return marginY;
}

SDL_Rect RUItemArea::getLocationRect() const
{
	SDL_Rect location;
	location.x = x;
	location.y = y;
	location.w = width;
	location.h = height;
	return location;
}

bool RUItemArea::isVisible() const
{
	return visible;
}

bool RUItemArea::getDrawUpdateRequired() const
{
	return drawUpdate;
}

void RUItemArea::setX(int newX)
{
	x = newX;
}

void RUItemArea::setY(int newY)
{
	y = newY;
}

void RUItemArea::setWidth(int newWidth)
{
	width = newWidth;
}

void RUItemArea::setHeight(int newHeight)
{
	height = newHeight;
}

void RUItemArea::setPadding(int newPadding)
{
	paddingX = newPadding;
	paddingY = newPadding;
}

void RUItemArea::setPaddingX(int newPadding)
{
	paddingX = newPadding;
}

void RUItemArea::setPaddingY(int newPadding)
{
	paddingY = newPadding;
}

void RUItemArea::setMargin(int newMargin)
{
	marginX = newMargin;
	marginY = newMargin;
}

void RUItemArea::setMarginX(int newMargin)
{
	marginX = newMargin;
}

void RUItemArea::setMarginY(int newMargin)
{
	marginY = newMargin;
}

void RUItemArea::setVisible(bool newVisibility)
{
	visible = newVisibility;
}

void RUItemArea::requireDrawUpdate()
{
	drawUpdate = true;
}

bool RUItemArea::inRange(int eventX, int eventY) const
{
	if (!((getX() >= 0) && (getY() >= 0)))
		return false;

	if (!((width > 0) && (height > 0)))
		return false;

	// lower bound
	if (!((eventX >= getX()) && (eventY >= getY())))
		return false;

	// upper bound
	if (!((eventX < getX() + width) && (eventY < getY() + height)))
		return false;

	return true;
}

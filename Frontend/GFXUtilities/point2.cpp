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
#include "point2.h"

Point2::Point2()
{
	setX(0.0f);
	setY(0.0f);
}

Point2::Point2(double newX, double newY)
{
	setX(newX);
	setY(newY);
}

Point2::Point2(const Point2& p)
{
	setX(p.x);
	setY(p.y);
}

Point2::~Point2()
{
	setX(0.0f);
	setY(0.0f);
}

double Point2::getX() const
{
	return x;
}

double Point2::getY() const
{
	return y;
}

double Point2::length() const
{
	return sqrt(x * x + y * y);
}

void Point2::set(double newX, double newY)
{
	setX(newX);
	setY(newY);
}

void Point2::setX(double newX)
{
	x = newX;
}

void Point2::setY(double newY)
{
	y = newY;
}

void Point2::normalize()
{
	double len = length();
	if (len == 0.0f)
		return;

	x /= len;
	y /= len;
}

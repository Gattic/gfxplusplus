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
#include "Circle.h"
#include "point2.h"

Circle::Circle()
{
	radius = 0.0f;
	center=new Point2(0,0);
}

Circle::~Circle()
{
	radius = 0.0f;
	center=NULL;
}

void Circle::setCenter(const Point2* newCenter)
{
	if (!newCenter)
		return;

	center = new Point2(newCenter->getX(), newCenter->getY());
}

void Circle::setRadius(double newRadius)
{
	radius = newRadius;
}

const Point2* Circle::getCenter() const
{
	return center;
}

double Circle::getRadius() const
{
	return radius;
}

shmea::GPointer<const shmea::GList> Circle::toXVectorData() const
{
	shmea::GList* xData = new shmea::GList();
	xData->addFloat(getCenter()->getX());
	return shmea::GPointer<const shmea::GList>(xData);
}

shmea::GPointer<const shmea::GList> Circle::toYVectorData() const
{
	shmea::GList* yData = new shmea::GList();
	yData->addFloat(getCenter()->getY());
	return shmea::GPointer<const shmea::GList>(yData);
}

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
#include "Horizontal_Line.h"

Horizontal_Line::Horizontal_Line()
{
	set(0.0f);
}

Horizontal_Line::Horizontal_Line(double newY)
{
	set(newY);
}

Horizontal_Line::Horizontal_Line(const Horizontal_Line& p)
{
	set(p.y);
}

Horizontal_Line::~Horizontal_Line()
{
	set(0.0f);
}

double Horizontal_Line::getY() const
{
	return y;
}

void Horizontal_Line::set(double newY)
{
	setY(newY);
}

void Horizontal_Line::setY(double newY)
{
	y = newY;
}

shmea::GPointer<const shmea::GList> Horizontal_Line::toXVectorData() const
{
	shmea::GList* xData = new shmea::GList();
	// No data
	return shmea::GPointer<const shmea::GList>(xData);
}

shmea::GPointer<const shmea::GList> Horizontal_Line::toYVectorData() const
{
	shmea::GList* yData = new shmea::GList();
	yData->addFloat(getY());
	return shmea::GPointer<const shmea::GList>(yData);
}

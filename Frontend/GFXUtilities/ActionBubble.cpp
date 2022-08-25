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
#include "ActionBubble.h"
#include "point2.h"
//#include "../Graphics/graphics.h"

ActionBubble::ActionBubble()
{
	radius = AB_DEFAULT_RADIUS;
	focalPoint = NULL;
	cost = 0.0f;
	quantity = 0;
	actionType = -1;
}

ActionBubble::ActionBubble(int graphXVal, float newCost, int newQuantity, int newActionType)
{
	radius = AB_DEFAULT_RADIUS;
	cost = newCost;
	quantity = newQuantity;
	actionType = newActionType;
	focalPoint = new Point2(graphXVal, newCost);
}

ActionBubble::~ActionBubble()
{
	radius = AB_DEFAULT_RADIUS;
	focalPoint = NULL;
	cost = 0.0f;
	quantity = 0;
	actionType = -1;
}

void ActionBubble::setFocalPoint(const Point2* newFocalPoint)
{
	if (!newFocalPoint)
		return;

	focalPoint = new Point2(floor(newFocalPoint->getX()), floor(newFocalPoint->getY()));
}

void ActionBubble::setRadius(double newRadius)
{
	radius = newRadius;
}

void ActionBubble::setCost(float newCost)
{
	cost = newCost;
}

void ActionBubble::setQuantity(int newQuantity)
{
	quantity = newQuantity;
}

void ActionBubble::setActionType(int newActionType)
{
	actionType = newActionType;
}

const Point2* ActionBubble::getFocalPoint() const
{
	return focalPoint;
}

double ActionBubble::getRadius() const
{
	return radius;
}

float ActionBubble::getCost() const
{
	return cost;
}

int ActionBubble::getQuantity() const
{
	return quantity;
}

int ActionBubble::getActionType() const
{
	return actionType;
}

shmea::GPointer<const shmea::GList> ActionBubble::toXVectorData() const
{
	shmea::GList* xData = new shmea::GList();
	// No data
	return shmea::GPointer<const shmea::GList>(xData);
}

shmea::GPointer<const shmea::GList> ActionBubble::toYVectorData() const
{
	shmea::GList* yData = new shmea::GList();
	// No data
	return shmea::GPointer<const shmea::GList>(yData);
}

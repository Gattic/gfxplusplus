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
#include "RUDrawComponent.h"
#include "../../include/Backend/Database/gtype.h"
#include "../GFXUtilities/point2.h"
#include "Circle.h"
#include "GraphLine.h"
#include "GraphScatter.h"
#include "Graphable.h"
#include "RUGraph.h"

RUDrawComponent::RUDrawComponent(int newWidth, int newHeight, int newQuadrants)
	: RUGraph(newWidth, newHeight, newQuadrants)
{
	penWidth = 8;
	clickMode = MODE_CIRCLES;
	prevCircle = NULL;

	setBGColor(RUColors::COLOR_WHITE);
	buildDotMatrix();
}

RUDrawComponent::~RUDrawComponent()
{
	penWidth = 8;
	clickMode = MODE_CIRCLES;

	clear();

	// plotter mutex
	pthread_mutex_destroy(plotMutex);
	free(plotMutex);
}

int RUDrawComponent::getMode() const
{
	return clickMode;
}

float RUDrawComponent::getPenWidth() const
{
	return penWidth;
}

void RUDrawComponent::toggleMode()
{
	clickMode = !clickMode;
}

void RUDrawComponent::setPenWidth(float newPenWidth)
{
	penWidth = newPenWidth;
}

void RUDrawComponent::updateBackground(SDL_Renderer* renderer)
{
	// draw the circles
	pthread_mutex_lock(plotMutex);
	for (unsigned int i = 0; i < circles.size(); ++i)
	{
		Graphable* g = circles[i];
		if (g)
			g->updateBackground(renderer);
	}
	pthread_mutex_unlock(plotMutex);
}

void RUDrawComponent::clear(bool toggleDraw)
{
	pthread_mutex_lock(plotMutex);
	std::map<std::string, Graphable*>::iterator it;

	for (it = lines.begin(); it != lines.end(); ++it)
		delete it->second;
	lines.clear();
	pthread_mutex_unlock(plotMutex);

	// clearCircles();
	circles.clear();

	if (toggleDraw)
		drawUpdate = true;
}

void RUDrawComponent::addCircle(const Point2* focalPoint, double radius)
{
	// printf("Circle(%f, %f, %f)\n", focalPoint->getX(), focalPoint->getY(), radius);

	std::string newCircleName = "Circle: " + shmea::GType::longTOstring(circles.size());
	SDL_Color newCircleColor = {0x00, 0x00, 0xFF, 0xFF};
	Circle* newCircle = new Circle(this, newCircleColor);
	newCircle->addFocalPoint(focalPoint);
	newCircle->setRadius(radius);
	prevCircle = newCircle;

	pthread_mutex_lock(plotMutex);
	lines.insert(std::pair<std::string, Graphable*>(newCircleName, newCircle));
	circles.push_back(newCircle);
	pthread_mutex_unlock(plotMutex);

	drawUpdate = true;
}

void RUDrawComponent::onMouseDown(GPanel* cPanel, int eventX, int eventY)
{
	if (clickMode == MODE_CIRCLES)
	{
		// Focal Point
		Point2* cFocalPoint = new Point2(eventX - getX(), eventY - getY());
		addCircle(cFocalPoint, getPenWidth());
	}
	else if (clickMode == MODE_NELLIPSE)
	{
		// Focal Point
		Point2* cFocalPoint = new Point2(eventX - getX(), eventY - getY());

		// Switch the type of point we are waiting for
		if (circles.size() == 0)
			addCircle(cFocalPoint, getPenWidth());
		else
		{
			if (prevCircle)
				prevCircle->addFocalPoint(cFocalPoint);

			drawUpdate = true;
		}
	}
}

std::string RUDrawComponent::getType() const
{
	return "RUDrawComponent";
}

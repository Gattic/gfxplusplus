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
	radius = 0.0f;
	maxHit = 0;
}

ActionBubble::~ActionBubble()
{
	radius = 0.0f;
	maxHit = 0;
}

void ActionBubble::addFocalPoint(const Point2* newFocalPoint)
{
	if (!newFocalPoint)
		return;

	const Point2* newFocalPointCopy =
		new Point2(floor(newFocalPoint->getX()), floor(newFocalPoint->getY()));
	foci.push_back(newFocalPointCopy);
	createHeatmap();
}

void ActionBubble::setRadius(double newRadius)
{
	radius = newRadius;
	createHeatmap();
}

void ActionBubble::createHeatmap()
{
	heatmap.clear();

	// ActionBubble
	if (foci.size() == 1)
	{
		for (unsigned int focalIndex = 0; focalIndex < foci.size(); ++focalIndex)
		{
			const Point2* cFocalPoint = foci[focalIndex];
			for (int i = -radius; i < radius; ++i)
			{
				int xIndex = cFocalPoint->getX() + i;

				std::map<int, int> newMap;
				for (int j = -radius; j < radius; ++j)
				{
					int yIndex = cFocalPoint->getY() + j;

					// calculate the distance
					double distance = sqrt(pow(((double)i), 2.0f) + pow(((double)j), 2.0f));
					double hue =
						distance / sqrt(pow(((double)radius), 2.0f) + pow(((double)radius), 2.0f));
					if (distance > radius)
						continue;

					// printf("i(%d,%d:%ld)\n", focalIndex, xIndex, heatmap.size());
					if (heatmap.find(xIndex) == heatmap.end())
						heatmap.insert(std::pair<int, std::map<int, int> >(xIndex, newMap));

					// printf("j(%d:%d:%ld)\n", focalIndex, yIndex, heatmap[xIndex].size());
					if (heatmap[xIndex].find(yIndex) == heatmap[xIndex].end())
						heatmap[xIndex].insert(std::pair<int, int>(yIndex, 0));

					// tick
					++heatmap[xIndex][yIndex];

					if (heatmap[xIndex][yIndex] > maxHit)
						maxHit = heatmap[xIndex][yIndex];
				}
			}
		}
	}
}

const Point2* ActionBubble::getFocalPoint(unsigned int index) const
{
	if (index >= foci.size())
		return NULL;

	return foci[index];
}

double ActionBubble::getRadius() const
{
	return radius;
}

int ActionBubble::getMaxHit() const
{
	return maxHit;
}

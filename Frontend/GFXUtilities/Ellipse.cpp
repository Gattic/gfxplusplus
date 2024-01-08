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
#include "Ellipse.h"
#include "point2.h"
//#include "../Graphics/graphics.h"

Ellipse::Ellipse()
{
	radius = 0.0f;
	maxHit = 0;
}

Ellipse::~Ellipse()
{
	radius = 0.0f;
	maxHit = 0;
}

void Ellipse::addFocalPoint(const Point2* newFocalPoint)
{
	if (!newFocalPoint)
		return;

	const Point2* newFocalPointCopy =
		new Point2(floor(newFocalPoint->getX()), floor(newFocalPoint->getY()));
	foci.push_back(newFocalPointCopy);
	updateHeatmap();
}

void Ellipse::setRadius(double newRadius)
{
	radius = newRadius;
	createHeatmap();
}

void Ellipse::createHeatmap()
{
	heatmap.clear();

	// Circle
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
	// Ellipse
	else if (foci.size() > 1)
	{
		// Keep ellipseDetail (0,1]
		double ellipseDetail = 0.25f;
		for (unsigned int focalIndex1 = 0; focalIndex1 < foci.size(); ++focalIndex1)
		{
			const Point2* fp1 = foci[focalIndex1];
			for (unsigned int focalIndex2 = 0; focalIndex2 < foci.size(); ++focalIndex2)
			{
				const Point2* fp2 = foci[focalIndex2];

				if (focalIndex1 == focalIndex2)
					continue;

				// slope of a line
				double deltaX = fp2->getX() - fp1->getX();
				double deltaY = fp2->getY() - fp1->getY();
				double slope = deltaY / deltaX;

				double stepX = abs(deltaX * ellipseDetail);
				double stepY = abs(deltaY * ellipseDetail);

				Point2 cp((deltaX >= 0) ? fp1->getX() : fp2->getX(),
						  (deltaY >= 0) ? fp1->getY() : fp2->getY());
				while (((deltaX >= 0) ? cp.getX() <= fp2->getX() : cp.getX() <= fp1->getX()) &&
					   ((deltaY >= 0) ? cp.getY() <= fp2->getY() : cp.getY() <= fp1->getY()))
				{
					// draw a Ellipse around a point
					for (int i = -radius; i < radius; ++i)
					{
						int xIndex = cp.getX() + i;
						std::map<int, int> newMap;
						for (int j = -radius; j < radius; ++j)
						{
							int yIndex = cp.getY() + j;

							// calculate the distance
							double distance = sqrt(pow(((double)i), 2.0f) + pow(((double)j), 2.0f));
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

					cp.setX(cp.getX() + stepX);
					cp.setY(cp.getY() + stepY);
				}
			}
		}
	}
}

//Just a COPY and PASTE of createHeatmap() but just the last focal point
void Ellipse::updateHeatmap()
{
	// Circle
	if (foci.size() == 1)
	{
		for (unsigned int focalIndex = foci.size()-1; focalIndex < foci.size(); ++focalIndex)
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
	// Ellipse
	else if (foci.size() > 1)
	{
		// Keep ellipseDetail (0,1]
		double ellipseDetail = 0.25f;
		for (unsigned int focalIndex1 = 0; focalIndex1 < foci.size(); ++focalIndex1)
		{
			const Point2* fp1 = foci[focalIndex1];
			unsigned int focalIndex2 = 0;
			if(focalIndex1 == foci.size()-1)
			    focalIndex2 = 0;
			else
			    focalIndex2 = foci.size()-1;
			for (; focalIndex2 < foci.size(); ++focalIndex2)
			{
				const Point2* fp2 = foci[focalIndex2];

				if (focalIndex1 == focalIndex2)
					continue;

				// slope of a line
				double deltaX = fp2->getX() - fp1->getX();
				double deltaY = fp2->getY() - fp1->getY();
				double slope = deltaY / deltaX;

				double stepX = abs(deltaX * ellipseDetail);
				double stepY = abs(deltaY * ellipseDetail);

				Point2 cp((deltaX >= 0) ? fp1->getX() : fp2->getX(),
						  (deltaY >= 0) ? fp1->getY() : fp2->getY());
				while (((deltaX >= 0) ? cp.getX() <= fp2->getX() : cp.getX() <= fp1->getX()) &&
					   ((deltaY >= 0) ? cp.getY() <= fp2->getY() : cp.getY() <= fp1->getY()))
				{
					// draw a Ellipse around a point
					for (int i = -radius; i < radius; ++i)
					{
						int xIndex = cp.getX() + i;
						std::map<int, int> newMap;
						for (int j = -radius; j < radius; ++j)
						{
							int yIndex = cp.getY() + j;

							// calculate the distance
							double distance = sqrt(pow(((double)i), 2.0f) + pow(((double)j), 2.0f));
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

					cp.setX(cp.getX() + stepX);
					cp.setY(cp.getY() + stepY);
				}
			}
		}
	}
}

const Point2* Ellipse::getFocalPoint(unsigned int index) const
{
	if (index >= foci.size())
		return NULL;

	return foci[index];
}

double Ellipse::getRadius() const
{
	return radius;
}

int Ellipse::getMaxHit() const
{
	return maxHit;
}

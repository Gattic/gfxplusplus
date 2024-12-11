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
	createHeatmap();
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
        const Point2* cFocalPoint = foci[0];
        for (int i = -radius; i < radius; ++i)
        {
            int xIndex = cFocalPoint->getX() + i;
            std::map<int, int>& row = heatmap[xIndex]; // Reference to reduce repeated lookups

            for (int j = -radius; j < radius; ++j)
            {
                int yIndex = cFocalPoint->getY() + j;

                // Calculate the distance
                double distance = sqrt(static_cast<double>(i * i + j * j));
                if (distance > radius)
                {
                    continue;
                }

                // Update the heatmap
                int& hitCount = row[yIndex]; // Reference to directly access the value
                ++hitCount;

                if (hitCount > maxHit)
                {
                    maxHit = hitCount;
                }
            }
        }
    }
    // Ellipse
    else if (foci.size() > 1)
    {
        // Keep ellipseDetail in range (0, 1]
        double ellipseDetail = 0.25f;

        for (unsigned int focalIndex1 = 0; focalIndex1 < foci.size(); ++focalIndex1)
        {
            const Point2* fp1 = foci[focalIndex1];
            for (unsigned int focalIndex2 = 0; focalIndex2 < foci.size(); ++focalIndex2)
            {
                if (focalIndex1 == focalIndex2)
                {
                    continue;
                }

                const Point2* fp2 = foci[focalIndex2];

                // Slope of the line
                double deltaX = fp2->getX() - fp1->getX();
                double deltaY = fp2->getY() - fp1->getY();

                double stepX = ellipseDetail * fabs(deltaX);
                double stepY = ellipseDetail * fabs(deltaY);

                Point2 cp((deltaX >= 0) ? fp1->getX() : fp2->getX(),
                          (deltaY >= 0) ? fp1->getY() : fp2->getY());

                while (((deltaX >= 0) ? cp.getX() <= fp2->getX() : cp.getX() <= fp1->getX()) &&
                       ((deltaY >= 0) ? cp.getY() <= fp2->getY() : cp.getY() <= fp1->getY()))
                {
                    int baseX = cp.getX();
                    int baseY = cp.getY();

                    for (int i = -radius; i < radius; ++i)
                    {
                        int xIndex = baseX + i;
                        std::map<int, int>& row = heatmap[xIndex]; // Reference to reduce repeated lookups

                        for (int j = -radius; j < radius; ++j)
                        {
                            int yIndex = baseY + j;

                            // Calculate the distance
                            double distance = sqrt(static_cast<double>(i * i + j * j));
                            if (distance > radius)
                            {
                                continue;
                            }

                            // Update the heatmap
                            int& hitCount = row[yIndex]; // Reference to directly access the value
                            ++hitCount;

                            if (hitCount > maxHit)
                            {
                                maxHit = hitCount;
                            }
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

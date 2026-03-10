// Copyright 2026 Robert Carneiro, Derek Meer, Matthew Tabak, Eric Lujan
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
#include "Graphable.h"
#include "RUGraph.h"
#include "../GFXUtilities/Ellipse.h"
#include "../Graphics/GfxRenderer.h"

// On Windows, wingdi.h declares an Ellipse() function that hides our Ellipse class.
// Use elaborated type specifier "class Ellipse" to refer to the class unambiguously.

template <>
void Graphable<class Ellipse>::computeAxisRanges(bool additionOptimization)
{
	if (!parent)
		return;

	if (points.empty())
		return;

	redoRange = !additionOptimization;
	if(additionOptimization)
	{
		// keep current ranges; ellipse points are pre-normalized elsewhere
	}

	// Use focal point as reference for ranges
	unsigned int i = 0;
	for (; i < points.size(); ++i)
	{
		class Ellipse* pt = points[i];
		const Point2* f = pt->getFocalPoint(0);
		if (!f)
			continue;
		float x_pt = f->getX();
		float y_pt = f->getY();

		setXMax(x_pt);
		setXMin(x_pt);
		setYMax(y_pt);
		setYMin(y_pt);
	}

	// Set the parents
	parent->setXMin(getXMin());
	parent->setXMax(getXMax());
	parent->setYMin(getYMin());
	parent->setYMax(getYMax());

	// Normalize the points
	normalizedPoints = points;
	parent->requireDrawUpdate();
}

template <>
void Graphable<class Ellipse>::draw(gfxpp* cGfx)
{
	if (!parent)
		return;

	if (cGfx->getDraw())
		cGfx->getDraw()->setDrawColor(getColor().r, getColor().g, getColor().b,
					   getColor().a);

	for (unsigned int p = 0; p < normalizedPoints.size(); ++p)
	{
		class Ellipse* pt = normalizedPoints[p];
		if (!pt)
			continue;

		double r = pt->getRadius();
		const Point2* f = pt->getFocalPoint(0);
		if (!f)
			continue;
		int cx = static_cast<int>(f->getX());
		int cy = static_cast<int>(f->getY());

		// draw a circle approximation from radius
		for (int i = -static_cast<int>(r); i <= static_cast<int>(r); ++i)
		{
			int j = static_cast<int>(sqrt(r*r - i*i));
			if (cGfx->getDraw())
			{
				cGfx->getDraw()->drawPoint(cx + i, cy + j);
				cGfx->getDraw()->drawPoint(cx + i, cy - j);
			}
		}
	}
}

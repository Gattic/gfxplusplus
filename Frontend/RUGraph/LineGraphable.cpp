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
#include "Graphable.h"

//TODO: Separate Scalar1D from Point2D and implement this add fncality

template <>
void Graphable<Point2>::computeAxisRanges(gfxpp* cGfx, bool additionOptimization)
{
	if (!cGfx)
		return;

	if (!parent)
		return;

	if (!cGfx->getRenderer())
		return;

	if (points.empty())
		return;

	redoRange = !additionOptimization;
	if(additionOptimization)
	{
		// Is the latest y not within the current range?
		float cY = points[points.size()-1]->getY();
		if(!((cY >= yMin) && (cY <= yMax)))
			redoRange = true;
	}

	if(redoRange)
	{
		float y_max = points[0]->getY();
		float y_min = y_max;

		for (unsigned int i = 1; i < points.size(); ++i)
		{
			Point2* pt = points[i];
			float y_pt = pt->getY();

			if (y_pt > y_max)
				y_max = y_pt;

			else if (y_pt < y_min)
				y_min = y_pt;
		}

		yMin = y_min;
		yMax = y_max;
	}

	// Set the pixels array
	int width = parent->getWidth();
	int height = parent->getHeight();
	if((oldWidth*oldHeight != width*height) || (!pixels))
	{
		pixels = (unsigned int*)malloc(width * height * sizeof(*pixels));
	}

	oldWidth = width;
	oldHeight = height;

	//==============================================Normalize the points==============================================

	float xRange = (float)points.size();
	float yRange = yMax - yMin;

	float pointXGap = ((float)parent->getWidth()) / xRange;
	float pointYGap = ((float)parent->getHeight()) / yRange;

	unsigned int i = 0;
	if(redoRange)
		normalizedPoints.clear();
	else
		i = points.size()-1;

	for (; i < points.size(); ++i)
	{
		float newXValue = i * pointXGap;
		float newYValue = (points[i]->getY() - yMin) * pointYGap;
		normalizedPoints.push_back(new Point2(parent->getAxisOriginX() + newXValue,
							parent->getAxisOriginY() + parent->getHeight() - newYValue));
	}
}

template <>
void Graphable<Point2>::draw(gfxpp* cGfx)
{
	int width = parent->getWidth();
	int height = parent->getHeight();

	unsigned int cBGColor = parent->getBGColor().r;
	cBGColor = cBGColor*0x100 + parent->getBGColor().g;
	cBGColor = cBGColor*0x100 + parent->getBGColor().b;
	cBGColor = cBGColor*0x100 + parent->getBGColor().a;

	unsigned int cColor = getColor().r;
	cColor = cColor*0x100 + getColor().g;
	cColor = cColor*0x100 + getColor().b;
	cColor = cColor*0x100 + getColor().a;

	// The background color
	for (int y = 0; y < height; ++y)
	{
		unsigned int* row = pixels + y * width;
		for (int x = 0; x < width; ++x)
		{
			row[x] = cBGColor;
		}
	}

	// Draw the line and give it thickness
	for (unsigned int i=0; i < normalizedPoints.size(); ++i)
	{
		Point2* cPoint = normalizedPoints[i];
		int cX = ((unsigned int)cPoint->getX());
		int cY = ((unsigned int)cPoint->getY());

		// Line - 1
		if((cX < width) && (cY-1 < height) && (cY-1 > 0))
		{
			unsigned int* row = pixels + ((cY-1) * width);
			row[cX] = cColor;
		}

		// Line
		if((cX < width) && (cY < height))
		{
			unsigned int* row = pixels + (cY * width);
			row[cX] = cColor;
		}

		// Line + 1
		if((cX < width) && (cY+1 < height))
		{
			unsigned int* row = pixels + ((cY+1) * width);
			row[cX] = cColor;
		}
	}

	SDL_Rect dRect;
	dRect.x = parent->getAxisOriginX();
	dRect.y = parent->getAxisOriginY();
	dRect.w = parent->getWidth();
	dRect.h = parent->getHeight();

	SDL_UpdateTexture(parent->getBackground(), NULL, pixels, width * sizeof(*pixels));
}

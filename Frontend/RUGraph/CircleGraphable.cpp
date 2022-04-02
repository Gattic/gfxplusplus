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
#include "RUGraph.h"
#include "../GFXUtilities/Circle.h"

template <>
void Graphable<Circle>::computeAxisRanges(bool additionOptimization)
{
	if (!parent)
		return;

	for (unsigned int i = 1; i < points.size(); ++i)
	{
		Circle* pt = points[i];
		pt->createHeatmap();
	}

	parent->requireDrawUpdate();
}

template <>
void Graphable<Circle>::draw(gfxpp* cGfx)
{
	for (unsigned int i = 1; i < points.size(); ++i)
	{
		Circle* pt = points[i];

		if (pt->getRadius() <= 0)
			return;
	
		if (pt->foci.size() == 0)
			return;
	
		SDL_SetRenderDrawColor(cGfx->getRenderer(), getColor().r, getColor().g, getColor().b,
							   getColor().a);
	
		std::map<int, std::map<int, int> >::const_iterator itr = pt->heatmap.begin();
		for (; itr != pt->heatmap.end(); ++itr)
		{
			int xIndex = itr->first;
			std::map<int, int>::const_iterator itr2 = pt->heatmap[xIndex].begin();
			for (; itr2 != pt->heatmap[xIndex].end(); ++itr2)
			{
				int yIndex = itr2->first;
				int cHeat = itr2->second;
				// printf("pt->heatmap[%d][%d]: %d\n", xIndex, yIndex, cHeat);
	
				// calculate the hue
				double hue = ((double)cHeat) / ((double)pt->getMaxHit());
				hue = 1.0f - hue;
	
				// get the color
				int8_t redMask = 0;
				int8_t greenMask = 0;
				int8_t blueMask = 0;
				unsigned int colorMask = gfxpp::RGBfromHue(hue, &redMask, &greenMask, &blueMask);
	
				// set the color and draw the point
				SDL_SetRenderDrawColor(cGfx->getRenderer(), redMask, greenMask, blueMask,
									   SDL_ALPHA_OPAQUE);
				SDL_RenderDrawPoint(cGfx->getRenderer(), parent->getX() + xIndex,
									parent->getY() + yIndex);
			}
		}
	}
}

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
#include "../GFXUtilities/ActionBubble.h"

template <>
void Graphable<ActionBubble>::computeAxisRanges(bool additionOptimization)
{
	if (!parent)
		return;

	//TODO: Normalize like LineGraphable

	parent->requireDrawUpdate();
}

template <>
void Graphable<ActionBubble>::draw(gfxpp* cGfx)
{
	//TODO: Use the action type to change the color for each point in the first loop
	SDL_SetRenderDrawColor(cGfx->getRenderer(), getColor().r, getColor().g, getColor().b, getColor().a);

	for (unsigned int i = 1; i < points.size(); ++i)
	{
		const ActionBubble* cBubble = points[i];
		if (!cBubble)
			continue;

		const Point2* cFocalPoint = cBubble->getFocalPoint();
		if (!cFocalPoint)
			continue;

		double radius = cBubble->getRadius();
		if (radius <= 0)
			continue;
	
		for (int i = -radius; i < radius; ++i)
		{
			int xIndex = cFocalPoint->getX() + i;

			std::map<int, int> newMap;
			for (int j = -radius; j < radius; ++j)
			{
				int yIndex = cFocalPoint->getY() + j;

	
				// set the color and draw the point
				SDL_RenderDrawPoint(cGfx->getRenderer(), parent->getX() + xIndex,
									parent->getY() + yIndex);
			}
		}
	}
}

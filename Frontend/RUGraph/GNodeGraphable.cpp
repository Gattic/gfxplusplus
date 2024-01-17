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
#include "../GFXUtilities/gnode.h"

//TODO: Separate Scalar1D from Point2D and implement this add fncality

template <>
void Graphable<GNode>::computeAxisRanges(bool additionOptimization)
{
	if (!parent)
		return;

	if (points.empty())
		return;
}

template <>
void Graphable<GNode>::draw(gfxpp* cGfx)
{
	if (!parent)
		return;

	SDL_SetRenderDrawColor(cGfx->getRenderer(), getColor().r, getColor().g, getColor().b,
						   getColor().a);

	if (points.empty())
		return;
	
	// Layers, Nodes, Edges
	int xBlock = parent->getWidth() / points.size();
	for(unsigned int i = 0; i < points.size(); ++i) // x axis
	{
		const GNode* cLayer = points[i];
		const std::vector<std::pair<std::vector<double>, double> >& nodes = cLayer->nodes;

		int xStart = (xBlock * i);
		int xEnd = (xBlock * (i+1));
		int xCenter = (xStart + xEnd) / 2;

		int yBlock = parent->getHeight() / nodes.size();
		for(unsigned int j = 0; j < nodes.size(); ++j) // y axis
		{
			const std::pair<std::vector<double>, double>& cNode = nodes[j];
			double nodeWeight = cNode.second;

			int yStart = (yBlock * j);
			int yEnd = (yBlock * (j+1));
			int yCenter = (yStart + yEnd) / 2;



		//SDL_Rect bgRect;
		//bgRect.x =
		//bgRect.w =
		//SDL_RenderFillRect(cGfx->getRenderer(), &bgRect);


			double xRadius = xBlock / 2.0f;
			for (int xPt = -xRadius; xPt < xRadius; ++xPt)
			{
				double xDistance = sqrt(pow(((double)xPt), 2.0f));
				if (xDistance > xRadius)
					continue;

				int xIndex = xCenter + xPt;

				std::map<int, int> newMap;
				double yRadius = yBlock / 2.0f;
				for (int yPt = -yRadius; yPt < yRadius; ++yPt)
				{
					double yDistance = sqrt(pow(((double)yPt), 2.0f));
					if (yDistance > yRadius)
						continue;

					int yIndex = yCenter + yPt;

					// calculate the distance
					double distance = sqrt(pow(((double)xPt), 2.0f) + pow(((double)yPt), 2.0f));
					double hue = distance / sqrt(pow(((double)xRadius), 2.0f) + pow(((double)yRadius), 2.0f));

					// get the color
					int8_t redMask = 0;
					int8_t greenMask = 0;
					int8_t blueMask = 0;
					unsigned int colorMask = gfxpp::RGBfromHue(hue, &redMask, &greenMask, &blueMask);
		
					// Set the color and draw the point
					SDL_SetRenderDrawColor(cGfx->getRenderer(), redMask, greenMask, blueMask, SDL_ALPHA_OPAQUE);
					SDL_RenderDrawPoint(cGfx->getRenderer(), xIndex, yIndex);
					//printf("DRAW-CIRCLE[%d][%d]: %08X\n", xIndex, yIndex, colorMask);
				}
			}




			// Draw the edges connecting the nodes
			for(unsigned int k = 0; k < cNode.first.size(); ++k)
			{
				double edge = cNode.first[k];
			}
		}
	}
}

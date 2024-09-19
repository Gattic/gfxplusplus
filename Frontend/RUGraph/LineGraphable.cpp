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
#include "../GFXUtilities/point2.h"

//TODO: Separate Scalar1D from Point2D and implement this add fncality

template <>
void Graphable<Point2>::draw(gfxpp* cGfx)
{
    if (!parent)
        return;

    SDL_Color color = getColor();
    SDL_SetRenderDrawColor(cGfx->getRenderer(), color.r, color.g, color.b, color.a);

    float vscale = parent->getVScale();

    if (normalizedPoints.empty())
        return;

    // Prepare the points array for SDL_RenderDrawLines
    std::vector<SDL_Point> points(normalizedPoints.size());

    for (unsigned int i = 0; i < normalizedPoints.size(); ++i)
    {
        Point2* cPoint = normalizedPoints[i];
        SDL_Point point;
        point.x = static_cast<int>(cPoint->getX());
        point.y = static_cast<int>(cPoint->getY());
        points[i] = point;
    }

    // Draw all the lines at once
    SDL_RenderDrawLines(cGfx->getRenderer(), &points[0], points.size());
}

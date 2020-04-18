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

#include "GRelativeLayout.h"
#include "../GFXUtilities/EventTracker.h"
#include "../GItems/Mini/RUBackgroundComponent.h"
#include "../GItems/Mini/RUBorderComponent.h"

GRelativeLayout::GRelativeLayout(std::string layoutName)
{
	name = layoutName;
	layoutType = 0; // 0 = Relative; 1 = Linear
}

void GRelativeLayout::calculateSubItemPositions(std::pair<int, int> parentOffset)
{
	//
}

void GRelativeLayout::processSubItemEvents(gfxpp* cGfx, EventTracker* eventsStatus,
										   GPanel* parentPanel, SDL_Event event, int mouseX,
										   int mouseY)
{
	//
}

void GRelativeLayout::updateBackground(SDL_Renderer* renderer)
{
	//
}

void GRelativeLayout::updateBackgroundHelper(SDL_Renderer* renderer)
{
	//
}

std::string GRelativeLayout::getType() const
{
	return "GRelativeLayout";
}

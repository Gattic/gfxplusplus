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

#include "GLinearLayout.h"
#include "../GFXUtilities/EventTracker.h"
#include "../GItems/Mini/RUBackgroundComponent.h"
#include "../GItems/Mini/RUBorderComponent.h"
#include "../Graphics/graphics.h"
#include <stdio.h>

GLinearLayout::GLinearLayout(shmea::GString layoutName, int newOrientation)
{
	name = layoutName;
	layoutType = 1; // 0 = Relative; 1 = Linear
	orientation = newOrientation;
	horizontalAlign = ALIGN_LEFT;
}

int GLinearLayout::getOrientation() const
{
	return orientation;
}

void GLinearLayout::setOrientation(int newOrientation)
{
	orientation = newOrientation;
}

void GLinearLayout::setAlignment(HAlign align)
{
	horizontalAlign = align;
}

GLinearLayout::HAlign GLinearLayout::getAlignment() const
{
	return horizontalAlign;
}

void GLinearLayout::updateBackground(gfxpp* cGfx)
{
	//
}

void GLinearLayout::calculateSubItemPositions(std::pair<int, int> parentOffset)
{
	// Prep the other dimenion for scaling
	width = 0;
	height = 0;

	// First pass: compute max width (VERTICAL) or max height (HORIZONTAL)
	int maxChildWidth = 0;
	int maxChildHeight = 0;
	for (unsigned int i = 0; i < subitems.size(); ++i)
	{
		GItem* cItem = subitems[i];
		if (!cItem || !cItem->isVisible())
			continue;
		// Allow children to compute their own sizes before we position
		cItem->calculateSubItemPositions(parentOffset);
		if (orientation == VERTICAL)
		{
			int w = cItem->getPaddingX() + cItem->getWidth();
			if (w > maxChildWidth) maxChildWidth = w;
		}
		else
		{
			int h = cItem->getPaddingY() + cItem->getHeight();
			if (h > maxChildHeight) maxChildHeight = h;
		}
	}

	// Default layout coordinates
	std::pair<int, int> cItemOffset(parentOffset.first + getX(), parentOffset.second + getY());
	for (unsigned int i = 0; i < subitems.size(); ++i)
	{
		GItem* cItem = subitems[i];
		if (cItem == NULL)
			continue;

		// Compute aligned X for vertical stacks
		int alignedX = cItemOffset.first + cItem->getMarginX();
		if (orientation == VERTICAL)
		{
			const int itemW = cItem->getWidth();
			if (horizontalAlign == ALIGN_CENTER)
				alignedX += (maxChildWidth - itemW) / 2;
			else if (horizontalAlign == ALIGN_RIGHT)
				alignedX += (maxChildWidth - itemW);
			// ALIGN_LEFT -> no change
		}
		cItem->setX(alignedX);
		cItem->setY(cItemOffset.second + cItem->getMarginY());

		if(!cItem->isVisible())
			continue;

		// Prepare for the next item offset
		if (orientation == VERTICAL)
		{
			// Update y position for component rendering.
			cItemOffset.second += cItem->getHeight() + getPaddingY();
			height += cItem->getPaddingY() + cItem->getHeight();

			// Update the other dimensions max
			if (maxChildWidth > width)
				width = maxChildWidth;
		}
		else if (orientation == HORIZONTAL)
		{
			// Update x position for component rendering.
			cItemOffset.first += cItem->getWidth() + getPaddingX();
			width += cItem->getPaddingX() + cItem->getWidth();

			// Update the other dimensions max
			if (maxChildHeight > height)
				height = maxChildHeight;
		}

		// Create an overflow variable
		// If next row exceeds layout height, stop rendering components. Exit loop.
		/*if (cItemOffset.second > getHeight())
			setHeight(cItemOffset.second);*/
	}
}

void GLinearLayout::processSubItemEvents(gfxpp* cGfx, EventTracker* eventsStatus,
							 GPanel* parentPanel, GfxEvent event, int mouseX,
							 int mouseY)
{
	if (!eventsStatus)
		return;

	if (!parentPanel)
		return;

	if (!visible)
		return;

	// Linear layout coordinates
	for (unsigned int i = 0; i < subitems.size(); ++i)
	{
		GItem* cItem = subitems[i];
		if (cItem == NULL)
			continue;

		//
		EventTracker* subEventsStatus =
			cItem->processEvents(cGfx, parentPanel, event, mouseX, mouseY);
		if (subEventsStatus->hovered)
			eventsStatus->hovered = true;

		if (subEventsStatus->downClicked)
		{
			eventsStatus->downClicked = true;
			clickedSubItems.insert(std::pair<int, GItem*>(subitems[i]->getID(), subitems[i]));
		}
	}
}

void GLinearLayout::updateBackgroundHelper(gfxpp* cGfx)
{
	// In OpenGL mode there is no SDL_Renderer; use draw abstraction instead
	if (!cGfx->getDraw())
		return;

	if (!visible)
		return;

	// Go backwards because of dropdowns
	for (unsigned int i = subitems.size(); i > 0; --i)
	{
		GItem* cItem = subitems[i - 1]; //-1 buffer/padding for the counter because its unsigned
		if (cItem == NULL)
			continue;

		// draw the item
		cItem->updateBackgroundHelper(cGfx);
	}
}

void GLinearLayout::onMouseDown(gfxpp* cGfx, GPanel* cPanel, int eventX, int eventY)
{
	std::pair<int, int> parentOffset(0, 0);
	calculateSubItemPositions(parentOffset);
	drawUpdate = true;
}

shmea::GString GLinearLayout::getType() const
{
	return "GLinearLayout";
}

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

#include "GRelativeLayout.h"
#include "../GFXUtilities/EventTracker.h"
#include "../GItems/Mini/RUBackgroundComponent.h"
#include "../GItems/Mini/RUBorderComponent.h"
#include "../Graphics/graphics.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

GRelativeLayout::GRelativeLayout(shmea::GString layoutName)
{
	name = layoutName;
	layoutType = 0; // 0 = Relative; 1 = Linear
	computedContentWidth = 0;
	computedContentHeight = 0;
}

// Compute the union bounds of children relative to the layout's anchor origin (0,0)
// using alignment to place each child inside an initially zero-sized content rect,
// then derive the final width/height. Margins are not included in size; they are
// applied during placement.
void GRelativeLayout::computeContentExtents()
{
	int minX = 0;
	int minY = 0;
	int maxX = 0;
	int maxY = 0;
	bool any = false;

	// We estimate child positions within a content box centered at (0,0) initially.
	// For size calculation, treat content origin as (0,0) and align children accordingly,
	// then grow bounds to include each child's rectangle [x, y, x+w, y+h].
	for (unsigned int i = 0; i < subitems.size(); ++i)
	{
		GItem* cItem = subitems[i];
		if (!cItem || !cItem->isVisible())
			continue;

		int itemW = cItem->getWidth();
		int itemH = cItem->getHeight();
		// If size is unknown, attempt a pre-pass layout on the child to derive its size
		if (itemW <= 0 || itemH <= 0)
		{
			std::pair<int,int> zeroOffset(0,0);
			cItem->calculateSubItemPositions(zeroOffset);
			itemW = cItem->getWidth();
			itemH = cItem->getHeight();
		}
		if (itemW <= 0 || itemH <= 0)
			continue;

		int align = TOP_LEFT;
		std::map<GItem*, int>::const_iterator it = itemAlignment.find(cItem);
		if (it != itemAlignment.end())
			align = it->second;

		int x = 0;
		int y = 0;
		// For extents we don't know final content size yet; handle by anchoring relative positions:
		// - TOP_LEFT: (0,0)
		// - TOP: (-(w/2), 0)
		// - TOP_RIGHT: (-w, 0)
		// - LEFT: (0, -(h/2))
		// - CENTER: (-(w/2), -(h/2))
		// - RIGHT: (-w, -(h/2))
		// - BOTTOM_LEFT: (0, -h)
		// - BOTTOM: (-(w/2), -h)
		// - BOTTOM_RIGHT: (-w, -h)
		switch (align)
		{
			case TOP: x = -(itemW / 2); y = 0; break;
			case TOP_RIGHT: x = -itemW; y = 0; break;
			case LEFT: x = 0; y = -(itemH / 2); break;
			case CENTER: x = -(itemW / 2); y = -(itemH / 2); break;
			case RIGHT: x = -itemW; y = -(itemH / 2); break;
			case BOTTOM_LEFT: x = 0; y = -itemH; break;
			case BOTTOM: x = -(itemW / 2); y = -itemH; break;
			case BOTTOM_RIGHT: x = -itemW; y = -itemH; break;
			case TOP_LEFT:
			default: x = 0; y = 0; break;
		}

		// Grow extents to include this child's rect at (x,y)
		if (!any)
		{
			minX = x; minY = y; maxX = x + itemW; maxY = y + itemH; any = true;
		}
		else
		{
			if (x < minX) minX = x;
			if (y < minY) minY = y;
			if (x + itemW > maxX) maxX = x + itemW;
			if (y + itemH > maxY) maxY = y + itemH;
		}
	}

	if (!any)
	{
		computedContentWidth = 0;
		computedContentHeight = 0;
		return;
	}

	computedContentWidth = (maxX - minX);
	computedContentHeight = (maxY - minY);
}

void GRelativeLayout::calculateSubItemPositions(std::pair<int, int> parentOffset)
{
	// Pre-size pass: allow child layouts to compute their own sizes before centering
	for (unsigned int i = 0; i < subitems.size(); ++i)
	{
		GItem* cItem = subitems[i];
		if (!cItem) continue;
		cItem->calculateSubItemPositions(parentOffset);
	}

	// Compute content size from children now that they have sizes
	computeContentExtents();

	// If layout width/height are not set, treat (getX(),getY()) as an anchor point and
	// center the entire content box around it; else respect the explicit box from top-left.
	int contentX = 0;
	int contentY = 0;
	int contentW = 0;
	int contentH = 0;

	if (getWidth() <= 0 || getHeight() <= 0)
	{
		// Centered around anchor
		contentW = computedContentWidth;
		contentH = computedContentHeight;
		const int anchorX = parentOffset.first + getX();
		const int anchorY = parentOffset.second + getY();
		contentX = anchorX - (contentW / 2);
		contentY = anchorY - (contentH / 2);
		// Apply vertical padding only; horizontal remains true center
		contentY += getPaddingY();
		// No additional global horizontal shift; handle per-child for centered alignments only
	}
	else
	{
		// Explicit size: keep vertical as before; horizontally center computed content within explicit box
		contentY = parentOffset.second + getY() + getPaddingY();
		contentH = getHeight() - (2 * getPaddingY());
		if (computedContentWidth > 0)
		{
			int anchorCenterX = parentOffset.first + getX() + (getWidth() / 2);
			contentX = anchorCenterX - (computedContentWidth / 2);
			contentW = computedContentWidth;
		}
		else
		{
			contentX = parentOffset.first + getX() + getPaddingX();
			contentW = getWidth() - (2 * getPaddingX());
		}
		// No additional global horizontal shift; handle per-child for centered alignments only

	}

	// Prepare outer bounds for side alignments (explicit mode only)
	bool explicitMode = (getWidth() > 0 && getHeight() > 0);
	int outerX = 0;
	int outerW = 0;
	if (explicitMode)
	{
		outerX = parentOffset.first + getX() + getPaddingX();
		outerW = getWidth() - (2 * getPaddingX());
	}

	for (unsigned int i = 0; i < subitems.size(); ++i)
	{
		GItem* cItem = subitems[i];
		if (cItem == NULL)
			continue;

		// Determine alignment for this item (default TOP_LEFT)
		int align = TOP_LEFT;
		std::map<GItem*, int>::const_iterator it = itemAlignment.find(cItem);
		if (it != itemAlignment.end())
			align = it->second;

		// Compute anchored position inside content rect
		int targetX = contentX;
		int targetY = contentY;
		const int itemW = cItem->getWidth();
		const int itemH = cItem->getHeight();
		// No global horizontal bias; respect alignment strictly within content/outer bounds

		switch (align)
		{
			case TOP:
				targetX = contentX + (contentW - itemW) / 2;
				targetY = contentY;
				break;
			case TOP_RIGHT:
				targetX = explicitMode ? (outerX + outerW - itemW) : (contentX + contentW - itemW);
				targetY = contentY;
				break;
			case LEFT:
				targetX = explicitMode ? outerX : contentX;
				targetY = contentY + (contentH - itemH) / 2;
				break;
			case CENTER:
				targetX = contentX + (contentW - itemW) / 2;
				targetY = contentY + (contentH - itemH) / 2;
				break;
			case RIGHT:
				targetX = explicitMode ? (outerX + outerW - itemW) : (contentX + contentW - itemW);
				targetY = contentY + (contentH - itemH) / 2;
				break;
			case BOTTOM_LEFT:
				targetX = explicitMode ? outerX : contentX;
				targetY = contentY + contentH - itemH;
				break;
			case BOTTOM:
				targetX = contentX + (contentW - itemW) / 2;
				targetY = contentY + contentH - itemH;
				break;
			case BOTTOM_RIGHT:
				targetX = explicitMode ? (outerX + outerW - itemW) : (contentX + contentW - itemW);
				targetY = contentY + contentH - itemH;
				break;
			case TOP_LEFT:
			default:
				targetX = explicitMode ? outerX : contentX;
				targetY = contentY;
				break;
		}

		// No additional bias adjustments

		// Apply per-item margins
		targetX += cItem->getMarginX();
		targetY += cItem->getMarginY();

		{
			int align = TOP_LEFT;
			std::map<GItem*, int>::const_iterator it = itemAlignment.find(cItem);
			if (it != itemAlignment.end()) align = it->second;
		}

		// Commit absolute position
		cItem->setX(targetX);
		cItem->setY(targetY);

		if(!cItem->isVisible())
			continue;

		// Recurse for subitems using original parentOffset (consistent with other layouts)
		cItem->calculateSubItemPositions(parentOffset);
	}
}

void GRelativeLayout::processSubItemEvents(gfxpp* cGfx, EventTracker* eventsStatus,
							   GPanel* parentPanel, GfxEvent event, int mouseX,
							   int mouseY)
{
	if (!eventsStatus)
		return;

	if (!parentPanel)
		return;

	if (!visible)
		return;

	clickedSubItems.clear();
	for (unsigned int i = 0; i < subitems.size(); ++i)
	{
		GItem* cItem = subitems[i];
		if (cItem == NULL)
			continue;

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

void GRelativeLayout::updateBackground(gfxpp* cGfx)
{
	//
}

void GRelativeLayout::updateBackgroundHelper(gfxpp* cGfx)
{
	// In OpenGL mode there is no SDL_Renderer; use draw abstraction instead
	if (!cGfx->getDraw())
		return;

	if (!visible)
		return;


	// Draw children back-to-front (reverse for dropdown-like overlays)
	for (unsigned int i = subitems.size(); i > 0; --i)
	{
		GItem* cItem = subitems[i - 1];
		if (cItem == NULL)
			continue;

		cItem->updateBackgroundHelper(cGfx);
	}
}

shmea::GString GRelativeLayout::getType() const
{
	return "GRelativeLayout";
}

void GRelativeLayout::addSubItem(GItem* newItem, unsigned int newZIndex)
{
	if (!newItem)
		return;

	// Default to TOP_LEFT if not specified
	itemAlignment[newItem] = TOP_LEFT;
	GItem::addSubItem(newItem, newZIndex);
	// Recompute positions immediately so there is no visible shift on next frame
	std::pair<int, int> offset(0, 0);
	calculateSubItemPositions(offset);
	drawUpdate = true;
}

void GRelativeLayout::addSubItemAligned(GItem* newItem, Alignment alignment, unsigned int newZIndex)
{
	if (!newItem)
		return;

	itemAlignment[newItem] = (int)alignment;
	GItem::addSubItem(newItem, newZIndex);
	// Recompute positions immediately so there is no visible shift on next frame
	std::pair<int, int> offset(0, 0);
	calculateSubItemPositions(offset);
	drawUpdate = true;
}

void GRelativeLayout::setItemAlignment(GItem* item, Alignment alignment)
{
	if (!item)
		return;

	itemAlignment[item] = (int)alignment;
	std::pair<int, int> offset(0, 0);
	calculateSubItemPositions(offset);
	drawUpdate = true;
}

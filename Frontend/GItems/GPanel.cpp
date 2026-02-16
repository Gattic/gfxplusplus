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
#include "GPanel.h"
#include "../GFXUtilities/EventTracker.h"
#include "../GUI/RUMsgBox.h"
#include "../GUI/Text/RUTextComponent.h"
#include "../Graphics/graphics.h"
#ifdef GFX_HAVE_OPENGL
#include <GLFW/glfw3.h>
#endif
#include "Backend/Networking/main.h"
#include "GItem.h"
#include "GLayout.h"
#include "Mini/RUBackgroundComponent.h"
#include "Mini/RUBorderComponent.h"
#include "RUColors.h"
#include "RUComponent.h"
#include "../Graphics/GfxRenderer.h"

GPanel::GPanel(const shmea::GString& newName, int newWidth, int newHeight)
{
	name = newName;
	width = newWidth;
	height = newHeight;
	focus = false;
	setBGColor(RUColors::DEFAULT_COLOR_BACKGROUND);
	lastMouseMotionItem = NULL;

	qMutex = shmea::GPointer<shmea::GMutex>(new shmea::GMutex());
}

GPanel::~GPanel()
{
	//
}

void GPanel::onShow(gfxpp* cGfx)
{
	focus = true;
	cGfx->setFocusedPanel(this);
}

void GPanel::onHide(gfxpp* cGfx)
{
	focus = false;
}

void GPanel::show(gfxpp* cGfx)
{
	onShow(cGfx);
}

void GPanel::hide(gfxpp* cGfx)
{
	onHide(cGfx);
}

void GPanel::hover(gfxpp* cGfx)
{
	//
}

void GPanel::unhover(gfxpp* cGfx)
{
	//
}

void GPanel::addSubItem(GItem* newItem, unsigned int newZIndex)
{
	if (!newItem)
		return;

	shmea::GString itemName = newItem->getName();
	if (getItemByName(itemName))
	{
		printf("GUI item name '%s' already exists. Skipping.\n", itemName.c_str());
		return;
	}

	newItem->setZIndex(newZIndex);
	if (newZIndex == Z_BACK) // -2
	{
		// Add to back of GPanel subitems
		subitems.insert(subitems.begin(), newItem);
	}
	else if (newZIndex == Z_FRONT) // -1
	{
		// Add to front of GPanel subitems
		subitems.push_back(newItem);
	}
	else
	{
		// Index in the render loop
		if (newZIndex >= subitems.size())
			subitems.push_back(newItem);
		else
			subitems.insert(subitems.begin() + newZIndex, newItem);
	}

	std::pair<int, int> offset(0, 0);
	calculateSubItemPositions(offset);
	drawUpdate = true;
}

void GPanel::clearItems(unsigned int numToSave)
{
	lastMouseMotionItem = NULL;
	GItem::clearItems(numToSave);
}

void GPanel::calculateSubItemPositions(std::pair<int, int> parentOffset)
{
	// We just pass in manual X/Y on subitem creation
	for (unsigned int i = 0; i < subitems.size(); ++i)
	{
		GItem* cItem = subitems[i];
		if (cItem == NULL)
			continue;

		// If a layout has no explicit size, default it to fill the panel.
		if (cItem->wantsAutoSize() &&
			(cItem->getWidth() <= 0 || cItem->getHeight() <= 0))
		{
			cItem->setWidth(width);
			cItem->setHeight(height);
		}

		// draw the item
		cItem->calculateSubItemPositions(parentOffset);
	}
}

void GPanel::processSubItemEvents(gfxpp* cGfx, EventTracker* eventsStatus, GPanel* parentPanel,
				  GfxEvent event, int mouseX, int mouseY)
{
	if (!cGfx)
		return;

	if (!focus)
		return;

	// Key events only matter to the focused item; don't scan the whole UI.
	if (event.type == GFX_KEYDOWN || event.type == GFX_KEYUP)
	{
		GItem* focused = cGfx->getFocusedItem();
		if (focused)
			focused->processEvents(cGfx, this, event, mouseX, mouseY);
		return;
	}

	// Mouse move is by far the most frequent event. Cache the previously-hovered
	// top-level item and only process that subtree plus the current target.
	if (event.type == GFX_MOUSEMOTION)
	{
		GItem* candidate = NULL;
		for (unsigned int i = 0; i < subitems.size(); ++i)
		{
			GItem* cItem = subitems[i];
			if (!cItem)
				continue;
			if (!cItem->isVisible())
				continue;
			if (cItem->containsPoint(mouseX, mouseY))
				candidate = cItem; // keep last match (stable with previous multi-item behavior)
		}

		// Let the previous hovered item see the motion event so it can unhover itself.
		if (lastMouseMotionItem && lastMouseMotionItem != candidate)
			lastMouseMotionItem->processEvents(cGfx, this, event, mouseX, mouseY);

		bool hovered = false;
		if (candidate)
		{
			EventTracker* st = candidate->processEvents(cGfx, this, event, mouseX, mouseY);
			if (st && st->hovered)
			{
				hovered = true;
				lastMouseMotionItem = candidate;
			}
			else
				lastMouseMotionItem = NULL;
		}
		else
			lastMouseMotionItem = NULL;

		if (!hovered)
		{
			// Set the default cursor
			GfxCursor* renderCursor = cGfx->getSystemCursor();
			GFX_SetCursor(renderCursor);
		}
		return;
	}

	bool hovered = false;
	for (unsigned int i = 0; i < subitems.size(); ++i)
	{
		GItem* cItem = subitems[i];
		if (!cItem)
			continue;

		EventTracker* subEventsStatus = cItem->processEvents(cGfx, this, event, mouseX, mouseY);
		if (subEventsStatus->hovered)
			hovered = true;
	}

	if (!hovered)
	{
		// Set the default cursor
		GfxCursor* renderCursor = cGfx->getSystemCursor();
		GFX_SetCursor(renderCursor);
	}
}

void GPanel::processQ(gfxpp* cGfx)
{
	//
	unsigned int counter = 0;
	unsigned int maxAtOnce = 100; // Smaller the number, slower but more fluid graphics
	while((updateQueue.size() > 0) && (cGfx->getRunning()) && (counter < maxAtOnce))
	{
		popQ();
		++counter;
	}
}

void GPanel::addToQ(const shmea::ServiceData* cData)
{
    shmea::GMutexLock lock(qMutex.get());
	updateQueue.push(cData);
}

void GPanel::popQ()
{
	const shmea::ServiceData* cData;
	{
        shmea::GMutexLock lock(qMutex.get());
		cData = updateQueue.front();
		updateQueue.pop();
	}

	updateFromQ(cData);
	delete cData;
}

void GPanel::updateFromQ(const shmea::ServiceData* cData)
{
	// Keep this empty in this class
}

void GPanel::updateBackgroundHelper(gfxpp* cGfx)
{
	if (!focus)
		return;

	if (!cGfx)
		return;

	if (!cGfx->getDraw())
		return;

	if (!visible)
		return;

	if (!((width > 0) && (height > 0)))
		return;

	// Update the GUI based on a message queue
	processQ(cGfx);

	// Rebuild the offscreen texture if needed
	bool hasTexture = rebuildTexture(cGfx);
	if (hasTexture)
	{
		blitTexture(cGfx);
	}
	else if (cGfx->getRenderBackend() == gfxpp::RENDER_BACKEND_OPENGL)
	{
		// Fallback path: draw directly when render targets are unavailable.
#ifdef GFX_HAVE_OPENGL
		glPushMatrix();
		glLoadIdentity();
		glTranslatef((float)getX(), (float)getY(), 0.0f);
		updateBGBackground(cGfx);
		updateBackground(cGfx);
		updateBorderBackground(cGfx);
		for (int i = subitems.size() - 1; i >= 0; --i)
			subitems[i]->updateBackgroundHelper(cGfx);
		glPopMatrix();
		return;
#endif
	}

	// Go backwards because of dropdowns
	for (int i = subitems.size() - 1; i >= 0; --i)
		subitems[i]->updateBackgroundHelper(cGfx);
}

void GPanel::updateBackground(gfxpp* cGfx)
{
	// set the background rect
	GfxRect fullRect;
	fullRect.x = 0;
	fullRect.y = 0;
	fullRect.w = width;
	fullRect.h = height;

	drawVerticalGradient(cGfx->getDraw(), fullRect, RUColors::COLOR_DARK_GRAY, RUColors::COLOR_BLACK, 0);
}

shmea::GString GPanel::getType() const
{
	return "GPanel";
}

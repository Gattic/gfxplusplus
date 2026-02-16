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

#include "RUComponent.h"
#include "../GFXUtilities/EventTracker.h"
#include "../Graphics/graphics.h"
#ifdef GFX_HAVE_OPENGL
#include <GLFW/glfw3.h>
#endif
#include "Mini/RUBackgroundComponent.h"
#include "Mini/RUBorderComponent.h"
#include "RUColors.h"
#include "../Graphics/GfxRenderer.h"

RUComponent::RUComponent()
{
	setBGColor(RUColors::DEFAULT_COMPONENT_BACKGROUND);
	lastMouseMotionSubItem = NULL;
}

void RUComponent::clearItems(unsigned int numToSave)
{
	lastMouseMotionSubItem = NULL;
	GItem::clearItems(numToSave);
}

void RUComponent::calculateSubItemPositions(std::pair<int, int> parentOffset)
{
	// Default layout coordinates
	std::pair<int, int> cItemOffset(parentOffset.first + getX(), parentOffset.second + getY());
	for (unsigned int i = 0; i < subitems.size(); ++i)
	{
		GItem* cItem = subitems[i];
		if (cItem == NULL)
			continue;

		cItem->setX(cItemOffset.first + cItem->getMarginX());
		cItem->setY(cItemOffset.second + cItem->getMarginY());

		// draw the item
		cItem->calculateSubItemPositions(parentOffset);
	}
}

void RUComponent::processSubItemEvents(gfxpp* cGfx, EventTracker* eventsStatus, GPanel* parentPanel,
                           GfxEvent event, int mouseX, int mouseY)
{
	if (!eventsStatus)
		return;

	if (!parentPanel)
		return;

	if (!visible)
		return;

	if (!((width > 0) && (height > 0)))
		return;

	// Optimize mouse motion: only process the previously-hovered child and the current target.
	if (event.type == GFX_MOUSEMOTION)
	{
		clickedSubItems.clear();

		GItem* candidate = NULL;
		for (unsigned int i = 0; i < subitems.size(); ++i)
		{
			GItem* cItem = subitems[i];
			if (!cItem)
				continue;
			if (!cItem->isVisible())
				continue;
			if (cItem->containsPoint(mouseX, mouseY))
				candidate = cItem;
		}

		if (lastMouseMotionSubItem && lastMouseMotionSubItem != candidate)
			lastMouseMotionSubItem->processEvents(cGfx, parentPanel, event, mouseX, mouseY);

		if (candidate)
		{
			EventTracker* subEventsStatus =
				candidate->processEvents(cGfx, parentPanel, event, mouseX, mouseY);
			if (subEventsStatus && subEventsStatus->hovered)
			{
				eventsStatus->hovered = true;
				lastMouseMotionSubItem = candidate;
			}
			else
				lastMouseMotionSubItem = NULL;
		}
		else
			lastMouseMotionSubItem = NULL;

		return;
	}

	dispatchSubItemEvents(cGfx, eventsStatus, parentPanel, event, mouseX, mouseY);
}

void RUComponent::updateBackgroundHelper(gfxpp* cGfx)
{
	if (!cGfx->getDraw())
		return;

	if (!visible)
		return;

	if (!((width > 0) && (height > 0)))
		return;

	// Rebuild the offscreen texture if needed
	bool hasTexture = rebuildTexture(cGfx);
	if (hasTexture)
	{
		blitTexture(cGfx);
	}
	else if (cGfx->getRenderBackend() == gfxpp::RENDER_BACKEND_OPENGL)
	{
		// Fallback: draw directly when render targets are unavailable.
#ifdef GFX_HAVE_OPENGL
		glPushMatrix();
		glTranslatef((float)getX(), (float)getY(), 0.0f);
		updateBGBackground(cGfx);
		updateBackground(cGfx);
		updateBorderBackground(cGfx);
		glPopMatrix();
#endif
	}

	// Go backwards for consistent z-order with panels and layouts
	for (int i = subitems.size() - 1; i >= 0; --i)
		subitems[i]->updateBackgroundHelper(cGfx);
}

void RUComponent::hover(gfxpp* cGfx)
{
	//
}

void RUComponent::unhover(gfxpp* cGfx)
{
	//
}

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

	// Pass on the event to the subcomps
	clickedSubItems.clear();

	// Optimize mouse motion: only process the previously-hovered child and the current target.
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

	for (unsigned int i = 0; i < subitems.size(); ++i)
	{
		EventTracker* subEventsStatus =
			subitems[i]->processEvents(cGfx, parentPanel, event, mouseX, mouseY);
		if (subEventsStatus->hovered)
			eventsStatus->hovered = true;

		if (subEventsStatus->downClicked)
		{
			eventsStatus->downClicked = true;
			clickedSubItems.insert(std::pair<int, GItem*>(subitems[i]->getID(), subitems[i]));
		}
	}
}

void RUComponent::updateBackgroundHelper(gfxpp* cGfx)
{
	if (!cGfx->getDraw())
		return;

	if (!visible)
		return;

	if (!((width > 0) && (height > 0)))
		return;

	if (getDrawUpdateRequired())
	{
		drawUpdate = false;

		// Drop any cached background texture and rebuild it.
		if (background)
			GFX_DestroyTexture(background);
		background = NULL;

		// Preferred path (SDL + OpenGL-with-FBO): render component once into an offscreen texture.
		background = cGfx->getDraw()->createRenderTargetTexture(width, height);
		if (background)
		{
			cGfx->getDraw()->setTargetTexture(background);
			cGfx->getDraw()->setTextureBlendMode(background, GFX_BLENDMODE_BLEND);
			cGfx->getDraw()->setDrawColor(0, 0, 0, 0);
			cGfx->getDraw()->clear();

			updateBGBackground(cGfx);
			updateBackground(cGfx);
			updateBorderBackground(cGfx);

			cGfx->getDraw()->resetTarget();
		}
		else
		{
			// Fallback: draw directly when render targets are unavailable.
			if (cGfx->getRenderBackend() == gfxpp::RENDER_BACKEND_OPENGL)
			{
#ifdef GFX_HAVE_OPENGL
				glPushMatrix();
				glTranslatef((float)getX(), (float)getY(), 0.0f);
				updateBGBackground(cGfx);
				updateBackground(cGfx);
				updateBorderBackground(cGfx);
				glPopMatrix();
#endif
			}
		}
	}

	// set the background rect
	GfxRect fullRect;
	fullRect.x = 0;
	fullRect.y = 0;
	fullRect.w = width;
	fullRect.h = height;

	// draw the background
	GfxRect dRect = getLocationRect();
	dRect.x = getX();
	dRect.y = getY();
	GfxTexture* geBackground = getBackground();
	if (geBackground && cGfx->getDraw())
		cGfx->getDraw()->copyTexture(geBackground, NULL, &dRect);
	else if (cGfx->getRenderBackend() == gfxpp::RENDER_BACKEND_OPENGL)
	{
#ifdef GFX_HAVE_OPENGL
		// No cached texture available: draw directly.
		glPushMatrix();
		glTranslatef((float)getX(), (float)getY(), 0.0f);
		updateBGBackground(cGfx);
		updateBackground(cGfx);
		updateBorderBackground(cGfx);
		glPopMatrix();
#endif
	}

	for (unsigned int i = 0; i < subitems.size(); ++i)
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

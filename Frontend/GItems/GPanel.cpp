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
#include "GPanel.h"
#include "../GFXUtilities/EventTracker.h"
#include "../GUI/RUMsgBox.h"
#include "../GUI/Text/RUTextComponent.h"
#include "../Graphics/graphics.h"
#include "Backend/Networking/main.h"
#include "GItem.h"
#include "GLayout.h"
#include "Mini/RUBackgroundComponent.h"
#include "Mini/RUBorderComponent.h"
#include "RUColors.h"
#include "RUComponent.h"

GPanel::GPanel(const shmea::GString& newName, int newWidth, int newHeight)
{
	name = newName;
	width = newWidth;
	height = newHeight;
	focus = false;
	setBGColor(RUColors::DEFAULT_COLOR_BACKGROUND);

	qMutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_init(qMutex, NULL);
}

GPanel::~GPanel()
{
	pthread_mutex_destroy(qMutex);
	if (qMutex)
		free(qMutex);
}

void GPanel::onShow(gfxpp* cGfx)
{
	focus = true;
	cGfx->focusedPanel = this;
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

void GPanel::calculateSubItemPositions(std::pair<int, int> parentOffset)
{
	// We just pass in manual X/Y on subitem creation
	for (unsigned int i = 0; i < subitems.size(); ++i)
	{
		GItem* cItem = subitems[i];
		if (cItem == NULL)
			continue;

		// draw the item
		cItem->calculateSubItemPositions(parentOffset);
	}
}

void GPanel::processSubItemEvents(gfxpp* cGfx, EventTracker* eventsStatus, GPanel* parentPanel,
								  SDL_Event event, int mouseX, int mouseY)
{
	if (!cGfx)
		return;

	if (!focus)
		return;

	bool hovered = false;
	for (unsigned int i = 0; i < subitems.size(); ++i)
	{
		GItem* cItem = subitems[i];
		if (!cItem)
			continue;

		EventTracker* eventsStatus = cItem->processEvents(cGfx, this, event, mouseX, mouseY);
		if (eventsStatus->hovered)
			hovered = true;
	}

	if (!hovered)
	{
		// Set the default cursor
		SDL_Cursor* renderCursor = cGfx->getSystemCursor();
		SDL_SetCursor(renderCursor);
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
	pthread_mutex_lock(qMutex);
	updateQueue.push(cData);
	pthread_mutex_unlock(qMutex);
}

void GPanel::popQ()
{
	const shmea::ServiceData* cData;
	pthread_mutex_lock(qMutex);
	cData = updateQueue.front();
	updateQueue.pop();
	pthread_mutex_unlock(qMutex);

	updateFromQ(cData);
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

	if (!cGfx->getRenderer())
		return;

	if (!visible)
		return;

	if (!((width > 0) && (height > 0)))
		return;

	// Update the GUI based on a message queue
	processQ(cGfx);

	// Do we want to redraw the panel
	if (getDrawUpdateRequired())
	{
		drawUpdate = false;

		// reset the backgrounds
		if (background)
			SDL_DestroyTexture(background);
		background = NULL;

		// draw the background
		background = SDL_CreateTexture(cGfx->getRenderer(), SDL_PIXELFORMAT_RGBA8888,
									   SDL_TEXTUREACCESS_TARGET, width, height);
		if (!background)
		{
			background = NULL;
			return;
		}

		SDL_SetRenderTarget(cGfx->getRenderer(), background);
		SDL_SetTextureBlendMode(background, SDL_BLENDMODE_BLEND);

		// draw the background
		SDL_SetRenderTarget(cGfx->getRenderer(), background);
		updateBGBackground(cGfx);

		// Call the component draw call
		SDL_SetRenderTarget(cGfx->getRenderer(), background);
		updateBackground(cGfx);

		// draw the border
		SDL_SetRenderTarget(cGfx->getRenderer(), background);
		updateBorderBackground(cGfx);

		// reset the render target to default
		SDL_SetRenderTarget(cGfx->getRenderer(), NULL);
	}

	// draw the background
	SDL_Rect dRect = getLocationRect();
	dRect.x = getX();
	dRect.y = getY();
	SDL_Texture* geBackground = getBackground();
	if (geBackground)
		SDL_RenderCopy(cGfx->getRenderer(), geBackground, NULL, &dRect);

	/*
	// Setup the render vector
	std::vector<GItem*> sortedRenderItems;
	if (subitems.size() > 0)
		sortedRenderItems.push_back(subitems[0]);

	// insertion sort the render items
	for (int i = 1; i < subitems.size(); ++i)
	{
		int cZIndex = subitems[i]->getZIndex();
		bool addedSubItem = false;
		for (int j = 0; j < sortedRenderItems.size(); ++j)
		{
			if (cZIndex >= sortedRenderItems[j]->getZIndex())
			{
				sortedRenderItems.insert(sortedRenderItems.begin() + j, subitems[i]);
				addedSubItem = true;
				break;
			}
		}

		if (!addedSubItem)
			sortedRenderItems.insert(sortedRenderItems.begin(), subitems[i]);
	}*/

	// Go backwards because of dropdowns
	for (int i = subitems.size() - 1; i >= 0; --i)
		subitems[i]->updateBackgroundHelper(cGfx);
}

void GPanel::updateBackground(gfxpp* cGfx)
{
	// set the background rect
	SDL_Rect fullRect;
	fullRect.x = 0;
	fullRect.y = 0;
	fullRect.w = width;
	fullRect.h = height;

	drawVerticalGradient(cGfx->getRenderer(), fullRect, RUColors::COLOR_DARK_GRAY, RUColors::COLOR_BLACK, 0);
}

shmea::GString GPanel::getType() const
{
	return "GPanel";
}

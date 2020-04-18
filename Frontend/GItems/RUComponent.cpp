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

#include "RUComponent.h"
#include "../GFXUtilities/EventTracker.h"
#include "Mini/RUBackgroundComponent.h"
#include "Mini/RUBorderComponent.h"

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
									   SDL_Event event, int mouseX, int mouseY)
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

void RUComponent::updateBackgroundHelper(SDL_Renderer* renderer)
{
	if (!renderer)
		return;

	if (!visible)
		return;

	if (!((width > 0) && (height > 0)))
		return;

	if (getDrawUpdateRequired())
	{
		drawUpdate = false;

		// draw the new background
		if (!background)
			background = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
										   SDL_TEXTUREACCESS_TARGET, width, height);

		// still?
		if (!background)
		{
			background = NULL;
			return;
		}

		// Assign the background as the render target and reset the background
		SDL_SetRenderTarget(renderer, background);
		SDL_RenderClear(renderer);
		SDL_SetTextureBlendMode(background, SDL_BLENDMODE_BLEND);

		// draw the background
		updateBGBackground(renderer);

		// Call the component draw call
		updateBackground(renderer);

		// draw the border
		updateBorderBackground(renderer);

		// Reset the render target to default
		SDL_SetRenderTarget(renderer, NULL);
	}

	// draw the background
	SDL_Rect dRect = getLocationRect();
	dRect.x = getX();
	dRect.y = getY();
	SDL_Texture* geBackground = getBackground();
	if (geBackground)
		SDL_RenderCopy(renderer, geBackground, NULL, &dRect);

	for (unsigned int i = 0; i < subitems.size(); ++i)
		subitems[i]->updateBackgroundHelper(renderer);
}

void RUComponent::hover(gfxpp* cGfx)
{
	//
}

void RUComponent::unhover(gfxpp* cGfx)
{
	//
}

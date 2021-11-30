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

#include "GItem.h"
#include "../GFXUtilities/EventTracker.h"
#include "../Graphics/graphics.h"
#include "GPanel.h"
#include "RUColors.h"

GItem::GItem()
{
	id = 0;
	name = "";
	background = NULL;
	zindex = -2;

	eventsStatus = new EventTracker();

	// Ready
	visible = true;
	drawUpdate = true;
}

GItem::GItem(int newX, int newY, int newWidth, int newHeight)
{
	id = 0;
	name = "";
	background = NULL;
	zindex = -2;

	eventsStatus = new EventTracker();

	// Ready
	visible = true;
	drawUpdate = true;
}

GItem::~GItem()
{
	eventsStatus = NULL;

	if(background)
		SDL_DestroyTexture(background);
	background = NULL;
	//Do more here?
}

int GItem::getID() const
{
	return id;
}

std::string GItem::getName() const
{
	return name;
}

GItem* GItem::getItemByID(int compID)
{
	for (unsigned int i = 0; i < subitems.size(); ++i)
	{
		if (subitems[i]->getID() == compID)
		{
			return subitems[i];
		}
	}
	return NULL;
}

GItem* GItem::getItemByName(const std::string& compName)
{
	for (unsigned int i = 0; i < subitems.size(); ++i)
	{
		if (subitems[i]->getName() == compName)
		{
			return subitems[i];
		}
	}
	return NULL;
}

unsigned int GItem::getZIndex() const
{
	return zindex;
}

SDL_Texture* GItem::getBackground()
{
	return background;
}

std::vector<GItem*> GItem::getItems() const
{
	return subitems;
}

void GItem::setID(int newID)
{
	id = newID;
}

void GItem::setName(const std::string& newName)
{
	name = newName;
}

void GItem::setWidth(int newWidth)
{
	// Overload
	width = newWidth;
	refreshImage();
	drawUpdate = true;
}

void GItem::setHeight(int newHeight)
{
	// Overload
	height = newHeight;
	refreshImage();
	drawUpdate = true;
}

void GItem::setZIndex(unsigned int newZIndex)
{
	zindex = newZIndex;
}

/*void GItem::setBGImageFromLocation(const std::string& newBGImageLocation)
{
	if (!bgComp)
		return;

	bgComp->setBGImageFromLocation(newBGImageLocation);
	if (newBGImageLocation.length() > 0)
		drawUpdate = true;
}*/

void GItem::addSubItem(GItem* newItem, unsigned int newZIndex)
{
	if (!newItem)
		return;

	// Back of screen or front of screen (opposite of the order)
	setZIndex(newZIndex);
	if (newZIndex == Z_BACK)
	{
		// Add to back of subitems
		subitems.insert(subitems.begin(), newItem);
	}
	else if (newZIndex == Z_FRONT)
	{
		// Add to front of subitems
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

void GItem::removeItem(gfxpp* cGfx, int itemID)
{
	for (unsigned int i = 0; i < subitems.size(); ++i)
	{
		if (subitems[i]->getID() == itemID)
		{
			subitems.erase(subitems.begin() + i); // Remove item from this layout
			cGfx->removeItem(itemID);			  // Remove from master vector of GUI items
			break;
		}
	}
}

void GItem::removeItem(gfxpp* cGfx, const std::string& itemName)
{
	if (itemName == "")
		return;

	for (unsigned int i = 0; i < subitems.size(); ++i)
	{
		if (subitems[i]->getName() == itemName)
		{
			subitems.erase(subitems.begin() + i);	// Remove item from this layout
			cGfx->removeItem(subitems[i]->getID()); // Remove from master vector of GUI items
			break;
		}
	}
}

/*!
 * @brief clear some or all sub components
 * @details clears the sub components, optionally saves the first `numToSave` of them
 * @param numToSave the number of components to save from the beginning of the list. Default to 0.
 */
void GItem::clearItems(unsigned int numToSave)
{
	if (numToSave == 0)
		subitems.clear();
	else if (numToSave < subitems.size())
		subitems.resize(numToSave);
	drawUpdate = true;
}

EventTracker* GItem::processEvents(gfxpp* cGfx, GPanel* parentPanel, SDL_Event event, int mouseX,
								   int mouseY)
{
	eventsStatus->reset();

	if (!cGfx)
		return eventsStatus;

	if (!parentPanel)
		return eventsStatus;

	if (!visible)
		return eventsStatus;

	//
	processSubItemEvents(cGfx, eventsStatus, parentPanel, event, mouseX, mouseY);

	// Dont want to toggle dropdowns all the time
	bool dropdownToggle = (getType() == "RUDropdown");
	if (dropdownToggle)
	{
		if (clickedSubItems.size() == 0)
			dropdownToggle = false;
	}

	//
	if (event.type == SDL_MOUSEBUTTONDOWN)
		onMouseDownHelper(cGfx, eventsStatus, parentPanel, mouseX, mouseY, dropdownToggle);
	else if (event.type == SDL_MOUSEBUTTONUP)
		onMouseUpHelper(cGfx, eventsStatus, parentPanel, mouseX, mouseY, dropdownToggle);
	else if (event.type == SDL_MOUSEMOTION)
	{
		onMouseMotionHelper(cGfx, eventsStatus, parentPanel, mouseX, mouseY, dropdownToggle);

		// hover and unhover subevents
		if (eventsStatus->hovered)
			hover(cGfx);
		else
		{
			if (!unhovered)
			{
				// Dont want to override subcomps cursor
				if (customCursor)
				{
					// Set the cursor
					SDL_Cursor* renderCursor = cGfx->getSystemCursor();
					SDL_SetCursor(renderCursor);
				}

				unhover(cGfx);
				unhovered = true;
			}
		}
	}
	else if (event.type == SDL_MOUSEWHEEL)
	{
		if (event.wheel.y > 0)
		{
			// Scroll down
			onMouseWheelHelper(cGfx, eventsStatus, parentPanel, mouseX, mouseY, SCROLL_DOWN,
							   dropdownToggle);
		}
		else if (event.wheel.y < 0)
		{
			// Scroll up
			onMouseWheelHelper(cGfx, eventsStatus, parentPanel, mouseX, mouseY, SCROLL_UP,
							   dropdownToggle);
		}

		if (event.wheel.x > 0)
		{
			// Scroll right
			onMouseWheelHelper(cGfx, eventsStatus, parentPanel, mouseX, mouseY, SCROLL_RIGHT,
							   dropdownToggle);
		}
		else if (event.wheel.x < 0)
		{
			// Scroll left
			onMouseWheelHelper(cGfx, eventsStatus, parentPanel, mouseX, mouseY, SCROLL_LEFT,
							   dropdownToggle);
		}
	}
	else if (event.type == SDL_KEYDOWN)
	{
		SDL_Keycode keyPressed = event.key.keysym.sym;
		Uint16 keyModPressed = event.key.keysym.mod;
		// Send a key press to the focused ui element
		onKeyDownHelper(cGfx, eventsStatus, parentPanel, keyPressed, keyModPressed);
	}
	else if (event.type == SDL_KEYUP)
	{
		SDL_Keycode keyPressed = event.key.keysym.sym;
		Uint16 keyModPressed = event.key.keysym.mod;
		// Send a key release to the focused ui element
		onKeyUpHelper(cGfx, eventsStatus, parentPanel, keyPressed, keyModPressed);
	}

	return eventsStatus;
}

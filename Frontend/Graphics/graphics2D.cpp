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
#include "../GItems/GItem.h"
#include "../GItems/GLayout.h"
#include "../GItems/RUComponent.h"
#include "../GUI/Text/RUTextComponent.h"
#include "graphics.h"

unsigned int gfxpp::RGBfromHue(double hue, int8_t* r, int8_t* g, int8_t* b)
{
	int h = int(hue * 256 * 6);
	int x = h % 0x100;

	(*r) = 0;
	(*g) = 0;
	(*b) = 0;
	switch (h / 256)
	{
	case 0:
		(*r) = 0xFF;
		(*g) = x;
		break;
	case 1:
		(*g) = 0xFF;
		(*r) = 0xFF - x;
		break;
	case 2:
		(*g) = 0xFF;
		(*b) = x;
		break;
	case 3:
		(*b) = 0xFF;
		(*g) = 0xFF - x;
		break;
	case 4:
		(*b) = 0xFF;
		(*r) = x;
		break;
	case 5:
		(*r) = 0xFF;
		(*b) = 0xFF - x;
		break;
	}

	return (*r) + ((*g) << 8) + ((*b) << 16);
}

SDL_Cursor* gfxpp::getSystemCursor()
{
	return systemCursor;
}

void gfxpp::addGradient(int x, int y, int size)
{
	// check the renderer
	if (!renderer)
	{
		printf("[GFX] Renderer error: %s\n", SDL_GetError());
		return;
	}

	/*for (int i = (-(size / 2)); i < size / 2; ++i)
	{
		for (int j = (-(size / 2)); j < size / 2; ++j)
		{
			// calculate the hue
			double hue = ((double)((i * i) + (j * j))) / ((double)(size * size));

			// get the color
			int8_t redMask = 0;
			int8_t greenMask = 0;
			int8_t blueMask = 0;
			unsigned int colorMask = RGBfromHue(hue, &redMask, &greenMask, &blueMask);

			// set the color and draw the point
			SDL_SetRenderDrawColor(renderer, redMask, greenMask, blueMask, SDL_ALPHA_OPAQUE);
			SDL_RenderDrawPoint(renderer, x + i, y + j);
		}
	}*/
}

void gfxpp::addItem(GItem* newItem)
{
	if (!newItem)
		return;

	const shmea::GString options = "0123456789";
	const int keyLength = 6;
	shmea::GString newItemID = "";
	int itemID = 0;

	do
	{
		newItemID = "";
		for (int i = 0; i < keyLength; ++i)
		{
			int newIndex = rand() % options.length();
			char newChar = options[newIndex];
			newItemID += newChar;
		}

		itemID = atoi(newItemID.c_str());

	} while (getItemByID(itemID)); // Generate new item id if already used

	// Assign same id to object of GItem and add it
	newItem->setID(itemID);
	guiElements.push_back(newItem);
}

// Unique ID for each item, but names may be the same.
void gfxpp::removeItem(int itemID)
{
	if (!itemID)
		return;

	for (unsigned int i = 0; i < guiElements.size(); ++i)
	{
		if (guiElements[i]->getID() == itemID)
		{
			guiElements.erase(guiElements.begin() + i);
			break;
		}
	}
}

// Unique ID for each item, but names may be the same.
GItem* gfxpp::getItemByID(int itemID)
{
	for (unsigned int i = 0; i < guiElements.size(); ++i)
	{
		if (guiElements[i]->getID() == itemID)
			return guiElements[i];
	}
	return NULL;
}

void gfxpp::setFocus(GItem* newFocusedItem)
{
	if (!newFocusedItem)
		return;

	if (focusedItem)
	{
		focusedItem->unsetFocus();
		if (focusedPanel)
			focusedItem->triggerLoseFocusEvent(focusedPanel);
	}

	focusedItem = newFocusedItem;
	focusedItem->setFocus();
}

int gfxpp::getWidth() const
{
	return width;
}

int gfxpp::getHeight() const
{
	return height;
}

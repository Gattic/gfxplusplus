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
#include "RUListbox.h"
#include "../GItems/RUColors.h"
#include "RUScrollbar.h"
#include "Text/RULabel.h"

RUListbox::RUListbox()
{
	toggleBorder(true);
	multiSelectEnabled = true;
	optionsShown = 0;
	itemHovered = (unsigned int)-1;

	// add the scrollbar
	scrollbar = new RUScrollbar();
	scrollbar->setWidth(DEFAULT_SCROLLBAR_WIDTH);
	scrollbar->setOptionsShown(0);
	scrollbar->setVisible(false);
	addSubItem(scrollbar);
}

RUListbox::~RUListbox()
{
	toggleBorder(true);
	optionsShown = 0;
	itemsSelected.clear();

	// delete the scrollbar
	if (scrollbar)
		delete scrollbar;
	scrollbar = NULL;

	// delete the labels
	for (unsigned int i = 0; i < items.size(); ++i)
	{
		if (items[i])
			delete items[i];
		items[i] = NULL;
	}
}

bool RUListbox::getMultiSelect() const
{
	return multiSelectEnabled;
}

unsigned int RUListbox::getOptionsShown() const
{
	return optionsShown;
}

const std::vector<unsigned int> RUListbox::getItemsSelected() const
{
	return itemsSelected;
}

unsigned int RUListbox::size() const
{
	return items.size();
}

void RUListbox::setWidth(int newWidth)
{
	if (!scrollbar)
		return;

	width = newWidth;

	// update the labels
	int newLabelWidth = getWidth();
	if (items.size() + 1 > optionsShown)
	{
		//
		newLabelWidth -= scrollbar->getWidth();
		for (unsigned int i = 0; i < items.size(); ++i)
			items[i]->setWidth(newLabelWidth);
	}

	// Some basic scrollbar width behavior
	if (getWidth() <= DEFAULT_SCROLLBAR_WIDTH * 2)
	{
		scrollbar->setWidth(getWidth() / 15);
		scrollbar->setMarginX(getWidth() - scrollbar->getWidth());
	}
	else
	{
		scrollbar->setWidth(DEFAULT_SCROLLBAR_WIDTH);
		scrollbar->setMarginX(getWidth() - scrollbar->getWidth());
	}

	drawUpdate = true;
}

void RUListbox::setHeight(int newHeight)
{
	if (!scrollbar)
		return;

	height = newHeight;

	// update the labels
	for (unsigned int i = 0; i < items.size(); ++i)
	{
		if (optionsShown > 0)
		{
			int labelHeight = getHeight() / optionsShown;
			items[i]->setMarginY(i * labelHeight);
			items[i]->setHeight(labelHeight);
			items[i]->setFontSize(labelHeight / 2);
		}
		else
		{
			items[i]->setMarginY(0);
			items[i]->setHeight(0);
			items[i]->setFontSize(0);
		}
	}

	// update the scrollbar
	scrollbar->setHeight(getHeight());

	drawUpdate = true;
}

void RUListbox::setMultiSelect(bool newMultiSelectEnabled)
{
	multiSelectEnabled = newMultiSelectEnabled;
}

void RUListbox::setOptionsShown(unsigned int newOptionsShown)
{
	optionsShown = newOptionsShown;

	// update the scrollbar
	scrollbar->setOptionsShown(optionsShown);

	drawUpdate = true;
}

void RUListbox::addOption(std::string newItemText)
{
	int newLabelWidth = getWidth();
	if (items.size() + 1 > optionsShown)
	{
		//
		newLabelWidth -= scrollbar->getWidth();
		for (unsigned int i = 0; i < items.size(); ++i)
			items[i]->setWidth(newLabelWidth);
	}

	// create the new item label
	RULabel* newLabel = new RULabel(newItemText);
	newLabel->setMarginX(0);
	newLabel->setWidth(newLabelWidth);
	newLabel->setCursor(SDL_SYSTEM_CURSOR_HAND);
	newLabel->toggleBorder(true);
	if (optionsShown > 0)
	{
		unsigned int cIndex = scrollbar->getValue();
		int labelHeight = getHeight() / optionsShown;
		newLabel->setMarginY(items.size() * labelHeight);
		newLabel->setHeight(labelHeight);
		newLabel->setFontSize(labelHeight / 2);
		newLabel->setVisible((items.size() >= cIndex) && (items.size() - cIndex < optionsShown));
	}
	else
	{
		newLabel->setMarginY(0);
		newLabel->setHeight(0);
		newLabel->setFontSize(0);
		newLabel->setVisible(false);
	}

	// add the label
	items.push_back(newLabel);
	addSubItem(newLabel);
	scrollbar->setMaxValue(items.size());
	if (items.size() > optionsShown)
		scrollbar->setVisible(true);

	// single select
	if ((!multiSelectEnabled) && (itemsSelected.size() == 1))
		itemsSelected.push_back(0);

	drawUpdate = true;
}

void RUListbox::addSelection(unsigned int newIndex)
{
	if ((newIndex != (unsigned int)-1) && (newIndex >= size()))
		return;

	// single select
	if ((!multiSelectEnabled) && (itemsSelected.size() > 0))
		clearSelections();

	bool itemFound = false;
	int insertIndex = itemsSelected.size();
	for (unsigned int i = 0; i < itemsSelected.size(); ++i)
	{
		if (itemsSelected[i] == newIndex)
		{
			itemsSelected.erase(itemsSelected.begin() + i);
			itemFound = true;
			// unhighlight label
			items[itemsSelected[i]]->setBGColor(RUColors::DEFAULT_COLOR_BACKGROUND);
			break;
		}
		else if (itemsSelected[i] > newIndex)
		{
			insertIndex = i;
			break;
		}
	}

	// insert the item
	if (!itemFound)
	{
		itemsSelected.insert(itemsSelected.begin() + insertIndex, newIndex);
		// highlight label
		items[itemsSelected[insertIndex]]->setBGColor(RUColors::DEFAULT_BUTTON_HOVER_BLUE);
	}
}

void RUListbox::clearSelections()
{
	itemsSelected.clear();
	drawUpdate = true;
}

/*void RUListbox::clear()
{
	clearSelections();
	clearOptions();
	clearItems();
	scrollbar->setVisible(false);
	addSubItem(scrollbar);
	drawUpdate = true;
}*/

/*!
 * @brief remove all items
 * @details clear all labels from the list
 */
void RUListbox::clearOptions()
{
	// Trim the rows
	subitems.clear();
	for (unsigned int i = 0; i < items.size(); ++i)
	{
		RULabel* cLabel = items[i];
		if (!cLabel)
			continue;

		cLabel->setVisible(false);
		delete cLabel;
	}
	items.clear();
	addSubItem(scrollbar);

	// reset the scrollbar position
	scrollbar->setValue(0);
	scrollbar->setMaxValue(0);
	updateLabels();
	itemsSelected.clear();
	clearItems(1);
	drawUpdate = true;
}

void RUListbox::updateBackground(SDL_Renderer* renderer)
{
	if (optionsShown > 0)
	{
		for (unsigned int i = 0; i < items.size(); ++i)
		{
			// Check if selected item
			bool isSelected = false;
			for (unsigned int j = 0; j < itemsSelected.size(); ++j)
			{
				// Don't do anything to keep existing highlight
				if (i == itemsSelected[j])
				{
					isSelected = true;
					break;
				}
			}

			if (!isSelected)
			{
				// Add/Remove highlight if not selected
				if (i == itemHovered)
					items[itemHovered]->setBGColor(RUColors::DEFAULT_BUTTON_BLUE);
				else
					items[i]->setBGColor(RUColors::DEFAULT_COLOR_BACKGROUND);
			}
		}
	}
}

/*!
 * @brief update visible labels
 * @details update the label visibility based on the scrollbar position
 */
void RUListbox::updateLabels()
{
	unsigned int cIndex = scrollbar->getValue();
	for (unsigned int i = 0; i < items.size(); ++i)
	{
		if (optionsShown > 0)
		{
			int labelHeight = getHeight() / optionsShown;
			items[i]->setMarginY((i - cIndex) * labelHeight);
			items[i]->setHeight(labelHeight);
			items[i]->setFontSize(labelHeight / 2);
			items[i]->setVisible((i >= cIndex) && (i - cIndex < optionsShown));
		}
		else
		{
			items[i]->setMarginY(0);
			items[i]->setHeight(0);
			items[i]->setFontSize(0);
			items[i]->setVisible(false);
		}
	}

	std::pair<int, int> offset(0, 0);
	calculateSubItemPositions(offset);

	drawUpdate = true;
}

void RUListbox::onMouseMotion(gfxpp* cGfx, GPanel* cPanel, int eventX, int eventY)
{
	// selecting listbox items
	if (optionsShown > 0)
	{
		int scrollbarVal = scrollbar->getValue();
		int labelHeight = getHeight() / optionsShown;
		itemHovered = eventY - (eventY % labelHeight);
		itemHovered /= labelHeight;
		itemHovered += scrollbarVal;
	}

	updateLabels();
}

void RUListbox::onMouseDown(gfxpp* cGfx, GPanel* cPanel, int eventX, int eventY)
{
	// printf("RUListbox: onMouseDown(%d, %d);\n", eventX, eventY);

	// scrollbar click
	if ((eventX >= scrollbar->getMarginX()) && (eventX < getWidth()))
	{
		updateLabels();
		return;
	}

	// selecting listbox items
	if (optionsShown > 0)
	{
		int scrollbarVal = scrollbar->getValue();
		int labelHeight = getHeight() / optionsShown;
		unsigned int itemClicked = eventY - (eventY % labelHeight);
		itemClicked /= labelHeight;
		itemClicked += scrollbarVal;
		if (itemClicked >= items.size())
			return;

		// single select
		if ((!multiSelectEnabled) && (itemsSelected.size() > 0))
			clearSelections();

		bool itemFound = false;
		unsigned int insertIndex = itemsSelected.size();
		for (unsigned int i = 0; i < itemsSelected.size(); ++i)
		{
			if (itemsSelected[i] == itemClicked)
			{
				itemsSelected.erase(itemsSelected.begin() + i);
				itemFound = true;
				// unhighlight label here
				items[itemClicked]->setBGColor(RUColors::DEFAULT_COLOR_BACKGROUND);
				break;
			}
			else if (itemsSelected[i] > itemClicked)
			{
				insertIndex = i;
				break;
			}
		}

		// insert the item
		if (!itemFound)
		{
			itemsSelected.insert(itemsSelected.begin() + insertIndex, itemClicked);
			// highlight label here
			items[itemClicked]->setBGColor(RUColors::DEFAULT_BUTTON_HOVER_BLUE);
		}
	}

	updateLabels();
}

void RUListbox::onMouseWheel(gfxpp* cGfx, GPanel* cPanel, int eventX, int eventY, int scrollType)
{
	// printf("RUListbox: onMouseWheel(%d);\n", scrollType);

	// scrolling down on the labels
	if (!(eventX >= scrollbar->getMarginX() && eventX < getWidth()))
	{
		if (scrollType == SCROLL_DOWN)
			scrollbar->decrement();
		else if (scrollType == SCROLL_UP)
			scrollbar->increment();

		updateLabels();
	}
}

std::string RUListbox::getType() const
{
	return "RUListbox";
}

void RUListbox::unhover()
{
	if (itemHovered != (unsigned int)-1)
	{
		itemHovered = (unsigned int)-1;
		drawUpdate = true;
	}
}

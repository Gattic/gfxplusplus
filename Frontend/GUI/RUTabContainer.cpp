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
#include "RUTabContainer.h"
#include "../GItems/RUColors.h"
#include "Text/RULabel.h"

RUTabContainer::RUTabContainer()
{
	orientation = HORIZONTAL_TABS;
	optionsShown = 0;
	itemHovered = (unsigned int)-1;
}

RUTabContainer::~RUTabContainer()
{
	// delete the labels
	for (unsigned int i = 0; i < items.size(); ++i)
	{
		if (items[i].first)
			delete items[i].first;
		items[i].first = NULL;

		for (int j = 0; j < items[i].second.size(); ++j)
		{
			if (items[i].second[j])
				delete items[i].second[j];
			items[i].second[j] = NULL;
		}
	}

	optionsShown = 0;
	prevTabSelected = -1;
	tabSelected = -1;
}

unsigned int RUTabContainer::getOptionsShown() const
{
	return optionsShown;
}

unsigned int RUTabContainer::getTabSelected()
{
	return tabSelected;
}

unsigned int RUTabContainer::size() const
{
	return items.size();
}

void RUTabContainer::setWidth(int newWidth)
{
	width = newWidth;

	// update the labels
	if (items.size() + 1 > optionsShown)
	{
		for (unsigned int i = 0; i < items.size(); ++i)
			items[i].first->setWidth(width);
	}

	drawUpdate = true;
}

void RUTabContainer::setHeight(int newHeight)
{
	height = newHeight;

	// update the labels
	for (unsigned int i = 0; i < items.size(); ++i)
	{
		if (optionsShown > 0)
		{
			int labelHeight = getHeight() / optionsShown;
			// items[i]->setMarginY(i * labelHeight);
			items[i].first->setHeight(labelHeight);
			items[i].first->setFontSize(labelHeight / 2);
		}
		else
		{
			items[i].first->setMarginY(0);
			items[i].first->setHeight(0);
			items[i].first->setFontSize(0);
		}
	}

	drawUpdate = true;
}

void RUTabContainer::setOptionsShown(unsigned int newOptionsShown)
{
	optionsShown = newOptionsShown;
	setWidth((width * optionsShown) + (getPaddingX() * optionsShown));
	drawUpdate = true;
}

void RUTabContainer::addTab(std::string newItemText)
{
	// add the label
	int newLabelWidth = (getWidth() - (getPaddingX() * optionsShown)) / optionsShown;
	if (items.size() + 1 > optionsShown)
	{
		for (unsigned int i = 0; i < items.size(); ++i)
			items[i].first->setWidth(newLabelWidth);
	}

	// create the new item label
	RULabel* newLabel = new RULabel(newItemText);
	newLabel->setMarginX(0);
	newLabel->setWidth(newLabelWidth);
	newLabel->setCursor(SDL_SYSTEM_CURSOR_HAND);
	newLabel->toggleBorder(true);
	if (optionsShown > 0)
	{
		int labelWidth = (getWidth() - (getPaddingX() * optionsShown)) / optionsShown;
		int labelHeight = getHeight();
		if (items.size() > 0)
			newLabel->setMarginX(items.size() * labelWidth + (items.size() * getPaddingX()));

		// newLabel->setMarginY(items.size() * labelHeight);
		newLabel->setHeight(labelHeight);
		newLabel->setFontSize(labelHeight / 2);
		newLabel->setVisible(items.size() < optionsShown);
	}
	else
	{
		newLabel->setMarginY(0);
		newLabel->setHeight(0);
		newLabel->setFontSize(0);
		newLabel->setVisible(false);
	}

	// add the label
	std::pair<RULabel*, std::vector<GItem*> > newItem;
	newItem.first = newLabel;
	items.push_back(newItem);
	addSubItem(newLabel);

	// Auto select first added tab
	if (items.size() == 1)
	{
		prevTabSelected = tabSelected;
		tabSelected = 0;
		items[0].first->setBGColor(RUColors::DEFAULT_BUTTON_HOVER_BLUE);
	}

	drawUpdate = true;
}

void RUTabContainer::addItemToTab(std::string tabText, GItem* tabItem)
{

	for (unsigned int i = 0; i < items.size(); ++i)
	{
		if (items[i].first->getText() == tabText)
		{
			items[i].second.push_back(tabItem);
			drawUpdate = true;
			return;
		}
	}
	printf("addItemToTab() failed. Tab with text '%s' not found.\n", tabText.c_str());
}

/*!
 * @brief remove all items
 * @details clear all labels from the list
 */
void RUTabContainer::clearOptions()
{
	// Trim the rows
	subitems.clear();
	for (unsigned int i = 0; i < items.size(); ++i)
	{
		RULabel* cLabel = items[i].first;
		if (!cLabel)
			continue;

		cLabel->setVisible(false);
		delete cLabel;
	}
	items.clear();

	updateLabels();
	prevTabSelected = tabSelected;
	tabSelected = -1;
	clearItems(1);
	drawUpdate = true;
}

// int = index
void RUTabContainer::setSelectedTab(unsigned int newIndex)
{
	if (size() > 0)
	{
		if ((newIndex != (unsigned int)-1) && (newIndex < size()))
		{
			prevTabSelected = tabSelected;
			tabSelected = newIndex;
			items[tabSelected].first->setBGColor(RUColors::DEFAULT_BUTTON_HOVER_BLUE);
		}
		else
		{
			prevTabSelected = tabSelected;
			tabSelected = -1;
		}
	}
	else
	{
		prevTabSelected = tabSelected;
		tabSelected = -1;
	}

	drawUpdate = true;
}

// string = tab name
void RUTabContainer::setSelectedTab(std::string tabName)
{
	bool tabFound = false;
	if (size() > 0)
	{
		for (int i = 0; i < items.size(); ++i)
		{
			RULabel* tabItem = items[i].first;
			if (!tabItem)
				continue;

			if (tabItem->getText() == tabName)
			{
				tabFound = true;
				prevTabSelected = tabSelected;
				tabSelected = i;
				items[tabSelected].first->setBGColor(RUColors::DEFAULT_BUTTON_HOVER_BLUE);
			}
		}

		if (!tabFound)
		{
			prevTabSelected = tabSelected;
			tabSelected = -1;
		}
	}
	else
	{
		prevTabSelected = tabSelected;
		tabSelected = -1;
	}

	drawUpdate = true;
}

void RUTabContainer::setOptionChangedListener(void (GPanel::*f)(int))
{
	OptionChangedListener = f;
}

void RUTabContainer::updateBackground(SDL_Renderer* renderer)
{
	if (optionsShown > 0)
	{
		for (int i = 0; i < items.size(); ++i)
		{
			// If selected tab changed last frame...
			if (tabSelected != prevTabSelected)
			{
				if (i == tabSelected)
				{
					std::vector<GItem*> selectedTabContainer = items[i].second;
					// Show selected tab items
					for (int j = 0; j < selectedTabContainer.size(); ++j)
						selectedTabContainer[j]->setVisible(true);
				}
				else
				{
					// Add/Remove highlight if not selected
					if (i == itemHovered)
						items[i].first->setBGColor(RUColors::DEFAULT_BUTTON_BLUE);
					else
						items[i].first->setBGColor(RUColors::DEFAULT_COLOR_BACKGROUND);

					std::vector<GItem*> tabContainer = items[i].second;
					// Hide unselected tabs' GItems
					for (int j = 0; j < tabContainer.size(); ++j)
						tabContainer[j]->setVisible(false);
				}
			}
			else
			{
				// Check if not selected tab
				if (i != tabSelected)
				{
					// Add/Remove hover highlight
					if (i == itemHovered)
						items[i].first->setBGColor(RUColors::DEFAULT_BUTTON_BLUE);
					else
						items[i].first->setBGColor(RUColors::DEFAULT_COLOR_BACKGROUND);
				}
			}
		}
		prevTabSelected = tabSelected;
	}
}

/*!
 * @brief update visible labels
 * @details update the label visibility
 */
void RUTabContainer::updateLabels()
{
	for (unsigned int i = 0; i < items.size(); ++i)
	{
		if (optionsShown > 0)
		{
			int labelWidth = (getWidth() - (getPaddingX() * optionsShown)) / optionsShown;
			int labelHeight = getHeight();
			items[i].first->setHeight(labelHeight);
			items[i].first->setFontSize(labelHeight / 2);
			items[i].first->setVisible(i < optionsShown);
		}
		else
		{
			items[i].first->setMarginY(0);
			items[i].first->setHeight(0);
			items[i].first->setFontSize(0);
			items[i].first->setVisible(false);
		}
	}

	std::pair<int, int> offset(0, 0);
	calculateSubItemPositions(offset);

	drawUpdate = true;
}

void RUTabContainer::unhover()
{
	if (itemHovered != (unsigned int)-1)
	{
		itemHovered = (unsigned int)-1;
		drawUpdate = true;
	}
}

std::string RUTabContainer::getType() const
{
	return "RUTabContainer";
}

void RUTabContainer::onMouseDown(gfxpp* cGfx, GPanel* cPanel, int eventX, int eventY)
{
	// printf("RUTabContainer: onMouseDown(%d, %d);\n", eventX, eventY);

	// Ignore clicks on padding
	if (optionsShown > 0 && getPaddingX() > 0)
	{
		int labelWidth = (getWidth() - (getPaddingX() * optionsShown)) / optionsShown;
		for (unsigned int i = 1; i <= items.size(); ++i)
		{
			int paddingEnd = i * labelWidth + (i * getPaddingX());
			int paddingStart = paddingEnd - getPaddingX();
			// printf("eventX: %d; paddingStart: %d; paddingEnd: %d\n", eventX, paddingStart,
			// paddingEnd);
			if (eventX > paddingStart && eventX < paddingEnd)
				return;
		}
	}

	// selecting listbox items
	if (optionsShown > 0)
	{
		int labelWidth = getWidth() / optionsShown;
		unsigned int itemClicked = eventX - (eventX % labelWidth);
		itemClicked /= labelWidth;
		if (itemClicked >= items.size())
			return;

		// insert the item
		if (itemClicked != tabSelected)
		{
			// unhighlight current label selection
			items[itemClicked].first->setBGColor(RUColors::DEFAULT_COLOR_BACKGROUND);
			prevTabSelected = tabSelected;
			tabSelected = itemClicked;
			// highlight new label selection here
			items[tabSelected].first->setBGColor(RUColors::DEFAULT_BUTTON_HOVER_BLUE);
		}
	}

	updateLabels();
}

void RUTabContainer::onMouseMotion(gfxpp* cGfx, GPanel* cPanel, int eventX, int eventY)
{

	// Ignore clicks on padding
	if (optionsShown > 0 && getPaddingX() > 0)
	{
		int labelWidth = (getWidth() - (getPaddingX() * optionsShown)) / optionsShown;
		for (unsigned int i = 1; i <= items.size(); ++i)
		{
			int paddingEnd = i * labelWidth + (i * getPaddingX());
			int paddingStart = paddingEnd - getPaddingX();
			// printf("eventX: %d; paddingStart: %d; paddingEnd: %d\n", eventX, paddingStart,
			// paddingEnd);
			if (eventX > paddingStart && eventX < paddingEnd)
			{
				itemHovered = -1;
				return;
			}
		}
	}

	// selecting listbox items
	if (optionsShown > 0)
	{
		int labelWidth = getWidth() / optionsShown;
		itemHovered = eventX - (eventX % labelWidth);
		itemHovered /= labelWidth;
		// printf("\neventX: %d; labelWidth: %d; itemHovered: %d", eventX, labelWidth, itemHovered);
	}

	updateLabels();
}

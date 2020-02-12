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
#include "RUDropdown.h"
#include "../GItems/RUColors.h"
#include "RUImageComponent.h"
#include "RUListbox.h"
#include "RUScrollbar.h"
#include "Text/RULabel.h"

RUDropdown::RUDropdown()
{
	toggleBorder(true);
	open = false;
	OptionChangedListener = 0;

	// selected label
	selectedIndex = -1;
	prevSelectedIndex = -1;
	selectedLabel = new RULabel();
	selectedLabel->setBGColor(getBGColor());
	selectedLabel->toggleBorder(true);
	addSubItem(selectedLabel);

	// arrow
	arrowLocation = "resources/gui/Scrollbar/ArrowDown.bmp";
	arrow = new RUImageComponent(arrowLocation);
	arrow->setBorderColor(RUColors::DEFAULT_DROPDOWN_ARROW);
	arrow->toggleBorder(true);
	arrow->setCursor(SDL_SYSTEM_CURSOR_HAND);
	addSubItem(arrow);

	// lb dropdown
	lbItems = new RUListbox();
	lbItems->setMultiSelect(false);
	lbItems->setVisible(false);
	addSubItem(lbItems);
}

RUDropdown::~RUDropdown()
{
	open = false;
	arrowLocation = "";

	// delete the selection label
	if (selectedLabel)
		delete selectedLabel;
	selectedLabel = NULL;

	// delete the arrow
	if (arrow)
		delete arrow;
	arrow = NULL;

	// delete the listbox
	if (lbItems)
		delete lbItems;
	lbItems = NULL;
}

bool RUDropdown::isOpen() const
{
	return open;
}

unsigned int RUDropdown::getOptionsShown() const
{
	return lbItems->getOptionsShown();
}

unsigned int RUDropdown::getSelectedIndex()
{
	if (lbItems->size() > 0)
		selectedIndex = lbItems->getItemsSelected()[0];
	else
		selectedIndex = -1;

	return selectedIndex;
}
/*!
 * @brief get selected text
 * @details gets the text content of the selected label
 */
std::string RUDropdown::getSelectedText() const
{
	return selectedLabel->getText();
}

void RUDropdown::toggleOpen()
{
	open = !open;
	lbItems->setVisible(open);
	if (open)
		arrowLocation = "resources/gui/Scrollbar/ArrowUp.bmp";
	else
		arrowLocation = "resources/gui/Scrollbar/ArrowDown.bmp";
	arrow->setBGImageFromLocation(arrowLocation);

	drawUpdate = true;
}

void RUDropdown::setWidth(int newWidth)
{
	if (!arrow)
		return;

	if (!lbItems)
		return;

	width = newWidth;

	arrow->setWidth(DEFAULT_SIDE_WIDTH);

	// set the new x and width of the sub subcomps
	selectedLabel->setWidth(getWidth() * (13.0f / 15.0f));
	arrow->setMarginX(getWidth() - arrow->getWidth());
	lbItems->setWidth(getWidth());

	drawUpdate = true;
}

void RUDropdown::setHeight(int newHeight)
{
	if (!arrow)
		return;

	if (!lbItems)
		return;

	height = newHeight;

	// update the selected label
	selectedLabel->setHeight(getHeight());

	// update the arrow
	arrow->setHeight(getHeight());

	// update the listbox
	lbItems->setMarginY(height);
	lbItems->setHeight(height * getOptionsShown());

	drawUpdate = true;
}

void RUDropdown::setOptionsShown(unsigned int newOptionsShown)
{
	lbItems->setOptionsShown(newOptionsShown);
	lbItems->setHeight(height * getOptionsShown());

	drawUpdate = true;
}

void RUDropdown::setSelectedIndex(unsigned int newIndex)
{
	if (lbItems->size() > 0)
	{
		if ((newIndex != (unsigned int)-1) && (newIndex < lbItems->size()))
		{
			selectedIndex = newIndex;
			lbItems->addSelection(selectedIndex);
			selectedLabel->setText(lbItems->items[selectedIndex]->getText());
		}
		else
			selectedIndex = -1;
	}
	else
		selectedIndex = -1;

	drawUpdate = true;
}

void RUDropdown::addOption(std::string newItemText)
{
	// add the label
	lbItems->addOption(newItemText);

	// select the item if its the first
	if ((!lbItems->getMultiSelect()) && (lbItems->size() == 1))
	{
		selectedIndex = 0;
		selectedLabel->setText(lbItems->items[selectedIndex]->getText());
		lbItems->addSelection(selectedIndex);
	}

	drawUpdate = true;
}

void RUDropdown::clearOptions()
{
	if (open)
		toggleOpen();

	lbItems->clearOptions();
	selectedLabel->setText("");
	drawUpdate = true;
}

void RUDropdown::updateBackground(SDL_Renderer* renderer)
{
	//
}

void RUDropdown::setOptionChangedListener(void (GPanel::*f)(int))
{
	OptionChangedListener = f;
}

void RUDropdown::onMouseDown(GPanel* cPanel, int eventX, int eventY)
{
	// printf("RUDropdown: onMouseDown(%d, %d);\n", eventX, eventY);

	// open/close the listbox
	if (eventY <= getHeight())
	{
		// arrowdown click
		if ((eventX >= lbItems->scrollbar->getMarginX()) && (eventX < getWidth()))
			toggleOpen();
	}
	else
	{
		// not scrollbar click
		if (!((eventX >= lbItems->scrollbar->getMarginX()) && (eventX < getWidth())))
		{
			if (getSelectedIndex() >= lbItems->items.size())
				return;

			selectedLabel->setText(lbItems->items[getSelectedIndex()]->getText());
			toggleOpen();
		}
	}

	// On Option changed event
	if (getSelectedIndex() != prevSelectedIndex)
	{
		if (OptionChangedListener != 0)
			(cPanel->*OptionChangedListener)(getSelectedIndex());
	}
	prevSelectedIndex = getSelectedIndex();

	/*// open/close the listbox
	if ((eventY <= getHeight()) && (eventX >= arrow->getMarginX()))
	{
		// arrow click
		toggleOpen();
	}
	else
	{
		//In the listbox
		if(eventY > getHeight())
		{
			// not scrollbar click
			if(!(lbItems->scrollbar->isVisible() && (eventX >= lbItems->scrollbar->getMarginX())))
			{
				if (getSelectedIndex() >= lbItems->items.size())
					return;

				selectedLabel->setText(lbItems->items[getSelectedIndex()]->getText());
				toggleOpen();
			}
		}
	}*/
}

void RUDropdown::onMouseWheel(GPanel* cPanel, int eventX, int eventY, int scrollType)
{
	// printf("RUDropdown: onMouseWheel(%d);\n", scrollType);

	drawUpdate = true;
}

std::string RUDropdown::getType() const
{
	return "RUDropdown";
}

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
#include "RUScrollbar.h"
#include "../GItems/RUColors.h"
#include "RUImageComponent.h"

// set default image paths
const std::string RUScrollbar::arrowUpLocation = "resources/gui/Scrollbar/ArrowUp.bmp";
const std::string RUScrollbar::arrowDownLocation = "resources/gui/Scrollbar/ArrowDown.bmp";
const std::string RUScrollbar::scrollBarLocation = "resources/gui/Scrollbar/Scrollbar.bmp";

RUScrollbar::RUScrollbar()
{
	value = 0;
	maxValue = 0;
	toggleBorder(true);
	optionsShown = 0;
	orientation = ORIENTATION_VERTICAL;
	barColor = RUColors::DEFAULT_COLOR_SCROLLBAR;
	arrowSpace = 0.0f;

	// scrollbar images
	arrowUp = new RUImageComponent(arrowUpLocation);
	arrowUp->setBorderColor(RUColors::DEFAULT_DROPDOWN_ARROW);
	arrowUp->toggleBorder(true);
	arrowUp->setCursor(SDL_SYSTEM_CURSOR_HAND);
	addSubItem(arrowUp);

	arrowDown = new RUImageComponent(arrowDownLocation);
	arrowDown->setBorderColor(RUColors::DEFAULT_DROPDOWN_ARROW);
	arrowDown->toggleBorder(true);
	arrowDown->setCursor(SDL_SYSTEM_CURSOR_HAND);
	addSubItem(arrowDown);

	scrollBar = new RUImageComponent(scrollBarLocation);
	scrollBar->setCursor(SDL_SYSTEM_CURSOR_HAND);
	addSubItem(scrollBar);
}

RUScrollbar::RUScrollbar(int newMaxValue)
{
	value = 0;
	maxValue = newMaxValue;
	toggleBorder(true);
	optionsShown = 0;
	orientation = ORIENTATION_VERTICAL;
	barColor = RUColors::DEFAULT_COLOR_SCROLLBAR;
	arrowSpace = 0.0f;

	// scrollbar images
	arrowUp = new RUImageComponent(arrowUpLocation);
	arrowUp->toggleBorder(true);
	addSubItem(arrowUp);

	arrowDown = new RUImageComponent(arrowDownLocation);
	arrowDown->toggleBorder(true);
	addSubItem(arrowDown);

	scrollBar = new RUImageComponent(scrollBarLocation);
	scrollBar->setCursor(SDL_SYSTEM_CURSOR_HAND);
	addSubItem(scrollBar);
}

RUScrollbar::~RUScrollbar()
{
	value = 0;
	maxValue = 0;
	toggleBorder(true);
	optionsShown = 0;
	orientation = ORIENTATION_VERTICAL;
	barColor = RUColors::DEFAULT_COLOR_SCROLLBAR;
	arrowSpace = 0.0f;

	// scrollbar images
	if (arrowUp)
		delete arrowUp;
	arrowUp = NULL;
	if (arrowDown)
		delete arrowDown;
	arrowDown = NULL;
	if (scrollBar)
		delete scrollBar;
	scrollBar = NULL;
}

int RUScrollbar::getValue() const
{
	return value;
}

int RUScrollbar::getMaxValue() const
{
	return maxValue;
}

int RUScrollbar::getOptionsShown() const
{
	return optionsShown;
}

int RUScrollbar::getOrientation() const
{
	return orientation;
}

void RUScrollbar::setWidth(int newWidth)
{
	if (!arrowUp)
		return;

	if (!arrowDown)
		return;

	if (!scrollBar)
		return;

	width = newWidth;

	// scrollbar images
	arrowUp->setWidth(getWidth());
	arrowDown->setWidth(getWidth());
	scrollBar->setWidth(getWidth());

	drawUpdate = true;
}

void RUScrollbar::setHeight(int newHeight)
{
	if (!arrowUp)
		return;

	if (!arrowDown)
		return;

	if (!scrollBar)
		return;

	height = newHeight;

	// scrollbar images
	arrowSpace = getWidth();
	arrowUp->setWidth(getWidth());
	arrowUp->setHeight(arrowSpace);
	arrowDown->setMarginY(getHeight() - arrowSpace);
	arrowDown->setHeight(arrowSpace);

	calculateBarPos();
	drawUpdate = true;
}

int RUScrollbar::increment()
{
	if (value < maxValue - optionsShown)
	{
		++value;

		calculateBarPos();
		drawUpdate = true;
	}
	return value;
}

int RUScrollbar::decrement()
{
	if (value > 0)
	{
		--value;

		calculateBarPos();
		drawUpdate = true;
	}
	return value;
}

void RUScrollbar::setValue(int newValue)
{
	value = newValue;
	if (value < 0)
		value = 0;
	else if (value > maxValue)
		value = maxValue;

	calculateBarPos();
	drawUpdate = true;
}

void RUScrollbar::setMaxValue(int newMaxValue)
{
	maxValue = newMaxValue;
	if (maxValue < 0)
		maxValue = 0;
	setValue(value); // for refresh

	calculateBarPos();
	drawUpdate = true;
}

void RUScrollbar::setOptionsShown(int newOptionsShown)
{
	optionsShown = newOptionsShown;
	if (optionsShown < 0)
		optionsShown = 0;

	calculateBarPos();
	drawUpdate = true;
}

void RUScrollbar::clear()
{
	setMaxValue(0);
}

void RUScrollbar::setOrientation(int newOrientation)
{
	orientation = newOrientation;

	calculateBarPos();
	drawUpdate = true;
}

void RUScrollbar::calculateBarPos()
{
	if (maxValue == 0)
		return;

	// draw the bar in the right spot
	int blockSpace = getHeight() - (arrowSpace * 2);
	int blockSize = blockSpace / maxValue;
	int blockOffset = arrowSpace + (blockSize * value);
	int newHeight = blockSize * optionsShown;

	scrollBar->setMarginY(blockOffset);
	scrollBar->setHeight(newHeight);
	scrollBar->setVisible((maxValue > 0) && (optionsShown > 0));
	arrowUp->setVisible((maxValue > 0) && (optionsShown > 0));
	arrowDown->setVisible((maxValue > 0) && (optionsShown > 0));
}

void RUScrollbar::updateBackground(SDL_Renderer* renderer)
{
	//
}

void RUScrollbar::onMouseDown(GPanel* cPanel, int eventX, int eventY)
{
	// printf("RUScrollbar: onMouseDown(%d, %d);\n", eventX, eventY);
	if (eventY <= arrowSpace)
		decrement();
	else if (eventY >= getHeight() - arrowSpace)
		increment();

	drawUpdate = true;
}

void RUScrollbar::onMouseWheel(GPanel* cPanel, int eventX, int eventY, int scrollType)
{
	// printf("RUScrollbar: onMouseWheel(%d);\n", scrollType);

	if (scrollType == SCROLL_DOWN)
		decrement();
	else if (scrollType == SCROLL_UP)
		increment();

	std::pair<int, int> offset(0, 0);
	calculateSubItemPositions(offset);

	drawUpdate = true;
}

std::string RUScrollbar::getType() const
{
	return "RUScrollbar";
}

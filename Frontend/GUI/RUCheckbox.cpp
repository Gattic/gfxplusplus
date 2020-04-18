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
#include "RUCheckbox.h"
#include "../GItems/RUColors.h"
#include "RUImageComponent.h"
#include "Text/RULabel.h"

RUCheckbox::RUCheckbox(std::string message)
{
	checked = false;

	// checkbox
	checkboxLocation = "resources/gui/Checkbox/unchecked.bmp";
	checkbox = new RUImageComponent(checkboxLocation);
	checkbox->setCursor(SDL_SYSTEM_CURSOR_HAND);
	checkbox->setBorderColor(RUColors::DEFAULT_BUTTON_BORDER_BLUE);
	checkbox->setBorderWidth(1);
	checkbox->toggleBorder(true);
	addSubItem(checkbox);

	// checkbox label
	checkboxLabel = new RULabel();
	checkboxLabel->setText(message);
	addSubItem(checkboxLabel);
}

RUCheckbox::~RUCheckbox()
{
	checked = false;
	checkboxLocation = "";

	// delete the selection label
	if (checkboxLabel)
		delete checkboxLabel;
	checkboxLabel = NULL;

	// delete the checkbox
	if (checkbox)
		delete checkbox;
	checkbox = NULL;
}

bool RUCheckbox::isChecked() const
{
	return checked;
}

/*!
 * @brief get selected text
 * @details gets the text content of the selected label
 */
std::string RUCheckbox::getLabel() const
{
	return checkboxLabel->getText();
}

void RUCheckbox::toggleCheck()
{
	checked = !checked;
	if (checked)
		checkboxLocation = "resources/gui/Checkbox/checked.bmp";
	else
		checkboxLocation = "resources/gui/Checkbox/unchecked.bmp";
	checkbox->setBGImageFromLocation(checkboxLocation);

	drawUpdate = true;
}

void RUCheckbox::setWidth(int newWidth)
{
	if (!checkbox)
		return;

	width = newWidth;

	checkbox->setWidth(DEFAULT_SIDE_WIDTH);

	// set the new x and width of the subcomp label
	checkboxLabel->setMarginX(checkbox->getWidth());
	checkboxLabel->setWidth(getWidth() - checkbox->getWidth());

	drawUpdate = true;
}

void RUCheckbox::setHeight(int newHeight)
{
	if (!checkbox)
		return;

	height = newHeight;

	// update the selected label
	checkboxLabel->setHeight(getHeight());

	// update the checkbox
	checkbox->setHeight(getHeight());

	drawUpdate = true;
}

void RUCheckbox::setCheck(bool value)
{
	checked = value;

	if (checked)
		checkboxLocation = "resources/gui/Checkbox/checked.bmp";
	else
		checkboxLocation = "resources/gui/Checkbox/unchecked.bmp";
	checkbox->setBGImageFromLocation(checkboxLocation);

	drawUpdate = true;
}

void RUCheckbox::updateBackground(SDL_Renderer* renderer)
{
	//
}

void RUCheckbox::onMouseDown(gfxpp* cGfx, GPanel* cPanel, int eventX, int eventY)
{
	// printf("RUCheckbox: onMouseDown(%d, %d);\n", eventX, eventY);

	// check/uncheck the checkbox
	if (eventY <= getHeight())
	{
		// checkbox click
		toggleCheck();
	}
}

void RUCheckbox::onMouseWheel(gfxpp* cGfx, GPanel* cPanel, int eventX, int eventY, int scrollType)
{
	// printf("RUCheckbox: onMouseWheel(%d);\n", scrollType);
	drawUpdate = true;
}

std::string RUCheckbox::getType() const
{
	return "RUCheckbox";
}

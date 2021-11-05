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
#include "RUTable.h"
#include "../GItems/RUColors.h"
#include "../Graphics/graphics.h"
#include "RUScrollbar.h"
#include "Text/RULabel.h"

/*!
 * @brief RUTable constructor
 * @details constructs an empty RUTable
 */
RUTable::RUTable()
{
	toggleBorder(true);
	rowsShown = 0;

	// add the scrollbar
	scrollbar = new RUScrollbar();
	scrollbar->setWidth(DEFAULT_SCROLLBAR_WIDTH);
	scrollbar->setOptionsShown(0);
	scrollbar->setVisible(false);
	addSubItem(scrollbar);
}

/*!
 * @brief RUTable destructor
 * @details cleanup
 */
RUTable::~RUTable()
{
	toggleBorder(true);
	rowsShown = 0;

	// delete the scrollbar
	if (scrollbar)
		delete scrollbar;
	scrollbar = NULL;
}

/*!
 * @brief get # of rows shown
 * @details get the number of RUTable rows shown by default. This does not include the header, only
 * data.
 * @return the number of RUTable rows shown
 */
unsigned int RUTable::getRowsShown() const
{
	return rowsShown;
}

/*!
 * @brief RUTable import
 * @details imports an RUTable from a GTable, copying its rows, headers, etc
 * @param importTable the GTable from which to import
 */
void RUTable::import(const shmea::GTable& newTable)
{
	copy(newTable);

	updateLabels();
	drawUpdate = true;
}

/*!
 * @brief set rows shown
 * @details set the number of rows shown in the component, thus changing the size of the component
 * @param newRowsShown the new # of rows shown
 */
void RUTable::setRowsShown(unsigned int newRowsShown)
{
	// Set the rows to show
	rowsShown = newRowsShown;

	// update the scrollbar
	scrollbar->setOptionsShown(rowsShown);

	refreshLabels();
	drawUpdate = true;
}

void RUTable::setWidth(int newWidth)
{
	if (!scrollbar)
		return;

	width = newWidth;

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

void RUTable::setHeight(int newHeight)
{
	if (!scrollbar)
		return;

	height = newHeight;

	// update the scrollbar
	scrollbar->setHeight(getHeight());

	drawUpdate = true;
}

/*!
 * @brief clear
 * @details clears the RUTable of contents
 */
void RUTable::clear()
{
	shmea::GTable::clear();
	scrollbar->setValue(0);
	scrollbar->setMaxValue(0);
	scrollbar->setVisible(false);

	updateLabels();
	drawUpdate = true;
}

/*!
 * @brief update visible labels
 * @details update the label visibility based on the scrollbar position
 */
void RUTable::updateLabels()
{
	unsigned int cIndex = scrollbar->getValue();

	// Number of visible rows in the new table
	unsigned int maxRows = cIndex + rowsShown;
	if (maxRows > numberOfRows())
		maxRows = numberOfRows();

	// Trim the rows
	subitems.clear();
	for (unsigned int row = 0; row < textLabels.size(); ++row)
	{
		int toggleVis = 0;
		if (row >= numberOfRows())
			++toggleVis;

		// Trim the cols
		for (unsigned int col = 0; col < textLabels[row].size(); ++col)
		{
			if ((col >= textLabels[row].size()) || (toggleVis > 0))
			{
				RULabel* cLabel = textLabels[row][col];
				if (!cLabel)
					continue;

				cLabel->setVisible(false);
				delete cLabel;
			}
		}
	}
	textLabels.clear();
	addSubItem(scrollbar);

	// No data
	if (numberOfCols() == 0)
		return;

	// No rows shown
	if (!(rowsShown > 0))
	{
		// clear the scrollbar
		scrollbar->setValue(0);
		scrollbar->setMaxValue(0);
		scrollbar->setVisible(false);

		drawUpdate = true;
		return;
	}

	// Put the scrollbar back
	int newMaxValue = numberOfRows();
	if (newMaxValue < 0)
	{
		newMaxValue = numberOfRows();
		scrollbar->setVisible(false);
	}
	else
		scrollbar->setVisible(true);
	scrollbar->setMaxValue(newMaxValue);

	// update the labels
	int cellHeight = height / rowsShown;
	int cellWidth = width / numberOfCols();
	if (scrollbar->isVisible())
		cellWidth = (width - scrollbar->getWidth()) / numberOfCols();
	for (unsigned int row = 0; row < maxRows; ++row)
	{
		std::vector<RULabel*> newRow;
		for (unsigned int col = 0; col < numberOfCols(); ++col)
		{
			RULabel* newItem = new RULabel(getCell(row, col).c_str());
			newItem->setMarginX(col * cellWidth);
			newItem->setMarginY(row * cellHeight);
			newItem->setWidth(cellWidth);
			newItem->setHeight(cellHeight);
			// newItem->setFontSize(cellHeight / 2);
			newItem->toggleBorder(true);
			newRow.push_back(newItem);
			addSubItem(newItem);
		}
		textLabels.push_back(newRow);
	}

	// Refresh the text in the labels
	refreshLabels();

	std::pair<int, int> offset(0, 0);
	calculateSubItemPositions(offset);

	drawUpdate = true;
}

void RUTable::refreshLabels()
{
	if (rowsShown == 0)
		return;

	if (numberOfRows() == 0)
		return;

	if (numberOfCols() == 0)
		return;

	// Draw the labels
	unsigned int cIndex = scrollbar->getValue();
	unsigned int maxRows = cIndex + rowsShown;
	if (maxRows > numberOfRows())
		maxRows = numberOfRows();

	// Cell sizes
	int cellHeight = height / rowsShown;
	int cellWidth = width / numberOfCols();
	if (scrollbar->isVisible())
		cellWidth = (width - scrollbar->getWidth()) / numberOfCols();

	// Only the visible labels
	for (unsigned int row = 0; row < maxRows - cIndex; ++row)
	{
		for (unsigned int col = 0; col < numberOfCols(); ++col)
		{
			textLabels[row][col]->setMarginX(col * cellWidth);
			textLabels[row][col]->setMarginY(row * cellHeight);
			textLabels[row][col]->setWidth(cellWidth);
			textLabels[row][col]->setHeight(cellHeight);
			// textLabels[row][col]->setFontSize(cellHeight / 2);
			textLabels[row][col]->setText(getCell(row + cIndex, col).c_str());
		}
	}

	// Hide the scrollbar if we dont need it
	if (numberOfRows() > (unsigned int)scrollbar->getOptionsShown())
		scrollbar->setVisible(true);
	else
		scrollbar->setVisible(false);

	std::pair<int, int> offset(0, 0);
	calculateSubItemPositions(offset);
	drawUpdate = true;
}

void RUTable::updateBackground(gfxpp* cGfx)
{
	//
}

/*!
 * @brief mouse down event handler
 * @details responds to scrolls within the component. In this case, checks if the scroll happens
 * within the scrollbar and respond accordingly.
 * @param cPanel the panel where the event occured
 * @param eventX the x-coordinate of the click
 * @param eventY the y-coordinate of the click
 * @param scrollType the type of scroll
 */
void RUTable::onMouseWheel(gfxpp* cGfx, GPanel* cPanel, int eventX, int eventY, int scrollType)
{
	// printf("RUTable: onMouseWheel(%d);\n", scrollType);

	// scrolling down on the labels
	if (!(eventX >= scrollbar->getMarginX() && eventX < getWidth()))
	{
		int oldIndex = scrollbar->getValue();
		if (scrollType == SCROLL_DOWN)
			scrollbar->decrement();
		else if (scrollType == SCROLL_UP)
			scrollbar->increment();

		if (oldIndex != scrollbar->getValue())
		{
			refreshLabels();
			drawUpdate = true;
		}
	}
}

std::string RUTable::getType() const
{
	return "RUTable";
}

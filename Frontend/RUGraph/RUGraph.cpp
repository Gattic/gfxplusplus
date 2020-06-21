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
#include "RUGraph.h"
#include "../GFXUtilities/point2.h"
#include "../GUI/Text/RULabel.h"
#include "../Graphics/graphics.h"
#include "Backend/Database/GList.h"
#include "Backend/Database/GTable.h"
#include "Backend/Database/GType.h"

RUGraph::RUGraph(int newWidth, int newHeight, int newQuadrants)
{
	width = newWidth;
	height = newHeight;
	quadrants = newQuadrants;

	// set the origin
	if (quadrants == QUADRANTS_ONE)
	{
		axisOriginX = 0;
		axisOriginY = 0;
	}
	else if (quadrants == QUADRANTS_FOUR)
	{
		axisOriginX = width / 2;
		axisOriginY = height / 2;
	}

	graphSize = DEFAULT_GRAPH_SIZE;
	axisWidth = DEFAULT_AXIS_WIDTH;
	gridEnabled = false;
	gridLineWidth = DEFAULT_GRIDLINE_WIDTH;

	// checkbox label
	titleLabel = new RULabel();
	titleLabel->setWidth(350);
	titleLabel->setHeight(30);
	titleLabel->setMarginX(getMarginX() + (width / 2) - (titleLabel->getWidth() / 2));
	titleLabel->setMarginY(getMarginY() - 30);
	titleLabel->setText("");
	titleLabel->setVisible(false);
	addSubItem(titleLabel);
}

RUGraph::~RUGraph()
{
	width = 0;
	height = 0;
	graphSize = 0;
	axisOriginX = 0;
	axisOriginY = 0;
	axisWidth = 0;
	gridEnabled = false;
	gridLineWidth = 0;
	quadrants = QUADRANTS_ONE;
}

int RUGraph::getGraphSize() const
{
	return graphSize;
}

int RUGraph::getAxisOriginX() const
{
	return axisOriginX;
}

int RUGraph::getAxisOriginY() const
{
	return axisOriginY;
}

int RUGraph::getAxisWidth() const
{
	return axisWidth;
}

bool RUGraph::getGridEnabled() const
{
	return gridEnabled;
}

int RUGraph::getGridLineWidth() const
{
	return gridLineWidth;
}

int RUGraph::getQuadrants() const
{
	return quadrants;
}

float RUGraph::getQuadrantOffsetX() const
{
	// quadrant offset
	float quadrantOffsetX = 0.0f;
	if (quadrants == QUADRANTS_ONE)
		quadrantOffsetX = 0;
	else if (quadrants == QUADRANTS_FOUR)
		quadrantOffsetX = ((float)getWidth()) / 2.0f;

	return quadrantOffsetX;
}

float RUGraph::getQuadrantOffsetY() const
{
	// quadrant offset
	float quadrantOffsetY = 0.0f;
	if (quadrants == QUADRANTS_ONE)
		quadrantOffsetY = getHeight();
	else if (quadrants == QUADRANTS_FOUR)
		quadrantOffsetY = ((float)getHeight()) / 2.0f;

	return quadrantOffsetY;
}

void RUGraph::setGraphSize(int newGraphSize)
{
	graphSize = newGraphSize;
	drawUpdate = true;
}

void RUGraph::setAxisWidth(int newAxisWidth)
{
	axisWidth = newAxisWidth;
	drawUpdate = true;
}

void RUGraph::setGridEnabled(bool newGridEnabled)
{
	gridEnabled = newGridEnabled;
	drawUpdate = true;
}

void RUGraph::setGridLineWidth(int newGridLineWidth)
{
	gridLineWidth = newGridLineWidth;
	drawUpdate = true;
}

void RUGraph::setQuadrants(int newQuadrants)
{
	quadrants = newQuadrants;
	drawUpdate = true;
}

void RUGraph::setTitleLabel(std::string newLabel)
{
	if (newLabel == "" || newLabel.empty())
	{
		titleLabel->setVisible(false);
		return;
	}

	titleLabel->setText(newLabel);
	titleLabel->setVisible(true);
}

void RUGraph::updateBackground(gfxpp* cGfx)
{
	// draw the axes
	if (axisWidth > 0)
	{
		// x axis
		SDL_SetRenderDrawColor(cGfx->getRenderer(), getBorderColor().r, getBorderColor().g,
							   getBorderColor().b, 0xFF);

		// set the x rect
		SDL_Rect axisX;
		axisX.x = 0;
		axisX.y = getQuadrantOffsetY() + axisOriginY - (axisWidth / 2);
		axisX.w = width;
		axisX.h = axisWidth;

		// set the y rect
		SDL_Rect axisY;
		axisY.x = getQuadrantOffsetX() + axisOriginX - (axisWidth / 2);
		axisY.y = 0;
		axisY.w = axisWidth;
		axisY.h = height;

		SDL_RenderFillRect(cGfx->getRenderer(), &axisX);
		SDL_RenderFillRect(cGfx->getRenderer(), &axisY);

		if ((gridEnabled) && (gridLineWidth > 0))
		{
			SDL_SetRenderDrawColor(cGfx->getRenderer(), 0x61, 0x61, 0x61, 0xFF); // gray

			// x grid
			int lineCount = graphSize * DEFAULT_NUM_ZONES; // 10 spaces per axis
			int lineGap = ((float)width) / ((float)lineCount);
			int lineWidth = gridLineWidth;
			int lineHeight = height;

			for (int i = (quadrants == QUADRANTS_FOUR) ? -lineCount : 0; i < lineCount; ++i)
			{
				// add it to the background
				SDL_Rect lineXRect;
				lineXRect.x = axisOriginX + (i * lineGap) - (lineWidth / 2);
				if (quadrants == QUADRANTS_ONE)
					lineXRect.y = getQuadrantOffsetY() + axisOriginY - (lineHeight);
				else if (quadrants == QUADRANTS_FOUR)
					lineXRect.y = getQuadrantOffsetY() + axisOriginY - (lineHeight / 2);
				lineXRect.w = lineWidth;
				lineXRect.h = lineHeight;

				SDL_RenderFillRect(cGfx->getRenderer(), &lineXRect);
			}

			// y grid
			lineCount = graphSize * DEFAULT_NUM_ZONES; // 10 spaces per axis
			lineGap = ((float)height) / ((float)lineCount);
			lineWidth = width;
			lineHeight = gridLineWidth;

			for (int i = (quadrants == QUADRANTS_FOUR) ? -lineCount : 0; i < lineCount; ++i)
			{
				// add it to the background
				SDL_Rect lineYRect;
				if (quadrants == QUADRANTS_ONE)
					lineYRect.x = getQuadrantOffsetY() + axisOriginX - (lineWidth);
				else if (quadrants == QUADRANTS_FOUR)
					lineYRect.x = getQuadrantOffsetY() + axisOriginX - (lineWidth / 2);
				lineYRect.y = axisOriginY + (i * lineGap) - (lineHeight / 2);
				lineYRect.w = lineWidth;
				lineYRect.h = lineHeight;

				SDL_RenderFillRect(cGfx->getRenderer(), &lineYRect);
			}
		}

		// x ticks
		int tickCount = graphSize * DEFAULT_NUM_ZONES; // 10 spaces per axis
		int tickGap = ((float)width) / ((float)tickCount);
		int tickWidth = axisWidth;
		int tickHeight = axisWidth * 5;
		if (quadrants == QUADRANTS_FOUR)
			tickWidth = axisWidth / DEFAULT_AXIS_WIDTH;
		SDL_SetRenderDrawColor(cGfx->getRenderer(), getBorderColor().r, getBorderColor().g,
							   getBorderColor().b, getBorderColor().a);

		for (int i = (quadrants == QUADRANTS_FOUR) ? -tickCount : 0; i < tickCount; ++i)
		{
			// add it to the background
			SDL_Rect tickXRect;
			tickXRect.x = axisOriginX + (i * tickGap) - (tickWidth / 2);
			tickXRect.y = getQuadrantOffsetY() + axisOriginY - (tickHeight / 2);
			tickXRect.w = tickWidth;
			tickXRect.h = tickHeight;

			SDL_RenderFillRect(cGfx->getRenderer(), &tickXRect);
		}

		// y ticks
		tickCount = graphSize * DEFAULT_NUM_ZONES; // 10 spaces per axis
		tickGap = ((float)height) / ((float)tickCount);
		tickWidth = axisWidth * 5;
		tickHeight = axisWidth;
		if (quadrants == QUADRANTS_FOUR)
			tickHeight = axisWidth / DEFAULT_AXIS_WIDTH;

		for (int i = (quadrants == QUADRANTS_FOUR) ? -tickCount : 0; i < tickCount; ++i)
		{
			// add it to the background
			SDL_Rect tickYRect;
			tickYRect.x = getQuadrantOffsetX() + axisOriginX - (tickWidth / 2);
			tickYRect.y = axisOriginY + (i * tickGap) - (tickHeight / 2);
			tickYRect.w = tickWidth;
			tickYRect.h = tickHeight;

			SDL_RenderFillRect(cGfx->getRenderer(), &tickYRect);
		}
	}
}

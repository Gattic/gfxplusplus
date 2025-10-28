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
#include "RULabel.h"
#include "../../GItems/RUColors.h"
#include "../../Graphics/graphics.h"
#include "../../GItems/GPanel.h"

RULabel::RULabel()
{
	//
	toggleBG(false);
	//setBGColor(RUColors::COLOR_DARK_GRAY);
	setAutoWidthToText(true);
}

RULabel::RULabel(shmea::GString newText)
{
	toggleBG(false);
	//setBGColor(RUColors::COLOR_DARK_GRAY);
	setText(newText);
	setAutoWidthToText(true);
}

RULabel::~RULabel()
{
	//
}

void RULabel::updateBackground(gfxpp* cGfx)
{
	if (getAutoWidthToText())
	{
		int newW = measureFullTextWidth(cGfx);
		if (newW != getWidth())
		{
			setWidth(newW);
			// Request a full panel redraw to prevent stale overlaps when bounds change
			if (cGfx && cGfx->focusedPanel)
			{
				std::pair<int,int> zero(0,0);
				cGfx->focusedPanel->calculateSubItemPositions(zero);
				cGfx->focusedPanel->requireDrawUpdate();
			}
			requireDrawUpdate();
		}
	}
	if (getAutoHeightToFont())
	{
		int newH = measureFontPixelHeight(cGfx);
		if (newH > 0 && newH != getHeight())
		{
			setHeight(newH);
			if (cGfx && cGfx->focusedPanel)
			{
				std::pair<int,int> zero(0,0);
				cGfx->focusedPanel->calculateSubItemPositions(zero);
				cGfx->focusedPanel->requireDrawUpdate();
			}
			requireDrawUpdate();
		}
	}
	drawText(cGfx);
}

shmea::GString RULabel::getType() const
{
	return "RULabel";
}

void RULabel::onMouseDown(gfxpp* cGfx, GPanel* cPanel, int eventX, int eventY)
{
	// printf("RUTextComponent: onMouseDown(%d, %d);\n", eventX, eventY);
}

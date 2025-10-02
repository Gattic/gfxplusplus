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
#include "RUTextComponent.h"
#include "../../Graphics/graphics.h"
#include "Backend/Database/GString.h"
#include "GFont.h"
#include "../../Graphics/GfxRenderer.h"
#include "../../GItems/GPanel.h"
#ifdef GFX_HAVE_OPENGL
#include "GLTextRenderer.h"
#endif
#include <string>

RUTextComponent::RUTextComponent()
{
	text = "";
	strDrawText = "";
	strWidth = 0.0f;
	cursorX = 0;
	dimRatio = 0.0f;
	passwordChar = '*';
	passwordField = false;
	cursorStart = 0;
	readOnly = true;
	FONT_COLOR = 0;
	xClick = 0;
	fontPixelHeight = 0;
	autoWidthToText = true;
	autoHeightToFont = true;

	// event listeners
	KeyListener = 0;
}

RUTextComponent::~RUTextComponent()
{
	text = "";
	strDrawText = "";
	strWidth = 0.0f;
	cursorX = 0;
	dimRatio = 0.0f;
	passwordChar = '*';
	passwordField = false;
	readOnly = true;
	xClick = 0;
	FONT_COLOR = 0;
	fontPixelHeight = 0;
	autoWidthToText = true;
	autoHeightToFont = true;

	// event listeners
	KeyListener = 0;
}
void RUTextComponent::updateBackgroundHelper(gfxpp* cGfx)
{
    // If auto-height is enabled and height is not set, compute a sensible height early
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
    // If auto-width is enabled and width is not set, compute width from text early
    if (getAutoWidthToText())
    {
        int newW = measureFullTextWidth(cGfx);
        if (newW <= 0)
        {
            // Provide a sensible default width for empty text (textbox case)
            int ph = measureFontPixelHeight(cGfx);
            if (ph <= 0)
                ph = 16;
            newW = ph * 8; // approx. 8-character field as a baseline
        }
        if (newW > 0 && newW != getWidth())
        {
            setWidth(newW);
            if (cGfx && cGfx->focusedPanel)
            {
                std::pair<int,int> zero(0,0);
                cGfx->focusedPanel->calculateSubItemPositions(zero);
                cGfx->focusedPanel->requireDrawUpdate();
            }
            requireDrawUpdate();
        }
    }
    RUComponent::updateBackgroundHelper(cGfx);
}

shmea::GString RUTextComponent::getText() const
{
	return text;
}

char RUTextComponent::getPasswordChar() const
{
	return passwordChar;
}

bool RUTextComponent::isPasswordField() const
{
	return passwordField;
}

bool RUTextComponent::getReadOnly() const
{
	return readOnly;
}

void RUTextComponent::setText(const char* newCStrText)
{
	shmea::GString newText(newCStrText);
	setText(newText);
}

void RUTextComponent::setText(shmea::GType newItem)
{
	if (text == newItem)
		return;

	if (newItem.getType() == shmea::GType::STRING_TYPE)
		text = newItem;
	else if (newItem.getType() == shmea::GType::CHAR_TYPE)
		text = shmea::GString::charTOstring(newItem.getChar());
	else if (newItem.getType() == shmea::GType::SHORT_TYPE)
		text = shmea::GString::shortTOstring(newItem.getShort());
	else if (newItem.getType() == shmea::GType::INT_TYPE)
		text = shmea::GString::intTOstring(newItem.getInt());
	else if (newItem.getType() == shmea::GType::LONG_TYPE)
		text = shmea::GString::longTOstring(newItem.getLong());
	else if (newItem.getType() == shmea::GType::FLOAT_TYPE)
		text = shmea::GString::floatTOstring(newItem.getFloat());
	else if (newItem.getType() == shmea::GType::DOUBLE_TYPE)
		text = shmea::GString::doubleTOstring(newItem.getDouble());
	else if (newItem.getType() == shmea::GType::BOOLEAN_TYPE)
		text = newItem.getBoolean() ? "True" : "False";

	strDrawText = "";
	strWidth = 0.0f;
	cursorX = 0;
	cursor.reset();
	drawUpdate = true;
}

void RUTextComponent::setPasswordChar(char newPasswordChar)
{
	passwordChar = newPasswordChar;
	requireDrawUpdate();
}

void RUTextComponent::setPasswordField(bool newPasswordField)
{
	passwordField = newPasswordField;
	cursor.reset();
	strDrawText = "";
	strWidth = 0.0f;
	requireDrawUpdate();
}

void RUTextComponent::setReadOnly(bool newReadOnly)
{
	readOnly = newReadOnly;
}

void RUTextComponent::setFontColor(int newFontColor)
{
	FONT_COLOR = newFontColor;
	drawUpdate = true;
}

void RUTextComponent::setFontSize(int newPixelHeight)
{
	fontPixelHeight = newPixelHeight;
	requireDrawUpdate();
}

int RUTextComponent::getFontSize() const
{
	return fontPixelHeight;
}

void RUTextComponent::setAutoWidthToText(bool enable)
{
	autoWidthToText = enable;
	requireDrawUpdate();
}

bool RUTextComponent::getAutoWidthToText() const
{
	return autoWidthToText;
}

void RUTextComponent::setAutoHeightToFont(bool enable)
{
	autoHeightToFont = enable;
	requireDrawUpdate();
}

bool RUTextComponent::getAutoHeightToFont() const
{
	return autoHeightToFont;
}

int RUTextComponent::measureFullTextWidth(gfxpp* cGfx) const
{
	if (!cGfx)
		return 0;

	GFont* cFont = NULL;
	int fontColor = FONT_COLOR;
	std::map<int, GFont*>::const_iterator it = cGfx->graphicsFonts.find(fontColor);
	if (it != cGfx->graphicsFonts.end())
		cFont = it->second;
	if (!cFont)
		return 0;

	const char* raw = text.c_str();
	if (!raw)
		return 0;
	std::string fullText = std::string(raw);
	if (fullText.empty())
		return 0;

	// Use masked text for measurement when password mode is enabled
	std::string maskedText;
	if (passwordField)
		maskedText.assign(fullText.size(), passwordChar);

#ifdef GFX_HAVE_OPENGL
	if (cGfx->getRenderBackend() == gfxpp::RENDER_BACKEND_OPENGL)
	{
		const char* fontPathC = cFont->getFontPath().c_str();
		std::string fontPathStr = fontPathC ? std::string(fontPathC) : std::string();
		int glPixelHeight = (fontPixelHeight > 0) ? fontPixelHeight : (cFont->getFontSize() > 0 ? cFont->getFontSize() : (getHeight() > 0 ? getHeight() : 16));
		GLTextRenderer* glText = cGfx->getGLText(fontPathStr, glPixelHeight);
		if (!glText)
			return 0;
		const std::string& textToMeasure = passwordField ? maskedText : fullText;
		return glText->measureTextWidth(textToMeasure);
	}
#endif

#ifdef GFX_HAVE_SDL2
	if (cFont->getFont())
	{
		int baseHeight = cFont->getMaxHeight();
		if (baseHeight <= 0)
			baseHeight = 1;
		int targetPixelHeight = (fontPixelHeight > 0) ? fontPixelHeight : (getHeight() > 0 ? getHeight() : (cFont->getFontSize() > 0 ? cFont->getFontSize() : baseHeight));
		float ratio = ((float)targetPixelHeight) / ((float)baseHeight);
		int widthSum = 0;
		const std::string& textToMeasure = passwordField ? maskedText : fullText;
		for (size_t i = 0; i < textToMeasure.size(); ++i)
		{
			GLetter* cLetter = cFont->getLetter(textToMeasure[i]);
			if (!cLetter)
				continue;
			widthSum += cLetter->getWidth();
		}
		return (int)(ratio * (float)widthSum);
	}
#endif

	return 0;
}

int RUTextComponent::measureFontPixelHeight(gfxpp* cGfx) const
{
    if (!cGfx)
        return 0;

    GFont* cFont = NULL;
    int fontColor = FONT_COLOR;
    std::map<int, GFont*>::const_iterator it = cGfx->graphicsFonts.find(fontColor);
    if (it != cGfx->graphicsFonts.end())
        cFont = it->second;
    if (!cFont)
        return 0;

#ifdef GFX_HAVE_OPENGL
    if (cGfx->getRenderBackend() == gfxpp::RENDER_BACKEND_OPENGL)
    {
        const char* fontPathC = cFont->getFontPath().c_str();
        std::string fontPathStr = fontPathC ? std::string(fontPathC) : std::string();
        int glPixelHeight = (fontPixelHeight > 0) ? fontPixelHeight : (cFont->getFontSize() > 0 ? cFont->getFontSize() : (getHeight() > 0 ? getHeight() : 16));
        return glPixelHeight;
    }
#endif

#ifdef GFX_HAVE_SDL2
    if (cFont->getFont())
    {
        int baseHeight = cFont->getMaxHeight();
        if (baseHeight <= 0)
            baseHeight = 1;
        int targetPixelHeight = (fontPixelHeight > 0) ? fontPixelHeight : (getHeight() > 0 ? getHeight() : (cFont->getFontSize() > 0 ? cFont->getFontSize() : baseHeight));
        return targetPixelHeight;
    }
#endif

    return 0;
}

void RUTextComponent::calculateRenderInfo(GFont* cFont)
{
	if ((!cFont) || (!cFont->getFont()))
		return;

	// set the text to draw
	/*if (passwordField)
	{
		strDrawText = "";
		for (unsigned int i = 0; i < text.length(); ++i)
			strDrawText += '*';
	}*/

	if (text.length())
	{
		int newHeight = cFont->getMaxHeight();
		int targetPixelHeight = (fontPixelHeight > 0) ? fontPixelHeight : getHeight();
		dimRatio = (((float)(targetPixelHeight)) / ((float)(newHeight)));

		// Text has not tested it bounds
		do
		{
			if ((strWidth < getWidth()) && (cursor.index + cursor.maxLen + 1 <= text.length()))
				++cursor.maxLen;

			bool cursorSet = false;
			int newWidth = 0;
			shmea::GString rawSubG = text.substr(cursor.index, cursor.maxLen);
			const char* rawSubC = rawSubG.c_str();
			std::string rawSub = rawSubC ? std::string(rawSubC) : std::string();
			if (passwordField)
			{
				std::string masked(rawSub.size(), passwordChar);
				strDrawText = masked.c_str();
			}
			else
			{
				strDrawText = rawSubG;
			}
			for (unsigned int i = 0; i < rawSub.length(); ++i)
			{
				if (i == cursor.cursorIndex)
				{
					cursorX = dimRatio * newWidth;
					cursorSet = true;
				}

				int prevWidth = dimRatio * newWidth;
				char widthCh = passwordField ? passwordChar : rawSub[i];
				GLetter* cLetter = cFont->getLetter(widthCh);
				if (!cLetter)
					continue;

				newWidth += cLetter->getWidth();

				// Move the cursor to the click
				if ((xClick) && (dimRatio * newWidth >= xClick))
				{
					cursor.cursorIndex = i;
					if (abs(dimRatio * newWidth - xClick) < abs(prevWidth - xClick))
						++cursor.cursorIndex;
					xClick = 0;
				}
			}

			strWidth = dimRatio * newWidth;
			if (!cursorSet)
				cursorX = strWidth;

			if (xClick)
			{
				cursor.cursorIndex = cursor.maxLen;
				xClick = 0;
			}

		} while ((strWidth < getWidth()) && (cursor.index + cursor.maxLen + 1 <= text.length()));

		// Text went over its bounds
		while (strWidth > getWidth())
		{
			//
			if (cursor.cursorIndex == cursor.maxLen)
			{
				--cursor.cursorIndex;
				--cursor.maxLen;
				++cursor.index;
			}
			else
			{
				--cursor.maxLen;
			}

			int newWidth = 0;
			shmea::GString rawSub2G = text.substr(cursor.index, cursor.maxLen);
			const char* rawSub2C = rawSub2G.c_str();
			std::string rawSub2 = rawSub2C ? std::string(rawSub2C) : std::string();
			if (passwordField)
			{
				std::string masked2(rawSub2.size(), passwordChar);
				strDrawText = masked2.c_str();
			}
			else
			{
				strDrawText = rawSub2G;
			}
			for (unsigned int i = 0; i < rawSub2.length(); ++i)
			{
				char widthCh2 = passwordField ? passwordChar : rawSub2[i];
				GLetter* cLetter2 = cFont->getLetter(widthCh2);
				if (!cLetter2)
					continue;

				newWidth += cLetter2->getWidth();
			}

			strWidth = dimRatio * newWidth;
		}
	}
	else
	{
		strDrawText = "";
		strWidth = 0.0f;
		cursorX = 0;
		cursor.reset();
	}
}

void RUTextComponent::drawText(gfxpp* cGfx)
{
	if (!cGfx)
		return;

	if (!cGfx->getDraw())
		return;

	GFont* cFont = NULL;
	int fontColor = FONT_COLOR;
	std::map<int, GFont*>::iterator it = cGfx->graphicsFonts.find(fontColor);
	if (it != cGfx->graphicsFonts.end())
	{
		cFont = it->second;
	}

	if (!cFont)
		return;

	int caretPixelHeight = (fontPixelHeight > 0) ? fontPixelHeight : getHeight();
	float cursorYGap = (getHeight() - caretPixelHeight);

	// For SDL path, draw to texture; for GL path, draw directly
#ifdef GFX_HAVE_SDL2
	if (cGfx->getRenderBackend() == gfxpp::RENDER_BACKEND_SDL2)
	{
		if (cGfx->getDraw())
			cGfx->getDraw()->setTargetTexture(getBackground());
		cGfx->getDraw()->setDrawColor(0, 0, 0, 0);
		cGfx->getDraw()->clear();
		updateBGBackground(cGfx);
	}
#endif

	// Draw the string (if any)
	GfxRect textRect;
	textRect.x = 0;
	textRect.y = 0;
	textRect.w = getWidth();
	textRect.h = getHeight();

#ifdef GFX_HAVE_OPENGL
	if (cGfx->getRenderBackend() == gfxpp::RENDER_BACKEND_OPENGL)
	{
		const char* fontPathC = cFont->getFontPath().c_str();
		std::string fontPathStr = fontPathC ? std::string(fontPathC) : std::string();
		int glPixelHeight = (fontPixelHeight > 0) ? fontPixelHeight : (cFont->getFontSize() > 0 ? cFont->getFontSize() : getHeight());
		GLTextRenderer* glText = cGfx->getGLText(fontPathStr, glPixelHeight);
		if (glText)
		{
			// Compute visible substring based on cursor/index using glyph advances
			if (text.length())
			{
				// Expand visible window while it fits width
				do
				{
					if ((strWidth < getWidth()) && (cursor.index + cursor.maxLen + 1 <= text.length()))
						++cursor.maxLen;

					bool cursorSet = false;
			int newWidth = 0;
			shmea::GString rawSubG = text.substr(cursor.index, cursor.maxLen);
			const char* rawSubC = rawSubG.c_str();
			std::string rawSub = rawSubC ? std::string(rawSubC) : std::string();
					if (passwordField)
					{
						std::string masked(rawSub.size(), passwordChar);
						strDrawText = masked.c_str();
					}
					else
					{
						strDrawText = rawSubG;
					}
					for (unsigned int i = 0; i < rawSub.length(); ++i)
					{
						if (i == cursor.cursorIndex)
						{
							cursorX = newWidth;
							cursorSet = true;
						}

						int prevWidth = newWidth;
						char widthCh = passwordField ? passwordChar : rawSub[i];
						newWidth += glText->getGlyphAdvance(widthCh);

						// Move the cursor to the click
						if ((xClick) && (newWidth >= xClick))
						{
							cursor.cursorIndex = i;
							if (abs(newWidth - xClick) < abs(prevWidth - xClick))
								++cursor.cursorIndex;
							xClick = 0;
						}
					}

					strWidth = newWidth;
					if (!cursorSet)
						cursorX = strWidth;

					if (xClick)
					{
						cursor.cursorIndex = cursor.maxLen;
						xClick = 0;
					}

				} while ((strWidth < getWidth()) && (cursor.index + cursor.maxLen + 1 <= text.length()));

				// Shrink if overflowed
				while (strWidth > getWidth())
				{
					if (cursor.cursorIndex == cursor.maxLen)
					{
						--cursor.cursorIndex;
						--cursor.maxLen;
						++cursor.index;
					}
					else
					{
						--cursor.maxLen;
					}

			int newWidth = 0;
			shmea::GString rawSub2G = text.substr(cursor.index, cursor.maxLen);
			const char* rawSub2C = rawSub2G.c_str();
			std::string rawSub2 = rawSub2C ? std::string(rawSub2C) : std::string();
					if (passwordField)
					{
						std::string masked2(rawSub2.size(), passwordChar);
						strDrawText = masked2.c_str();
					}
					else
					{
						strDrawText = rawSub2G;
					}
					for (unsigned int i = 0; i < rawSub2.length(); ++i)
					{
						char widthCh2 = passwordField ? passwordChar : rawSub2[i];
						newWidth += glText->getGlyphAdvance(widthCh2);
					}

					strWidth = newWidth;
				}

				// After bounds adjustments, recompute cursorX precisely and clamp to width
				{
					int recomputedWidth = 0;
					int recomputedCursorX = 0;
					for (unsigned int i = 0; i < strDrawText.length(); ++i)
					{
						char widthCh3 = passwordField ? passwordChar : strDrawText[i];
						int adv = glText->getGlyphAdvance(widthCh3);
						if (i < cursor.cursorIndex)
							recomputedCursorX += adv;
						recomputedWidth += adv;
					}
					strWidth = recomputedWidth;
					if ((unsigned)cursor.cursorIndex >= strDrawText.length())
						recomputedCursorX = recomputedWidth;
					cursorX = recomputedCursorX;
					if (cursorX > getWidth())
						cursorX = getWidth();
				}
			}
			else
			{
				strDrawText = "";
				strWidth = 0.0f;
				cursorX = 0;
				cursor.reset();
			}

			// Draw only the computed visible substring
			const char* raw = strDrawText.c_str();
			const std::string safe = raw ? std::string(raw) : std::string();
			float baselineY = (float)getHeight() - 2.0f; // small padding like SDL path
			glText->drawText(0.0f, baselineY, safe, cFont->getTextColor().r, cFont->getTextColor().g, cFont->getTextColor().b, cFont->getTextColor().a);

			// Draw caret directly in GL to avoid any renderer inconsistencies
			if (!readOnly && isFocused())
			{
				if (cursorStart == 0)
					cursorStart = time(NULL);
				unsigned int secondsSinceFocusGL = (unsigned int)(time(NULL) - cursorStart);
				unsigned int cursorCounterGL = secondsSinceFocusGL % 2;
				if ((cursorStart > 0) && (cursorCounterGL == 0))
				{
					int glPixelHeightCaret = cFont->getFontSize();
					if (glPixelHeightCaret <= 0)
						glPixelHeightCaret = getHeight();
					if (glPixelHeightCaret > getHeight())
						glPixelHeightCaret = getHeight();
					float caretX = (float)cursorX;
					float caretY = baselineY - (float)glPixelHeightCaret;
					float caretW = 2.0f;
					float caretH = (float)glPixelHeightCaret;
					glDisable(GL_TEXTURE_2D);
					glColor4ub(cFont->getTextColor().r, cFont->getTextColor().g, cFont->getTextColor().b, cFont->getTextColor().a);
					glBegin(GL_QUADS);
						glVertex2f(caretX    , caretY);
						glVertex2f(caretX+caretW, caretY);
						glVertex2f(caretX+caretW, caretY+caretH);
						glVertex2f(caretX    , caretY+caretH);
					glEnd();
				}
			}
		}
	}
	else
#endif
#ifdef GFX_HAVE_SDL2
	if (cFont->getFont())
	{
		// SDL path uses pre-baked letter textures and needs render info
		calculateRenderInfo(cFont);
		for (unsigned int i = 0; i < strDrawText.length(); ++i)
		{
			GLetter* cLetter = cFont->getLetter(strDrawText[i]);
			if (!cLetter)
				continue;
			int glyphW = static_cast<int>(dimRatio * cLetter->getWidth());
			GfxRect dstRect;
			dstRect.x = textRect.x;
			dstRect.y = (cursorYGap > 0 ? (int)(cursorYGap / 2.0f) : 0);
			dstRect.w = glyphW;
			dstRect.h = (int)((fontPixelHeight > 0) ? fontPixelHeight : getHeight());
			if (cGfx->getDraw())
				cGfx->getDraw()->copyTexture(cLetter->getTexture(), NULL, &dstRect);
			textRect.x += glyphW;
		}
	}
#endif

	// Draw blinking caret when focused (even if text is empty)
	if (!readOnly && isFocused())
	{
		if (cursorStart == 0)
			cursorStart = time(NULL);
		unsigned int secondsSinceFocus = (unsigned int)(time(NULL) - cursorStart);
		unsigned int cursorCounter = secondsSinceFocus % 2;
		if ((cursorStart > 0) && (cursorCounter == 0))
		{
			cGfx->getDraw()->setDrawColor(cFont->getTextColor().r,
				   cFont->getTextColor().g, cFont->getTextColor().b,
				   cFont->getTextColor().a);
			GfxRect cursorRect;
			cursorRect.x = cursorX;
#ifdef GFX_HAVE_OPENGL
			if (cGfx->getRenderBackend() == gfxpp::RENDER_BACKEND_OPENGL)
			{
				// Align caret to GL text baseline and pixel height used
				const char* fontPathC2 = cFont->getFontPath().c_str();
				std::string fontPathStr2 = fontPathC2 ? std::string(fontPathC2) : std::string();
				int glPixelHeight2 = (fontPixelHeight > 0) ? fontPixelHeight : (cFont->getFontSize() > 0 ? cFont->getFontSize() : getHeight());
				float baselineY2 = (float)getHeight() - 2.0f;
				cursorRect.y = (int)(baselineY2 - glPixelHeight2);
				cursorRect.w = 2;
				cursorRect.h = glPixelHeight2;
			}
			else
#endif
			{
				cursorRect.y = cursorYGap / 2.0f;
				cursorRect.w = 2;
				cursorRect.h = (int)((fontPixelHeight > 0) ? fontPixelHeight : (((float)height) - cursorYGap));
			}
			cGfx->getDraw()->fillRect(&cursorRect);
		}
	}

	// Reset target
	if (cGfx->getDraw())
		cGfx->getDraw()->resetTarget();

	// Continuously request redraw so caret can blink and clear on unfocus
	drawUpdate = true;

}

void RUTextComponent::drawCursor(gfxpp* cGfx, float cursorYGap)
{
	// caret is drawn during drawText when focused; no-op here to avoid duplicate/stale cursor
}

void RUTextComponent::setKeyListener(void (GPanel::*f)(char))
{
	KeyListener = f;
}

void RUTextComponent::onMouseDown(gfxpp* cGfx, GPanel* cPanel, int eventX, int eventY)
{
	// printf("RUTextComponent: onMouseDown(%d, %d);\n", eventX, eventY);
	cursorStart = time(NULL);
	xClick = eventX;
	drawUpdate = true;
}

void RUTextComponent::onKey(gfxpp* cGfx, char eventKeyPressed)
{
	// printf("RUTextComponent: onKey(%c);\n", eventKeyPressed);
}

bool RUTextComponent::onKeyHelper(gfxpp* cGfx, GPanel* cPanel, GfxKeycode eventKeyPressed,
				  Uint16 eventKeyModPressed)
{
	bool typed = false;

	if (!cPanel)
		return typed;

	char eventChar = GFont::keycodeTOchar(eventKeyPressed);

	// make the character caps
	if ((eventKeyModPressed & GFXMOD_SHIFT) || (eventKeyModPressed & GFXMOD_LSHIFT) ||
		(eventKeyModPressed & GFXMOD_RSHIFT))
		eventChar = shmea::GString::toUpper(eventChar);

	// toggle the case because of caps lock
	if (eventKeyModPressed & GFXMOD_CAPS)
		eventChar = shmea::GString::toggleCase(eventChar);

	// write to the text component
	if (!readOnly)
	{
		// interact with the component
		if (eventKeyPressed == GFXK_BACKSPACE)
		{
			if ((text.length() > 0) && (cursor.cursorIndex > 0))
			{
				// Delete the character
				if (cursor.cursorIndex)
				{
					if (cursor.index + cursor.cursorIndex <= text.length())
						text = text.substr(0, cursor.index + cursor.cursorIndex - 1) +
						   text.substr(cursor.index + cursor.cursorIndex);
					else if (cursor.index + cursor.cursorIndex == cursor.maxLen)
						text = text.substr(0, cursor.index + cursor.cursorIndex - 1);
					else if (text.length() == 1)
						text = "";
				}

				// Shift the cursor
				if ((cursor.cursorIndex) && (cursor.index == 0))
					--cursor.cursorIndex;
				else if (cursor.index)
					--cursor.index;

				if (cursor.index + cursor.cursorIndex < cursor.maxLen)
					cursor.maxLen = 0;
			}
		}
		else if (eventKeyPressed == GFXK_DELETE)
		{
			if ((text.length() > 0) && (cursor.index + cursor.cursorIndex < text.length()))
			{
				text = text.substr(0, cursor.index + cursor.cursorIndex) +
					   text.substr(cursor.index + cursor.cursorIndex + 1);

				if (cursor.index + cursor.cursorIndex < cursor.maxLen)
					cursor.maxLen = 0;
			}
		}
		else if ((eventKeyPressed == GFXK_UP) || (eventKeyPressed == GFXK_HOME))
		{
			// cursorStart = time(NULL);
		}
		else if ((eventKeyPressed == GFXK_DOWN) || (eventKeyPressed == GFXK_END))
		{
			// cursorStart = time(NULL);
		}
		else if (eventKeyPressed == GFXK_LEFT)
		{
			if (cursor.cursorIndex)
				--cursor.cursorIndex;
			else if (cursor.index)
				--cursor.index;

			cursorStart = time(NULL);
		}
		else if (eventKeyPressed == GFXK_RIGHT)
		{
			if (cursor.cursorIndex < cursor.maxLen)
				++cursor.cursorIndex;
			else if (cursor.index + cursor.maxLen < text.length())
				++cursor.index;

			cursorStart = time(NULL);
		}
		else
		{
			if (GFont::validChar(eventChar))
			{
				// Handle special characters mapping for keycodes
				if ((eventKeyModPressed & GFXMOD_SHIFT) || (eventKeyModPressed & GFXMOD_LSHIFT) ||
					(eventKeyModPressed & GFXMOD_RSHIFT))
				{
					eventChar = GFont::specialChar(eventChar);
				}

				// Append/Insert the Character
				if (cursor.index + cursor.cursorIndex >= text.length())
				{
					text += eventChar;
					++cursor.cursorIndex;
					if (cursor.cursorIndex > cursor.maxLen)
						++cursor.maxLen;
				}
				else
				{
					text = text.substr(0, cursor.index + cursor.cursorIndex) + eventChar +
						   text.substr(cursor.index + cursor.cursorIndex);
					++cursor.cursorIndex;
				}
			}
		}

		drawUpdate = true;
	}

	// pass down the event
	if (eventChar > 0x00)
		onKey(cGfx, eventChar);

	if (KeyListener != 0)
		(cPanel->*KeyListener)(eventKeyPressed);

	typed = true;
	return typed;
}

// Ensure caret state resets on losing focus
void RUTextComponent::unsetFocus()
{
    RULoseFocus::unsetFocus();
    cursorStart = 0;
    requireDrawUpdate();
}


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
#include "Backend/Database/gtype.h"
#include "GFont.h"

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

	// event listeners
	KeyListener = 0;
}

std::string RUTextComponent::getText() const
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
	std::string newText(newCStrText);
	setText(newText);
}

void RUTextComponent::setText(std::string newText)
{
	if (text == newText)
		return;

	// 0 means no cursor.maxLen
	if ((cursor.maxLen > 0) && (newText.length() > cursor.maxLen))
		return;

	text = newText;
	cursor.reset();
	strWidth = 0.0; // Force recalculate
	drawUpdate = true;
}

void RUTextComponent::setPasswordChar(char newPasswordChar)
{
	passwordChar = newPasswordChar;
}

void RUTextComponent::setPasswordField(bool newPasswordField)
{
	passwordField = newPasswordField;
}

void RUTextComponent::setReadOnly(bool newReadOnly)
{
	readOnly = newReadOnly;
}

void RUTextComponent::calculateRenderInfo(GFont* cFont)
{
	if ((!cFont) || (!cFont->getFont()))
		return;

	int newWidth = 0;
	int newHeight = 0;
	strDrawText = text;

	// set the text to draw
	if (passwordField)
	{
		strDrawText = "";
		for (unsigned int i = 0; i < text.length(); ++i)
			strDrawText += '*';
	}

	// First run
	if (strWidth == 0.0f)
	{
		for (unsigned int i = 0; i < strDrawText.length(); ++i)
		{
			GLetter* cLetter = cFont->getLetter(strDrawText[i]);
			newWidth += cLetter->getWidth();
		}

		newHeight = cFont->getMaxHeight();
		dimRatio = (((float)(getHeight())) / ((float)(newHeight)));
		strWidth = dimRatio * newWidth;
	}

	if (getType() == "RUTextbox")
	{
		// Chop the text
		if (cursor.maxLen)
			strDrawText = strDrawText.substr(cursor.index, cursor.maxLen);

		for (unsigned int i = 0; i < strDrawText.length(); ++i)
		{
			GLetter* cLetter = cFont->getLetter(strDrawText[i]);
			newWidth += cLetter->getWidth();
		}

		// Text dimensions
		newHeight = cFont->getMaxHeight();
		dimRatio = (((float)(getHeight())) / ((float)(newHeight)));
		strWidth = dimRatio * newWidth;

		// We have to chop/grow the text
		if (strWidth > getWidth())
		{
			if (cursor.maxLen == 0)
			{
				// First time overflowing in the textbox
				std::string testText = text.substr(1, strDrawText.length() - 1);
				cursor.maxLen = strDrawText.length() - 1;
				cursor.index = 1;
				--cursor.cursorIndex; // Special case because of rendering
			}
			else if (cursor.maxLen > 0)
			{
				if (cursor.cursorIndex > 0)
				{
					// Compensate for big characters
					std::string testText = text.substr(cursor.index + 1, cursor.maxLen - 1);
					--cursor.maxLen;
					++cursor.index;
					--cursor.cursorIndex; // Special case because of rendering
				}
			}
		}
		else
		{
			// Compensate for small characters where large characters used to be
			if (cursor.maxLen > 1)
			{
				if (text.length() > strDrawText.length() + 1)
				{
					if (cursor.cursorIndex == cursor.maxLen)
					{
						std::string testText = text[cursor.index - 1] + strDrawText;
						GLetter* cLetter = cFont->getLetter(text[cursor.index - 1]);
						float testWidth = newWidth + cLetter->getWidth();
						testWidth = dimRatio * testWidth;
						if (testWidth < getWidth())
						{
							--cursor.index;
							++cursor.maxLen;
							++cursor.cursorIndex;
						}
					}
					else if ((cursor.cursorIndex == 0) &&
							 (cursor.index + cursor.maxLen + 1 < text.length()))
					{
						std::string testText = strDrawText + text[cursor.index + cursor.maxLen + 1];
						GLetter* cLetter = cFont->getLetter(text[cursor.index + cursor.maxLen + 1]);
						float testWidth = newWidth + cLetter->getWidth();
						testWidth = dimRatio * testWidth;
						if (testWidth < getWidth())
						{
							//--cursor.index;
							++cursor.maxLen;
						}
					}
				}
			}
		}

		// Cursor dimensions
		if (cursor.cursorIndex <= strDrawText.length())
		{
			newWidth = 0;
			for (unsigned int i = 0; i < cursor.cursorIndex; ++i)
				newWidth += cFont->getLetter(strDrawText[i])->getWidth();

			cursorX = dimRatio * newWidth;
		}
	}
}

void RUTextComponent::drawText(gfxpp* cGfx)
{
	if (!cGfx)
		return;

	GFont* cFont = cGfx->cFont;
	if (!cFont)
		return;

	float cursorYGap = (getHeight() - cFont->getFontSize());
	calculateRenderInfo(cFont);

	// Draw the string
	if (strDrawText.length() > 0)
	{
		// Check the font
		if (!cFont->getFont())
		{
			printf("[GUI] TTF Font load error 2: %s\n", SDL_GetError());
			return;
		}

		SDL_Rect textRect;
		textRect.x = 0;
		textRect.y = 0;
		textRect.w = 0;
		textRect.h = getHeight();

		SDL_SetRenderTarget(cGfx->getRenderer(), getBackground());
		for (unsigned int i = 0; i < strDrawText.length(); ++i)
		{
			GLetter* cLetter = cFont->getLetter(strDrawText[i]);
			textRect.w = dimRatio * cLetter->getWidth();

			SDL_RenderCopy(cGfx->getRenderer(), cLetter->getTexture(), NULL, &textRect);
			textRect.x += textRect.w;
		}
	}

	drawCursor(cGfx, cursorYGap);
}

void RUTextComponent::drawCursor(gfxpp* cGfx, float cursorYGap)
{
	if (!cGfx)
		return;

	GFont* cFont = cGfx->cFont;
	if (!cFont)
		return;

	if (readOnly)
		return;

	if (!isFocused())
		return;

	unsigned int cursorCounter = (time(NULL) - cursorStart) % 2;
	if ((cursorStart > 0) && (cursorCounter == 0))
	{
		SDL_SetRenderDrawColor(cGfx->getRenderer(), cFont->getTextColor().r,
							   cFont->getTextColor().g, cFont->getTextColor().b,
							   cFont->getTextColor().a);

		SDL_Rect cursorRect;
		cursorRect.x = cursorX;
		cursorRect.y = cursorYGap / 2.0f;
		cursorRect.w = 2;
		cursorRect.h = ((float)height) - cursorYGap;

		SDL_RenderFillRect(cGfx->getRenderer(), &cursorRect);
	}

	drawUpdate = true;
}

void RUTextComponent::setKeyListener(void (GPanel::*f)(char))
{
	KeyListener = f;
}

void RUTextComponent::onMouseDown(gfxpp* cGfx, GPanel* cPanel, int eventX, int eventY)
{
	// printf("RUTextComponent: onMouseDown(%d, %d);\n", eventX, eventY);
	cursorStart = time(NULL);
	drawUpdate = true;
}

void RUTextComponent::onKey(gfxpp* cGfx, char eventKeyPressed)
{
	// printf("RUTextComponent: onKey(%c);\n", eventKeyPressed);
}

bool RUTextComponent::onKeyHelper(gfxpp* cGfx, GPanel* cPanel, SDL_Keycode eventKeyPressed,
								  Uint16 eventKeyModPressed)
{
	bool typed = false;

	if (!cPanel)
		return typed;

	char eventChar = GFont::keycodeTOchar(eventKeyPressed);

	// make the character caps
	if ((eventKeyModPressed & KMOD_SHIFT) || (eventKeyModPressed & KMOD_LSHIFT) ||
		(eventKeyModPressed & KMOD_RSHIFT))
		eventChar = shmea::GType::toUpper(eventChar);

	// toggle the case because of caps lock
	if (eventKeyModPressed & KMOD_CAPS)
		eventChar = shmea::GType::toggleCase(eventChar);

	// write to the text component
	if (!readOnly)
	{
		// interact with the component
		if (eventKeyPressed == SDLK_BACKSPACE)
		{
			if ((text.length() > 0) && ((cursor.index > 0) || (cursor.cursorIndex > 0)))
			{

				// Delete the character
				if (cursor.index + cursor.cursorIndex == cursor.maxLen)
					text = text.substr(0, cursor.index + cursor.cursorIndex - 1);
				else
					text = text.substr(0, cursor.index + cursor.cursorIndex - 1) +
						   text.substr(cursor.index + cursor.cursorIndex);

				// Shift the cursor
				if ((cursor.cursorIndex) && (cursor.index == 0))
					--cursor.cursorIndex;
				else if (cursor.index)
					--cursor.index;

				if (cursor.index + cursor.cursorIndex < cursor.maxLen)
					cursor.maxLen = 0;
			}
		}
		else if (eventKeyPressed == SDLK_DELETE)
		{
			if ((text.length() > 0) && (cursor.index + cursor.cursorIndex < text.length()))
			{
				// text = text.substr(0, cursor.index + cursor.cursorIndex -1) +
				// text.substr(cursor.index +
				// cursor.cursorIndex - 1);
			}
		}
		else if ((eventKeyPressed == SDLK_UP) || (eventKeyPressed == SDLK_HOME))
		{
			// cursorStart = time(NULL);
		}
		else if ((eventKeyPressed == SDLK_DOWN) || (eventKeyPressed == SDLK_END))
		{
			// cursorStart = time(NULL);
		}
		else if (eventKeyPressed == SDLK_LEFT)
		{
			if (cursor.cursorIndex)
				--cursor.cursorIndex;
			else if (cursor.index)
				--cursor.index;

			cursorStart = time(NULL);
		}
		else if (eventKeyPressed == SDLK_RIGHT)
		{
			if (cursor.maxLen == 0)
			{
				if (cursor.cursorIndex < text.length())
					++cursor.cursorIndex;
			}
			else if (cursor.maxLen > 0)
			{
				if (cursor.index + cursor.cursorIndex < text.length())
				{
					if (cursor.cursorIndex == cursor.maxLen)
						++cursor.index;
					else if (cursor.cursorIndex < cursor.maxLen)
						++cursor.cursorIndex;
				}
			}

			cursorStart = time(NULL);
		}
		else
		{
			if (GFont::validChar(eventChar))
			{
				// Handle special characters manually
				// Not sure why SDL_Keycode is not detecting special chars
				if ((eventKeyModPressed & KMOD_SHIFT) || (eventKeyModPressed & KMOD_LSHIFT) ||
					(eventKeyModPressed & KMOD_RSHIFT))
				{
					eventChar = GFont::specialChar(eventChar);
				}

				// Insert the Character
				text = text.substr(0, cursor.index + cursor.cursorIndex) + eventChar +
					   text.substr(cursor.index + cursor.cursorIndex);

				if (cursor.maxLen)
					++cursor.index;
				else
					++cursor.cursorIndex;
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

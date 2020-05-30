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

	text = newText;
	strDrawText = "";
	strWidth = 0.0f;
	cursorX = 0;
	cursor.reset();
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
		dimRatio = (((float)(getHeight())) / ((float)(newHeight)));

		// Text has not tested it bounds
		do
		{
			if ((strWidth < getWidth()) && (cursor.index + cursor.maxLen + 1 <= text.length()))
				++cursor.maxLen;

			bool cursorSet = false;
			int newWidth = 0;
			strDrawText = text.substr(cursor.index, cursor.maxLen);
			for (unsigned int i = 0; i < strDrawText.length(); ++i)
			{
				if (i == cursor.cursorIndex)
				{
					cursorX = dimRatio * newWidth;
					cursorSet = true;
				}

				GLetter* cLetter = cFont->getLetter(strDrawText[i]);
				newWidth += cLetter->getWidth();
			}

			strWidth = dimRatio * newWidth;
			if (!cursorSet)
				cursorX = strWidth;

		} while ((strWidth < getWidth()) && (cursor.index + cursor.maxLen + 1 <= text.length()));
		printf("SHMEA0[%s]: %d:%f\n", strDrawText.c_str(), strWidth, getWidth());
		printf("SHMEA1[%s]: %d:%d:%d:%ld\n", strDrawText.c_str(), cursor.index, cursor.maxLen,
			   cursor.cursorIndex, text.length());

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
			strDrawText = text.substr(cursor.index, cursor.maxLen);
			for (unsigned int i = 0; i < strDrawText.length(); ++i)
			{
				GLetter* cLetter = cFont->getLetter(strDrawText[i]);
				newWidth += cLetter->getWidth();
			}

			strWidth = dimRatio * newWidth;
		}
		printf("SHMEA2[%s]: %d:%f\n", strDrawText.c_str(), strWidth, getWidth());
		printf("SHMEA3[%s]: %d:%d:%d:%ld\n", strDrawText.c_str(), cursor.index, cursor.maxLen,
			   cursor.cursorIndex, text.length());
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
		else if (eventKeyPressed == SDLK_DELETE)
		{
			if ((text.length() > 0) && (cursor.index + cursor.cursorIndex < text.length()))
			{
				text = text.substr(0, cursor.index + cursor.cursorIndex) +
					   text.substr(cursor.index + cursor.cursorIndex + 1);

				if (cursor.index + cursor.cursorIndex < cursor.maxLen)
					cursor.maxLen = 0;
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
				// Handle special characters manually
				// Not sure why SDL_Keycode is not detecting special chars
				if ((eventKeyModPressed & KMOD_SHIFT) || (eventKeyModPressed & KMOD_LSHIFT) ||
					(eventKeyModPressed & KMOD_RSHIFT))
				{
					eventChar = GFont::specialChar(eventChar);
				}

				// Insert the Character
				if (cursor.index + cursor.cursorIndex == text.length())
				{
					text += eventChar;
				}
				else
					text = text.substr(0, cursor.index + cursor.cursorIndex) + eventChar +
						   text.substr(cursor.index + cursor.cursorIndex);

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

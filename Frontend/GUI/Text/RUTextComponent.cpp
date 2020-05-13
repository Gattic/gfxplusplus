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
	cursorXGap = 0.0f;
	cursorYGap = 0.0f;
	cursorX = 0;
	dimRatio = 0.0f;
	boxInnerIndex = 0;
	boxIndex = 0;
	boxLen = 0;
	passwordChar = '*';
	passwordField = false;
	cursorStart = 0;
	readOnly = true;

	// event listeners
	KeyListener = 0;
	cFont = NULL;
}

RUTextComponent::~RUTextComponent()
{
	text = "";
	strDrawText = "";
	strWidth = 0.0f;
	cursorXGap = 0.0f;
	cursorYGap = 0.0f;
	cursorX = 0;
	dimRatio = 0.0f;
	boxInnerIndex = 0;
	boxIndex = 0;
	boxLen = 0;
	passwordChar = '*';
	passwordField = false;
	readOnly = true;

	// event listeners
	KeyListener = 0;

	cFont = NULL;
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

	// 0 means no boxLen
	if ((boxLen > 0) && (newText.length() > boxLen))
		return;

	text = newText;
	boxInnerIndex = 0;
	boxIndex = 0;
	boxLen = 0;
	strWidth = 0.0;
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

void RUTextComponent::setFont(GFont* newFont)
{
	cFont = newFont;
	drawUpdate = true;
}

void RUTextComponent::calculateRenderInfo()
{
	if ((!cFont) || (!cFont->getFont()))
		return;

	// Font letter w/h
	int newWidth = 0;
	int newHeight = 0;
	strDrawText = text;
	cursorYGap = (getHeight() - cFont->getFontSize());

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
		TTF_SizeText(cFont->getFont(), strDrawText.c_str(), &newWidth, &newHeight);
		dimRatio = (((float)(getHeight())) / ((float)(newHeight)));
		strWidth = dimRatio * newWidth;
	}

	if (getType() == "RUTextbox")
	{
		// printf("Text: %s\n", text.c_str());
		/*printf("%s(%ld): (%u,%u,%u)\n", strDrawText.c_str(), strDrawText.length(), boxIndex,
			   boxInnerIndex, boxLen);*/

		// Chop the text
		if (boxLen)
			strDrawText = strDrawText.substr(boxIndex, boxLen);

		// Text dimensions
		TTF_SizeText(cFont->getFont(), strDrawText.c_str(), &newWidth, &newHeight);
		dimRatio = (((float)(getHeight())) / ((float)(newHeight)));
		strWidth = dimRatio * newWidth;

		// We have to chop/grow the text
		if (strWidth > getWidth())
		{
			if (boxLen == 0)
			{
				// First time overflowing in the textbox
				boxLen = strDrawText.length() - 1;
				boxIndex = 1;
			}
			else if (boxLen > 0)
			{
				// Compensate for big characters
				--boxLen;
				++boxIndex;
			}

			--boxInnerIndex; // Special case because of rendering
		}
		else
		{
			// Compensate for small characters where large characters used to be
			if ((boxLen > 1) && (boxIndex + boxInnerIndex + boxLen < text.length()))
			{
				// if(CanLeft()) Left()
			}
		}

		// Cursor dimensions
		TTF_SizeText(cFont->getFont(), strDrawText.substr(0, boxInnerIndex).c_str(), &newWidth,
					 &newHeight);
		float cursorDimRatio = (((float)(getHeight())) / ((float)(newHeight)));
		cursorX = cursorDimRatio * newWidth;
	}
}

void RUTextComponent::drawText(gfxpp* cGfx)
{
	if ((!cFont) || (!cFont->getFont()))
		return;

	calculateRenderInfo();

	// Draw the string
	if (strDrawText.length() > 0)
	{
		// Check the font
		if (!cFont->getFont())
		{
			printf("[GUI] TTF Font load error 2: %s\n", SDL_GetError());
			return;
		}

		SDL_Surface* textMessage =
			TTF_RenderText_Solid(cFont->getFont(), strDrawText.c_str(), cFont->getTextColor());
		if (!textMessage)
		{
			printf("[GUI] Text create error: %s\n", SDL_GetError());
			return;
		}

		//
		SDL_Texture* textTex = SDL_CreateTextureFromSurface(cGfx->getRenderer(), textMessage);
		if (!textTex)
		{
			printf("[GUI] Texture error: %s\n", SDL_GetError());
			if (textMessage)
				SDL_FreeSurface(textMessage);
			return;
		}

		SDL_Rect textRect;
		textRect.x = 0;
		textRect.y = 0;
		textRect.w = strWidth;
		textRect.h = getHeight();

		SDL_RenderCopy(cGfx->getRenderer(), textTex, NULL, &textRect);

		// Cleanup
		if (textMessage)
			SDL_FreeSurface(textMessage);
		if (textTex)
			SDL_DestroyTexture(textTex);
	}

	drawCursor(cGfx);
}

void RUTextComponent::drawCursor(gfxpp* cGfx)
{
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
			if ((text.length() > 0) && ((boxIndex > 0) || (boxInnerIndex > 0)))
			{

				// Delete the character
				if (boxIndex + boxInnerIndex == boxLen)
					text = text.substr(0, boxIndex + boxInnerIndex - 1);
				else
					text = text.substr(0, boxIndex + boxInnerIndex - 1) +
						   text.substr(boxIndex + boxInnerIndex);

				// Shift the cursor
				if ((boxInnerIndex) && (boxIndex == 0))
					--boxInnerIndex;
				else if (boxIndex)
					--boxIndex;

				if (boxIndex + boxInnerIndex < boxLen)
					boxLen = 0;
			}
		}
		else if (eventKeyPressed == SDLK_DELETE)
		{
			if ((text.length() > 0) && (boxIndex + boxInnerIndex < text.length()))
			{
				// text = text.substr(0, boxIndex + boxInnerIndex -1) + text.substr(boxIndex +
				// boxInnerIndex - 1);
			}
		}
		else if ((eventKeyPressed == SDLK_UP) || (eventKeyPressed == SDLK_HOME))
		{
			cursorStart = time(NULL);
		}
		else if ((eventKeyPressed == SDLK_DOWN) || (eventKeyPressed == SDLK_END))
		{
			cursorStart = time(NULL);
		}
		else if (eventKeyPressed == SDLK_LEFT)
		{
			if (boxInnerIndex)
				--boxInnerIndex;
			else if (boxIndex)
				--boxIndex;

			cursorStart = time(NULL);
		}
		else if (eventKeyPressed == SDLK_RIGHT)
		{
			if (boxLen == 0)
			{
				if (boxInnerIndex < text.length())
					++boxInnerIndex;
			}
			else if (boxLen > 0)
			{
				if (boxIndex + boxInnerIndex < text.length())
				{
					if (boxInnerIndex == boxLen)
						++boxIndex;
					else if (boxInnerIndex < boxLen)
						++boxInnerIndex;
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
				text = text.substr(0, boxIndex + boxInnerIndex) + eventChar +
					   text.substr(boxIndex + boxInnerIndex);

				if (boxLen)
					++boxIndex;
				else
					++boxInnerIndex;

				// Shift the cursor
				/*if (boxLen)
					++boxIndex;
				else
					++boxInnerIndex;

				if(boxIndex > 0)
					--boxIndex;

				if(boxInnerIndex < boxLen)
					++boxInnerIndex;*/
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

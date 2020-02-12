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
#include "../../../include/Backend/Database/gtype.h"
#include "../../GItems/RUColors.h"
#include "../../Graphics/graphics.h"

std::string RUTextComponent::FONT_PATH = "resources/fonts/carlenlund_helmet/Helmet-Regular.ttf";
TTF_Font* RUTextComponent::font = NULL;

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
	fontSize = DEFAULT_FONT_SIZE;
	cursorStart = 0;
	readOnly = true;

	// event listeners
	KeyListener = 0;

	setTextColor(RUColors::DEFAULT_TEXT_COLOR);
	if (!font)
	{
		font = TTF_OpenFont(FONT_PATH.c_str(), 100);
		if (!font)
		{
			printf("[GUI] TTF Font load error 1: %s\n", TTF_GetError());
			font = NULL;
		}
	}
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
	fontSize = DEFAULT_FONT_SIZE;
	readOnly = true;

	// event listeners
	KeyListener = 0;

	if (font)
		TTF_CloseFont(font);
	font = NULL;
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

SDL_Color RUTextComponent::getTextColor() const
{
	return textColor;
}

int RUTextComponent::getFontSize() const
{
	return fontSize;
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

void RUTextComponent::setTextColor(SDL_Color newTextColor)
{
	textColor = newTextColor;
	drawUpdate = true;
}

void RUTextComponent::setFontSize(int newFontSize)
{
	fontSize = newFontSize;
	drawUpdate = true;
}

void RUTextComponent::calculateRenderInfo()
{
	// Font letter w/h
	int newWidth = 0;
	int newHeight = 0;
	strDrawText = text;
	cursorYGap = (getHeight() - fontSize);

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
		TTF_SizeText(font, strDrawText.c_str(), &newWidth, &newHeight);
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
		TTF_SizeText(font, strDrawText.c_str(), &newWidth, &newHeight);
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
		TTF_SizeText(font, strDrawText.substr(0, boxInnerIndex).c_str(), &newWidth, &newHeight);
		float cursorDimRatio = (((float)(getHeight())) / ((float)(newHeight)));
		cursorX = cursorDimRatio * newWidth;
	}
}

void RUTextComponent::drawText(SDL_Renderer* renderer)
{
	calculateRenderInfo();

	// Draw the string
	if (strDrawText.length() > 0)
	{
		// Check the font
		if (!font)
		{
			printf("[GUI] TTF Font load error 2: %s\n", SDL_GetError());
			return;
		}

		SDL_Surface* textMessage = TTF_RenderText_Solid(font, strDrawText.c_str(), textColor);
		if (!textMessage)
		{
			printf("[GUI] Text create error: %s\n", SDL_GetError());
			return;
		}

		//
		SDL_Texture* textTex = SDL_CreateTextureFromSurface(renderer, textMessage);
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

		SDL_RenderCopy(renderer, textTex, NULL, &textRect);

		// Cleanup
		if (textMessage)
			SDL_FreeSurface(textMessage);
		if (textTex)
			SDL_DestroyTexture(textTex);
	}

	drawCursor(renderer);
}

void RUTextComponent::drawCursor(SDL_Renderer* renderer)
{
	if (!readOnly)
	{
		if (Graphics::focusedItem == this)
		{
			unsigned int cursorCounter = (time(NULL) - cursorStart) % 2;
			if ((cursorStart > 0) && (cursorCounter == 0))
			{
				SDL_SetRenderDrawColor(renderer, textColor.r, textColor.g, textColor.b,
									   textColor.a);

				SDL_Rect cursorRect;
				cursorRect.x = cursorX;
				cursorRect.y = cursorYGap / 2.0f;
				cursorRect.w = 2;
				cursorRect.h = ((float)height) - cursorYGap;

				SDL_RenderFillRect(renderer, &cursorRect);
			}

			drawUpdate = true;
		}
	}
}

void RUTextComponent::setKeyListener(void (GPanel::*f)(char))
{
	KeyListener = f;
}

void RUTextComponent::onMouseDown(GPanel* cPanel, int eventX, int eventY)
{
	// printf("RUTextComponent: onMouseDown(%d, %d);\n", eventX, eventY);
	cursorStart = time(NULL);
	drawUpdate = true;
}

void RUTextComponent::onKey(char eventKeyPressed)
{
	// printf("RUTextComponent: onKey(%c);\n", eventKeyPressed);
}

bool RUTextComponent::onKeyHelper(GPanel* cPanel, SDL_Keycode eventKeyPressed,
								  Uint16 eventKeyModPressed)
{
	bool typed = false;

	if (!cPanel)
		return typed;

	char eventChar = keycodeTOchar(eventKeyPressed);

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
			if (validChar(eventChar))
			{
				// Handle special characters manually
				// Not sure why SDL_Keycode is not detecting special chars
				if ((eventKeyModPressed & KMOD_SHIFT) || (eventKeyModPressed & KMOD_LSHIFT) ||
					(eventKeyModPressed & KMOD_RSHIFT))
				{
					eventChar = specialChar(eventChar);
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
		onKey(eventChar);

	if (KeyListener != 0)
		(cPanel->*KeyListener)(eventKeyPressed);

	typed = true;
	return typed;
}

bool RUTextComponent::validChar(char text)
{
	text = shmea::GType::toLower(text);
	switch (text)
	{
	// letters
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':

	// numbers
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':

	// symbols
	case ' ':
	case '+':
	case '-':
	case '_':
	case '!':
	case '@':
	case '#':
	case '$':
	case '*':
	case '?':
	case '^':
	case '(':
	case ')':
	case '&':
	case '.':
	case ',':
	case '<':
	case '>':
	case '/':
	case '\\':
	case ':':
	case ';':
	case '[':
	case ']':
	case '=':
	case '%':

		// valid character
		return true;

	default:
		return false;
	}
}

char RUTextComponent::specialChar(char keyPressed)
{
	switch (keyPressed)
	{
	case '0':
		return ')';
	case '1':
		return '!';
	case '2':
		return '@';
	case '3':
		return '#';
	case '4':
		return '$';
	case '5':
		return '%';
	case '6':
		return '^';
	case '7':
		return '&';
	case '8':
		return '*';
	case '9':
		return '(';
	case '`':
		return '~';
	case '-':
		return '_';
	case '=':
		return '+';
	case '[':
		return '{';
	case ']':
		return '}';
	case '\\':
		return '|';
	case ';':
		return ':';
	case '\'':
		return '"';
	case ',':
		return '<';
	case '.':
		return '>';
	case '/':
		return '?';

	default:
		return keyPressed; // Add char as is to text string
	}
}

char RUTextComponent::keycodeTOchar(SDL_Keycode keyPressed)
{
	switch (keyPressed)
	{
	// letters
	case SDLK_a:
		return 'a';
	case SDLK_b:
		return 'b';
	case SDLK_c:
		return 'c';
	case SDLK_d:
		return 'd';
	case SDLK_e:
		return 'e';
	case SDLK_f:
		return 'f';
	case SDLK_g:
		return 'g';
	case SDLK_h:
		return 'h';
	case SDLK_i:
		return 'i';
	case SDLK_j:
		return 'j';
	case SDLK_k:
		return 'k';
	case SDLK_l:
		return 'l';
	case SDLK_m:
		return 'm';
	case SDLK_n:
		return 'n';
	case SDLK_o:
		return 'o';
	case SDLK_p:
		return 'p';
	case SDLK_q:
		return 'q';
	case SDLK_r:
		return 'r';
	case SDLK_s:
		return 's';
	case SDLK_t:
		return 't';
	case SDLK_u:
		return 'u';
	case SDLK_v:
		return 'v';
	case SDLK_w:
		return 'w';
	case SDLK_x:
		return 'x';
	case SDLK_y:
		return 'y';
	case SDLK_z:
		return 'z';

	// numbers
	case SDLK_0:
		return '0';
	case SDLK_1:
		return '1';
	case SDLK_2:
		return '2';
	case SDLK_3:
		return '3';
	case SDLK_4:
		return '4';
	case SDLK_5:
		return '5';
	case SDLK_6:
		return '6';
	case SDLK_7:
		return '7';
	case SDLK_8:
		return '8';
	case SDLK_9:
		return '9';

	// symbols
	case SDLK_SPACE:
		return ' ';
	case SDLK_PLUS:
		return '+';
	case SDLK_MINUS:
		return '-';
	case SDLK_UNDERSCORE:
		return '_';
	case SDLK_EXCLAIM:
		return '!';
	case SDLK_AT:
		return '@';
	case SDLK_HASH:
		return '#';
	case SDLK_DOLLAR:
		return '$';
	case SDLK_ASTERISK:
		return '*';
	case SDLK_QUESTION:
		return '?';
	case SDLK_CARET:
		return '^';
	case SDLK_LEFTPAREN:
		return '(';
	case SDLK_RIGHTPAREN:
		return ')';
	case SDLK_AMPERSAND:
		return '&';
	case SDLK_PERIOD:
		return '.';
	case SDLK_COMMA:
		return ',';
	case SDLK_LESS:
		return '<';
	case SDLK_GREATER:
		return '>';
	case SDLK_SLASH:
		return '/';
	case SDLK_BACKSLASH:
		return '\\';
	case SDLK_COLON:
		return ':';
	case SDLK_SEMICOLON:
		return ';';
	case SDLK_LEFTBRACKET:
		return '[';
	case SDLK_RIGHTBRACKET:
		return ']';
	case SDLK_EQUALS:
		return '=';
	case SDLK_PERCENT:
		return '%';

	default:
		return 0x00; // no key
	}
}

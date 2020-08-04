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
#include "GFont.h"
#include "../../GItems/RUColors.h"
#include "Backend/Database/GType.h"

GFont::GFont()
{
	fontPath = "resources/fonts/Open_Sans/OpenSans-SemiBold.ttf";
	fontSize = DEFAULT_FONT_SIZE;
	font = TTF_OpenFont(fontPath.c_str(), 10 * fontSize);
	maxHeight = 0;

	if (!font)
	{
		printf("[GUI] TTF Font load error 1: %s\n", TTF_GetError());
		font = NULL;
	}

	setTextColor(RUColors::DEFAULT_TEXT_COLOR);
	loadLetters();
}

GFont::GFont(SDL_Renderer* newRenderer, std::string newFontPath)
{
	cRenderer = newRenderer;
	fontPath = newFontPath;
	if (fontPath.length() == 0)
		fontPath = "resources/fonts/Open_Sans/OpenSans-SemiBold.ttf";

	fontSize = DEFAULT_FONT_SIZE;
	font = TTF_OpenFont(fontPath.c_str(), 10 * fontSize);
	maxHeight = 0;

	if (!font)
	{
		printf("[GUI] TTF Font load error 2: %s\n", TTF_GetError());
		font = NULL;
	}

	setTextColor(RUColors::DEFAULT_TEXT_COLOR);
	loadLetters();
}

GFont::GFont(const GFont& otherFont)
{
	fontPath = otherFont.fontPath;
	font = otherFont.font;
	fontSize = otherFont.fontSize;
	textColor = otherFont.textColor;
	textureMap = otherFont.textureMap;
	cRenderer = otherFont.cRenderer;
	maxHeight = otherFont.maxHeight;
}

GFont::~GFont()
{
	maxHeight = 0;
	fontPath = "";
	fontSize = DEFAULT_FONT_SIZE;

	//if (font)
	//	TTF_CloseFont(font);
	font = NULL;

	// DONT FREE THIS
	cRenderer = NULL;
}

void GFont::loadLetters()
{
	if (!font)
		return;

	if (!cRenderer)
		return;

	maxHeight = 0;
	for (unsigned char i = 1; i != 0; ++i)
	{
		SDL_Surface* textMessage =
			TTF_RenderText_Solid(font, shmea::GType::charTOstring(i).c_str(), textColor);

		if (!textMessage)
		{
			printf("[GUI] GFont create error[%d]: %s\n", i, SDL_GetError());
			continue;
		}

		SDL_Texture* textTex = SDL_CreateTextureFromSurface(cRenderer, textMessage);
		if (!textTex)
		{
			printf("[GUI] Texture error: %s\n", SDL_GetError());
			if (textMessage)
				SDL_FreeSurface(textMessage);
			return;
		}

		// Letter texture dimensions
		int newWidth = 0;
		int newHeight = 0;
		std::string letterAsString = "";
		letterAsString += shmea::GType::charTOstring(i);
		TTF_SizeText(getFont(), letterAsString.c_str(), &newWidth, &newHeight);

		if (newHeight > maxHeight)
			maxHeight = newHeight;

		// Create the new letters
		GLetter* cLetter = new GLetter(i, textTex, newWidth);
		textureMap[i] = cLetter;
	}
}

SDL_Color GFont::getTextColor() const
{
	return textColor;
}

TTF_Font* GFont::getFont() const
{
	return font;
}
GLetter* GFont::getLetter(char cLetter) const
{
	std::map<char, GLetter*>::const_iterator itr = textureMap.find(cLetter);
	if (itr == textureMap.end())
		return NULL;

	return itr->second;
}

int GFont::getMaxHeight() const
{
	return maxHeight;
}

int GFont::getFontSize() const
{
	return fontSize;
}

std::string GFont::getFontPath() const
{
	return fontPath;
}

void GFont::setFontSize(int newFontSize)
{
	fontSize = newFontSize;
}

void GFont::setTextColor(SDL_Color newTextColor)
{
	textColor = newTextColor;
	loadLetters();
}

bool GFont::validChar(char text)
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

char GFont::specialChar(char keyPressed)
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

char GFont::keycodeTOchar(SDL_Keycode keyPressed)
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

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
#include "Backend/Database/GString.h"
#include "../../Graphics/GfxRenderer.h"

GFont::GFont()
{
	fontPath = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";
	fontSize = DEFAULT_FONT_SIZE;
	font = NULL;
	ownsTTF = false;
#ifdef GFX_HAVE_SDL2
	if (TTF_WasInit())
	{
		font = TTF_OpenFont(fontPath.c_str(), 10 * fontSize);
		if (!font)
		{
			const char* sysFonts[] = {
				"/usr/share/fonts/truetype/freefont/FreeSans.ttf",
				"/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
				"/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
				"resources/fonts/osaka-re.ttf",
				"resources/fonts/Open_Sans/OpenSans-SemiBold.ttf"
			};
			for (size_t i = 0; (!font) && (i < (sizeof(sysFonts)/sizeof(sysFonts[0]))); ++i)
			{
				TTF_Font* tryFont = TTF_OpenFont(sysFonts[i], 10 * fontSize);
				if (tryFont)
				{
					font = tryFont;
					fontPath = sysFonts[i];
				}
			}
		}
	}
	maxHeight = 0;

	if (TTF_WasInit() && !font)
	{
		printf("[GUI] TTF Font load error 1: %s\n", TTF_GetError());
		font = NULL;
	}
#else
	maxHeight = 0;
#endif

	setTextColor(RUColors::DEFAULT_TEXT_COLOR);
	loadLetters();
}

GFont::GFont(GfxNativeRenderer* newRenderer, shmea::GString newFontPath)
{
	cRenderer = newRenderer;
	fontPath = newFontPath;
	if (fontPath.length() == 0)
		fontPath = "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf";

	fontSize = DEFAULT_FONT_SIZE;
	font = NULL;
	ownsTTF = false;
#ifdef GFX_HAVE_SDL2
	if (TTF_WasInit())
	{
		font = TTF_OpenFont(fontPath.c_str(), 10 * fontSize);
		if (!font)
		{
			const char* sysFonts[] = {
				"/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
				"/usr/share/fonts/truetype/freefont/FreeSans.ttf",
				"/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf",
				"/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf",
				"resources/fonts/osaka-re.ttf",
				"resources/fonts/Open_Sans/OpenSans-SemiBold.ttf"
			};
			for (size_t i = 0; (!font) && (i < (sizeof(sysFonts)/sizeof(sysFonts[0]))); ++i)
			{
				TTF_Font* tryFont = TTF_OpenFont(sysFonts[i], 10 * fontSize);
				if (tryFont)
				{
					font = tryFont;
					fontPath = sysFonts[i];
				}
			}
		}
	}
	maxHeight = 0;

	if (TTF_WasInit() && !font)
	{
		printf("[GUI] TTF Font load error 2: %s\n", TTF_GetError());
		font = NULL;
	}
#else
	maxHeight = 0;
#endif

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

#ifdef GFX_HAVE_SDL2
	if (font && TTF_WasInit() && ownsTTF)
		TTF_CloseFont(font);
#endif
	font = NULL;

	for (std::map<char, GLetter*>::iterator itr = textureMap.begin(); itr != textureMap.end(); ++itr)
	{
		delete itr->second;
	}

	textureMap.clear();

	// DONT FREE THIS
	cRenderer = NULL;
}

void GFont::loadLetters()
{
#ifdef GFX_HAVE_SDL2
	if (!font || !cRenderer)
		return;

	maxHeight = 0;
	for (unsigned char i = 1; i != 0; ++i)
	{
		GfxSurface* textMessage =
			TTF_RenderText_Solid(font, shmea::GString::charTOstring(i).c_str(), textColor);

		if (!textMessage)
		{
			printf("[GUI] GFont create error[%d]: %s\n", i, GFX_GetError());
			continue;
		}

		GfxTexture* textTex = NULL;
		// If a renderer is passed through elsewhere, we could use it; fallback to native path here
		textTex = GFX_CreateTextureFromSurface(cRenderer, textMessage);
		if (textMessage)
			GFX_FreeSurface(textMessage);
		if (!textTex)
		{
			printf("[GUI] Texture error: %s\n", GFX_GetError());
			return;
		}

		// Letter texture dimensions
		int newWidth = 0;
		int newHeight = 0;
		shmea::GString letterAsString = "";
		letterAsString += shmea::GString::charTOstring((char)i);
		TTF_SizeText(getFont(), letterAsString.c_str(), &newWidth, &newHeight);

		if (newHeight > maxHeight)
			maxHeight = newHeight;

		// Create the new letters
		GLetter* cLetter = new GLetter(i, textTex, newWidth);
		textureMap[i] = cLetter;
	}
#else
	// No SDL_ttf path in OpenGL-only; keep map empty and maxHeight 0
	(void)textColor;
	(void)cRenderer;
	maxHeight = 0;
#endif
}

GfxColor GFont::getTextColor() const
{
	return textColor;
}

TTF_Font_Type* GFont::getFont() const
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

shmea::GString GFont::getFontPath() const
{
	return fontPath;
}

void GFont::setFontSize(int newFontSize)
{
	fontSize = newFontSize;
}

void GFont::setTextColor(GfxColor newTextColor)
{
	textColor = newTextColor;
	loadLetters();
}

//
bool GFont::validChar(char text)
{
	text = shmea::GString::toLower(text);
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

char GFont::keycodeTOchar(GfxKeycode keyPressed)
{
	switch (keyPressed)
	{
	// letters
	case GFXK_a:
		return 'a';
	case GFXK_b:
		return 'b';
	case GFXK_c:
		return 'c';
	case GFXK_d:
		return 'd';
	case GFXK_e:
		return 'e';
	case GFXK_f:
		return 'f';
	case GFXK_g:
		return 'g';
	case GFXK_h:
		return 'h';
	case GFXK_i:
		return 'i';
	case GFXK_j:
		return 'j';
	case GFXK_k:
		return 'k';
	case GFXK_l:
		return 'l';
	case GFXK_m:
		return 'm';
	case GFXK_n:
		return 'n';
	case GFXK_o:
		return 'o';
	case GFXK_p:
		return 'p';
	case GFXK_q:
		return 'q';
	case GFXK_r:
		return 'r';
	case GFXK_s:
		return 's';
	case GFXK_t:
		return 't';
	case GFXK_u:
		return 'u';
	case GFXK_v:
		return 'v';
	case GFXK_w:
		return 'w';
	case GFXK_x:
		return 'x';
	case GFXK_y:
		return 'y';
	case GFXK_z:
		return 'z';

	// numbers
	case GFXK_0:
		return '0';
	case GFXK_1:
		return '1';
	case GFXK_2:
		return '2';
	case GFXK_3:
		return '3';
	case GFXK_4:
		return '4';
	case GFXK_5:
		return '5';
	case GFXK_6:
		return '6';
	case GFXK_7:
		return '7';
	case GFXK_8:
		return '8';
	case GFXK_9:
		return '9';

	// symbols
	case GFXK_SPACE:
		return ' ';
	case GFXK_PLUS:
		return '+';
	case GFXK_MINUS:
		return '-';
	case GFXK_UNDERSCORE:
		return '_';
	case GFXK_EXCLAIM:
		return '!';
	case GFXK_AT:
		return '@';
	case GFXK_HASH:
		return '#';
	case GFXK_DOLLAR:
		return '$';
	case GFXK_ASTERISK:
		return '*';
	case GFXK_QUESTION:
		return '?';
	case GFXK_CARET:
		return '^';
	case GFXK_LEFTPAREN:
		return '(';
	case GFXK_RIGHTPAREN:
		return ')';
	case GFXK_AMPERSAND:
		return '&';
	case GFXK_PERIOD:
		return '.';
	case GFXK_COMMA:
		return ',';
	case GFXK_LESS:
		return '<';
	case GFXK_GREATER:
		return '>';
	case GFXK_SLASH:
		return '/';
	case GFXK_BACKSLASH:
		return '\\';
	case GFXK_COLON:
		return ':';
	case GFXK_SEMICOLON:
		return ';';
	case GFXK_LEFTBRACKET:
		return '[';
	case GFXK_RIGHTBRACKET:
		return ']';
	case GFXK_EQUALS:
		return '=';
	case GFXK_PERCENT:
		return '%';

	default:
		return 0x00; // no key
	}
}

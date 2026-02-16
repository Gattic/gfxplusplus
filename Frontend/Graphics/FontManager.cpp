// Copyright 2026 Robert Carneiro, Derek Meer, Matthew Tabak, Eric Lujan
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
#include "FontManager.h"
#include "../GUI/Text/GFont.h"
#include "../GItems/RUColors.h"
#ifdef GFX_HAVE_OPENGL
#include "../GUI/Text/GLTextRenderer.h"
#endif
#include <stdio.h>

FontManager::FontManager()
{
}

void FontManager::initSDL(GfxNativeRenderer* sdlRenderer)
{
	if (sdlRenderer)
	{
		cFont = shmea::GPointer<GFont>(new GFont(sdlRenderer));
		graphicsFonts.insert(std::pair<int, shmea::GPointer<GFont> >(0, cFont));

		shmea::GPointer<GFont> fontGreen(new GFont(sdlRenderer));
		fontGreen->setTextColor(RUColors::TEXT_COLOR_GREEN);
		graphicsFonts.insert(std::pair<int, shmea::GPointer<GFont> >(1, fontGreen));

		shmea::GPointer<GFont> fontRed(new GFont(sdlRenderer));
		fontRed->setTextColor(RUColors::TEXT_COLOR_RED);
		graphicsFonts.insert(std::pair<int, shmea::GPointer<GFont> >(2, fontRed));
	}
	else
	{
		initOpenGL();
	}
}

void FontManager::initOpenGL()
{
	cFont = shmea::GPointer<GFont>(new GFont());
	graphicsFonts.insert(std::pair<int, shmea::GPointer<GFont> >(0, cFont));

	shmea::GPointer<GFont> fontGreen(new GFont());
	fontGreen->setTextColor(RUColors::TEXT_COLOR_GREEN);
	graphicsFonts.insert(std::pair<int, shmea::GPointer<GFont> >(1, fontGreen));

	shmea::GPointer<GFont> fontRed(new GFont());
	fontRed->setTextColor(RUColors::TEXT_COLOR_RED);
	graphicsFonts.insert(std::pair<int, shmea::GPointer<GFont> >(2, fontRed));
}

void FontManager::cleanup(bool isOpenGL)
{
	graphicsFonts.clear();
	cFont.reset();
#ifdef GFX_HAVE_OPENGL
	if (isOpenGL)
	{
		for (std::map<std::string, GLTextRenderer*>::iterator it = glTextCache.begin(); it != glTextCache.end(); ++it)
			delete it->second;
		glTextCache.clear();
	}
#else
	(void)isOpenGL;
#endif
}

GFont* FontManager::getDefaultFont()
{
	return cFont;
}

GFont* FontManager::getFontByColor(int fontColor)
{
	std::map<int, shmea::GPointer<GFont> >::iterator it = graphicsFonts.find(fontColor);
	if (it != graphicsFonts.end())
		return it->second;
	return NULL;
}

#ifdef GFX_HAVE_OPENGL
GLTextRenderer* FontManager::getGLText(const std::string& fontPath, int pixelHeight)
{
	char keyBuf[1024];
	keyBuf[0] = '\0';
	snprintf(keyBuf, sizeof(keyBuf)-1, "%s|%d", fontPath.c_str(), pixelHeight);
	std::string key(keyBuf);

	std::map<std::string, GLTextRenderer*>::iterator it = glTextCache.find(key);
	if (it != glTextCache.end())
		return it->second;

	GLTextRenderer* glRenderer = new GLTextRenderer();
	if (!glRenderer->init(fontPath, pixelHeight))
	{
		delete glRenderer;
		return NULL;
	}
	glTextCache.insert(std::make_pair(key, glRenderer));
	return glRenderer;
}
#endif

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
#include "RUBackgroundComponent.h"
#include "Backend/Database/image.h"
#include "Backend/Database/png-helper.h"
#include "../../Graphics/graphics.h"
#include "../../Graphics/GfxRenderer.h"
#include "../GItem.h"
#include "../RUColors.h"
#include <cmath>
#ifdef GFX_HAVE_OPENGL
#include <GLFW/glfw3.h>
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif
#define STB_IMAGE_IMPLEMENTATION
#include "../../stb_image.h"
#endif

RUBackgroundComponent::RUBackgroundComponent()
{
	bgEnabled = true;
	bgColorEnabled = true;
	surfaceTheUSA = NULL;
	bgImageLocation = DEFAULT_IMAGE_BG;
	bgImageType = TYPE_NONE;
	setBGColor(RUColors::COLOR_TRANSPARENT);
	//bgCache = shmea::GPointer<shmea::Image>(new shmea::Image());
#ifdef GFX_HAVE_OPENGL
	glTextureId = 0;
	glTextureWidth = 0;
	glTextureHeight = 0;
	glTextureValid = false;
#endif
}

RUBackgroundComponent::RUBackgroundComponent(GfxColor newBGColor)
{
	bgEnabled = true;
	bgColorEnabled = true;
	surfaceTheUSA = NULL;
	bgImageLocation = DEFAULT_IMAGE_BG;
	bgImageType = TYPE_NONE;
	setBGColor(newBGColor);
	//bgCache = shmea::GPointer<shmea::Image>(new shmea::Image());
#ifdef GFX_HAVE_OPENGL
	glTextureId = 0;
	glTextureWidth = 0;
	glTextureHeight = 0;
	glTextureValid = false;
#endif
}

RUBackgroundComponent::~RUBackgroundComponent()
{
	bgEnabled = false;
	bgColorEnabled = false;

#ifdef GFX_HAVE_SDL2
	if (surfaceTheUSA)
		GFX_FreeSurface(surfaceTheUSA);
#endif
	surfaceTheUSA = NULL;

	bgImageLocation = DEFAULT_IMAGE_BG;
	bgImageType = TYPE_NONE;
	//bgCache = shmea::GPointer<shmea::Image>(new shmea::Image());
#ifdef GFX_HAVE_OPENGL
	destroyGLTexture();
#endif
}

bool RUBackgroundComponent::resetSurface()
{
	if ((getWidth() == 0) || (getHeight() == 0))
		return false;

#ifdef GFX_HAVE_SDL2
	if (!surfaceTheUSA)
	{
		surfaceTheUSA = GFX_CreateRGBSurface(0, getWidth(), getHeight(), 32, rmask, gmask, bmask, amask);// 32 bits (depth)
		//bgCache->Allocate(getWidth(), getHeight());
	}
	else
	{
		// optional caching path
	}

	if (!surfaceTheUSA)
	{
		printf("[GUI] Surface create fail: %s\n", GFX_GetError());
		return false;
	}

	// Success
	GFX_FillRect(surfaceTheUSA, NULL, GFX_MapRGB(surfaceTheUSA->format, bgColor.r, bgColor.g, bgColor.b));
	GFX_SetSurfaceBlendMode(surfaceTheUSA, GFX_BLENDMODE_BLEND);

	return true;
#else
	(void)rmask; (void)gmask; (void)bmask; (void)amask; return false;
#endif
}

void RUBackgroundComponent::refreshImage()
{
	resetSurface();
	if (bgImageType == TYPE_FILE)
		setBGImageFromLocation(bgImageLocation);
	else if (bgImageType == TYPE_GIMAGE)
		setBGImage(bgImage);
}

void RUBackgroundComponent::fromSurface(GfxSurface* newSurfaceImage)
{
#ifdef GFX_HAVE_SDL2
	if (!newSurfaceImage)
		return;

	for (int y = 0; y < newSurfaceImage->h; ++y)
	{
		for (int x = 0; x < newSurfaceImage->w; ++x)
		{
			const unsigned char* surfacePixelData = (const unsigned char*)newSurfaceImage->pixels;
			unsigned int cPixel =
				*((unsigned int*)(surfacePixelData + (y * newSurfaceImage->pitch) +
							  (x * sizeof(unsigned int))));

			shmea::RGBA c;
			c.r = cPixel & rmask;
			c.g = cPixel & gmask;
			c.b = cPixel & bmask;
			c.a = cPixel & amask;
			bgImage->SetPixel(x, y, c);
		}
	}
#else
	(void)newSurfaceImage;
#endif
}

void RUBackgroundComponent::fromImage(shmea::GPointer<shmea::Image> newBGImage)
{
	bgImage = newBGImage;
	bgImageType = TYPE_GIMAGE;

#ifdef GFX_HAVE_SDL2
	GfxSurface* newImageSurface = GFX_CreateRGBSurface(0, bgImage->getWidth(), bgImage->getHeight(), 32, rmask, gmask, bmask, amask);
	if (!newImageSurface)
	{
		printf("[GUI] Surface create fail: %s\n", GFX_GetError());
		return;
	}

	// Success
	GFX_SetSurfaceBlendMode(newImageSurface, GFX_BLENDMODE_BLEND);

	for (unsigned int cy = 0; cy < bgImage->getHeight(); ++cy)
	{
		for (unsigned int cx = 0; cx < bgImage->getWidth(); ++cx)
		{
			shmea::RGBA c = bgImage->GetPixel(cx, cy);
			uint32_t cPixel = GFX_MapRGBA(newImageSurface->format, c.r, c.g, c.b, c.a);

			uint8_t* surfacePixelData = (uint8_t*)newImageSurface->pixels;
			*((unsigned int*)(surfacePixelData + (cy * newImageSurface->pitch) + (cx * newImageSurface->format->BytesPerPixel))) = cPixel;
		}
	}

	setBGImageFromSurface(newImageSurface);
#ifdef GFX_HAVE_OPENGL
	glTextureValid = false;
#endif
#endif
}

bool RUBackgroundComponent::getBGEnabled() const
{
	return bgEnabled;
}

bool RUBackgroundComponent::getBGColorEnabled() const
{
	return bgColorEnabled;
}

GfxColor RUBackgroundComponent::getBGColor() const
{
	return bgColor;
}

void RUBackgroundComponent::setBGImageFromLocation(const shmea::GString& newBGImageLocation)
{
	if (bgImageLocation.length() == 0)
		return;

	bgImageType = TYPE_FILE;
	bgImageLocation = newBGImageLocation;

#ifdef GFX_HAVE_SDL2
	GfxSurface* newSurfaceImage = GFX_IMG_Load(newBGImageLocation.c_str());
	if (!newSurfaceImage)
	{
		printf("[GFX] IMG_Load error: %s\n", GFX_IMG_GetError());
		bgImageType = TYPE_NONE;
		return;
	}

	setBGImageFromSurface(newSurfaceImage);
#endif
#ifdef GFX_HAVE_OPENGL
	// Use stb_image to load any supported image formats into RGBA
	int iw = 0, ih = 0, comp = 0;
	unsigned char* pixels = stbi_load(newBGImageLocation.c_str(), &iw, &ih, &comp, STBI_rgb_alpha);
	if (!pixels)
	{
		printf("[GFX] stb_image load failed: %s\n", newBGImageLocation.c_str());
		bgImageType = TYPE_NONE;
		return;
	}
	shmea::Image* loaded = new shmea::Image();
	loaded->Allocate((unsigned int)iw, (unsigned int)ih);
	const int stride = iw * 4;
	for (int y = 0; y < ih; ++y)
	{
		const unsigned char* row = pixels + y * stride;
		for (int x = 0; x < iw; ++x)
		{
			const unsigned char* p = row + x * 4;
			shmea::RGBA c(p[0], p[1], p[2], p[3]);
			loaded->SetPixel((unsigned int)x, (unsigned int)y, c);
		}
	}
	stbi_image_free(pixels);
	bgImage = shmea::GPointer<shmea::Image>(loaded);
	glTextureValid = false; // force reupload
#endif
}

void RUBackgroundComponent::setBGImageFromSurface(GfxSurface* newSurfaceImage)
{
#ifdef GFX_HAVE_SDL2
	if (!resetSurface())
		return;

	GfxSurface* optimizedSurface = GFX_ConvertSurface(newSurfaceImage, surfaceTheUSA->format, 0);
	GFX_FreeSurface(newSurfaceImage);

	if (GFX_BlitScaled(optimizedSurface, NULL, surfaceTheUSA, NULL) < 0)
	{
		printf("[GUI] Surface Blit error: %s\n", GFX_GetError());
		return;
	}

	GFX_FreeSurface(optimizedSurface);
#ifdef GFX_HAVE_OPENGL
	glTextureValid = false;
#endif
#else
	(void)newSurfaceImage;
#endif
}

void RUBackgroundComponent::toggleBG(bool newBGEnabled)
{
	bgEnabled = newBGEnabled;
	drawUpdate = true;
}

void RUBackgroundComponent::toggleBGColor(bool newBGColorEnabled)
{
	bgColorEnabled = newBGColorEnabled;
	drawUpdate = true;
}

void RUBackgroundComponent::setBGImage(shmea::GPointer<shmea::Image> newBGImage)
{
	if (!newBGImage)
		return;

	fromImage(newBGImage);
	drawUpdate = true;
}

void RUBackgroundComponent::setBGColor(GfxColor newBGColor)
{
	bgColor = newBGColor;
}

void RUBackgroundComponent::updateBGBackground(gfxpp* cGfx)
{
	if (!bgEnabled)
		return;

	if (!((getWidth() > 0) && (getHeight() > 0)))
		return;

	// draw the background
	GfxRect bgRect;
	bgRect.x = 0;
	bgRect.y = 0;
	bgRect.w = getWidth();
	bgRect.h = getHeight();

	if (bgColorEnabled)
	{
		if (cGfx->getDraw())
			drawVerticalGradient(cGfx->getDraw(), bgRect, getBGColor(), getBGColor(), 6);
	}

	// draw the background image
	if (!bgImageType)
		return;

#ifdef GFX_HAVE_SDL2
	if (!surfaceTheUSA)
		return;

	if ((surfaceTheUSA->w == 0) || (surfaceTheUSA->h == 0))
		return;

	GfxTexture* bgImageTex = NULL;
	if (cGfx->getDraw())
		bgImageTex = cGfx->getDraw()->createTextureFromSurface(surfaceTheUSA);
	if (!bgImageTex)
	{
		return;
	}

	// Draw the texture covering the full component rect and enable alpha blending
	if (cGfx->getDraw())
	{
		cGfx->getDraw()->setTextureBlendMode(bgImageTex, GFX_BLENDMODE_BLEND);
		cGfx->getDraw()->copyTexture(bgImageTex, NULL, &bgRect);
		GFX_DestroyTexture(bgImageTex);
	}
#endif

#ifdef GFX_HAVE_OPENGL
	// OpenGL textured quad path using png-helper data
	if (cGfx->getRenderBackend() == gfxpp::RENDER_BACKEND_OPENGL)
	{
		if (!bgImage)
			return;
		ensureGLTexture();
		if (!glTextureValid)
			return;

		// enable blending for alpha PNGs
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D, glTextureId);

		// Draw quad covering rect
		glColor4ub(255,255,255,255);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex2f((GLfloat)bgRect.x, (GLfloat)bgRect.y);
			glTexCoord2f(1.0f, 0.0f); glVertex2f((GLfloat)(bgRect.x + bgRect.w), (GLfloat)bgRect.y);
			glTexCoord2f(1.0f, 1.0f); glVertex2f((GLfloat)(bgRect.x + bgRect.w), (GLfloat)(bgRect.y + bgRect.h));
			glTexCoord2f(0.0f, 1.0f); glVertex2f((GLfloat)bgRect.x, (GLfloat)(bgRect.y + bgRect.h));
		glEnd();

		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}
#endif
}

#ifdef GFX_HAVE_OPENGL
void RUBackgroundComponent::destroyGLTexture()
{
	if (glTextureId)
	{
		glDeleteTextures(1, &glTextureId);
		glTextureId = 0;
	}
	glTextureWidth = 0;
	glTextureHeight = 0;
	glTextureValid = false;
}

void RUBackgroundComponent::ensureGLTexture()
{
	if (glTextureValid && glTextureId)
		return;
	if (!bgImage)
		return;

	// Upload bgImage to GL texture
	if (!glTextureId)
		glGenTextures(1, &glTextureId);
	glBindTexture(GL_TEXTURE_2D, glTextureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTextureWidth = (int)bgImage->getWidth();
	glTextureHeight = (int)bgImage->getHeight();

	std::vector<unsigned char> pixels = bgImage->getPixels();
	if (pixels.empty())
	{
		glBindTexture(GL_TEXTURE_2D, 0);
		glTextureValid = false;
		return;
	}

	// OpenGL expects RGBA byte order; Image::getPixels returns contiguous RGBA
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, glTextureWidth, glTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &pixels[0]);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTextureValid = true;
}
#endif

void RUBackgroundComponent::drawVerticalGradient(GfxRenderer* renderer, GfxRect rect, GfxColor color1, GfxColor color2, int cornerRadius)
{
	if (!renderer)
		return;

	if (rect.w <= 0 || rect.h <= 0)
		return;

	int rectX = rect.x;
	int rectY = rect.y;
	int rectW = rect.w;
	int rectH = rect.h;
	int rectXW = rectX + rectW - 1;
	int rectYH = rectY + rectH - 1;

	// Precompute color deltas
	float deltaR = static_cast<float>(color2.r - color1.r) / rectH;
	float deltaG = static_cast<float>(color2.g - color1.g) / rectH;
	float deltaB = static_cast<float>(color2.b - color1.b) / rectH;
	float deltaA = static_cast<float>(color2.a - color1.a) / rectH;

	float accR = static_cast<float>(color1.r);
	float accG = static_cast<float>(color1.g);
	float accB = static_cast<float>(color1.b);
	float accA = static_cast<float>(color1.a);

	int radius = cornerRadius > 0 ? cornerRadius : 0;
	int r2 = radius * radius;

	// Draw one horizontal scanline per row
	for (int i = 0; i < rectH; ++i)
	{
		int y = rectY + i;

		Uint8 r = static_cast<Uint8>(accR);
		Uint8 g = static_cast<Uint8>(accG);
		Uint8 b = static_cast<Uint8>(accB);
		Uint8 a = static_cast<Uint8>(accA);

		accR += deltaR;
		accG += deltaG;
		accB += deltaB;
		accA += deltaA;

		int inset = 0;
		if (radius > 0)
		{
			int topOverlap = (rectY + radius) - y;
			int bottomOverlap = y - (rectYH - radius);

			int dy = 0;
			if (topOverlap > 0)
				dy = topOverlap;
			else if (bottomOverlap > 0)
				dy = bottomOverlap;

			if (dy > 0 && dy < radius)
			{
				int dx = static_cast<int>(std::floor(std::sqrt(static_cast<double>(r2 - dy * dy))));
				inset = radius - dx;
				if (inset < 0) inset = 0;
				if (inset * 2 >= rectW)
					continue;
			}
		}

		int xStart = rectX + inset;
		int xEnd = rectXW - inset;
		if (xStart > xEnd)
			continue;

		GfxRect lineRect;
		lineRect.x = xStart;
		lineRect.y = y;
		lineRect.w = (xEnd - xStart + 1);
		lineRect.h = 1;

		renderer->setDrawColor(r, g, b, a);
		renderer->fillRect(&lineRect);
	}
}

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
#include "RUBackgroundComponent.h"
#include "Backend/Database/image.h"
#include "../../Graphics/graphics.h"
#include "../GItem.h"
#include "../RUColors.h"

RUBackgroundComponent::RUBackgroundComponent()
{
	bgEnabled = true;
	bgImage = NULL;
	surfaceTheUSA = NULL;
	bgImageLocation = DEFAULT_IMAGE_BG;
	bgImageType = TYPE_NONE;
	setBGColor(RUColors::DEFAULT_COMPONENT_BACKGROUND);
}

RUBackgroundComponent::RUBackgroundComponent(SDL_Color newBGColor)
{
	bgEnabled = true;
	bgImage = NULL;
	surfaceTheUSA = NULL;
	bgImageLocation = DEFAULT_IMAGE_BG;
	bgImageType = TYPE_NONE;
	setBGColor(newBGColor);
}

RUBackgroundComponent::~RUBackgroundComponent()
{
	bgEnabled = false;

	if (bgImage)
		delete bgImage;
	bgImage = NULL;

	if (surfaceTheUSA)
		SDL_FreeSurface(surfaceTheUSA);
	surfaceTheUSA = NULL;

	bgImageLocation = DEFAULT_IMAGE_BG;
	bgImageType = TYPE_NONE;
}

bool RUBackgroundComponent::resetSurface()
{
	// return Graphics::clearSurface(getWidth(), getHeight(), &surfaceTheUSA, bgColor);
	/*SDL_Surface* newSurfaceImage;
	/bool retVal = Graphics::clearSurface(getWidth(), getHeight(), &newSurfaceImage, bgColor);
	surfaceTheUSA = newSurfaceImage;
	return retVal;*/

	if ((getWidth() == 0) || (getHeight() == 0))
		return false;

	if (!surfaceTheUSA)
		surfaceTheUSA = SDL_CreateRGBSurface(0, getWidth(), getHeight(),
											 32, // 32 bits (depth)
											 rmask, gmask, bmask, amask);
	else
		SDL_FillRect(surfaceTheUSA, NULL,
					 SDL_MapRGB(surfaceTheUSA->format, bgColor.r, bgColor.g, bgColor.b));

	if (!surfaceTheUSA)
	{
		printf("[GUI] Surface SDL_CreateRGBSurface fail: %s\n", SDL_GetError());
		return false;
	}

	// Success
	// SDL_BLENDMODE_ADD instead?
	SDL_SetSurfaceBlendMode(surfaceTheUSA, SDL_BLENDMODE_NONE);
	return true;
}

void RUBackgroundComponent::refreshImage()
{
	resetSurface();
	if (bgImageType == TYPE_FILE)
		setBGImageFromLocation(bgImageLocation);
	else if (bgImageType == TYPE_GIMAGE)
		setBGImage(bgImage);
}

void RUBackgroundComponent::fromSurface(SDL_Surface* newSurfaceImage)
{
	if (!newSurfaceImage)
		return;

	for (int y = 0; y < newSurfaceImage->h; ++y)
	{
		for (int x = 0; x < newSurfaceImage->w; ++x)
		{
			// Current surface pixel
			// printf("(x,y): %d:%d\n", x, y);
			const unsigned char* surfacePixelData = (const unsigned char*)newSurfaceImage->pixels;
			unsigned int cPixel =
				*((unsigned int*)(surfacePixelData + (y * newSurfaceImage->pitch) +
								  (x * sizeof(unsigned int))));

			// Copy it to the image
			shmea::RGBA c;
			c.r = cPixel & rmask;
			c.g = cPixel & gmask;
			c.b = cPixel & bmask;
			c.a = cPixel & amask;
			bgImage->SetPixel(x, y, c);
		}
	}
}

void RUBackgroundComponent::fromImage(shmea::Image* newBGImage)
{
	bgImage = newBGImage;
	bgImageType = TYPE_GIMAGE;
	for (int y = 0; y < bgImage->getHeight(); ++y)
	{
		for (int x = 0; x < bgImage->getWidth(); ++x)
		{
			// Current image pixel
			unsigned int cPixel = 0x00000000;
			shmea::RGBA c = bgImage->GetPixel(x, y);
			((unsigned char*)&cPixel)[RED_INDEX] = c.r;
			((unsigned char*)&cPixel)[GREEN_INDEX] = c.g;
			((unsigned char*)&cPixel)[BLUE_INDEX] = c.b;
			((unsigned char*)&cPixel)[ALPHA_INDEX] = c.a;

			// Copy it to the surface
			// printf("(x,y): %d:%d\n", x, y);
			unsigned int* surfacePixelData = (unsigned int*)surfaceTheUSA->pixels;
			*((unsigned int*)(surfacePixelData + (y * surfaceTheUSA->pitch) +
							  (x * sizeof(unsigned int)))) = cPixel;
		}
	}
}

bool RUBackgroundComponent::getBGEnabled() const
{
	return bgEnabled;
}

SDL_Color RUBackgroundComponent::getBGColor() const
{
	return bgColor;
}

void RUBackgroundComponent::setBGImageFromLocation(const std::string& newBGImageLocation)
{
	if (bgImageLocation.length() == 0)
		return;

	// Set the new image type
	bgImageType = TYPE_FILE;
	bgImageLocation = newBGImageLocation;

	// Load the image
	SDL_Surface* newSurfaceImage = IMG_Load(newBGImageLocation.c_str());
	if (!newSurfaceImage)
	{
		printf("[GFX] IMG_Load error: %s\n", IMG_GetError());
		bgImageType = TYPE_NONE;
		return;
	}

	setBGImageFromSurface(newSurfaceImage);
}

void RUBackgroundComponent::setBGImageFromSurface(SDL_Surface* newSurfaceImage)
{
	if (!resetSurface())
		return;

	// For converting between RGB/RGBA etc
	SDL_Surface* optimizedSurface = SDL_ConvertSurface(newSurfaceImage, surfaceTheUSA->format, 0);
	SDL_FreeSurface(newSurfaceImage);

	// Copy the Surface
	if (SDL_BlitScaled(optimizedSurface, NULL, surfaceTheUSA, NULL) < 0)
	{
		printf("[GUI] Surface Blit error: %s\n", SDL_GetError());
		return;
	}

	SDL_FreeSurface(optimizedSurface);
}

void RUBackgroundComponent::toggleBG(bool newBGEnabled)
{
	bgEnabled = newBGEnabled;
	drawUpdate = true;
}

void RUBackgroundComponent::setBGImage(shmea::Image* newBGImage)
{
	if (!newBGImage)
		return;

	printf("DERP0\n");
	fromImage(newBGImage);
}

void RUBackgroundComponent::setBGColor(SDL_Color newBGColor)
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
	SDL_Rect bgRect;
	bgRect.x = 0;
	bgRect.y = 0;
	bgRect.w = getWidth();
	bgRect.h = getHeight();

	SDL_SetRenderDrawColor(cGfx->getRenderer(), bgColor.r, bgColor.g, bgColor.b, bgColor.a);
	SDL_RenderFillRect(cGfx->getRenderer(), &bgRect);

	// draw the background image
	if (!bgImageType)
		return;

	if (!surfaceTheUSA)
		return;

	if ((surfaceTheUSA->w == 0) || (surfaceTheUSA->h == 0))
		return;

	// Replace newBMP with surfaceTheUSA
	SDL_Texture* bgImageTex = SDL_CreateTextureFromSurface(cGfx->getRenderer(), surfaceTheUSA);
	if (!bgImageTex)
	{
		printf("[GUI] bgImageTex error: %s\n", SDL_GetError());
		return;
	}

	SDL_RenderCopy(cGfx->getRenderer(), bgImageTex, NULL, NULL);
}

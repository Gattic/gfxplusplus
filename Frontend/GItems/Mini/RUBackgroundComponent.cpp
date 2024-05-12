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
	bgColorEnabled = true;
	surfaceTheUSA = NULL;
	bgImageLocation = DEFAULT_IMAGE_BG;
	bgImageType = TYPE_NONE;
	setBGColor(RUColors::COLOR_TRANSPARENT);
	//bgCache = shmea::GPointer<shmea::Image>(new shmea::Image());
}

RUBackgroundComponent::RUBackgroundComponent(SDL_Color newBGColor)
{
	bgEnabled = true;
	bgColorEnabled = true;
	surfaceTheUSA = NULL;
	bgImageLocation = DEFAULT_IMAGE_BG;
	bgImageType = TYPE_NONE;
	setBGColor(newBGColor);
	//bgCache = shmea::GPointer<shmea::Image>(new shmea::Image());
}

RUBackgroundComponent::~RUBackgroundComponent()
{
	bgEnabled = false;
	bgColorEnabled = false;

	if (surfaceTheUSA)
		SDL_FreeSurface(surfaceTheUSA);
	surfaceTheUSA = NULL;

	bgImageLocation = DEFAULT_IMAGE_BG;
	bgImageType = TYPE_NONE;
	//bgCache = shmea::GPointer<shmea::Image>(new shmea::Image());
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
	{
		surfaceTheUSA = SDL_CreateRGBSurface(0, getWidth(), getHeight(), 32, rmask, gmask, bmask, amask);// 32 bits (depth)
		//bgCache->Allocate(getWidth(), getHeight());
	}
	else
	{
		// Replace with the bgCache instead of this color
		/*for (int cx = 0; cx < getWidth(); ++cx)
		{
			for (int cy = 0; cy < getHeight(); ++cy)
			{
				shmea::RGBA c = bgCache->GetPixel(cx, cy);
				uint32_t cPixel = SDL_MapRGBA(surfaceTheUSA->format, c.r, c.g, c.b, c.a);

				// Copy it to the surface
				// printf("(cx,cy): %d:%d\n", cx, cy);
				uint8_t* surfacePixelData = (uint8_t*)surfaceTheUSA->pixels;
				*((unsigned int*)(surfacePixelData + (cy * surfaceTheUSA->pitch) + (cx * surfaceTheUSA->format->BytesPerPixel))) = cPixel;
			}
		}*/
	}

	if (!surfaceTheUSA)
	{
		printf("[GUI] Surface SDL_CreateRGBSurface fail: %s\n", SDL_GetError());
		return false;
	}

	// Success
	SDL_FillRect(surfaceTheUSA, NULL, SDL_MapRGB(surfaceTheUSA->format, bgColor.r, bgColor.g, bgColor.b));
	SDL_SetSurfaceBlendMode(surfaceTheUSA, SDL_BLENDMODE_BLEND);


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
			//TODO: Fix pointer logic via fromImage!!!!!!
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

void RUBackgroundComponent::fromImage(shmea::GPointer<shmea::Image> newBGImage)
{
	bgImage = newBGImage;
	bgImageType = TYPE_GIMAGE;

	SDL_Surface* newImageSurface = SDL_CreateRGBSurface(0, bgImage->getWidth(), bgImage->getHeight(), 32, rmask, gmask, bmask, amask);
	if (!newImageSurface)
	{
		printf("[GUI] Surface SDL_CreateRGBSurface fail: %s\n", SDL_GetError());
		return;
	}

	// Success
	// SDL_BLENDMODE_ADD instead?
	SDL_SetSurfaceBlendMode(newImageSurface, SDL_BLENDMODE_BLEND);

	//printf("fromImage(%d, %d)\n", getWidth(), getHeight());
	//printf("fromGImage(%d, %d)\n", bgImage->getWidth(), bgImage->getHeight());
	//printf("img-info(%d, %d)\n", newImageSurface->pitch, newImageSurface->format->BytesPerPixel);
	for (unsigned int cy = 0; cy < bgImage->getHeight(); ++cy)
	{
		for (unsigned int cx = 0; cx < bgImage->getWidth(); ++cx)
		{
			// Current image pixel
			/*unsigned int cPixel = 0x00000000;
			shmea::RGBA c = bgImage->GetPixel(cx, cy);
			((unsigned char*)&cPixel)[RED_INDEX] = c.r;
			((unsigned char*)&cPixel)[GREEN_INDEX] = c.g;
			((unsigned char*)&cPixel)[BLUE_INDEX] = c.b;
			((unsigned char*)&cPixel)[ALPHA_INDEX] = c.a;*/
			//printf("cx,cy: (%d,%d): 0x%08X\n", cx, cy, cPixel);
			shmea::RGBA c = bgImage->GetPixel(cx, cy);
			uint32_t cPixel = SDL_MapRGBA(newImageSurface->format, c.r, c.g, c.b, c.a);

			// Copy it to the surface
			// printf("(cx,cy): %d:%d\n", cx, cy);
			uint8_t* surfacePixelData = (uint8_t*)newImageSurface->pixels;
			*((unsigned int*)(surfacePixelData + (cy * newImageSurface->pitch) + (cx * newImageSurface->format->BytesPerPixel))) = cPixel;
		}
	}

	setBGImageFromSurface(newImageSurface);
}

bool RUBackgroundComponent::getBGEnabled() const
{
	return bgEnabled;
}

bool RUBackgroundComponent::getBGColorEnabled() const
{
	return bgColorEnabled;
}

SDL_Color RUBackgroundComponent::getBGColor() const
{
	return bgColor;
}

void RUBackgroundComponent::setBGImageFromLocation(const shmea::GString& newBGImageLocation)
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

	if (bgColorEnabled)
	{
		SDL_SetRenderDrawColor(cGfx->getRenderer(), bgColor.r, bgColor.g, bgColor.b, bgColor.a);
		//SDL_RenderFillRect(cGfx->getRenderer(), &bgRect);
		drawVerticalGradient(cGfx->getRenderer(), bgRect, getBGColor(), getBGColor(), 6);
	}

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

	// Copy the old background into a cache
	// WAIT SHOULD THE BG CACHE HOLD THE PIXELS BEFORE THE IMAGE OR OF THE IMAGE?
	for (int cx = 0; cx < bgRect.w; ++cx)
	{
		for (int cy = 0; cy < bgRect.h; ++cy)
		{
			shmea::RGBA c;
			SDL_GetRGBA(*((Uint32*)surfaceTheUSA->pixels + cx + cy * bgRect.w), surfaceTheUSA->format, &c.r, &c.g, &c.b, &c.a);
			//bgCache->SetPixel(cx, cy, c);
		}
	}

	// Draw the texture
	SDL_RenderCopy(cGfx->getRenderer(), bgImageTex, &bgRect, NULL);

	/*if(bgImage)
	{
		for (int cx = 0; cx < bgImage->getWidth(); ++cx)
		{
			for (int cy = 0; cy < bgImage->getHeight(); ++cy)
			{
				shmea::RGBA c = bgImage->GetPixel(cx, cy);
				SDL_SetRenderDrawColor(cGfx->getRenderer(), c.r, c.g, c.b, c.a);
				SDL_RenderDrawPoint(cGfx->getRenderer(), cx, cy);
				
			}
		}
	}*/
}

void RUBackgroundComponent::drawVerticalGradient(SDL_Renderer* renderer, SDL_Rect rect, SDL_Color color1, SDL_Color color2, int cornerRadius)
{
    int startY = rect.y;
    int endY = startY + rect.h;

    // Calculate color deltas
    float deltaR = static_cast<float>(color2.r - color1.r) / rect.h;
    float deltaG = static_cast<float>(color2.g - color1.g) / rect.h;
    float deltaB = static_cast<float>(color2.b - color1.b) / rect.h;
    float deltaA = static_cast<float>(color2.a - color1.a) / rect.h;

    // Precompute frequently used values
    int rectX = rect.x;
    int rectY = rect.y;
    int rectW = rect.w;
    int rectH = rect.h;
    int rectXW = rectX + rectW - 1;
    int rectYH = rectY + rectH - 1;

    // Draw the vertical gradient with adjustable round corners
    for (int y = startY; y < endY; ++y)
    {
        // Calculate current color
        Uint8 r = static_cast<Uint8>(color1.r + static_cast<Uint8>(deltaR * (y - startY)));
        Uint8 g = static_cast<Uint8>(color1.g + static_cast<Uint8>(deltaG * (y - startY)));
        Uint8 b = static_cast<Uint8>(color1.b + static_cast<Uint8>(deltaB * (y - startY)));
        Uint8 a = static_cast<Uint8>(color1.a + static_cast<Uint8>(deltaA * (y - startY)));

        std::vector<SDL_Point> pixelsToDraw;

        for (int x = rectX; x < rectXW + 1; ++x)
        {
            // Check if the pixel is within the rounded corner area
            bool drawPixel = true;

            int dx = 0, dy = 0;

            if (x < rectX + cornerRadius)
            {
                // Calculate the left side of the rect
                dx = x - (rectX + cornerRadius);
                if (y < rectY + cornerRadius) // Top left corner
                    dy = y - (rectY + cornerRadius);
                else if (y >= rectYH - cornerRadius) // Bottom left corner
                    dy = y - (rectYH - cornerRadius);
                else
                {
                    if (y < rectY || y >= rectYH)
                        drawPixel = false;
                    else if (x < rectX)
                    {
                        drawPixel = false;
                        dx = x - rectX;
                    }
                }
            }
            else if (x >= rectXW - cornerRadius)
            {
                // Calculate the right side of the rect
                dx = x - (rectXW - cornerRadius);
                if (y < rectY + cornerRadius) // Top right corner
                    dy = y - (rectY + cornerRadius);
                else if (y >= rectYH - cornerRadius) // Bottom right corner
                    dy = y - (rectYH - cornerRadius);
                else
                {
                    if (y < rectY || y >= rectYH)
                        drawPixel = false;
                    else if (x > rectXW)
                    {
                        drawPixel = false;
                        dx = x - rectXW;
                    }
                }
            }
            else if (y < rectY + cornerRadius || y >= rectYH - cornerRadius)
            {
                if (x < rectX || x >= rectXW)
                    drawPixel = false;
            }

            if (drawPixel)
            {
                int radiusSquared = cornerRadius * cornerRadius;
                if ((dx * dx + dy * dy) <= radiusSquared)
                {
                    SDL_Point point;
                    point.x = x;
                    point.y = y;
                    pixelsToDraw.push_back(point);
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        if (!pixelsToDraw.empty())
        {
            SDL_RenderDrawPoints(renderer, &pixelsToDraw[0], pixelsToDraw.size());
        }
    }
}

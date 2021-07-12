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
#ifndef _GIMAGE_H_
#define _GIMAGE_H_

#include <stdio.h>
#include <string.h>
#include <string>

class RUBackgroundComponent;

namespace shmea {

class GString;

// 32 bit color pixel density
class RGBA
{
public:
	RGBA(unsigned char red = 0xFF, unsigned char green = 0xFF, unsigned char blue = 0xFF,
		 unsigned char alpha = 0xFF)
		: r(red), g(green), b(blue), a(alpha)
	{
		//
	}

	bool isWhite() const
	{
		return r == 0xFF && g == 0xFF && b == 0xFF;
	}

	unsigned char r, g, b, a;
};

//	bmp, png	(when T == RGBA)
//	ppm			(when T == RGB)
//	pbm			(when T == bool)

class Image
{
	friend RUBackgroundComponent;

private:
	int width;
	int height;
	int pitch;
	RGBA* data;

	void copy_helper(const Image& image)
	{
		pitch = image.pitch;
		Allocate(image.getWidth(), image.getHeight());
		for (int i = 0; i < image.getWidth(); i++)
		{
			for (int j = 0; j < image.getHeight(); j++)
				this->SetPixel(i, j, image.GetPixel(i, j));
		}
	}

	bool LoadPPM(const GString&);
	bool LoadPBM(const GString&);
	bool SavePPM(const GString&) const;
	bool SavePBM(const GString&) const;
	void LoadBMP(const GString&);

public:
	Image() : width(0), height(0), pitch(0), data(NULL)
	{
		//
	}

	Image(const Image& image) : data(NULL)
	{
		copy_helper(image);
	}

	const Image& operator=(const Image& image)
	{
		if (this != &image)
			copy_helper(image);
		return *this;
	}

	~Image()
	{
		delete[] data;
	}

	// initialize an image of a specific size
	void Allocate(int w, int h)
	{
		width = w;
		height = h;
		delete[] data;
		if (width == 0 && height == 0)
		{
			data = NULL;
		}
		else
		{
			if (!(width > 0 && height > 0))
				return;
			data = new RGBA[width * height];
		}
	}

	// gets
	int getWidth() const
	{
		return width;
	}

	int getHeight() const
	{
		return height;
	}

	RGBA GetPixel(int x, int y) const
	{
		if (!(x >= 0 && x < width))
			return RGBA();

		if (!(y >= 0 && y < height))
			return RGBA();

		return data[y * width + x];
	}

	// sets
	void SetAllPixels(const RGBA& value)
	{
		for (int i = 0; i < width * height; i++)
			data[i] = value;
	}

	void SetPixel(int x, int y, const RGBA& value)
	{
		if (!(x >= 0 && x < width))
			return;

		if (!(y >= 0 && y < height))
			return;

		data[y * width + x] = value;
	}
};
};

#endif

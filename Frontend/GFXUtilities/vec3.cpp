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
#include "vec3.h"

Vec3::Vec3()
{
	setX(0.0f);
	setY(0.0f);
	setZ(0.0f);
}

Vec3::Vec3(double newX, double newY, double newZ)
{
	setX(newX);
	setY(newY);
	setZ(newZ);
}

Vec3::Vec3(const Vec3& v2)
{
	setX(v2.x);
	setY(v2.y);
	setZ(v2.z);
}

Vec3::~Vec3()
{
	setX(0.0f);
	setY(0.0f);
	setZ(0.0f);
}

double Vec3::getX() const
{
	return x;
}

double Vec3::getY() const
{
	return y;
}

double Vec3::getZ() const
{
	return z;
}

double Vec3::length() const
{
	return sqrt(x * x + y * y + z * z);
}

void Vec3::set(double newX, double newY, double newZ)
{
	setX(newX);
	setY(newY);
	setZ(newZ);
}

void Vec3::setX(double newX)
{
	x = newX;
}

void Vec3::setY(double newY)
{
	y = newY;
}

void Vec3::setZ(double newZ)
{
	z = newZ;
}

void Vec3::normalize()
{
	double len = length();
	if (len == 0.0f)
		return;

	x /= len;
	y /= len;
	z /= len;
}

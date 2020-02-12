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
#include "quaternion.h"
#include "vec3.h"

Quaternion::Quaternion()
{
	setW(1.0f);
	setX(0.0f);
	setY(0.0f);
	setZ(0.0f);
}

Quaternion::Quaternion(double newW, double newX, double newY, double newZ)
{
	setW(newW);
	setX(newX);
	setY(newY);
	setZ(newZ);
}

Quaternion::Quaternion(double angle, Vec3 axis)
{
	axis.normalize();
	double sin_a_2 = sin(angle * M_PI / 360.0f);
	setW(cos(angle * M_PI / 360.0f));
	setX(axis.getX() * sin_a_2);
	setY(axis.getY() * sin_a_2);
	setZ(axis.getZ() * sin_a_2);
}

Quaternion::Quaternion(const Quaternion& q2)
{
	setW(q2.w);
	setX(q2.x);
	setY(q2.y);
	setZ(q2.z);
}

Quaternion::~Quaternion()
{
	setW(1.0f);
	setX(0.0f);
	setY(0.0f);
	setZ(0.0f);
}

double Quaternion::getW() const
{
	return w;
}

double Quaternion::getX() const
{
	return x;
}

double Quaternion::getY() const
{
	return y;
}

double Quaternion::getZ() const
{
	return z;
}

double Quaternion::length() const
{
	return sqrt(w * w + x * x + y * y + z * z);
}

Quaternion Quaternion::getConjugate() const
{
	Quaternion q(w, -x, -y, -z);
	return q;
}

void Quaternion::set(double newW, double newX, double newY, double newZ)
{
	setW(newW);
	setX(newX);
	setY(newY);
	setZ(newZ);
}

void Quaternion::setW(double newW)
{
	w = newW;
}

void Quaternion::setX(double newX)
{
	x = newX;
}

void Quaternion::setY(double newY)
{
	y = newY;
}

void Quaternion::setZ(double newZ)
{
	z = newZ;
}

void Quaternion::normalize()
{
	double len = length();
	if (len == 0.0f)
		return;

	w /= len;
	x /= len;
	y /= len;
	z /= len;
}

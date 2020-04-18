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
#include "graphics.h"
#include "object.h"

void gfxpp::addCube()
{
	Object* newObject = (Object*)malloc(sizeof(Object));
	*newObject = Object();
	newObject->setType(Object::CUBE_TYPE);

	if (!contains(newObject))
		objects.push_back(newObject);
}

void gfxpp::addBasis()
{
	Object* newObject = (Object*)malloc(sizeof(Object));
	*newObject = Object();
	newObject->setType(Object::BASIS_TYPE);

	if (!contains(newObject))
		objects.push_back(newObject);
}

bool gfxpp::contains(const Object* newObject) const
{
	// check if the other object is already in the vector
	std::vector<Object*>::const_iterator itr = objects.begin();
	for (; itr != objects.end(); ++itr)
	{
		if ((*(*itr)) == (*newObject))
			return true;
	}

	return false;
}

bool gfxpp::contains(const Object newObject) const
{
	// check if the other object is already in the vector
	std::vector<Object*>::const_iterator itr = objects.begin();
	for (; itr != objects.end(); ++itr)
	{
		if ((*(*itr)) == newObject)
			return true;
	}

	return false;
}

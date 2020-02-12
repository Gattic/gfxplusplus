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
#include "object.h"
#include "../GFXUtilities/quaternion.h"
#include "../GFXUtilities/vec3.h"
#include "graphics.h"

Object::Object()
{
	type = EMPTY_TYPE;
	center = Vec3(0.0f, 0.0f, 0.0f);
	dimensions = Vec3(1.0f, 1.0f, 1.0f);
	rotation = Quaternion(1.0f, 0.0f, 0.0f, 0.0f);
}

Object::Object(const Vec3& newCenter, const Vec3& newDimensions, const Quaternion& newRotation)
{
	type = EMPTY_TYPE;
	setCenter(newCenter);
	setDimensions(newDimensions);
	setRotation(newRotation);
}

void Object::Render()
{
	glPushMatrix();
	glTranslatef(center.getX(), center.getY(), center.getZ());
	glScalef(dimensions.getX(), dimensions.getY(), dimensions.getZ());
	rotation.normalize();

	// Quaternion to matrix
	const double x_x = rotation.getX() * rotation.getX(), y_y = rotation.getY() * rotation.getY(),
				 z_z = rotation.getZ() * rotation.getZ();
	const double w_x = rotation.getW() * rotation.getX(), w_y = rotation.getW() * rotation.getY(),
				 w_z = rotation.getW() * rotation.getZ();
	const double x_y = rotation.getX() * rotation.getY(), x_z = rotation.getX() * rotation.getZ(),
				 y_z = rotation.getY() * rotation.getZ();

	GLfloat matrix[16];

	// Column 1					//Column 2					//Column 3					//Column 4
	matrix[0] = 1 - 2 * (y_y + z_z);
	matrix[4] = 2 * (x_y - w_z);
	matrix[8] = 2 * (x_z + w_y);
	matrix[12] = 0;
	matrix[1] = 2 * (x_y + w_z);
	matrix[5] = 1 - 2 * (x_x + z_z);
	matrix[9] = 2 * (y_z - w_x);
	matrix[13] = 0;
	matrix[2] = 2 * (x_z - w_y);
	matrix[6] = 2 * (y_z + w_x);
	matrix[10] = 1 - 2 * (x_x + y_y);
	matrix[14] = 0;
	matrix[3] = 0;
	matrix[7] = 0;
	matrix[11] = 0;
	matrix[15] = 1;

	glMultMatrixf(matrix);

	if (type == BASIS_TYPE)
	{
		glBegin(GL_QUADS);

		// X
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(0, 0.05, 0.05);
		glVertex3f(1, 0.05, 0.05);
		glVertex3f(1, -0.05, -0.05);
		glVertex3f(0, -0.05, -0.05);

		// Y
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(0.05, 0, 0.05);
		glVertex3f(0.05, 1, 0.05);
		glVertex3f(-0.05, 1, -0.05);
		glVertex3f(-0.05, 0, -0.05);

		// Z
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(0.05, 0.05, 0);
		glVertex3f(0.05, 0.05, 1);
		glVertex3f(-0.05, -0.05, 1);
		glVertex3f(-0.05, -0.05, 0);
	}
	/*else if(type == NODE_TYPE)
	{
		//Draw the node?
		if(node == NULL)
			return;

		//Get the SGD
		std::vector<std::vector<double> > *vertices;//=&(node->vertices);

		//Render the node
		glBegin(GL_LINES);

		for(int i=0;i<vertices->size();++i)
		{
			for(int j=0;j<(*vertices)[i].size();++j)
			{
				//normalize the i and j
				double vertex[]={
					((double)i)/((double)vertices->size()),
					((double)j)/((double)(*vertices)[i].size()),
					(*vertices)[i][j]
				};

				//determine the color
				int counter=0;
				if((i == 0) || (i == vertices->size()))
					++counter;
				if((j == 0) || (j == (*vertices)[i].size()))
					++counter;
				if(((*vertices)[i][j] == 0) || ((*vertices)[i][j] == 1.0f))
					++counter;

				//get the color
				int8_t redMask=0;
				int8_t greenMask=0;
				int8_t blueMask=0;
				unsigned int colorMask=Graphics::RGBfromHue(vertex[2], &redMask, &greenMask,
	&blueMask);
				glColor3d(redMask, greenMask, blueMask);

				//draw the vertex
				glVertex3dv(vertex);
			}
		}
	}*/
	else if (type == CUBE_TYPE)
	{
		glBegin(GL_QUADS);

		// Front
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(-1, 1, 1);
		glVertex3f(1, 1, 1);
		glVertex3f(1, -1, 1);
		glVertex3f(-1, -1, 1);

		// Back
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-1, 1, -1);
		glVertex3f(1, 1, -1);
		glVertex3f(1, -1, -1);
		glVertex3f(-1, -1, -1);

		// Left
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-1, 1, 1);
		glVertex3f(-1, -1, 1);
		glVertex3f(-1, -1, -1);
		glVertex3f(-1, 1, -1);

		// Right
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(1, 1, 1);
		glVertex3f(1, -1, 1);
		glVertex3f(1, -1, -1);
		glVertex3f(1, 1, -1);

		// Bottom
		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(-1, -1, 1);
		glVertex3f(1, -1, 1);
		glVertex3f(1, -1, -1);
		glVertex3f(-1, -1, -1);

		// Top
		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex3f(-1, 1, 1);
		glVertex3f(1, 1, 1);
		glVertex3f(1, 1, -1);
		glVertex3f(-1, 1, -1);
	}

	glEnd();
	glPopMatrix();
}

Vec3 Object::getCenter() const
{
	return center;
}

Vec3 Object::getDimensions() const
{
	return dimensions;
}

Quaternion Object::getRotation() const
{
	return rotation;
}

int Object::getType() const
{
	return type;
}

void Object::setCenter(const Vec3& newCenter)
{
	center = newCenter;
}

void Object::setDimensions(const Vec3& newDimensions)
{
	dimensions = newDimensions;
}

void Object::setRotation(const Quaternion& newRotation)
{
	rotation = newRotation;
	rotation.normalize();
}

void Object::setType(int newType)
{
	type = newType;
}

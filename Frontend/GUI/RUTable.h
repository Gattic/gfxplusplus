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
#ifndef _RUTABLE
#define _RUTABLE

#include "../../include/Backend/Database/gtable.h"
#include "../GItems/RUComponent.h"
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

class RULabel;
class RUScrollbar;

namespace shmea {
class GTable;
};

class RUTable : public RUComponent, public shmea::GTable
{
	friend class RUDropdown;

protected:
	std::vector<std::vector<RULabel*> > textLabels; // size == visible(r, c)
	RUScrollbar* scrollbar;
	unsigned int rowsShown;

	// events
	virtual void onMouseWheel(GPanel*, int, int, int);

public:
	static const int DEFAULT_SCROLLBAR_WIDTH = 20;

	// constructors & destructor
	RUTable();
	~RUTable();

	// gets
	unsigned int getRowsShown() const;

	// sets
	void import(const shmea::GTable&);
	void setRowsShown(unsigned int);
	void setWidth(int);
	void setHeight(int);
	void clear();

	// render
	void updateLabels();
	void refreshLabels();
	virtual void updateBackground(SDL_Renderer*);
	virtual std::string getType() const;
};

#endif

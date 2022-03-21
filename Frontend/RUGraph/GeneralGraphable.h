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

#ifndef _RUGENERALGRAPHABLE_H
#define _RUGENERALGRAPHABLE_H

#include "Graphable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>

// Replicate a vtable with virtual functions.
// Could also do this with function pointers
class GeneralGraphable
{
	class GraphableAbstract
	{
	public:
		virtual ~GraphableAbstract()
		{
			//
		}

		template< typename T>
		void add(const T* newPoint, bool recompute);
		template< typename T>
		void set(const std::vector<T*>& newPoints);
		virtual void computeAxisRanges(bool additionOptimization = false);
		virtual void updateBackground(gfxpp* cGfx);
	};

	template< typename T>
	class GraphableConcept
	{
	public:
		virtual ~GraphableConcept()
		{
			//
		}

		virtual void add(const T* newPoint, bool recompute) = 0;
		virtual void set(const std::vector<T*>& newPoints) = 0;
		virtual void computeAxisRanges(bool additionOptimization = false) = 0;
		virtual void updateBackground(gfxpp* cGfx) = 0;
	};

	template< typename T>
	class GraphableModel : public GraphableConcept<T>
	{
	protected:

		Graphable<T*> g;

	public:

		GraphableModel()
		{
			//
		}

		GraphableModel(const std::vector<T*>& newPoints)
		{
			g->set(newPoints);
		}
		
		void set(const std::vector<T*>& newPoints)
		{
			g->set(newPoints);
		}
		
		// This function is recommended for TimeSeries optimizations.
		void add(const T* newPoint, bool recompute)
		{
			g->add(newPoint, recompute);
		}

		void computeAxisRanges(bool additionOptimization)
		{
			g->computeAxisRanges(additionOptimization);
		}

		void updateBackground(gfxpp* cGfx)
		{
			g->updateBackground(cGfx);
		}

		virtual ~GraphableModel()
		{
			//
		}
	};

	GraphableAbstract* object;

public:

	GeneralGraphable()
	{
		object = NULL;
	}

	template< typename T>
	GeneralGraphable(T* cGraphable)
	{
		object = new GraphableModel<T>(cGraphable);
	}

	template< typename T>
	void add(const T* newPoint, bool recompute)
	{
		if(object)
			object->add(newPoint, recompute);
	}

	template< typename T>
	void set(const std::vector<T*>& newPoints)
	{
		if(object)
			object->set(newPoints);
	}

	void computeAxisRanges(bool additionOptimization = false)
	{
		if(object)
			object->computeAxisRanges(additionOptimization);
	}

	void updateBackground(gfxpp* cGfx)
	{
		if(object)
			object->updateBackground(cGfx);
	}

	virtual ~GeneralGraphable()
	{
		//
	}
};

#endif

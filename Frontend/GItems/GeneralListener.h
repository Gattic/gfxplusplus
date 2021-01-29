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

#ifndef _RUGENERALLISTENER
#define _RUGENERALLISTENER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>

//TODO:
// MAKE GITEMS = EVENTS (use operators)

// Replicate a vtable with virtual functions.
// Could also do this with function pointers
//class GeneralListener
class GeneralListener
{
	class ListenerConcept
	{
	public:
		virtual ~ListenerConcept()
		{
			//
		}

		virtual void call()=0;
		virtual void call(int scrollType)=0;
		virtual void call(int x, int y)=0;
		virtual void call(const std::string& str, int x, int y)=0;
		virtual void call(const std::string& str)=0;
	};

	template< typename T>
	class ListenerModel : public ListenerConcept
	{
	protected:

		T cbPanel;
		void (T::*listener)();
		//void (T::*listener)();
		//void (GPanel::*MouseDownListener)(const std::string&, int, int);
 		//(void (*)())&SimulationPanel::GuiCommander1)

	public:
		ListenerModel(const T& t, void (T::*newListener)()) : cbPanel(t), listener((void (T::*)())newListener)
		{
			//
		}

		ListenerModel(const T& t, void (T::*newListener)(int)) : cbPanel(t), listener((void (T::*)())newListener)
		{
			//
		}

		ListenerModel(const T& t, void (T::*newListener)(int, int)) : cbPanel(t), listener((void (T::*)())newListener)
		{
			//
		}

		ListenerModel(const T& t, void (T::*newListener)(const std::string&, int, int)) : cbPanel(t), listener((void (T::*)())newListener)
		{
			//
		}

		ListenerModel(const T& t, void (T::*newListener)(const std::string&)) : cbPanel(t), listener((void (T::*)())newListener)
		{
			//
		}

		void call()
		{
			void (T::*typedListener)() = listener;
			(&cbPanel->*typedListener)();
		}

		void call(int scrollType)
		{
			void (T::*typedListener)(int) = (void (T::*)(int))listener;
			(cbPanel.*typedListener)(scrollType);
		}

		void call(int x, int y)
		{
			void (T::*typedListener)(int, int) = (void (T::*)(int, int))listener;
			(cbPanel.*typedListener)(x, y);
		}

		void call(const std::string& str, int x, int y)
		{
			void (T::*typedListener)(const std::string&, int, int) = (void (T::*)(const std::string&, int, int))listener;
			(cbPanel.*typedListener)(str, x, y);
		}

		void call(const std::string& str)
		{
			void (T::*typedListener)(const std::string&) = (void (T::*)(const std::string&))listener;
			(cbPanel.*typedListener)(str);
		}

		virtual ~ListenerModel()
		{
			//
		}
	};

	ListenerConcept* object;

public:

	GeneralListener()
	{
		object = NULL;
	}

	template< typename T>
	GeneralListener(const T& callbackPanel, void (T::*newListener)())
	{
		object = new ListenerModel<T>(callbackPanel, newListener);
	}

	template< typename T>
	GeneralListener(const T& callbackPanel, void (T::*newListener)(int))
	{
		object = new ListenerModel<T>(callbackPanel, newListener);
	}

	template< typename T>
	GeneralListener(const T& callbackPanel, void (T::*newListener)(int, int))
	{
		object = new ListenerModel<T>(callbackPanel, newListener);
	}

	template< typename T>
	GeneralListener(const T& callbackPanel, void (T::*newListener)(const std::string&, int, int))
	{
		object = new ListenerModel<T>(callbackPanel, newListener);
	}

	template< typename T>
	GeneralListener(const T& callbackPanel, void (T::*newListener)(const std::string&))
	{
		object = new ListenerModel<T>(callbackPanel, newListener);
	}

	template< typename T, typename U >
	void set(const T& callbackPanel, const U& newListener)
	{
		object = new ListenerModel<T>(callbackPanel, newListener);
	}

	void call()
	{
		if(object)
			object->call();
	}

	void call(int scrollType)
	{
		if(object)
			object->call(scrollType);
	}

	void call(int x, int y)
	{
		if(object)
			object->call(x, y);
	}

	void call(const std::string& str, int x, int y)
	{
		if(object)
			object->call(str, x, y);
	}

	void call(const std::string& str)
	{
		if(object)
			object->call(str);
	}

	virtual ~GeneralListener()
	{
		//
	}
};

void lExample();

#endif

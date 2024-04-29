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
#include "../GItems/GItem.h"
#include "../GItems/GLayout.h"
#include "../GItems/GPanel.h"
#include "../GItems/RUColors.h"
#include "../GItems/RUComponent.h"
#include "../GUI/Text/GFont.h"
//#include "../GUI/Text/RULabel.h"

gfxpp::gfxpp()
{
	systemCursor = NULL;
	width = 800;
	height = 600;

	errorFlag = initHelper(false, "gfxplusplus", true);
}

gfxpp::gfxpp(shmea::GString newTitle, int newRenderStatus, bool fullScreenMode, bool compatMode, int newWidth, int newHeight)
{
	systemCursor = NULL;
	width = newWidth;
	height = newHeight;

	errorFlag = initHelper(fullScreenMode, newTitle, compatMode);
}

int gfxpp::getErrorFlag() const
{
	return errorFlag;
}

SDL_Renderer* gfxpp::getRenderer()
{
	return renderer;
}

int gfxpp::initHelper(bool fullscreenMode, shmea::GString title, bool compatMode)
{
	running = false;
	hunterZolomon = 1.0f;

	frames = 0;
	rotate = false;
	move = false;
	now = 0;
	then = SDL_GetTicks();

	// for mouse
	mouseX = 0;
	mouseY = 0;

	// for key presses
	CTRLPressed = false;
	ALTPressed = false;
	spacePressed = false;
	fPressed = false;
	uPressed = false;
	qPressed = false;
	gPressed = false;
	rPressed = false;
	lPressed = false;
	upPressed = false;
	downPressed = false;
	leftPressed = false;
	rightPressed = false;

	window = NULL;
	renderer = NULL;

	focusedItem = NULL;
	focusedPanel = NULL;

	fps = 0;

	// Initialize SDL
	int sdlStatus = SDL_Init(SDL_INIT_VIDEO);
	if (sdlStatus < 0)
	{
		printf("[GFX] Initialization error: %s\n", SDL_GetError());
		finish();
		return -1;
	}

	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

	// Create a new window
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	if (fullscreenMode)
	{
		if(compatMode)
			window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
								  getWidth(), getHeight(), SDL_WINDOW_FULLSCREEN_DESKTOP);
		else
			window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
								  getWidth(), getHeight(), SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);

		// get the width and height
		SDL_DisplayMode DM;
		int errorNo = SDL_GetDesktopDisplayMode(0, &DM);
		if (errorNo < 0)
			return -5;
		width = DM.w;
		height = DM.h;
	}
	else
	{
		if(compatMode)
			window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							 getWidth(), getHeight(), SDL_WINDOW_RESIZABLE);
		else
			window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							 getWidth(), getHeight(), SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	}

	if (!window)
	{
		printf("[GFX] Window error: %s\n", SDL_GetError());
		finish();
		return -2;
	}

	int errorNo = init2D(compatMode);
	if (errorNo < 0)
	    return errorNo;

	// Set the SDL system cursor
	systemCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);

	// ALl good with initialization, return success
	return 0;
}

int gfxpp::init2D(bool compatMode)
{
	// Create a new renderer; -1 loads the default video driver we need
	if(compatMode)
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE);
	else
		renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
	if (!renderer)
	{
		printf("[GFX] Renderer error: %s\n", SDL_GetError());
		finish();
		return -3;
	}
	else
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	// Init ttf
	if (TTF_Init() == -1)
	{
		printf("[GFX] TTF Init error: %s\n", SDL_GetError());
		finish();
		return -4;
	}

	cFont = new GFont(renderer);
	graphicsFonts.insert(std::pair<int, GFont*>(0, cFont));

	GFont* fontGreen = new GFont(renderer);
	fontGreen->setTextColor(RUColors::TEXT_COLOR_GREEN);
	graphicsFonts.insert(std::pair<int, GFont*>(1, fontGreen));

	GFont* fontRed = new GFont(renderer);
	fontRed->setTextColor(RUColors::TEXT_COLOR_RED);
	graphicsFonts.insert(std::pair<int, GFont*>(2, fontRed));

	// Load support for the PNG, TIF, and JPG image formats
	int flags = IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_JPG;
	int imgInitStatus = IMG_Init(flags);
	if ((imgInitStatus & flags) != flags)
	{
		printf("[GFX] Image Init error: %s\n", IMG_GetError());
		return -5;
	}

	return 0;
}

void gfxpp::run()
{
	// the display loop
	display();
	finish();
}

void gfxpp::display()
{
	running = true;
	frames = 0;
	fps = 0;
	rotate = false;
	move = false;
	now = 0;
	then = SDL_GetTicks();

	// for mouse
	mouseX = 0;
	mouseY = 0;

	// for key presses
	CTRLPressed = false;
	ALTPressed = false;
	spacePressed = false;
	fPressed = false;
	uPressed = false;
	qPressed = false;
	gPressed = false;
	rPressed = false;
	lPressed = false;
	upPressed = false;
	downPressed = false;
	leftPressed = false;
	rightPressed = false;

	// draw/event loop
	while (running)
	{
		++frames;

		//=================EVENTS=================
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			// close the window
			if (event.type == SDL_QUIT)
				running = false;

			SDL_Keycode keyPressed = 0x00;
			//Uint16 keyModPressed = 0x00;
			if ((event.type == SDL_KEYUP) || (event.type == SDL_KEYDOWN))
			{
				// set the key event vars
				keyPressed = event.key.keysym.sym;
				//keyModPressed = event.key.keysym.mod;

				// if((keyModPressed & KMOD_CTRL) || (keyModPressed & KMOD_LCTRL) || (keyModPressed
				// & KMOD_RCTRL))
				if ((keyPressed == SDLK_LCTRL) || (keyPressed == SDLK_RCTRL))
				{
					if (event.type == SDL_KEYUP) // Key release
						CTRLPressed = false;
					else if (event.type == SDL_KEYDOWN) // Key press
						CTRLPressed = true;
				}
				else if ((keyPressed == SDLK_LALT) || (keyPressed == SDLK_RALT))
				{
					if (event.type == SDL_KEYUP) // Key release
						ALTPressed = false;
					else if (event.type == SDL_KEYDOWN) // Key press
						ALTPressed = true;
				}
				else if (keyPressed == SDLK_SPACE)
				{
					if (event.type == SDL_KEYUP) // Key release
						spacePressed = false;
					else if (event.type == SDL_KEYDOWN) // Key press
						spacePressed = true;
				}
				else if (keyPressed == SDLK_f)
				{
					if (event.type == SDL_KEYUP) // Key release
						fPressed = false;
					else if (event.type == SDL_KEYDOWN) // Key press
						fPressed = true;
				}
				else if (keyPressed == SDLK_u)
				{
					if (event.type == SDL_KEYUP) // Key release
						uPressed = false;
					else if (event.type == SDL_KEYDOWN) // Key press
						uPressed = true;
				}
				else if (keyPressed == SDLK_q)
				{
					if (event.type == SDL_KEYUP) // Key release
						qPressed = false;
					else if (event.type == SDL_KEYDOWN) // Key press
						qPressed = true;
				}
				else if (keyPressed == SDLK_g)
				{
					if (event.type == SDL_KEYUP) // Key release
						gPressed = false;
					else if (event.type == SDL_KEYDOWN) // Key press
						gPressed = true;
				}
				else if (keyPressed == SDLK_r)
				{
					if (event.type == SDL_KEYUP) // Key release
						rPressed = false;
					else if (event.type == SDL_KEYDOWN) // Key press
						rPressed = true;
				}
				else if (keyPressed == SDLK_l)
				{
					if (event.type == SDL_KEYUP) // Key release
						lPressed = false;
					else if (event.type == SDL_KEYDOWN) // Key press
						lPressed = true;
				}
				else if (keyPressed == SDLK_UP)
				{
					if (event.type == SDL_KEYUP) // Key release
						upPressed = false;
					else if (event.type == SDL_KEYDOWN) // Key press
						upPressed = true;
				}
				else if (keyPressed == SDLK_DOWN)
				{
					if (event.type == SDL_KEYUP) // Key release
						downPressed = false;
					else if (event.type == SDL_KEYDOWN) // Key press
						downPressed = true;
				}
				else if (keyPressed == SDLK_LEFT)
				{
					if (event.type == SDL_KEYUP) // Key release
						leftPressed = false;
					else if (event.type == SDL_KEYDOWN) // Key press
						leftPressed = true;
				}
				else if (keyPressed == SDLK_RIGHT)
				{
					if (event.type == SDL_KEYUP) // Key release
						rightPressed = false;
					else if (event.type == SDL_KEYDOWN) // Key press
						rightPressed = true;
				}

				// which command
				if (CTRLPressed)
				{
					if (qPressed)
						running = false;

					if (gPressed)
						running = false;

					/*if ((fPressed) && (uPressed))
						NNetwork::caboose = true;*/

					if (lPressed)
						system("clear");
				}

				// quit the gui window
				if (keyPressed == SDLK_ESCAPE)
					running = false;
			}


				if ((event.type == SDL_MOUSEBUTTONDOWN) || (event.type == SDL_MOUSEBUTTONUP) ||
					(event.type == SDL_MOUSEMOTION))
				{
					mouseX = event.button.x;
					mouseY = event.button.y;
				}

				// Events for the focused panel
				if (focusedPanel)
					focusedPanel->processSubItemEvents(this, NULL, NULL, event, mouseX, mouseY);
		}

		//=================Render 2D=================
		// Render the focused panel
		if (focusedPanel)
		{
		    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
		    SDL_RenderClear(renderer);
		    focusedPanel->updateBackgroundHelper(this);
		}

		// fps
		now = SDL_GetTicks();
		float cFrames = ((float)frames * 1000.0f) / ((float)(now - then));
		/*if (running)
		{
			char fpsBuffer[26];
			bzero(&fpsBuffer, 26);
			sprintf(fpsBuffer, "%2.1f fps", cFrames);
		}*/

		// Cap the frame rate
		fps = cFrames;
		if ((cFrames - MAX_FRAMES_PER_SECOND > 0) && (cFrames > MAX_FRAMES_PER_SECOND))
			SDL_Delay((cFrames - MAX_FRAMES_PER_SECOND) * 10.0f);

		//Scale the screen to the window size:
		SDL_RenderSetLogicalSize(renderer, getWidth(), getHeight());

		// Update the screen
		if (renderer)
			SDL_RenderPresent(renderer);
	}
}

// UPDATE FUNCTION
void gfxpp::clean2D()
{
	// clean up the componenets
	focusedItem = NULL;

	// gui
	for (unsigned int i = 0; i < guiElements.size(); ++i)
	{
		GItem* cItem = guiElements[i];
		if (cItem)
			delete cItem;
	}

	guiElements.clear();

	if (renderer)
	{
		//SDL_DestroyRenderer(renderer); // TODO: CRASHES
		renderer = NULL;
	}

	if (cFont)
		delete cFont;
	cFont = NULL;

	// Iterate GFonts in graphicsFonts
	/*std::map<int, GFont*>::iterator itr = graphicsFonts.begin();
	for (; itr != graphicsFonts.end(); ++itr)
	{
		if (itr->second)
			delete itr->second;
		itr->second = NULL;
	}*/
}

void gfxpp::finish()
{
	running = false;
	clean2D();

	if (window)
	{
		SDL_DestroyWindow(window);
		window = NULL;
	}

	SDL_Quit();
	TTF_Quit();
	IMG_Quit();
}

bool gfxpp::getRunning() const
{
	return running;
}

unsigned int gfxpp::RGBfromHue(double hue, int8_t* r, int8_t* g, int8_t* b)
{
	int h = int(hue * 256 * 6);
	int x = h % 0x100;

	(*r) = 0;
	(*g) = 0;
	(*b) = 0;
	switch (h / 256)
	{
	case 0:
		(*r) = 0xFF;
		(*g) = x;
		break;
	case 1:
		(*g) = 0xFF;
		(*r) = 0xFF - x;
		break;
	case 2:
		(*g) = 0xFF;
		(*b) = x;
		break;
	case 3:
		(*b) = 0xFF;
		(*g) = 0xFF - x;
		break;
	case 4:
		(*b) = 0xFF;
		(*r) = x;
		break;
	case 5:
		(*r) = 0xFF;
		(*b) = 0xFF - x;
		break;
	}

	return (*r) + ((*g) << 8) + ((*b) << 16);
}

SDL_Cursor* gfxpp::getSystemCursor()
{
	return systemCursor;
}

void gfxpp::addGradient(int x, int y, int size)
{
	// check the renderer
	if (!renderer)
	{
		printf("[GFX] Renderer error: %s\n", SDL_GetError());
		return;
	}

	/*for (int i = (-(size / 2)); i < size / 2; ++i)
	{
		for (int j = (-(size / 2)); j < size / 2; ++j)
		{
			// calculate the hue
			double hue = ((double)((i * i) + (j * j))) / ((double)(size * size));

			// get the color
			int8_t redMask = 0;
			int8_t greenMask = 0;
			int8_t blueMask = 0;
			unsigned int colorMask = RGBfromHue(hue, &redMask, &greenMask, &blueMask);

			// set the color and draw the point
			SDL_SetRenderDrawColor(renderer, redMask, greenMask, blueMask, SDL_ALPHA_OPAQUE);
			SDL_RenderDrawPoint(renderer, x + i, y + j);
		}
	}*/
}

void gfxpp::addItem(GItem* newItem)
{
	if (!newItem)
		return;

	const shmea::GString options = "0123456789";
	const int keyLength = 6;
	shmea::GString newItemID = "";
	int itemID = 0;

	do
	{
		newItemID = "";
		for (int i = 0; i < keyLength; ++i)
		{
			int newIndex = rand() % options.length();
			char newChar = options[newIndex];
			newItemID += newChar;
		}

		itemID = atoi(newItemID.c_str());

	} while (getItemByID(itemID)); // Generate new item id if already used

	// Assign same id to object of GItem and add it
	newItem->setID(itemID);
	guiElements.push_back(newItem);
}

// Unique ID for each item, but names may be the same.
void gfxpp::removeItem(int itemID)
{
	if (!itemID)
		return;

	for (unsigned int i = 0; i < guiElements.size(); ++i)
	{
		if (guiElements[i]->getID() == itemID)
		{
			guiElements.erase(guiElements.begin() + i);
			break;
		}
	}
}

// Unique ID for each item, but names may be the same.
GItem* gfxpp::getItemByID(int itemID)
{
	for (unsigned int i = 0; i < guiElements.size(); ++i)
	{
		if (guiElements[i]->getID() == itemID)
			return guiElements[i];
	}
	return NULL;
}

void gfxpp::setFocus(GItem* newFocusedItem)
{
	if (!newFocusedItem)
		return;

	if (focusedItem)
	{
		focusedItem->unsetFocus();
		if (focusedPanel)
			focusedItem->triggerLoseFocusEvent(focusedPanel);
	}

	focusedItem = newFocusedItem;
	focusedItem->setFocus();
}

int gfxpp::getWidth() const
{
	return width;
}

int gfxpp::getHeight() const
{
	return height;
}

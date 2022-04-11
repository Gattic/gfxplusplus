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
#include "../GFXUtilities/quaternion.h"
#include "../GItems/GItem.h"
#include "../GItems/GLayout.h"
#include "../GItems/RUColors.h"
#include "../GItems/RUComponent.h"
#include "../GUI/Text/GFont.h"
#include "../GUI/Text/RULabel.h"
#include "object.h"

gfxpp::gfxpp()
{
	systemCursor = NULL;
	width = 800;
	height = 600;
	renderStatus = _2D;

	errorFlag = initHelper(true, "gfxplusplus");
}

gfxpp::gfxpp(shmea::GString newTitle, int newRenderStatus, bool fullScreenMode, int newWidth,
			 int newHeight)
{
	systemCursor = NULL;
	renderStatus = newRenderStatus;
	width = newWidth;
	height = newHeight;

	errorFlag = initHelper(fullScreenMode, newTitle);
}

int gfxpp::getErrorFlag() const
{
	return errorFlag;
}

SDL_Renderer* gfxpp::getRenderer()
{
	return renderer;
}

int gfxpp::initHelper(bool fullscreenMode, shmea::GString title)
{
	running = false;
	hunterZolomon = 1.0f;
	renderStatus = _2D;

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

	fpsLabel = NULL;

	roll = Quaternion(0.0f, 1.0f, 0.0f, 0.0f);
	pitch = Quaternion(0.0f, 0.0f, 1.0f, 0.0f);
	yaw = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
	cObjIndex = -1;

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
		window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
								  getWidth(), getHeight(),
								  SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN_DESKTOP);

		// get the width and height
		SDL_DisplayMode DM;
		int errorNo = SDL_GetDesktopDisplayMode(0, &DM);
		if (errorNo < 0)
			return -5;
		width = DM.w;
		height = DM.h;
	}
	else
		window =
			SDL_CreateWindow("gfxplusplus", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
							 getWidth(), getHeight(), SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

	if (!window)
	{
		printf("[GFX] Window error: %s\n", SDL_GetError());
		finish();
		return -2;
	}

	if (renderStatus == _2D)
	{
		int errorNo = init2D();
		if (errorNo < 0)
			return errorNo;
	}
	else if (renderStatus == _3D)
		init3D();

	// Set the SDL system cursor
	systemCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);

	// ALl good with initialization, return success
	return 0;
}

int gfxpp::init2D()
{
	// Create a new renderer; -1 loads the default video driver we need
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_TARGETTEXTURE);
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

void gfxpp::init3D()
{
	// Create a new context
	context = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1);

	// Set 3D rendering settings
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glShadeModel(GL_SMOOTH);
}

void gfxpp::run()
{
	if (renderStatus == _2D)
	{
		// Set the FPS Component
		fpsLabel = new RULabel();
		fpsLabel->setWidth(60);
		fpsLabel->setHeight(20);
		fpsLabel->setX(10);
		fpsLabel->setY(1050);
		fpsLabel->setText("");
		// fpsLabel->setFontSize(40);
		// fpsLabel->toggleBG(false);
		addItem(fpsLabel);
	}
	else if (renderStatus == _3D)
	{
		// Other testing
		// SDL_Color textColor={0x00, 0x00, 0x00};
		// Graphics::addText("Hello World!", textColor);
		// Graphics::addBMP("graphics/xorgate.bmp");
		// Graphics::addGradient(800/2, 600/2, 500);
		// Graphics::addBasis();
		addCube();
	}

	// the display loop
	display();
	//finish();
}

void gfxpp::display()
{
	running = true;
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

				// Cycle between objects
				if (spacePressed)
				{
					if (objects.size() > 0)
					{
						++cObjIndex;
						if (cObjIndex >= objects.size())
							cObjIndex = 0;
					}
				}

				// quit the gui window
				if (keyPressed == SDLK_ESCAPE)
					running = false;
			}

			if (renderStatus == _2D)
			{

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
			else if (renderStatus == _3D)
			{
				// are we holding click?
				if (event.type == SDL_MOUSEBUTTONDOWN)
				{
					if (CTRLPressed)
					{
						rotate = true;
						move = false;
					}
					else
					{
						rotate = false;
						move = true;
					}
				}
				else if (event.type == SDL_MOUSEBUTTONUP)
				{
					rotate = false;
					move = false;
				}

				// Scrollwheel
				if (event.type == SDL_MOUSEWHEEL)
				{
					if (rPressed)
					{
						// Move the Object (z)
						if ((objects.size() > 0) && (cObjIndex != (unsigned int)-1))
						{
							Vec3 position = objects[cObjIndex]->getCenter();
							Vec3 mouseVec(0.0f, 0.0f, event.wheel.y);
							position = position + mouseVec;

							// Set the center/position of the selected object
							objects[cObjIndex]->setCenter(position);
						}
					}
					else
					{
						// Camera Zoom
						if (event.wheel.y > 0)
						{
							// Scroll down
							if (hunterZolomon > 0.0f)
								hunterZolomon -= 0.1;
						}
						else if (event.wheel.y < 0)
						{
							// Scroll up
							hunterZolomon += 0.1;
						}
					}
				}

				// Interact with the object
				if ((objects.size() > 0) && (cObjIndex != (unsigned int)-1))
				{
					if (event.type == SDL_MOUSEMOTION)
					{
						double a = event.motion.yrel;
						double b = event.motion.xrel;

						// Rotate the object
						if (rotate)
						{
							Quaternion rotation = objects[cObjIndex]->getRotation();

							// Normalize the rotation quat
							rotation.normalize();

							// Create the mouse movement quat
							Quaternion mouseQuat(360, a, b, 0);
							mouseQuat.normalize();

							// Apply the change vector
							rotation = rotation * mouseQuat;
							rotation.normalize();

							// Set the rotation of the selected object
							objects[cObjIndex]->setRotation(rotation);
						}

						// Move the Object (x and y)
						if (move)
						{
							Vec3 position = objects[cObjIndex]->getCenter();
							Vec3 mouseVec(b / 40.0f, -a / 40.0f, 0.0f);
							position = position + mouseVec;

							// Set the center/position of the selected object
							objects[cObjIndex]->setCenter(position);
						}
					}

					// Edit the dimensions of the object
					if (upPressed)
					{
						Vec3 cDimensions = objects[cObjIndex]->getDimensions();
						Vec3 mouseVec(0.0f, 0.1f, 0.0f);
						cDimensions = cDimensions + mouseVec;

						// Set the center/cDimensions of the selected object
						objects[cObjIndex]->setDimensions(cDimensions);
					}
					else if (downPressed)
					{
						Vec3 cDimensions = objects[cObjIndex]->getDimensions();
						Vec3 mouseVec(0.0f, -0.1f, 0.0f);
						cDimensions = cDimensions + mouseVec;

						// Set the center/cDimensions of the selected object
						objects[cObjIndex]->setDimensions(cDimensions);
					}
					else if (leftPressed)
					{
						Vec3 cDimensions = objects[cObjIndex]->getDimensions();
						Vec3 mouseVec(0.1f, 0.0f, 0.0f);
						cDimensions = cDimensions + mouseVec;

						// Set the center/cDimensions of the selected object
						objects[cObjIndex]->setDimensions(cDimensions);
					}
					else if (rightPressed)
					{
						Vec3 cDimensions = objects[cObjIndex]->getDimensions();
						Vec3 mouseVec(-0.1f, 0.0f, 0.0f);
						cDimensions = cDimensions + mouseVec;

						// Set the center/cDimensions of the selected object
						objects[cObjIndex]->setDimensions(cDimensions);
					}
				}
			}
		}

		//=================Render=================
		if (renderStatus == _2D)
		{
			// Render the focused panel
			if (focusedPanel)
			{
				SDL_RenderClear(renderer);
				focusedPanel->updateBackgroundHelper(this);
			}
		}
		else if (renderStatus == _3D)
		{
			// Set our viewport
			glMatrixMode(GL_PROJECTION);
			SDL_GL_MakeCurrent(window, context);
			glViewport(0, 0, getWidth(), getHeight());

			// gray background color
			glClearColor(0.38f, 0.38f, 0.38f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// orientation
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glOrtho(-2.0f * hunterZolomon, 2.0f * hunterZolomon, -2.0f * hunterZolomon,
					2.0f * hunterZolomon, -20.0f * hunterZolomon, 20.0f * hunterZolomon);

			// Render the objects
			std::vector<Object*>::const_iterator itr = objects.begin();
			for (; itr != objects.end(); ++itr)
				(*itr)->Render();

			SDL_GL_SwapWindow(window);
			glFlush();
		}

		// fps
		now = SDL_GetTicks();
		float cFrames = ((float)frames * 1000.0f) / ((float)(now - then));
		if (fpsLabel)
		{
			char fpsBuffer[26];
			bzero(&fpsBuffer, 26);
			sprintf(fpsBuffer, "%2.1f fps", cFrames);
			fpsLabel->setText(fpsBuffer);
		}

		// Cap the frame rate
		if ((cFrames - MAX_FRAMES_PER_SECOND > 0) && (cFrames > MAX_FRAMES_PER_SECOND))
			SDL_Delay((cFrames - MAX_FRAMES_PER_SECOND) * 10.0f);

		// global gui elements
		if (fpsLabel)
			fpsLabel->updateBackgroundHelper(this);

		// Update the screen
		if (renderer)
			SDL_RenderPresent(renderer);
	}
}

void gfxpp::changeRenderStatus(int newRenderStatus)
{
	if (renderStatus != newRenderStatus)
	{
		// New SDL instance
		int errorNo = 0;
		renderStatus = newRenderStatus;
		if (renderStatus == _2D)
		{
			errorNo = init2D();

			// Set the FPS Component
			fpsLabel = new RULabel();
			fpsLabel->setWidth(200);
			fpsLabel->setHeight(40);
			fpsLabel->setX(width - fpsLabel->getWidth() - 6);
			fpsLabel->setY(0);
			fpsLabel->setText("");
			// fpsLabel->setFontSize(40);
			// fpsLabel->toggleBG(false);
			addItem(fpsLabel);
		}
		else if (renderStatus == _3D)
		{
			init3D();
			addBasis();
			addCube();
		}

		// switch the type
		if (errorNo != 0)
			running = false;
		else
		{
			/*if(renderStatus == _2D)
				clean3D();
			else if(renderStatus == _3D)
				clean2D();*/
		}
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

void gfxpp::clean3D()
{
	// gui
	std::vector<Object*>::iterator itr = objects.begin();
	for (; itr != objects.end(); ++itr)
	{
		Object* cObject = (*itr);
		if (cObject)
			delete cObject;
	}
	objects.clear();

	SDL_GL_DeleteContext(context);
}

void gfxpp::finish()
{
	running = false;
	if (renderStatus == _2D)
		clean2D();
	else if (renderStatus == _3D)
		clean3D();

	if (window)
	{
		SDL_DestroyWindow(window);
		window = NULL;
	}

	SDL_Quit();
	TTF_Quit();
	IMG_Quit();
}

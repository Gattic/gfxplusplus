#include "gfxpp3D.h"
#include "../GFXUtilities/quaternion.h"
#include "object.h"

gfxpp3D::gfxpp3D()
{
    width = 800;
    height = 600;

    errorFlag = initHelper(false, "gfxplusplus", true);

}

gfxpp3D::gfxpp3D(shmea::GString newTitle, bool fullScreenMode, bool compatMode, int newWidth, int newHeight)
{
	width = newWidth;
	height = newHeight;

	errorFlag = initHelper(fullScreenMode, newTitle, compatMode);
}

int gfxpp3D::getErrorFlag() const
{
	return errorFlag;
}

int gfxpp3D::initHelper(bool fullscreenMode, shmea::GString title, bool compatMode)
{
    running = false;
    hunterZolomon = 1.0f;

    frames = 0;
    rotate = false;
    move = false;
    now = 0;
    then = SDL_GetTicks();

    //For Mouse
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
	//Removed 2d vs 3d if else statement
	init3D();

	// ALl good with initialization, return success
	return 0;
}

void gfxpp3D::init3D()
{
	// Create a new context
	context = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(1);

	// Set 3D rendering settings
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glShadeModel(GL_SMOOTH);
}

void gfxpp3D::run()
{
    // Other testing
    // SDL_Color textColor={0x00, 0x00, 0x00};
    // Graphics::addText("Hello World!", textColor);
    // Graphics::addBMP("graphics/xorgate.bmp");
    // Graphics::addGradient(800/2, 600/2, 500);
    // Graphics::addBasis();
    addCube();

    display();
    finish();

}


void gfxpp3D::display()
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
		glOrtho(-2.0f * hunterZolomon, 2.0f * hunterZolomon, -2.0f * hunterZolomon, 2.0f * hunterZolomon, -20.0f * hunterZolomon, 20.0f * hunterZolomon);

		// Render the objects
		std::vector<Object*>::const_iterator itr = objects.begin();
		for (; itr != objects.end(); ++itr)
		    (*itr)->Render();
		SDL_GL_SwapWindow(window);
		glFlush();

	}
}

void gfxpp3D::clean3D()
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

void gfxpp3D::finish()
{
	running = false;
	clean3D();

	if (window)
	{
		SDL_DestroyWindow(window);
		window = NULL;
	}

	SDL_Quit();
}

bool gfxpp3D::getRunning() const
{
	return running;
}

void gfxpp3D::addCube()
{
	Object* newObject = (Object*)malloc(sizeof(Object));
	*newObject = Object();
	newObject->setType(Object::CUBE_TYPE);

	if (!contains(newObject))
		objects.push_back(newObject);
}

void gfxpp3D::addBasis()
{
	Object* newObject = (Object*)malloc(sizeof(Object));
	*newObject = Object();
	newObject->setType(Object::BASIS_TYPE);

	if (!contains(newObject))
		objects.push_back(newObject);
}

bool gfxpp3D::contains(const Object* newObject) const
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

bool gfxpp3D::contains(const Object newObject) const
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


int gfxpp3D::getWidth() const
{
	return width;
}

int gfxpp3D::getHeight() const
{
	return height;
}

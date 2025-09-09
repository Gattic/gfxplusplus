#include "GfxRenderer.h"
#ifdef GFX_HAVE_SDL2
#include "GfxRendererSDL.h"
#endif
#ifdef GFX_HAVE_OPENGL
#ifdef GFX_HAVE_SDL2
#include "GfxRendererGL.h"
#endif
#include "GfxRendererGLFW.h"
#endif

GfxRenderer* GfxRenderer::createSDL(GfxNativeRenderer* sdlRenderer, SDL_Window* sdlWindow)
{
#ifdef GFX_HAVE_SDL2
	return new GfxRendererSDL(sdlRenderer, sdlWindow);
#else
	(void)sdlRenderer; (void)sdlWindow; return NULL;
#endif
}

GfxRenderer* GfxRenderer::createOpenGL(SDL_Window* sdlWindow, SDL_GLContext glContext)
{
#if defined(GFX_HAVE_OPENGL) && defined(GFX_HAVE_SDL2)
	return new GfxRendererGL(sdlWindow, glContext);
#else
	(void)sdlWindow; (void)glContext; return NULL;
#endif
}

GfxRenderer* GfxRenderer::createOpenGL(GLFWwindow* glfwWindow)
{
#ifdef GFX_HAVE_OPENGL
	return new GfxRendererGLFW(glfwWindow);
#else
	(void)glfwWindow; return NULL;
#endif
}



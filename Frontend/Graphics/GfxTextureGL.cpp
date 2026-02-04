// OpenGL texture wrapper implementation for builds without SDL2.
#include "GfxTextureGL.h"

#if defined(GFX_HAVE_OPENGL) && !defined(GFX_HAVE_SDL2)
#include <GL/gl.h>

// Minimal EXT/core function pointer typedef to delete FBOs without GLEW.
#ifndef APIENTRY
#define APIENTRY
#endif
typedef void (APIENTRY *PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei n, const GLuint* framebuffers);

void gfxDestroyTexture(GfxTexture* texture)
{
	if (!texture)
		return;

	GfxTextureOpaque* t = reinterpret_cast<GfxTextureOpaque*>(texture);
	if (t->fboId)
	{
		// Try core then EXT entry points via GLFW.
		PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffersP =
			(PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffers");
		if (!glDeleteFramebuffersP)
			glDeleteFramebuffersP =
				(PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffersEXT");
		if (glDeleteFramebuffersP)
			glDeleteFramebuffersP(1, &t->fboId);
		t->fboId = 0;
	}

	if (t->textureId)
	{
		glDeleteTextures(1, &t->textureId);
		t->textureId = 0;
	}

	delete t;
}

#else
// If SDL2 is enabled or OpenGL is disabled, this translation unit is unused.
#endif


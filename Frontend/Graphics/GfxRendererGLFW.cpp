#include "GfxRendererGLFW.h"

#ifdef GFX_HAVE_OPENGL
#include <GL/gl.h>

#if !defined(GFX_HAVE_SDL2)
#include "GfxTextureGL.h"
#ifndef _WIN32
#include <GL/glext.h>
#endif

// Fallback constants if headers don't provide them (common without GLEW).
#ifndef GL_CLAMP_TO_EDGE
#define GL_CLAMP_TO_EDGE 0x812F
#endif
#ifndef GL_FRAMEBUFFER
#define GL_FRAMEBUFFER 0x8D40
#endif
#ifndef GL_FRAMEBUFFER_EXT
#define GL_FRAMEBUFFER_EXT 0x8D40
#endif
#ifndef GL_COLOR_ATTACHMENT0
#define GL_COLOR_ATTACHMENT0 0x8CE0
#endif
#ifndef GL_COLOR_ATTACHMENT0_EXT
#define GL_COLOR_ATTACHMENT0_EXT 0x8CE0
#endif
#ifndef GL_FRAMEBUFFER_COMPLETE
#define GL_FRAMEBUFFER_COMPLETE 0x8CD5
#endif
#ifndef GL_FRAMEBUFFER_COMPLETE_EXT
#define GL_FRAMEBUFFER_COMPLETE_EXT 0x8CD5
#endif

// EXT/core function pointer typedefs (minimal set we need)
#ifndef APIENTRY
#define APIENTRY
#endif
typedef void (APIENTRY *PFNGLGENFRAMEBUFFERSPROC)(GLsizei n, GLuint* framebuffers);
typedef void (APIENTRY *PFNGLBINDFRAMEBUFFERPROC)(GLenum target, GLuint framebuffer);
typedef void (APIENTRY *PFNGLFRAMEBUFFERTEXTURE2DPROC)(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
typedef GLenum (APIENTRY *PFNGLCHECKFRAMEBUFFERSTATUSPROC)(GLenum target);
typedef void (APIENTRY *PFNGLDELETEFRAMEBUFFERSPROC)(GLsizei n, const GLuint* framebuffers);

static void* getGLProc(GLFWwindow* window, const char* name)
{
	(void)window;
	return (void*)glfwGetProcAddress(name);
}

static void loadFBOProcs(GLFWwindow* window,
			 void*& genFB,
			 void*& bindFB,
			 void*& fbTex2D,
			 void*& checkFB,
			 void*& delFB)
{
	// Try core names first
	genFB = getGLProc(window, "glGenFramebuffers");
	bindFB = getGLProc(window, "glBindFramebuffer");
	fbTex2D = getGLProc(window, "glFramebufferTexture2D");
	checkFB = getGLProc(window, "glCheckFramebufferStatus");
	delFB = getGLProc(window, "glDeleteFramebuffers");

	// Fall back to EXT names
	if (!genFB) genFB = getGLProc(window, "glGenFramebuffersEXT");
	if (!bindFB) bindFB = getGLProc(window, "glBindFramebufferEXT");
	if (!fbTex2D) fbTex2D = getGLProc(window, "glFramebufferTexture2DEXT");
	if (!checkFB) checkFB = getGLProc(window, "glCheckFramebufferStatusEXT");
	if (!delFB) delFB = getGLProc(window, "glDeleteFramebuffersEXT");
}

#endif // !GFX_HAVE_SDL2

GfxRendererGLFW::GfxRendererGLFW(GLFWwindow* w)
	: window(w),
	  r(0),
	  g(0),
	  b(0),
	  a(255),
	  logicalW(0),
	  logicalH(0)
#if defined(GFX_HAVE_OPENGL) && !defined(GFX_HAVE_SDL2)
	  ,
	  p_glGenFramebuffers(NULL),
	  p_glBindFramebuffer(NULL),
	  p_glFramebufferTexture2D(NULL),
	  p_glCheckFramebufferStatus(NULL),
	  p_glDeleteFramebuffers(NULL),
	  currentTarget(NULL)
#endif
{
#if defined(GFX_HAVE_OPENGL) && !defined(GFX_HAVE_SDL2)
	loadFBOProcs(window, p_glGenFramebuffers, p_glBindFramebuffer, p_glFramebufferTexture2D,
		     p_glCheckFramebufferStatus, p_glDeleteFramebuffers);
#endif
}

GfxRendererGLFW::~GfxRendererGLFW()
{
	// Nothing: textures are owned/destroyed by callers via GFX_DestroyTexture.
}

void GfxRendererGLFW::setDrawColor(Uint8 rr, Uint8 gg, Uint8 bb, Uint8 aa)
{
	r = rr;
	g = gg;
	b = bb;
	a = aa;
	glColor4ub(r, g, b, a);
}

void GfxRendererGLFW::clear()
{
	glClearColor(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void GfxRendererGLFW::present()
{
	glfwSwapBuffers(window);
}

void GfxRendererGLFW::setLogicalSize(int width, int height)
{
	logicalW = width;
	logicalH = height;

	int fbW = 0, fbH = 0;
	glfwGetFramebufferSize(window, &fbW, &fbH);
	if (fbW <= 0 || fbH <= 0)
	{
		fbW = width;
		fbH = height;
	}
	glViewport(0, 0, fbW, fbH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (GLdouble)width, (GLdouble)height, 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void GfxRendererGLFW::drawPoint(int x, int y)
{
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_POINTS);
	glVertex2i(x, y);
	glEnd();
}

void GfxRendererGLFW::drawPoints(const std::vector<GfxPoint>& points)
{
	if (points.empty())
		return;
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_POINTS);
	for (size_t i = 0; i < points.size(); ++i)
		glVertex2i(points[i].x, points[i].y);
	glEnd();
}

void GfxRendererGLFW::drawLine(int x1, int y1, int x2, int y2)
{
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINES);
	glVertex2i(x1, y1);
	glVertex2i(x2, y2);
	glEnd();
}

void GfxRendererGLFW::drawLines(const std::vector<GfxPoint>& points)
{
	if (points.size() < 2)
		return;
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINE_STRIP);
	for (size_t i = 0; i < points.size(); ++i)
		glVertex2i(points[i].x, points[i].y);
	glEnd();
}

void GfxRendererGLFW::drawRect(const GfxRect* rect)
{
	if (!rect)
		return;
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINE_LOOP);
	glVertex2i(rect->x, rect->y);
	glVertex2i(rect->x + rect->w, rect->y);
	glVertex2i(rect->x + rect->w, rect->y + rect->h);
	glVertex2i(rect->x, rect->y + rect->h);
	glEnd();
}

void GfxRendererGLFW::fillRect(const GfxRect* rect)
{
	if (!rect)
		return;
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
	glVertex2i(rect->x, rect->y);
	glVertex2i(rect->x + rect->w, rect->y);
	glVertex2i(rect->x + rect->w, rect->y + rect->h);
	glVertex2i(rect->x, rect->y + rect->h);
	glEnd();
}

void GfxRendererGLFW::copyTexture(GfxTexture* texture, const GfxRect* src, const GfxRect* dst)
{
#if defined(GFX_HAVE_OPENGL) && !defined(GFX_HAVE_SDL2)
	if (!texture)
		return;

	GfxTextureOpaque* t = reinterpret_cast<GfxTextureOpaque*>(texture);
	if (!t->textureId || t->width <= 0 || t->height <= 0)
		return;

	float u0 = 0.0f, v0 = 0.0f, u1 = 1.0f, v1 = 1.0f;
	if (src && src->w > 0 && src->h > 0)
	{
		u0 = (float)src->x / (float)t->width;
		v0 = (float)src->y / (float)t->height;
		u1 = (float)(src->x + src->w) / (float)t->width;
		v1 = (float)(src->y + src->h) / (float)t->height;
	}

	// FBO-rendered textures are stored with OpenGL's bottom-left origin, while our UI coordinate
	// system is top-left. Flip V when blitting render targets back to the screen.
	if (t->isRenderTarget)
	{
		v0 = 1.0f - v0;
		v1 = 1.0f - v1;
	}

	int dx = 0, dy = 0, dw = t->width, dh = t->height;
	if (dst)
	{
		dx = dst->x;
		dy = dst->y;
		dw = dst->w;
		dh = dst->h;
	}

	// Apply blend mode
	if (t->blendMode == GFX_BLENDMODE_NONE)
		glDisable(GL_BLEND);
	else
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, t->textureId);
	glColor4ub(255, 255, 255, 255);
	glBegin(GL_QUADS);
		glTexCoord2f(u0, v0); glVertex2i(dx, dy);
		glTexCoord2f(u1, v0); glVertex2i(dx + dw, dy);
		glTexCoord2f(u1, v1); glVertex2i(dx + dw, dy + dh);
		glTexCoord2f(u0, v1); glVertex2i(dx, dy + dh);
	glEnd();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);

	// Restore current draw color
	glColor4ub(r, g, b, a);
#else
	(void)texture;
	(void)src;
	(void)dst;
#endif
}

GfxTexture* GfxRendererGLFW::createRenderTargetTexture(int width, int height)
{
#if defined(GFX_HAVE_OPENGL) && !defined(GFX_HAVE_SDL2)
	if (width <= 0 || height <= 0)
		return NULL;
	if (!p_glGenFramebuffers || !p_glBindFramebuffer || !p_glFramebufferTexture2D || !p_glCheckFramebufferStatus)
		return NULL;

	GfxTextureOpaque* t = new GfxTextureOpaque();
	t->width = width;
	t->height = height;
	t->isRenderTarget = true;

	glGenTextures(1, &t->textureId);
	glBindTexture(GL_TEXTURE_2D, t->textureId);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	PFNGLGENFRAMEBUFFERSPROC glGenFramebuffersP = (PFNGLGENFRAMEBUFFERSPROC)p_glGenFramebuffers;
	PFNGLBINDFRAMEBUFFERPROC glBindFramebufferP = (PFNGLBINDFRAMEBUFFERPROC)p_glBindFramebuffer;
	PFNGLFRAMEBUFFERTEXTURE2DPROC glFramebufferTexture2DP = (PFNGLFRAMEBUFFERTEXTURE2DPROC)p_glFramebufferTexture2D;
	PFNGLCHECKFRAMEBUFFERSTATUSPROC glCheckFramebufferStatusP = (PFNGLCHECKFRAMEBUFFERSTATUSPROC)p_glCheckFramebufferStatus;

	glGenFramebuffersP(1, &t->fboId);
	glBindFramebufferP(GL_FRAMEBUFFER_EXT, t->fboId);
	glFramebufferTexture2DP(GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, t->textureId, 0);
	GLenum status = glCheckFramebufferStatusP(GL_FRAMEBUFFER_EXT);
	glBindFramebufferP(GL_FRAMEBUFFER_EXT, 0);

	if (status != GL_FRAMEBUFFER_COMPLETE_EXT)
	{
		// Clean up
		if (t->fboId && p_glDeleteFramebuffers)
		{
			PFNGLDELETEFRAMEBUFFERSPROC glDeleteFramebuffersP = (PFNGLDELETEFRAMEBUFFERSPROC)p_glDeleteFramebuffers;
			glDeleteFramebuffersP(1, &t->fboId);
		}
		if (t->textureId)
			glDeleteTextures(1, &t->textureId);
		delete t;
		return NULL;
	}

	return (GfxTexture*)t;
#else
	(void)width;
	(void)height;
	return NULL;
#endif
}

void GfxRendererGLFW::setTargetTexture(GfxTexture* texture)
{
#if defined(GFX_HAVE_OPENGL) && !defined(GFX_HAVE_SDL2)
	if (!texture)
		return;

	if (!p_glBindFramebuffer)
		return;

	GfxTextureOpaque* t = reinterpret_cast<GfxTextureOpaque*>(texture);
	if (!t->isRenderTarget || !t->fboId)
		return;

	PFNGLBINDFRAMEBUFFERPROC glBindFramebufferP = (PFNGLBINDFRAMEBUFFERPROC)p_glBindFramebuffer;
	glBindFramebufferP(GL_FRAMEBUFFER_EXT, t->fboId);
	currentTarget = texture;

	// Render into the texture in pixel coordinates with top-left origin.
	glViewport(0, 0, t->width, t->height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0, (GLdouble)t->width, (GLdouble)t->height, 0.0, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
#else
	(void)texture;
#endif
}

void GfxRendererGLFW::resetTarget()
{
#if defined(GFX_HAVE_OPENGL) && !defined(GFX_HAVE_SDL2)
	if (!p_glBindFramebuffer)
		return;
	PFNGLBINDFRAMEBUFFERPROC glBindFramebufferP = (PFNGLBINDFRAMEBUFFERPROC)p_glBindFramebuffer;
	glBindFramebufferP(GL_FRAMEBUFFER_EXT, 0);
	currentTarget = NULL;

	// Restore projection/viewport for the main framebuffer.
	if (logicalW > 0 && logicalH > 0)
		setLogicalSize(logicalW, logicalH);
#endif
}

void GfxRendererGLFW::setTextureBlendMode(GfxTexture* texture, int mode)
{
#if defined(GFX_HAVE_OPENGL) && !defined(GFX_HAVE_SDL2)
	if (!texture)
		return;
	GfxTextureOpaque* t = reinterpret_cast<GfxTextureOpaque*>(texture);
	t->blendMode = mode;
#else
	(void)texture;
	(void)mode;
#endif
}

GfxTexture* GfxRendererGLFW::createTextureFromSurface(GfxSurface* surface)
{
	// Not available without SDL2 surfaces. Image upload is handled in GUI code for GL.
	(void)surface;
	return NULL;
}

#endif // GFX_HAVE_OPENGL


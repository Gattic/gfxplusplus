#include "GLTextRenderer.h"
#include <vector>

GLTextRenderer::GLTextRenderer()
{
	initialized = false;
	ft = NULL;
	face = NULL;
    fontPathUsed = "";
    pixelHeightUsed = 0;
    maxBearingY = 0;
}

GLTextRenderer::~GLTextRenderer()
{
#ifdef GFX_HAVE_OPENGL
	for (std::map<char, GLGlyph>::iterator it = glyphs.begin(); it != glyphs.end(); ++it)
		glDeleteTextures(1, &it->second.textureId);
	glyphs.clear();
	if (face)
		FT_Done_Face(face);
	if (ft)
		FT_Done_FreeType(ft);
#else
	glyphs.clear();
#endif
}

bool GLTextRenderer::init(const std::string& fontPath, int pixelHeight)
{
#ifdef GFX_HAVE_OPENGL
	if (FT_Init_FreeType(&ft))
		return false;
	// Try provided path, then a few common fallbacks if that fails
	std::vector<std::string> candidates;
	candidates.push_back(fontPath);
	candidates.push_back("./" + fontPath);
	candidates.push_back("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
	candidates.push_back("/usr/share/fonts/truetype/freefont/FreeSans.ttf");
	candidates.push_back("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");
	candidates.push_back("/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf");
	candidates.push_back("resources/fonts/osaka-re.ttf");
	candidates.push_back("resources/fonts/Open_Sans/OpenSans-SemiBold.ttf");
	bool faceOk = false;
	for (size_t i = 0; i < candidates.size() && !faceOk; ++i)
	{
		if (FT_New_Face(ft, candidates[i].c_str(), 0, &face) == 0)
			faceOk = true;
	}
	if (!faceOk)
		return false;
	FT_Set_Pixel_Sizes(face, 0, pixelHeight);

	// Prepare OpenGL state
	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Load ASCII glyphs
	maxBearingY = 0;
	for (unsigned char c = 0; c < 128; ++c)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
			continue;
		FT_GlyphSlot g = face->glyph;
		GLuint tex;
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		// Ensure byte alignment for 1-byte glyph bitmaps
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);
		// Use fixed-function modulation so glColor tints the alpha glyph
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		GLGlyph glyph;
		glyph.textureId = tex;
		glyph.width = g->bitmap.width;
		glyph.height = g->bitmap.rows;
		glyph.bearingX = g->bitmap_left;
		glyph.bearingY = g->bitmap_top;
		glyph.advance = g->advance.x >> 6;
		glyphs.insert(std::make_pair(c, glyph));
		if (glyph.bearingY > maxBearingY)
			maxBearingY = glyph.bearingY;
	}
	initialized = true;
	// Best-effort record for change detection
	fontPathUsed = candidates[0];
	pixelHeightUsed = pixelHeight;
	return true;
#else
	(void)fontPath; (void)pixelHeight; return false;
#endif
}

bool GLTextRenderer::initOrUpdate(const std::string& fontPath, int pixelHeight)
{
#ifdef GFX_HAVE_OPENGL
    if (!initialized)
        return init(fontPath, pixelHeight);
    if (fontPath == fontPathUsed && pixelHeight == pixelHeightUsed)
        return true;
    // Recreate face and glyphs for new size/path
    for (std::map<char, GLGlyph>::iterator it = glyphs.begin(); it != glyphs.end(); ++it)
        glDeleteTextures(1, &it->second.textureId);
    glyphs.clear();
    if (face)
        FT_Done_Face(face);
    face = NULL;
    // Try provided path first, then fallbacks as in init()
    std::vector<std::string> candidates;
    candidates.push_back(fontPath);
    candidates.push_back("./" + fontPath);
    candidates.push_back("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    candidates.push_back("/usr/share/fonts/truetype/freefont/FreeSans.ttf");
    candidates.push_back("/usr/share/fonts/truetype/liberation/LiberationSans-Regular.ttf");
    candidates.push_back("/usr/share/fonts/truetype/ubuntu/Ubuntu-R.ttf");
    candidates.push_back("resources/fonts/osaka-re.ttf");
    candidates.push_back("resources/fonts/Open_Sans/OpenSans-SemiBold.ttf");
    bool faceOk = false;
    std::string usedPath = fontPath;
    for (size_t i = 0; i < candidates.size() && !faceOk; ++i)
    {
        if (FT_New_Face(ft, candidates[i].c_str(), 0, &face) == 0)
        {
            faceOk = true;
            usedPath = candidates[i];
        }
    }
    if (!faceOk)
        return false;
    FT_Set_Pixel_Sizes(face, 0, pixelHeight);
    maxBearingY = 0;
    for (unsigned char c = 0; c < 128; ++c)
    {
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            continue;
        FT_GlyphSlot g = face->glyph;
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);
        glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        GLGlyph glyph;
        glyph.textureId = tex;
        glyph.width = g->bitmap.width;
        glyph.height = g->bitmap.rows;
        glyph.bearingX = g->bitmap_left;
        glyph.bearingY = g->bitmap_top;
        glyph.advance = g->advance.x >> 6;
        glyphs.insert(std::make_pair(c, glyph));
        if (glyph.bearingY > maxBearingY)
            maxBearingY = glyph.bearingY;
    }
    fontPathUsed = usedPath;
    pixelHeightUsed = pixelHeight;
    return true;
#else
    (void)fontPath; (void)pixelHeight; return false;
#endif
}

void GLTextRenderer::drawText(float x, float y, const std::string& text, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
#ifdef GFX_HAVE_OPENGL
	if (!initialized)
		return;

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glColor4ub(r, g, b, a);
	// Fixed-function pipeline expects texturing enabled and projection/modelview already set.
	// Guard against stale GL state that could fully transparent the output.
	glDisable(GL_COLOR_LOGIC_OP);
	glDisable(GL_STENCIL_TEST);
	glDisable(GL_CULL_FACE);
	// Treat incoming (x,y) as the BASELINE position (x at left, y at baseline), consistent with
	// the previous renderer expectations. Normalize initial X by the first glyph's left bearing
	// so left edges align visually with SDL path.
	float baselineY = y;

	// Determine first drawable glyph to normalize left alignment by its bearingX
	int firstBearingX = 0;
	for (size_t i = 0; i < text.size(); ++i)
	{
		std::map<char, GLGlyph>::iterator it = glyphs.find(text[i]);
		if (it != glyphs.end())
		{
			firstBearingX = it->second.bearingX;
			break;
		}
	}
	float posX = x - (float)firstBearingX;
	for (size_t i = 0; i < text.size(); ++i)
	{
		char c = text[i];
		std::map<char, GLGlyph>::iterator it = glyphs.find(c);
		if (it == glyphs.end())
			continue;
		const GLGlyph& ch = it->second;
		float xpos = posX + (float)ch.bearingX;
		float ypos = baselineY - (float)ch.bearingY;
		float w = (float)ch.width;
		float h = (float)ch.height;
		glBindTexture(GL_TEXTURE_2D, ch.textureId);
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 1.0f); glVertex2f(xpos    , ypos + h);
			glTexCoord2f(1.0f, 1.0f); glVertex2f(xpos + w, ypos + h);
			glTexCoord2f(1.0f, 0.0f); glVertex2f(xpos + w, ypos    );
			glTexCoord2f(0.0f, 0.0f); glVertex2f(xpos    , ypos    );
		glEnd();
		posX += ch.advance;
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_2D);
#else
	(void)x; (void)y; (void)text; (void)r; (void)g; (void)b; (void)a;
#endif
}

int GLTextRenderer::measureTextWidth(const std::string& text) const
{
	int width = 0;
	for (size_t i = 0; i < text.size(); ++i)
	{
		std::map<char, GLGlyph>::const_iterator it = glyphs.find(text[i]);
		if (it == glyphs.end())
			continue;
		width += it->second.advance;
	}
	return width;
}

int GLTextRenderer::getGlyphAdvance(char c) const
{
	std::map<char, GLGlyph>::const_iterator it = glyphs.find(c);
	if (it == glyphs.end())
		return 0;
	return it->second.advance;
}



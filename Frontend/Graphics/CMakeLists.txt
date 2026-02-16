# Backend-specific dependencies
if(ENABLE_OPENGL)
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

# FreeType for OpenGL text rendering
find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})

# GLFW for OpenGL window/context and events (OpenGL backend)
find_package(glfw3 3.3 REQUIRED)
endif()

if(ENABLE_SDL2)
include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2 sdl2_ttf sdl2_image)
include_directories(${SDL2_INCLUDE_DIRS})
endif()

set(GFX_src_files
	graphics.cpp
	graphics.h
	GfxRenderer.cpp
	GfxRenderer.h
	InputState.cpp
	InputState.h
	FontManager.cpp
	FontManager.h
)

if(ENABLE_SDL2)
	list(APPEND GFX_src_files
		GfxRendererSDL.h)
endif()

if(ENABLE_OPENGL)
	list(APPEND GFX_src_files
		GfxRendererGLFW.h
		GfxRendererGLFW.cpp
		GlfwEventBridge.h
		GlfwEventBridge.cpp)
	# When SDL2 is disabled, provide OpenGL texture wrappers (FBO render targets, etc.)
	if(NOT ENABLE_SDL2)
		list(APPEND GFX_src_files
			GfxTextureGL.h
			GfxTextureGL.cpp)
	endif()
	# If GUI/Text is not built (SDL2 disabled), include GLTextRenderer here
	if(NOT ENABLE_SDL2)
		list(APPEND GFX_src_files
			../GUI/Text/GLTextRenderer.cpp)
	endif()
endif()
add_library(GFX ${GFX_src_files})

if(ENABLE_OPENGL)
target_link_libraries(GFX ${OPENGL_LIBRARIES} ${FREETYPE_LIBRARIES} glfw)
endif()
if(ENABLE_SDL2)
target_link_libraries(GFX ${SDL2_LIBRARIES} SDL2_ttf SDL2_image)
endif()

install(TARGETS GFX EXPORT gfxplusplusConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT gfxplusplusConfig DESTINATION share/gfxplusplus/cmake)

export(TARGETS GFX FILE gfxplusplusConfig.cmake)

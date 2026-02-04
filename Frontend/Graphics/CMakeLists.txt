# Backend-specific dependencies
if(ENABLE_OPENGL)
    set(OpenGL_GL_PREFERENCE "GLVND")
    find_package(OpenGL REQUIRED)

    # FreeType for OpenGL text rendering
    find_package(Freetype REQUIRED)

    # GLFW for OpenGL window/context and events (OpenGL backend)
    find_package(glfw3 3.3 QUIET)
    if(NOT glfw3_FOUND)
        find_package(GLFW3 3.3 REQUIRED)
    endif()
endif()

if(ENABLE_SDL2)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(SDL2_image REQUIRED)
endif()

set(GFX_src_files
	graphics.cpp
	graphics.h
	GfxRenderer.cpp
	GfxRenderer.h
)

if(ENABLE_SDL2)
	list(APPEND GFX_src_files
		GfxRendererSDL.h)
endif()

if(ENABLE_OPENGL)
	list(APPEND GFX_src_files
		GfxRendererGLFW.h
		GfxRendererGLFW.cpp)
	# If GUI/Text is not built (SDL2 disabled), include GLTextRenderer here
	if(NOT ENABLE_SDL2)
		list(APPEND GFX_src_files
			../GUI/Text/GLTextRenderer.cpp)
	endif()
endif()

add_library(GFX STATIC ${GFX_src_files})

target_include_directories(GFX PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Frontend/Graphics>
)

if(ENABLE_OPENGL)
    target_link_libraries(GFX PUBLIC OpenGL::GL Freetype::Freetype)
    if(TARGET glfw)
        target_link_libraries(GFX PUBLIC glfw)
    else()
        target_link_libraries(GFX PUBLIC glfw3)
    endif()
endif()
if(ENABLE_SDL2)
    target_link_libraries(GFX PUBLIC SDL2::SDL2 SDL2_ttf::SDL2_ttf SDL2_image::SDL2_image)
endif()

if(MSVC)
  set_property(TARGET GFX PROPERTY MSVC_RUNTIME_LIBRARY "${GFXPP_MSVC_RUNTIME}")
endif()

install(TARGETS GFX
    EXPORT gfxplusplusTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

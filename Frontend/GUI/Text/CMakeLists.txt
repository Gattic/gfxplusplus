find_package(Freetype REQUIRED)
include_directories(${FREETYPE_INCLUDE_DIRS})

set(TextC_src_files
	GFont.cpp
	GFont.h
	RUTextComponent.cpp
	RUTextComponent.h
	RUButton.cpp
	RUButton.h
	RULabel.cpp
	RULabel.h
	RUTextbox.cpp
	RUTextbox.h
)

# When both OpenGL and SDL2 are enabled, include GL text rendering in TextC
if(ENABLE_OPENGL AND ENABLE_SDL2)
	set(OpenGL_GL_PREFERENCE "GLVND")
	find_package(OpenGL REQUIRED)
	include_directories(${OPENGL_INCLUDE_DIRS})
	list(APPEND TextC_src_files
		GLTextRenderer.cpp
		GLTextRenderer.h)
endif()
add_library(TextC ${TextC_src_files})

# Ensure core graphics symbols (e.g., gfxDestroyTexture) are available to consumers
# when SDL2 is disabled and we use OpenGL-backed textures.
target_link_libraries(TextC PUBLIC GFX)

target_link_libraries(TextC PUBLIC ${FREETYPE_LIBRARIES})
if(ENABLE_SDL2)
if(WIN32)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2_ttf CONFIG REQUIRED)
    find_package(SDL2_image CONFIG REQUIRED)
    target_link_libraries(TextC PUBLIC SDL2::SDL2 SDL2_ttf::SDL2_ttf SDL2_image::SDL2_image)
else()
    include(FindPkgConfig)
    pkg_search_module(SDL2 REQUIRED sdl2 sdl2_ttf sdl2_image)
    include_directories(${SDL2_INCLUDE_DIRS})
    target_link_libraries(TextC PUBLIC ${SDL2_LIBRARIES} SDL2_ttf SDL2_image)
endif()
endif()
if(ENABLE_OPENGL AND ENABLE_SDL2)
	target_link_libraries(TextC PUBLIC ${OPENGL_LIBRARIES})
endif()

install(TARGETS TextC EXPORT gfxplusplusTargets
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT gfxplusplusTargets DESTINATION share/gfxplusplus/cmake)


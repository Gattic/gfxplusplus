# Add GUI Text subdir only when SDL2 is enabled (TextC depends on SDL2)
#opengl lib (optional)
if(ENABLE_OPENGL)
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
endif()

# SDL2 for GUI (optional when building GUI)
if(ENABLE_SDL2)
if(WIN32)
    find_package(SDL2 CONFIG REQUIRED)
    find_package(SDL2_ttf CONFIG REQUIRED)
    find_package(SDL2_image CONFIG REQUIRED)
else()
    include(FindPkgConfig)
    pkg_search_module(SDL2 REQUIRED sdl2 sdl2_ttf sdl2_image)
    include_directories(${SDL2_INCLUDE_DIRS})
endif()
endif()

set(GUI_src_files
	RUTabContainer.cpp
	RUTabContainer.h
	RUForm.cpp
	RUForm.h
	RUImageComponent.cpp
	RUImageComponent.h
	RUProgressBar.cpp
	RUProgressBar.h
	RUScrollbar.cpp
	RUScrollbar.h
	RUListbox.cpp
	RUListbox.h
	RUDropdown.cpp
	RUDropdown.h
	RUMsgBox.cpp
	RUMsgBox.h
	RUTable.cpp
	RUTable.h
	RUCheckbox.cpp
	RUCheckbox.h
)
add_library(GUI ${GUI_src_files})

# Always build and link TextC (our own static lib)
add_subdirectory("Text")
target_link_libraries(GUI TextC)
if(ENABLE_SDL2)
if(WIN32)
    target_link_libraries(GUI SDL2::SDL2 SDL2_ttf::SDL2_ttf SDL2_image::SDL2_image)
else()
    target_link_libraries(GUI ${SDL2_LIBRARIES} SDL2_ttf SDL2_image)
endif()
endif()
if(ENABLE_OPENGL)
    target_link_libraries(GUI ${OPENGL_LIBRARIES})
endif()

install(TARGETS GUI EXPORT gfxplusplusConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT gfxplusplusConfig DESTINATION share/gfxplusplus/cmake)

export(TARGETS GUI FILE gfxplusplusConfig.cmake)

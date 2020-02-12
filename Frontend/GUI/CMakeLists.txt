add_subdirectory("RUGraph")
add_subdirectory("Text")

#opengl lib
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

#sdl2 lib
include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2 sdl2_ttf sdl2_image)
include_directories(${SDL2_INCLUDE_DIR})

set(GUI_src_files
	RUTabContainer.cpp
	RUTabContainer.h
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

target_link_libraries(GUI GraphGUI TextC ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES} SDL2_ttf SDL2_image)

install(TARGETS GUI EXPORT gfxConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT gfxConfig DESTINATION share/glades/cmake)

export(TARGETS GUI FILE gfxConfig.cmake)

add_subdirectory("Mini")

#opengl lib
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

set(GFXItem_src_files
	RUColors.cpp
	RUColors.h
	GItem.cpp
	GItem.h
	RUComponent.cpp
	RUComponent.h
	GLayout.cpp
	GLayout.h
	GPanel.cpp
	GPanel.h
	RUItemArea.cpp
	RUItemArea.h
)
add_library(GFXItem ${GFXItem_src_files})

target_link_libraries(GFXItem MiniC)

install(TARGETS GFXItem EXPORT gfxConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT gfxConfig DESTINATION share/glades/cmake)

export(TARGETS GFXItem FILE gfxConfig.cmake)

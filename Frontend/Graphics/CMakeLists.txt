#opengl lib
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

set(GFX_src_files
	graphics.cpp
	graphics.h
	gfxpp3D.cpp
	gfxpp3D.h
	object.cpp
	object.h
)
add_library(GFX ${GFX_src_files})

target_link_libraries(GFX ${OPENGL_LIBRARIES} ${SDL2_LIBRARIES} SDL2_ttf SDL2_image)

install(TARGETS GFX EXPORT gfxplusplusConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT gfxplusplusConfig DESTINATION share/gfxplusplus/cmake)

export(TARGETS GFX FILE gfxplusplusConfig.cmake)

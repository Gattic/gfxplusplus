set(GFX_src_files
	graphics2D.cpp
	graphics3D.cpp
	graphics.cpp
	graphics.h
	object.cpp
	object.h
)
add_library(GFX ${GFX_src_files})

install(TARGETS GFX EXPORT gfxplusplusConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT gfxplusplusConfig DESTINATION share/gfxplusplus/cmake)

export(TARGETS GFX FILE gfxplusplusConfig.cmake)

#sdl2 lib (only when SDL2 is enabled)
if(ENABLE_SDL2)
include(FindPkgConfig)
pkg_search_module(SDL2 REQUIRED sdl2 sdl2_ttf sdl2_image)
include_directories(${SDL2_INCLUDE_DIR})
endif()

set(MiniC_src_files
	RUBorderComponent.cpp
	RUBorderComponent.h
	RUBackgroundComponent.cpp
	RUBackgroundComponent.h
	RUMouseDown.cpp
	RUMouseDown.h
	RUMouseUp.cpp
	RUMouseUp.h
	RUMouseMotion.cpp
	RUMouseMotion.h
	RUMouseWheel.cpp
	RUMouseWheel.h
	RULoseFocus.cpp
	RULoseFocus.h
	RUKeyDown.cpp
	RUKeyDown.h
	RUKeyUp.cpp
	RUKeyUp.h
)
add_library(MiniC ${MiniC_src_files})
target_link_libraries(MiniC PUBLIC GFX)
if(ENABLE_OPENGL)
	target_link_libraries(MiniC PUBLIC ${OPENGL_LIBRARIES})
endif()
if(ENABLE_SDL2)
	target_link_libraries(MiniC PUBLIC ${SDL2_LIBRARIES} SDL2_ttf SDL2_image)
endif()

install(TARGETS MiniC EXPORT gfxplusplusConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT gfxplusplusConfig DESTINATION share/gfxplusplus/cmake)

export(TARGETS MiniC FILE gfxplusplusConfig.cmake)

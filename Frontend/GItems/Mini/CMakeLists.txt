if(ENABLE_SDL2)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(SDL2_image REQUIRED)
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
add_library(MiniC STATIC ${MiniC_src_files})

if(ENABLE_OPENGL)
	target_link_libraries(MiniC PUBLIC OpenGL::GL)
endif()
if(ENABLE_SDL2)
	target_link_libraries(MiniC PUBLIC SDL2::SDL2 SDL2_ttf::SDL2_ttf SDL2_image::SDL2_image)
endif()

target_include_directories(MiniC PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Frontend/GItems/Mini>
)

if(MSVC)
  set_property(TARGET MiniC PROPERTY MSVC_RUNTIME_LIBRARY "${GFXPP_MSVC_RUNTIME}")
endif()

install(TARGETS MiniC
    EXPORT gfxplusplusTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

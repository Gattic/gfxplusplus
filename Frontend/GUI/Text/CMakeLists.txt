find_package(Freetype REQUIRED)

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

if(ENABLE_OPENGL AND ENABLE_SDL2)
	set(OpenGL_GL_PREFERENCE "GLVND")
	find_package(OpenGL REQUIRED)
	list(APPEND TextC_src_files
		GLTextRenderer.cpp
		GLTextRenderer.h)
endif()

add_library(TextC STATIC ${TextC_src_files})

target_link_libraries(TextC PUBLIC Freetype::Freetype)

if(ENABLE_SDL2)
	find_package(SDL2 REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(SDL2_image REQUIRED)
	target_link_libraries(TextC PUBLIC SDL2::SDL2 SDL2_ttf::SDL2_ttf SDL2_image::SDL2_image)
endif()

if(ENABLE_OPENGL AND ENABLE_SDL2)
	target_link_libraries(TextC PUBLIC OpenGL::GL)
endif()

target_include_directories(TextC PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Frontend/GUI/Text>
)

if(MSVC)
  set_property(TARGET TextC PROPERTY MSVC_RUNTIME_LIBRARY "${GFXPP_MSVC_RUNTIME}")
endif()

install(TARGETS TextC
    EXPORT gfxplusplusTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

add_subdirectory("Mini")

# Optional OpenGL
if(ENABLE_OPENGL)
    set(OpenGL_GL_PREFERENCE "GLVND")
    find_package(OpenGL REQUIRED)
endif()

set(GFXItem_src_files
	RUColors.cpp
	RUColors.h
	GeneralListener.h
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
add_library(GFXItem STATIC ${GFXItem_src_files})

target_link_libraries(GFXItem PUBLIC MiniC GU)
if(ENABLE_OPENGL)
    target_link_libraries(GFXItem PUBLIC OpenGL::GL)
endif()

target_include_directories(GFXItem PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Frontend/GItems>
)

if(MSVC)
  set_property(TARGET GFXItem PROPERTY MSVC_RUNTIME_LIBRARY "${GFXPP_MSVC_RUNTIME}")
endif()

install(TARGETS GFXItem
    EXPORT gfxplusplusTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

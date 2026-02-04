add_subdirectory("Mini")

# Optional OpenGL (used only when OpenGL backend is enabled)
if(ENABLE_OPENGL)
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
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
add_library(GFXItem ${GFXItem_src_files})

target_link_libraries(GFXItem PUBLIC GFX MiniC GU)
if(ENABLE_OPENGL)
    target_link_libraries(GFXItem PUBLIC ${OPENGL_LIBRARIES})
endif()

install(TARGETS GFXItem EXPORT gfxplusplusConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT gfxplusplusConfig DESTINATION share/gfxplusplus/cmake)

export(TARGETS GFXItem FILE gfxplusplusConfig.cmake)

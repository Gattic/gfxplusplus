# Optional OpenGL
if(ENABLE_OPENGL)
    set(OpenGL_GL_PREFERENCE "GLVND")
    find_package(OpenGL REQUIRED)
endif()

# SDL2 for GUI (optional)
if(ENABLE_SDL2)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(SDL2_image REQUIRED)
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
add_library(GUI STATIC ${GUI_src_files})

# Always build and link TextC
add_subdirectory("Text")
target_link_libraries(GUI PUBLIC TextC)

if(ENABLE_SDL2)
    target_link_libraries(GUI PUBLIC SDL2::SDL2 SDL2_ttf::SDL2_ttf SDL2_image::SDL2_image)
endif()
if(ENABLE_OPENGL)
    target_link_libraries(GUI PUBLIC OpenGL::GL)
endif()

target_include_directories(GUI PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Frontend/GUI>
)

if(MSVC)
  set_property(TARGET GUI PROPERTY MSVC_RUNTIME_LIBRARY "${GFXPP_MSVC_RUNTIME}")
endif()

install(TARGETS GUI
    EXPORT gfxplusplusTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

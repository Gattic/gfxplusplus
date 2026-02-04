# Needs OpenGL and SDL2
set(GFX_src_files
  graphics.cpp graphics.h
  gfxpp3D.cpp gfxpp3D.h
  object.cpp object.h
)
add_library(GFX ${GFX_src_files})
target_link_libraries(GFX PUBLIC shmea::shmea SDL2::SDL2 OpenGL::GL)

target_include_directories(GFX PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
install(TARGETS GFX EXPORT gfxppTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

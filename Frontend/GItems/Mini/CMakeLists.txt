# MiniC (input events/components)
set(MiniC_src_files
  RUBorderComponent.cpp RUBorderComponent.h
  RUBackgroundComponent.cpp RUBackgroundComponent.h
  RUMouseDown.cpp RUMouseDown.h
  RUMouseUp.cpp RUMouseUp.h
  RUMouseMotion.cpp RUMouseMotion.h
  RUMouseWheel.cpp RUMouseWheel.h
  RULoseFocus.cpp RULoseFocus.h
  RUKeyDown.cpp RUKeyDown.h
  RUKeyUp.cpp RUKeyUp.h
)
add_library(MiniC ${MiniC_src_files})
target_link_libraries(MiniC PUBLIC shmea::shmea SDL2::SDL2 OpenGL::GL)

target_include_directories(MiniC PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
install(TARGETS MiniC EXPORT gfxppTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

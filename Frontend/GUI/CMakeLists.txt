# First bring in Text (fonts/widgets)
add_subdirectory(Text) # defines TextC

set(GUI_src_files
  RUTabContainer.cpp RUTabContainer.h
  RUImageComponent.cpp RUImageComponent.h
  RUProgressBar.cpp RUProgressBar.h
  RUScrollbar.cpp RUScrollbar.h
  RUListbox.cpp RUListbox.h
  RUDropdown.cpp RUDropdown.h
  RUMsgBox.cpp RUMsgBox.h
  RUTable.cpp RUTable.h
  RUCheckbox.cpp RUCheckbox.h
)
add_library(GUI ${GUI_src_files})
target_link_libraries(GUI PUBLIC shmea::shmea TextC SDL2::SDL2 OpenGL::GL)

target_include_directories(GUI PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS GUI EXPORT gfxppTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

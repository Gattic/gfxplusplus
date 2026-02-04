add_subdirectory(Mini)

# GFXItem (widgets/containers)
set(GFXItem_src_files
  RUColors.cpp RUColors.h
  GeneralListener.h
  GItem.cpp GItem.h
  RUComponent.cpp RUComponent.h
  GLayout.cpp GLayout.h
  GPanel.cpp GPanel.h
  RUItemArea.cpp RUItemArea.h
)
add_library(GFXItem ${GFXItem_src_files})
target_link_libraries(GFXItem PUBLIC shmea::shmea MiniC SDL2::SDL2 OpenGL::GL)

target_include_directories(GFXItem PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
install(TARGETS GFXItem EXPORT gfxppTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
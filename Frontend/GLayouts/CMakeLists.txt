set(GLayout_src_files
  GLinearLayout.cpp GLinearLayout.h
  GRelativeLayout.cpp GRelativeLayout.h
)
add_library(GLayout ${GLayout_src_files})
target_link_libraries(GLayout PUBLIC SDL2::SDL2 shmea::shmea)

target_include_directories(GLayout PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS GLayout EXPORT gfxppTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

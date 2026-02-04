set(TextC_src_files
  GFont.cpp GFont.h
  RUTextComponent.cpp RUTextComponent.h
  RUButton.cpp RUButton.h
  RULabel.cpp RULabel.h
  RUTextbox.cpp RUTextbox.h
)
add_library(TextC ${TextC_src_files})
target_link_libraries(TextC PUBLIC shmea::shmea SDL2::SDL2)

target_include_directories(TextC PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
install(TARGETS TextC EXPORT gfxppTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
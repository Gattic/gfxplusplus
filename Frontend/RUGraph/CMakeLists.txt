set(GraphGUI_src_files
  GeneralGraphable.h
  LineGraphable.cpp
  CandleGraphable.cpp
  CircleGraphable.cpp
  EllipseGraphable.cpp
  ActionBubbleGraphable.cpp
  NeuralNetGraphable.cpp
  GraphableAttr.cpp GraphableAttr.h
  HLineGraphable.cpp
  Graphable.h
  RUGraph.cpp RUGraph.h
)
add_library(GraphGUI ${GraphGUI_src_files})
target_link_libraries(GraphGUI PUBLIC GU shmea::shmea SDL2::SDL2 OpenGL::GL)

target_include_directories(GraphGUI PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
install(TARGETS GraphGUI EXPORT gfxppTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

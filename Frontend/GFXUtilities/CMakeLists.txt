# Simple math/utility lib
set(GU_src_files
  quaternion.cpp quaternion.h
  vec3.cpp vec3.h
  point2.cpp point2.h
  scalar1.cpp scalar1.h
  Candle.cpp Candle.h
  ActionBubble.cpp ActionBubble.h
  Circle.cpp Circle.h
  Ellipse.cpp Ellipse.h
  EventTracker.cpp EventTracker.h
  Horizontal_Line.cpp Horizontal_Line.h
  DrawNeuron.cpp DrawNeuron.h
  DrawNeuralNet.cpp DrawNeuralNet.h
  RULayerInfo.cpp RULayerInfo.h
)
add_library(GU ${GU_src_files})
target_link_libraries(GU PUBLIC shmea::shmea SDL2::SDL2)

target_include_directories(GU PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

install(TARGETS GU EXPORT gfxppTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

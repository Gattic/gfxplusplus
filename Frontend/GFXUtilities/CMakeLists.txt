set(GU_src_files
	quaternion.cpp
	quaternion.h
	vec3.cpp
	vec3.h
	point2.cpp
	point2.h
	scalar1.cpp
	scalar1.h
	Candle.cpp
	Candle.h
	ActionBubble.cpp
	ActionBubble.h
	Circle.cpp
	Circle.h
	Ellipse.cpp
	Ellipse.h
	EventTracker.cpp
	EventTracker.h
	Horizontal_Line.cpp
	Horizontal_Line.h
	DrawNeuron.cpp
	DrawNeuron.h
	DrawNeuralNet.cpp
	DrawNeuralNet.h
	RULayerInfo.cpp
	RULayerInfo.h
)
add_library(GU STATIC ${GU_src_files})

target_include_directories(GU PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Frontend/GFXUtilities>
)

if(MSVC)
  set_property(TARGET GU PROPERTY MSVC_RUNTIME_LIBRARY "${GFXPP_MSVC_RUNTIME}")
endif()

install(TARGETS GU
    EXPORT gfxplusplusTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

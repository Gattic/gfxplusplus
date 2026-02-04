set(GraphGUI_src_files
	GeneralGraphable.h
	LineGraphable.cpp
	CandleGraphable.cpp
	CircleGraphable.cpp
	EllipseGraphable.cpp
	ActionBubbleGraphable.cpp
	NeuralNetGraphable.cpp
	GraphableAttr.cpp
	GraphableAttr.h
	HLineGraphable.cpp
	Graphable.h
	RUGraph.cpp
	RUGraph.h
)
add_library(GraphGUI STATIC ${GraphGUI_src_files})

target_include_directories(GraphGUI PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Frontend/RUGraph>
)

if(MSVC)
  set_property(TARGET GraphGUI PROPERTY MSVC_RUNTIME_LIBRARY "${GFXPP_MSVC_RUNTIME}")
endif()

install(TARGETS GraphGUI
    EXPORT gfxplusplusTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

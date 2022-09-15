set(GraphGUI_src_files
	GeneralGraphable.h
	LineGraphable.cpp
	CandleGraphable.cpp
	CircleGraphable.cpp
	ActionBubbleGraphable.cpp
	GraphableAttr.cpp
	GraphableAttr.h
	HLineGraphable.cpp
	GNodeGraphable.cpp
	Graphable.h
	RUGraph.cpp
	RUGraph.h
	#RUDrawComponent.cpp # And Reimplement Circle too
	#RUDrawComponent.h
)
add_library(GraphGUI ${GraphGUI_src_files})

install(TARGETS GraphGUI EXPORT gfxplusplusConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT gfxplusplusConfig DESTINATION share/gfxplusplus/cmake)

export(TARGETS GraphGUI FILE gfxplusplusConfig.cmake)

set(GraphGUI_src_files
	Graphable.cpp
	Graphable.h
	GraphLine.cpp
	GraphLine.h
	GraphCandlestick.cpp
	GraphCandlestick.h
	GraphScatter.cpp
	GraphScatter.h
	Circle.cpp
	Circle.h
	RUGraph.cpp
	RUGraph.h
	RUDrawComponent.cpp
	RUDrawComponent.h
	RULineGraph.cpp
	RULineGraph.h
	RUScatterGraph.cpp
	RUScatterGraph.h
	RUCandleGraph.cpp
	RUCandleGraph.h
)
add_library(GraphGUI ${GraphGUI_src_files})

install(TARGETS GraphGUI EXPORT gfxplusplusConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT gfxplusplusConfig DESTINATION share/gfxplusplus/cmake)

export(TARGETS GraphGUI FILE gfxplusplusConfig.cmake)

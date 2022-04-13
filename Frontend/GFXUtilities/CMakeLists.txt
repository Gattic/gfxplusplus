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
	SimpleLine.cpp
	SimpleLine.h
)
add_library(GU ${GU_src_files})

install(TARGETS GU EXPORT gfxplusplusConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT gfxplusplusConfig DESTINATION share/gfxplusplus/cmake)

export(TARGETS GU FILE gfxplusplusConfig.cmake)

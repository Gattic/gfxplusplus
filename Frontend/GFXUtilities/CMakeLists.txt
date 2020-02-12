set(GU_src_files
	quaternion.cpp
	quaternion.h
	vec3.cpp
	vec3.h
	point2.cpp
	point2.h
	EventTracker.cpp
	EventTracker.h
)
add_library(GU ${GU_src_files})

install(TARGETS GU EXPORT gfxConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT gfxConfig DESTINATION share/glades/cmake)

export(TARGETS GU FILE gfxConfig.cmake)

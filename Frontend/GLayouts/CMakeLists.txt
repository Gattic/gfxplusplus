set(GLayout_src_files
	GLinearLayout.cpp
	GLinearLayout.h
	GRelativeLayout.cpp
	GRelativeLayout.h
)
add_library(GLayout ${GLayout_src_files})

install(TARGETS GLayout EXPORT gfxConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT gfxConfig DESTINATION share/glades/cmake)

export(TARGETS GLayout FILE gfxConfig.cmake)

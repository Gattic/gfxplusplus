set(MiniC_src_files
	RUBorderComponent.cpp
	RUBorderComponent.h
	RUBackgroundComponent.cpp
	RUBackgroundComponent.h
	RUMouseDown.cpp
	RUMouseDown.h
	RUMouseUp.cpp
	RUMouseUp.h
	RUMouseMotion.cpp
	RUMouseMotion.h
	RUMouseWheel.cpp
	RUMouseWheel.h
	RULoseFocus.cpp
	RULoseFocus.h
	RUKeyDown.cpp
	RUKeyDown.h
	RUKeyUp.cpp
	RUKeyUp.h
)
add_library(MiniC ${MiniC_src_files})

install(TARGETS MiniC EXPORT gfxplusplusConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT gfxplusplusConfig DESTINATION share/gfxplusplus/cmake)

export(TARGETS MiniC FILE gfxplusplusConfig.cmake)

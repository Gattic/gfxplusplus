set(TextC_src_files
	GFont.cpp
	GFont.h
	RUTextComponent.cpp
	RUTextComponent.h
	RUButton.cpp
	RUButton.h
	RULabel.cpp
	RULabel.h
	RUTextbox.cpp
	RUTextbox.h
)
add_library(TextC ${TextC_src_files})

install(TARGETS TextC EXPORT gfxplusplusConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT gfxplusplusConfig DESTINATION share/gfxplusplus/cmake)

export(TARGETS TextC FILE gfxplusplusConfig.cmake)

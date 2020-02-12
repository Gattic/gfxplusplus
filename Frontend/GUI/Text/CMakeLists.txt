set(TextC_src_files
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

install(TARGETS TextC EXPORT gfxConfig
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT gfxConfig DESTINATION share/glades/cmake)

export(TARGETS TextC FILE gfxConfig.cmake)

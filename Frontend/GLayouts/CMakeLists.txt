set(GLayout_src_files
	GLinearLayout.cpp
	GLinearLayout.h
	GRelativeLayout.cpp
	GRelativeLayout.h
)
add_library(GLayout STATIC ${GLayout_src_files})

target_include_directories(GLayout PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/Frontend/GLayouts>
)

if(MSVC)
  set_property(TARGET GLayout PROPERTY MSVC_RUNTIME_LIBRARY "${GFXPP_MSVC_RUNTIME}")
endif()

install(TARGETS GLayout
    EXPORT gfxplusplusTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

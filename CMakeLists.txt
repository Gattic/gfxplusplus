cmake_minimum_required(VERSION 3.21)

project(gfxplusplus VERSION 0.58 LANGUAGES CXX)

include(GNUInstallDirs)

# --------------------
# Project-wide settings
# --------------------
set(CMAKE_CXX_STANDARD 11) # ShmeaDB uses 11
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

# Default build type for single-config generators
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()

# Warnings/flags
if(MSVC)
  add_compile_options(/W3 /EHsc)
else()
  add_compile_options(-Wall -Wextra -Wno-unused-variable -Wno-unused-parameter)
endif()

# MSVC static runtime per-target
set(GFXPP_MSVC_RUNTIME "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Default install prefix
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  if(WIN32)
    set(_default_prefix "$ENV{USERPROFILE}/.local")
  else()
    set(_default_prefix "$ENV{HOME}/.local")
  endif()
  if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${_default_prefix}" CACHE PATH "Install prefix" FORCE)
  endif()
endif()

# Backend build options
option(ENABLE_SDL2 "Build with SDL2 backend" OFF)
option(ENABLE_OPENGL "Build with OpenGL/GLFW backend" ON)

# Propagate compile definitions
if(ENABLE_SDL2)
  add_definitions(-DGFX_HAVE_SDL2)
endif()
if(ENABLE_OPENGL)
  add_definitions(-DGFX_HAVE_OPENGL)
endif()

# Import libs
find_package(shmea REQUIRED)

# --------------------
# Subdirectories
# --------------------
add_subdirectory(Frontend/GFXUtilities)
add_subdirectory(Frontend/Graphics)
add_subdirectory(Frontend/GItems)
add_subdirectory(Frontend/GUI)
add_subdirectory(Frontend/RUGraph)
add_subdirectory(Frontend/GLayouts)

set(MAIN_SRC_FILES
	main.cpp
	main.h
)

# Umbrella library target: gfxplusplus
add_library(${PROJECT_NAME} STATIC ${MAIN_SRC_FILES})

# Link all internal libs
target_link_libraries(${PROJECT_NAME} PUBLIC
	GFX
    GU
    GUI
    TextC
    GLayout
    GraphGUI
    GFXItem
    shmea::shmea
)

# Include dirs
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/services>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

# MSVC static runtime
if(MSVC)
  set_property(TARGET ${PROJECT_NAME} PROPERTY MSVC_RUNTIME_LIBRARY "${GFXPP_MSVC_RUNTIME}")
endif()

# --------------------
# Install headers
# --------------------
if(EXISTS "${PROJECT_SOURCE_DIR}/include")
  install(DIRECTORY "${PROJECT_SOURCE_DIR}/include/"
          DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
endif()

# --------------------
# Install targets
# --------------------
install(TARGETS ${PROJECT_NAME}
  EXPORT   gfxplusplusTargets
  ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR}
  INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# --------------------
# Package config + export
# --------------------
include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/gfxplusplusConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY SameMajorVersion
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/gfxplusplusConfig.cmake"
  "include(CMakeFindDependencyMacro)\n"
  "find_dependency(shmea)\n"
  "include(\"\${CMAKE_CURRENT_LIST_DIR}/gfxplusplusTargets.cmake\")\n"
)

install(EXPORT gfxplusplusTargets
  NAMESPACE gfxplusplus::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gfxplusplus
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/gfxplusplusConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/gfxplusplusConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gfxplusplus
)

# Optional: export from build tree
export(EXPORT gfxplusplusTargets
  NAMESPACE gfxplusplus::
  FILE "${CMAKE_CURRENT_BINARY_DIR}/gfxplusplusTargets.cmake"
)

# --------------------
# Dev helpers
# --------------------
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
  add_custom_target(fmt
    COMMAND ${CMAKE_COMMAND} -E echo "Run ./clang-format-all"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )

  add_custom_target(uninstall
    COMMAND ${CMAKE_COMMAND} -E echo "Uninstalling..."
    COMMAND xargs rm < ${CMAKE_BINARY_DIR}/install_manifest.txt
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  )
endif()

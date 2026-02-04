# gfxplusplus / top-level CMakeLists.txt (package of static libs + umbrella interface)

cmake_minimum_required(VERSION 3.21)
project(gfxplusplus VERSION 0.57 LANGUAGES CXX)

# ---------- Global: all static, MSVC static runtime ----------
set(BUILD_SHARED_LIBS OFF)
if(MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG TRUE)

# ---------- Default install prefix ----------
if(WIN32)
  set(_default_prefix "$ENV{USERPROFILE}/.local")
else()
  set(_default_prefix "$ENV{HOME}/.local")
endif()
if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "${_default_prefix}" CACHE PATH "Install prefix" FORCE)
endif()

# ---------- Tooling / flags ----------
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS Debug Release RelWithDebInfo MinSizeRel)
endif()
if(MSVC)
  add_compile_options(/W3 /EHsc)
else()
  add_compile_options(-Wall -Wextra -Wno-unused-variable -Wno-unused-parameter)
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
endif()

include(GNUInstallDirs)

# ---------- Make user-local shmea discoverable ----------
if(WIN32)
  set(_default_local "$ENV{USERPROFILE}/.local")
else()
  set(_default_local "$ENV{HOME}/.local")
endif()
if(EXISTS "${_default_local}/lib/cmake/shmea/shmeaConfig.cmake")
  list(APPEND CMAKE_PREFIX_PATH "${_default_local}")
endif()

# ---------- Dependencies ----------
find_package(shmea CONFIG REQUIRED)        # shmea::shmea
set(OpenGL_GL_PREFERENCE "GLVND")
find_package(OpenGL REQUIRED)

# On Windows, prefer vcpkg config packages for SDL2*
if(WIN32)
  find_package(SDL2 CONFIG REQUIRED)
  find_package(SDL2_ttf CONFIG REQUIRED)
  find_package(SDL2_image CONFIG REQUIRED)
  set(SDL2_LIBRARIES SDL2::SDL2 SDL2_ttf::SDL2_ttf SDL2_image::SDL2_image)
else()
  # (Optional) add your Linux discovery here (pkg-config) if you want cross-platform
endif()

# ---------- Subdirectories (each defines a STATIC lib and installs into gfxppTargets) ----------
add_subdirectory(Frontend/GFXUtilities)  # -> GU
add_subdirectory(Frontend/GItems)        # -> MiniC, GFXItem
add_subdirectory(Frontend/GLayouts)      # -> GLayout
add_subdirectory(Frontend/Graphics)      # -> GFX
add_subdirectory(Frontend/GUI)           # -> GUI, TextC (inside its CMakeLists)
add_subdirectory(Frontend/RUGraph)       # -> GraphGUI

# ---------- Umbrella INTERFACE target for consumers ----------
add_library(gfxplusplus INTERFACE)
target_link_libraries(gfxplusplus INTERFACE
  GUI GFX GLayout GraphGUI GFXItem GU MiniC
  OpenGL::GL
  ${SDL2_LIBRARIES}
  shmea::shmea
)

# ---------- Install umbrella target only here ----------
install(TARGETS gfxplusplus EXPORT gfxppTargets)

# ---------- Package config (single export set emitted only here) ----------
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/gfxplusplusConfigVersion.cmake"
  VERSION "${PROJECT_VERSION}"
  COMPATIBILITY SameMajorVersion
)
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/gfxplusplusConfig.cmake"
  "include(\"\${CMAKE_CURRENT_LIST_DIR}/gfxppTargets.cmake\")\n"
)
install(EXPORT gfxppTargets
  NAMESPACE gfxpp::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gfxplusplus
)
install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/gfxplusplusConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/gfxplusplusConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/gfxplusplus
)

# Format all
add_custom_target(fmt
    COMMAND ./clang-format-all
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Custom run modes
add_custom_target(fullscreen
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME} fullscreen
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

add_custom_target(nogui
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME} nogui
    DEPENDS ${PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

# Linux-specific targets
if(UNIX)
    add_custom_target(debug
        COMMAND gdb ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}
        DEPENDS ${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

    add_custom_target(mem
        COMMAND valgrind --leak-check=full --track-origins=yes ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}
        DEPENDS ${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )

    add_custom_target(profile
        COMMAND valgrind --tool=callgrind ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${PROJECT_NAME}
        DEPENDS ${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    )
endif()

# Uninstall
add_custom_target(uninstall
    COMMAND xargs rm < ${CMAKE_BINARY_DIR}/install_manifest.txt
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

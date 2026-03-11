if(NOT (${CMAKE_BINARY_DIR} STREQUAL "${CMAKE_SOURCE_DIR}/build"))
	message(FATAL_ERROR "Must be in \"build\" directory")
endif()

if(WIN32)
    set(CMAKE_INSTALL_PREFIX "$ENV{USERPROFILE}/gfxplusplus" CACHE PATH "default install path" FORCE)
else()
    set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local" CACHE PATH "default install path" FORCE)
endif()
file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/include)

#Init
cmake_minimum_required(VERSION 3.5.1)
cmake_policy(SET CMP0091 NEW)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

#Compiler Flags
set(CMAKE_BUILD_TYPE Release)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

#Project
project(gfxplusplus)
set(G_VERSION_MAJOR 0)
set(G_VERSION_MINOR 58)

# Compiler flags must come after project() so MSVC variable is available
if(MSVC)
    add_compile_options(/W3 /wd4101 /wd4100)
    add_compile_definitions(NOMINMAX _CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS_RELEASE "/O2")
elseif(WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g")
else()
    set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-variable -Wno-unused-parameter")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -g")
endif()

# Backend build options
option(ENABLE_SDL2 "Build with SDL2 backend" OFF)
option(ENABLE_OPENGL "Build with OpenGL/GLFW backend" ON)
option(DEV_MODE "Copy shmea headers into include/ from source tree for development" OFF)

# Propagate compile definitions for conditional compilation
if(ENABLE_SDL2)
  add_definitions(-DGFX_HAVE_SDL2)
endif()
if(ENABLE_OPENGL)
  add_definitions(-DGFX_HAVE_OPENGL)
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

#Import libs
find_package(shmea REQUIRED)
if(DEV_MODE)
    # Dev mode: copy shmea headers from source tree into include/
    if(NOT SHMEA_SOURCE_DIR)
        set(SHMEA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../ShmeaDB")
    endif()
    if(NOT EXISTS "${SHMEA_SOURCE_DIR}/Backend")
        message(FATAL_ERROR "DEV_MODE requires SHMEA_SOURCE_DIR (default: ../ShmeaDB). Not found at: ${SHMEA_SOURCE_DIR}")
    endif()
    message(STATUS "DEV_MODE: copying shmea headers from ${SHMEA_SOURCE_DIR} into include/")
    file(COPY "${SHMEA_SOURCE_DIR}/Backend/Core/" DESTINATION "${CMAKE_SOURCE_DIR}/include/Backend/Core" FILES_MATCHING PATTERN "*.h")
    file(COPY "${SHMEA_SOURCE_DIR}/Backend/Database/" DESTINATION "${CMAKE_SOURCE_DIR}/include/Backend/Database" FILES_MATCHING PATTERN "*.h")
    file(COPY "${SHMEA_SOURCE_DIR}/Backend/Networking/" DESTINATION "${CMAKE_SOURCE_DIR}/include/Backend/Networking" FILES_MATCHING PATTERN "*.h")
    file(COPY "${SHMEA_SOURCE_DIR}/Backend/Plotter/" DESTINATION "${CMAKE_SOURCE_DIR}/include/Backend/Plotter" FILES_MATCHING PATTERN "*.h")
    file(COPY "${SHMEA_SOURCE_DIR}/main.h" DESTINATION "${CMAKE_SOURCE_DIR}/include")
    file(COPY "${SHMEA_SOURCE_DIR}/g_export.h" DESTINATION "${CMAKE_SOURCE_DIR}/include")
    include_directories("include")
else()
    get_target_property(SHMEA_INCLUDE_DIRS shmea INTERFACE_INCLUDE_DIRECTORIES)
    include_directories(${SHMEA_INCLUDE_DIRS})
endif()

include(GNUInstallDirs)

#Subdirectories
include_directories(${CMAKE_SOURCE_DIR})
include_directories("services")
add_subdirectory("Frontend/GFXUtilities")
add_subdirectory("Frontend/Graphics")
add_subdirectory("Frontend/GItems")
add_subdirectory("Frontend/GUI")
add_subdirectory("Frontend/RUGraph")
add_subdirectory("Frontend/GLayouts")

set(MAIN_SRC_FILES
	main.cpp
	main.h
)

add_library(${PROJECT_NAME} SHARED ${MAIN_SRC_FILES})

#Link libraries
target_link_libraries(${PROJECT_NAME} PUBLIC
	GFX GU GUI TextC GLayout GraphGUI GFXItem shmea)

install(TARGETS ${PROJECT_NAME} EXPORT gfxplusplusTargets
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

# Install headers
install(DIRECTORY Frontend/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Frontend FILES_MATCHING PATTERN "*.h")
install(DIRECTORY services/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/services FILES_MATCHING PATTERN "*.h")
install(FILES main.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# ---- Package config / exports ----
set(gfxplusplus_INSTALL_CMAKEDIR "${CMAKE_INSTALL_DATADIR}/gfxplusplus/cmake")

export(EXPORT gfxplusplusTargets FILE "${CMAKE_CURRENT_BINARY_DIR}/gfxplusplusTargets.cmake")

# Config file that finds transitive dependencies before loading targets
file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/gfxplusplusConfig.cmake"
"# Auto-generated by gfxplusplus/CMakeLists.txt\n"
"\n"
"include(CMakeFindDependencyMacro)\n"
"find_dependency(shmea REQUIRED)\n"
"find_dependency(Freetype REQUIRED)\n"
"if(NOT TARGET glfw)\n"
"    find_dependency(glfw3 REQUIRED)\n"
"endif()\n"
"\n"
"include(\"\${CMAKE_CURRENT_LIST_DIR}/gfxplusplusTargets.cmake\")\n"
)

install(EXPORT gfxplusplusTargets
    FILE gfxplusplusTargets.cmake
    DESTINATION "${gfxplusplus_INSTALL_CMAKEDIR}")
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/gfxplusplusConfig.cmake"
    DESTINATION "${gfxplusplus_INSTALL_CMAKEDIR}")

if(NOT WIN32)

#make fmt
add_custom_target(fmt
	COMMAND ./clang-format-all
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

endif()

#make fullscreen
add_custom_target(fullscreen
	COMMAND ${PROJECT_NAME} fullscreen
	DEPENDS ${PROJECT_NAME}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

#make nogui
add_custom_target(nogui
	COMMAND ${PROJECT_NAME} nogui
	DEPENDS ${PROJECT_NAME}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

if(NOT WIN32)

#make debug
add_custom_target(debug
	COMMAND gdb ./build/${PROJECT_NAME}
	DEPENDS ${PROJECT_NAME}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

#make mem
	add_custom_target(mem
	COMMAND valgrind --leak-check=full --track-origins=yes ./build/${PROJECT_NAME}
	DEPENDS ${PROJECT_NAME}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

#make profile
	add_custom_target(profile
	COMMAND valgrind --tool=callgrind ./build/${PROJECT_NAME}
	DEPENDS ${PROJECT_NAME}
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

#make uninstall
	add_custom_target(uninstall
	COMMAND xargs rm < ./build/install_manifest.txt
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

endif()

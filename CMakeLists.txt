if(NOT (${CMAKE_BINARY_DIR} STREQUAL "${CMAKE_SOURCE_DIR}/build"))
	message(FATAL_ERROR "Must be in \"build\" directory")
endif()

set (CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local" CACHE PATH "default install path" FORCE)
file(MAKE_DIRECTORY ${CMAKE_INSTALL_PREFIX}/include)

#Init
cmake_minimum_required(VERSION 3.5.1)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

#Compiler Flags
#set(DEFAULT_BUILD_TYPE "Release")
set(CMAKE_BUILD_TYPE Debug) # -g

if(WIN32)
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
else()
	#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC") # its set below
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-variable -Wno-unused-parameter")
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#Project
project(gfxplusplus)
set(G_VERSION_MAJOR 0)
set(G_VERSION_MINOR 57)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

#Import libs
find_package(shmea REQUIRED)

include(GNUInstallDirs)

#Subdirectories
include_directories(${CMAKE_SOURCE_DIR})
include_directories("include")
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
	GUI GFX GLayout GraphGUI GFXItem GU GNet DB)#custom libs

#target_include_directories(gfxplusplus PUBLIC "include")
#target_include_directories(gfxplusplus PUBLIC "Frontend")
target_include_directories(gfxplusplus PUBLIC  
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>  
    $<INSTALL_INTERFACE:include>  # <prefix>/include
)

#target_include_directories(${PROJECT_NAME} PUBLIC
#    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
#    $<INSTALL_INTERFACE:include>
#    PRIVATE Frontend)

install(TARGETS ${PROJECT_NAME} EXPORT gfxplusplusConfig
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    ARCHIVE  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY  DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME  DESTINATION ${CMAKE_INSTALL_BINDIR})

install(EXPORT gfxplusplusConfig DESTINATION share/gfxplusplus/cmake)

export(TARGETS ${PROJECT_NAME} FILE gfxplusplusConfig.cmake)

#make fmt
add_custom_target(fmt
	COMMAND ./clang-format-all
	WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

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

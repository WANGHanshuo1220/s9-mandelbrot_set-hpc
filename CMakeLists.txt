cmake_minimum_required(VERSION 3.2)
project(fractal)

# Specify bin and lib paths
set (EXECUTABLE_OUTPUT_PATH bin/)

# Enable C++11
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "AppleClang")
	set (CMAKE_CXX_STANDARD 14)
else ()
	set (CMAKE_CXX_STANDARD 11)
endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

option (ENABLE_SDL "Enable SDL display"                          ON )
option (USE_FLOAT  "Use simple precision floating-point numbers" OFF)

if (USE_FLOAT)
	add_definitions(-DUSE_FLOAT)
endif (USE_FLOAT)

if (ENABLE_SDL)
	find_package(SDL2)
	if (SDL2_FOUND)
		add_definitions("-DUSE_SDL")
		message(STATUS "SDL found")
		include_directories(${SDL2_INCLUDE_DIR})
	else(SDL2_FOUND)
		message(WARNING "SDL can't be found, the binaries will be compiled without SDL.")
	endif(SDL2_FOUND)
endif (ENABLE_SDL)

file( GLOB IMPL_SRC src/implem/*.cpp)
foreach( impl_src_file ${IMPL_SRC} )
	string(REPLACE ".cpp" "" implname ${impl_src_file})
	string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/src/implem/" "" implname ${implname})
	string(REPLACE "fractal_" "" implem ${implname})

	set(SOURCE_FILES src/main.cpp src/implem/${implname}.cpp)

	add_executable(${implname} ${SOURCE_FILES})

	# link SDL
	if (ENABLE_SDL)
		if (SDL2_FOUND)
			target_link_libraries(${implname} ${SDL2_LIBRARY})
			target_link_libraries(${implname} m)
		endif (SDL2_FOUND)
	endif (ENABLE_SDL)
endforeach( impl_src_file ${IMPL_SRC} )

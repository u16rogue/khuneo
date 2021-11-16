cmake_minimum_required(VERSION 3.18)

project(khuneo)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Libraries
add_subdirectory(khuneo-lang)

# Projects
option(KHUNEO_BUILD_RUNTIME "Build the khuneo runtime executable." OFF)
if (KHUNEO_BUILD_RUNTIME)
	add_subdirectory(khuneo-runtime)
endif()
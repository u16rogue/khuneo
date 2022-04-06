cmake_minimum_required(VERSION 3.18)

project(
	khuneo
	VERSION 0.1.0
	DESCRIPTION "A modular and abstracted embedded programming language for C++"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

option(KH_LOCAL_TEST "Local testing executable" OFF)
option(KH_TESTS "Compiles the test executable that verifies that khuneo is working." OFF)


# "${CMAKE_CURRENT_SOURCE_DIR}/include/khuneo/.hpp"
# "${CMAKE_CURRENT_SOURCE_DIR}/src/.cpp"

add_library(
	${PROJECT_NAME}

	"${CMAKE_CURRENT_SOURCE_DIR}/include/khuneo/khuneo.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/khuneo.cpp"

	"${CMAKE_CURRENT_SOURCE_DIR}/include/khuneo/parser.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/parser.cpp"

	"${CMAKE_CURRENT_SOURCE_DIR}/include/khuneo/string_literals.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/string_literals.cpp"
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)

if (KH_LOCAL_TEST)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/localtest")
endif()

if (KH_TESTS)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif()
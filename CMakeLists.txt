cmake_minimum_required(VERSION 3.18)

project(
	khuneo
	VERSION 0.1.0
	DESCRIPTION "A modular and abstracted embedded programming language for C++"
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

option(KH_LOCAL_TEST "Local testing executable" OFF)
option(KH_TESTS "Compiles the test executable that verifies that khuneo is working." OFF) # TODO: setup test to only run test on included components
option(KH_INCLUDE_COMPILER "Includes the compiler component to khuneo." ON)

add_subdirectory("kh_core")
add_subdirectory("kh_runtime")

add_library(
	${PROJECT_NAME}

	"${CMAKE_CURRENT_SOURCE_DIR}/kh/include/khuneo/khuneo.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/kh/src/khuneo.cpp"
)

target_link_libraries(
	${PROJECT_NAME}
	PUBLIC
	KH_CORE
	KH_RUNTIME
)

if (KH_INCLUDE_COMPILER)
	add_subdirectory("kh_compiler")
	target_link_libraries(
		${PROJECT_NAME}
		PUBLIC
		KH_COMPILER
	)
endif()

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/kh/include"
)

if (KH_LOCAL_TEST)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/localtest")
endif()

if (KH_TESTS)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/test")
endif()
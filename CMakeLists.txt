cmake_minimum_required(VERSION 3.18)

project(
	khuneo
	VERSION 0.1.0
	DESCRIPTION "A modular and abstracted embedded programming language for C++"
)

add_library(
	${PROJECT_NAME}

	"${CMAKE_CURRENT_SOURCE_DIR}/include/khuneo/khuneo.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/khuneo.cpp"
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)
cmake_minimum_required(VERSION 3.18)

if (NOT TARGET KH_CORE)
	message(FATAL_ERROR "KH_COMPILER requires the KH_CORE component to be an available target.")
endif()

project(
	KH_COMPILER
	VERSION 1.0.0
	DESCRIPTION "Compiler component of khuneo"
)

add_library(
	${PROJECT_NAME}

	"${CMAKE_CURRENT_SOURCE_DIR}/include/khuneo/compiler/parser.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/parser.cpp"

	"${CMAKE_CURRENT_SOURCE_DIR}/include/khuneo/compiler/lexer.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/lexer.cpp"

	"${CMAKE_CURRENT_SOURCE_DIR}/include/khuneo/compiler/ast.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/ast.cpp"
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	KH_CORE
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)
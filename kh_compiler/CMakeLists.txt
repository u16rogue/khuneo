cmake_minimum_required(VERSION 3.18)

if (NOT TARGET KH_CORE OR NOT TARGET KH_RUNTIME)
	message(FATAL_ERROR "KH_COMPILER requires the KH_CORE and KH_RUNTIME component to be an available target.")
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
 
	"${CMAKE_CURRENT_SOURCE_DIR}/include/khuneo/compiler/info.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/info.cpp"

	"${CMAKE_CURRENT_SOURCE_DIR}/include/khuneo/compiler/bc_compiler.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/bc_compiler.cpp"

	 "${CMAKE_CURRENT_SOURCE_DIR}/include/khuneo/compiler/lang_impl.hpp"
	 "${CMAKE_CURRENT_SOURCE_DIR}/src/lang_impl.cpp"
)

target_link_libraries(
	${PROJECT_NAME}
	PRIVATE
	KH_CORE
	KH_RUNTIME
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)
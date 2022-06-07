cmake_minimum_required(VERSION 3.18)

project(
	KH_CORE
	VERSION 1.0.0
	DESCRIPTION "Core component of khuneo shared across modules"
)

add_library(
	${PROJECT_NAME}

	"${CMAKE_CURRENT_SOURCE_DIR}/include/khuneo/string_literals.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/string_literals.cpp"

	"${CMAKE_CURRENT_SOURCE_DIR}/include/khuneo/defs.hpp"

	"${CMAKE_CURRENT_SOURCE_DIR}/include/khuneo/filo_stack.hpp"

	"${CMAKE_CURRENT_SOURCE_DIR}/include/khuneo/xxhash.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/xxhash.cpp" 
	
	"${CMAKE_CURRENT_SOURCE_DIR}/include/khuneo/kh_hashmap.hpp"
)

target_include_directories(
	${PROJECT_NAME}
	PUBLIC
	"${CMAKE_CURRENT_SOURCE_DIR}/include"
)
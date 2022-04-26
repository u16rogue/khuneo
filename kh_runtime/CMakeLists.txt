cmake_minimum_required(VERSION 3.18)

if (NOT TARGET KH_CORE)
	message(FATAL_ERROR "KH_RUNTIME requires the KH_CORE component to be an available target.")
endif()

project(
	KH_RUNTIME
)

add_library(
	${PROJECT_NAME}	

	"${CMAKE_CURRENT_SOURCE_DIR}/include/khuneo/runtime/vm.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/vm.cpp"
	
	"${CMAKE_CURRENT_SOURCE_DIR}/include/khuneo/runtime/kh_module.hpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/src/kh_module.cpp"
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
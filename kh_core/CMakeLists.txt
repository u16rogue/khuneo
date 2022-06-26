cmake_minimum_required(VERSION 3.18)

project(
	khuneo_core
	DESCRIPTION "khuneo's core component"
)

add_library(
	${PROJECT_NAME}
	INTERFACE # STATIC
)

target_include_directories(
	${PROJECT_NAME}
	INTERFACE
	"include/"
)

set_target_properties(
	${PROJECT_NAME}
	PROPERTIES
	CXX_STANDARD 20
	LINKER_LANGUAGE	CXX
)
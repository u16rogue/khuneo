cmake_minimum_required(VERSION 3.18)

project(
	khuneo_extra
	DESCRIPTION "Extra library for khuneo providing trivial utility functions"
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

if (TARGET khuneo_compiler)
	message("khuneo_extra >> Extra library found khuneo_compiler as a target!")
	target_link_libraries(
		${PROJECT_NAME}
		INTERFACE
		khuneo_compiler
	)
else()
	# message(AUTHOR_WARNING "khuneo_extra >> Extra library did not find khuneo_compiler as a target! Ignoring.")
endif()

set_target_properties(
	${PROJECT_NAME}
	PROPERTIES
	CXX_STANDARD 20
	LINKER_LANGUAGE	CXX
)
cmake_minimum_required(VERSION 3.18)

if (NOT TARGET khuneo_core)
    message(FATAL_ERROR "khuneo_core must be an availble target for khuneo_compiler")
endif()

project(
    khuneo_compiler
    DESCRIPTION "Optional compiler component for compiling khuneo source code"
)

add_library(
    ${PROJECT_NAME}
    INTERFACE
)

target_include_directories(
    ${PROJECT_NAME}
    INTERFACE
    "include/"
)

target_link_libraries(
	${PROJECT_NAME}
    INTERFACE
    khuneo_core
)

set_target_properties(
    ${PROJECT_NAME}
    PROPERTIES
    CXX_STANDARD 20
)

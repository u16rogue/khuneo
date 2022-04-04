cmake_minimum_required(VERSION 3.18)

if (NOT TARGET khuneo)
	message(FATAL_ERROR "The test requires khuneo to be an available target.")
endif()

project(
	kh_test
	DESCRIPTION "Test run for khuneo"
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME} "main.cpp")

target_link_libraries(${PROJECT_NAME} PRIVATE khuneo)
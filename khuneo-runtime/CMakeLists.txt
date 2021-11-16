cmake_minimum_required(VERSION 3.18)

project(khuneo-runtime)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(khuneo-runtime
	"khuneo_runtime.hpp"
	"khuneo_runtime.cpp"
)

target_link_libraries(khuneo-runtime PRIVATE khuneo-lang)
target_include_directories(khuneo-runtime PRIVATE ${khuneo-lang-includes})
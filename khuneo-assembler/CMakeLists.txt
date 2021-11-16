cmake_minimum_required(VERSION 3.18)

project(khuneo-assembler)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(khuneo-assembler
    "khuneo_assembler.cpp"
    "khuneo_assembler.hpp"
)

target_link_libraries(khuneo-assembler PRIVATE khuneo-lang)
target_include_directories(khuneo-assembler PRIVATE ${khuneo-lang-includes})

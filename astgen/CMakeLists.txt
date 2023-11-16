cmake_minimum_required(VERSION 3.18)

project("kh-astgen")

add_library(${PROJECT_NAME} STATIC
  "include/kh-astgen/lexer.h"
  "src/lexer.c"
)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")

#set_target_properties(${PROJECT_NAME} PROPERTIES)

target_compile_options(${PROJECT_NAME}
  PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>: -Wall -Wextra -Wpedantic -Werror -Wsign-conversion>
    $<$<CXX_COMPILER_ID:MSVC>: /W4 /WX>
)

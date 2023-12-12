cmake_minimum_required(VERSION 3.18)

project("kh")

add_subdirectory("core/")
add_subdirectory("astgen/")

add_library(${PROJECT_NAME} STATIC
  "include/kh/kh.h"
  "src/kh.c"
)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")

set_target_properties(${PROJECT_NAME} PROPERTIES
  C_STANDARD 17
)

target_compile_options(${PROJECT_NAME}
  PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>: -Wall -Wextra -Wpedantic -Werror -Wsign-conversion>
    $<$<CXX_COMPILER_ID:MSVC>: /W4 /WX>
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC kh-core kh-astgen
)

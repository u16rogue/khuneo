cmake_minimum_required(VERSION 3.18)

message("-- kh > gen > Loading")
project(kh-gen)

add_library(${PROJECT_NAME} STATIC
  "include/kh-gen/lexer.h"
  "src/lexer.c"
)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")

set_target_properties(${PROJECT_NAME} PROPERTIES
  C_STANDARD 17
)

target_link_libraries(${PROJECT_NAME}
  PUBLIC kh-core
)

kh_target_pedantic(${PROJECT_NAME})

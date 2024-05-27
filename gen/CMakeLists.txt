cmake_minimum_required(VERSION 3.18)

message("-- kh > gen > Loading")
set(_NAME kh-gen)

add_library(${_NAME} STATIC
  "include/kh-gen/lexer.h"
  "src/lexer.c"

  "include/kh-gen/parser.h"
  "src/parser.c"
)

target_include_directories(${_NAME} PUBLIC "include/")

set_target_properties(${_NAME} PROPERTIES
  C_STANDARD 17
)

target_link_libraries(${_NAME}
  PUBLIC kh-core
)

kh_target_pedantic(${_NAME})

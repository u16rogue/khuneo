cmake_minimum_required(VERSION 3.18)

if (NOT TARGET kh_core)
  message(FATAL_ERROR "-- khuneo > astgen > Missing Core target!")
endif()

project(kh_astgen)

add_library(
  ${PROJECT_NAME}
  STATIC
  "include/kh-astgen/lexer.h"
  "src/lexer.c"

  "src/lexers.h"
  "src/lexers.c"
)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
  C_STANDARD 17
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  "include/"
)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
  kh_core
)

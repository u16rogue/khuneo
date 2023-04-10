cmake_minimum_required(VERSION 3.18)

project(kh_core)

add_library(
  ${PROJECT_NAME}
  STATIC
  "kh-core/types.h"
  "kh-core/types.c"

  "kh-core/utf8.h"
  "kh-core/utf8.c"
)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
  C_STANDARD 17
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}"
)

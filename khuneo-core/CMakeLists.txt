cmake_minimum_required(VERSION 3.18)

project(kh_core)

add_library(
  ${PROJECT_NAME}
  STATIC

  "include/kh-core/utilities.h"

  "include/kh-core/types.h"
  "src/types.c"

  "include/kh-core/utf8.h"
  "src/utf8.c"

  "include/kh-core/refobj.h"
  "src/refobj.c"

  "include/kh-core/identifier_encoding.h"
  "src/identifier_encoding.c"
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

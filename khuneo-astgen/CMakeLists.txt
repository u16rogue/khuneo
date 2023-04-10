cmake_minimum_required(VERSION 3.18)

if (NOT TARGET kh_core)
  message(FATAL_ERROR "-- khuneo > astgen > kh_astgen requires kh_core as an available target. Check your build to load khuneo-core first.")
endif()

project(kh_astgen)

add_library(
  ${PROJECT_NAME}
  STATIC
  "kh-astgen/lexer.h"
  "kh-astgen/lexer.c"
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

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC
  kh_core
)

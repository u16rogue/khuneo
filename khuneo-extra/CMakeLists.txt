cmake_minimum_required(VERSION 3.18)

if (NOT TARGET kh_core)
  message(FATAL_ERROR "-- khuneo > extra > Target dependency `kh_core` not found. Target must be available in CMake before including extra")
endif()

project(kh_extra)

add_library(
  ${PROJECT_NAME}
  STATIC
)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")
target_link_libraries(${PROJECT_NAME} PRIVATE kh_core)

if (TARGET kh_astgen)
  message("-- khuneo > extra > astgen > Enabled!")
  target_link_libraries(${PROJECT_NAME} PRIVATE kh_astgen)
  target_sources(
    ${PROJECT_NAME}
    PRIVATE
    "include/kh-extra/astgen/lexer.h"
    "src/lexer.c"
  )
  target_compile_definitions(${PROJECT_NAME} PUBLIC KH_EXTRA_ASTGEN=1)
else()
  message("-- khuneo > extra > astgen > Disabled!")
  target_compile_definitions(${PROJECT_NAME} PUBLIC KH_EXTRA_ASTGEN=0)
endif()

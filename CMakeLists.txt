cmake_minimum_required(VERSION 3.18)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

option(KH_LOCAL_TEST       "Local test project"          OFF)
option(KH_INCLUDE_COMPILER "Include the khuneo compiler" ON)
option(KH_INCLUDE_EXTRA    "Include extra libraries for khuneo, these libraries are purely optional and are for trivial things" OFF)

add_subdirectory("kh-core/")

project(
  khuneo
  DESCRIPTION "An embeddable programming language"
  LANGUAGES CXX C
)

add_library(
  ${PROJECT_NAME}
  STATIC
  "kh/include/khuneo/khuneo.hpp"
  "kh/src/khuneo.cpp"
)

target_include_directories(
  ${PROJECT_NAME}
  PUBLIC
  "kh/include/"
)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC
  khuneo_core
)

if (KH_INCLUDE_COMPILER)
  add_subdirectory("kh-compiler/")
  target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    khuneo_compiler
  )
endif()

if (KH_INCLUDE_EXTRA)
  add_subdirectory("kh-extra/")
  target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    khuneo_extra
  )
endif()

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
  LINKER_LANGUAGE CXX
  CXX_STANDARD 20
)

if (KH_LOCAL_TEST)
    add_subdirectory("localtest/")
endif()

cmake_minimum_required(VERSION 3.18)

project(kh_test)

add_executable(
  ${PROJECT_NAME}
  "helper.h"
  "main.c"
)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
  OUTPUT_NAME "test"
)

target_link_libraries(
  ${PROJECT_NAME}
  PRIVATE
  kh_core
)

if (TARGET kh_core)
  message("-- khuneo > test > core > Enabled!")
  target_sources(
    ${PROJECT_NAME}
    PRIVATE
    "core/core.h"
    "core/core.c"
  )

  target_compile_definitions(${PROJECT_NAME} PRIVATE KH_TEST_CORE=1)
else()
  target_compile_definitions(${PROJECT_NAME} PRIVATE KH_TEST_CORE=0)
  message("-- khuneo > test > core > Disabled!")
endif()

if (TARGET kh_astgen)
  message("-- khuneo > test > astgen > Enabled!")
  target_sources(
    ${PROJECT_NAME}
    PRIVATE
    "astgen/astgen.h"
    "astgen/astgen.c"
  )

  target_compile_definitions(${PROJECT_NAME} PRIVATE KH_TEST_ASTGEN=1)

  target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    kh_astgen
  )
else()
  target_compile_definitions(${PROJECT_NAME} PRIVATE KH_TEST_ASTGEN=0)
  message("-- khuneo > test > astgen > Disabled!")
endif()

cmake_minimum_required(VERSION 3.18)

if (NOT TARGET kh_extra)
  message(FATAL_ERROR "kh_extra component is required by kh_test.")
endif()

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

target_link_libraries(${PROJECT_NAME} PRIVATE kh_extra)

if (TARGET kh_core)
  message("-- khuneo > test > core > Enabled!")
  target_sources(
    ${PROJECT_NAME}
    PRIVATE
    "core/core.h"
    "core/core.c"
  )
  target_link_libraries(${PROJECT_NAME} PRIVATE kh_core)
  target_compile_definitions(${PROJECT_NAME} PRIVATE KH_TEST_CORE=1)
else()
  target_compile_definitions(${PROJECT_NAME} PRIVATE KH_TEST_CORE=0)
  message("-- khuneo > test > core > Disabled!")
  message(FATAL_ERROR "-- khuneo > test > core > Core component cannot be disabled!")
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

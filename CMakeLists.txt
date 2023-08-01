cmake_minimum_required(VERSION 3.18)

project(khuneo)

# --- Project Configuration ---
option(KH_BUILD_TEST     "Build the unit test project of khuneo"   ON)
option(KH_BUILD_RUN_TEST "Automatically run the unit test"         ON)
option(KH_COMP_ASTGEN    "Enable khuneo's AST Generator component" ON)

add_library(
  ${PROJECT_NAME}
  STATIC
  "include/kh/khuneo.h"
  "src/khuneo.c"
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

# --- Core
add_subdirectory("khuneo-core")
target_link_libraries(${PROJECT_NAME} PUBLIC kh_core)

# --- AST Generator
if (KH_COMP_ASTGEN)
  add_subdirectory("khuneo-astgen")
  target_link_libraries(${PROJECT_NAME} PUBLIC kh_astgen)
  message("-- khuneo > astgen > Component enabled.")
endif()

# --- Extra
#option(KH_COMP_EXTRA "Enable khuneo's extra component which provides non critical utility functions. (aka bloat)" ON)
#if (KH_COMP_EXTRA)
#  add_subdirectory("khuneo-extra/")
#  target_link_libraries(${PROJECT_NAME} INTERFACE kh_extra)
#  message("-- khuneo > extra > Component enabled.")
#endif()

# --- TESTS ---
if (KH_BUILD_TEST)
  message("-- khuneo > test > Enabled!")
  add_subdirectory("test")
else()
  message("-- khuneo > test > Disabled!")
endif()

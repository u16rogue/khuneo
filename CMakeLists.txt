cmake_minimum_required(VERSION 3.18)

option(KH_TRACK_LINE_COLUMN "Tracks the line and column of a khuneo source file." OFF)
set(KH_TAB_SPACE_COUNT "2" CACHE STRING "The number of spaces equivalent to a single tab character. (Only used if KH_TRACK_LINE_COLUMN is enabled)")

if (KH_TRACK_LINE_COLUMN)
  message("-- khuneo > Consider adding a \"add_compile_definitions(KH_TRACK_LINE_COLUMN=1)\" to your project to properly propagate the macro.")
  message("-- khuneo > Line and column tracking for source file is enabled with ${KH_TAB_SPACE_COUNT} spaces to tab character")
  add_compile_definitions(
    KH_TRACK_LINE_COLUMN=1
    KH_TAB_SPACE_COUNT=${KH_TAB_SPACE_COUNT}
  )
else()
  message("-- khuneo > Line and column will not be tracked for parsing source files.")
endif()

# -- Components
# Always required
add_subdirectory("khuneo-core")

# -- Head project
project(khuneo)

add_library(
  ${PROJECT_NAME}
  STATIC
  "dummy.c"
)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES
  C_STANDARD 17
)

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC
  kh_core
)

# -- AST Generation component
option(KH_ENPR_ASTGEN "Enable project component \"AST Generation\" for parsing khuneo source." ON)
if (KH_ENPR_ASTGEN)
  message("-- khuneo > astgen > Component is enabled!")
  add_subdirectory("khuneo-astgen")
  target_link_libraries(${PROJECT_NAME} PUBLIC kh_astgen)
else()
  message("-- khuneo > astgen > Component is disabled!")
endif()

# -- Extra utilities component
# option(KH_ENPR_ASTGEN "Enable project component \"AST Generation\" for parsing khuneo source." ON)
# if (KH_ENPR_ASTGEN)
#   message("-- khuneo > astgen > Component is enabled!")
#   add_subdirectory("khuneo-astgen")
#   target_link_libraries(${PROJECT_NAME} PUBLIC kh_astgen)
# else()
#   message("-- khuneo > astgen > Component is disabled!")
# endif()

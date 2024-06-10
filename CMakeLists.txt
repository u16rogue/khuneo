cmake_minimum_required(VERSION 3.18)

option(KH_COMP_MAIN  "Enable the main implementation" ON )
option(KH_COMP_GEN   "Enable 'gen' component"         ON )
option(KH_COMP_EXTRA "Enable 'extra' component"       ON )

#-------------------------------------------------------------------------------

project("kh")
add_library(${PROJECT_NAME} STATIC)

set_target_properties(${PROJECT_NAME} PROPERTIES
  C_STANDARD 17
)

target_compile_options(${PROJECT_NAME}
  PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
      -Wall -Wextra -Wpedantic -Werror -Wsign-conversion>
    $<$<CXX_COMPILER_ID:MSVC>:
      /W4 /WX>
)

#-------------------------------------------------------------------------------

add_subdirectory("core/")

if (KH_COMP_GEN)
  add_subdirectory("gen/")
endif()

if (KH_COMP_MAIN)
  target_sources(${PROJECT_NAME} PUBLIC
    "include/kh/kh.h"
    "src/kh.c"
  )
  target_include_directories(${PROJECT_NAME} PUBLIC "include/")
endif()

# Load last
if (KH_COMP_EXTRA)
  add_subdirectory("extra/") 
endif()

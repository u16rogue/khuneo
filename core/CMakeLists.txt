cmake_minimum_required(VERSION 3.18)

option(KH_USERDEF_MEM "Determines if the library should be aware of and use the user defined memory manager. See: `kh-core/impldec.h` for declaring your own." OFF)

project("kh-core")

add_library(${PROJECT_NAME} STATIC
  "include/kh-core/defs.h"
  "include/kh-core/mem.h"
  "src/mem.c"
  "include/kh-core/str.h"
  "src/str.c"
)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")

set_target_properties(${PROJECT_NAME} PROPERTIES
  C_STANDARD 17
)

target_compile_definitions(${PROJECT_NAME}
  PUBLIC $<$<BOOL:${KH_USERDEF_MEM}>:KH_USERDEF_MEM=1>
)

target_compile_options(${PROJECT_NAME}
  PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>: -Wall -Wextra -Wpedantic -Werror -Wsign-conversion>
    $<$<CXX_COMPILER_ID:MSVC>: /W4 /WX>
)

if (KH_USERDEF_MEM)
  message("-- kh > core > Default memory management definitions are off which requires you to implement your own! Check 'KH_USERDEF_MEM'.")
else()
  message("-- kh > core > Default memory management definitions are on this will cause the library to use the standard library! Check 'KH_USERDEF_MEM'.")
endif()

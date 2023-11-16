cmake_minimum_required(VERSION 3.18)

option(KH_USE_STD_ALLOC_DEFAULT "Implements the definition for the library's internal memory management. Uses the standard library, disable to manually implement one. See: `kh-core/impldec.h`." ON)

project("kh-core")

add_library(${PROJECT_NAME} STATIC
  "include/kh-core/defs.h"

  "include/kh-core/alloc.h"
  "src/alloc.c"
  "include/kh-core/str.h"
  "src/str.c"
)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")

#set_target_properties(${PROJECT_NAME} PROPERTIES)

target_compile_definitions(${PROJECT_NAME}
  PUBLIC $<$<BOOL:${KH_USE_STD_ALLOC_DEFAULT}>:KH_USE_STD_ALLOC_DEFAULT=1>
)

target_compile_options(${PROJECT_NAME}
  PRIVATE
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>: -Wall -Wextra -Wpedantic -Werror -Wsign-conversion>
    $<$<CXX_COMPILER_ID:MSVC>: /W4 /WX>
)

if (KH_USE_STD_ALLOC_DEFAULT)
  message("-- kh > core > Default memory management definitions are on this will cause the library to use the standard library! Check 'KH_USE_STD_ALLOC_DEFAULT'.")
else()
  message("-- kh > core > Default memory management definitions are off which requires you to implement your own! Check 'KH_USE_STD_ALLOC_DEFAULT'.")
endif()

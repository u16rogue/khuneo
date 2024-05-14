cmake_minimum_required(VERSION 3.18)

message("-- kh > extra > Loading")

set(_NAME kh-extra)
add_library(${_NAME} STATIC)
kh_target_pedantic(${_NAME})
target_link_libraries(${_NAME} PUBLIC kh-core)

#-------------------------------------------------------------------------------

function (KH_PROMPT_LOAD target status)
  message("-- kh > extra > Load extra library for '${target}'... ${status}")
endfunction()

# Message Load Available
function (KH_MSGLDAV target)
  if (TARGET ${target})
    KH_PROMPT_LOAD("${target}" "Yes.")
  else()
    KH_PROMPT_LOAD("${target}" "No.")
  endif()
endfunction()

#-------------------------------------------------------------------------------

# [25-01-2024 - r] TODO: Turn into a macro
KH_MSGLDAV(kh-gen)
if (TARGET kh-gen)
  target_link_libraries(${_NAME} PUBLIC kh-gen)
  target_sources(${_NAME} PUBLIC
    "incl-gen/kh-gen-extra/stringify.h"
    "src-gen/stringify.c"
  )
  target_include_directories(${_NAME} PUBLIC
    "incl-gen/"
  )
endif()

cmake_minimum_required(VERSION 3.18)

message("-- kh > extra > Loading")
set(_NAME kh-extra)

add_library(${_NAME} STATIC)

# [25-01-2024 - r] TODO: Turn into a macro
if (TARGET kh-gen)
  message("-- kh > extra > Load extra library for 'kh-gen'... Yes.")
  target_sources(${_NAME} PUBLIC
    "incl-gen/kh-extra-gen/stringify.h"
    "src-gen/stringify.c"
  )
  target_include_directories(${_NAME} PUBLIC
    "incl-gen/"
  )
  target_link_libraries(${_NAME} PUBLIC kh-gen)
else()
  message("-- kh > extra > Load extra library for 'kh-gen'... No.")
endif()

kh_target_pedantic(${_NAME})

target_link_libraries(${_NAME} PUBLIC kh-core)

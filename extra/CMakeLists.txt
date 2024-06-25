cmake_minimum_required(VERSION 3.18)

message("-- kh > extra > Loading")

#-------------------------------------------------------------------------------

function(KH_LOAD_OPT component)
  function (KH_PROMPT_LOAD component status)
    message("-- kh > extra > Load extra library for '${component}'... ${status}")
  endfunction()
  if (${KH_COMP_${component}})
    KH_PROMPT_LOAD("${component}" "Yes.")
    cmake_parse_arguments(
      PARSE_ARGV 1
      KH_OPTS
      ""
      ""
      "SOURCES;INCLUDE"
    )
    target_sources(${PROJECT_NAME} PUBLIC ${KH_OPTS_SOURCES})
    target_include_directories(${PROJECT_NAME} PUBLIC ${KH_OPTS_INCLUDE})
  else()
    KH_PROMPT_LOAD("${opt}" "No.")
  endif()
endfunction()

#-------------------------------------------------------------------------------

KH_LOAD_OPT(GEN
  SOURCES
    "incl-gen/kh-gen-extra/stringify.h"
    "src-gen/stringify.c"
  INCLUDE
    "incl-gen/"
)

set(KH_COMP_EXTRA ON PARENT_SCOPE)

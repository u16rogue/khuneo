cmake_minimum_required(VERSION 3.18)

message("-- kh > gen > Loading")

target_sources(${PROJECT_NAME} PUBLIC
  "include/kh-gen/lexer.h"
             "src/lexer.c"

  "include/kh-gen/parser.h"
             "src/parser.c"

  "include/kh-gen/psm.h"
  #           "src/psm.c"
  #           "src/psm_instructions.c"
)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")

set(KH_COMP_GEN ON PARENT_SCOPE)

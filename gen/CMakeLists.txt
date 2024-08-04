cmake_minimum_required(VERSION 3.18)

message("-- kh > gen > Loading")

target_sources(${PROJECT_NAME} PUBLIC
  "include/kh-gen/psm.h"
             "src/psm.c"
)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")

set(KH_COMP_GEN ON PARENT_SCOPE)

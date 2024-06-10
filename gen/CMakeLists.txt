cmake_minimum_required(VERSION 3.18)

message("-- kh > gen > Loading")

target_sources(${PROJECT_NAME} PUBLIC
  "include/kh-gen/lexer.h"
  "src/lexer.c"

  "include/kh-gen/parser.h"
  "src/parser.c"
)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")

cmake_minimum_required(VERSION 3.18)

project("kh")

add_subdirectory("core/")
add_subdirectory("gen/")

add_subdirectory("extra/") # Load last

add_library(${PROJECT_NAME} STATIC
  "include/kh/kh.h"
  "src/kh.c"
)

target_include_directories(${PROJECT_NAME} PUBLIC "include/")

set_target_properties(${PROJECT_NAME} PROPERTIES
  C_STANDARD 17
)

kh_target_pedantic(${PROJECT_NAME})

target_link_libraries(${PROJECT_NAME}
  PUBLIC kh-core
    $<$<TARGET_EXISTS:kh-gen>:kh-gen> 
    $<$<TARGET_EXISTS:kh-extra>:kh-extra>
)

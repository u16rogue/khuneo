message("-- kh > core > cmakeutils > Loading")

function (kh_target_pedantic target)
  target_compile_options(${target}
    PRIVATE
      $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>: -Wall -Wextra -Wpedantic -Werror -Wsign-conversion>
      $<$<CXX_COMPILER_ID:MSVC>: /W4 /WX>
  )
endfunction()

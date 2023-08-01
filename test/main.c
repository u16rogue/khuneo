#include "helper.h"
#include <stdio.h>
#include <kh-core/types.h>

#if !defined(KH_TEST_ASTGEN)
  #error "A test macro was not defined. Project is probably misconfigured."
#endif

#include "core/core.h"

#if KH_TEST_ASTGEN
#include "astgen/astgen.h"
#endif

static void _run_tests(struct test_context * UNIT_CTX) {
  GROUP_TEST("Core component", test_core);

  #if KH_TEST_ASTGEN
    GROUP_TEST("AST Generator component", test_astgen);
  #endif
}

int main(int argc, char ** argv) {
  (void)argc;
  (void)argv;

  struct test_context ctx = { 0 };
  _run_tests(&ctx);

  printf(
    " │\n"
    "[!] %s\n"
    " ├─ Ran %d tests.\n"
    " ├─ %d test failed.\n"
    " └─ %d test succeeded.\n"
    , ctx.halted ? "HALTED!" : "FINISHED!"
    , ctx.ran
    , ctx.failed
    , ctx.success
  );
  return ctx.failed != 0 || ctx.halted != 0;
}

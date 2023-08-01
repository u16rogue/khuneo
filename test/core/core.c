#include "core.h"
#include <kh-core/types.h>
#include <kh-core/utilities.h>
#include <kh-core/utf8.h>

DEF_TEST_UNIT(utf8_size) {
  struct cmp_set {
    const kh_utf8 chara;
    const kh_u8   exp;
  };

  const struct cmp_set sets[] = {
    { 'A',   1 },
    { 0xC0 , 2 },
    { 0xE0 , 3 },
    { 0xF0 , 4 },
  };

  kh_bool did_fail = KH_FALSE;

  for (int i = 0; i < (int)kh_narray(sets); ++i) {
    const struct cmp_set * set = &sets[i];
    kh_u8 v = kh_utf8_char_sz(set->chara);
    if (v != set->exp) {
      did_fail = KH_TRUE;
      MSG_UNIT_FMT("Expected UTF8 multi byte size to be %d but instead got %d", set->exp, v);
    }
  }

  if (kh_utf8_char_sz(0x80) != KH_UTF8_INVALID_SZ) {
    did_fail = KH_TRUE;
    MSG_UNIT("A single significant bit should be invalid.");
  }

  if (did_fail) {
    FAIL_UNIT("UTF8 size parsing is incorrect.");
  } else {
    PASS_UNIT();
  }
}

START_UNIT_TESTS(tests)
  ADD_UNIT_TEST("Core UTF8 size parsing", utf8_size)
END_UNIT_TESTS(tests)

DEF_TEST_UNIT_GROUP(test_core) {
  RUN_UNIT_TESTS(tests);
}

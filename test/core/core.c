#include "core.h"
#include "kh-core/refobj.h"
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

static kh_bool refobj_cb(struct kh_refobj_rcb_info * info) {
  (void)info;
  return KH_TRUE;
}

DEF_TEST_UNIT(refobj_counting) {
  struct kh_refobj ro;
  kh_refobji roi = KH_REFOBJ_INVALID_IREF;
  kh_refobj_init(&ro, (kh_vptr)0xCC, &roi, refobj_cb);

  if (ro._count != 1 || ro._object != (kh_vptr)0xCC || ro._resource_callback != refobj_cb) {
    FAIL_UNIT("Reference object initialization failed.");
  }

  kh_refobji roi2 = KH_REFOBJ_INVALID_IREF;
  if (kh_refobj_imove(&roi, &roi2) == KH_FALSE || roi != KH_REFOBJ_INVALID_IREF || roi2 == KH_REFOBJ_INVALID_IREF || ro._count != 1) {
    FAIL_UNIT("Reference object move failed.");
  }

  kh_refobji roi3 = KH_NULLPTR;
  if (kh_refobj_icopy(&roi2, &roi3) == KH_FALSE || roi2 == KH_REFOBJ_INVALID_IREF || roi2 != roi3 || ro._count != 2) {
    FAIL_UNIT("Rference object copy failed.");
  }

  if (kh_refobj_iremove(&roi2) == KH_FALSE || roi2 != KH_REFOBJ_INVALID_IREF || ro._count != 1) {
    FAIL_UNIT("Reference object removal failed.");
  }

  kh_refobji pretend_arg1 = kh_refobj_imovearg(&roi3);
  if (pretend_arg1 == KH_REFOBJ_INVALID_IREF || ro._count != 1 || roi3 != KH_REFOBJ_INVALID_IREF) {
    FAIL_UNIT("Reference object move args failed.");
  }

  kh_refobji pretend_arg2 = kh_refobj_icopyarg(&pretend_arg1);
  if (pretend_arg1 == KH_REFOBJ_INVALID_IREF || ro._count != 2 || pretend_arg1 == KH_REFOBJ_INVALID_IREF) {
    FAIL_UNIT("Reference object copy args failed.");
  }

  if (kh_refobj_iremove(&pretend_arg1) == KH_FALSE || pretend_arg1 != KH_REFOBJ_INVALID_IREF || ro._count != 1) {
    FAIL_UNIT("Reference object removal of arg type failed.");
  }

  if (kh_refobj_ialive(pretend_arg2) == KH_FALSE || kh_refobj_alive(&ro) == KH_FALSE) {
    FAIL_UNIT("Reference object is supposed to be still alive.");
  }

  if (kh_refobj_iremove(&pretend_arg2) == KH_FALSE || pretend_arg2 != KH_REFOBJ_INVALID_IREF || ro._count != 0) {
    FAIL_UNIT("Reference object removal of arg type failed.");
  }

  if (kh_refobj_alive(&ro) == KH_TRUE || ro._count != 0) {
    FAIL_UNIT("Reference object is supposed to be dead.");
  }

  PASS_UNIT();
}


START_UNIT_TESTS(tests)
  ADD_UNIT_TEST("Core UTF8 size parsing", utf8_size)
  ADD_UNIT_TEST("Reference object counting", refobj_counting)
END_UNIT_TESTS(tests)

DEF_TEST_UNIT_GROUP(test_core) {
  RUN_UNIT_TESTS(tests);
}

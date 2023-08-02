#include "astgen.h"
#include "kh-core/utf8.h"

#include <kh-core/types.h>
#include <kh-core/utilities.h>
#include <kh-astgen/lexer.h>

#include <string.h>

#define ZERO_MEMORY(bf) memset(&bf, 0, sizeof(bf))

DEF_TEST_UNIT(t_ll_parse_match_symbols) {
  const kh_utf8 symbols[] = "!@#%^&*()[]{}\\|;,./<>-=+";
  kh_bool did_fail = KH_FALSE;
  for (int i = 0; i < (int)kh_narray(symbols) - 1; ++i) {
    kh_utf8 code[4] = { symbols[i] };
    struct kh_lexer_ll_parse_result result;
    result.status = KH_LEXER_STATUS_OK;

    enum kh_lexer_token_type matched = kh_ll_lexer_parse(code, sizeof(code), &result);
    if (matched != KH_LEXER_TOKEN_TYPE_SYMBOL || result.status != KH_LEXER_STATUS_OK) {
      did_fail = KH_TRUE;
      MSG_UNIT_FMT("Did not match symbol: '%c'. Matched as: %d", symbols[i], matched);
    }
  }

  if (did_fail) {
    FAIL_UNIT("Symbol parsing did not match.");
  }

  PASS_UNIT();
}

DEF_TEST_UNIT(t_ll_parse_match_identifiers) {

  kh_bool did_fail = KH_FALSE;

  struct match_set {
    const kh_utf8 * code;
    int             sz;
    int             expect;
  };

  #define _set(code, expect) { code, sizeof(code) - 1, expect }
  const struct match_set pass[] = {
    _set("foo",      3),
    _set("$bar",     4),
    _set("_zoo",     4),
    _set("f00_$12a", 8),
    _set("f00 $12a", 3),
    _set("$bar",     4),
    _set("_z-0",     2),
  };

  const struct match_set fail[] = {
    _set(" foo", 0),
    _set("1bar", 0),
  };
  #undef _set

  for (int i = 0; i < (int)kh_narray(pass); ++i) {
    const struct match_set * set = &pass[i];
    struct kh_lexer_ll_parse_result res = { 0 };
    res.status = KH_LEXER_STATUS_OK;

    if (kh_ll_lexer_parse(set->code, set->sz, &res) != KH_LEXER_TOKEN_TYPE_IDENTIFIER || res.status != KH_LEXER_STATUS_OK || (int)res.value.marker.size != set->expect) {
      did_fail = KH_TRUE;
      MSG_UNIT_FMT("Parsing code '%s' expecting size %d failed. Reported size was %d with status flag %x", set->code, set->expect, res.value.marker.size, res.status);
    }
  }

  for (int i = 0; i < (int)kh_narray(fail); ++i) {
    const struct match_set * set = &fail[i];
    struct kh_lexer_ll_parse_result res = { 0 };
    res.status = KH_LEXER_STATUS_OK;
    if (kh_ll_lexer_parse(set->code, set->sz, &res) == KH_LEXER_TOKEN_TYPE_IDENTIFIER && res.status == KH_LEXER_STATUS_OK && res.value.marker.size != 0) {
      did_fail = KH_TRUE;
      MSG_UNIT_FMT("Parsing code '%s' expecting to fail. Reported size was %d", set->code, res.value.marker.size);
    }
  }

  // Test UTF8 overflow
  {
    const kh_utf8 code_utf8_overflow[] = { 'H', 'e', 'l', 'l', 0xE0, 'o' };
    struct kh_lexer_ll_parse_result res = { 0 };
    res.status = KH_LEXER_STATUS_OK;

    enum kh_lexer_token_type mtype = kh_ll_lexer_parse(code_utf8_overflow, sizeof(code_utf8_overflow), &res);
    if (mtype != KH_LEXER_TOKEN_TYPE_IDENTIFIER || res.status != KH_LEXER_STATUS_CODE_PARSE_OVERFLOW) {
      did_fail = KH_TRUE;
      MSG_UNIT_FMT("Expected code parsing to flow with match to identifier failed. Reported size: %d, mtype: %d, status: %d, u8z: %d", res.value.marker.size, mtype, res.status, kh_utf8_char_sz(0xE0));
    }
  }

  if (did_fail) {
    FAIL_UNIT("Identifier parsing did not match expect result from `code`.");
  } else {
    PASS_UNIT();
  }
}

DEF_TEST_UNIT(t_ll_parse_match_basic_string) {
  struct match_set {
    const kh_utf8 * code;
    int             sz;
    int             expect;
  };

  #define _set(code, expect) { code, sizeof(code) - 1, expect }
  const struct match_set pass[] = {
    _set("'foo'",     5),
    _set("'fo'o",     4),
    _set("''",        2),
    _set("'\\''",     4),
    _set("'\\n\\''",  6),
    _set("'$b\\'ar'", 8),
  };
  #undef _set

  kh_bool did_fail = KH_FALSE;

  for (int i = 0; i < (int)kh_narray(pass); ++i) {
    const struct match_set * set = &pass[i];
    struct kh_lexer_ll_parse_result res = { 0 };
    res.status = KH_LEXER_STATUS_OK;

    if (kh_ll_lexer_parse(set->code, set->sz, &res) != KH_LEXER_TOKEN_TYPE_STRING || res.status != KH_LEXER_STATUS_OK || (int)res.value.marker.size != set->expect) {
      did_fail = KH_TRUE;
      MSG_UNIT_FMT("Parsing code \"%s\" expecting size %d failed. Reported size was %d with status flag %x", set->code, set->expect, res.value.marker.size, res.status);
    }
  }

  // Test missing closing string tag syntax error
  {
    struct kh_lexer_ll_parse_result res = { 0 };
    res.status = KH_LEXER_STATUS_OK;
    if (kh_ll_lexer_parse("' foo", 5, &res) != KH_LEXER_TOKEN_TYPE_STRING || res.status != KH_LEXER_STATUS_SYNTAX_ERROR) {
      did_fail = KH_TRUE;
      MSG_UNIT_FMT("Parsing code '%s' expecting to to cause a string type syntax error failed.", "' foo");
    }
  }

  if (did_fail) {
    FAIL_UNIT("String parsing failed.");
  } else {
    PASS_UNIT();
  }
}


START_UNIT_TESTS(tests)
  ADD_UNIT_TEST("Low level token lexer - Match symbols",            t_ll_parse_match_symbols)
  ADD_UNIT_TEST("Low level token lexer - Match identifiers",    t_ll_parse_match_identifiers)
  ADD_UNIT_TEST("Low level token lexer - Match basic strings", t_ll_parse_match_basic_string)
END_UNIT_TESTS(tests)

DEF_TEST_UNIT_GROUP(test_astgen) {
  RUN_UNIT_TESTS(tests);
}

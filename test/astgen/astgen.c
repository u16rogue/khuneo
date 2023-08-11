#include "astgen.h"
#include "kh-core/refobj.h"

#include <kh-core/types.h>
#include <kh-core/utilities.h>
#include <kh-astgen/lexer.h>

#include <kh-extra/astgen/lexer.h>

#include <string.h>

#define ZERO_MEMORY(bf) memset(&bf, 0, sizeof(bf))

struct code_set_marker_test_entry {
  const kh_utf8 * code;
  const kh_sz     code_size;
  const kh_sz     expected_marker_size;
};

struct code_set_marker_test_pretext {
  enum kh_lexer_token_type expected_type;
  kh_bool should_type;
  enum kh_lexer_status expected_status;
  kh_bool should_status;
  kh_bool ignore_size;

  enum kh_lexer_status(*ll_lexer)(const kh_utf8 *, kh_sz, struct kh_lexer_parse_result *);
};


struct code_set_marker_test_context {
  struct code_set_marker_test_entry * tests;
  kh_sz ntests;

  struct code_set_marker_test_pretext pretext;

  const char * failmsg;
  kh_bool pass;
};

static void generic_marker_test(struct code_set_marker_test_context * ctx) {
  for (int i = 0; i < (int)ctx->ntests; ++i) {
    const struct code_set_marker_test_entry * test = &ctx->tests[i];
    struct kh_lexer_parse_result res = { 0 };
    res.type = KH_LEXER_TOKEN_TYPE_NONE;

    enum kh_lexer_status status = ctx->pretext.ll_lexer(test->code, test->code_size, &res);
    kh_bool rtype = (res.type == ctx->pretext.expected_type)   ? KH_FALSE : KH_TRUE  == ctx->pretext.should_type;
    kh_bool rstat = (status   == ctx->pretext.expected_status) ? KH_FALSE : KH_TRUE  == ctx->pretext.should_status;
    kh_bool rsz   = (ctx->pretext.ignore_size == KH_FALSE && res.value.marker.size != test->expected_marker_size);
    if (rtype || rstat || rsz)  {
      ctx->pass = KH_FALSE;
      MSG_UNIT_FMT("%s [Code \"%s\", Expected size %d, Reported size %d, Status flag %s, mtype: %s | T:%d, S:%d, s:%d]",
        ctx->failmsg, test->code, (int)test->expected_marker_size, res.value.marker.size, kh_extra_lexer_tostr_ctx_status(status), kh_extra_lexer_tostr_token_type(res.type), rtype, rstat, rsz
      );
    }
  }
}

#define CODE_SET(code, exp) { code, sizeof(code) - 1, exp }


DEF_TEST_UNIT(t_ll_lex_match_symbols) {
  const kh_utf8 symbols[] = "!@#%^&*()[]{}\\|;,./<>-=+";
  kh_bool did_fail = KH_FALSE;
  for (int i = 0; i < (int)kh_narray(symbols) - 1; ++i) {
    kh_utf8 code[4] = { symbols[i] };
    struct kh_lexer_parse_result result;
    result.type = KH_LEXER_TOKEN_TYPE_NONE;

    enum kh_lexer_status status = kh_ll_lexer_parse_type(code, sizeof(code), &result);
    if (result.type != KH_LEXER_TOKEN_TYPE_SYMBOL || status != KH_LEXER_STATUS_MATCH) {
      did_fail = KH_TRUE;
      MSG_UNIT_FMT("Did not match symbol: '%c'. Matched as: %s", symbols[i], kh_extra_lexer_tostr_token_type(result.type));
    }
  }

  if (did_fail) {
    FAIL_UNIT("Symbol parsing did not match.");
  }

  PASS_UNIT();
}

DEF_TEST_UNIT(t_ll_lex_match_identifiers) {
  struct code_set_marker_test_context ctx; 
  ctx.pretext.ll_lexer = kh_ll_lexer_parse_type;

  const struct code_set_marker_test_entry pass[] = {
    CODE_SET("foo",      3),
    CODE_SET("$bar",     4),
    CODE_SET("_zoo",     4),
    CODE_SET("f00_$12a", 8),
    CODE_SET("f00 $12a", 3),
    CODE_SET("$bar",     4),
    CODE_SET("_z-0",     2),
  };
  ctx.pass = KH_TRUE;
  ctx.pretext.ignore_size = KH_FALSE;
  ctx.tests = (struct code_set_marker_test_entry *)&pass;
  ctx.ntests = kh_narray(pass);
  ctx.pretext.expected_type   = KH_LEXER_TOKEN_TYPE_IDENTIFIER;
  ctx.pretext.should_type     = KH_TRUE;
  ctx.pretext.expected_status = KH_LEXER_STATUS_MATCH;
  ctx.pretext.should_status   = KH_TRUE;
  ctx.failmsg = "Expected a pass parsing identifiers. ";
  generic_marker_test(&ctx);

  if (ctx.pass) {
    PASS_UNIT();
  } else {
    FAIL_UNIT("Identifier parsing did not match expect result from `code`.");
  }
}

DEF_TEST_UNIT(t_ll_lex_match_basic_string) {
  struct code_set_marker_test_context ctx; 
  ctx.pretext.ll_lexer = kh_ll_lexer_parse_type;

  const struct code_set_marker_test_entry pass_str[] = {
    CODE_SET("'foo'",     5),
    CODE_SET("'fo'o",     4),
    CODE_SET("''",        2),
    CODE_SET("'\\''",     4),
    CODE_SET("'\\n\\''",  6),
    CODE_SET("'$b\\'ar'", 8), 
  };

  ctx.pass = KH_TRUE;
  ctx.pretext.ignore_size = KH_FALSE;
  ctx.tests = (struct code_set_marker_test_entry *)&pass_str;
  ctx.ntests = kh_narray(pass_str);
  ctx.pretext.expected_type   = KH_LEXER_TOKEN_TYPE_STRING; 
  ctx.pretext.should_type     = KH_TRUE;
  ctx.pretext.expected_status = KH_LEXER_STATUS_MATCH;
  ctx.pretext.should_status   = KH_TRUE;
  ctx.failmsg = "Expected a pass parsing identifiers. ";
  generic_marker_test(&ctx);

  const struct code_set_marker_test_entry pass_strintrp[] = {
    CODE_SET("`foo`",     5),
    CODE_SET("`fo`o",     4),
    CODE_SET("``",        2),
    CODE_SET("`\\``",     4),
    CODE_SET("`\\n\\``",  6),
    CODE_SET("`$b\\`ar`", 8),
  };

  ctx.tests = (struct code_set_marker_test_entry *)&pass_strintrp;
  ctx.ntests = kh_narray(pass_strintrp);
  ctx.pretext.expected_type = KH_LEXER_TOKEN_TYPE_STRING_INTRP; 
  generic_marker_test(&ctx);

  const struct code_set_marker_test_entry pass_strhsh[] = {
    CODE_SET("\"foo\"",      5),
    CODE_SET("\"fo\"o",      4),
    CODE_SET("\"\"",         2),
    CODE_SET("\"\\\"\"",     4),
    CODE_SET("\"\\n\\\"\"",  6),
    CODE_SET("\"$b\\\"ar\"", 8),
  };

  ctx.tests = (struct code_set_marker_test_entry *)&pass_strhsh;
  ctx.ntests = kh_narray(pass_strhsh);
  ctx.pretext.expected_type = KH_LEXER_TOKEN_TYPE_STRING_FXHSH; 
  generic_marker_test(&ctx);

  // Test missing closing string tag syntax error
  {
    struct kh_lexer_parse_result res = { 0 };
    res.type = KH_LEXER_TOKEN_TYPE_NONE;
    if (kh_ll_lexer_parse_type("' foo", 5, &res) != KH_LEXER_STATUS_SYNTAX_ERROR || res.type != KH_LEXER_TOKEN_TYPE_STRING) {
      ctx.pass = KH_FALSE;
      MSG_UNIT_FMT("Parsing code '%s' expecting to to cause a string type syntax error failed.", "' foo");
    }
  }

  if (ctx.pass) {
    PASS_UNIT();
  } else {
    FAIL_UNIT("String parsing failed.");
  }
}

DEF_TEST_UNIT(t_ll_lex_match_unsigned_numbers) {
  struct code_set_marker_test_context ctx; 
  ctx.pretext.ll_lexer = kh_ll_lexer_parse_type;

  struct code_set_marker_test_entry pass[] = {
    CODE_SET("1234",         4),
    CODE_SET("0123456789",  10),
    CODE_SET("12.34",        2),
    CODE_SET("12x34",        2),
    CODE_SET("0x1a3F",       6),
  };
  ctx.pass = KH_TRUE;
  ctx.pretext.ignore_size = KH_FALSE;
  ctx.tests = (struct code_set_marker_test_entry *)&pass;
  ctx.ntests = kh_narray(pass);
  ctx.pretext.expected_type   = KH_LEXER_TOKEN_TYPE_NUMBER;
  ctx.pretext.should_type     = KH_TRUE;
  ctx.pretext.expected_status = KH_LEXER_STATUS_MATCH;
  ctx.pretext.should_status   = KH_TRUE;
  ctx.failmsg = "Expected a pass parsing numerical values.";
  generic_marker_test(&ctx);

  if (ctx.pass) {
    PASS_UNIT();
  } else {
    FAIL_UNIT("Number parsing failed.");
  }
}


DEF_TEST_UNIT(t_ll_lex_match_groups) {
  struct code_set_marker_test_context ctx; 
  ctx.pretext.ll_lexer = kh_ll_lexer_parse_group;
  struct code_set_marker_test_entry pass[] = {
    CODE_SET("{ hello }",      9),
    CODE_SET("{[[[}",          5),
    CODE_SET("[][]",           2),
    CODE_SET("((fgfff[{{[))", 13),
  };
  ctx.pass = KH_TRUE;
  ctx.pretext.ignore_size = KH_FALSE;
  ctx.tests = (struct code_set_marker_test_entry *)&pass;
  ctx.ntests = kh_narray(pass);
  ctx.pretext.expected_type   = KH_LEXER_TOKEN_TYPE_GROUP;
  ctx.pretext.should_type     = KH_TRUE;
  ctx.pretext.expected_status = KH_LEXER_STATUS_MATCH;
  ctx.pretext.should_status   = KH_TRUE;
  ctx.failmsg = "Expected a pass parsing group contexts.";
  generic_marker_test(&ctx);

  struct code_set_marker_test_entry fail[] = {
    CODE_SET("[[]", 0),
    CODE_SET("(()", 0),
    CODE_SET("{{}", 0),
  };
  ctx.pretext.ignore_size = KH_TRUE;
  ctx.tests = (struct code_set_marker_test_entry *)&fail;
  ctx.ntests = kh_narray(fail);
  ctx.pretext.expected_status = KH_LEXER_STATUS_SYNTAX_ERROR;
  ctx.failmsg = "Expected a fail parsing group contexts.";
  generic_marker_test(&ctx);

  if (ctx.pass) {
    PASS_UNIT();
  } else {
    FAIL_UNIT("Group parsing failed.");
  }
}

DEF_TEST_UNIT(t_ll_lex_run_lexer_next) {
  const kh_utf8 code[] =
    "/*                             \n"
    " * multiline                   \n"
    " */                            \n"
    "// what to say                 \n"
    "var x: string = 'hello world'; \n"
    "// does the talking            \n"
    "// abcdef                      \n"
    "fn greet() nil {               \n"
    "  print(x);                    \n"
    "}                              \n"
  ;

  const enum kh_lexer_token_type matches[] = {
    KH_LEXER_TOKEN_TYPE_IDENTIFIER, // var
    KH_LEXER_TOKEN_TYPE_IDENTIFIER, // x
    KH_LEXER_TOKEN_TYPE_SYMBOL,     // :
    KH_LEXER_TOKEN_TYPE_IDENTIFIER, // string
    KH_LEXER_TOKEN_TYPE_SYMBOL,     // =
    KH_LEXER_TOKEN_TYPE_STRING,     // 'hello world'
    KH_LEXER_TOKEN_TYPE_SYMBOL,     // ;
    KH_LEXER_TOKEN_TYPE_IDENTIFIER, // fn
    KH_LEXER_TOKEN_TYPE_IDENTIFIER, // greet
    KH_LEXER_TOKEN_TYPE_SYMBOL,     // (
    KH_LEXER_TOKEN_TYPE_SYMBOL,     // )
    KH_LEXER_TOKEN_TYPE_IDENTIFIER, // nil
    KH_LEXER_TOKEN_TYPE_SYMBOL,     // {
    KH_LEXER_TOKEN_TYPE_IDENTIFIER, // print
    KH_LEXER_TOKEN_TYPE_SYMBOL,     // (
    KH_LEXER_TOKEN_TYPE_IDENTIFIER, // x
    KH_LEXER_TOKEN_TYPE_SYMBOL,     // )
    KH_LEXER_TOKEN_TYPE_SYMBOL,     // ;
    KH_LEXER_TOKEN_TYPE_SYMBOL,     // }
  };

  struct kh_refobj ro_code;
  kh_refobji roi_code = KH_REFOBJ_INVALID_IREF;
  if (kh_refobj_init(&ro_code, (kh_vptr)code, &roi_code, KH_NULLPTR) == KH_FALSE) {
    FAIL_UNIT("Failed to initialize code ref object");
  }

  struct kh_lexer_context ctx;
  if (kh_lexer_context_init(&ctx, kh_refobj_imovearg(&roi_code), sizeof(code) - 1) == KH_FALSE) {
    FAIL_UNIT("Failed to initialize lexer context.");
  }

  for (int i = 0; i < (int)kh_narray(matches); ++i) {
    if (ctx._code_index >= ctx._code_size) {
      MSG_UNIT_FMT("Index off bound without completing full match. Index: %d", ctx._code_index);
      FAIL_UNIT("Failed to parse code.");
    }
    struct kh_lexer_parse_result res;
    res.type = KH_LEXER_TOKEN_TYPE_NONE;
    enum kh_lexer_status status = kh_lexer_context_parse_next(&ctx, &res);
    if (res.type != matches[i] || status != KH_LEXER_STATUS_MATCH) {
      MSG_UNIT_FMT("Parsing code at lexing stage failed. Expecting type: %s [%d], Got type: %s, Status: %s, Position: %d",
          kh_extra_lexer_tostr_token_type(matches[i]),
          i + 1,
          kh_extra_lexer_tostr_token_type(res.type),
          kh_extra_lexer_tostr_ctx_status(status),
          ctx._code_index
      );
      MSG_UNIT_FMT(">> %s", &code[ctx._code_index]);
      FAIL_UNIT("Failed to parse code.");
    }
  }

  if (kh_lexer_context_uninit(&ctx) == KH_FALSE) {
    FAIL_UNIT("Failed to uninitialize lexer context.");
  }

  if (kh_refobj_alive(&ro_code) == KH_TRUE) {
    FAIL_UNIT("Code reference object should be dead.");
  }

  PASS_UNIT();
}

START_UNIT_TESTS(tests)
  ADD_UNIT_TEST("Low level token lexer - Match symbols",          t_ll_lex_match_symbols)
  ADD_UNIT_TEST("Low level token lexer - Match identifiers",      t_ll_lex_match_identifiers)
  ADD_UNIT_TEST("Low level token lexer - Match basic strings",    t_ll_lex_match_basic_string)
  ADD_UNIT_TEST("Low level token lexer - Match unsigned numbers", t_ll_lex_match_unsigned_numbers)
  ADD_UNIT_TEST("Low level token lexer - Match groups",           t_ll_lex_match_groups)
  ADD_UNIT_TEST("Low level token lexer - Run lexer parser ",      t_ll_lex_run_lexer_next)
END_UNIT_TESTS(tests)

DEF_TEST_UNIT_GROUP(test_astgen) {
  RUN_UNIT_TESTS(tests);
}

#undef CODE_SET

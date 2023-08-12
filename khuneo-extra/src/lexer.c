#include <kh-extra/astgen/lexer.h>

#define CASE_STR(e) case e: return &((#e)[offset])
#define SET_OFFSET(substr) const int offset = sizeof(substr) - 1

const char * kh_extra_lexer_tostr_ctx_status(enum kh_lexer_status status) {
  SET_OFFSET("KH_LEXER_STATUS_");
  switch (status) {
    CASE_STR(KH_LEXER_STATUS_OK);
    CASE_STR(KH_LEXER_STATUS_MATCH);

    CASE_STR(KH_LEXER_STATUS_PASS);
    CASE_STR(KH_LEXER_STATUS_NOMATCH);

    CASE_STR(KH_LEXER_STATUS_EOB);

    CASE_STR(KH_LEXER_STATUS_UNKNOWN_WARNING);
    CASE_STR(KH_LEXER_STATUS_CODE_PARSE_OVERFLOW);

    CASE_STR(KH_LEXER_STATUS_UNKNOWN_ERROR);
    CASE_STR(KH_LEXER_STATUS_UTF8_INVALID);
    CASE_STR(KH_LEXER_STATUS_SYNTAX_ERROR);

    case KH_LEXER_STATUS_EXTENDED:
      return "<flag>";
  }

  return "<not found>";
}


const char * kh_extra_lexer_tostr_token_type(enum kh_lexer_token_type type) {
  SET_OFFSET("KH_LEXER_TOKEN_TYPE_");
  switch (type) {
    CASE_STR(KH_LEXER_TOKEN_TYPE_NONE);
    CASE_STR(KH_LEXER_TOKEN_TYPE_GROUP);
    CASE_STR(KH_LEXER_TOKEN_TYPE_SYMBOL);
    CASE_STR(KH_LEXER_TOKEN_TYPE_IDENTIFIER);
    CASE_STR(KH_LEXER_TOKEN_TYPE_U64);
    CASE_STR(KH_LEXER_TOKEN_TYPE_F64);
    CASE_STR(KH_LEXER_TOKEN_TYPE_STRING);
    CASE_STR(KH_LEXER_TOKEN_TYPE_STRING_INTRP);
    CASE_STR(KH_LEXER_TOKEN_TYPE_STRING_FXHSH);
    CASE_STR(KH_LEXER_TOKEN_TYPE_WHITESPACE);
    CASE_STR(KH_LEXER_TOKEN_TYPE_COMMENT);
  }

  return "<not found>";
}

#undef SET_OFFSET
#undef CASE_STR

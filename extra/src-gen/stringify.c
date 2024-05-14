#include <kh-gen-extra/stringify.h>

#define case_str(x) \
  case x: r = #x; break

const kh_U8Char * kh_stringify_token_type(enum kh_TokenType type) {
  const char * r = "<invalid>";
  switch (type) {
    case_str(KH_TOKEN_TYPE_INVALID);
    case_str(KH_TOKEN_TYPE_IDENTIFIER);
    case_str(KH_TOKEN_TYPE_NUMBER);
    case_str(KH_TOKEN_TYPE_WHITESPACE);
    case_str(KH_TOKEN_TYPE_STRING);
    case_str(KH_TOKEN_TYPE_SYMBOL);
    case_str(KH_TOKEN_TYPE_KEYWORD);
  }
  return (const kh_U8Char *)r;
}

const kh_U8Char * kh_stringify_lexer_response(enum kh_LexerResponse response) {
  const char * r = "<invalid>";
  switch (response) {
    case_str(KH_LEXER_RES_OK);
    case_str(KH_LEXER_RES_MATCH);
    case_str(KH_LEXER_RES_PASS);
    case_str(KH_LEXER_RES_FAIL);
    case_str(KH_LEXER_RES_UNDESCRIBED);
    case_str(KH_LEXER_RES_UNCLOSED);
    case_str(KH_LEXER_RES_EXTENDED);
    case_str(KH_LEXER_RES_INVALID_CTX);
  }
  return (const kh_U8Char *)r;
}

#undef case_str

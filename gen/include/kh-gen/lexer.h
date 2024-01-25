#pragma once

#include <kh-core/defs.h>
#include <kh-core/str.h>

//------------------------------------------------------------------------------

struct kh_LexerContext {
  char _;
};

//------------------------------------------------------------------------------

enum kh_LexerInitResponse {
  KH_LEXER_INIT_OK,
};

enum kh_LexerInitResponse
kh_lexer_init(
  struct kh_LexerContext * const ctx
);

//------------------------------------------------------------------------------

enum kh_TokenType {
  KH_TOKEN_TYPE_INVALID,
  KH_TOKEN_TYPE_IDENTIFIER,
  KH_TOKEN_TYPE_NUMBER,
};

struct kh_LexerDescribeResult {
  enum kh_TokenType type;
  kh_u32 size;
};

enum kh_LexerResponse {
  KH_LEXER_RES_OK          = 0x00,
  KH_LEXER_RES_MATCH       = 0x01,
  KH_LEXER_RES_PASS        = 0x02,
  //
  KH_LEXER_RES_FAIL        = 0x80,
  KH_LEXER_RES_UNDESCRIBED = 0x80 | 0x01,
  //
  KH_LEXER_RES_EXTENDED    = 0xFF,
};

//------------------------------------------------------------------------------

enum kh_LexerResponse
kh_ll_lexer_describe(
  KH_ANT_ARG_IN  const kh_U8Char * const chunk,
  KH_ANT_ARG_IN  const kh_U8StringSize chunk_range,
  KH_ANT_ARG_OUT struct kh_LexerDescribeResult * described
);

//------------------------------------------------------------------------------

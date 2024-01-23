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

enum kh_LexerType {
  KH_LEXER_TYPE_INVALID,
  KH_LEXER_TYPE_IDENTIFIER,
};

struct kh_LexerDescribeResult {
  enum kh_LexerType type;
  kh_u32 offset;
  kh_u32 size;
};

enum kh_LexerDescribeResponse {
  KH_LEXER_DESCRIBE_OK          = 0x00,
  KH_LEXER_DESCRIBE_MATCH       = 0x01,
  KH_LEXER_DESCRIBE_MISS        = 0x02,
  //
  KH_LEXER_DESCRIBE_FAIL        = 0x80,
  KH_LEXER_DESCRIBE_UNDESCRIBED = 0x80 | 0x01,
  //
  KH_LEXER_DESCRIBE_EXTENDED    = 0xFF,
};

enum kh_LexerDescribeResponse
kh_ll_lexer_describe(
  KH_ANT_ARG_IN  const kh_U8Char * const chunk,
  KH_ANT_ARG_IN  const kh_U8StringSize chunk_range,
  KH_ANT_ARG_OUT struct kh_LexerDescribeResult * described
);

//------------------------------------------------------------------------------

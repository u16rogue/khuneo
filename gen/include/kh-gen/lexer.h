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
  KH_TOKEN_TYPE_WHITESPACE,
  KH_TOKEN_TYPE_SYMBOL,
  KH_TOKEN_TYPE_STRING,
};

enum kh_TokenSymbol {
  KH_TOKEN_SYMBOL_INVALID,
  //
  KH_TOKEN_SYMBOL_EXC,     // !
  KH_TOKEN_SYMBOL_AT,      // @
  KH_TOKEN_SYMBOL_HSH,     // #
  KH_TOKEN_SYMBOL_HAT,     // ^
  KH_TOKEN_SYMBOL_AND,     // &
  KH_TOKEN_SYMBOL_STR,     // *
  KH_TOKEN_SYMBOL_PRN_S,   // (
  KH_TOKEN_SYMBOL_PRN_E,   // )
  KH_TOKEN_SYMBOL_BRK_S,   // [
  KH_TOKEN_SYMBOL_BRK_E,   // ]
  KH_TOKEN_SYMBOL_CBK_S,   // {
  KH_TOKEN_SYMBOL_CBK_E,   // }
  KH_TOKEN_SYMBOL_SLH,     // /
  KH_TOKEN_SYMBOL_BSL,     // '\'
  KH_TOKEN_SYMBOL_COM,     // ,
  KH_TOKEN_SYMBOL_DOT,     // .
  KH_TOKEN_SYMBOL_SCL,     // ;
  KH_TOKEN_SYMBOL_COL,     // :
  KH_TOKEN_SYMBOL_DSH,     // -
  KH_TOKEN_SYMBOL_EQ,      // =
  KH_TOKEN_SYMBOL_PLS,     // +
  KH_TOKEN_SYMBOL_QSM,     // ?
  //
  KH_TOKEN_SYMBOL_EXC_EQ,  // !=
  KH_TOKEN_SYMBOL_LT_EQ,   // <=
  KH_TOKEN_SYMBOL_GT_EQ,   // >=
  KH_TOKEN_SYMBOL_EQ_EQ,   // ==
  KH_TOKEN_SYMBOL_COL_EQ,  // :=
  KH_TOKEN_SYMBOL_TIL_EQ,  // ~=
  KH_TOKEN_SYMBOL_PLS_EQ,  // +=
  KH_TOKEN_SYMBOL_DSH_EQ,  // -=
  KH_TOKEN_SYMBOL_STR_EQ,  // *=
  KH_TOKEN_SYMBOL_SLH_EQ,  // /=
  KH_TOKEN_SYMBOL_PER_EQ,  // %=
  KH_TOKEN_SYMBOL_BAR_EQ,  // |=
  KH_TOKEN_SYMBOL_AND_EQ,  // &=
  KH_TOKEN_SYMBOL_HAT_EQ,  // ^=
  KH_TOKEN_SYMBOL_PLS_PLS, // ++
  KH_TOKEN_SYMBOL_DSH_DSH, // --
  KH_TOKEN_SYMBOL_AND_AND, // &&
  KH_TOKEN_SYMBOL_BAR_BAR, // ||
};

enum kh_TokenKeyword {
  KH_TOKEN_KEYWORD_INVALID,
  //
  
};

struct kh_LexerDescribeResult {
  enum kh_TokenType type;
  union {
    kh_u32 size;

    struct {
      enum kh_TokenSymbol type : 28;
      kh_u8               size :  4;
    } symbol;               // = 32
    
    struct {
      enum kh_TokenKeyword type : 28;
      kh_u8                size :  4;
    } keyword;               // = 32
  };
};

enum kh_LexerResponse {
  KH_LEXER_RES_OK          = 0x00,
  KH_LEXER_RES_MATCH       = 0x01,
  KH_LEXER_RES_PASS        = 0x02,
  //
  KH_LEXER_RES_FAIL        = 0x80,
  KH_LEXER_RES_UNDESCRIBED = 0x80 | 0x01,
  KH_LEXER_RES_UNCLOSED    = 0x80 | 0x02,
  //
  KH_LEXER_RES_EXTENDED    = 0xFF,
};

//------------------------------------------------------------------------------

/*
 *  Describe what the token is in a given chunk buffer. Operates purely on the
 *  premise of identification NOT parsing.
 */
enum kh_LexerResponse
kh_ll_lexer_describe(
  KH_ANT_ARG_IN  const kh_U8Char * const chunk,
  KH_ANT_ARG_IN  const kh_U8StringSize chunk_range,
  KH_ANT_ARG_OUT struct kh_LexerDescribeResult * described
);

enum kh_LexerResponse
kh_ll_lexer_identifier_to_keyword(
  KH_ANT_ARG_IN  const kh_U8Char * const chunk,
  KH_ANT_ARG_IN  const kh_U8StringSize chunk_range,
  KH_ANT_ARG_OUT struct kh_LexerDescribeResult * described
);

//------------------------------------------------------------------------------

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

  // Any wordset that starts with a alphabet, an underscore, or a dollar sign
  // followed consecutively by the same parameters with the addition of numbers.
  KH_TOKEN_TYPE_IDENTIFIER,

  KH_TOKEN_TYPE_NUMBER,
  KH_TOKEN_TYPE_WHITESPACE,
  KH_TOKEN_TYPE_SYMBOL,
  KH_TOKEN_TYPE_STRING,
};

/*
 *  Defines the format of the lexed number token matched.
 */
enum kh_TokenNumber {
  KH_TOKEN_NUMBER_INVALID,
  KH_TOKEN_NUMBER_WHOLE,
  KH_TOKEN_NUMBER_FLOATING,
  KH_TOKEN_NUMBER_HEX,
};

enum kh_TokenSymbol {
  KH_TOKEN_SYMBOL_INVALID,

  #define __KH_TOKSYM1_DEF(b, e) e,
  #define __KH_TOKSYM2_DEF(b, e) __KH_TOKSYM1_DEF(b, e)
  #include "lst/symbols.lst"
  #undef __KH_TOKSYM2_DEF
  #undef __KH_TOKSYM1_DEF
};

/*
 *  Defines the matched keyword of the lexed identifier.
 */
enum kh_TokenKeyword {
  KH_TOKEN_KEYWORD_INVALID,

  #define __KH_TOKKW2_DEF(b, e) e,
  #define __KH_TOKKW3_DEF(b, e) __KH_TOKKW2_DEF(b, e)
  #include "lst/keywords.lst"
  #undef __KH_TOKKW3_DEF
  #undef __KH_TOKKW2_DEF
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

  /*
   *  [14.03.2024 @u16rogue] NOTE: Preserves the KH_LEXER_RES_FAIL bit.
   *  An extended response can be designated a failure by setting the bit eg.
   *  `FUTUREFAILFLAG = KH_LEXER_RES_FAIL | KH_LEXER_RES_EXTENDED | (ID << 8)`
   *  This makes it easy to determine if a result is a failure or not without
   *  any lookup.
   */
  KH_LEXER_RES_EXTENDED    = 0x7F, // Extended result, see next byte.
};

//------------------------------------------------------------------------------

/*
 *  Describe what the token is in a given chunk buffer. Operates purely on the
 *  premise of identification NOT parsing.
 */
enum kh_LexerResponse
kh_ll_lexer_describe(
  // Start chunk
  KH_ANT_ARG_IN  const kh_U8Char * const         chunk,
  // Chunk range for the lexer
  KH_ANT_ARG_IN  const kh_U8StringSize           chunk_range,
  // Result of the lexer
  KH_ANT_ARG_OUT struct kh_LexerDescribeResult * described
);

/*
 *  Identifies if a given identifier result is a built in keyword. Used in
 *  conjunction with `kh_ll_lexer_describe` after receiving a
 *  `KH_TOKEN_TYPE_IDENTIFIER`. This fills the `keyword.type` field.
 */
enum kh_LexerResponse
kh_ll_lexer_identifier_to_keyword(
  // Start chunk
  KH_ANT_ARG_IN  const kh_U8Char * const         chunk,
  // Chunk range for the lexer
  KH_ANT_ARG_IN  const kh_U8StringSize           chunk_range,
  // Result of the lexer
  KH_ANT_ARG_OUT struct kh_LexerDescribeResult * described
);

//------------------------------------------------------------------------------

enum kh_LexerResponse kh_lexer_gobble_step(
  struct kh_LexerContext * ctx
);

enum kh_LexerResponse kh_lexer_gobble_stop(
  struct kh_LexerContext * ctx
);

enum kh_LexerResponse kh_lexer_gobble_start(
  struct kh_LexerContext * ctx,
  struct kh_U8StringView code
);

//------------------------------------------------------------------------------

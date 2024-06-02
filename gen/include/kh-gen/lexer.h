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

  // Any wordset that starts with an alphabet, underscore, or a dollar sign
  // followed consecutively by the same parameters with the addition of numbers.
  KH_TOKEN_TYPE_IDENTIFIER,
  KH_TOKEN_TYPE_KEYWORD,

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

  #define __KH_TOKSYM_DEF(b, e) e,
  #include "lst/symbols1.lst"
  #include "lst/symbols2.lst"
  #undef __KH_TOKSYM_DEF
};

/*
 *  Defines the matched keyword of the lexed identifier.
 */
enum kh_TokenKeyword {
  KH_TOKEN_KEYWORD_INVALID,

  #define __KH_TOKKW_DEF(b, e) e,
  #include "lst/keywords2.lst"
  #include "lst/keywords3.lst"
  #undef __KH_TOKKW_DEF
};

/*
 *  Description of a single token
 */
struct kh_LexerDescription {
  enum kh_TokenType type;
  union {
    kh_u32 size;

    struct {
      enum kh_TokenSymbol symb : 28;
      kh_u8               size :  4;
    } symbol;               // = 32
    
    struct {
      enum kh_TokenKeyword what : 28;
      kh_u8                size :  4;
    } keyword;               // = 32
    
    struct {
      kh_u32               value : 28;
      kh_u8                size  :  4;
    } generic;

  };
};

kh_u32 kh_lexer_describe_size_get(
  const struct kh_LexerDescription * const description
);

//------------------------------------------------------------------------------

enum kh_LexerResponse {
  KH_LEXER_RES_OK          = 0x00,
  //
  KH_LEXER_RES_PASS        = KH_MSGBIT_SET(1),
  KH_LEXER_RES_END         = KH_MSGBIT_SET(2),
  //
  KH_LEXER_RES_FAIL        = KH_FAIL_BIT,
  KH_LEXER_RES_UNDESCRIBED = KH_FAIL_BIT | KH_MSGBIT_SET(1),
  KH_LEXER_RES_UNCLOSED    = KH_FAIL_BIT | KH_MSGBIT_SET(2),
  KH_LEXER_RES_INVALID_CTX = KH_FAIL_BIT | KH_MSGBIT_SET(3),
  KH_LEXER_RES_INVALID_OFF = KH_FAIL_BIT | KH_MSGBIT_SET(4),

  /*
   *  [14.03.2024 @u16rogue] NOTE: Preserves the KH_LEXER_RES_FAIL bit.
   *  An extended response can be designated a failure by setting the bit eg.
   *  `FUTUREFAILFLAG = KH_LEXER_RES_FAIL | KH_LEXER_RES_EXTENDED | (ID << 8)`
   *  This makes it easy to determine if a result is a failure or not without
   *  any lookup.
   */
  KH_LEXER_RES_EXTENDED    = 0xFE, // Extended result, see next byte.
};

//------------------------------------------------------------------------------

// [28.05.2024 @u16rogue NOTE] This exists so we can cache the values we need
// when doing a straight up parse as we dispatch it to the lexer and other comp
// this way we dont have to keep calling kh_u8string*_* functions since we cache
// it before lexing. This is within the assumption that at that point the source
// code we're working on is in an immutable state.

struct kh_SourceCodeBuffer {
  const kh_U8Char * _buffer;
  kh_U8StringSize _size;
};

/*
enum kh_LexerResponse
kh_lexer_u8strview_to_scb(
  KH_ANT_ARG_OUT struct kh_SourceCodeBuffer * scb,
  KH_ANT_ARG_IN  struct kh_U8StringView     * view
);
*/

//------------------------------------------------------------------------------

/*
 *  Describe what the token is in a given chunk buffer. Operates purely on the
 *  premise of identification NOT parsing.
 *
 *  NOTE: WILL NOT MATCH `KEYWORD` SEE `kh_ll_lexer_identifier_to_keyword`
 */
enum kh_LexerResponse
kh_ll_lexer_describe(
  // Start chunk
  KH_ANT_ARG_IN  const kh_U8Char * const      chunk,
  // Chunk range for the lexer
  KH_ANT_ARG_IN  const kh_U8StringSize        chunk_range,
  // Result of the lexer
  KH_ANT_ARG_OUT struct kh_LexerDescription * described
);

/*
 *  Identifies if a given identifier description is a built in keyword. Used in
 *  conjunction with `kh_ll_lexer_describe` after receiving a
 *  `KH_TOKEN_TYPE_IDENTIFIER`. This fills the `keyword.type` field and updates
 *  IDENTIFIER to KEYWORD on match.
 */
enum kh_LexerResponse
kh_ll_lexer_identifier_to_keyword(
  // Start chunk
  KH_ANT_ARG_IN  const kh_U8Char * const      chunk,
  // Size of the identifier chunk
  KH_ANT_ARG_IN  const kh_U8StringSize        identifier_size,
  // Result of the lexer
  KH_ANT_ARG_OUT struct kh_LexerDescription * described
);

//------------------------------------------------------------------------------

struct kh_LexerGobbleContext {
  kh_u32 _cursor;
  const struct kh_SourceCodeBuffer * _code;
  struct kh_LexerDescription description;
};

enum kh_LexerResponse
kh_lexer_gobble_analyze(
  struct kh_LexerGobbleContext * ctx
);

enum kh_LexerResponse
kh_lexer_gobble_step(
  struct kh_LexerGobbleContext * ctx
);

enum kh_LexerResponse
kh_lexer_gobble_stop(
  struct kh_LexerGobbleContext * ctx
);

enum kh_LexerResponse
kh_lexer_gobble_start(
  struct kh_LexerGobbleContext * ctx,
  const struct kh_SourceCodeBuffer * code
);

//------------------------------------------------------------------------------

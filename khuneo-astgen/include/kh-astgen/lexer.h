#pragma once

#include <kh-core/refobj.h>

typedef struct _kh_lexer_ll_parse_result_value {
  kh_utf8 token;
  struct {
    kh_u32 offset; // Starting position of the chunk
    kh_u32 size;   // Size of the chunk (in bytes)
  } code_marker;   // Refers a chunk of the raw source buffer
} kh_lexer_ll_parse_result_value;

typedef enum _kh_lexer_token_type {
  KH_LEXER_TOKEN_TYPE_NONE,
  KH_LEXER_TOKEN_TYPE_GROUP,      // Refers to a range group used for lazy evaluation. Uses the `code_marker` value.
  KH_LEXER_TOKEN_TYPE_SYMBOL,     // Refers to a single byte symbol. Uses the `token` value.
  KH_LEXER_TOKEN_TYPE_IDENTIFIER, // Refers to an identifier. Uses the `code_marker` value.
  KH_LEXER_TOKEN_TYPE_UNUM,
  KH_LEXER_TOKEN_TYPE_FLOAT,
  KH_LEXER_TOKEN_TYPE_STRING,
  KH_LEXER_TOKEN_TYPE_STRING_INTRP,
} kh_lexer_token_type;

typedef struct _kh_lexer_ll_parse_result {
  kh_lexer_ll_parse_result_value value;
} kh_lexer_ll_parse_result;

kh_lexer_token_type kh_ll_lexer_parse(const kh_utf8 * code, kh_sz size, kh_lexer_ll_parse_result * out_result);

typedef enum _kh_lexer_context_status {
  KH_LEXER_CONTEXT_STATUS_ = 0,

  KH_LEXER_CONTEXT_STATUS_WARNING = 0x40, // WARNING FLAG using 2nd most significant bit (0100_0000)

  KH_LEXER_CONTEXT_STATUS_FAILURE = 0x80, // FAILURE FLAG using the most significant bit (1000_0000)
} kh_lexer_context_status;

#define KH_LEXER_CONTEXT_STATUS_DECODE_FLAG 0x3F // Decode mask flag (0011_1111)

typedef struct _kh_lexer_context {
  kh_refobji _code_buffer;
  kh_u32     _code_index; // Current index of the code being lexed.
  kh_u32     _code_size;  // Size of code buffer in size
                      
  kh_u32     line;
  kh_u32     column;
} kh_lexer_context;

kh_bool kh_lexer_context_init(kh_lexer_context * ctx, kh_refobji code, kh_sz code_size);

kh_bool kh_lexer_context_destroy(kh_lexer_context * ctx);

kh_bool kh_lexer_parse_next_token(kh_lexer_context * ctx, kh_lexer_ll_parse_result * out_result);


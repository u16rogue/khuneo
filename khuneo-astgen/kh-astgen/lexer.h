#pragma once

#include <kh-core/types.h>

typedef enum _kh_token_type {
  KH_TOK_INVALID,
  KH_TOK_IDENT,
  KH_TOK_KEYWORD,
  KH_TOK_STRING,
  KH_TOK_CHARSYM,
} kh_token_type;

typedef enum _kh_keyword {
  KH_KW_INVALID,
  KH_KW_NULL,
  KH_KW_UNDEFINED,
  KH_KW_IF,
  KH_KW_ELSE,
  KH_KW_LET,
  KH_KW_CONST,
} kh_keyword;

typedef struct _kh_lexer_token_entry {

  kh_token_type type;

#if defined(KH_TRACK_LINE_COLUMN)
  int line;
  int column;
#endif

} kh_lexer_token_entry;

typedef enum _kh_lexer_status {
  KH_LEXER_STATUS_OK,
  KH_LEXER_STATUS_UNKERR,
  KH_LEXER_STATUS_INVALID_UTF8,
  KH_LEXER_STATUS_NO_LEX_MATCH,
} kh_lexer_status;

typedef struct _kh_lexer_run_context {

  kh_lexer_status status;

  kh_lexer_token_entry * token_buffer;      // Pointer to a buffer
  kh_sz                  token_buffer_size; // TOTAL size of the buffer in SIZE (not count)
  kh_sz                  itoken_buffer;     // Token buffer index (in bytes)

  const kh_utf8 * src;      // Source
  kh_sz           src_size; // Source length (in bytes)
  kh_sz           isrc;     // Source index

#if defined(KH_TRACK_LINE_COLUMN)
  kh_u32 line;
  kh_u32 column;
#endif

} kh_lexer_run_context;

typedef enum _kh_lexer_response {
  // Lexer finished parsing the tokens
  KH_RESP_LEXER_OK,

  // Lexer failure (must abort)
  KH_RESP_LEXER_ERROR,

  // Provided buffer in context has been exhausted, replace the buffer with a new one and call lexer again
  // with the same context to continue. The memory reallocation is left to the caller. You can either create a new buffer
  // or realloc the same memory then just set token_buffer at the start of the extended memory and setting token_buffer_size
  // to the extended memory size keeping the entire memory block contiguous
  KH_RESP_LEXER_BUFFER_EXHAUSTED, 

} kh_lexer_response;

kh_lexer_response kh_lexer(kh_lexer_run_context * ctx);

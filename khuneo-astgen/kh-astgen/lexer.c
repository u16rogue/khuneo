#include "lexer.h"
#include <kh-core/utf8.h>

// ---------------------------------------------------------------------------------------------------- 

static const kh_utf8 tok_charsyms[] = {
  '+',
  '-',
  '*',
  '/',
  '#',
  '\\',
  '|',
  '&',
  '=',
  '@',
  '!',
  ':',
};

static const kh_utf8 tok_charsyms_pair[][2] = {
  { '(', ')' },
  { '{', '}' },
  { '[', ']' },
  { '<', '>' },
};

static const kh_utf8 tok_stringcont[] = {
  '\'',
  '`',
  '"'
};

// ---------------------------------------------------------------------------------------------------- 

#if defined(KH_TRACK_LINE_COLUMN)
  #define KH_HLP_ADD_COLUMN(x) ctx->column += x
#else
  #define KH_HLP_ADD_COLUMN(x)
#endif

static kh_bool is_src_end(kh_lexer_run_context * ctx, kh_u32 offset) {
  return (ctx->isrc + offset) >= ctx->src_size;
}

static kh_lexer_token_entry * acquire_entry(kh_lexer_run_context * ctx) {
  kh_sz new_index = ctx->itoken_buffer + sizeof(kh_lexer_run_context);
  if (new_index >= ctx->token_buffer_size)
    return 0;

  kh_lexer_token_entry * new_entry = (kh_lexer_token_entry *)((kh_u8 *)ctx->token_buffer + ctx->itoken_buffer);
  ctx->itoken_buffer = new_index;
  return new_entry;
}

// ---------------------------------------------------------------------------------------------------- 

typedef enum _kh_lex_resp {
  KH_LEX_MATCH,
  KH_LEX_PASS,
  KH_LEX_ABORT, // [10/04/2023] When returning an abort status it's recommended to set ctx->status to let the caller know what happened. or dont. (you can use KH_LEXER_STATUS_UNKERR)
} kh_lex_resp;

static kh_lex_resp lex_whitespace(kh_lexer_run_context * ctx) {
  switch (ctx->src[ctx->isrc]) {
    case ' ':
      KH_HLP_ADD_COLUMN(1);
      break;
    case '\r':
#if defined(KH_TRACK_LINE_COLUMN)
      ctx->column = 1;
#endif
      break;
    case '\n':
#if defined(KH_TRACK_LINE_COLUMN)
      ctx->column = 1;
      ++ctx->line;
#endif
      break;
    case '\t':
#if defined(KH_TRACK_LINE_COLUMN)
#if !defined(KH_TAB_SPACE_COUNT)
#error "khuneo > astgen > lexer > KH_TRACK_LINE_COLUMN is enabled please define KH_TAB_SPACE_COUNT to a numerical value to represent the equivalent space count of a tab character."
#endif
      ctx->column += KH_TAB_SPACE_COUNT;
#endif
      break;
    default:
      return KH_LEX_PASS;
  }

  ++ctx->isrc;
  return KH_LEX_MATCH;
}

static kh_lex_resp lex_comments(kh_lexer_run_context * ctx) {
  if (ctx->src[ctx->isrc] != '/' || is_src_end(ctx, 1))
    return KH_LEX_PASS;

  kh_utf8 cnext = ctx->src[ctx->isrc + 1];
  kh_bool single_line = cnext == '/';
  if (!single_line && cnext != '*')
    return KH_LEX_PASS;

  ctx->isrc += 2;
  KH_HLP_ADD_COLUMN(2);

  if (single_line) {
    while (!is_src_end(ctx, 0)) {
      if (ctx->src[ctx->isrc] == '\n') {
        ++ctx->isrc;
        KH_HLP_ADD_COLUMN(1);
        break;
      }

      // [11/04/2023] TODO: not necessary but it would be good to check if it responds with a KH_LEX_ABORT
      if (lex_whitespace(ctx) == KH_LEX_MATCH) { // [11/04/2023] NOTE: lex_whitespace already advances ctx->isrc that's why we cant just continue
        continue;
      }

      kh_sz char_sz = kh_utf8_char_len(ctx->src[ctx->isrc]);
      if (char_sz == -1) {
        ctx->status = KH_LEXER_STATUS_INVALID_UTF8;
        return KH_LEX_ABORT;
      }
      ctx->isrc += char_sz;
      KH_HLP_ADD_COLUMN(1);
    }
  } else { // Multiline comments
    while (!is_src_end(ctx, 1)) {
      if ( *(const kh_u16 *)&ctx->src[ctx->isrc] == *(const kh_u16 *)"*/" ) {
        ctx->isrc += 2;
        KH_HLP_ADD_COLUMN(2);
        break;
      }

      // [11/04/2023] TODO: not necessary but it would be good to check if it responds with a KH_LEX_ABORT
      if (lex_whitespace(ctx) == KH_LEX_MATCH) { // [11/04/2023] NOTE: lex_whitespace already advances ctx->isrc that's why we cant just continue
        continue;
      }

      kh_sz char_sz = kh_utf8_char_len(ctx->src[ctx->isrc]);
      if (char_sz == -1) {
        ctx->status = KH_LEXER_STATUS_INVALID_UTF8;
        return KH_LEX_ABORT;
      }
      ctx->isrc += char_sz;
      KH_HLP_ADD_COLUMN(1);
    }
  }

  return KH_LEX_MATCH;
}

static kh_lex_resp lex_charsymbols(kh_lexer_run_context * ctx) {
  return KH_LEX_PASS;
}

static kh_lex_resp lex_keywords(kh_lexer_run_context * ctx) {
  return KH_LEX_PASS;
}

static kh_lex_resp lex_identifiers(kh_lexer_run_context * ctx) {
  return KH_LEX_PASS;
}

static kh_lex_resp lex_numbers(kh_lexer_run_context * ctx) {
  return KH_LEX_PASS;
}

typedef kh_lex_resp(*lexer_cb_t)(kh_lexer_run_context *);

static const lexer_cb_t lexers[] = {
  lex_whitespace,
  lex_comments,
  lex_keywords,
  lex_identifiers,
  lex_numbers,
};

// ---------------------------------------------------------------------------------------------------- 

kh_lexer_response kh_lexer(kh_lexer_run_context * ctx) {
  const int nlexers = sizeof(lexers) / sizeof(void *);
  kh_lex_resp resp = KH_LEX_ABORT;

  while (!is_src_end(ctx, 0)) {

    for (int i = 0; i < nlexers; ++i) {
      resp = lexers[i](ctx);
      if (resp == KH_LEX_PASS) {
        continue;
      } else if (resp == KH_LEX_ABORT) {
        return KH_RESP_LEXER_ERROR; // [10/04/2023] We dont set ctx->status as the lexer callbacks might've set it
      } else if (resp == KH_LEX_MATCH) {
        break;
      } else {
        // [10/04/2023] TODO: maybe throw an error? this should never be reachable
      }
      // [10/04/2023] This too should not reachable
    }

    // [10/04/2023] Abort if its still a pass which indicates no lexer matches
    if (resp == KH_LEX_PASS) {
      ctx->status = KH_LEXER_STATUS_NO_LEX_MATCH;
      return KH_RESP_LEXER_ERROR;
    }

    // [10/04/2023] Under the assumption that the state is OK and we had a match
  }
  return KH_RESP_LEXER_OK;
}

#undef KH_HLP_ADD_COLUMN

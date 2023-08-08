#include <kh-astgen/lexer.h>
#include <kh-core/utilities.h>

#include <kh-core/refobj.h>
#include <kh-core/types.h>

#include "lexers.h"

// -------------------------------------------------- 

struct lmp_entry {
  enum kh_lexer_token_type( * const lexer_matcher)(const kh_utf8 * const code, const kh_sz size, struct kh_lexer_ll_parse_result * out_result);
};

static const struct lmp_entry lex_matchers[] = {
  { lmp_symbols,     },
  { lmp_identifiers, },
  { lmp_string,      },
  { lmp_number,      },
};

enum kh_lexer_token_type kh_ll_lexer_parse_type(const kh_utf8 * code, kh_sz size, struct kh_lexer_ll_parse_result * out_result) {
  const kh_u8 n_lmp = kh_narray(lex_matchers);
  for (kh_u8 i_lmp = 0; i_lmp < n_lmp; ++i_lmp) {
    // [31/07/2023] Upon throw of non ok status, return the lexer match type that threw it for possibly diagnostic reasons
    const enum kh_lexer_token_type result = lex_matchers[i_lmp].lexer_matcher(code, size, out_result);
    if (result != KH_LEXER_TOKEN_TYPE_NONE || out_result->status != KH_LEXER_STATUS_OK) { 
      return result;
    }
  }
  return KH_LEXER_TOKEN_TYPE_NONE;
}

static const kh_utf8 group_matchers[][2] = {
  { '(', ')' },
  { '[', ']' },
  { '{', '}' },
};

enum kh_lexer_token_type kh_ll_lexer_parse_group(const kh_utf8 * code, kh_sz size, struct kh_lexer_ll_parse_result * out_result) {
  for (int i = 0; i < (int)kh_narray(group_matchers); ++i) {
    const kh_utf8 * group = group_matchers[i];
    if (code[0] != group[0]) {
      continue;
    }

    const kh_utf8 delim_start = group[0];
    const kh_utf8 delim_end   = group[1];
    const kh_utf8 * current   = code + 1;
    const kh_utf8 * const end = code + size;

    kh_sz scope = 0; 

    while (KH_TRUE) {
      if (current >= end) {
        out_result->status = KH_LEXER_STATUS_SYNTAX_ERROR;
        return KH_LEXER_TOKEN_TYPE_GROUP;
      }

      if (current[0] == delim_start) {
        ++scope;
      } else if (current[0] == delim_end) {
        if (scope == 0) {
          out_result->value.marker.size = current - code + 1;
          return KH_LEXER_TOKEN_TYPE_GROUP;
        } else {
          --scope;
        }
      }

      ++current;
    }
  }

  return KH_LEXER_TOKEN_TYPE_NONE;
}

// -------------------------------------------------- 

kh_bool kh_lexer_context_init(struct kh_lexer_context * ctx, kh_refobji code, kh_sz code_size) {
  ctx->_code_index = 0;
  ctx->_code_size  = code_size;

  if (!kh_refobj_imove(&code, &ctx->_code_buffer)) {
    return KH_FALSE;
  }

  return KH_TRUE;
}

kh_bool kh_lexer_context_uninit(struct kh_lexer_context * ctx) {
  if (kh_refobj_iremove(&ctx->_code_buffer) == KH_FALSE) {
    return KH_FALSE;
  }

  ctx->_code_size  = 0;
  ctx->_code_index = 0;

  return KH_TRUE;
}

static kh_bool is_whitespace(const kh_utf8 c) {
  switch (c) {
    case ' ':
    case '\n':
    case '\t':
    case '\r':
      return KH_TRUE;
  }

  return KH_FALSE;
}

static kh_bool is_comment_start(const kh_utf8 * const code) {
  return (code[0] == '/' && (code[1] == '*' || code[1] == '/') ) ? KH_TRUE : KH_FALSE;
}

enum kh_lexer_token_type kh_lexer_context_parse_next(struct kh_lexer_context * ctx, struct kh_lexer_ll_parse_result * out_result) {
  const kh_utf8 * const code = (const kh_utf8 *)kh_refobj_get_object(ctx->_code_buffer);

  // Whitespaces
  while (KH_TRUE) {
    if (ctx->_code_index >= ctx->_code_size) {
      return KH_LEXER_TOKEN_TYPE_NONE;
    }

    if (is_whitespace(code[ctx->_code_index])) {
      ++ctx->_code_index;
      continue;
    } else {
      break;
    }
  }

  // Comments
  while (ctx->_code_index + 1 < ctx->_code_size && is_comment_start(&code[ctx->_code_index])) {
    const kh_bool is_multi = code[ctx->_code_size + 1] == '*' ? KH_TRUE : KH_FALSE;
    ctx->_code_index += 2;

    while (KH_TRUE) {
      if (ctx->_code_index > ctx->_code_size) {
        return KH_LEXER_TOKEN_TYPE_NONE;
      }

      if (
        //(is_multi && ctx->_code_index + 1 && *(const kh_u16 *)(code + ctx->_code_index) == *(const kh_u16 *)"*/")
        //||
        (!is_multi && code[ctx->_code_index] == '\n')
      ) {
        ctx->_code_index += is_multi ? 2 : 1;
        if (ctx->_code_index >= ctx->_code_size) {
          return KH_LEXER_TOKEN_TYPE_NONE;
        } else {
          break;
        }
      }

      ++ctx->_code_index;
    }
  }

  enum kh_lexer_token_type type = kh_ll_lexer_parse_type(code + ctx->_code_index, ctx->_code_size - ctx->_code_index, out_result);
  if (out_result->status != KH_LEXER_STATUS_OK) {
    return type;
  }

  switch (type) {
    case KH_LEXER_TOKEN_TYPE_NONE:
      break;
    case KH_LEXER_TOKEN_TYPE_SYMBOL:
      ++ctx->_code_index;
      break;
    case KH_LEXER_TOKEN_TYPE_GROUP:
    case KH_LEXER_TOKEN_TYPE_IDENTIFIER:
    case KH_LEXER_TOKEN_TYPE_NUMBER:
    case KH_LEXER_TOKEN_TYPE_STRING:
    case KH_LEXER_TOKEN_TYPE_STRING_INTRP:
    case KH_LEXER_TOKEN_TYPE_STRING_FXHSH:
      out_result->value.marker.offset = ctx->_code_index;
      ctx->_code_index += out_result->value.marker.size;
      break;
  }

  return type;
}

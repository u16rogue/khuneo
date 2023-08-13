#include <kh-astgen/lexer.h>
#include <kh-core/utilities.h>

#include <kh-core/refobj.h>
#include <kh-core/types.h>

#include "lexers.h"

// -------------------------------------------------- 

struct lmp_entry {
  enum kh_lexer_status( * const lexer_matcher)(const kh_utf8 * const, const kh_sz, struct kh_lexer_parse_result *, kh_sz *);
};

static const struct lmp_entry lex_matchers[] = {
  { lmp_whitespace,  },
  { lmp_comments,    },
  { lmp_symbols,     },
  { lmp_identifiers, },
  { lmp_string,      },
  { lmp_number,      },
};

enum kh_lexer_status kh_ll_lexer_parse_type(const kh_utf8 * code, kh_sz size, struct kh_lexer_parse_result * out_result, kh_sz * out_nconsume) {
  const kh_u8 n_lmp = kh_narray(lex_matchers);
  for (kh_u8 i_lmp = 0; i_lmp < n_lmp; ++i_lmp) {
    // [31/07/2023] Upon throw of non ok status, return the lexer match type that threw it for possibly diagnostic reasons
    const enum kh_lexer_status status = lex_matchers[i_lmp].lexer_matcher(code, size, out_result, out_nconsume);
    if (status == KH_LEXER_STATUS_MATCH || status != KH_LEXER_STATUS_PASS) {
      return status;
    }
  }
  return KH_LEXER_STATUS_NOMATCH;
}

static const kh_utf8 group_matchers[][2] = {
  { '(', ')' },
  { '[', ']' },
  { '{', '}' },
};

enum kh_lexer_status kh_ll_lexer_parse_group(const kh_utf8 * code, kh_sz size, struct kh_lexer_parse_result * out_result, kh_sz * out_nconsume) {
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
        out_result->type = KH_LEXER_TOKEN_TYPE_GROUP;
        return KH_LEXER_STATUS_SYNTAX_ERROR;
      }

      if (current[0] == delim_start) {
        ++scope;
      } else if (current[0] == delim_end) {
        if (scope == 0) {
          *out_nconsume = out_result->value.marker.size = current - code + 1;
          out_result->type = KH_LEXER_TOKEN_TYPE_GROUP;
          return KH_LEXER_STATUS_MATCH;
        } else {
          --scope;
        }
      }

      ++current;
    }
  }

  return KH_LEXER_STATUS_NOMATCH;
}

enum kh_lexer_status kh_ll_lexer_context_determine(struct kh_lexer_context * ctx, struct kh_lexer_parse_result * out_result, kh_sz * out_nconsume) {
  if (ctx->_code_index >= ctx->_code_size) {
    return KH_LEXER_STATUS_EOB;
  }

  const kh_utf8 * const code = ((const kh_utf8 *)kh_refobj_get_object(ctx->_code_buffer)) + ctx->_code_index;
  const kh_sz           size = ctx->_code_size - ctx->_code_index;

  enum kh_lexer_status status = kh_ll_lexer_parse_group(code, size, out_result, out_nconsume);
  if (status != KH_LEXER_STATUS_NOMATCH) {
    return status;
  }

  status = kh_ll_lexer_parse_type(code, size, out_result, out_nconsume);
  return status;
}

enum kh_lexer_status kh_ll_lexer_context_apply(struct kh_lexer_context * ctx, struct kh_lexer_parse_result * out_result, kh_sz nconsume) {
  switch (out_result->type) {
    case KH_LEXER_TOKEN_TYPE_NONE:
    case KH_LEXER_TOKEN_TYPE_SYMBOL:
    case KH_LEXER_TOKEN_TYPE_UNUM:
    case KH_LEXER_TOKEN_TYPE_IFLT:
      break;
    case KH_LEXER_TOKEN_TYPE_WHITESPACE:
    case KH_LEXER_TOKEN_TYPE_COMMENT:
    case KH_LEXER_TOKEN_TYPE_GROUP:
    case KH_LEXER_TOKEN_TYPE_IDENTIFIER:
    case KH_LEXER_TOKEN_TYPE_STRING:
    case KH_LEXER_TOKEN_TYPE_STRING_INTRP:
    case KH_LEXER_TOKEN_TYPE_STRING_FXHSH:
      out_result->value.marker.offset = ctx->_code_index;
      break;
  }

  ctx->_code_index += nconsume;
  if (ctx->_code_index > ctx->_code_size) {
    return KH_LEXER_STATUS_CODE_PARSE_OVERFLOW;
  }

  return KH_LEXER_STATUS_MATCH;
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

//==================================================================================================== 

enum kh_lexer_status kh_lexer_context_parse_next(struct kh_lexer_context * ctx, struct kh_lexer_parse_result * out_result) {
  kh_sz nconsume = 0;
  enum kh_lexer_status status = kh_ll_lexer_context_determine(ctx, out_result, &nconsume);
  if (status != KH_LEXER_STATUS_MATCH) {
    return status;
  }
  return kh_ll_lexer_context_apply(ctx, out_result, nconsume);
}

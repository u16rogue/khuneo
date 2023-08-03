#include <kh-astgen/lexer.h>
#include <kh-core/utilities.h>

#include "kh-core/refobj.h"
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

enum kh_lexer_token_type kh_ll_lexer_parse(const kh_utf8 * code, kh_sz size, struct kh_lexer_ll_parse_result * out_result) {
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

// -------------------------------------------------- 

kh_bool kh_lexer_context_init(struct kh_lexer_context * ctx, kh_refobji code, kh_sz code_size) {
  ctx->_code_index = 0;
  ctx->_code_size  = code_size;

  if (!kh_refobj_imove(&code, &ctx->_code_buffer)) {
    return KH_FALSE;
  }

  return KH_TRUE;
}

kh_bool kh_lexer_context_destroy(struct kh_lexer_context * ctx) {
  if (kh_refobj_iremove(&ctx->_code_buffer) == KH_FALSE) {
    return KH_FALSE;
  }

  ctx->_code_size  = 0;
  ctx->_code_index = 0;

  return KH_TRUE;
}

enum kh_lexer_token_type kh_lexer_context_parse_next(struct kh_lexer_context * ctx, struct kh_lexer_ll_parse_result * out_result) {
  (void)ctx;
  (void)out_result;
  return KH_FALSE;
}

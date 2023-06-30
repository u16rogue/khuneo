#include <kh-astgen/lexer.h>
#include <kh-core/utilities.h>

#include "kh-core/refobj.h"
#include "lexers.h"

// -------------------------------------------------- 

struct lmp_entry {
  kh_bool(* const lexer_matcher)(const kh_utf8 * code, kh_sz size, kh_lexer_ll_parse_result * out_result);
  const kh_lexer_token_type resulting_type;
};

static const struct lmp_entry lex_matchers[] = {
  {    lmp_symbols,      KH_LEXER_TOKEN_TYPE_SYMBOL },
  { lmp_identifiers, KH_LEXER_TOKEN_TYPE_IDENTIFIER },
};

kh_lexer_token_type kh_ll_lexer_parse(const kh_utf8 * code, kh_sz size, kh_lexer_ll_parse_result * out_result) {
  const kh_u8 n_lmp = kh_narray(lex_matchers);
  for (kh_u8 i_lmp = 0; i_lmp < n_lmp; ++i_lmp) {
    if (lex_matchers[i_lmp].lexer_matcher(code, size, out_result) == KH_TRUE) {
      return lex_matchers[i_lmp].resulting_type;
    }
  }
  return KH_LEXER_TOKEN_TYPE_NONE;
}

// -------------------------------------------------- 

kh_bool kh_lexer_context_init(kh_lexer_context * ctx, kh_refobji code, kh_sz code_size) {
  ctx->_code_index = 0;
  ctx->_code_size  = code_size;
  ctx->line        = 1;
  ctx->column      = 1;

  if (!kh_refobj_imove(&ctx->_code_buffer, &code)) {
    return KH_FALSE;
  }

  return KH_TRUE;
}

kh_bool kh_lexer_context_destroy(kh_lexer_context * ctx) {
  if (kh_refobj_iremove(&ctx->_code_buffer) == KH_FALSE) {
    return KH_FALSE;
  }

  ctx->_code_size  = 0;
  ctx->_code_index = 0;
  ctx->line        = 0;
  ctx->column      = 0;

  return KH_TRUE;
}

kh_bool kh_lexer_parse_next_token(kh_lexer_context * ctx, kh_lexer_ll_parse_result * out_result) {
  (void)ctx;
  (void)out_result;
  return KH_FALSE;
}

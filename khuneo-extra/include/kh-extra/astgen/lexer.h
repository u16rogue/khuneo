#pragma once
#if defined(KH_EXTRA_ASTGEN) && KH_EXTRA_ASTGEN == 1
#include <kh-astgen/lexer.h>

const char * kh_extra_lexer_tostr_ctx_status(enum kh_lexer_status status);
const char * kh_extra_lexer_tostr_token_type(enum kh_lexer_token_type type);

kh_bool kh_extra_lexer_tostr_result_value(
  const struct kh_utf8sp * const in_raw_code,
  const struct kh_lexer_parse_result * const in_result,
  struct kh_utf8sp_s * const out_value_str
);

kh_sz kh_extra_lexer_predict_tostr_result_value_size(const struct kh_lexer_parse_result * const in_result);

#endif // KH_EXTRA_ASTGEN

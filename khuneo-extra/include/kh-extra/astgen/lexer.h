#pragma once
#if defined(KH_EXTRA_ASTGEN) && KH_EXTRA_ASTGEN == 1
#include <kh-astgen/lexer.h>

const char * kh_extra_lexer_tostr_ctx_status(enum kh_lexer_status status);
const char * kh_extra_lexer_tostr_token_type(enum kh_lexer_token_type type);

#endif // KH_EXTRA_ASTGEN

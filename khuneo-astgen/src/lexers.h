#pragma once

#include <kh-astgen/lexer.h>
#include <kh-core/types.h>

kh_bool lmp_symbols     (const kh_utf8 * code, kh_sz size, struct kh_lexer_ll_parse_result * out_result);
kh_bool lmp_identifiers (const kh_utf8 * code, kh_sz size, struct kh_lexer_ll_parse_result * out_result);
kh_bool lmp_string      (const kh_utf8 * code, kh_sz size, struct kh_lexer_ll_parse_result * out_result);

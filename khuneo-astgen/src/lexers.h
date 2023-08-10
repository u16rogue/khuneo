#pragma once

#include <kh-astgen/lexer.h>
#include <kh-core/types.h>

enum kh_lexer_status lmp_symbols     (const kh_utf8 * const code, const kh_sz size, struct kh_lexer_parse_result * out_result);
enum kh_lexer_status lmp_identifiers (const kh_utf8 * const code, const kh_sz size, struct kh_lexer_parse_result * out_result);
enum kh_lexer_status lmp_string      (const kh_utf8 * const code, const kh_sz size, struct kh_lexer_parse_result * out_result);
enum kh_lexer_status lmp_number      (const kh_utf8 * const code, const kh_sz size, struct kh_lexer_parse_result * out_result);

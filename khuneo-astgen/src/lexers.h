#pragma once

#include <kh-astgen/lexer.h>
#include <kh-core/types.h>

enum kh_lexer_status lmp_comments    (const struct kh_utf8sp * const codesp, struct kh_lexer_parse_result * out_result, kh_sz * out_nconsume);
enum kh_lexer_status lmp_whitespace  (const struct kh_utf8sp * const codesp, struct kh_lexer_parse_result * out_result, kh_sz * out_nconsume);
enum kh_lexer_status lmp_symbols     (const struct kh_utf8sp * const codesp, struct kh_lexer_parse_result * out_result, kh_sz * out_nconsume);
enum kh_lexer_status lmp_identifiers (const struct kh_utf8sp * const codesp, struct kh_lexer_parse_result * out_result, kh_sz * out_nconsume);
enum kh_lexer_status lmp_string      (const struct kh_utf8sp * const codesp, struct kh_lexer_parse_result * out_result, kh_sz * out_nconsume);
enum kh_lexer_status lmp_number      (const struct kh_utf8sp * const codesp, struct kh_lexer_parse_result * out_result, kh_sz * out_nconsume);

#pragma once

#include <kh-core/types.h>
#include <kh-astgen/common.h>
#include <kh-astgen/lexer.h>

void dummy_0(void);

void kh_ll_parser_identify_tokens(
  const struct kh_lexer_parse_result * const tokens,
  const kh_sz ntoken
);

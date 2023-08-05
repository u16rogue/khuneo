#include "lexers.h"
#include <kh-astgen/lexer.h>
#include <kh-core/types.h>
#include <kh-core/utilities.h>
#include <kh-core/utf8.h>

enum kh_lexer_token_type lmp_symbols(const kh_utf8 * const code, const kh_sz size, struct kh_lexer_ll_parse_result * out_result) {
  (void)size;

  const kh_utf8 symbols[] = {
    '!',  /* 33i8 */
    '#',  /* 35i8 */
    '%',  /* 37i8 */
    '&',  /* 38i8 */
  };

  const kh_u8 symbol_ranges[][2] = {
    { '(', '/' }, /* ')', '*', '+', ',', '-', '.', */ /* 40  ~  47 */
    { ':', '@' }, /* ';', '<', '=', '>', '?', */      /* 58  ~  64 */
    { '[', '^' }, /* '\', ']',  */                    /* 91  ~  94 */
    { '{', '~' }, /* '|', '}',  */                    /* 123 ~ 126 */
  };

  const kh_utf8 symbol = code[0];

  for (kh_u8 i = 0; i < kh_narray(symbols); ++i) {
    if (symbols[i] == symbol) {
      out_result->value.symbol = symbol;
      return KH_LEXER_TOKEN_TYPE_SYMBOL;
    }
  }

  for (kh_u8 i = 0; i < kh_narray(symbol_ranges); ++i) {
    const kh_u8 * symbol_range = symbol_ranges[i];
    if (symbol_range[0] <= symbol && symbol_range[1] >= code[0]) {
      out_result->value.symbol = symbol;
      return KH_LEXER_TOKEN_TYPE_SYMBOL;
    }
  }

  return KH_LEXER_TOKEN_TYPE_NONE;
}

static kh_bool valid_ident_char(const kh_utf8 c) {
  return (
            kh_utf8_is_alpha(c) == KH_TRUE ||
            kh_utf8_is_num(c)   == KH_TRUE ||
            c == '$'                       ||
            c == '_'
         ) ? KH_TRUE : KH_FALSE;
}

enum kh_lexer_token_type lmp_identifiers(const kh_utf8 * const code, const kh_sz size, struct kh_lexer_ll_parse_result * out_result) {
  if (code[0] != '$' && code[0] != '_' && !kh_utf8_is_alpha(code[0])) {
    return KH_LEXER_TOKEN_TYPE_NONE;
  }

  const kh_utf8 * current   = code + 1;
  const kh_utf8 * const end = code + size;

  while (current < end) {
    const kh_u8 utf_sz = kh_utf8_char_sz(current[0]);
    if (utf_sz == KH_UTF8_INVALID_SZ) {
      out_result->status = KH_LEXER_STATUS_UTF8_INVALID; 
      return KH_LEXER_TOKEN_TYPE_NONE;
    }

    // Only do validation if its ascii, if its utf8 let it pass
    // to allow UTF8 identifiers
    if (utf_sz == 1 && !valid_ident_char(current[0])) {
      break;
    }

    current += utf_sz;
  }

  // Must either hit `end` or be lesser.
  // Going over `end` means we have an invalid UTF8 overflow
  if (current > end) {
    out_result->status = KH_LEXER_STATUS_CODE_PARSE_OVERFLOW;
    return KH_LEXER_TOKEN_TYPE_IDENTIFIER;
  }

  out_result->value.marker.size = current - code;
  
  return KH_LEXER_TOKEN_TYPE_IDENTIFIER;
}

enum kh_lexer_token_type lmp_string(const kh_utf8 * const code, const kh_sz size, struct kh_lexer_ll_parse_result * out_result) {
  struct sym_set {
    const kh_utf8 sym;
    const enum kh_lexer_token_type type;
  };

  const struct sym_set string_symbol[] = {
    { '\'', KH_LEXER_TOKEN_TYPE_STRING       },
    { '`',  KH_LEXER_TOKEN_TYPE_STRING_INTRP },
  };

  for (int i = 0; i < (int)kh_narray(string_symbol); ++i ) {
    const struct sym_set * const set = &string_symbol[i];
    if (code[0] != set->sym) {
      continue;
    }

    const kh_utf8 * current = code + 1;
    const kh_utf8 * end     = code + size;

    while (KH_TRUE) {
      if (current[0] == set->sym && (current - 1)[0] != '\\' ) {
        out_result->value.marker.size = current - code + 1;
        return set->type;
      }
      if (current >= end) {
        out_result->status = KH_LEXER_STATUS_SYNTAX_ERROR; // Missing string token to end
        return set->type;
      }
      ++current;
    }
  }

  return KH_LEXER_TOKEN_TYPE_NONE;
}


enum kh_lexer_token_type lmp_number(const kh_utf8 * const code, const kh_sz size, struct kh_lexer_ll_parse_result * out_result) {
  if (kh_utf8_is_num(code[0]) == KH_FALSE) {
    return KH_LEXER_TOKEN_TYPE_NONE;
  }

  const kh_utf8 * current    = code + 1;
  const kh_utf8 * const end  = code + size;

  while (current < end && kh_utf8_is_num(current[0]) == KH_TRUE) {
    ++current;
  }

  out_result->value.marker.size = current - code;

  return KH_LEXER_TOKEN_TYPE_NUMBER;
}

#include "lexers.h"
#include <kh-astgen/lexer.h>
#include <kh-core/types.h>
#include <kh-core/utilities.h>
#include <kh-core/utf8.h>
#include <kh-core/math.h>

/*
 *  [05/08/2023]
 *  DECISION: On whether lmp_* should verify UTF8 validity.
 *  VERDICT: It shouldn't, to minimize confusion UTF8 validity should
 *  be confirmed somewhere else take an example of
 *  ```
 *  def str = 'hello <invalid utf8 + 3>'; def str2 = 'world';
 *  ```
 *  When the UTF8 byte is parsed we move current by 3 missing `'` which
 *  has the possibility of throwing off the result since it might match the
 *  ending string delimeter much later giving us `'hello <inv>'; def str2 ='`
 *  because the invalid UTF8 byte caused it to skip. This is inpart due to lmp_
 *  doing a lazy UTF8 parsing but by ignoring UTF8 validity we would still properly
 *  match the ending string delimeter giving us `'hello <inv>'` this also makes validation
 *  easier as we have the expected size of a lexed type which in the case of an invalid UTF8
 *  would cause it to overflow not only that but it makes spotting issues easier as we can
 *  visually confirm the range where it occurs.
 *
 */

enum kh_lexer_status lmp_whitespace(const kh_utf8 * const code, const kh_sz size, struct kh_lexer_parse_result * out_result, kh_sz * out_nconsume) {
  if (kh_utf8_is_whitespace(code[0]) == KH_FALSE) {
    return KH_LEXER_STATUS_PASS;
  }

  const kh_utf8 * current = code + 1;
  const kh_utf8 * const end = code + size;

  while (current < end && kh_utf8_is_whitespace(current[0])) {
    ++current;
  }

  out_result->type = KH_LEXER_TOKEN_TYPE_WHITESPACE;
  *out_nconsume = out_result->value.marker.size = current - code;
  return KH_LEXER_STATUS_MATCH;
}

enum kh_lexer_status lmp_comments(const kh_utf8 * const code, const kh_sz size, struct kh_lexer_parse_result * out_result, kh_sz * out_nconsume) {
  if (
      (size <= 1)
      ||
      !(code[0] == '/' && (code[1] == '/' || code[1] == '*'))
  ) {
    return KH_LEXER_STATUS_PASS;
  }

  kh_bool is_single = (code[1] == '/') ? KH_TRUE : KH_FALSE;

  const kh_utf8 * current = code + 2;
  const kh_utf8 * const end = code + size;

  while (KH_TRUE) {
    if (current >= end) {
      break;
    }

    if (
        (is_single == KH_TRUE  && current[0] == '\n')
        ||
        (is_single == KH_FALSE && current + 1 < end && current[0] == '*' && current[1] == '/')
    ) {
      current += (is_single == KH_TRUE) ? 1 : 2;
      break;
    }

    ++current;
  }

  out_result->type = KH_LEXER_TOKEN_TYPE_COMMENT;
  *out_nconsume = out_result->value.marker.size = current - code;
  return KH_LEXER_STATUS_MATCH;
}

enum kh_lexer_status lmp_symbols(const kh_utf8 * const code, const kh_sz size, struct kh_lexer_parse_result * out_result, kh_sz * out_nconsume) {
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
      out_result->type         = KH_LEXER_TOKEN_TYPE_SYMBOL;
      *out_nconsume            = 1;
      return KH_LEXER_STATUS_MATCH;
    }
  }

  for (kh_u8 i = 0; i < kh_narray(symbol_ranges); ++i) {
    const kh_u8 * symbol_range = symbol_ranges[i];
    if (symbol >= symbol_range[0] && symbol <= symbol_range[1]) {
      out_result->value.symbol = symbol;
      out_result->type         = KH_LEXER_TOKEN_TYPE_SYMBOL;
      *out_nconsume            = 1;
      return KH_LEXER_STATUS_MATCH;
    }
  }

  return KH_LEXER_STATUS_PASS;
}

static kh_bool valid_ident_char(const kh_utf8 c) {
  return (
            kh_utf8_is_alpha(c) == KH_TRUE ||
            kh_utf8_is_num(c)   == KH_TRUE ||
            c == '$'                       ||
            c == '_'
         ) ? KH_TRUE : KH_FALSE;
}

enum kh_lexer_status lmp_identifiers(const kh_utf8 * const code, const kh_sz size, struct kh_lexer_parse_result * out_result, kh_sz * out_nconsume) {
  if (code[0] != '$' && code[0] != '_' && !kh_utf8_is_alpha(code[0]) && !kh_utf8_is_utf8_lazy(code[0])) {
    return KH_LEXER_STATUS_PASS;
  } 

  const kh_utf8 * current   = code + 1;
  const kh_utf8 * const end = code + size;

  while (current < end) {
    // Only do validation if its ascii, if its utf8 let it pass
    // to allow UTF8 identifiers
    if (!kh_utf8_is_utf8_lazy(current[0]) && !valid_ident_char(current[0])) {
      break;
    }

    ++current;
  }

  // Must either hit `end` or be lesser.
  // Going over `end` means we have an invalid UTF8 overflow
  if (current > end) {
    out_result->type = KH_LEXER_TOKEN_TYPE_IDENTIFIER;
    return KH_LEXER_STATUS_CODE_PARSE_OVERFLOW;
  }

  out_result->type = KH_LEXER_TOKEN_TYPE_IDENTIFIER;
  *out_nconsume = out_result->value.marker.size = current - code;
  return KH_LEXER_STATUS_MATCH;
}

enum kh_lexer_status lmp_string(const kh_utf8 * const code, const kh_sz size, struct kh_lexer_parse_result * out_result, kh_sz * out_nconsume) {
  struct sym_set {
    const kh_utf8 sym;
    const enum kh_lexer_token_type type;
  };

  const struct sym_set string_symbol[] = {
    { '\'', KH_LEXER_TOKEN_TYPE_STRING       },
    { '`',  KH_LEXER_TOKEN_TYPE_STRING_INTRP },
    { '"',  KH_LEXER_TOKEN_TYPE_STRING_FXHSH },
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
        out_result->type = set->type;
        *out_nconsume = out_result->value.marker.size = current - code + 1;
        return KH_LEXER_STATUS_MATCH;
      }
      if (current >= end) {
        out_result->type = set->type;
        return KH_LEXER_STATUS_SYNTAX_ERROR;
      }

      ++current;
    }
  }

  return KH_LEXER_STATUS_PASS;
}


enum kh_lexer_status lmp_number(const kh_utf8 * const code, const kh_sz size, struct kh_lexer_parse_result * out_result, kh_sz * out_nconsume) {
  kh_bool is_hex = (size >= 3 && code[0] == '0' && code[1] == 'x' && kh_utf8_is_hex(code[2])) ? KH_TRUE : KH_FALSE;
  kh_bool is_num = kh_utf8_is_num(code[0]) == KH_TRUE ? KH_TRUE : KH_FALSE;
  if (
      (is_hex == KH_FALSE && is_num == KH_FALSE)
  ) {
    return KH_LEXER_STATUS_PASS;
  }

  const kh_utf8 * current   = code;
  const kh_utf8 * const end = code + size;
  if (is_hex == KH_TRUE) {
    current += 2;
  }

  kh_i8  floating_point = -1;
  kh_unum tval = 0;

  while (KH_TRUE) {
    if (current >= end) {
      break;
    }

    if (current[0] == '.') {
      if (floating_point != -1 || is_hex == KH_TRUE) {
        out_result->type = is_hex == KH_TRUE ? KH_LEXER_TOKEN_TYPE_UNUM : KH_LEXER_TOKEN_TYPE_IFLT;
        return KH_LEXER_STATUS_SYNTAX_ERROR;
      }
      ++current;
      ++floating_point;
      continue;
    }

    if (is_hex == KH_TRUE && kh_utf8_is_hex(current[0])) {
      tval <<= 4;
      tval |= kh_utf8_hexchar_to_nibble(current[0]);
      ++current;
      continue;
    } else if (is_hex == KH_FALSE && kh_utf8_is_num(current[0])) {
      if (floating_point != -1)
        ++floating_point;
      tval *= 10;
      tval += kh_utf8_char_to_num(current[0]);
      ++current;
      continue;
    } 

    break;
  }

  if (floating_point == -1) {
    out_result->type = KH_LEXER_TOKEN_TYPE_UNUM;
    out_result->value.number.unum = tval;
  } else {
    out_result->type = KH_LEXER_TOKEN_TYPE_IFLT;
    out_result->value.number.iflt = (kh_iflt)tval / kh_math_pow_unum(10, floating_point);
  }

  *out_nconsume = current - code;

  return KH_LEXER_STATUS_MATCH;
}

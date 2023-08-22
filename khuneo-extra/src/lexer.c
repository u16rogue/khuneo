#include <kh-extra/astgen/lexer.h>
#include <kh-extra/helper.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const char * kh_extra_lexer_tostr_ctx_status(enum kh_lexer_status status) {
  KH_EXTRA_SET_OFFSET("KH_LEXER_STATUS_");
  if (status & KH_LEXER_STATUS_EXTENDED) return "<extended>";
  switch (status) {
    KH_EXTRA_CASE_STR(KH_LEXER_STATUS_OK);
    KH_EXTRA_CASE_STR(KH_LEXER_STATUS_MATCH);
    KH_EXTRA_CASE_STR(KH_LEXER_STATUS_PASS);
    KH_EXTRA_CASE_STR(KH_LEXER_STATUS_NOMATCH);
    KH_EXTRA_CASE_STR(KH_LEXER_STATUS_EOB);
    KH_EXTRA_CASE_STR(KH_LEXER_STATUS_UNKNOWN_WARNING);
    KH_EXTRA_CASE_STR(KH_LEXER_STATUS_CODE_PARSE_OVERFLOW);
    KH_EXTRA_CASE_STR(KH_LEXER_STATUS_UNKNOWN_ERROR);
    KH_EXTRA_CASE_STR(KH_LEXER_STATUS_UTF8_INVALID);
    KH_EXTRA_CASE_STR(KH_LEXER_STATUS_SYNTAX_ERROR);

    case KH_LEXER_STATUS_EXTENDED:
      return "<error>";
  }

  return "<invalid>";
}


const char * kh_extra_lexer_tostr_token_type(enum kh_lexer_token_type type) {
  KH_EXTRA_SET_OFFSET("KH_LEXER_TOKEN_TYPE_");
  switch (type) {
    KH_EXTRA_CASE_STR(KH_LEXER_TOKEN_TYPE_NONE);
    KH_EXTRA_CASE_STR(KH_LEXER_TOKEN_TYPE_GROUP);
    KH_EXTRA_CASE_STR(KH_LEXER_TOKEN_TYPE_SYMBOL);
    KH_EXTRA_CASE_STR(KH_LEXER_TOKEN_TYPE_IDENTIFIER);
    KH_EXTRA_CASE_STR(KH_LEXER_TOKEN_TYPE_UNUM);
    KH_EXTRA_CASE_STR(KH_LEXER_TOKEN_TYPE_IFLT);
    KH_EXTRA_CASE_STR(KH_LEXER_TOKEN_TYPE_STRING);
    KH_EXTRA_CASE_STR(KH_LEXER_TOKEN_TYPE_STRING_INTRP);
    KH_EXTRA_CASE_STR(KH_LEXER_TOKEN_TYPE_STRING_FXHSH);
    KH_EXTRA_CASE_STR(KH_LEXER_TOKEN_TYPE_WHITESPACE);
    KH_EXTRA_CASE_STR(KH_LEXER_TOKEN_TYPE_COMMENT);
  }

  return "<invalid>";
}

kh_bool kh_extra_lexer_tostr_result_value(
  const struct kh_utf8sp * const in_raw_code,
  const struct kh_lexer_parse_result * const in_result,
  struct kh_utf8sp_s * const out_value_str
) {
  if (out_value_str->buffer_size < 2) {
    return KH_FALSE;
  }

  switch (in_result->type) {
    case KH_LEXER_TOKEN_TYPE_NONE:
      snprintf(out_value_str->str.buffer, out_value_str->buffer_size, "%s", "<none>");
      out_value_str->str.size = 6;
      break;
    case KH_LEXER_TOKEN_TYPE_SYMBOL:
      snprintf(out_value_str->str.buffer, out_value_str->buffer_size, "%c", in_result->value.symbol);
      out_value_str->str.size = 1;
      break;
    case KH_LEXER_TOKEN_TYPE_UNUM:
      snprintf(out_value_str->str.buffer, out_value_str->buffer_size, "%llu", in_result->value.number.unum);
      out_value_str->str.size = strlen(out_value_str->str.buffer);
      break;
    case KH_LEXER_TOKEN_TYPE_IFLT:
      snprintf(out_value_str->str.buffer, out_value_str->buffer_size, "%f", in_result->value.number.iflt);
      out_value_str->str.size = strlen(out_value_str->str.buffer);
      break;
    case KH_LEXER_TOKEN_TYPE_GROUP:
    case KH_LEXER_TOKEN_TYPE_IDENTIFIER:
    case KH_LEXER_TOKEN_TYPE_STRING:
    case KH_LEXER_TOKEN_TYPE_STRING_INTRP:
    case KH_LEXER_TOKEN_TYPE_STRING_FXHSH:
    case KH_LEXER_TOKEN_TYPE_WHITESPACE:
    case KH_LEXER_TOKEN_TYPE_COMMENT: {
      kh_utf8 * tmp = malloc(in_result->value.marker.size + 1);
      tmp[in_result->value.marker.size] = '\0';
      for (kh_u32 i = 0; i < in_result->value.marker.size; ++i) {
        tmp[i] = in_raw_code->buffer[in_result->value.marker.offset + i];
      }
      snprintf(out_value_str->str.buffer, out_value_str->buffer_size, "%s", (kh_utf8 *)tmp);
      out_value_str->str.size = in_result->value.marker.size;
      free(tmp);
      break;
    }
  }

  return KH_TRUE;
}

kh_sz kh_extra_lexer_predict_tostr_result_value_size(const struct kh_lexer_parse_result * const in_result) {
  switch (in_result->type) {
    case KH_LEXER_TOKEN_TYPE_NONE:
      return sizeof("<none>") - 1;
      break;
    case KH_LEXER_TOKEN_TYPE_SYMBOL:
      return 3;
      break;
    case KH_LEXER_TOKEN_TYPE_UNUM:
    case KH_LEXER_TOKEN_TYPE_IFLT:
      return 32;
      break;
    case KH_LEXER_TOKEN_TYPE_GROUP:
    case KH_LEXER_TOKEN_TYPE_IDENTIFIER:
    case KH_LEXER_TOKEN_TYPE_STRING:
    case KH_LEXER_TOKEN_TYPE_STRING_INTRP:
    case KH_LEXER_TOKEN_TYPE_STRING_FXHSH:
    case KH_LEXER_TOKEN_TYPE_WHITESPACE:
    case KH_LEXER_TOKEN_TYPE_COMMENT:
      return in_result->value.marker.size;
      break;
  }
  return 0;
}

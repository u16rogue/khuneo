#include <kh-gen/lexer.h>

enum kh_LexerInitResponse kh_lexer_init(struct kh_LexerContext * const ctx) {
  (void)ctx;
  return KH_LEXER_INIT_OK;
}

//------------------------------------------------------------------------------

static kh_bool is_alpha_char(const kh_U8Char ch) {
  return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

static kh_bool is_number_char(const kh_U8Char ch) {
  return (ch >= '0' && ch <= '9');
}

static kh_bool is_identifier_char(const kh_U8Char ch) {
  return is_alpha_char(ch) || is_number_char(ch) || ch == '$' || ch == '_';
}

static enum kh_LexerResponse describe_identifier(const kh_U8Char * const chunk, const kh_U8StringSize chunk_range, struct kh_LexerDescribeResult * const described) {
  if (is_number_char(chunk[0]) || !is_identifier_char(chunk[0])) {
    return KH_LEXER_RES_PASS;
  }

  described->type = KH_TOKEN_TYPE_IDENTIFIER;

  kh_u32 i = 1;
  for (; i < chunk_range && is_identifier_char(chunk[i]); ++i) {}
  described->size = i; 

  return KH_LEXER_RES_MATCH;
}

static enum kh_LexerResponse(* const describers[])(const kh_U8Char * const, const kh_U8StringSize, struct kh_LexerDescribeResult * const) = {
  describe_identifier,
};

enum kh_LexerResponse kh_ll_lexer_describe(const kh_U8Char * const chunk, const kh_U8StringSize chunk_range, struct kh_LexerDescribeResult * const described) {
  described->type = KH_TOKEN_TYPE_INVALID;

  for (kh_u8 i = 0; i < kh_array_length(describers); ++i) {
    const enum kh_LexerResponse response = describers[i](chunk, chunk_range, described);
    if (response & KH_LEXER_RES_FAIL) {
      return response;
    }

    if (response == KH_LEXER_RES_MATCH) {
      return KH_LEXER_RES_OK;
    }
  }

  return KH_LEXER_RES_UNDESCRIBED;
}

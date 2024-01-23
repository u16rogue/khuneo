#include <kh-astgen/lexer.h>

enum kh_LexerInitResponse kh_lexer_init(struct kh_LexerContext * const ctx) {
  (void)ctx;
  return KH_LEXER_INIT_OK;
}

//------------------------------------------------------------------------------

typedef enum kh_LexerDescribeResponse(DescriberFn)(const kh_U8Char * const chunk, const kh_U8StringSize chunk_range, struct kh_LexerDescribeResult * const described);

static enum kh_LexerDescribeResponse describe_identifier(const kh_U8Char * const chunk, const kh_U8StringSize chunk_range, struct kh_LexerDescribeResult * const described) {
  KH_UNUSED(chunk);
  KH_UNUSED(chunk_range);
  KH_UNUSED(described);
  return KH_LEXER_DESCRIBE_MISS;
}

static DescriberFn * const describers[] = {
  describe_identifier,
};

enum kh_LexerDescribeResponse kh_ll_lexer_describe(const kh_U8Char * const chunk, const kh_U8StringSize chunk_range, struct kh_LexerDescribeResult * const described) {
  described->type = KH_LEXER_TYPE_INVALID;

  for (kh_u8 i = 0; i < kh_array_length(describers); ++i) {
    const enum kh_LexerDescribeResponse response = describers[i](chunk, chunk_range, described);
    if (response & KH_LEXER_DESCRIBE_FAIL) {
      return response;
    }

    if (response == KH_LEXER_DESCRIBE_MATCH) {
      return KH_LEXER_DESCRIBE_OK;
    }
  }

  return KH_LEXER_DESCRIBE_OK;
}

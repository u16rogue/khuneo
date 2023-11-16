#include <kh-astgen/lexer.h>

enum kh_LexerInitResponse kh_lexer_init(struct kh_LexerContext * ctx) {
  (void)ctx;
  return KH_LEXER_INIT_OK;
}

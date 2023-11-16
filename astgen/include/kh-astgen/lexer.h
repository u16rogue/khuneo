#pragma once

struct kh_LexerContext {
  char _;
};

enum kh_LexerInitResponse {
  KH_LEXER_INIT_OK,
};

enum kh_LexerInitResponse kh_lexer_init(struct kh_LexerContext * ctx);

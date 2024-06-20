#pragma once

#include <kh-common/defs.h>
#include <kh-gen/lexer.h>

struct kh_ParserContext {
  kh_u8 _;
};

struct _kh_stubctx {
  struct kh_LexerDescription * description;
  kh_U8Char * content;
};

enum kh_ParserResult {
  KH_PARSER_RES_OK   = 0,
  KH_PARSER_RES_FAIL = KH_FAIL_BIT,
};

enum kh_ParserResult
kh_parser_start(
  struct kh_ParserContext * ctx
);

enum kh_ParserResult
kh_parser_stop(
  struct kh_ParserContext * ctx
);

enum kh_ParserResult
kh_parser_feed(
  struct kh_ParserContext * ctx,
  struct _kh_stubctx * stub
);

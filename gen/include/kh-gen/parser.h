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
  KH_PARSER_RES_OK   = KH_OK,
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
  struct kh_ParserContext *     ctx,
  struct kh_LexerDescription ** descriptions,
  const  kh_U8Char **           content,
  const kh_u8                   ncount
);

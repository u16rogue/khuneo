#pragma once

#include <kh-common/defs.h>
#include <kh-gen/lexer.h>

struct kh_ParserContext {
  kh_u8 _set_index;
  kh_u8 _ins_index;

  // [02.07.2024 @u16rogue REFACTOR] Removed in favor of just setting `description` to a null pointer
  // signifying that the current description has already been consumed.
  // kh_bool _fed; 
};

struct _kh_stubctx {
  struct kh_LexerDescription * description;
  kh_U8Char * content;
};

enum kh_ParserResult {
  KH_PARSER_RES_OK        = KH_OK,
  KH_PARSER_RES_HUNGRY    = KH_MSG_SET(1),
  KH_PARSER_RES_PASS      = KH_MSG_SET(2),
  KH_PARSER_RES_END       = KH_MSG_SET(3),

  KH_PARSER_RES_FAIL      = KH_FAIL_BIT,
  KH_PARSER_RES_INV_INSTR = KH_FAIL_SET(1),
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
  struct kh_ParserContext *           ctx,
  const struct kh_LexerDescription ** descriptions,
  const  kh_U8Char **                 content,
  const  kh_u8                        ncount
);

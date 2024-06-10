#include <kh-gen/parser.h>

enum kh_ParserResult kh_parser_start(struct kh_ParserContext * ctx) {
  KH_DEBUG_UNUSED(ctx);
  return KH_PARSER_RES_OK;
}

enum kh_ParserResult kh_parser_stop(struct kh_ParserContext * ctx) {
  KH_DEBUG_UNUSED(ctx);
  return KH_PARSER_RES_OK;
}

enum kh_ParserResult kh_parser_feed(struct kh_ParserContext * ctx, struct _kh_stubctx * stub) {
  KH_DEBUG_UNUSED(ctx);
  KH_DEBUG_UNUSED(stub);
  return KH_PARSER_RES_OK;
}

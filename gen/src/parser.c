#include <kh-gen/parser.h>
#include <kh-gen/psm.h>

typedef kh_ParserSM_Instruction Instr;

// (kw:var)(id:*)(?sym:':'(id:*))(?sym:'='(expr))(sym:';')
static const Instr match_var_decl[] = {
  KH_PSM_IMATCH_KW, KH_TOKEN_KEYWORD_VAR,
  KH_PSM_IACQUIRE,
  KH_PSM_IMAYBE_WS,
  KH_PSM_IMATCH_ID,
  KH_PSM_IMAYBE_WS,
  KH_PSM_IMAYBE_SYM, KH_TOKEN_SYMBOL_COL,
    KH_PSM_ISCP_START,
      KH_PSM_IMATCH_ID,
    KH_PSM_ISCP_END,
  KH_PSM_IMAYBE_WS,
};

enum kh_ParserResult kh_parser_start(struct kh_ParserContext * ctx) {
  KH_DEBUG_UNUSED(ctx);
  return KH_PARSER_RES_OK;
}

enum kh_ParserResult kh_parser_stop(struct kh_ParserContext * ctx) {
  KH_DEBUG_UNUSED(ctx);
  return KH_PARSER_RES_OK;
}

enum kh_ParserResult kh_parser_feed(struct kh_ParserContext * ctx, struct kh_LexerDescription ** descriptions, const kh_U8Char ** contents, const kh_u8 ncount) {
  KH_DEBUG_UNUSED(ctx);
  KH_DEBUG_UNUSED(descriptions);
  KH_DEBUG_UNUSED(contents);
  KH_DEBUG_UNUSED(ncount);
  KH_DEBUG_UNUSED(match_var_decl);
  return KH_PARSER_RES_OK;
}

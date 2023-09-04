#include <kh-astgen/parser.h>
#include "kh-astgen/lexer.h"
#include "kh-core/refobj.h"
#include "kh-core/utilities.h"
#include "parsers.h"

typedef enum kh_parser_status(* parse_matches_fn_t)(struct _draft_pmp_args *);
static const parse_matches_fn_t parse_matches[] = {
  pmp_function,
  pmp_declvar,
};

enum kh_parser_status kh_ll_parser_identify_tokens(struct _draft_pmp_args * _args) {
  for (int i = 0; i < (int)kh_narray(parse_matches); ++i) {
    enum kh_parser_status status = parse_matches[i](_args);
    if (status != KH_PARSER_STATUS_PASS) {
      return status;
    }
  }

  return KH_PARSER_STATUS_NOMATCH;
}

kh_bool kh_parser_context_init(struct kh_parser_context * context, kh_refobji code, kh_refobji tokens, const kh_sz ntokens) {
  if (kh_refobj_imove(&code, &context->_code) != KH_TRUE || kh_refobj_imove(&tokens, &context->_tokens) != KH_TRUE) {
    return KH_FALSE;
  }
  context->_ntokens = ntokens;
  context->_index   = 0;
  return KH_TRUE;
}

kh_bool kh_parser_context_uninit(struct kh_parser_context * context) {
  if (kh_refobj_iremove(&context->_code) != KH_TRUE || kh_refobj_iremove(&context->_tokens) != KH_TRUE) {
    return KH_FALSE;
  }
  context->_ntokens = 0;
  context->_index   = 0;
  return KH_TRUE;
}

enum kh_parser_status kh_parser_parse_token_next(struct kh_parser_context * ctx, struct kh_ll_parser_parse_result ** out_result) {
  struct kh_lexer_parse_result * tokens = (struct kh_lexer_parse_result *)kh_refobj_get_object(ctx->_tokens);
  do {
    if (ctx->_index >= ctx->_ntokens)                               return KH_PARSER_STATUS_EOB;
    if (tokens[ctx->_index].type != KH_LEXER_TOKEN_TYPE_WHITESPACE) break;
    ++ctx->_index;
  } while (KH_TRUE);

  kh_sz nconsume = 0;

  struct _draft_pmp_args _args = {
    .out_result   = out_result,
    .raw_code     = kh_refobj_get_object(ctx->_code),
    .tokens       = &tokens[ctx->_index],
    .ntokens      = ctx->_ntokens - ctx->_index,
    .out_nconsume = &nconsume,
  };

  const enum kh_parser_status status = kh_ll_parser_identify_tokens(&_args);
  ctx->_index += nconsume;
  return status;
}

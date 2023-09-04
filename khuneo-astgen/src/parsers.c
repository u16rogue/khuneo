#include "parsers.h"
#include <kh-astgen/common.h>
#include <kh-core/utf8.h>
#include <kh-core/utilities.h>

#define hlp_mk_strp(nm, str)    \
  const struct kh_utf8sp nm = { \
    .size   = sizeof(str) - 1,  \
    .buffer = str,              \
  }

#define hlp_match_marker(marker, str, expect)                                \
  hlp_mk_strp(kh_mglue(__cmp, __LINE__), str);                               \
  if (strcmp_marker(raw_code, marker, &kh_mglue(__cmp, __LINE__)) == expect)

// [18/08/2023] WARNING: This will make the assumption that the marker's offset is within bounds.
static kh_bool strcmp_marker(raw_code_t raw_code, const struct kh_astgen_marker * const marker, const struct kh_utf8sp * const match) {
  // [18/08/2023]
  // Preemptively check the size so we can return early
  // This check is also done in `kh_utf8_strcmp` so we might
  // be doing this twice but much much earlier. TODO: Properly benchmark.
  if (match->size != marker->size) {
    return KH_FALSE;
  }
  struct kh_utf8sp partial;
  partial.buffer = (kh_utf8 *)&raw_code->buffer[marker->offset];
  partial.size   = marker->size;
  return kh_utf8_strcmp(&partial, match);
}

static kh_bool is_tok_comment(tokens_t tokens, kh_sz i) {
  return kh_bool_expr(tokens[i].type == KH_LEXER_TOKEN_TYPE_COMMENT);
}

static kh_bool is_tok_whitespace(tokens_t tokens, kh_sz i) {
  return kh_bool_expr(tokens[i].type == KH_LEXER_TOKEN_TYPE_WHITESPACE);
}

static kh_bool skip_whitespace(tokens_t tokens, ntokens_t ntokens, kh_sz * itoken) {
  const kh_sz old = *itoken;
  while (*itoken < ntokens && is_tok_whitespace(tokens, *itoken) == KH_TRUE) {
    ++(*itoken);
  }

  return kh_bool_expr(old != *itoken);
}

static kh_bool skip_comments(tokens_t tokens, ntokens_t ntokens, kh_sz * itoken) {
  const kh_sz old = *itoken;
  while (*itoken < ntokens && is_tok_comment(tokens, *itoken) == KH_TRUE) {
    ++(*itoken);
  }

  return kh_bool_expr(old != *itoken);
}

static kh_bool skip_wc_expecting(tokens_t tokens, ntokens_t ntokens, kh_sz * itoken) {
  while (skip_comments(tokens, ntokens, itoken) || skip_whitespace(tokens, ntokens, itoken));
  if (*itoken >= ntokens) {
    return KH_FALSE;
  }

  return KH_TRUE;
}

#define m_glob_tokens_till(expr)                                                  \
  while (KH_TRUE) {                                                               \
    if (itoken >= ntokens) return KH_PARSER_STATUS_SYNTAX_ERROR;                  \
    if (tokens[itoken].type == KH_LEXER_TOKEN_TYPE_SYMBOL) {                      \
      const kh_utf8 symbol = tokens[itoken].value.symbol;                         \
      if ((expr)) {                                                               \
        type->value.uneval_group.size = itoken - type->value.uneval_group.offset; \
        break;                                                                    \
      }                                                                           \
    }                                                                             \
    ++itoken;                                                                     \
  }

enum kh_parser_status pmp_declvar(struct _draft_pmp_args * args) {
  if (args->tokens[0].type != KH_LEXER_TOKEN_TYPE_IDENTIFIER) {
    return KH_PARSER_STATUS_PASS;
  }

  // Match keyword 'let' or 'var'
  struct kh_utf8sp kw_m_decl = {
    .size   = 3,
    .buffer = "let",
  };

  const kh_bool is_constant = strcmp_marker(args->raw_code, &args->tokens[0].value.marker, &kw_m_decl);
  if (is_constant == KH_FALSE) {
    kw_m_decl.buffer = "var";
    if (strcmp_marker(args->raw_code, &args->tokens[0].value.marker, &kw_m_decl) == KH_FALSE) {
      return KH_PARSER_STATUS_PASS;
    }
  }

  struct kh_ll_parser_parse_result * const dom = args->out_result[KH_PARSER_NODE_IDX_VARIABLE_DOM];
  dom->type = KH_PARSER_NODE_TYPE_VARIABLE;
  dom->value.variable.flags.is_constant = is_constant == KH_TRUE ? 1 : 0;

  kh_sz itoken = 1;

  // Match variable name
  if (skip_wc_expecting(args->tokens, args->ntokens, &itoken) == KH_FALSE || args->tokens[itoken].type != KH_LEXER_TOKEN_TYPE_IDENTIFIER) return KH_PARSER_STATUS_SYNTAX_ERROR;
  dom->value.variable.name = args->tokens[itoken].value.marker;

  ++itoken;
  if (skip_wc_expecting(args->tokens, args->ntokens, &itoken) == KH_FALSE) return KH_PARSER_STATUS_SYNTAX_ERROR;

  // Consume optional type
  if (args->tokens[itoken].type == KH_LEXER_TOKEN_TYPE_SYMBOL && args->tokens[itoken].value.symbol == ':') {
    struct kh_ll_parser_parse_result * const type = args->out_result[KH_PARSER_NODE_IDX_VARIABLE_TYPE];
    type->type = KH_PARSER_NODE_TYPE_EXPRESSION;
    type->attributes.unevaluated = 1;
    type->value.unevaluated.offset = ++itoken;
    while (KH_TRUE) {
      if (itoken >= args->ntokens) return KH_PARSER_STATUS_SYNTAX_ERROR;
      if (args->tokens[itoken].type == KH_LEXER_TOKEN_TYPE_SYMBOL) {
        const kh_utf8 symbol = args->tokens[itoken].value.symbol; 
        if (symbol == '=' || symbol == ';') {
          type->value.unevaluated.size = itoken - type->value.unevaluated.offset;
          break;
        }
      }
      ++itoken;
    }
  }

  // Consume optional initializer
  if (args->tokens[itoken].type == KH_LEXER_TOKEN_TYPE_SYMBOL && args->tokens[itoken].value.symbol == '=') {
    struct kh_ll_parser_parse_result * const initexpr = args->out_result[KH_PARSER_NODE_IDX_VARIABLE_INITIALIZER];
    initexpr->type = KH_PARSER_NODE_TYPE_EXPRESSION; 
    initexpr->attributes.unevaluated = 1;
    initexpr->value.unevaluated.offset = ++itoken;
    while (KH_TRUE) {
      if (itoken >= args->ntokens) return KH_PARSER_STATUS_SYNTAX_ERROR;
      if (args->tokens[itoken].type == KH_LEXER_TOKEN_TYPE_SYMBOL) {
        const kh_utf8 symbol = args->tokens[itoken].value.symbol; 
        if (symbol == ';') {
          initexpr->value.unevaluated.size = itoken - initexpr->value.unevaluated.offset;
          break;
        }
      }
      ++itoken;
    }
  }

  if (args->tokens[itoken].type == KH_LEXER_TOKEN_TYPE_SYMBOL && args->tokens[itoken].value.symbol == ';') {
    *args->out_nconsume = itoken + 1;
    return KH_PARSER_STATUS_MATCH;
  }

  return KH_PARSER_STATUS_SYNTAX_ERROR;
}

enum kh_parser_status pmp_function(struct _draft_pmp_args * args) {
  if (args->tokens[0].type != KH_LEXER_TOKEN_TYPE_IDENTIFIER) {
    return KH_PARSER_STATUS_PASS;
  }

  const struct kh_utf8sp kw_fn = {
    .size = 2,
    .buffer = "fn",
  };

  // Match keyword 'fn'
  if (strcmp_marker(args->raw_code, &args->tokens[0].value.marker, &kw_fn) == KH_FALSE) {
    return KH_PARSER_STATUS_PASS;
  }

  struct kh_ll_parser_parse_result * const dom = args->out_result[KH_PARSER_NODE_IDX_FUNCTION_DOM];
  dom->type = KH_PARSER_NODE_TYPE_FUNCTION;

  kh_sz itoken = 1;
  if (skip_wc_expecting(args->tokens, args->ntokens, &itoken) == KH_FALSE) return KH_PARSER_STATUS_SYNTAX_ERROR;

  // Match optional function name
  if (args->tokens[itoken].type == KH_LEXER_TOKEN_TYPE_IDENTIFIER) {
    dom->value.function.name = args->tokens[itoken].value.marker;
    ++itoken;
    if (skip_wc_expecting(args->tokens, args->ntokens, &itoken) == KH_FALSE) return KH_PARSER_STATUS_SYNTAX_ERROR;
  } else {
    const struct kh_astgen_marker empty = { .offset = 0, .size = 0 };
    dom->value.function.name = empty;
  }


  // Match optional args
  if (args->tokens[itoken].type == KH_LEXER_TOKEN_TYPE_GROUP && args->raw_code->buffer[args->tokens[itoken].value.marker.offset] == '(') {
    struct kh_ll_parser_parse_result * const fnargs = args->out_result[KH_PARSER_NODE_IDX_FUNCTION_ARGS];
    fnargs->type = KH_PARSER_NODE_TYPE_VARIANT;
    fnargs->attributes.unevaluated = 1;
    fnargs->value.unevaluated.offset = itoken;
    fnargs->value.unevaluated.size = 1;
    ++itoken;
    if (skip_wc_expecting(args->tokens, args->ntokens, &itoken) == KH_FALSE) return KH_PARSER_STATUS_SYNTAX_ERROR;
  }


  // Match optional return type
  if (args->tokens[itoken].type == KH_LEXER_TOKEN_TYPE_SYMBOL && args->tokens[itoken].value.symbol == ':') {
    struct kh_ll_parser_parse_result * const rtype = args->out_result[KH_PARSER_NODE_IDX_FUNCTION_RTYPE];
    rtype->type = KH_PARSER_NODE_TYPE_EXPRESSION;
    rtype->attributes.unevaluated = 1;
    rtype->value.unevaluated.offset = ++itoken;

    while (KH_TRUE) {
      if (++itoken >= args->ntokens) {
        return KH_PARSER_STATUS_SYNTAX_ERROR;
      }

      if (args->tokens[itoken].type == KH_LEXER_TOKEN_TYPE_GROUP && args->raw_code->buffer[args->tokens[itoken].value.marker.offset] == '{') {
        break;
      }
    }

    rtype->value.unevaluated.size = itoken - rtype->value.unevaluated.offset;
    if (skip_wc_expecting(args->tokens, args->ntokens, &itoken) == KH_FALSE) return KH_PARSER_STATUS_SYNTAX_ERROR;
  }

  if (args->tokens[itoken].type != KH_LEXER_TOKEN_TYPE_GROUP || args->raw_code->buffer[args->tokens[itoken].value.marker.offset] != '{') {
    return KH_PARSER_STATUS_SYNTAX_ERROR;
  }

  struct kh_ll_parser_parse_result * const body = args->out_result[KH_PARSER_NODE_IDX_FUNCTION_BODY];
  body->type = KH_PARSER_NODE_TYPE_EXPRESSION;
  body->attributes.unevaluated = 1;
  body->value.unevaluated.offset = itoken;
  body->value.unevaluated.size = 1;

  *args->out_nconsume = itoken + 1;
  return KH_PARSER_STATUS_MATCH;
}

//static enum kh_parser_status pmp_hanging_literal(raw_code_t raw_code, tokens_t tokens, ntokens_t ntokens, out_result_t out_result) {
//  (void)raw_code;
//  switch (tokens[0].type) {
//    case KH_LEXER_TOKEN_TYPE_IDENTIFIER:
//    case KH_LEXER_TOKEN_TYPE_UNUM:
//    case KH_LEXER_TOKEN_TYPE_IFLT:
//    case KH_LEXER_TOKEN_TYPE_STRING:
//      break;
//    default:
//      return KH_PARSER_STATUS_PASS;
//  }
//
//  kh_sz itoken = 1;
//  if (skip_wc_expecting(tokens, ntokens, &itoken) == KH_TRUE)
//    return KH_PARSER_STATUS_PASS;
//
//  switch (tokens[0].type) {
//    case KH_LEXER_TOKEN_TYPE_IDENTIFIER: {
//      out_result->dom->type             = KH_PARSER_NODE_TYPE_IDENTIFIER;
//      out_result->dom->value.identifier = tokens[0].value.marker;
//      break;
//    }
//    case KH_LEXER_TOKEN_TYPE_UNUM:
//    case KH_LEXER_TOKEN_TYPE_IFLT:
//    case KH_LEXER_TOKEN_TYPE_STRING:
//
//    default:
//      return KH_PARSER_STATUS_CONTEXT_ERROR;
//  }
//
//  return KH_PARSER_STATUS_MATCH;
//}

#undef hlp_match_marker
#undef hlp_mk_strp

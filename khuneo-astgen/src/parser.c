#include <kh-astgen/parser.h>
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

typedef const struct kh_utf8sp * const raw_code_t;
typedef const struct kh_lexer_parse_result * const tokens_t;
typedef const kh_sz  ntokens_t;
typedef struct kh_ll_parser_parse_result_nodes * out_result_t;

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

#define glob_tokens_till()

static enum kh_parser_status pmp_declvar(raw_code_t raw_code, tokens_t tokens, ntokens_t ntokens, out_result_t out_result) {
  if (tokens[0].type != KH_LEXER_TOKEN_TYPE_IDENTIFIER) {
    return KH_PARSER_STATUS_PASS;
  }

  struct kh_utf8sp kw_m_decl = {
    .size   = 3,
    .buffer = "let",
  };

  const kh_bool is_constant = strcmp_marker(raw_code, &tokens[0].value.marker, &kw_m_decl);
  if (is_constant == KH_FALSE) {
    kw_m_decl.buffer = "var";
    if (strcmp_marker(raw_code, &tokens[0].value.marker, &kw_m_decl) == KH_FALSE) {
      return KH_PARSER_STATUS_PASS;
    }
  }

  struct kh_ll_parser_parse_result * const dom = out_result->dom;
  dom->type = KH_PARSER_DOM_NODE_TYPE_DECLVAR;
  dom->value.declvar.flags.is_constant = is_constant == KH_TRUE ? 1 : 0;

  kh_sz itoken = 1;

  if (skip_wc_expecting(tokens, ntokens, &itoken) == KH_FALSE || kh_lexer_parse_result_type_is(&tokens[itoken]) != KH_LEXER_TOKEN_TYPE_IDENTIFIER) return KH_PARSER_STATUS_SYNTAX_ERROR;
  dom->value.declvar.name = tokens[itoken].value.marker;

  ++itoken;
  if (skip_wc_expecting(tokens, ntokens, &itoken) == KH_FALSE) return KH_PARSER_STATUS_SYNTAX_ERROR;

  if (tokens[itoken].type == KH_LEXER_TOKEN_TYPE_SYMBOL && tokens[itoken].value.symbol == ':') {
    struct kh_ll_parser_parse_result * const type = out_result->leaf1.type;
    type->type = KH_PARSER_DOM_NODE_TYPE_UNEVAL_GROUP;
    type->value.uneval_group.offset = ++itoken;
    while (KH_TRUE) {
      if (itoken >= ntokens) return KH_PARSER_STATUS_SYNTAX_ERROR;
      if (tokens[itoken].type == KH_LEXER_TOKEN_TYPE_SYMBOL) {
        const kh_utf8 symbol = tokens[itoken].value.symbol; 
        if (symbol == '=' || symbol == ';') {
          type->value.uneval_group.size = itoken - type->value.uneval_group.offset;
          break;
        }
      }
      ++itoken;
    }
  }

  if (tokens[itoken].type == KH_LEXER_TOKEN_TYPE_SYMBOL && tokens[itoken].value.symbol == '=') {
    struct kh_ll_parser_parse_result * const initexpr = out_result->leaf2.initializer;
    initexpr->type = KH_PARSER_DOM_NODE_TYPE_UNEVAL_GROUP; 
    initexpr->value.uneval_group.offset = ++itoken;
    while (KH_TRUE) {
      if (itoken >= ntokens) return KH_PARSER_STATUS_SYNTAX_ERROR;
      if (tokens[itoken].type == KH_LEXER_TOKEN_TYPE_SYMBOL) {
        const kh_utf8 symbol = tokens[itoken].value.symbol; 
        if (symbol == ';') {
          initexpr->value.uneval_group.size = itoken - initexpr->value.uneval_group.offset;
          break;
        }
      }
      ++itoken;
    }
  }

  if (tokens[itoken].type == KH_LEXER_TOKEN_TYPE_SYMBOL && tokens[itoken].value.symbol == ';') {
    return KH_PARSER_STATUS_MATCH;
  }

  return KH_PARSER_STATUS_SYNTAX_ERROR;
}

enum kh_parser_status kh_ll_parser_identify_tokens(
  const struct kh_utf8sp * const raw_code,
  const struct kh_lexer_parse_result * const tokens,
  const kh_sz ntokens,
  struct kh_ll_parser_parse_result_nodes * out_result
) {
  return pmp_declvar(raw_code, tokens, ntokens, out_result);
}

#undef hlp_match_marker
#undef hlp_mk_strp

#pragma once

#include <kh-core/types.h>
#include <kh-astgen/common.h>
#include <kh-astgen/lexer.h>

enum kh_parser_dom_node_type {
  #define KH_PARSER_NODEIDX_DOM 0
  KH_PARSER_DOM_NODE_TYPE_NONE,
  KH_PARSER_DOM_NODE_TYPE_DECLVAR,
  KH_PARSER_DOM_NODE_TYPE_EXPRESSION,
  KH_PARSER_DOM_NODE_TYPE_UNEVAL_GROUP,
};

#define KH_PARSER_CONTEXT_STATUS_FLAG_BITS 0x1F // Decode mask flag (0001_1111)
enum kh_parser_status {
  KH_PARSER_STATUS_OK      = 0,
  KH_PARSER_STATUS_MATCH   = 1,
  KH_PARSER_STATUS_NOMATCH = 2,
  KH_PARSER_STATUS_EOB     = 3,
  KH_PARSER_STATUS_PASS    = 4,

  KH_PARSER_STATUS_WARNING  = 0x20,
                                   // WARNING FLAG using the 3rd most significant bit (0010_0000)- 

  KH_PARSER_STATUS_ERROR    = 0x40,
                                   // ERROR FLAG using the 2nd most significant bit (0100_0000)

  KH_PARSER_STATUS_EXTENDED = 0x80,
                                   // EXTEND FLAG using the most significant bit (1000_0000)

  // NOTE: Max flag bit value is 32

  // -- Warnings
  KH_PARSER_STATUS_UNKNOWN_WARNING = (0 & KH_PARSER_CONTEXT_STATUS_FLAG_BITS) | KH_PARSER_STATUS_WARNING, // Unspecified warning (Yes the or is pointless, its for verbosity)

  // -- Errors
  KH_PARSER_STATUS_UNKNOWN_ERROR   = (0 & KH_PARSER_CONTEXT_STATUS_FLAG_BITS) | KH_PARSER_STATUS_ERROR, // Unspecified error (Yes the or is pointless, its for verbosity)
  KH_PARSER_STATUS_SYNTAX_ERROR    = (1 & KH_PARSER_CONTEXT_STATUS_FLAG_BITS) | KH_PARSER_STATUS_ERROR,
};

struct kh_ll_parser_domnode_t_declvar {
  struct {
    kh_u8 is_constant : 1;
  } flags;
  struct kh_astgen_marker name;
};

struct kh_ll_parser_parse_result {
  enum kh_parser_dom_node_type type;
  union {
    struct kh_ll_parser_domnode_t_declvar declvar;
    struct kh_astgen_marker uneval_group;
  } value;
};

struct kh_ll_parser_parse_result_nodes {
  struct kh_ll_parser_parse_result * dom;

  union {
    struct kh_ll_parser_parse_result * self;
    struct kh_ll_parser_parse_result * type;
  } leaf1;

  union {
    struct kh_ll_parser_parse_result * self;
    struct kh_ll_parser_parse_result * initializer;
  } leaf2;
};

enum kh_parser_status kh_ll_parser_identify_tokens(
  const struct kh_utf8sp * const raw_code,
  const struct kh_lexer_parse_result * const tokens,
  const kh_sz ntokens,
  struct kh_ll_parser_parse_result_nodes * out_result
);

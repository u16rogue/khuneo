#pragma once

#include <kh-core/types.h>
#include <kh-astgen/common.h>
#include <kh-astgen/lexer.h>

enum kh_parser_node_type {
  KH_PARSER_NODE_TYPE_NONE,

  KH_PARSER_NODE_TYPE_VARIABLE,
  #define KH_PARSER_NODE_IDX_VARIABLE_DOM         0
  #define KH_PARSER_NODE_IDX_VARIABLE_INITIALIZER 1
  #define KH_PARSER_NODE_IDX_VARIABLE_TYPE        2

  KH_PARSER_NODE_TYPE_EXPRESSION,
  #define KH_PARSER_NODE_IDX_EXPRESSION_DOM       0

  KH_PARSER_NODE_TYPE_IDENTIFIER,
  #define KH_PARSER_NODE_IDX_IDENTIFIER_DOM       0

  KH_PARSER_NODE_TYPE_LITERAL_UNUM,
  #define KH_PARSER_NODE_IDX_UNUM_DOM             0

  KH_PARSER_NODE_TYPE_FUNCTION,
  #define KH_PARSER_NODE_IDX_FUNCTION_DOM         0
  #define KH_PARSER_NODE_IDX_FUNCTION_BODY        1
  #define KH_PARSER_NODE_IDX_FUNCTION_ARGS        2
  #define KH_PARSER_NODE_IDX_FUNCTION_RTYPE       3

  KH_PARSER_NODE_TYPE_VARIANT, // Denotes that its node type is expressed by its dominant node
};

#define KH_PARSER_NODE_IDX_MAX 3

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
  KH_PARSER_STATUS_CONTEXT_ERROR   = (2 & KH_PARSER_CONTEXT_STATUS_FLAG_BITS) | KH_PARSER_STATUS_ERROR,
};

struct kh_ll_parser_domnode_t_declvar {
  struct {
    kh_u8 is_constant : 1;
  } flags;
  struct kh_astgen_marker name;
};

struct kh_ll_parser_domnode_t_function {
  struct kh_astgen_marker name;
};

struct kh_ll_parser_parse_result {
  enum kh_parser_node_type type;

  struct {
    kh_bool unevaluated : 1; // The value is currently unevaluated and represents a marker that references the tokens
  } attributes;

  union {
    struct kh_ll_parser_domnode_t_declvar  variable;
    struct kh_astgen_marker                unevaluated; // Refers to token group
    struct kh_astgen_marker                identifier;  // Refers to raw code group
    struct kh_ll_parser_domnode_t_function function;
  } value;
};

/*
struct kh_ll_parser_parse_result_nodes {
union {
  struct kh_ll_parser_parse_result * nodes[4];
struct {
  struct kh_ll_parser_parse_result * dom;

  union {
    // USED BY: KH_PARSER_DOM_NODE_TYPE_FUNCTION as Function body
    struct kh_ll_parser_parse_result * expression;
    // USED BY: KH_PARSER_DOM_NODE_TYPE_VARIABLE as Variable initialization
    struct kh_ll_parser_parse_result * initializer;
  };

  union {
    // USED BY: KH_PARSER_DOM_NODE_TYPE_VARIABLE as variable tyoe; 
    //          KH_PARSER_DOM_NODE_TYPE_FUNCTION as function return type
    struct kh_ll_parser_parse_result * type;
  };

  union {
    // USED BY: KH_PARSER_DOM_NODE_TYPE_FUNCTION as function arguments
    struct kh_ll_parser_parse_result * fn_args;
  };
};
};
};
*/

// ----------------------------------------------------------------------------------------------------
// temporary args struct to prevent changing all pmp_* signatures
typedef const struct kh_utf8sp * const raw_code_t;
typedef const struct kh_lexer_parse_result * const tokens_t;
typedef const kh_sz  ntokens_t;
typedef struct kh_ll_parser_parse_result * out_result_t;

struct _draft_pmp_args {
  raw_code_t   raw_code;
  tokens_t     tokens;
  ntokens_t    ntokens;
  out_result_t out_result;
};
// ----------------------------------------------------------------------------------------------------

enum kh_parser_status kh_ll_parser_identify_tokens(struct _draft_pmp_args * args);

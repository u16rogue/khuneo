#pragma once

#include <kh-core/types.h>
#include <kh-core/refobj.h>
#include <kh-astgen/common.h>

enum kh_lexer_token_type {
  KH_LEXER_TOKEN_TYPE_NONE,
  
  // Refers to a range group used for lazy evaluation. Uses the `code_marker` value.
  KH_LEXER_TOKEN_TYPE_GROUP,
  
  // Refers to a single byte symbol. Represented by its own ASCII value.
  KH_LEXER_TOKEN_TYPE_SYMBOL,

  // Refers to an identifier. Uses the `code_marker` value.
  KH_LEXER_TOKEN_TYPE_IDENTIFIER, 

  // Refers to a 64 bit unsigned value. Can be produced by base10 and base16 literals.
  KH_LEXER_TOKEN_TYPE_UNUM,

  KH_LEXER_TOKEN_TYPE_IFLT,

  KH_LEXER_TOKEN_TYPE_STRING,

  KH_LEXER_TOKEN_TYPE_STRING_INTRP,
  //
  // `Fixed Hash` Can be used to optimize string into a hash value. FOR: Can be used for fast pseudo enums
  KH_LEXER_TOKEN_TYPE_STRING_FXHSH, 

  // Refers to a single or group of whitespaces
  // REASONING: can be useful to check for syntactic structure at parsing
  // level without doing it in the lexer. eg. 123kk -> { [u64][identifier] } | 123 kk -> { [u64][whitespace][identifier]}
  // allowing us to treat these syntax differently
  // NOTE: This is generally grouped and represented by a marker, consecutive whitespaces are grouped together
  KH_LEXER_TOKEN_TYPE_WHITESPACE,

  // Refers to a code comment
  KH_LEXER_TOKEN_TYPE_COMMENT,
};

#define KH_LEXER_CONTEXT_STATUS_FLAG_BITS 0x1F // Decode mask flag (0001_1111)
enum kh_lexer_status {
  KH_LEXER_STATUS_OK      = 0, // Default setup OK status
  KH_LEXER_STATUS_MATCH   = 1, // A token has been identified
  KH_LEXER_STATUS_NOMATCH = 2, // A token cant be identified
  KH_LEXER_STATUS_EOB     = 3, // End of Buffer (ran the entire buffer)         NOTE: Only viable in parser
  KH_LEXER_STATUS_PASS    = 4, // Lexeme didnt match and should let others try. NOTE: Only viable on matchers (see `lexers.c`)

  KH_LEXER_STATUS_WARNING  = 0x20, // Denotes that the status could possiblly be incorrect and the context should be
                                   // inspected and tended to.
                                   // WARNING FLAG using the 3rd most significant bit (0010_0000)- 

  KH_LEXER_STATUS_ERROR    = 0x40, // Denotes that the status is fatal and the context should be disposed
                                   // ERROR FLAG using the 2nd most significant bit (0100_0000)

  KH_LEXER_STATUS_EXTENDED = 0x80, // Denotes that the flag size (1 byte by default) is now extended by the power of 2.
                                   // The next extended 8 bits will have its most significant bit also represent an extended state
                                   // Reasoning: Future proofing
                                   // EXTEND FLAG using the most significant bit (1000_0000)

  // NOTE: Max flag bit value is 32

  // -- Warnings
  KH_LEXER_STATUS_UNKNOWN_WARNING = (0 & KH_LEXER_CONTEXT_STATUS_FLAG_BITS) | KH_LEXER_STATUS_WARNING, // Unspecified warning (Yes the or is pointless, its for verbosity)
  KH_LEXER_STATUS_EOI_OVERFLOW    = (1 & KH_LEXER_CONTEXT_STATUS_FLAG_BITS) | KH_LEXER_STATUS_WARNING, // End of index overflow: Parsing index went over the size limit.

  // -- Errors
  KH_LEXER_STATUS_UNKNOWN_ERROR   = (0 & KH_LEXER_CONTEXT_STATUS_FLAG_BITS) | KH_LEXER_STATUS_ERROR, // Unspecified error (Yes the or is pointless, its for verbosity)
  KH_LEXER_STATUS_UTF8_INVALID    = (1 & KH_LEXER_CONTEXT_STATUS_FLAG_BITS) | KH_LEXER_STATUS_ERROR, // Invalid UTF8 byte was met in the buffer.
  KH_LEXER_STATUS_SYNTAX_ERROR    = (2 & KH_LEXER_CONTEXT_STATUS_FLAG_BITS) | KH_LEXER_STATUS_ERROR, // Invalid syntax
};

/*
 *  Holds a union of the different token type values.
 *
 *  See `kh_lexer_token_type` for reference as to what each token
 *  type uses to store and represent its parsed value.
 */
union kh_lexer_parse_result_value {
  kh_utf8 symbol;
  struct kh_astgen_marker marker; // Refers a chunk of the raw source buffer

  struct {
    union { kh_unum unum; kh_iflt iflt; };
    // kh_sz marker_size; [11/08/2023] We use nconsume to track how much we should move code index.
  } number;
  
};

struct kh_lexer_parse_result {
  enum  kh_lexer_token_type         type;
  union kh_lexer_parse_result_value value;
};

/*
 *  # Low Level Lexer Parse Type / Group
 *
 *  ## Arguments
 *  - code         : UTF8 khuneo code
 *  - out_result   : Outbound structure that contains the type and value of the token matched or the lexer
 *                   that threw the exception.
 *                   NOTE: Due to the immutable nature and how this function consumes code. Token
 *                   types that uses the `marker` value will not have its `offset` updated. Because
 *                   its impossible as this function has no access to the context. This will be normally
 *                   be updated by a higher function.
 *  - out_nconsume : Outbound unsigned value that denotes how much of the `code` was consumed to match the entire parse
 *                   result.
 *
 *  ## Return
 *  - return : The parsing status.
 *
 *  ## Description
 *  A low level function to determine a match based off a provided code.
 *  This function serves as a "immutable" parser that's only job is to catalogue
 *  and produce result by determining what the code's token type is as the lexer context
 *  is never passed to this function.
 *
 *  ## Reasoning
 *  As khuneo is designed to be modular this function allows library consumers
 *  to parse a piece of code to determine its type without modifying an underlying context
 *  in contrast to calling `kh_lexer_context_parse_next` which modifies the lexer context.
 *  In the case of trying to extend the language the need of determining whether a piece of
 *  code is valid or not while retaining the current context this function can be utilized to
 *  fullfill that necessity eliminating the need to either duplicate the underlying lexer or
 *  writing your own implementation. Simply put, a reusable component of khuneo.
 */
enum kh_lexer_status kh_ll_lexer_parse_type  (const struct kh_utf8sp * const codesp, struct kh_lexer_parse_result * out_result, kh_sz * out_nconsume);
enum kh_lexer_status kh_ll_lexer_parse_group (const struct kh_utf8sp * const codesp, struct kh_lexer_parse_result * out_result, kh_sz * out_nconsume);

/*
 *  Represents a Lexer context storing a lexer state
 */
struct kh_lexer_context {
  kh_refobji _code_buffer; // Type of <kh_utf8sp *>
  kh_u32     _code_index;  // Current index of the code being lexed.
};

/*
 *  Determines a possible match from the current
 *  lexer state.
 */
enum kh_lexer_status kh_ll_lexer_context_determine(struct kh_lexer_context * ctx, struct kh_lexer_parse_result * out_result, kh_sz * out_nconsume);

/*
 *  Applies the side effects to a lexer context from
 *  the determined match.
 *  NOTE: Only call when a `KH_LEXER_STATUS_MATCH` is
 *  created from a `kh_ll_lexer_context_determine` call.
 */
enum kh_lexer_status kh_ll_lexer_context_apply(struct kh_lexer_context * ctx, struct kh_lexer_parse_result * out_result, kh_sz nconsume);

/*
 *  # Lexer context initializer
 *  Initializes a `kh_lexer_context` structure
 *
 *  ## Arguments
 *  - ctx       : Pointer to a `kh_lexer_context` structure
 *  - code      : A reference object that contains the raw UTF8 khuneo code to be parsed
 */
kh_bool kh_lexer_context_init(struct kh_lexer_context * ctx, kh_refobji code);

/*
 *  # Lexer context uninitializer
 *  Uninitializes a `kh_lexer_context` structure after
 *  its use.
 *  NOTE: Uninitialize != Free. If this structure is heap alloced
 *  you'll have to free it yourself. (This allows the usage of stack
 *  memory)
 *
 *  ## Arguments
 *  - ctx       : Pointer to a valid `kh_lexer_context` structure
 */
kh_bool kh_lexer_context_uninit(struct kh_lexer_context * ctx);

/*
 *  # Context Lexer Next
 *  Consecutively parses a khuneo code upon call and returns upon
 *  a lexer match or exception while providing information through
 *  `out_result`.
 *
 *  ## Arguments
 *  - ctx        : Lexer context 
 *  - out_result : Contains the type and value of the matched type
 *                 or offending parser. (`Offending` refers the the
 *                 lexer type that have thrown an exception)
 *
 *  ## Return
 *  - return     : Returns the lexer status
 */
enum kh_lexer_status kh_lexer_context_parse_next(struct kh_lexer_context * ctx, struct kh_lexer_parse_result * out_result);

/*
 *  Determines the token type of a result.
 */
enum kh_lexer_token_type kh_lexer_parse_result_type_is(const struct kh_lexer_parse_result * const result);

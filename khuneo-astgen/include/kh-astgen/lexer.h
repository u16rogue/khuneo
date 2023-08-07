#pragma once

#include <kh-core/refobj.h>

enum kh_lexer_token_type {
  KH_LEXER_TOKEN_TYPE_NONE,
  KH_LEXER_TOKEN_TYPE_GROUP,      // Refers to a range group used for lazy evaluation. Uses the `code_marker` value.
  KH_LEXER_TOKEN_TYPE_SYMBOL,     // Refers to a single byte symbol. Uses the `token` value.
  KH_LEXER_TOKEN_TYPE_IDENTIFIER, // Refers to an identifier. Uses the `code_marker` value.
  KH_LEXER_TOKEN_TYPE_NUMBER,
  KH_LEXER_TOKEN_TYPE_STRING,
  KH_LEXER_TOKEN_TYPE_STRING_INTRP,
  KH_LEXER_TOKEN_TYPE_STRING_FXHSH, // `Fixed Hash` Can be used to optimize string into a hash value. FOR: Can be used for fast pseudo enums
};

#define KH_LEXER_CONTEXT_STATUS_FLAG_BITS 0x1F // Decode mask flag (0001_1111)

enum kh_lexer_status {
  KH_LEXER_STATUS_ = 0,
  KH_LEXER_STATUS_OK = 0,
  KH_LEXER_STATUS_WARNING  = 0x20, // Denotes that the status could possiblly be incorrect and the context should be
                                   // checked and tended to.
                                   // WARNING FLAG using the 3rd most significant bit (0010_0000)- 

  KH_LEXER_STATUS_ERROR    = 0x40, // Denotes that the status is fatal and the context should be disposed
                                   // ERROR FLAG using the 2nd most significant bit (0100_0000)

  KH_LEXER_STATUS_EXTENDED = 0x80, // Denotes that the flag size (1 byte by default) is now extended by the power of 2.
                                   // The next extended 8 bits will have its most significant bit also represent an extended state
                                   // Reasoning: Future proofing
                                   // EXTEND FLAG using the most significant bit (1000_0000)

  // NOTE: Max flag bit value is 32

  // -- Warnings
  KH_LEXER_STATUS_UNKNOWN_WARNING     = (0 & KH_LEXER_CONTEXT_STATUS_FLAG_BITS) | KH_LEXER_STATUS_WARNING, // Unspecified warning (Yes the or is pointless, its for verbosity)
  KH_LEXER_STATUS_CODE_PARSE_OVERFLOW = (1 & KH_LEXER_CONTEXT_STATUS_FLAG_BITS) | KH_LEXER_STATUS_WARNING, // Parsing index went over the size limit.

  // -- Errors
  KH_LEXER_STATUS_UNKNOWN_ERROR       = (0 & KH_LEXER_CONTEXT_STATUS_FLAG_BITS) | KH_LEXER_STATUS_ERROR, // Unspecified error (Yes the or is pointless, its for verbosity)
  KH_LEXER_STATUS_UTF8_INVALID        = (1 & KH_LEXER_CONTEXT_STATUS_FLAG_BITS) | KH_LEXER_STATUS_ERROR, // Invalid UTF8 byte was met in the buffer.
  KH_LEXER_STATUS_SYNTAX_ERROR        = (2 & KH_LEXER_CONTEXT_STATUS_FLAG_BITS) | KH_LEXER_STATUS_ERROR, // Invalid syntax
};

/*
 *  Holds a union of the different token type values.
 *
 *  See `kh_lexer_token_type` for reference as to what each token
 *  type uses to store and represent its parsed value.
 */
union kh_lexer_ll_parse_result_value {
  kh_utf8 symbol;
  struct {
    kh_u32 offset; // Starting position of the chunk
    kh_u32 size;   // Size of the chunk (in bytes)
  } marker;        // Refers a chunk of the raw source buffer
};

struct kh_lexer_ll_parse_result {
  enum  kh_lexer_status                status;
  union kh_lexer_ll_parse_result_value value;
};

/*
 *  # Low Level Lexer Parse Type / Group
 *
 *  ## Arguments
 *  - code       : UTF8 khuneo code
 *  - size       : Size of the code in bytes not length. (utf8)
 *  - out_result : Holds the parsing context and resulting token value produced by the token
 *                 type.
 *                 NOTE: Due to the immutable nature and how this function consumes code. Token
 *                 types that uses the `marker` value will not have its `offset` updated. Because
 *                 its impossible as this function has no access to the context.
 *
 *  ## Return
 *  - return : The matched token type.
 *             NOTE: The return type does not denote the success or failure of the function but
 *             rather by the `status` field of `kh_lexer_ll_parse_result`.
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
enum kh_lexer_token_type kh_ll_lexer_parse_type(const kh_utf8 * code, kh_sz size, struct kh_lexer_ll_parse_result * out_result);
enum kh_lexer_token_type kh_ll_lexer_parse_group(const kh_utf8 * code, kh_sz size, struct kh_lexer_ll_parse_result * out_result);

/*
 *  Represents a Lexer context storing a lexer state
 */
struct kh_lexer_context {
  kh_refobji _code_buffer;
  kh_u32     _code_index; // Current index of the code being lexed.
  kh_u32     _code_size;  // Size of code buffer in size
};

/*
 *  # Lexer context initializer
 *  Initializes a `kh_lexer_context` structure
 *
 *  ## Arguments
 *  - ctx       : Pointer to a `kh_lexer_context` structure
 *  - code      : A reference object that contains the raw UTF8 khuneo code to be parsed
 *  - code_size : Size of the code in bytes not in length. (UTF8)
 */
kh_bool kh_lexer_context_init(struct kh_lexer_context * ctx, kh_refobji code, kh_sz code_size);

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
 *  Parses the current context and returns the determined lexed type
 *  followed by advancing the context.
 *
 *  ## Arguments
 *  - ctx        : Lexer context 
 *  - out_result : Contains the status and value of the matched type
 *
 *  ## Return
 *  - return     : Returns the lexed token type presented in the `kh_lexer_token_type`
 *  enumerator
 */
enum kh_lexer_token_type kh_lexer_context_parse_next(struct kh_lexer_context * ctx, struct kh_lexer_ll_parse_result * out_result);

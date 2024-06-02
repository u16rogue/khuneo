#include <kh-gen/lexer.h>

enum kh_LexerInitResponse kh_lexer_init(struct kh_LexerContext * const ctx) {
  (void)ctx;
  return KH_LEXER_INIT_OK;
}

//------------------------------------------------------------------------------

static kh_bool is_char_alpha(const kh_U8Char ch) {
  return (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
}

static kh_bool is_char_numeric(const kh_U8Char ch) {
  return (ch >= '0' && ch <= '9');
}

static kh_bool is_char_identifier(const kh_U8Char ch) {
  return is_char_alpha(ch) || is_char_numeric(ch) || ch == '$' || ch == '_';
}

static kh_bool is_char_whitespace(const kh_U8Char ch) {
  switch (ch) {
    case ' ':
    case '\n':
    case '\r':
    case '\t':
      return KH_TRUE;
  }
  return KH_FALSE;
}

static kh_bool is_char_string_delim(const kh_U8Char ch) {
  switch (ch) {
    case '\'':
    case '"':
    case '`':
      return KH_TRUE;
  }
  return KH_FALSE;
}

//------------------------------------------------------------------------------

typedef const kh_U8Char * const DescriberChunk;
typedef const kh_U8StringSize DescriberRange;
typedef struct kh_LexerDescription * const DescriberResult;
typedef enum kh_LexerResponse(DescriberFn)(DescriberChunk, DescriberRange,
  DescriberResult);

//------------------------------------------------------------------------------

static enum kh_LexerResponse describe_identifier(DescriberChunk chunk,
                                                 DescriberRange chunk_range,
                                                 DescriberResult described) {
  // See `describers` for a relevant note.
  if (!is_char_identifier(chunk[0])) {
    return KH_LEXER_RES_PASS;
  }

  described->type = KH_TOKEN_TYPE_IDENTIFIER;

  kh_u32 i = 1;
  for (; i < chunk_range && is_char_identifier(chunk[i]); ++i) {}
  described->size = i; 

  return KH_LEXER_RES_MATCH;
}

static enum kh_LexerResponse describe_number(DescriberChunk chunk,
                                             DescriberRange chunk_range,
                                             DescriberResult described) {
  if (!is_char_numeric(chunk[0])) {
    return KH_LEXER_RES_PASS;
  }

  described->type = KH_TOKEN_TYPE_NUMBER;

  kh_u32 i = 1;
  for (; i < chunk_range && is_char_numeric(chunk[i]); ++i) {}
  described->size = i;

  return KH_LEXER_RES_MATCH;
}

static enum kh_LexerResponse describe_string(DescriberChunk chunk,
                                             DescriberRange chunk_range,
                                             DescriberResult described) {
  #define END_MARKER '\0'
  if (chunk_range < 2 || !is_char_string_delim(chunk[0])) {
    return KH_LEXER_RES_PASS; 
  }

  described->type = KH_TOKEN_TYPE_STRING;
  kh_U8Char marker = chunk[0];
  kh_u32 i = 1;

  for (; i < chunk_range; ++i) {
    if (chunk[i] == marker) {
      marker = END_MARKER;
      break;
    }
  }

  described->size = i;
  if (marker != END_MARKER) {
    return KH_LEXER_RES_UNCLOSED;
  }

  return KH_LEXER_RES_MATCH;
  #undef END_MARKER
}

static enum kh_LexerResponse describe_whitespace(DescriberChunk chunk,
                                                 DescriberRange chunk_range,
                                                 DescriberResult described) {
  if (!is_char_whitespace(chunk[0])) {
    return KH_LEXER_RES_PASS;
  }

  described->type = KH_TOKEN_TYPE_WHITESPACE;

  kh_u32 i = 1;
  for (;i < chunk_range && is_char_whitespace(chunk[i]); ++i) {}
  described->size = i;

  return KH_LEXER_RES_MATCH;
}

static enum kh_LexerResponse describe_symbol(DescriberChunk chunk,
                                             DescriberRange chunk_range,
                                             DescriberResult described) {

  enum kh_TokenSymbol symbol = KH_TOKEN_SYMBOL_INVALID;
  kh_u8 size   = 0;

  #define __KH_TOKSYM_DEF(b, e) case b: symbol = e; break;
  if (chunk_range > 1) {
    switch ( *(const kh_u16 *)chunk ) {
      #include <kh-gen/lst/symbols2.lst>
    }
  }

  if (symbol != KH_TOKEN_SYMBOL_INVALID) {
    size = 2;
  } else {
    switch (*chunk) {
      #include <kh-gen/lst/symbols1.lst>
    }
  }
  #undef __KH_TOKSYM_DEF

  if (symbol == KH_TOKEN_SYMBOL_INVALID) {
    return KH_LEXER_RES_PASS;  
  } else {
    size = 1;
  }

  described->type = KH_TOKEN_TYPE_SYMBOL;
  described->symbol.symb = symbol;
  described->symbol.size = size;
  return KH_LEXER_RES_MATCH;
}

// [15.05.2024 @u16rogue TODO] sort by average frequency
static DescriberFn * const describers[] = {
  // [15.04.2024 @u16rogue] NOTE: We make assumptions when parsing identifiers requiring numbers to be parsed first
  // this prevents `123abc` from being identified as an `identifier` without creating a separate check for `identifier`.
  // Before this note was placed we use a separate `is_char_identifier` without numbers as a factor, this should
  // eliminate the need for that.
  describe_number,
  describe_identifier,

  describe_symbol,
  describe_string,
  // [15.05.2024 @u16rogue NOTE] Placed this last incase someone wants to implement their own whitespace lexer thing
  describe_whitespace, 
};

enum kh_LexerResponse kh_ll_lexer_describe(const kh_U8Char * const chunk,
                                           const kh_U8StringSize chunk_range,
                                           struct kh_LexerDescription * const described) {
  described->type = KH_TOKEN_TYPE_INVALID;

  for (kh_u8 i = 0; i < kh_array_length(describers); ++i) {
    const enum kh_LexerResponse response = describers[i](chunk, chunk_range, described);
    if (response & KH_LEXER_RES_FAIL) {
      return response;
    }

    if (response == KH_LEXER_RES_MATCH) {
      return KH_LEXER_RES_OK;
    }
  }

  return KH_LEXER_RES_UNDESCRIBED;
}

enum kh_LexerResponse kh_ll_lexer_identifier_to_keyword(const kh_U8Char * const chunk,
                                                        const kh_U8StringSize identifier_size,
                                                        struct kh_LexerDescription * const described) {

  // [15.05.2024 @u16rogue NOTE] Should we even check? Only time we'll run this is if we're determined
  // that it IS an identifier, no loss is done on accidental call (afaik). Otherwise we'll be doing
  // double checks
  /*
  if (described->type != KH_TOKEN_TYPE_IDENTIFIER) {
    return KH_LEXER_RES_PASS;
  }
  */

  enum kh_TokenKeyword keyword = KH_TOKEN_KEYWORD_INVALID;

  #define __KH_TOKKW_DEF(b, e) case b: keyword = e; break;
  switch (identifier_size) {
    case 2: {
      switch ( *(const kh_u16 *)chunk ) {
        #include <kh-gen/lst/keywords2.lst>
      }
      break;
    }
    case 3: {
      switch ( ((const kh_u32)chunk[2] << 16) | *(const kh_u16 *)chunk ) {
        #include <kh-gen/lst/keywords3.lst>
      }
      break;
    }
  }
  #undef __KH_TOKKW_DEF

  if (keyword != KH_TOKEN_KEYWORD_INVALID) {
    described->type         = KH_TOKEN_TYPE_KEYWORD;
    described->keyword.what = keyword;
    described->keyword.size = identifier_size;
    return KH_LEXER_RES_MATCH;
  }

  return KH_LEXER_RES_PASS;
}

kh_u32 kh_lexer_describe_size_get(const struct kh_LexerDescription * const description) {
  switch (description->type) {
    case KH_TOKEN_TYPE_INVALID: {
      return 0;
    }
    case KH_TOKEN_TYPE_IDENTIFIER:
    case KH_TOKEN_TYPE_NUMBER:
    case KH_TOKEN_TYPE_WHITESPACE:
    case KH_TOKEN_TYPE_STRING:
    {
      return description->size;
      break;
    }
    case KH_TOKEN_TYPE_KEYWORD:
    case KH_TOKEN_TYPE_SYMBOL:
    {
      return description->generic.size;
      break;
    }
  }

  return 0;
}

static struct kh_LexerDescription * gobble_described_get(struct kh_LexerGobbleContext * ctx) {
  return &ctx->description;
}

enum kh_LexerResponse kh_lexer_gobble_analyze(struct kh_LexerGobbleContext * ctx) {
  // [17.05.2024 @u16rogue NOTE] `kh_ll_lexer_describe` already sets it to invalid.
  // gobble_described_get(ctx)->type = KH_TOKEN_TYPE_INVALID;

  const kh_U8Char * pcursor = ctx->_code->_buffer + ctx->_cursor;
  const kh_u32      size    = ctx->_code->_size - ctx->_cursor;

  enum kh_LexerResponse res = kh_ll_lexer_describe(pcursor, size, &ctx->description);
  if (res != KH_LEXER_RES_OK) {
    return res;
  }

  if (ctx->description.type == KH_TOKEN_TYPE_IDENTIFIER) {
    // [15.05.2024 @u16rogue] Ignoring return as this function does not error out and will only
    // respond if its either a keyword (match) or its not (pass).
    kh_ll_lexer_identifier_to_keyword(pcursor, kh_lexer_describe_size_get(gobble_described_get(ctx)), &ctx->description);
  }

  return KH_LEXER_RES_OK;
}

enum kh_LexerResponse kh_lexer_gobble_step(struct kh_LexerGobbleContext * ctx) {
  const kh_u32 offset = kh_lexer_describe_size_get(gobble_described_get(ctx));
  if (offset == 0) {
    return KH_LEXER_RES_INVALID_OFF;
  }

  if (ctx->_cursor + offset >= ctx->_code->_size) {
    // [17.05.2024 @u16rogue NOTE] automatic clamping to make sure it's always at buffer end.
    ctx->_cursor = ctx->_code->_size;
    return KH_LEXER_RES_END;
  }

  ctx->_cursor += offset;
  return KH_LEXER_RES_OK;
}

enum kh_LexerResponse kh_lexer_gobble_stop(struct kh_LexerGobbleContext * ctx) {
  KH_UNUSED(ctx);
  return KH_LEXER_RES_OK;
}

enum kh_LexerResponse kh_lexer_gobble_start(struct kh_LexerGobbleContext * ctx, const struct kh_SourceCodeBuffer * code) {
  ctx->_cursor          = 0;
  ctx->_code            = code;
  ctx->description.type = KH_TOKEN_TYPE_INVALID;
  ctx->description.size = 0;
  return KH_LEXER_RES_OK;
}

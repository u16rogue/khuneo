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

static kh_bool is_char_string(const kh_U8Char ch) {
  switch (ch) {
    case '\'':
    case '"':
    case '`':
      return KH_TRUE;
  }
  return KH_FALSE;
}

//------------------------------------------------------------------------------

typedef const kh_U8Char * const               DescriberChunk;
typedef const kh_U8StringSize                 DescriberRange;
typedef struct kh_LexerDescribeResult * const DescriberResult;
typedef enum kh_LexerResponse(DescriberFn)(DescriberChunk, DescriberRange, DescriberResult);

//------------------------------------------------------------------------------

static enum kh_LexerResponse describe_identifier(DescriberChunk chunk, DescriberRange chunk_range, DescriberResult described) {
  if (!is_char_alpha(chunk[0])
  &&  chunk[0] != '$'
  &&  chunk[0] != '_'
  ) {
    return KH_LEXER_RES_PASS;
  }

  described->type = KH_TOKEN_TYPE_IDENTIFIER;

  kh_u32 i = 1;
  for (; i < chunk_range && is_char_identifier(chunk[i]); ++i) {}
  described->size = i; 

  return KH_LEXER_RES_MATCH;
}

static enum kh_LexerResponse describe_number(DescriberChunk chunk, DescriberRange chunk_range, DescriberResult described) {
  if (!is_char_numeric(chunk[0])) {
    return KH_LEXER_RES_PASS;
  }

  described->type = KH_TOKEN_TYPE_NUMBER;

  kh_u32 i = 1;
  for (; i < chunk_range && is_char_numeric(chunk[i]); ++i) {}
  described->size = i;

  return KH_LEXER_RES_MATCH;
}

static enum kh_LexerResponse describe_string(DescriberChunk chunk, DescriberRange chunk_range, DescriberResult described) {
  #define END_MARKER '\0'
  if (chunk_range < 2 || !is_char_string(chunk[0])) {
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

static enum kh_LexerResponse describe_whitespace(DescriberChunk chunk, DescriberRange chunk_range, DescriberResult described) {
  if (!is_char_whitespace(chunk[0])) {
    return KH_LEXER_RES_PASS;
  }

  described->type = KH_TOKEN_TYPE_WHITESPACE;

  kh_u32 i = 1;
  for (;i < chunk_range && is_char_whitespace(chunk[i]); ++i) {}
  described->size = i;

  return KH_LEXER_RES_MATCH;
}

static enum kh_LexerResponse describe_symbol(DescriberChunk chunk, DescriberRange chunk_range, DescriberResult described) {
  enum kh_TokSymbol symbol = KH_TOKEN_SYMBOL_INVALID;
  kh_u8             size   = 0;

  if (chunk_range > 1) {
    switch ( *(const kh_u16 *)chunk ) {
      case 0x3D21: symbol = KH_TOKEN_SYMBOL_EXC_EQ  ; break; // !=
      case 0x3D3C: symbol = KH_TOKEN_SYMBOL_LT_EQ   ; break; // <=
      case 0x3D3E: symbol = KH_TOKEN_SYMBOL_GT_EQ   ; break; // >=
      case 0x3D3D: symbol = KH_TOKEN_SYMBOL_EQ_EQ   ; break; // ==
      case 0x3D3A: symbol = KH_TOKEN_SYMBOL_COL_EQ  ; break; // :=
      case 0x3D7E: symbol = KH_TOKEN_SYMBOL_TIL_EQ  ; break; // ~=
      case 0x3D2B: symbol = KH_TOKEN_SYMBOL_PLS_EQ  ; break; // +=
      case 0x3D2D: symbol = KH_TOKEN_SYMBOL_DSH_EQ  ; break; // -=
      case 0x3D2A: symbol = KH_TOKEN_SYMBOL_STR_EQ  ; break; // *=
      case 0x3D2F: symbol = KH_TOKEN_SYMBOL_SLH_EQ  ; break; // /=
      case 0x3D25: symbol = KH_TOKEN_SYMBOL_PER_EQ  ; break; // %=
      case 0x3D7C: symbol = KH_TOKEN_SYMBOL_BAR_EQ  ; break; // |=
      case 0x3D26: symbol = KH_TOKEN_SYMBOL_AND_EQ  ; break; // &=
      case 0x3D5E: symbol = KH_TOKEN_SYMBOL_HAT_EQ  ; break; // ^=
      case 0x2B2B: symbol = KH_TOKEN_SYMBOL_PLS_PLS ; break; // ++
      case 0x2D2D: symbol = KH_TOKEN_SYMBOL_DSH_DSH ; break; // --
      case 0x2626: symbol = KH_TOKEN_SYMBOL_AND_AND ; break; // &&
      case 0x7C7C: symbol = KH_TOKEN_SYMBOL_BAR_BAR ; break; // ||
    }
  }

  if (symbol != KH_TOKEN_SYMBOL_INVALID) {
    size = 2;
  } else {
    switch (*chunk) {
      case '!':  symbol = KH_TOKEN_SYMBOL_EXC   ; break;
      case '@':  symbol = KH_TOKEN_SYMBOL_AT    ; break;
      case '#':  symbol = KH_TOKEN_SYMBOL_HSH   ; break;
      case '^':  symbol = KH_TOKEN_SYMBOL_HAT   ; break;
      case '&':  symbol = KH_TOKEN_SYMBOL_AND   ; break;
      case '*':  symbol = KH_TOKEN_SYMBOL_STR   ; break;
      case '(':  symbol = KH_TOKEN_SYMBOL_PRN_S ; break;
      case ')':  symbol = KH_TOKEN_SYMBOL_PRN_E ; break;
      case '[':  symbol = KH_TOKEN_SYMBOL_BRK_S ; break;
      case ']':  symbol = KH_TOKEN_SYMBOL_BRK_E ; break;
      case '{':  symbol = KH_TOKEN_SYMBOL_CBK_S ; break;
      case '}':  symbol = KH_TOKEN_SYMBOL_CBK_E ; break;
      case '/':  symbol = KH_TOKEN_SYMBOL_SLH   ; break;
      case '\\': symbol = KH_TOKEN_SYMBOL_BSL   ; break;
      case ',':  symbol = KH_TOKEN_SYMBOL_COM   ; break;
      case '.':  symbol = KH_TOKEN_SYMBOL_DOT   ; break;
      case ';':  symbol = KH_TOKEN_SYMBOL_SCL   ; break;
      case ':':  symbol = KH_TOKEN_SYMBOL_COL   ; break;
      case '-':  symbol = KH_TOKEN_SYMBOL_DSH   ; break;
      case '=':  symbol = KH_TOKEN_SYMBOL_EQ    ; break;
      case '+':  symbol = KH_TOKEN_SYMBOL_PLS   ; break;
      case '?':  symbol = KH_TOKEN_SYMBOL_QSM   ; break;
    }
  }

  if (symbol != KH_TOKEN_SYMBOL_INVALID) {
    described->type = KH_TOKEN_TYPE_SYMBOL;
    described->symbol.type = symbol;
    described->symbol.size = size;
  }

  return KH_LEXER_RES_MATCH;
}

DescriberFn * const describers[] = {
  describe_whitespace,
  describe_identifier,
  describe_number,
};

enum kh_LexerResponse kh_ll_lexer_describe(const kh_U8Char * const chunk, const kh_U8StringSize chunk_range, struct kh_LexerDescribeResult * const described) {
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

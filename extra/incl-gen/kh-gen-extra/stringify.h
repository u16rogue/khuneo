#pragma once

#include <kh-core/str.h>
#include <kh-gen/lexer.h>

const kh_U8Char * kh_stringify_token_type(enum kh_TokenType type);
const kh_U8Char * kh_stringify_lexer_response(enum kh_LexerResponse response);

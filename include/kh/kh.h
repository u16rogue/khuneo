#pragma once

#include <kh-core/mem.h>
#include <kh-core/defs.h>
#include <kh-core/str.h>
#include <kh-astgen/lexer.h>

enum kh_EvalResponse {
  KH_EVAL_OK,
};

enum kh_EvalResponse kh_eval(const char * code);

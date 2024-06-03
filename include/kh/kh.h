#pragma once

#include <kh-core/defs.h>

enum kh_EvalResponse {
  KH_EVAL_OK   = 0,
  KH_EVAL_FAIL = KH_FAIL_BIT,
};

enum kh_EvalResponse kh_eval(const char * code);

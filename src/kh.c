#include <kh-core/impldec.h>
#include <kh/kh.h>

enum kh_EvalResponse kh_eval(const char * code) {
  (void)code;
  kh_alloc(50);
  return KH_EVAL_OK;
}

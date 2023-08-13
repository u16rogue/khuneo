#include "kh-core/types.h"
#include <kh-core/math.h>

kh_unum kh_math_pow_unum(kh_unum v, kh_unum n) {
  if (n == 0)
    return 1;
  if (n == 1)
    return v;

  kh_unum o = v;
  while (n > 1) {
    o *= v;
    --n;
  }

  return o;
}

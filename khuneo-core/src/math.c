#include "kh-core/types.h"
#include <kh-core/math.h>

kh_u64 kh_math_pow_u64(kh_u64 v, kh_sz n) {
  if (n == 0)
    return 1;
  if (n == 1)
    return v;

  kh_u64 o = v;
  while (n > 1) {
    o *= v;
    --n;
  }

  return o;
}

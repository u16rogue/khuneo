#pragma once

#include <kh-core/types.h>

typedef struct _kh_context_unit {
  kh_u8 __placeholder;
} kh_context;

typedef struct _kh_context_thread {
  kh_u8 __placeholder;
} kh_context_thread;

kh_bool kh_eval(void);

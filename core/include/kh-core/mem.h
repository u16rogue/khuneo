#pragma once

#include <kh-core/defs.h>

void *
kh_alloc(
  kh_u64 size
);

void *
kh_realloc(
  KH_ANT_ARG_IN kh_vptr current,
  kh_u64                newsize
);

kh_u64
kh_dealloc(
  KH_ANT_ARG_IN     kh_vptr ptr,
  KH_ANT_ARG_OPTNIL kh_u64 size
);

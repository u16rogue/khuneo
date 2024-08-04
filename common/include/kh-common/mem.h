#pragma once

#include <kh-common/defs.h>

void *
kh_alloc(
  kh_u64 size
);

void *
kh_realloc(
  kh_vptr current KH_ANT_ARG_IN
, kh_u64  newsize
);

kh_u64
kh_dealloc(
  kh_vptr ptr  KH_ANT_ARG_IN
, kh_u64  size KH_ANT_ARG_OPTNIL
);

#pragma once

#include <kh-common/defs.h>
#include <kh-common/str.h>

enum kh_PSM_RES {
  KH_PSM_RES_OK   = KH_OK,
  KH_PSM_RES_FAIL = KH_FAIL_BIT,
};

struct kh_PSMState {
  kh_u8 _;
};

enum kh_PSM_RES
kh_psm_start(
  struct kh_PSMState * state KH_ANT_ARG_OUT
);

enum kh_PSM_RES
kh_psm_end(
  struct kh_PSMState * state KH_ANT_ARG_IN
);

enum kh_PSM_RES
kh_psm_feed(
  struct kh_PSMState * state   KH_ANT_ARG_IN
, const kh_U8Char    * content KH_ANT_ARG_IN
, kh_U8StringSize      content_size
);

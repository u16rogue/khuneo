#pragma once

#include <kh-core/defs.h>

struct kh_CString {
  char * data;
};

typedef kh_u8 kh_utf8;

struct kh_U8String {
  union {
    kh_utf8 * pdata;
    kh_utf8   idata[8];
  };
  kh_u64 size;
};

kh_utf8 * kh_u8string_data(struct kh_U8String * u8string);

enum kh_U8FromCStringResponse {
  KH_U8_FROM_CSTR_OK,
};

enum kh_U8FromCStringResponse
kh_u8string_from_cstr(
  KH_ANT_ARG_OUT struct kh_U8String * out,
  KH_ANT_ARG_IN  const char * string
);

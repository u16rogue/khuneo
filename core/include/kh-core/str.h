#pragma once

#include <kh-core/defs.h>

//-[ String ]-------------------------------------------------------------------

typedef kh_u8      kh_U8Char;
typedef kh_U8Char* kh_U8CharPtr;
typedef kh_u32     kh_U8StringSize;
typedef kh_u32     kh_U8StringLength;

enum kh_U8StringAttributes {
  KH_U8STRING_ATTRIB_READONLY = (1 << 0),
};

struct kh_U8String {
  union {
    kh_U8CharPtr pdata;
    kh_U8Char    idata[8];
  };
  kh_U8StringSize size;
  enum kh_U8StringAttributes attrib;
};

kh_U8CharPtr
kh_u8string_data(
  struct kh_U8String * u8string
);

kh_bool
kh_u8string_from_cstr(
  KH_ANT_ARG_OUT struct kh_U8String * out,
  KH_ANT_ARG_IN  const char * string
);

//-[ String View ]--------------------------------------------------------------

struct kh_U8StringView {
  kh_U8CharPtr               data;
  kh_U8StringSize            size;
  enum kh_U8StringAttributes attrib;
};

kh_U8CharPtr
kh_u8stringview_data(
  KH_ANT_ARG_IN struct kh_U8StringView * view
);

kh_bool
kh_u8stringview_from_u8string(
  KH_ANT_ARG_OUT struct kh_U8StringView * view,
  KH_ANT_ARG_IN  struct kh_U8String * string
);

#define                                  \
kh_u8stringview_from_cstring(cstr) {     \
  .data   = (kh_U8CharPtr)cstr,          \
  .size   = sizeof(cstr) - 1,            \
  .attrib = KH_U8STRING_ATTRIB_READONLY, \
}

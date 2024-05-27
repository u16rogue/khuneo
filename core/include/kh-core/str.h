#pragma once

#include <kh-core/defs.h>

//-[ String ]-------------------------------------------------------------------

typedef kh_u8      kh_U8Char;
typedef kh_U8Char* kh_U8CharPtr;
typedef kh_u32     kh_U8StringSize;
typedef kh_u32     kh_U8StringLength;

enum kh_U8StringAttributes {
  KH_U8STRING_ATTRIB_READONLY = (1 << 0),
  // [27.05.2024 @u16rogue NOTE] If unset and READONLY it can be assumed its
  // a C string
  KH_U8STRING_ATTRIB_U8STR      = (1 << 1),
  KH_U8STRING_ATTRIB_NULLTERM   = (1 << 2),
  // KH_U8STRING_ATTRIB_PURE_ASCII = (1 << 3),

  //----------------------------------------------------------------------------

  // Raw C String
  KH_U8STRING_ATTRIB_IS_RCSTRING =
    KH_U8STRING_ATTRIB_READONLY | KH_U8STRING_ATTRIB_NULLTERM
    /*| KH_U8STRING_ATTRIB_PURE_ASCII*/,
};

struct kh_U8String {
  union {
    kh_U8CharPtr _pdata;
    kh_U8Char    _idata[8];
  };
  kh_U8StringSize _size;
  enum kh_U8StringAttributes _attrib;
};

kh_U8CharPtr
kh_u8string_data(
  KH_ANT_ARG_IN struct kh_U8String * string
);

kh_U8StringSize
kh_u8string_size(
  KH_ANT_ARG_IN struct kh_U8String * string
);

kh_U8StringSize
kh_u8string_length(
  KH_ANT_ARG_IN struct kh_U8String * string
);

kh_bool
kh_u8string_from_cstr(
  KH_ANT_ARG_OUT struct kh_U8String * out,
  KH_ANT_ARG_IN  const char * string
);

//-[ String View ]--------------------------------------------------------------

struct kh_U8StringView {
  union {
    struct {
      kh_U8CharPtr    _data;
      kh_U8StringSize _size;
    } _cstr_raw;
    struct kh_U8String * _u8str;
  };
  enum kh_U8StringAttributes _attrib;
};

kh_U8CharPtr
kh_u8stringview_data(
  KH_ANT_ARG_IN struct kh_U8StringView * view
);

kh_U8StringSize
kh_u8stringview_size(
  KH_ANT_ARG_IN struct kh_U8StringView * view
);

kh_U8StringSize
kh_u8stringview_length(
  KH_ANT_ARG_IN struct kh_U8StringView * view
);

kh_bool
kh_u8stringview_from_u8string(
  KH_ANT_ARG_OUT struct kh_U8StringView * view,
  KH_ANT_ARG_IN  struct kh_U8String     * string
);

#define                                      \
kh_u8stringview_from_cstring(cstr) {         \
  ._cstr_raw {                               \
    ._data   = (kh_U8CharPtr)cstr,           \
    ._size   = sizeof(cstr),                 \
  },                                         \
  ._attrib = KH_U8STRING_ATTRIB_IS_RCSTRING, \
}


//-[ String View ]--------------------------------------------------------------

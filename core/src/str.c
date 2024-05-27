#include <kh-core/str.h>

kh_U8Char * kh_u8string_data(struct kh_U8String * string) {
  return kh_u8string_size(string) <= 8 ? string->_idata : string->_pdata;
}

kh_U8StringSize kh_u8string_size(struct kh_U8String * string) {
  return string->_size;
}

// [28.05.2024 @u16rogue TODO] implement u8string length
kh_U8StringSize kh_u8string_length(struct kh_U8String * string) {
  KH_DEBUG_UNUSED(string);
  return 0;
}

kh_U8CharPtr kh_u8stringview_data(struct kh_U8StringView * view) {
  if (view->_attrib & KH_U8STRING_ATTRIB_U8STR) {
    return kh_u8string_data(view->_u8str);
  } else if (view->_attrib & KH_U8STRING_ATTRIB_IS_RCSTRING) {
    return view->_cstr_raw._data;
  }

  return KH_PNIL;
}

kh_U8StringSize kh_u8stringview_size(struct kh_U8StringView * view) {
  if (view->_attrib & KH_U8STRING_ATTRIB_U8STR) {
    return kh_u8string_size(view->_u8str);
  } else if (view->_attrib & KH_U8STRING_ATTRIB_IS_RCSTRING) {
    return view->_cstr_raw._size;
  }

  return 0;
}

kh_U8StringSize kh_u8stringview_length(struct kh_U8StringView * view) {
  if (view->_attrib & KH_U8STRING_ATTRIB_U8STR) {
    return kh_u8string_length(view->_u8str);
  } else if (view->_attrib & KH_U8STRING_ATTRIB_IS_RCSTRING) {
    return view->_cstr_raw._size - 1;
  }

  return 0;
}

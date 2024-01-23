#include <kh-core/str.h>

kh_U8Char * kh_u8string_data(struct kh_U8String * u8string) {
  return u8string->size <= 8 ? u8string->idata : u8string->pdata;
}

kh_U8CharPtr kh_u8stringview_data(struct kh_U8StringView * view) {
  return view->data;
}

kh_bool kh_u8stringview_from_u8string(struct kh_U8StringView * view, struct kh_U8String * string) {
  view->data = kh_u8string_data(string);
  view->size = string->size;
  return KH_TRUE;
}

kh_bool kh_u8char_is_alphabetic(kh_U8Char character) {
  return (character >= 'A' && character <= 'Z') || (character >= 'a' && character <= 'z');
}

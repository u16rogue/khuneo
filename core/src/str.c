#include <kh-core/str.h>

kh_utf8 * kh_u8string_data(struct kh_U8String * u8string) {
  return u8string->size <= 8 ? u8string->idata : u8string->pdata;
}

enum kh_U8FromCStringResponse kh_u8string_from_cstr(struct kh_U8String * out, const char * string) {
  (void)out;
  (void)string;
  return KH_U8_FROM_CSTR_OK;
}

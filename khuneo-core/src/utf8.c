#include <kh-core/utf8.h>

kh_bool kh_utf8_is_alpha(const kh_utf8 c) {
  return (
           (c >= 'A' && c <= 'Z') ||
           (c >= 'a' && c <= 'z')
         )
         ? KH_TRUE : KH_FALSE;
}

kh_bool kh_utf8_is_num(const kh_utf8 c) {
  return (c >= '0' && c <= '9')
         ? KH_TRUE : KH_FALSE;
}

kh_bool kh_utf8_is_hex(const kh_utf8 c) {
  return ( kh_utf8_is_num(c) == KH_TRUE || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f') )
         ? KH_TRUE : KH_FALSE;
}

kh_bool kh_utf8_is_whitespace(const kh_utf8 c) {
  return (c == '\r' || c == '\t' || c == '\n' || c == ' ')
         ? KH_TRUE : KH_FALSE;
}

kh_u8 kh_utf8_char_sz(const kh_utf8 c) {
  kh_u8 mask = *(kh_u8 *)&c & 0xF0;
  if (~mask & 0x80)
    return 1;

  #if 0
  // mask & 1100.0000 == 1000.0000 // Checks if the byte is 1000.0000 which means its not the starting byte
  if ((mask & 0xC0) == 0x80) {
    return -1; // Invalid starting character (not head)
  }
  #endif

  kh_sz sz = -1;
  while (mask & 0x80) {
    mask <<= 1;
    ++sz;
  }

  if (!sz)
    return KH_UTF8_INVALID_SZ;
  return sz + 1;
}

kh_bool kh_utf8_is_utf8_lazy(const kh_utf8 c) {
  return (c & 0x80) ? KH_TRUE : KH_FALSE;
}

kh_u8 kh_utf8_char_to_num(const kh_utf8 c) {
  if (c >= '0' && c <= '9')
    return c - '0';
  return KH_UTF8_INVALID_C2N;
}

kh_u8 kh_utf8_hexchar_to_nibble(const kh_utf8 c) {
  kh_u8 v = kh_utf8_char_to_num(c);
  if (v != KH_UTF8_INVALID_C2N)
    return v;
  if (c >= 'A' && c <= 'F') {
    return c - 'A' + 0xA;
  } else if (c >= 'a' && c <= 'f') {
    return c - 'a' + 0xa;
  }

  return KH_UTF8_INVALID_NIBBLE;
}

kh_bool kh_utf8_strcmp(const struct kh_utf8sp * const a, const struct kh_utf8sp * const b) {
  if (a->size != b->size) {
    return KH_FALSE;
  }

  for (kh_sz i = 0; i < a->size; ++i) {
    if (a->buffer[i] != b->buffer[i]) {
      return KH_FALSE;
    }
  }

  return KH_TRUE;
}

kh_bool kh_utf8_unsafe_strcpy(struct kh_utf8sp * dest, const struct kh_utf8sp * const src) {
  for (kh_sz i = 0; i < src->size; ++i) {
    dest->buffer[i] = src->buffer[i];
  }
  dest->size = src->size;
  return KH_TRUE;
}

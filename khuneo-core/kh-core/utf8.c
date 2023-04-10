#include "utf8.h"

kh_sz kh_utf8_char_len(const kh_utf8 c) {
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
    return -1;
  return sz + 1;
}

kh_u8 kh_utf8_hexchar_to_nibble(const kh_utf8 c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else if (c >= 'A' && c <= 'F') {
    return c - 'A' + 0xA;
  } else if (c >= 'a' && c <= 'f') {
    return c - 'a' + 0xa;
  }

  return -1;
}

kh_bool kh_utf8_is_alpha(const kh_utf8 c) {
  return c >= 'A' && c <= 'Z' ||
         c >= 'a' && c <= 'z';
}

kh_bool kh_utf8_is_num(const kh_utf8 c) {
  return c >= '0' && c <= '9';
}

kh_bool kh_utf8_is_hex(const kh_utf8 c) {
  return kh_utf8_is_num(c) || c >= 'A' && c <= 'F' || c >= 'a' && c <= 'f';
}

kh_bool kh_utf8_is_whitespace(const kh_utf8 c) {
  return c == '\r' || c == '\t' || c == '\n';
}

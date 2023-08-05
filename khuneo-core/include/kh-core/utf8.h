#pragma once

#include "types.h"

/*
 *  Determine the size of a UTF8
 *  > Returns a KH_UTF8_INVALID_LEN when invalid.
 */
kh_u8 kh_utf8_char_sz(const kh_utf8 c);
#define KH_UTF8_INVALID_SZ KH_U8_MAX

/*
 *  Determines whether a character is UTF8 or not
 *  NOTE: Check implementation is made lazily for speed
 *  this is only for determining if a character byte is
 *  UTF8 accompanied by a verification later.
 *  !!! DO NOT USE IF INTEGRITY AND VALIDITY IS IMPORTANT !!!
 */
kh_bool kh_utf8_is_utf8_lazy(const kh_utf8 c);

/*
 *  Converts a character representing a number to its
 *  numerical value
 *
 *  Returns a KH_U8_INVALID on failure.
 */
kh_u8 kh_utf8_char_to_num(const kh_utf8 c);
#define KH_UTF8_INVALID_C2N KH_U8_MAX

/*
 *  Converts a character representing a hex value to its
 *  4 bit / nibble numerical value.
 *
 *  Returns a KH_U8_INVALID on failure.
 */
kh_u8 kh_utf8_hexchar_to_nibble(const kh_utf8 c);
#define KH_UTF8_INVALID_NIBBLE KH_U8_MAX

/*
 *  Compares a kh_utf8 string if they match
 */
kh_bool kh_utf8_strcmp(const kh_utf8 * a, const kh_utf8 * b);

/*
 *  Copies a kh_utf8 string from src to dst
 *  NOTE: Intentionally straight forward implementation for perf
 *  HIGHLY UNSAFE! Only use where buffer checks are done.
 */
kh_bool kh_utf8_strcpy(kh_utf8 * dst, const kh_utf8 * src);

#if 0
/*
 *  Converts a u32 value to a string
 */
kh_bool kh_utf8_u32_str(const kh_u32 n, kh_utf8 * out, kh_sz sz);
#endif

kh_bool kh_utf8_is_alpha(const kh_utf8 c);
kh_bool kh_utf8_is_num(const kh_utf8 c);
kh_bool kh_utf8_is_hex(const kh_utf8 c);
kh_bool kh_utf8_is_whitespace(const kh_utf8 c);


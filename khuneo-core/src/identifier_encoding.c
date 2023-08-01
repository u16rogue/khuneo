#include "kh-core/types.h"
#include <kh-core/identifier_encoding.h>
//
// $ 35 -> 0 (48) ~ 9 (57) -> A (65) ~ Z (90) -> _ (95) -> a (97) ~ z (122) 
static const kh_utf8 encoding_map[] = { "$0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz" };

kh_u8 kh_iden_encode(const kh_utf8 character) {
  if (character == '$') {
    return 0;
  } else if (character >= '0' && character <= '9') {
    return character - '0' + 1;
  } else if (character >= 'A' && character <= 'Z') {
    return character - 'A' + 11;
  } else if (character == '_') {
    return 37;
  } else if (character >= 'a' && character <= 'z') {
    return character - 'a' + 38;
  }

  return KH_IDEN_INVALID_ENCODE_BFLAG;
}

kh_bool kh_iden_encode_valid(const kh_u8 encoded_character) {
  return ((encoded_character & KH_IDEN_INVALID_ENCODE_BFLAG) == 0 ? KH_TRUE : KH_FALSE);
}

#include <stdio.h>

kh_bool kh_iden_encoder(const kh_utf8 * in_string, const kh_sz string_size, kh_vptr out_buffer, const kh_sz buffer_size) {
  // Check if we have enough `buffer_size` for the entire `string_size`
  if (buffer_size * 8 < string_size * 6) {
    return KH_FALSE;
  }

  kh_u8 * out = (kh_u8 *)out_buffer;
  for (kh_sz i = 0; i < string_size; ++i) {
    const kh_sz set = i % 4;
    kh_u32 * ind = (kh_u32 *)(out + (i / 4 * 3));

    if (set == 0) {
      *ind = 0;
    }

    kh_u8 enc = kh_iden_encode(in_string[i]);
    if (kh_iden_encode_valid(enc) != KH_TRUE) {
      return KH_FALSE;
    }

    *ind |= ( (kh_u32)enc ) << (6 * set);
  }

  return KH_TRUE;
}

// TODO: [01/07/2023] Broken. Fix calculation of buffer size to string count
kh_bool kh_iden_decoder(const kh_vptr in_buffer, kh_sz buffer_size, kh_utf8 * out_string, kh_sz string_buffer_size) {
  if (string_buffer_size * 6 < buffer_size * 8) {
    return KH_FALSE;
  }

  kh_sz istr = 0;
  kh_u8 * out = (kh_u8 *)in_buffer;
  for (kh_sz i = 0; i < buffer_size * 8 / 6; ++i) {
    const kh_sz set = i % 4;
    kh_u32 * ind = (kh_u32 *)(out + (i / 4 * 3));
    out_string[istr] = encoding_map[ *ind >> (6 * set) & 0x3F ];
    ++istr;
  }

  return KH_TRUE;
}

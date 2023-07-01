#include <kh-core/identifier_encoding.h>

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

kh_bool kh_iden_encoder(const kh_utf8 * in_string, const kh_sz string_size, kh_vptr out_buffer, const kh_sz buffer_size) {
  // Check if we have enough `buffer_size` for the entire `string_size`
  if (buffer_size * 8 < string_size * 6) {
    return KH_FALSE;
  }
   
  kh_u8 * out      = (kh_u8 *)out_buffer;
  kh_u8   enc_char = KH_U8_MAX;

  for (kh_sz i_str = 0; i_str < string_size; ++i_str) {

    if (enc_char == KH_U8_MAX) {
      enc_char = kh_iden_encode(in_string[i_str]);
      if (!kh_iden_encode_valid(enc_char)) {
        return KH_FALSE;
      }
    }

    const kh_bool is_last = (i_str == string_size - 1) ? KH_TRUE : KH_FALSE;
    const kh_sz   out_idx = i_str - ( (1 + i_str) / 4);
    const kh_sz   set     = i_str % 3;

    const kh_u8 and_table[] = { 0x03, 0x0F, 0x00 };

    out[out_idx] |= enc_char >> (set * 2);
    if (is_last == KH_FALSE) {
      enc_char = kh_iden_encode(in_string[i_str + 1]);
      if (!kh_iden_encode_valid(enc_char)) {
        return KH_FALSE;
      }
      out[out_idx] |= (enc_char & and_table[set] << (6 - (set * 2)) );
    }
  }

  return KH_TRUE;
}

// $ 35 -> 0 (48) ~ 9 (57) -> A (65) ~ Z (90) -> _ (95) -> a (97) ~ z (122) 
static const kh_utf8 encoding_map[] = { "$0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ_abcdefghijklmnopqrstuvwxyz" };

kh_bool kh_iden_decoder(const kh_vptr in_buffer, kh_sz buffer_size, kh_utf8 * out_string, kh_sz string_buffer_size) {
  const kh_u8 * in = (kh_u8 *)in_buffer;

  (void)encoding_map;
  (void)in;
  (void)out_string;
  (void)string_buffer_size;

  kh_sz istr = 0;
  for (kh_sz i_buff = 0; i_buff < buffer_size; ++i_buff) {
    const kh_sz set = i_buff % 3;

    if (set == 0) {
      out_string[istr] = encoding_map[ in[i_buff] & 0x3F ];
      kh_u8 least = (in[i_buff] & 0xC0) >> 6;
      least |= (in[i_buff + 1] & 0xF) << 2;
      out_string[istr + 1] = encoding_map[least];
      istr += 2;
    } else if (set == 1) {
      kh_u8 least = (in[i_buff] >> 4);
      least |= (in[i_buff + 1] & 0x03) << 4;
      out_string[istr] = encoding_map[least];
      istr += 1;
    } else if (set == 2) {
      out_string[istr] = encoding_map[ in[i_buff] >> 2 ];
    }
  }

  return KH_TRUE;
}

#pragma once

#include <kh-core/types.h>

/*
 *  Encodes a single kh_utf8 character into its encoded bit representation
 */
kh_u8 kh_iden_encode(const kh_utf8 character);
#define KH_IDEN_INVALID_ENCODE_BFLAG 0xC0 // Represents the bits that should never be set when a character is encoded. Either bit set signifies its invalidity. (1100_0000)

/*
 *  Checks the result of `kh_iden_encode` bit's if the encoding result is valid or not. Simply checks for `KH_IDEN_INVALID_ENCODE_BFLAG`
 */
kh_bool kh_iden_encode_valid(const kh_u8 encoded_character);

kh_bool kh_iden_encoder(const kh_utf8 * in_string, const kh_sz in_strsz, kh_vptr out_buffer, const kh_sz buffer_size);

kh_bool kh_iden_decoder(const kh_vptr in_buffer, kh_sz buffer_size, kh_utf8 * out_string, kh_sz string_buffer_size);

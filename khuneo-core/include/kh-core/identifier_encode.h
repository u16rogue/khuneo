#pragma once

#include <kh-core/types.h>

kh_bool kh_identifier_encoder(const kh_utf8 * in_string, kh_vptr out_buffer, kh_sz buffer_size);

kh_bool kh_identifier_decoder(const kh_vptr in_buffer, kh_sz buffer_size, kh_utf8 * out_string, kh_sz string_buffer_size);

#include <kh-core/identifier_encode.h>


kh_bool kh_identifier_encoder(const kh_utf8 * in_string, kh_vptr out_buffer, kh_sz buffer_size) {
  (void)in_string;
  (void)out_buffer;
  (void)buffer_size;
  return KH_FALSE;
}


kh_bool kh_identifier_decoder(const kh_vptr in_buffer, kh_sz buffer_size, kh_utf8 * out_string, kh_sz string_buffer_size) {
  (void)in_buffer;
  (void)buffer_size;
  (void)out_string;
  (void)string_buffer_size;
  return KH_FALSE;
}

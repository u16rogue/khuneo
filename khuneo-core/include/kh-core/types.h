#pragma once

typedef unsigned char kh_u8;
#define KH_U8_MAX 0xFF
typedef char          kh_i8;
#define KH_I8_MAX 0x7F

typedef unsigned short kh_u16;
typedef short          kh_i16;

typedef unsigned int   kh_u32;
typedef int            kh_i32;

typedef unsigned long long kh_u64;
#define KH_U64_MAX 0xFFFFFFFFFFFFFFFF
typedef long long          kh_i64;
#define KH_I64_MAX 0x7FFFFFFFFFFFFFFF

typedef float  kh_f32;
typedef double kh_f64;

typedef void * kh_vptr;
#define KH_NULLPTR 0

#define KH_SZ_MAX KH_U64_MAX
typedef kh_u64 kh_sz;

typedef kh_u8  kh_bool;
#define KH_TRUE 1
#define KH_FALSE 0

typedef kh_i8 kh_utf8;

/*
 *  # UTF8 Structure pointer
 *  - Represents a UTF8 string buffer.
 *
 *  NOTE: `size` refers the the literal size in bytes that the buffer relatively occupies (used)
 *  `size` does not denote `length` as this can vary with UTF8.
 *  NOTE: Buffer may or may not include a null terminator
 *  therefore size should be taken into consideration.
 *  NOTE: Size should include the null terminator if exists
 *  NOTE: When `buffer` points to a constant buffer `kh_utf8sp` should also be constant.
 */
struct kh_utf8sp {
  kh_sz     size;  // Size refers the the size in bytes of the content and not by the size of the buffer
                   // Eg. [h][e][l][l][o][\0] = 6 | [h][e][l][l][o][ ] = 5 | [h][e][l][o][ ][ ] = 4
  kh_utf8 * buffer;
};

/*
 *  Extends the `kh_utf8sp` to include
 *  the `buffer`'s absolute size. (Including unused)
 */
struct kh_utf8sp_s {
  struct kh_utf8sp str;
  kh_sz buffer_size;
};

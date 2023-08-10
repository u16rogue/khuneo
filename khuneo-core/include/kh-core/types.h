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
 *  NOTE: Buffer may or may not include a null terminator
 *  therefore size should be taken into consideration.
 *  NOTE: Size should include the null terminator if exists
 */
struct kh_utf8sp {
  kh_sz     size;
  kh_utf8 * buffer;
};


#pragma once

// -- Types

typedef unsigned char kh_u8;
typedef char kh_i8;

typedef short kh_i16;
typedef unsigned short kh_u16;

typedef int kh_i32;
typedef unsigned int kh_u32;

typedef long long kh_i64;
typedef unsigned long long kh_u64;

typedef void * kh_vptr;
#define KH_NIL  0
#define KH_PNIL 0

// -- Boolean
typedef enum {
  KH_FALSE = 0,
  KH_TRUE  = 1,
} kh_bool;

// -- Annotation
#define KH_ANT_ARG_IN     // Function argument receives a value
#define KH_ANT_ARG_OUT    // Function argument outputs a value
#define KH_ANT_ARG_INOUT  // Function argument can receive and output a value
#define KH_ANT_ARG_OPTNIL // Function argument is optional, can either be
                          // KH_PNIL or the function's designated default value

#define KH_UNUSED(x) ((void)x)

#ifdef KH_DEBUG_ALLOW_UNUSED
  // [26.04.2024 @u16rogue] Use this macro if the unused variable is temporary
  // (mainly indev state). Prevents unused from being pushed to release
  #define KH_DEBUG_UNUSED(x) ((void)x)
#else
  #define KH_DEBUG_UNUSED(x)
#endif

// -- Helper Macro's
#define kh_array_length(arr) \
  (sizeof(arr) / sizeof(arr[0]))

#define KH_FAIL_BIT      0x1
#define KH_MSGBIT_SET(x) (x << 1)

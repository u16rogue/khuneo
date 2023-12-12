#pragma once

typedef unsigned char kh_u8;
typedef char kh_i8;

typedef short kh_i16;
typedef unsigned short kh_u16;

typedef int kh_i32;
typedef unsigned int kh_u32;

typedef long long kh_i64;
typedef unsigned long long kh_u64;

typedef void * kh_vptr;
#define KH_PNIL 0

// -- Annotation
#define KH_ANT_ARG_IN
#define KH_ANT_ARG_OUT
#define KH_ANT_ARG_INOUT
#define KH_ANT_ARG_OPTNIL

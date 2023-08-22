#pragma once

#define kh_narray(array) (sizeof(array) / sizeof(array[0]))

#define _kh_mglue(x, y) x##y
#define kh_mglue(x, y) _kh_mglue(x, y)

#define kh_bool_expr(expr) ((expr) ? KH_TRUE : KH_FALSE)

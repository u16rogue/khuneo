#pragma once
#include <kh-core/types.h>

#define KH_EXTRA_CASE_STR(e) case e: {        \
  static kh_utf8 * const s = &((#e)[offset]); \
  static const struct kh_utf8sp v = {         \
    .size = strlen(s),                        \
    .buffer = s,                              \
  };                                          \
  return &v;                                  \
}

#define KH_EXTRA_RETURN_CCSTR(str) do { \
  static const struct kh_utf8sp v = {   \
    .size   = sizeof(str) - 1,          \
    .buffer = str,                      \
  };                                    \
  return &v;                            \
} while (0);

#define KH_EXTRA_SET_OFFSET(substr) const int offset = sizeof(substr) - 1

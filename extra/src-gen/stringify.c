#include <kh-gen-extra/stringify.h>

#define case_str(x) \
  case x: r = #x; break

#undef case_str

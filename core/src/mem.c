#include <kh-core/mem.h>
#include <kh-core/impldec.h>

#if !defined(KH_USERDEF_MEM)

#include <stdlib.h>

void * kh__impl_alloc(KH_ANT_ARG_OPTNIL kh_vptr address, kh_u64 size, KH_ANT_ARG_INOUT struct kh_ImplAllocEx * extended) { (void)extended;
  if (address) {
    return realloc(address, size);
  } else {
    return malloc(size);
  }
}
 
kh_u64 kh__impl_dealloc(KH_ANT_ARG_IN kh_vptr address, kh_u64 size, KH_ANT_ARG_INOUT struct kh_ImplDeAllocEx * extended) { (void)extended;
  free(address);
  return size;
}

#endif

void * kh_alloc(kh_u64 size) {
  return kh__impl_alloc(KH_PNIL, size, KH_PNIL);
}

void * kh_realloc(kh_vptr ptr, kh_u64 newsize) {
  return kh__impl_alloc(ptr, newsize, KH_PNIL);
}

kh_u64 kh_dealloc(kh_vptr ptr, kh_u64 size) {
  return kh__impl_dealloc(ptr, size, KH_PNIL);
}


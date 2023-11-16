#include <kh-core/alloc.h>
#include <kh-core/impldec.h>

#if defined(KH_USE_STD_ALLOC_DEFAULT)
#include <stdlib.h>

void * kh__impl_alloc(KH_ARG_OPTNIL kh_vptr address, kh_u64 size, KH_ARG_INOUT struct kh_ImplAllocEx * extended) {
  (void)extended;
  void * p = KH_PNIL;
  if (address != KH_PNIL) {
    p = realloc(address, size);
  } else {
    p = malloc(size);
  }
  return p;
}
 
kh_u64 kh__impl_dealloc(KH_ARG_IN kh_vptr address, kh_u64 size, KH_ARG_INOUT struct kh_ImplDeAllocEx * extended) {
  (void)extended;
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

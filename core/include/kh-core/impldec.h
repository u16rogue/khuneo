#pragma once

/*
 *  Implementer declaration header
 *  ---------------------------------
 *  Contains decleration of implementer defined definitions.
 */

#include <kh-core/defs.h>

enum kh_ImplAllocHints {
  KH_ALLOC_HINT_NONE = 0x0,
};

struct kh_ImplAllocEx {
  enum kh_ImplAllocHints hints;
};

/*
 *  Allocation declaration.
 */
extern
void *
kh__impl_alloc(
  KH_ARG_OPTNIL kh_vptr address,                // Address preferred or to re allocate
  kh_u64 size,                                  // Memory size
  KH_ARG_INOUT struct kh_ImplAllocEx * extended // Extended information
);

enum kh_ImplDeAllocHints {
  KH_DEALLOC_HINT_NONE = 0x0,
};

struct kh_ImplDeAllocEx {
  enum kh_ImplDeAllocHints hints;
};

/*
 *  Deallocation declaration
 */
extern
kh_u64
kh__impl_dealloc(
  KH_ARG_IN kh_vptr address,
  kh_u64 size,
  KH_ARG_INOUT struct kh_ImplDeAllocEx * extended
);

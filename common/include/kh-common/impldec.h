#pragma once

/*
 *  Implementer declaration header
 *  ------------------------------------
 *  Contains the decleration of consumer
 *  implementable components.
 *  ------------------------------------
 *  Define these function declarations on
 *  your project to override the memory
 *  management.
 */

#include <kh-common/defs.h>

//------------------------------------------------------------ 
//--- Allocator implementation

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
void *                                              // Address of Allocated memory
kh__impl_alloc(
  kh_vptr address KH_ANT_ARG_OPTNIL                 // Preferred address or address for re-allocation
, kh_u64 size,                                      // Size in bytes to allocate
  struct kh_ImplAllocEx * extended KH_ANT_ARG_INOUT // Extended information
);

//------------------------------------------------------------ 
//--- Deallocator implementation

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
kh_u64                                                // Size of memory free'd (opt)
kh__impl_dealloc(
  kh_vptr address KH_ANT_ARG_IN                       // Memory address to deallocate
, kh_u64  size                                        // Size of the memory to deallocate
, struct kh_ImplDeAllocEx * extended KH_ANT_ARG_INOUT // Extended information
);

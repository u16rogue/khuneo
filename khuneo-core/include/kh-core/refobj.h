#pragma once

#include <kh-core/types.h>

/*
 *  Reason for a Reference Object Resource Callback invocation.
 */
enum kh_refobj_rcb_reason {
  KH_REFOBJ_RCB_REASON_LOCK_AND_WAIT,
  KH_REFOBJ_RCB_REASON_UNLOCK,
};

/*
 *  Data provided to a reference object's resource callback that
 *  contains relevant information.
 */
struct kh_refobj_rcb_info {
  enum kh_refobj_rcb_reason reason; // Reason for the invocation
};

typedef kh_bool(*kh_refobj_rcb_fnt)(struct kh_refobj_rcb_info *); // Function type for Reference Object's Resource callback function
#define KH_REFOBJ_RCB_NO_CALLBACK KH_NULLPTR

/*
 *  A reference object. Allows an abstract value
 *  to have reference counting based on movement.
 *
 *  This is used to manage resources that has an
 *  undertiministic lifetime.
 */
struct kh_refobj {
  kh_vptr           _object;            // Pointer to abstract object
  kh_u8             _count;             // Reference count
  kh_refobj_rcb_fnt _resource_callback; // Resource management callback
};

/*
 *  An instance of a reference to a `kh_refobj`
 *  [!] STRICLY ONLY use this value with kh_refobj_* functions
 *  NEVER handle manually.
 */
typedef struct _kh_refobji { kh_u8 __compatibility_padding_donottouch; } * kh_refobji;

// Indicates that the reference object instance is invalid.
#define KH_REFOBJ_INVALID_IREF KH_NULLPTR

/*
 *  Initializes a `kh_refobj` struct.
 *  [!] Only use when structure is previously unused.
 *
 *  > Parameter `resource_callback` is optional, provide `KH_NULLPTR` if no
 *  implementation is provided.
 */
kh_bool kh_refobj_init(struct kh_refobj * ro, kh_vptr value, kh_refobji * out_firstref, kh_refobj_rcb_fnt resource_callback);

/*
 *  Uninitializes a `kh_refobj` struct.
 */
kh_bool kh_refobj_uninit(struct kh_refobj * ro);

/*
 *  Moves a `kh_refobj` reference from one `kh_refobji` to another then
 *  sets the latter into `KH_REFOBJ_INVALID_IREF`.
 *  > Moving does not acquire and inc/dec the refrence count.
 */
kh_bool kh_refobj_imove(kh_refobji * inout_source, kh_refobji * out_dest);

/*
 *  Copies a `kh_refobj` reference from one `kh_refobji` to another which 
 *  (if applicable) acquires and increases the ref count.
 */
kh_bool kh_refobj_icopy(kh_refobji * in_source, kh_refobji * out_dest);

/*
 *  Moves a `kh_refobj` reference from one `kh_refobji` to a function argument
 *  through a return then sets the latter into `KH_REFOBJ_INVALID_IREF`.
 *  > Moving does not acquire and inc/dec the refrence count.
 *  [!] Only use to pass a `kh_refobji` variable to a `kh_refobji` function
 *  argument.
 */
kh_refobji kh_refobj_imovearg(kh_refobji * inout_source);

/*
 *  Copies a `kh_refobj` reference from one `kh_refobji` to a function argument
 *  which (if applicable) acquires and increases the ref count.
 *  [!] Only use to pass a `kh_refobji` variable to a `kh_refobji` function
 *  argument.
 */
kh_refobji kh_refobj_icopyarg(kh_refobji * in_source);

/*
 *  Removes a `kh_refobj` reference from a `kh_refobji` and (if applicable)
 *  acquires and decreases the ref count then sets the instance to `KH_REFOBJ_INVALID_IREF`
 */
kh_bool kh_refobj_iremove(kh_refobji * inout_source);

/*
 *  Checks if a reference object instance still has a non zero ref count
 */
kh_bool kh_refobj_ialive(kh_refobji ro);

/*
 *  Checks if a reference object still has a non zero ref count
 */
kh_bool kh_refobj_alive(struct kh_refobj * ro);

/*
 *  Get the internal object value
 */
kh_vptr kh_refobj_get_object(kh_refobji iro);

/*
 *  Get the reference count from a ref instance
 */
kh_sz kh_refobj_get_ref_count(kh_refobji iro);

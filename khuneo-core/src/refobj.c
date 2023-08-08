#include "kh-core/types.h"
#include <kh-core/refobj.h>

static struct kh_refobj * as_refobject(kh_refobji iref) {
  return (struct kh_refobj *)iref;
}

#if 0
static kh_bool verify_iref_has_refcount(kh_refobji inout) {
  return as_refobject(inout)->_count != 0
         ? KH_TRUE : KH_FALSE;
}

static kh_bool verify_iref_is_valid(kh_refobji out) {
  return out == KH_REFOBJ_INVALID_IREF
         ? KH_FALSE : KH_TRUE;
}
#endif

static kh_bool resource_acquire(kh_refobji i) {
  if (as_refobject(i)->_resource_callback == KH_NULLPTR) {
    return KH_TRUE;
  }

  struct kh_refobj_rcb_info info;
  info.reason = KH_REFOBJ_RCB_REASON_LOCK_AND_WAIT;
  return as_refobject(i)->_resource_callback(&info);
}

static kh_bool resource_release(kh_refobji i) {
  if (as_refobject(i)->_resource_callback == KH_NULLPTR) {
    return KH_TRUE;
  }

  struct kh_refobj_rcb_info info;
  info.reason = KH_REFOBJ_RCB_REASON_UNLOCK;
  return as_refobject(i)->_resource_callback(&info);
}

kh_bool kh_refobj_init(struct kh_refobj * ro, kh_vptr value, kh_refobji * out_firstref, kh_refobj_rcb_fnt resource_callback) {
  ro->_resource_callback = resource_callback;
  ro->_object   = value;
  ro->_count    = 1;
  *out_firstref = (kh_refobji)ro;

  return KH_TRUE;
}

kh_bool kh_refobj_imove(kh_refobji * inout_source, kh_refobji * out_dest) {
  *out_dest = *inout_source;
  *inout_source = KH_REFOBJ_INVALID_IREF;
  return KH_TRUE;
}

kh_bool kh_refobj_icopy(kh_refobji * in_source, kh_refobji * out_dest) {
  struct kh_refobj * ro = as_refobject(*in_source);

  if (resource_acquire(*in_source) == KH_FALSE) {
    return KH_FALSE;
  }

  ++ro->_count;
  
  if (resource_release(*in_source) == KH_FALSE) {
    return KH_FALSE;
  }

  *out_dest = *in_source;
  return KH_TRUE;
}

kh_refobji kh_refobj_imovearg(kh_refobji * in_source) {
  kh_refobji out = *in_source;
  *in_source = KH_REFOBJ_INVALID_IREF;
  return out;
}

kh_refobji kh_refobj_icopyarg(kh_refobji * in_source) {
  struct kh_refobj * ro = as_refobject(*in_source);

  if (resource_acquire(*in_source) == KH_FALSE) {
    return KH_REFOBJ_INVALID_IREF;
  }

  ++ro->_count;

  if (resource_release(*in_source) == KH_FALSE) {
    return KH_REFOBJ_INVALID_IREF;
  }

  return *in_source;
}

kh_bool kh_refobj_iremove(kh_refobji * inout_source) {
  struct kh_refobj * ro = as_refobject(*inout_source);

  if (resource_acquire(*inout_source) == KH_FALSE) {
    return KH_FALSE;
  }

  --ro->_count;

  if (resource_release(*inout_source) == KH_FALSE) {
    return KH_FALSE;
  }

  *inout_source = KH_REFOBJ_INVALID_IREF;
  return KH_TRUE;
}

kh_bool kh_refobj_ialive(kh_refobji ro) {
  return as_refobject(ro)->_count != 0 ? KH_TRUE : KH_FALSE;
}

kh_bool kh_refobj_alive(struct kh_refobj * ro) {
  return ro->_count != 0 ? KH_TRUE : KH_FALSE;
}

kh_vptr kh_refobj_get_object(kh_refobji iro) {
  return as_refobject(iro)->_object;
}

#include <kh-gen/psm.h>

enum kh_PSM_RES kh_psm_start(struct kh_PSMState * state) {
  return KH_PSM_RES_FAIL;
}

enum kh_PSM_RES kh_psm_end(struct kh_PSMState * state) {
  return KH_PSM_RES_FAIL;
}

enum kh_PSM_RES kh_psm_feed(struct kh_PSMState * state
                          , const kh_U8Char * content
                          , kh_U8StringSize content_size
                          ) {
  return KH_PSM_RES_FAIL;
}

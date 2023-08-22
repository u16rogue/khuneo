#define KH_EXTRA_CASE_STR(e) case e: return &((#e)[offset])
#define KH_EXTRA_SET_OFFSET(substr) const int offset = sizeof(substr) - 1

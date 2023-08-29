#pragma once
#if defined(KH_EXTRA_ASTGEN) && KH_EXTRA_ASTGEN == 1
#include <kh-core/types.h>
#include <kh-astgen/parser.h>

const struct kh_utf8sp * kh_extra_parser_tostr_ctx_status(enum kh_parser_status status);
const struct kh_utf8sp * kh_extra_parser_tostr_node_type(enum kh_parser_node_type type);

#endif // KH_EXTRA_ASTGEN

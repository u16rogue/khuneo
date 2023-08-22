#include <kh-extra/astgen/parser.h>
#include <kh-extra/helper.h>

const char * kh_extra_parser_tostr_ctx_status(enum kh_parser_status status) {
  KH_EXTRA_SET_OFFSET("KH_PARSER_STATUS_");
  if (status & KH_PARSER_STATUS_EXTENDED) return "<extended>";
  switch (status) {
    KH_EXTRA_CASE_STR(KH_PARSER_STATUS_OK);
    KH_EXTRA_CASE_STR(KH_PARSER_STATUS_MATCH);
    KH_EXTRA_CASE_STR(KH_PARSER_STATUS_NOMATCH);
    KH_EXTRA_CASE_STR(KH_PARSER_STATUS_EOB);
    KH_EXTRA_CASE_STR(KH_PARSER_STATUS_PASS);
    KH_EXTRA_CASE_STR(KH_PARSER_STATUS_UNKNOWN_WARNING);
    KH_EXTRA_CASE_STR(KH_PARSER_STATUS_UNKNOWN_ERROR);
    KH_EXTRA_CASE_STR(KH_PARSER_STATUS_SYNTAX_ERROR);

    case KH_PARSER_STATUS_EXTENDED:
      return "<error>";
  }

  return "<invalid>";
}

#pragma once

#include <kh-gen/parser.h>

typedef kh_u8 kh_ParserSM_Instruction;

// Parser State Machine Instruction

enum kh_ParserSM_I {
  KH_PSM_INOP,

  /* Match Keyword
  `KH_PSM_MATCH_KW @enum kh_TokenType`
  */
  KH_PSM_IMATCH_KW,

  /* Match whitespace
  */
  KH_PSM_IMATCH_WS,

  /* Optional whitespace
  */
  KH_PSM_IMAYBE_WS,

  /* Match identifier
  */
  KH_PSM_IMATCH_ID,

  /* Match symbol
  `KH_PSM_IMATCH_SYM @enum kh_TokenSymbol`
  */
  KH_PSM_IMATCH_SYM,

  /* Maybe symbol
  `KH_PSM_IMAYBE_SYM @enum kh_TokenSymbol`
  */
  KH_PSM_IMAYBE_SYM,

  /* Start of scope
  */
  KH_PSM_ISCP_START,

  /* End of scope
  */
  KH_PSM_ISCP_END,

  /* Acquire the current state selector index
  */
  KH_PSM_IACQUIRE,
};

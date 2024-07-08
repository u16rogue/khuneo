#pragma once

#include <kh-gen/parser.h>

typedef kh_u8 kh_ParserSM_Instruction;

// Parser State Machine Instruction

// [02.07.2024 @u16rogue REMARK] goofy name, pretty much means that if the bit flag is present
// the instruction needs to be "re-fed / consumes / consumed" a descriptor.
#define KH_PSM_HUNGRY 0x80

enum kh_ParserSM_I {
  KH_PSM_INOP       = 0,
  KH_PSM_EXIT       = 1,

  /* Optional whitespace
  */
  KH_PSM_IMAYBE_WS  = 2,

  /* Maybe symbol
  `KH_PSM_IMAYBE_SYM @enum kh_TokenSymbol`
  */
  KH_PSM_IMAYBE_SYM = 3,

  /* Start of scope
  */
  KH_PSM_ISCP_START = 4,

  /* End of scope
  */
  KH_PSM_ISCP_END   = 5,

  /* Acquire the current state selector index
  */
  KH_PSM_IACQUIRE   = 6,

  /* Match Keyword
  `KH_PSM_MATCH_KW @enum kh_TokenType`
  */
  KH_PSM_IMATCH_KW  = KH_PSM_HUNGRY | 0,

  /* Match whitespace
  */
  KH_PSM_IMATCH_WS  = KH_PSM_HUNGRY | 1,

  /* Match identifier
  */
  KH_PSM_IMATCH_ID  = KH_PSM_HUNGRY | 2,

  /* Match symbol
  `KH_PSM_IMATCH_SYM @enum kh_TokenSymbol`
  */
  KH_PSM_IMATCH_SYM = KH_PSM_HUNGRY | 3,
};

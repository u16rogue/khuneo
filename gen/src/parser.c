#include <kh-gen/parser.h>
#include <kh-gen/psm.h>

typedef kh_ParserSM_Instruction Instr;

//- Constants ------------------------------------------------------------------

static const kh_u8 INVALID_PC_SET_INDEX = 0xFF;

//- Instruction Sets -----------------------------------------------------------

// (kw:var)(id:*)(?sym:':'(id:*))(?sym:'='(expr))(sym:';')
static const Instr match_var_decl[] = {
  KH_PSM_IMATCH_KW, KH_TOKEN_KEYWORD_VAR,
  KH_PSM_IACQUIRE,
  KH_PSM_IMAYBE_WS,
  KH_PSM_IMATCH_ID,
  KH_PSM_IMAYBE_WS,
  KH_PSM_IMAYBE_SYM, KH_TOKEN_SYMBOL_COL,
    KH_PSM_ISCP_START,
      KH_PSM_IMATCH_ID,
    KH_PSM_ISCP_END,
  KH_PSM_IMAYBE_WS,
  KH_PSM_EXIT,
};

static const Instr * const psm_instr_set[] = {
  match_var_decl,
};

//- Internal Functions ---------------------------------------------------------

static enum kh_ParserResult psm_execute(struct kh_ParserContext * const ctx
                                      , const kh_u8 set_index 
                                      , const struct kh_LexerDescription * description
                                      , const kh_U8Char * content
                                      ) {

  const Instr * instructions = psm_instr_set[set_index];

  do {
    const Instr instruction = instructions[ctx->_ins_index];
    if (instruction & KH_PSM_HUNGRY) {
      if (description == KH_PNIL) {
        // Current descriptor has already been consumed, ask for a new descriptor
        return KH_PARSER_RES_HUNGRY;
      }

      // [02.07.2024 @u16rogue OPTI] premature opt. benchmark in the future
      // Remove the highest bit to assist the compiler in optimizing it with a jump table
      switch(0x7f & instruction) {
        case 0x7f & KH_PSM_IMATCH_KW: {
          if (description->type == KH_TOKEN_TYPE_KEYWORD && description->keyword.what == instructions[ctx->_ins_index + 1]) {
            ctx->_ins_index += 2;
            description = KH_PNIL;
            return KH_PARSER_RES_HUNGRY;
          } else {
            return KH_PARSER_RES_PASS;
          }
          break;
        } 
        case 0x7f & KH_PSM_IMATCH_WS: {
          break;
        }
        case 0x7f & KH_PSM_IMATCH_ID: { break; }
        case 0x7f & KH_PSM_IMATCH_SYM: { break; }
      }
    } else {
      switch (instruction) {
        case KH_PSM_INOP: {
          ++ctx->_ins_index;
          goto KH_CONTINUE_PSMI_SWITCH;
        }
        case KH_PSM_EXIT: {
          ctx->_ins_index = 0;
          ctx->_set_index = INVALID_PC_SET_INDEX;
          return KH_PARSER_RES_END;
        }
        case KH_PSM_IACQUIRE: {
          ctx->_ins_index = set_index;
          goto KH_CONTINUE_PSMI_SWITCH;
        }
        case KH_PSM_IMAYBE_WS: { break; }
        case KH_PSM_IMAYBE_SYM: { break; }
        case KH_PSM_ISCP_START: { break; }
        case KH_PSM_ISCP_END: { break; }
      }
    }

    KH_CONTINUE_PSMI_SWITCH:;
  } while (1);
  
  return KH_PARSER_RES_INV_INSTR;
}

//- Header Definition ----------------------------------------------------------

enum kh_ParserResult kh_parser_start(struct kh_ParserContext * ctx) {
  ctx->_set_index = INVALID_PC_SET_INDEX;
  ctx->_ins_index = 0;
  return KH_PARSER_RES_OK;
}

enum kh_ParserResult kh_parser_stop(struct kh_ParserContext * ctx) {
  return KH_PARSER_RES_OK;
}

enum kh_ParserResult kh_parser_feed(struct kh_ParserContext * ctx,
                                    const struct kh_LexerDescription ** descriptions,
                                    const kh_U8Char ** contents,
                                    const kh_u8 ncount) {

  // Run through all the available descriptions
  for (kh_u8 i = 0; i < ncount; ++i) {
    if (ctx->_set_index == INVALID_PC_SET_INDEX) { // If no set index search for a possible set
      for (int is_index = 0; is_index < (int)kh_array_length(psm_instr_set); ++is_index) {

      }
    } else {
      //return psm_execute(ctx, psm_instr_set[ctx->_set_index], );
    }
  }

  
  

  return KH_PARSER_RES_OK;
}

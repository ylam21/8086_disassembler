#ifndef DECODER_H
#define DECODER_H

#include <stdio.h>
#include "../common.h"
#include "opcodes.h"
#include "../utils/io_utils.h"
#include "../utils/string_utils.h"

u8 opcode_not_used(t_ctx *ctx);
u8 fmt_modrm_common(t_ctx *ctx);
u8 fmt_imm_to_acc(t_ctx *ctx);
u8 fmt_segment_push_pop(t_ctx *ctx);
u8 handle_segment_override(t_ctx *ctx);
u8 handle_daa_das_aaa_aas(t_ctx *ctx);
u8 handle_inc_dec_push_pop_reg_16(t_ctx *ctx);
u8 fmt_jump(t_ctx *ctx);
u8 fmt_imm_to_rm(t_ctx *ctx);
u8 fmt_modrm_test_xchg_mov(t_ctx *ctx);
u8 fmt_mov_sreg_common(t_ctx *ctx);
u8 fmt_lea_mem_to_reg_16(t_ctx *ctx);
u8 fmt_pop_rm_16(t_ctx *ctx);
u8 fmt_xchg_reg16_to_acc(t_ctx *ctx);
u8 fmt_call_far(t_ctx *ctx);
u8 handle_cbw_cwd_wait_pushf_popf_sahf_lahf(t_ctx *ctx);
u8 fmt_mov_mem_to_reg(t_ctx *ctx);
u8 fmt_movs_cmps_stos_lods_scas(t_ctx *ctx);
u8 fmt_test_imm_to_acc(t_ctx *ctx);
u8 fmt_mov_imm_to_reg(t_ctx *ctx);
u8 handle_ret(t_ctx *ctx);
u8 fmt_les_lds_mem16_to_reg16(t_ctx *ctx);
u8 fmt_mov_imm_to_mem(t_ctx *ctx);
u8 handle_interrupt(t_ctx *ctx);
u8 fmt_rol_ror_rcl_rcr_sal_shr_sar(t_ctx *ctx);
u8 handle_aam_aad_xlat(t_ctx *ctx);


u8 fmt_esc(t_ctx *ctx);

#endif
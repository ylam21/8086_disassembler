#ifndef DECODER_H
#define DECODER_H

#include <stdio.h>
#include "../common.h"
#include "opcodes.h"
#include "../utils/io_utils.h"
#include "../utils/string_utils.h"

u8 handle_seg_override(t_ctx *ctx);
u8 opcode_not_used(t_ctx *ctx);
u8 fmt_modrm_common(t_ctx *ctx);
u8 fmt_imm_to_acc(t_ctx *ctx);
u8 fmt_segment_fix(t_ctx *ctx);
u8 handle_daa_das_aaa_aas(t_ctx *ctx);
u8 handle_cbw_cwd_wait_pushf_popf_sahf_lahf(t_ctx *ctx);
u8 inc_dec_push_pop_reg_16(t_ctx *ctx);
u8 fmt_jump(t_ctx *ctx);
u8 fmt_rm_imm(t_ctx *ctx);

#endif
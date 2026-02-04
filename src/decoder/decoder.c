#include "decoder.h"

typedef enum 
{
    OPCODE_MOV_RMTOREG = 0x88,
    OPCODE_MOV_IMTORM = 0xC6,
    OPCODE_MOV_IMTOREG = 0xB0,
    OPCODE_MOV_MEMTOACC = 0xA0,
    OPCODE_MOV_ACCTOMEM = 0xA2,
    OPCODE_MOV_RMTOSREG = 0x8E,
    OPCODE_MOV_SREGTORM = 0x8C
}   opcode_mov_type;

typedef enum
{
    MASK_MS_4 = 0xF0,
    MASK_MS_6 = 0xFC,
    MASK_MS_7 = 0xFE,
    MASK_MS_8 = 0xFF,
} mask_ms_n;

u8 *table_reg_w_zero[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
u8 *table_reg_w_one[]  = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
u8 *table_mem_address_calc[] = {"bx + si", "bx + di", "bp + si", "bp + di", "si", "di", "bp", "bx"};
u8 *table_sreg[] = {"es", "cs", "ss", "ds"};
u8 table_seg_override[4] = "";

static u8 *decode_reg(u8 REG, u8 W)
{
    if (W == 0)
    {
        return (table_reg_w_zero[REG]);
    }
    else
    {
        return (table_reg_w_one[REG]);
    }
}



/* Following Intel convention, if the displacement is two bytes,
   the most-significant byte is stored second in the instruction. */

static u8 *decode_rm(t_ctx *ctx, u8 RM, u8 MOD, u8 W)
{
    u8 *base = table_mem_address_calc[RM];
    u8 *prefix = NULL;

    if (ctx->seg_prefix < 4)
    {
        prefix = table_sreg[ctx->seg_prefix];
    }

    if (MOD == 0x0 && RM == 0x6)
    {
        /* DIRECT ADDRESS */
        /* When R/M = 0b110, then 16-bit displacement follows */ 
        u16 disp = ctx->b[2] | (ctx->b[3] << 8);
        return strjoin_fmt(ctx->a, "%?s[0x%04X]", prefix, disp);
    }
    else if (MOD == 0x0)
    {
        /* Memory Mode, no displacement */
        return strjoin_fmt(ctx->a, "%?s[%s]", prefix, base);
    }
    else if (MOD == 0x1)
    {
        /* Memory Mode, 8-bit displacement follows */
        u8 disp = ctx->b[2];
        return strjoin_fmt(ctx->a, "%?s[%s + %u]", prefix, base, disp);
    }
    else if (MOD == 0x2)
    {
        /* Memory Mode, 16-bit displacement follows */
        u16 disp = ctx->b[2] | (ctx->b[3] << 8);
        return strjoin_fmt(ctx->a, "%?s[%s + %u]", prefix, base, disp);
    }
    else
    {
        /* Register Mode (no displacement) */
        return decode_reg(RM, W);
    }
}

static u8 match_MODRM_with_offset(u8 MOD, u8 RM)
{
    if (MOD == 0x0)
    {
        /* Memory Mode, no displacement follows
        Except when R/M = 110, then 16-bit displacement follows */ 
        if (RM == 0x6)
        {
            return 4;
        }
        else
        {
            return 2;
        }
    }
    else if (MOD == 0x1)
    {
        /* Memory Mode, 8-bit displacement follows */
        return 3;
    }
    else if (MOD == 0x2)
    {
        /* Memory Mode, 16-bit displacement follows */
        return 4;
    }
    else
    {
        /* Register Mode (no displacement) */
        return 2;
    }
}

// NOTE: SUS
/* The second byte of a two-byte immediate value is the most significant. */
char *decode_imm_to_reg(t_arena *a, u8 W, u8 b2, u8 b3)
{
    if (W == 0x0)
    {
        return itoa(a, (i32)b2);
    }
    else
    {
        return concat_2_bytes_to_str(a, b2, b3);
    }
}

u8 opcode_not_used(t_ctx *ctx)
{
    return OFFSET_OPCODE_NOT_USED;
}

static u8 *decode_immediate(t_ctx *ctx, u8 S, u8 W, u8 *imm_ptr)
{
    if (W == 0)
    {
        return strjoin_fmt(ctx->a, "0x%02X", *imm_ptr);
    }
    else if (S == 1)
    {
        i16 val = (i16)((i8)imm_ptr[0]);
        return strjoin_fmt(ctx->a, "0x%04X", (u16)val);
    }
    else
    {
        u16 val = imm_ptr[0] | (imm_ptr[1] << 8);
        return strjoin_fmt(ctx->a, "0x%04X", val);
    }
}

u8 fmt_imm_to_rm(t_ctx *ctx)
{
    u8 *mnemonics[8] = {"add","or","adc","sbb","and","sub","xor","cmp"};
    u8 idx = (ctx->b[1] >> 3) & 0x7;
    u8 S = (ctx->b[0] >> 1) & 0x1;
    u8 W = ctx->b[0] & 0x1;
    u8 MOD = (ctx->b[1] >> 6) & 0x3;
    u8 RM = ctx->b[1] & 0x7;

    u8* field_rm = decode_rm(ctx, RM, MOD, W);

    u8 current_len = match_MODRM_with_offset(MOD, RM);
    u8 *imm_ptr = &ctx->b[current_len];

    u8 *field_imm = decode_immediate(ctx, S, W, imm_ptr); 

    if (MOD != 0x3)    
    {
        if (W == 0x0)
        {
            field_rm = strjoin_fmt(ctx->a,"%s %s", "byte", field_rm);
        }
        else
        {
            field_rm = strjoin_fmt(ctx->a,"%s %s", "word", field_rm);
        }
    }

    u8 *operands = strjoin_fmt(ctx->a, "%s, %s", field_rm, field_imm);
    write_fmt_line(ctx, mnemonics[idx], operands);

    u8 imm_len;
    if (W == 1 && S == 0)
    {
        imm_len = 2;
    }
    else
    {
        imm_len = 1;
    }

    return current_len + imm_len;
}

u8 fmt_jump(t_ctx *ctx)
{
    u8 *mnemonics[16] =
    {
        "jo", "jno",
        "jb", "jnb",
        "je", "jne",
        "jbe","ja",
        "js", "jns",
        "jp", "jnp",
        "jl", "jnl",
        "jle", "jg"
    };

    u8 idx = ctx->b[0] & 0xF;
    i8 IP_INC8 = (i8)ctx->b[1];
    u16 target_address = (u16)(ctx->current_ip + 2 + IP_INC8);

    u8 *target_str = strjoin_fmt(ctx->a, "0x%04X", target_address);
    write_fmt_line(ctx, mnemonics[idx], target_str);

    return 2;
}

u8 handle_inc_dec_push_pop_reg_16(t_ctx *ctx)
{
    u8 *mnemonics[4] = {"inc", "dec", "push", "pop"};
    u8 idx = (ctx->b[0] >> 3) & 0x3;
    u8 REG = ctx->b[0] & 0x7;

    write_fmt_line(ctx, mnemonics[idx], table_reg_w_one[REG]);

    return 1;
}

u8 handle_cbw_cwd_wait_pushf_popf_sahf_lahf(t_ctx *ctx)
{
    u8 *mnemonics[8] = {"cbw", "cwd", "wait", "pushf", "popf", "sahf", "lahf"};
    u8 idx = ctx->b[0] & 0x7;
    write_string_fd(ctx->fd, mnemonics[idx]);
    return 1;
}

u8 handle_daa_das_aaa_aas(t_ctx *ctx)
{
    u8 *mnemonics[4] = {"daa", "das", "aaa", "aas"};
    u8 idx = (ctx->b[0] >> 3) & 0x3;

    write_fmt_line_no_operands(ctx, mnemonics[idx]);
    return 1;
}

u8 handle_seg_override(t_ctx *ctx)
{
    ctx->seg_prefix = (ctx->b[0] >> 3) & 0x3;
    return 1;
}

u8 fmt_segment_push_pop(t_ctx *ctx)
{
    u8 idx = ctx->b[0] & 0x1;
    u8 REG = (ctx->b[0] >> 3) & 0x3;
    u8 *SREG = table_sreg[REG];

    if (idx == 0)
    {
        write_fmt_line(ctx, "push", SREG);
    }
    else
    {
        write_fmt_line(ctx, "pop", SREG);
    }

    return 1;
}

u8 fmt_imm_to_acc(t_ctx *ctx)
{
    u8 *acc_mnemonics[8] = {"add", "or", "adc", "sbb", "and", "sub", "xor", "cmp"};
    u8 idx = (ctx->b[0] >> 3) & 0x7;
    u8 W = ctx->b[0] & 0x1;


    u8 *operands;
    if (W == 0)
    {
        operands = strjoin_fmt(ctx->a, "%s, 0x%02X", ACC_BYTE, ctx->b[1]);
        write_fmt_line(ctx, acc_mnemonics[idx], operands);
        return 2;
    }
    else
    {
        u16 val = (ctx->b[2] << 8) | ctx->b[1];
        operands = strjoin_fmt(ctx->a, "%s, 0x%04X", ACC_WORD, val);
        write_fmt_line(ctx, acc_mnemonics[idx], operands);
        return 3;
    }
}

u8 fmt_modrm_test_xchg_mov(t_ctx *ctx)
{
    u8 *mnemonics[8] = {"test", "test", "xchg", "xchg", "mov", "mov","mov", "mov"};
    u8 idx = ctx->b[0] - 0x84;
    u8 D = (ctx->b[0] >> 1) & 0x1;
    u8 W = ctx->b[0] & 0x1;
    u8 MOD = (ctx->b[1] >> 6) & 0x3; 
    u8 REG = (ctx->b[1] >> 3) & 0x7;
    u8 RM = ctx->b[1] & 0x7;

    u8 *field_RM = decode_rm(ctx, RM, MOD, W);
    u8 *field_REG = decode_reg(REG, W);

    u8 *operands;
    if (D == 0)
    {
        /* Instruction source is specifiend in REG field */   
        operands = strjoin_fmt(ctx->a, "%s, %s", field_RM, field_REG);
    }
    else
    {
        /* Instruction destination is specifiend in REG field */   
        operands = strjoin_fmt(ctx->a, "%s, %s", field_REG, field_RM);
    }

    write_fmt_line(ctx, mnemonics[idx], operands);

    return match_MODRM_with_offset(MOD, RM);
}

u8 fmt_modrm_common(t_ctx *ctx)
{
    u8 *mnemonics[8] = {"add", "or", "adc", "sbb", "and", "sub", "xor", "cmp"};
    u8 idx = (ctx->b[0] >> 3) & 0x7;
    u8 D = (ctx->b[0] >> 1) & 0x1;
    u8 W = ctx->b[0] & 0x1;
    u8 MOD = (ctx->b[1] >> 6) & 0x3; 
    u8 REG = (ctx->b[1] >> 3) & 0x7;
    u8 RM = ctx->b[1] & 0x7;

    u8 *field_RM = decode_rm(ctx, RM, MOD, W);
    u8 *field_REG = decode_reg(REG, W);

    u8 *operands;
    if (D == 0)
    {
        /* Instruction source is specifiend in REG field */   
        operands = strjoin_fmt(ctx->a, "%s, %s", field_RM, field_REG);
    }
    else
    {
        /* Instruction destination is specifiend in REG field */   
        operands = strjoin_fmt(ctx->a, "%s, %s", field_REG, field_RM);
    }

    write_fmt_line(ctx, mnemonics[idx], operands);

    return match_MODRM_with_offset(MOD, RM);
}
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

static u8 *get_seg_prefix_str(u8 prefix)
{
    if (prefix == 0x26)
    {
        return "es:";
    }
    else if (prefix == 0x2E)
    {
        return "cs:";
    }
    else if (prefix == 0x36)
    {
        return "ss:";
    }
    else if (prefix == 0x3E)
    {
        return "ds:";
    }
    else
    {
        return "";
    }
}

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

static u8 *decode_field_mem_disp_none(t_arena *a, u8 *prefix, u8 *base)
{
    //TODO: arena push format
    char prefix = '[';
    char suffix = ']';

    char *result;
    size_t size = s_len + 3;
    result = arena_alloc(a, size);

    result[0] = prefix;
    strncpy(result + 1, s, s_len);
    result[size - 2] = suffix;
    result[size - 1] = '\0';

    return result;
}

char *create_mem_operand_disp_8(t_arena *a, char *s, size_t s_len, u8 b3)
{
    char prefix = '[';
    char suffix = ']';
    char *separator = " + ";
    char *byte = itoa(a, (i32)b3);
    size_t sep_len = strlen(separator);
    size_t byte_len = strlen(byte);

    char *result;
    size_t size = s_len + sep_len + byte_len + 3;
    result = arena_alloc(a, size);
    if (!result)
    {
        printf("Error: Could not allocate memory to the arena\n");
        return 0;
    }

    size_t offset = 0;
    result[0] = prefix;
    offset += 1;
    strncpy(result + offset, s, s_len);
    offset += s_len;
    strncpy(result + offset , separator, sep_len);
    offset += sep_len;
    strncpy(result + offset, byte, byte_len);
    result[size - 2] = suffix;
    result[size - 1] = '\0';
    return result;
}

/* Following Intel convention, if the displacement is two bytes,
   the most-significant byte is stored second in the instruction. */
static u8 *concat_2_bytes(t_arena *a, u8 lsb, u8 msb)
{
    u8 *result;
    u16 concat = (msb << 8) | lsb;

    result = itoa(a, (i32)concat); 
    if (!result) {return NULL;}

    return result;
}

char *create_mem_operand_dir_add_disp_16(t_arena *a, u8 b3, u8 b4)
{
    char prefix = '[';
    char suffix = ']';

    char *byte = concat_2_bytes_to_str(a, b3, b4);
    if (!byte)
    {
        printf("Error: Failed to concatenate a string\n");
        return NULL;
    }
    size_t byte_len = strlen(byte);

    char *result;
    size_t size = byte_len + 3;
    result = arena_alloc(a, size);
    if (!result)
    {
        printf("Error: Could not allocate memory to the arena\n");
        return NULL;
    }

    result[0] = prefix;
    strncpy(result + 1, byte, byte_len);
    result[size - 2] = suffix;
    result[size - 1] = '\0';

    return result;
}

char *create_mem_operand_disp_16(t_arena *a, char *s, size_t s_len, u8 b3, u8 b4)
{
    char prefix = '[';
    char suffix = ']';
    char *separator = " + ";
    size_t sep_len = strlen(separator);

    char *byte = concat_2_bytes_to_str(a, b3, b4);
    size_t byte_len = strlen(byte);

    char *result;
    size_t size = s_len + sep_len + byte_len + 3;
    result = arena_alloc(a, size);
    if (!result)
    {
        printf("Error: Could not allocate memory to the arena\n");
        return NULL;
    }

    size_t offset = 0;
    result[0] = prefix;
    offset += 1;
    strncpy(result + offset, s, s_len);
    offset += s_len;
    strncpy(result + offset, separator, sep_len);
    offset += sep_len;
    strncpy(result + offset, byte, byte_len);
    result[size - 2] = suffix;
    result[size - 1] = '\0';

    return result;
}

static u8 *decode_rm(t_ctx *ctx, u8 RM, u8 MOD, u8 W)
{
    u8 *base = table_mem_address_calc[RM];
    u8 *prefix = ctx->seg_prefix;

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

size_t decode_mov_rmtoreg(int fd, u8 b1, u8 b2, u8 b3, u8 b4)
{
    t_arena *a = arena_create(1024);
    if (!a)
    {
        printf("Error: Could not create an arena\n");
        return 0;
    }

    size_t offset = 0;

    u8 D = (b1 & 0x2) >> 1;
    u8 W = b1 & 0x1;
    u8 MOD = (b2 & 0xC0) >> 6;
    u8 REG = (b2 & 0x38) >> 3;
    u8 RM = b2 & 0x7;
    
    char *field_reg;
    char *field_rm;
    
    field_reg = decode_field_reg(REG, W);
    if (!field_reg)
    {
        printf("Error: Could not decode REG field\n");
        arena_destroy(a);
        return 0;
    }
    field_rm = decode_field_rm(a, RM, MOD, W, b3, b4);
    if (!field_rm)
    {
        printf("Error: Could not decode R/M field\n");
        arena_destroy(a);
        return 0;
    }

    if (D == 0)
    {
        /* Instruction source is specifiend in REG field */   
        write_line(fd, "mov", field_rm, field_reg);
    }
    else
    {
        /* Instruction destination is specifiend in REG field */   
        write_line(fd, "mov", field_reg, field_rm);
    }
    arena_destroy(a);

    offset = match_MOD_with_offset(MOD, RM);
  
    return offset;

}

// NOTE: SUS
/* The second byte of a two-byte immediate value is the most significant. */
char *decode_field_imm_to_reg(t_arena *a, u8 W, u8 b2, u8 b3)
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

u8 *decode_immediate(t_arena *a, u8 S, u8 W, u8 *imm_ptr)
{
    if (W == 0)
    {

    }
    else if (S == 1)
    {
        i16 val = (i16)((i8)imm_ptr[0]);
    }
}

size_t decode_mov_imtorm(int fd, u8 b1, u8 b2, u8 b3, u8 b4, u8 b5, u8 b6)
{
    size_t offset = 0;
    t_arena *a = arena_create(1024);
    if (!a)
    {
        printf("Error: Could not create an arena\n");
        return 0;
    }

    u8 W = b1 & 0x1;
    u8 MOD = (b2 & 0xC0) >> 6;
    u8 RM = b2 & 0x7;

    char *field_rm = decode_field_rm(a, RM, MOD, W, b3, b4);
    if (!field_rm)
    {
        printf("Error: Could not decode R/M field\n");
        arena_destroy(a);
        return 0;
    }

    char *field_im = decode_field_immediate_torm(a, W, MOD, RM, b3, b4, b5, b6);
    if (!field_im)
    {
        printf("Error: Could not decode 'IM' field\n");
        arena_destroy(a);
        return 0;
    }

    write_line(fd, "mov", field_rm, field_im);
    arena_destroy(a);

    offset = match_MOD_with_offset(MOD, RM);
    offset += 1;
    if (W == 0x1)
    {
        offset += 1;
    }
    return offset;
}


size_t decode_mov_imtoreg(int fd, u8 b1, u8 b2, u8 b3)
{
    size_t offset;
    t_arena *a = arena_create(1024);
    if (!a)
    {
        printf("Error: Could not create an arena\n");
        return 0;
    }

    u8 W = (b1 & 0x8) >> 3;
    u8 REG = b1 & 0x7;
 
    char *field_reg = decode_field_reg(REG, W);
    if (!field_reg)
    {
        printf("Error: Could not decode REG field\n");
        arena_destroy(a);
        return 0;
    }

    char *field_im = decode_field_immediate_toreg(a, W, b2, b3);
    if (!field_im)
    {
        printf("Error: Could not decode 'IM' field\n");
        arena_destroy(a);
        return 0;
    }

    write_line(fd, "mov", field_reg, field_im);
    arena_destroy(a);

    if (W == 0)
    {
        offset = 2;
    }
    else
    {
        offset = 3;
    }

    return offset;
}

char *create_mem_direct_address_8(t_arena *a, u8 b3);
char *create_mem_direct_address_16(t_arena *a, u8 lsb, u8 msb);

char *decode_field_address(t_arena *a, u8 W, u8 b2, u8 b3)
{
    if (W == 0x0)
    {
        return create_mem_direct_address_8(a, b2);
    }
    else
    {
        return create_mem_direct_address_16(a, b2, b3);
    }
}

// TODO.1 :create some hepler
char *create_mem_direct_address_8(t_arena *a, u8 b2)
{
    char prefix = '[';
    char suffix = ']';

    char *value = itoa(a, b2);
    if (!value)
    {
        printf("Error: Itoa returned NULL\n");
        return NULL;
    }

    size_t value_len = strlen(value);

    char *result;
    size_t size = value_len + 3;
    result = arena_alloc(a, size);
    if (!result)
    {
        printf("Error: Could not allocate memory to the arena\n");
        return NULL;
    }

    result[0] = prefix;
    strncpy(result + 1, value, value_len);
    result[size - 2] = suffix;
    result[size - 1] = '\0';

    return result;
}

// TODO.1 :create some hepler
char *create_mem_direct_address_16(t_arena *a, u8 lsb, u8 msb)
{
    char prefix = '[';
    char suffix = ']';

    char *value = concat_2_bytes(a, lsb, msb);
    if (!value)
    {
        printf("Error: Failed to concatenate a string\n");
        return NULL;
    }

    size_t value_len = strlen(value);

    char *result;
    size_t size = value_len + 3;
    result = arena_alloc(a, size);
    if (!result)
    {
        printf("Error: Could not allocate memory to the arena\n");
        return NULL;
    }

    result[0] = prefix;
    strncpy(result + 1, value, value_len);
    result[size - 2] = suffix;
    result[size - 1] = '\0';

    return result;
}

size_t decode_mov_memtoacc(int fd, u8 b1, u8 b2, u8 b3)
{
    t_arena *a;

    a = arena_create(1024);
    if (!a)
    {
        printf("Error: Could not create an arena\n");
        return 0;
    }

    u8 W = b1 & 0x1;

    char *field_acc = "ax";
    char *field_add = decode_field_address(a, W, b2, b3);
    if (!field_add)
    {
        printf("Error: Could not decode address field\n");
        arena_destroy(a);
        return 0;
    }

    write_line(fd, "mov", field_acc, field_add);
    arena_destroy(a);

    if (W == 0x0)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

size_t decode_mov_acctomem(int fd, u8 b1, u8 b2, u8 b3)
{
    t_arena *a;

    a = arena_create(1024);
    if (!a)
    {
        printf("Error: Could not create an arena\n");
        return 0;
    }

    u8 W = b1 & 0x1;

    char *field_acc = "ax";
    char *field_add = decode_field_address(a, W, b2, b3);
    if (!field_add)
    {
        printf("Error: Could not decode address field\n");
        arena_destroy(a);
        return 0;
    }

    write_line(fd, "mov", field_add, field_acc);
    arena_destroy(a);

    if (W == 0x0)
    {
        return 2;
    }
    else
    {
        return 3;
    }
}

u8 decode_8086_instruction(i32 fd, u8 *buffer)
{
    u8 byte = buffer[0];
    if ((byte & MASK_MS_6) == OPCODE_MOV_RMTOREG)
    {
        return decode_mov_rmtoreg(fd, buffer[0], buffer[1], buffer[2], buffer[3]);
    }
    else if ((byte & MASK_MS_7) == OPCODE_MOV_IMTORM)
    {
        return decode_mov_imtorm(fd, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5]);
    }
    else if ((byte & MASK_MS_4) == OPCODE_MOV_IMTOREG)
    {
        return decode_mov_imtoreg(fd, buffer[0], buffer[1], buffer[2]);
    }
    else if ((byte & MASK_MS_7) == OPCODE_MOV_MEMTOACC)
    {
        return decode_mov_memtoacc(fd, buffer[0], buffer[1], buffer[2]); // TODO: memtoacc and acctomem share almost the same code
    }
    else if ((byte & MASK_MS_7) == OPCODE_MOV_ACCTOMEM)
    {
        return decode_mov_acctomem(fd, buffer[0], buffer[1], buffer[2]); // TODO: memtoacc and acctomem share almost the same code
    }
    else if ((byte & MASK_MS_8) == OPCODE_MOV_RMTOSREG)
    {
        printf("OPCODE for: rm to sreg\n");
        return 2;

    }
    else if ((byte & MASK_MS_8) == OPCODE_MOV_SREGTORM)
    {
        printf("OPCODE for: sreg to rm\n");
        return 2;

    }
    else
    {
        printf("Error: Unsupported opcode\n");
        return 2;
    }
}
// TODO: instead of b5 -> data_low
u8 opcode_not_used(t_ctx *ctx)
{
    return OFFSET_OPCODE_NOT_USED;
}

u8 fmt_imm_to_rm(t_ctx *ctx)
{
    u8 *mnemonics[8] = {"add","or","adc","sbb","and","sub","xor","cmp"};
    u8 idx = (ctx->b[1] >> 3) & 0x7;
    u8 S = (ctx->b[0] >> 1) & 0x1;
    u8 W = ctx->b[0] & 0x1;
    u8 RM = ctx->b[1] & 0x7;
    u8 MOD = (ctx->b[1] >> 6) & 0x7;

    u8* field_rm = decode_field_rm(ctx->a, RM, MOD, W, ctx->b[2], ctx->b[3], ctx->seg_prefix);
    if (!field_rm)
    {
        return EXIT_ERROR;
    }

    u8 *field_imm;
    if (S == 1 && W == 1)
    {
        /* Perfrom Sign Extension */


    }
    else if (S == 0 && W == 1)
    {
        field_imm = decode_field_imm_to_rm(ctx->a, W, MOD, RM, ctx->b[2], ctx->b[3], ctx->b[4], ctx->b[5]); 
    }
    else
    {

    }


    if (!field_imm)
    {
        return EXIT_ERROR;
    }

}

u8 fmt_jump(t_ctx *ctx)
{
    u8 *mnemonics[16] =
    {
        "jo", "jno",
        "jb", "jnb",
        "je", "jne",
        "jbe","jnbe",
        "js", "jns",
        "jp", "jnp",
        "jl", "jnl",
        "jle", "jnhle"
    };
    u8 idx = ctx->b[0] & 0xF;
    i8 IP_INC8 = (i8)ctx->b[1];
    i32 target_address = ctx->current_ip + 2 + IP_INC8;

    /* TODO: create fmt write func */
    write_line(ctx->fd, mnemonics[idx], &target_address, NULL);

    return 2;
}

u8 inc_dec_push_pop_reg_16(t_ctx *ctx)
{
    u8 *mnemonics[4] = {"inc", "dec", "push", "pop"};
    u8 idx = (ctx->b[0] >> 3) & 0x3;
    u8 REG = ctx->b[0] & 0x7;
    write_line_str(ctx->fd, mnemonics[idx], table_reg_w_one[REG]);
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
    write_string_fd(ctx->fd, mnemonics[idx]);
    return 1;
}

u8 fmt_segment_fix(t_ctx *ctx)
{
    u8 REG = (ctx->b[0] >> 3) & 0x3;
    u8 *field_sreg = table_sreg[REG];

    write_line_str(ctx->fd, mnemonic, field_sreg);
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
        u8 *line = strjoin_fmt(ctx->a, "%-7s %s", acc_mnemonics[idx], operands);
        write_line(ctx->fd, line);
        return 2;
    }
    else
    {
        u16 val = (ctx->b[2] << 8) | ctx->b[1];
        operands = strjoin_fmt(ctx->a, "%s, 0x%04X", ACC_WORD, val);
        u8 *line = strjoin_fmt(ctx->a, "%-7s %s", acc_mnemonics[idx], operands);
        write_line(ctx->fd, line);
        return 3;
    }
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
    if (!field_RM)
    {
        return EXIT_ERROR;
    }

    u8 *field_REG = decode_reg(REG, W);
    if (!field_REG)
    {
        return EXIT_ERROR;
    }

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

    u8 *line = strjoin_fmt(ctx->a, "%-7s %s", mnemonics[idx], operands);
    write_line(ctx->fd, line);

    return match_MODRM_with_offset(MOD, RM);
}
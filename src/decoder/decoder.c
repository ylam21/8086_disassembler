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

char *table_reg_w_zero[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
char *table_reg_w_one[]  = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
char *table_rm_memmode[] = {"bx + si", "bx + di", "bp + si", "bp + di", "si", "di", "bp", "bx"};



char *decode_field_reg(u8 REG, u8 W)
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

char *create_mem_operand_disp_none(t_arena *a, char *s, size_t s_len)
{
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

/*
 * Following Intel convention, if the displacement is two bytes, the most-significant byte is stored second in the instruction.
 */
char *concat_2_bytes_to_str(t_arena *a, u8 lsb, u8 msb)
{
    char *result;
    u16 byte_value = (msb << 8) | lsb;

    result = itoa(a, (i32)byte_value); // NOTE: just stick with i32
    if (!result)
    {
        printf("Error: itoa returns NULL value\n");
        return NULL;
    }
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

/* TODO: note that RM and REG field are 3bit fields */
char *decode_field_rm(t_arena *a, u8 RM, u8 MOD, u8 W, u8 b3, u8 b4)
{
    char *base = table_rm_memmode[RM];
    if (MOD == 0x0)
    {
        /* Memory Mode, no displacement follows
           Except when R/M = 110, then 16-bit displacement follows */ 
        if (RM == 0x6)
        {
            /* Effective Address Calculation
               DIRECT ADDRESS */
            return create_mem_operand_dir_add_disp_16(a, b3, b4);
        }
        else
        {
            /* Effective Address Calculation */
            return create_mem_operand_disp_none(a, base, strlen(base));
        }
    }
    else if (MOD == 0x1)
    {
        /* Memory Mode, 8-bit displacement follows
           Effective Address Calculation           */
        return create_mem_operand_disp_8(a, base, strlen(base), b3);
    }
    else if (MOD == 0x2)
    {
        /* Memory Mode, 16-bit displacement follows
           Effective Address Calculation           */
        return create_mem_operand_disp_16(a, base, strlen(base), b3, b4);
    }
    else
    {
        /* Register Mode (no displacement) */
        return decode_field_reg(RM, W);
    }
}

size_t match_MOD_with_offset(u8 MOD, u8 RM)
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

/* The second byte of a two-byte immediate value is the most significant. */
char *decode_field_immediate_toreg(t_arena *a, u8 W, u8 b2, u8 b3)
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

/* The second byte of a two-byte immediate value is the most significant. */
char *decode_field_immediate_torm(t_arena *a, u8 W, u8 MOD, u8 RM, u8 b3, u8 b4, u8 b5, u8 b6)
{
    char *value;
    char *byte = "byte ";
    char *word = "word ";

    if (MOD == 0x0)
    {
        /* Memory Mode, no displacement follows
           Except when R/M = 110, then 16-bit displacement follows */ 
        if (RM == 0x6)
        {
            // TODO: insert doc to W's
            if (W == 0x0)
            {
                value = itoa(a, (i32)b5);
                return strjoin(a, byte, value);
            }
            else
            {
                value = concat_2_bytes_to_str(a, b5, b6);
                return strjoin(a, word, value);
            }
        }
        else
        {
            if (W == 0x0)
            {
                value = itoa(a, (i32)b3);
                return strjoin(a, byte, value);
            }
            else
            {
                value = concat_2_bytes_to_str(a, b3, b4);
                return strjoin(a, word, value);
            }
        }
    }
    else if (MOD == 0x1)
    {
        /* Memory Mode, 8-bit displacement follows */
        if (W == 0x0)
        {
            value = itoa(a, (i32)b4);
            return strjoin(a, byte, value);
        }
        else
        {
            value = concat_2_bytes_to_str(a, b4, b5);
            return strjoin(a, word, value);
        }
    }
    else if (MOD == 0x2)
    {
        /* Memory Mode, 16-bit displacement follows */
        if (W == 0x0)
        {
            value = itoa(a, (i32)b5);
            return strjoin(a, byte, value);
        }
        else
        {
            value = concat_2_bytes_to_str(a, b5, b6);
            return strjoin(a, word, value);
        }
    }
    else
    {
        /* Register Mode (no displacement) */
        if (W == 0x0)
        {
            value = itoa(a, (i32)b3);
            return strjoin(a, byte, value);
        }
        else
        {
            value = concat_2_bytes_to_str(a, b3, b4);
            return strjoin(a, word, value);
        }
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

    char *value = concat_2_bytes_to_str(a, lsb, msb);
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

size_t decode_bin_to_asm_16(i32 fd, u8 *buffer)
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



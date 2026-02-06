#ifndef OPCODES_H
#define OPCODES_H

#include <stddef.h>
#include "../arena/arena.h"
#include "../common.h"

typedef struct
{
    t_arena *a;
    i32 fd;
    u8 *b;
    u8 seg_prefix;
    u32 current_ip;
} t_ctx;


typedef u8 (*func_ptr)(t_ctx *ctx); 

extern func_ptr META_TABLE[256];

#endif
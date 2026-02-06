#ifndef STRJOIN_FMT_H
#define STRJOIN_FMT_H

#include "../string_utils.h"
typedef struct
{
    u8 padding_char;
    i32 width;
    u8 left_align;
    u8 is_conditional;
} t_fmt_opt;

#include "conversion/conversion.h"

#define CHAR_SPACE ' '

typedef u8* (*fmt_handler_t)(t_arena *a, va_list args, t_fmt_opt opt);

extern fmt_handler_t specifier_table[256];

u8 *strjoin_fmt(t_arena *a, u8 *fmt, ...);

#endif
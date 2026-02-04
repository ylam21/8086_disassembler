#include "conversion.h"

u8 *handle_percent_literal(t_arena *a, va_list args, t_fmt_opt opt)
{
    (void)args;
    (void)opt;

    u8 *str = arena_alloc(a, 2);
    if (!str)
    {
        return NULL;
    }
    
    str[0] = '%';
    str[1] = '\0';

    return str;
}
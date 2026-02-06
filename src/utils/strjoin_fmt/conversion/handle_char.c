#include "conversion.h"

u8 *handle_char(t_arena *a, va_list args, t_fmt_opt opt)
{
    u8 c;

    c = (u8)va_arg(args, i32);
    u8 *str = arena_alloc(a, 1);
    if (!str)
    {
        return NULL;
    }

    str[0] = c;

    if (opt.width > 0)
    {
        return apply_padding(a, str, opt);
    }
    else
    {
        return str;
    }
}
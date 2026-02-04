#include "conversion.h"

u8 *handle_int(t_arena *a, va_list args, t_fmt_opt opt)
{
    i32 val;
    u8 *str;

    val = va_arg(args, i32);
    str = itoa(a, val);
    if (!str)
    {
        return NULL;
    }
    if (opt.width > 0)
    {
        return apply_padding(a, str, opt);
    }
    else
    {
        return str;
    }
}
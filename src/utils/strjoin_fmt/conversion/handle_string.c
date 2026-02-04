#include "conversion.h"

u8 *handle_string(t_arena *a, va_list args, t_fmt_opt opt)
{
    u8 *str = va_arg(args, u8 *);
    if (!str)
    {
        if (opt.is_conditional)
        {
            return (u8 *)"";
        }
        u8 size = strlen("(null)");
        str = arena_alloc(a, size + 1);
        if (!str)
        {
            return NULL;
        }
        memcpy(str, "(null)", size);
        str[size] = '\0';
        return str;
    }

    if (opt.is_conditional)
    {
        if (str[0] == '\0')
        {
            return (u8 *)"";
        }
        return strjoin_fmt(a, "%s:", str);
    }

    if (opt.width != 0)
    {
        return apply_padding(a, str, opt);
    }
    else
    {
        return str;
    }
}

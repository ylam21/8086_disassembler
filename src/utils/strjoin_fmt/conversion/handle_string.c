#include "conversion.h"

u8 *handle_string(t_arena *a, va_list args, t_fmt_opt opt)
{
    u8 *str = va_arg(args, u8 *);

    if (!str)
    {
        if (opt.is_conditional)
        {
            return NULL;
        }
        str = (u8 *)"(null)";
    }

    if (opt.is_conditional)
    {
        if (str[0] == '\0')
        {
            return NULL;
        }
        return strjoin_fmt(a, "%s:", str);
    }

    if (opt.width != 0)
    {
        return apply_padding(a, str, opt);
    }
    else
    {
        u64 len = strlen((char *)str);
        u8 *result = arena_alloc(a, len);
        if (!result) return NULL;
        
        memcpy(result, str, len);
        return result;
    }
}

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

    u64 len = strlen((char *)str);
    u64 needed_size = len;

    if (opt.is_conditional)
    {
        if (len == 0 || str[0] == '\0')
        {
            return NULL;
        }
        needed_size += 1;
    }

    u8 *result = arena_alloc(a, needed_size);
    if (!result)
    {
        return NULL;
    }

    memcpy(result, str, len);

    if (opt.is_conditional)
    {
        result[len] = ':';
    }

    return result;
}

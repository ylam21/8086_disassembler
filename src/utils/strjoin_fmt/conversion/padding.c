#include "conversion.h"

u8 *apply_padding(t_arena *a, u8 *str, t_fmt_opt opt)
{
    u64 len = strlen(str);

    if (len >= opt.width)
    {
        return str;
    }

    i32 total_padding = opt.width - len;
    u8 *padded = arena_alloc(a, opt.width + 1);
    if (!padded)
    {
        return NULL;
    }

    if (opt.left_align)
    {   
        /* <TEXT;SPACES> */
        memcpy(padded, str, len);
        memset(padded + len, CHAR_SPACE, total_padding);
    }
    else
    {
        /* <SPACES/ZEROS;TEXT> */
        memset(padded, opt.padding_char, total_padding);
        memcpy(padded + total_padding, str, len);
    }

    padded[opt.width] = '\0';

    return padded;
}
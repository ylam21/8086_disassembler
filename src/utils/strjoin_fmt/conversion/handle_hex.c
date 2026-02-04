#include "conversion.h"

static u8 *handle_hex(t_arena *a, va_list args, t_fmt_opt opt, u8 table[16])
{
    u32 val = va_arg(args, u32);
    u8 buffer[16];
    u32 len = 0;

    if (val == 0)
    {
        buffer[len++] = '0';
    }
    else
    {
        while (val && len < sizeof(buffer))
        {
            buffer[len++] = table[val & 0xF];
            val >>= 4;
        }
    }

    u8 *str = arena_alloc(a, len + 1);
    if (!str)
    {
        return NULL;
    }
    
    u32 i = 0;
    while (i < len)
    {
        str[i] = buffer[len - 1 - i];
        i++;
    }
    str[len] = '\0';

    if (opt.width > 0)
    {
        return apply_padding(a, str, opt);
    }
    else
    {
        return str;
    }
}

u8 *handle_upx(t_arena *a, va_list args, t_fmt_opt opt)
{
    return handle_hex(a, args, opt, (u8*)"0123456789ABCDEF");
}   

u8 *handle_x(t_arena *a, va_list args, t_fmt_opt opt)
{
    return handle_hex(a, args, opt, (u8*)"0123456789abdef");
}   
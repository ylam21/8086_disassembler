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

    u32 final_len = len;
    if (opt.width > (i32)len)
    {
        final_len = (u32)opt.width;
    }

    u8 *str = arena_alloc(a, final_len);
    if (!str)
    {
        return NULL;
    }

    memset(str, opt.padding_char, final_len);

    u32 offset;
    
    if (opt.left_align)
    {
        offset = 0;
    }
    else
    {
        offset = final_len - len;
    }

    u32 i = 0;
    while (i < len)
    {
        str[offset + i] = buffer[len - 1 - i];
        i++;
    }

    return str;
}

u8 *handle_upx(t_arena *a, va_list args, t_fmt_opt opt)
{
    return handle_hex(a, args, opt, (u8*)"0123456789ABCDEF");
}   

u8 *handle_x(t_arena *a, va_list args, t_fmt_opt opt)
{
    return handle_hex(a, args, opt, (u8*)"0123456789abcdef");
}
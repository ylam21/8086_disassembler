#ifndef CONVERSION_H
#define CONVERSION_H

#include "../strjoin_fmt.h"

u8 *handle_int(t_arena *a, va_list args, t_fmt_opt opt);
u8 *handle_x(t_arena *a, va_list args, t_fmt_opt opt);
u8 *handle_upx(t_arena *a, va_list args, t_fmt_opt opt);
u8 *handle_ptr(t_arena *a, va_list args, t_fmt_opt opt);
u8 *handle_string(t_arena *a, va_list args, t_fmt_opt opt);
u8 *handle_char(t_arena *a, va_list args, t_fmt_opt opt);
u8 *handle_percent_literal(t_arena *a, va_list args, t_fmt_opt opt);
u8 *apply_padding(t_arena *a, u8 *str, t_fmt_opt opt);

#endif
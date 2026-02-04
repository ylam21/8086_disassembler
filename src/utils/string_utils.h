#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "../common.h"
#include "../arena/arena.h"

u8 *strjoin(t_arena *a, u8 *s1, u8 *s2);
u8 *write_fmt_fd(i32 fd, t_arena *a, u8 *fmt, ...);
u8 *itoa(t_arena *a, i32 x);

#endif
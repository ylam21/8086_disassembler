#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "../common.h"
#include "../arena/arena.h"
#include "strjoin_fmt/strjoin_fmt.h"

u8 *strnjoin(t_arena *a, u8 *s1, u8 *s2, u64 n);
u8 *itoa(t_arena *a, i32 x);

#endif
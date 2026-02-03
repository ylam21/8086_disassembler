#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include "../common.h"
#include "../arena/arena.h"

char *strjoin(t_arena *a, char *s1, char *s2);
char *itoa(t_arena *a, i32 x);

#endif
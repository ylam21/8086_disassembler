#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>
#include "../common.h"

typedef struct s_arena
{
    void *buffer;
    u64 idx;
    u64 cap;
}   t_arena;

t_arena *arena_create(u64 size);
void *arena_alloc(t_arena *a, u64 size);
void arena_reset(t_arena *a);
void arena_destroy(t_arena *a);

#endif
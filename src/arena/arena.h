#ifndef ARENA_H
#define ARENA_H

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>

typedef struct s_arena
{
    void *buffer;
    size_t idx;
    size_t cap;
}   t_arena;

t_arena *arena_create(size_t size);
void *arena_alloc(t_arena *a, size_t size);
void arena_reset(t_arena *a);
void arena_destroy(t_arena *a);

#endif
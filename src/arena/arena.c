#include "arena.h"

t_arena *arena_create(u64 size)
{
    t_arena *a;
    a = malloc(sizeof(t_arena));
    if (a == NULL)
    {
        perror("malloc");
        return NULL;
    }
    a->buffer = malloc(size);
    if (a->buffer == NULL)
    {
        perror("malloc");
        free(a);
        return NULL;
    }
    a->cap = size;
    a->idx = 0;
    return a;
}

static u64 align_forward(u64 ptr, u64 align)
{
    uintptr_t p = ptr;
    return ((p + align - 1) & ~(align - 1));
}

void  *arena_alloc(t_arena *a, u64 size)
{
    size_t align = 1;

    if (!a || size == 0)
    {
        return NULL;
    }

    size_t current_ptr = (size_t)a->buffer + a->idx;
    size_t aligned_ptr = align_forward(current_ptr, align);

    size_t offset = aligned_ptr - (size_t)a->buffer;

    if (offset + size > a->cap)
    {
        return NULL;
    }

    a->idx = offset + size;
    return ((void*)aligned_ptr);
}

void arena_reset(t_arena *a)
{
    if (a)
    {
        a->idx = 0;
    }
}

void arena_destroy(t_arena *a)
{
    if (a)
    {
        free(a->buffer);
    }
    free(a);
}
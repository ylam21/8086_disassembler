#include "arena.h"

t_arena *arena_create(size_t size)
{
    t_arena *a;
    a = malloc(sizeof(t_arena));
    if (a == NULL)
    {
        return NULL;
    }
    a->buffer = malloc(size);
    if (a->buffer == NULL)
    {
        free(a);
        return NULL;
    }
    a->cap = size;
    a->idx = 0;
    return a;
}

static size_t align_forward(size_t ptr, size_t align)
{
    uintptr_t p = ptr;
    return ((p + align - 1) & ~(align - 1));
}

void  *arena_alloc(t_arena *a, size_t size)
{
    size_t align = sizeof(void *);

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
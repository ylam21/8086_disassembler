#include "string_utils.h"

static size_t i32len(i32 x)
{
	size_t len;

	len = (x <= 0);
	while (x)
	{
		len++;
		x /= 10;
	}
	return (len);
}

u8 *strnjoin(t_arena *a, u8 *s1, u8 *s2, u64 n)
{
    u8 *result;
    u64 s1_len;
    u64 s2_len;
    u64 size;


    s2_len = strlen(s2);
	if (n > s2_len)
	{
		return NULL;
	}
    s1_len = strlen(s1);

    size = s1_len + n + 1;
    result = arena_alloc(a, size);
    if (!result)
    {
        return NULL;
    }

    strncpy(result, s1, s1_len);
    strncpy(result + s1_len, s2, n);
    result[size - 1] = '\0';
    
    return result;
}

u8 *itoa(t_arena *a, i32 x)
{
	u64 len;
	i64 nb;
	u8 *result;

	len = i32len(x);

	result = arena_alloc(a, len + 1);
	if (result == NULL)
	{
		return NULL;
	}
	result[len] = '\0';

	nb = x;
	if (nb == 0)
	{
		result[0] = '0';
	}
	else if (nb < 0)
	{
		result[0] = '-';
		nb = -nb;
	}
	while (nb)
	{
		result[--len] = (nb % 10) + '0';
		nb /= 10;
	}
	return (result);
}
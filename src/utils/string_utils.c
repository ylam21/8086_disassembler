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

char *strjoin(t_arena *a, char *s1, char *s2)
{
    char *result;
    size_t s1_len;
    size_t s2_len;
    size_t size;

    s1_len = strlen(s1);
    s2_len = strlen(s2);
    size = s1_len + s2_len + 1;
    result = arena_alloc(a, size);
    if (!result)
    {
        printf("Error: Could not allocate memory to the arena\n");
        return NULL;
    }

    strncpy(result, s1, s1_len);
    strncpy(result + s1_len, s2, s2_len);
    result[size - 1] = '\0';
    
    return result;
}

char *itoa(t_arena *a, i32 x)
{
	size_t len;
	long nb;
	char *result;

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
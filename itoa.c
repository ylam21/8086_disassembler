#include "itoa.h"

static size_t int32len(int32_t x)
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

char *itoa(t_arena *a, int32_t x)
{
	size_t len;
	long nb;
	char *result;

	len = int32len(x);

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

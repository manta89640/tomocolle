// MSL's memory functions, as the C library of CodeWarrior for DS 2.0 has them: the four the game links. This
// project's own decompilation of the game's code, written against that MSL's headers.
#include <string.h>
#include <mem_funcs.h>

void *memcpy(void *dst, const void *src, size_t n)
{
	const char *p = (const char *)src;
	char *q = (char *)dst;
	if (n) {
		do
			*q++ = *p++;
		while (--n);
	}
	return dst;
}

void *memset(void *dst, int val, size_t n)
{
	__fill_mem(dst, val, n);
	return dst;
}

void *memchr(const void *src, int val, size_t n)
{
	const unsigned char *p = (const unsigned char *)src;
	unsigned char c = val;
	if (n) {
		do
			if (*p++ == c)
				return (void *)(p - 1);
		while (--n);
	}
	return NULL;
}

int memcmp(const void *src1, const void *src2, size_t n)
{
	const unsigned char *p1 = (const unsigned char *)src1;
	const unsigned char *p2 = (const unsigned char *)src2;
	if (n) {
		do
			if (*p1++ != *p2++)
				return (p1[-1] < p2[-1]) ? -1 : +1;
		while (--n);
	}
	return 0;
}

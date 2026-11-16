// MSL's wide memory functions, as the C library of CodeWarrior for DS 2.0 has them: the two the game links. This
// project's own decompilation of the game's code, written against that MSL's headers.
#include <wchar.h>
#include <string.h>

wchar_t *wmemcpy(wchar_t *dst, const wchar_t *src, size_t n)
{
	return memcpy(dst, src, n * sizeof(wchar_t));
}

wchar_t *wmemchr(const wchar_t *src, wchar_t c, size_t n)
{
	while (n) {
		if (*src == c)
			return (wchar_t *)src;
		src++;
		n--;
	}
	return NULL;
}

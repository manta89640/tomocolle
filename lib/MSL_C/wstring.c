// MSL's wide string functions, as the C library of CodeWarrior for DS 2.0 has them. This project's own decompilation
// of the game's code, written against that MSL's headers.
#include <wchar.h>

size_t wcslen(const wchar_t *str)
{
	size_t len = (size_t)-1;
	do
		len++;
	while (*str++);
	return len;
}

wchar_t *wcscpy(wchar_t *dst, const wchar_t *src)
{
	const wchar_t *p = src;
	wchar_t *q = dst;
	while (*q++ = *p++)
		;
	return dst;
}

wchar_t *wcsncpy(wchar_t *dst, const wchar_t *src, size_t n)
{
	const wchar_t *p = src;
	wchar_t *q = dst;
	if (n) {
		do {
			if (!(*q++ = *p++)) {
				while (--n)
					*q++ = 0;
				break;
			}
		} while (--n);
	}
	return dst;
}

wchar_t *wcscat(wchar_t *dst, const wchar_t *src)
{
	const wchar_t *p = src;
	wchar_t *q = dst;
	while (*q++)
		;
	q--;
	while (*q++ = *p++)
		;
	return dst;
}

wchar_t *wcsncat(wchar_t *dst, const wchar_t *src, size_t n)
{
	const wchar_t *p = src;
	wchar_t *q = dst;
	while (*q++)
		;
	q--;
	if (n) {
		do {
			if (!(*q++ = *p++)) {
				q--;
				break;
			}
		} while (--n);
	}
	*q = 0;
	return dst;
}

int wcscmp(const wchar_t *str1, const wchar_t *str2)
{
	const wchar_t *p1 = str1;
	const wchar_t *p2 = str2;
	wchar_t c1, c2;
	while ((c1 = *p1++) == (c2 = *p2++))
		if (!c1)
			return 0;
	return c1 - c2;
}

wchar_t *wcschr(const wchar_t *str, const wchar_t chr)
{
	const wchar_t *p = str;
	wchar_t c = chr;
	wchar_t ch;
	while (ch = *p++)
		if (ch == c)
			return (wchar_t *)(p - 1);
	return c ? 0 : (wchar_t *)(p - 1);
}

// MSL's string functions, as the C library of CodeWarrior for DS 2.0 has them: the eight the game links. This
// project's own decompilation of the game's code, written against that MSL's headers.
#include <string.h>

// A word has a zero byte when ((w + K2) & ~w & K1) != 0
#define K1 0x80808080
#define K2 0xfefefeff

size_t strlen(const char *str)
{
	size_t len = (size_t)-1;
	do
		len++;
	while (*str++);
	return len;
}

char *strcpy(char *dst, const char *src)
{
	const unsigned char *p = (const unsigned char *)src;
	unsigned char *q = (unsigned char *)dst;
	unsigned long align, w, t;

	align = (unsigned long)src & 3;
	if (((unsigned long)dst & 3) == align) {
		if (align) {
			if (!(*q = *p))
				return dst;
			for (align = 3 - align; align; align--)
				if (!(*++q = *++p))
					return dst;
			q++;
			p++;
		}
		w = *(unsigned long *)p;
		t = w + K2;
		t &= ~w;
		if (!(t & K1)) {
			q -= 4;
			do {
				*(unsigned long *)(q += 4) = w;
				w = *(unsigned long *)(p += 4);
				t = w + K2;
				t &= ~w;
			} while (!(t & K1));
			q += 4;
		}
	}
	if (!(*q = *p))
		return dst;
	while (*++q = *++p)
		;
	return dst;
}

char *strncpy(char *dst, const char *src, size_t n)
{
	const char *p = src;
	char *q = dst;
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

char *strcat(char *dst, const char *src)
{
	const char *p = src;
	char *q = dst;
	while (*q++)
		;
	q--;
	while (*q++ = *p++)
		;
	return dst;
}

char *strncat(char *dst, const char *src, size_t n)
{
	const char *p = src;
	char *q = dst;
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

int strcmp(const char *str1, const char *str2)
{
	const unsigned char *left = (const unsigned char *)str1;
	const unsigned char *right = (const unsigned char *)str2;
	unsigned long align, l1, r1, x;
	int diff;

	l1 = *left;
	if ((diff = l1 - *right))
		return diff;
	align = (unsigned long)left & 3;
	if (((unsigned long)right & 3) != align)
		goto bytes;
	if (align) {
		if (!l1)
			return 0;
		for (align = 3 - align; align; align--) {
			l1 = *++left;
			if ((diff = l1 - *++right))
				return diff;
			if (!l1)
				return 0;
		}
		left++;
		right++;
	}
	l1 = *(unsigned long *)left;
	x = l1 + K2;
	x &= ~l1;
	r1 = *(unsigned long *)right;
	if (!(x & K1)) {
		while (l1 == r1) {
			l1 = *(unsigned long *)(left += 4);
			r1 = *(unsigned long *)(right += 4);
			x = l1 + K2;
			if (x & K1)
				goto adjust;
		}
		left--;
		right--;
		goto bytes;
	}
adjust:
	l1 = *left;
	if ((diff = l1 - *right))
		return diff;
bytes:
	if (!l1)
		return 0;
	for (;;) {
		l1 = *++left;
		if ((diff = l1 - *++right))
			return diff;
		if (!l1)
			return 0;
	}
}

int strncmp(const char *str1, const char *str2, size_t n)
{
	const unsigned char *p1 = (const unsigned char *)str1;
	const unsigned char *p2 = (const unsigned char *)str2;
	unsigned char c1, c2;
	if (n) {
		do {
			if ((c1 = *p1++) != (c2 = *p2++))
				return c1 - c2;
			if (!c1)
				break;
		} while (--n);
	}
	return 0;
}

char *strchr(const char *str, int chr)
{
	const char *p = str;
	char c = chr;
	char ch;
	while (ch = *p++)
		if (ch == c)
			return (char *)(p - 1);
	return c ? 0 : (char *)(p - 1);
}

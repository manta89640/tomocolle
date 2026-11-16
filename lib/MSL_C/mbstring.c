// MSL's conversions between multibyte and wide characters, as the C library of CodeWarrior for DS 2.0 has them: the
// six functions the game links. mbtowc and wctomb go through the current locale, whose "C" locale converts a byte
// to a character and back unchanged (__mbtowc_noconv, __wctomb_noconv). This project's own decompilation of the
// game's code, written against that MSL's headers (locale_api.h and mbstring.h included).
#include <limits.h>
#include <locale_api.h>
#include <mbstring.h>
#include <stdlib.h>
#include <string.h>

int mbtowc(wchar_t *pwc, const char *s, size_t n)
{
	return _current_locale.ctype_cmpt_ptr->decode_mb(pwc, s, n);
}

int __mbtowc_noconv(wchar_t *pwc, const char *s, size_t n)
{
	if (s == NULL)
		return 0;
	if (n == 0)
		return -1;
	if (pwc)
		*pwc = (unsigned char)*s;
	if (!*s)
		return 0;
	return 1;
}

int __wctomb_noconv(char *s, wchar_t wchar)
{
	if (s == NULL)
		return 0;
	*s = wchar;
	return 1;
}

int wctomb(char *s, wchar_t wchar)
{
	return _current_locale.ctype_cmpt_ptr->encode_wc(s, wchar);
}

size_t mbstowcs(wchar_t *pwcs, const char *s, size_t n)
{
	int result;
	size_t count;
	size_t source_len;

	source_len = strlen(s);
	count = 0;
	if (pwcs) {
		while (count < n) {
			if (*s) {
				result = mbtowc(pwcs++, s, source_len);
				if (result > 0) {
					s += result;
					source_len -= result;
				} else
					return (size_t)-1;
			} else {
				*pwcs = 0;
				break;
			}
			count++;
		}
	}
	return count;
}

size_t wcstombs(char *s, const wchar_t *pwcs, size_t n)
{
	size_t chars_written;
	int result;
	char temp[MB_LEN_MAX];

	chars_written = 0;
	if (!s || !pwcs)
		return 0;
	while (chars_written <= n) {
		if (!*pwcs) {
			s[chars_written] = 0;
			break;
		}
		result = wctomb(temp, *pwcs++);
		if (chars_written + result > n)
			break;
		strncpy(s + chars_written, temp, result);
		chars_written += result;
	}
	return chars_written;
}

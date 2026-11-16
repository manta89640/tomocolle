// MSL's formatted output to wide strings, as the C library of CodeWarrior for DS 2.0 has it: swprintf and vswprintf
// and the formatter behind them, the wide twin of printf.c's. This project's own decompilation of the game's code,
// written against that MSL's headers (stdio_api.h and ansi_fp.h included). The game links neither wprintf nor the
// file functions, so they are not written here.
//
// Numbers are converted as in printf.c, and float2str does it in a char buffer, which it then widens with
// mbstowcs. Measured from the code, where it differs from what one would expect:
// - In the %s of a narrow string with the '#' flag (a Pascal string), the length is read through buff_ptr, not from
//   the string; and %hhn stores nothing.
// - __wStringWrite returns what wmemcpy returns, where printf.c's __StringWrite returns 1.
// - double2hex passes __num2dec a copy of num (ld), and uppercases its INF and NAN for 'A' only; float2str asks
//   iswupper.
// How the source is written matters to the code in places: float2str's "p = buff + size; *--p = 0;" makes the
// compiler reload the decimal's fields after each store through p, as the original does, where "p = buff + size -
// 1" does not; __wpformatter's fill character is chosen between L'0' and L' ', not '0' and ' '.
#include <ansi_fp.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdio_api.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>

#define conversion_buff_size 512
#define conversion_max 509
#define bad_conversion 0xFFFF

enum justification_options {
	left_justification,
	right_justification,
	zero_fill
};

enum sign_options {
	only_minus,
	sign_always,
	space_holder
};

enum argument_options {
	normal_argument,
	char_argument,
	short_argument,
	long_argument,
	long_long_argument,
	wchar_argument,
	intmax_argument,
	size_t_argument,
	ptrdiff_argument,
	long_double_argument
};

typedef struct {
	unsigned char justification_options;
	unsigned char sign_options;
	unsigned char precision_specified;
	unsigned char alternate_form;
	unsigned char argument_options;
	wchar_t conversion_char;
	int field_width;
	int precision;
} print_format;

static const wchar_t *parse_format(const wchar_t *format_string, va_list *arg, print_format *format)
{
	print_format f;
	const wchar_t *s = format_string;
	wchar_t c;
	int flag_found;

	f.justification_options = right_justification;
	f.sign_options = only_minus;
	f.precision_specified = 0;
	f.alternate_form = 0;
	f.argument_options = normal_argument;
	f.field_width = 0;
	f.precision = 0;

	if ((c = *++s) == '%') {
		f.conversion_char = c;
		*format = f;
		return s + 1;
	}

	for (;;) {
		flag_found = 1;
		switch (c) {
		case '-':
			f.justification_options = left_justification;
			break;
		case '+':
			f.sign_options = sign_always;
			break;
		case ' ':
			if (f.sign_options != sign_always)
				f.sign_options = space_holder;
			break;
		case '#':
			f.alternate_form = 1;
			break;
		case '0':
			if (f.justification_options != left_justification)
				f.justification_options = zero_fill;
			break;
		default:
			flag_found = 0;
			break;
		}
		if (flag_found)
			c = *++s;
		else
			break;
	}

	if (c == '*') {
		if ((f.field_width = va_arg(*arg, int)) < 0) {
			f.justification_options = left_justification;
			f.field_width = -f.field_width;
		}
		c = *++s;
	} else {
		while (iswdigit(c)) {
			f.field_width = (f.field_width * 10) + (c - '0');
			c = *++s;
		}
	}

	if (f.field_width > conversion_max) {
		f.conversion_char = bad_conversion;
		*format = f;
		return s + 1;
	}

	if (c == '.') {
		f.precision_specified = 1;
		if ((c = *++s) == '*') {
			if ((f.precision = va_arg(*arg, int)) < 0)
				f.precision_specified = 0;
			c = *++s;
		} else {
			while (iswdigit(c)) {
				f.precision = (f.precision * 10) + (c - '0');
				c = *++s;
			}
		}
	}

	flag_found = 1;
	switch (c) {
	case 'h':
		f.argument_options = short_argument;
		if (s[1] == 'h') {
			f.argument_options = char_argument;
			c = *++s;
		}
		break;
	case 'l':
		f.argument_options = long_argument;
		if (s[1] == 'l') {
			f.argument_options = long_long_argument;
			c = *++s;
		}
		break;
	case 'L':
		f.argument_options = long_double_argument;
		break;
	case 'j':
		f.argument_options = intmax_argument;
		break;
	case 't':
		f.argument_options = ptrdiff_argument;
		break;
	case 'z':
		f.argument_options = size_t_argument;
		break;
	default:
		flag_found = 0;
		break;
	}
	if (flag_found)
		c = *++s;

	f.conversion_char = c;

	switch (c) {
	case 'd':
	case 'i':
	case 'u':
	case 'o':
	case 'x':
	case 'X':
		if (f.argument_options == long_double_argument)
			f.argument_options = long_long_argument;
		if (!f.precision_specified)
			f.precision = 1;
		else if (f.justification_options == zero_fill)
			f.justification_options = right_justification;
		break;
	case 'f':
	case 'F':
		if (f.argument_options == short_argument || f.argument_options == intmax_argument ||
		    f.argument_options == size_t_argument || f.argument_options == ptrdiff_argument ||
		    f.argument_options == long_long_argument) {
			f.conversion_char = bad_conversion;
			break;
		}
		if (!f.precision_specified)
			f.precision = 6;
		break;
	case 'a':
	case 'A':
		if (!f.precision_specified)
			f.precision = 13;
		if (f.argument_options == short_argument || f.argument_options == intmax_argument ||
		    f.argument_options == size_t_argument || f.argument_options == ptrdiff_argument ||
		    f.argument_options == long_long_argument || f.argument_options == char_argument)
			f.conversion_char = bad_conversion;
		break;
	case 'g':
	case 'G':
		if (!f.precision)
			f.precision = 1;
	case 'e':
	case 'E':
		if (f.argument_options == short_argument || f.argument_options == intmax_argument ||
		    f.argument_options == size_t_argument || f.argument_options == ptrdiff_argument ||
		    f.argument_options == long_long_argument || f.argument_options == char_argument) {
			f.conversion_char = bad_conversion;
			break;
		}
		if (!f.precision_specified)
			f.precision = 6;
		break;
	case 'p':
		f.argument_options = long_argument;
		f.alternate_form = 1;
		f.conversion_char = 'x';
		f.precision = 8;
		break;
	case 'c':
		if (f.argument_options == long_argument)
			f.argument_options = wchar_argument;
		else if (f.precision_specified || f.argument_options != normal_argument)
			f.conversion_char = bad_conversion;
		break;
	case 's':
		if (f.argument_options == long_argument)
			f.argument_options = wchar_argument;
		else if (f.argument_options != normal_argument)
			f.conversion_char = bad_conversion;
		break;
	case 'n':
		if (f.argument_options == long_double_argument)
			f.argument_options = long_long_argument;
		break;
	default:
		f.conversion_char = bad_conversion;
		break;
	}

	*format = f;
	return s + 1;
}

static wchar_t *long2str(long num, wchar_t *buff, print_format format)
{
	unsigned long unsigned_num, base;
	wchar_t *p;
	int n, digits;
	int minus;

	unsigned_num = num;
	minus = 0;

	p = buff;
	*--p = 0;
	digits = 0;

	if (!num && !format.precision && !(format.alternate_form && format.conversion_char == 'o'))
		return p;

	switch (format.conversion_char) {
	case 'd':
	case 'i':
		base = 10;
		if (num < 0) {
			if (num != LONG_MIN)
				unsigned_num = -num;
			minus = 1;
		}
		break;
	case 'o':
		base = 8;
		format.sign_options = only_minus;
		break;
	case 'u':
		base = 10;
		format.sign_options = only_minus;
		break;
	case 'x':
	case 'X':
		base = 16;
		format.sign_options = only_minus;
		break;
	}

	do {
		n = unsigned_num % base;
		unsigned_num /= base;
		if (n < 10)
			n += '0';
		else {
			n -= 10;
			if (format.conversion_char == 'x')
				n += 'a';
			else
				n += 'A';
		}
		*--p = n;
		++digits;
	} while (unsigned_num != 0);

	if (base == 8 && format.alternate_form && *p != '0') {
		*--p = '0';
		++digits;
	}

	if (format.justification_options == zero_fill) {
		format.precision = format.field_width;
		if (minus || format.sign_options != only_minus)
			--format.precision;
		if (base == 16 && format.alternate_form)
			format.precision -= 2;
	}

	if (buff - p + format.precision > conversion_max)
		return NULL;

	while (digits < format.precision) {
		*--p = '0';
		++digits;
	}

	if (base == 16 && format.alternate_form) {
		*--p = format.conversion_char;
		*--p = '0';
	}

	if (minus)
		*--p = '-';
	else if (format.sign_options == sign_always)
		*--p = '+';
	else if (format.sign_options == space_holder)
		*--p = ' ';

	return p;
}

static wchar_t *longlong2str(long long num, wchar_t *buff, print_format format)
{
	unsigned long long unsigned_num, base;
	wchar_t *p;
	int n, digits;
	int minus;

	unsigned_num = num;
	minus = 0;

	p = buff;
	*--p = 0;
	digits = 0;

	if (!num && !format.precision && !(format.alternate_form && format.conversion_char == 'o'))
		return p;

	switch (format.conversion_char) {
	case 'd':
	case 'i':
		base = 10;
		if (num < 0) {
			if (num != LLONG_MIN)
				unsigned_num = -num;
			minus = 1;
		}
		break;
	case 'o':
		base = 8;
		format.sign_options = only_minus;
		break;
	case 'u':
		base = 10;
		format.sign_options = only_minus;
		break;
	case 'x':
	case 'X':
		base = 16;
		format.sign_options = only_minus;
		break;
	}

	do {
		n = unsigned_num % base;
		unsigned_num /= base;
		if (n < 10)
			n += '0';
		else {
			n -= 10;
			if (format.conversion_char == 'x')
				n += 'a';
			else
				n += 'A';
		}
		*--p = n;
		++digits;
	} while (unsigned_num != 0);

	if (base == 8 && format.alternate_form && *p != '0') {
		*--p = '0';
		++digits;
	}

	if (format.justification_options == zero_fill) {
		format.precision = format.field_width;
		if (minus || format.sign_options != only_minus)
			--format.precision;
		if (base == 16 && format.alternate_form)
			format.precision -= 2;
	}

	if (buff - p + format.precision > conversion_max)
		return NULL;

	while (digits < format.precision) {
		*--p = '0';
		++digits;
	}

	if (base == 16 && format.alternate_form) {
		*--p = format.conversion_char;
		*--p = '0';
	}

	if (minus)
		*--p = '-';
	else if (format.sign_options == sign_always)
		*--p = '+';
	else if (format.sign_options == space_holder)
		*--p = ' ';

	return p;
}

static wchar_t *double2hex(long double num, wchar_t *buff, print_format format)
{
	wchar_t *p;
	unsigned char *q;
	unsigned char working_byte;
	long double ld;
	int expmask;
	unsigned snum;
	long exp;
	print_format exp_format;
	int hex_precision;
	int i, n;
	decform form;
	decimal dec;

	ld = num;
	if (format.precision > conversion_max)
		return NULL;

	form.style = FLOATDECIMAL;
	form.digits = SIGDIGLEN;
	__num2dec(&form, ld, &dec);

	if (fabs(num) == 0.0) {
		p = buff - 6;
		wcscpy(p, L"0x0p0");
		return p;
	}

	if (*dec.sig.text == 'I') {
		if (dec.sgn) {
			p = buff - 5;
			if (format.conversion_char == 'A')
				wcscpy(p, L"-INF");
			else
				wcscpy(p, L"-inf");
		} else {
			p = buff - 4;
			if (format.conversion_char == 'A')
				wcscpy(p, L"INF");
			else
				wcscpy(p, L"inf");
		}
		return p;
	} else if (*dec.sig.text == 'N') {
		if (dec.sgn) {
			p = buff - 5;
			if (format.conversion_char == 'A')
				wcscpy(p, L"-NAN");
			else
				wcscpy(p, L"-nan");
		} else {
			p = buff - 4;
			if (format.conversion_char == 'A')
				wcscpy(p, L"NAN");
			else
				wcscpy(p, L"nan");
		}
		return p;
	}

	exp_format.justification_options = right_justification;
	exp_format.sign_options = sign_always;
	exp_format.precision_specified = 0;
	exp_format.alternate_form = 0;
	exp_format.argument_options = normal_argument;
	exp_format.field_width = 0;
	exp_format.precision = 1;
	exp_format.conversion_char = 'd';

	// To big-endian, so that the sign and exponent come first
	{
		char *cp = (char *)&num;
		for (i = 0; i < 4; i++) {
			char t = cp[i];
			cp[i] = cp[7 - i];
			cp[7 - i] = t;
		}
	}

	expmask = 0x7FF;
	snum = ((unsigned char *)&num)[0] << 25 | ((unsigned char *)&num)[1] << 17;
	snum = (snum >> 21) & expmask;
	exp = (long)snum - 1023;
	p = long2str(exp, buff, exp_format);
	if (format.conversion_char == 'a')
		*--p = 'p';
	else
		*--p = 'P';

	q = (unsigned char *)&num;
	n = 11 + 4 * format.precision;
	for (hex_precision = format.precision; hex_precision >= 1; hex_precision--, n -= 4) {
		if (n < 64) {
			working_byte = q[n >> 3] >> (7 - (n & 7));
			if ((n & ~7) != ((n - 4) & ~7))
				working_byte |= (q[(n >> 3) - 1] << 8) >> (7 - (n & 7));
			working_byte &= 0x0F;
			if (working_byte < 10)
				working_byte += '0';
			else if (format.conversion_char == 'a')
				working_byte += 'a' - 10;
			else
				working_byte += 'A' - 10;
		} else
			working_byte = '0';
		*--p = working_byte;
	}

	if (format.precision || format.alternate_form)
		*--p = '.';

	*--p = '1';
	if (format.conversion_char == 'a')
		*--p = 'x';
	else
		*--p = 'X';
	*--p = '0';

	if (dec.sgn)
		*--p = '-';
	else if (format.sign_options == sign_always)
		*--p = '+';
	else if (format.sign_options == space_holder)
		*--p = ' ';

	return p;
}

static void round_decimal(decimal *dec, int new_length)
{
	char c;
	char *p;
	int carry;

	if (new_length < 0) {
	return_zero:
		dec->exp = 0;
		dec->sig.length = 1;
		*dec->sig.text = '0';
		return;
	}

	if (new_length >= dec->sig.length)
		return;

	p = (char *)dec->sig.text + new_length + 1;
	c = *--p - '0';

	if (c == 5) {
		char *q = &((char *)dec->sig.text)[dec->sig.length];
		while (--q > p && *q == '0')
			;
		carry = (q == p) ? p[-1] & 1 : 1;
	} else
		carry = (c > 5);

	while (new_length != 0) {
		c = *--p - '0' + carry;
		if ((carry = (c > 9)) != 0 || c == 0)
			--new_length;
		else {
			*p = c + '0';
			break;
		}
	}

	if (carry != 0) {
		dec->exp += 1;
		dec->sig.length = 1;
		*dec->sig.text = '1';
		return;
	} else if (new_length == 0)
		goto return_zero;

	dec->sig.length = new_length;
}

static wchar_t *float2str(long double num, wchar_t *wbuff, print_format format)
{
	decimal dec;
	decform form;
	char *p;
	char *q;
	int n, digits, sign;
	int int_digits, frac_digits;
	char buff[conversion_buff_size];
	wchar_t *wp;

	if (format.precision > conversion_max)
		return NULL;

	form.style = FLOATDECIMAL;
	form.digits = SIGDIGLEN;
	__num2dec(&form, num, &dec);

	p = (char *)dec.sig.text + dec.sig.length;
	while (dec.sig.length > 1 && *--p == '0') {
		--dec.sig.length;
		++dec.exp;
	}

	switch (*dec.sig.text) {
	case '0':
		dec.exp = 0;
		break;
	case 'I':
		if (num < 0) {
			wp = wbuff - 5;
			if (iswupper(format.conversion_char))
				wcscpy(wp, L"-INF");
			else
				wcscpy(wp, L"-inf");
		} else {
			wp = wbuff - 4;
			if (iswupper(format.conversion_char))
				wcscpy(wp, L"INF");
			else
				wcscpy(wp, L"inf");
		}
		return wp;
	case 'N':
		if (dec.sgn) {
			wp = wbuff - 5;
			if (iswupper(format.conversion_char))
				wcscpy(wp, L"-NAN");
			else
				wcscpy(wp, L"-nan");
		} else {
			wp = wbuff - 4;
			if (iswupper(format.conversion_char))
				wcscpy(wp, L"NAN");
			else
				wcscpy(wp, L"nan");
		}
		return wp;
	}

	dec.exp += dec.sig.length - 1;
	p = buff + conversion_buff_size;
	*--p = 0;

	switch (format.conversion_char) {
	case 'g':
	case 'G':
		if (dec.sig.length > format.precision)
			round_decimal(&dec, format.precision);

		if (dec.exp < -4 || dec.exp >= format.precision) {
			if (format.alternate_form)
				--format.precision;
			else
				format.precision = dec.sig.length - 1;

			if (format.conversion_char == 'g')
				format.conversion_char = 'e';
			else
				format.conversion_char = 'E';

			goto e_format;
		}

		if (format.alternate_form)
			format.precision -= dec.exp + 1;
		else if ((format.precision = dec.sig.length - (dec.exp + 1)) < 0)
			format.precision = 0;

		goto f_format;

	case 'e':
	case 'E':
	e_format:
		if (dec.sig.length > format.precision + 1)
			round_decimal(&dec, format.precision + 1);

		n = dec.exp;
		sign = '+';
		if (n < 0) {
			n = -n;
			sign = '-';
		}

		for (digits = 0; n || digits < 2; ++digits) {
			*--p = n % 10 + '0';
			n /= 10;
		}

		*--p = sign;
		*--p = format.conversion_char;

		if (buff - p + format.precision > conversion_max)
			return NULL;

		if (dec.sig.length < format.precision + 1)
			for (n = format.precision + 2 - dec.sig.length; --n;)
				*--p = '0';

		for (n = dec.sig.length, q = (char *)dec.sig.text + dec.sig.length; --n;)
			*--p = *--q;

		if (format.precision || format.alternate_form)
			*--p = '.';

		*--p = *dec.sig.text;

		if (dec.sgn)
			*--p = '-';
		else if (format.sign_options == sign_always)
			*--p = '+';
		else if (format.sign_options == space_holder)
			*--p = ' ';
		break;

	case 'f':
	case 'F':
	f_format:
		if ((frac_digits = -dec.exp + dec.sig.length - 1) < 0)
			frac_digits = 0;

		if (frac_digits > format.precision) {
			round_decimal(&dec, dec.sig.length - (frac_digits - format.precision));
			if ((frac_digits = -dec.exp + dec.sig.length - 1) < 0)
				frac_digits = 0;
		}

		if ((int_digits = dec.exp + 1) < 0)
			int_digits = 0;

		if (int_digits + frac_digits > conversion_max)
			return NULL;

		q = (char *)dec.sig.text + dec.sig.length;

		for (digits = 0; digits < (format.precision - frac_digits); ++digits)
			*--p = '0';

		for (digits = 0; digits < frac_digits && digits < dec.sig.length; ++digits)
			*--p = *--q;

		for (; digits < frac_digits; ++digits)
			*--p = '0';

		if (format.precision || format.alternate_form)
			*--p = '.';

		if (int_digits) {
			for (digits = 0; digits < int_digits - dec.sig.length; ++digits)
				*--p = '0';
			for (; digits < int_digits; ++digits)
				*--p = *--q;
		} else
			*--p = '0';

		if (dec.sgn)
			*--p = '-';
		else if (format.sign_options == sign_always)
			*--p = '+';
		else if (format.sign_options == space_holder)
			*--p = ' ';
		break;
	}

	wp = wbuff - strlen(p) - 1;
	mbstowcs(wp, p, strlen(p));
	return wp;
}

static int __wpformatter(void *(*WriteProc)(void *, const wchar_t *, size_t), void *WriteProcArg,
                         const wchar_t *format_str, va_list arg)
{
	int num_chars, chars_written, field_width;
	const wchar_t *format_ptr;
	const wchar_t *curr_format;
	print_format format;
	long long_num;
	long long long_long_num;
	long double long_double_num;
	wchar_t buff[conversion_buff_size];
	wchar_t *buff_ptr;
	wchar_t *wstring_ptr;
	char *cstring_ptr;
	int n;
	wchar_t fill_char = ' ';

	format_ptr = format_str;
	chars_written = 0;

	while (*format_ptr) {
		if (!(curr_format = wcschr(format_ptr, '%'))) {
			num_chars = wcslen(format_ptr);
			chars_written += num_chars;
			if (num_chars && !(*WriteProc)(WriteProcArg, format_ptr, num_chars))
				return -1;
			break;
		}

		num_chars = curr_format - format_ptr;
		chars_written += num_chars;
		if (num_chars && !(*WriteProc)(WriteProcArg, format_ptr, num_chars))
			return -1;

		format_ptr = parse_format(curr_format, &arg, &format);

		switch (format.conversion_char) {
		case 'd':
		case 'i':
			if (format.argument_options == long_argument)
				long_num = va_arg(arg, long);
			else if (format.argument_options == long_long_argument)
				long_long_num = va_arg(arg, long long);
			else if (format.argument_options == intmax_argument)
				long_long_num = va_arg(arg, intmax_t);
			else if (format.argument_options == size_t_argument)
				long_num = va_arg(arg, size_t);
			else if (format.argument_options == ptrdiff_argument)
				long_num = va_arg(arg, ptrdiff_t);
			else
				long_num = va_arg(arg, int);

			if (format.argument_options == short_argument)
				long_num = (short)long_num;

			if (format.argument_options == long_long_argument || format.argument_options == intmax_argument) {
				if (!(buff_ptr = longlong2str(long_long_num, buff + conversion_buff_size, format)))
					goto conversion_error;
			} else {
				if (!(buff_ptr = long2str(long_num, buff + conversion_buff_size, format)))
					goto conversion_error;
			}

			num_chars = buff + conversion_buff_size - 1 - buff_ptr;
			break;

		case 'o':
		case 'u':
		case 'x':
		case 'X':
			if (format.argument_options == long_argument)
				long_num = va_arg(arg, unsigned long);
			else if (format.argument_options == long_long_argument)
				long_long_num = va_arg(arg, long long);
			else if (format.argument_options == intmax_argument)
				long_long_num = va_arg(arg, intmax_t);
			else if (format.argument_options == size_t_argument)
				long_num = va_arg(arg, size_t);
			else if (format.argument_options == ptrdiff_argument)
				long_num = va_arg(arg, ptrdiff_t);
			else
				long_num = va_arg(arg, unsigned int);

			if (format.argument_options == short_argument)
				long_num = (unsigned short)long_num;

			if (format.argument_options == long_long_argument || format.argument_options == intmax_argument) {
				if (!(buff_ptr = longlong2str(long_long_num, buff + conversion_buff_size, format)))
					goto conversion_error;
			} else {
				if (!(buff_ptr = long2str(long_num, buff + conversion_buff_size, format)))
					goto conversion_error;
			}

			num_chars = buff + conversion_buff_size - 1 - buff_ptr;
			break;

		case 'f':
		case 'F':
		case 'e':
		case 'E':
		case 'g':
		case 'G':
			if (format.argument_options == long_double_argument)
				long_double_num = va_arg(arg, long double);
			else
				long_double_num = va_arg(arg, double);

			if (!(buff_ptr = float2str(long_double_num, buff + conversion_buff_size, format)))
				goto conversion_error;

			num_chars = buff + conversion_buff_size - 1 - buff_ptr;
			break;

		case 'a':
		case 'A':
			if (format.argument_options == long_double_argument)
				long_double_num = va_arg(arg, long double);
			else
				long_double_num = va_arg(arg, double);

			if (!(buff_ptr = double2hex(long_double_num, buff + conversion_buff_size, format)))
				goto conversion_error;

			num_chars = buff + conversion_buff_size - 1 - buff_ptr;
			break;

		case 's':
			if (format.argument_options == wchar_argument) {
				buff_ptr = va_arg(arg, wchar_t *);
				if (buff_ptr == NULL)
					buff_ptr = L"";
				if (format.alternate_form) {
					num_chars = (unsigned char)*buff_ptr++;
					if (format.precision_specified && num_chars > format.precision)
						num_chars = format.precision;
				} else if (format.precision_specified) {
					num_chars = format.precision;
					if ((wstring_ptr = wmemchr(buff_ptr, 0, num_chars)) != NULL)
						num_chars = wstring_ptr - buff_ptr;
				} else
					num_chars = wcslen(buff_ptr);
			} else {
				cstring_ptr = va_arg(arg, char *);
				if (cstring_ptr == NULL)
					cstring_ptr = "";
				if (format.alternate_form) {
					n = (unsigned char)*buff_ptr;
					if (format.precision_specified && n > format.precision)
						n = format.precision;
				} else if (format.precision_specified) {
					char *end;
					n = format.precision;
					if ((end = memchr(cstring_ptr, 0, n)) != NULL)
						n = end - cstring_ptr;
				} else
					n = strlen(cstring_ptr);
				if ((num_chars = mbstowcs(buff, cstring_ptr, n)) < 0)
					goto conversion_error;
				buff_ptr = buff;
			}
			break;

		case 'n':
			buff_ptr = va_arg(arg, wchar_t *);

			switch (format.argument_options) {
			case normal_argument:
				*(int *)buff_ptr = chars_written;
				break;
			case short_argument:
				*(short *)buff_ptr = chars_written;
				break;
			case long_argument:
				*(long *)buff_ptr = chars_written;
				break;
			case intmax_argument:
				*(intmax_t *)buff_ptr = chars_written;
				break;
			case size_t_argument:
				*(size_t *)buff_ptr = chars_written;
				break;
			case ptrdiff_argument:
				*(ptrdiff_t *)buff_ptr = chars_written;
				break;
			case long_long_argument:
				*(long long *)buff_ptr = chars_written;
				break;
			}
			continue;

		case 'c':
			buff_ptr = buff;
			if (format.argument_options == wchar_argument) {
				*buff_ptr = va_arg(arg, int);
				num_chars = 1;
			} else {
				char chint = va_arg(arg, int);
				num_chars = mbtowc(buff, &chint, 1);
			}
			break;

		case '%':
			buff_ptr = buff;
			*buff_ptr = '%';
			num_chars = 1;
			break;

		case bad_conversion:
		default:
		conversion_error:
			num_chars = wcslen(curr_format);
			chars_written += num_chars;
			if (num_chars && !(*WriteProc)(WriteProcArg, curr_format, num_chars))
				return -1;
			return chars_written;
		}

		field_width = num_chars;

		if (format.justification_options != left_justification) {
			fill_char = (format.justification_options == zero_fill) ? L'0' : L' ';
			if ((*buff_ptr == '+' || *buff_ptr == '-' || *buff_ptr == ' ') && fill_char == '0') {
				if ((*WriteProc)(WriteProcArg, buff_ptr, 1) == 0)
					return -1;
				++buff_ptr;
				num_chars--;
			}
			while (field_width < format.field_width) {
				if ((*WriteProc)(WriteProcArg, &fill_char, 1) == 0)
					return -1;
				++field_width;
			}
		}

		if (num_chars && !(*WriteProc)(WriteProcArg, buff_ptr, num_chars))
			return -1;

		if (format.justification_options == left_justification) {
			while (field_width < format.field_width) {
				wchar_t blank = ' ';
				if ((*WriteProc)(WriteProcArg, &blank, 1) == 0)
					return -1;
				++field_width;
			}
		}

		chars_written += field_width;
	}

	return chars_written;
}

void *__wStringWrite(void *osc, const wchar_t *Buffer, size_t NumChars)
{
	size_t CharsToBeWritten;
	void *result;
	__wOutStrCtrl *Oscp = (__wOutStrCtrl *)osc;

	CharsToBeWritten = ((Oscp->CharsWritten + NumChars) <= Oscp->MaxCharCount) ? NumChars
	                                                                           : Oscp->MaxCharCount - Oscp->CharsWritten;
	result = wmemcpy(Oscp->wCharStr + Oscp->CharsWritten, Buffer, CharsToBeWritten);
	Oscp->CharsWritten += CharsToBeWritten;
	return result;
}

int swprintf(wchar_t *s, size_t n, const wchar_t *format, ...)
{
	va_list args;
	va_start(args, format);
	return vswprintf(s, n, format, args);
}

int vswprintf(wchar_t *s, size_t n, const wchar_t *format, va_list arg)
{
	int count;
	__wOutStrCtrl osc;

	osc.wCharStr = s;
	osc.MaxCharCount = n;
	osc.CharsWritten = 0;

	count = __wpformatter(&__wStringWrite, &osc, format, arg);

	if (count >= 0) {
		if (count < n)
			s[count] = 0;
		else {
			s[n - 1] = 0;
			count = -1;
		}
	}
	return count;
}

// MSL's formatted output (printf.c), as the C library of CodeWarrior for DS 2.0 has it. This project's own
// decompilation of the game's code, written against that MSL's headers. The game links only what it calls: the
// conversion helpers, __pformatter, __StringWrite, vsnprintf and snprintf. The file's other functions (__FileWrite,
// printf, vprintf, vfprintf, vsprintf, sprintf) were stripped from the game; they are written from the compiler's
// archive, whose printf.o this file compiles to function for function.
//
// Where the source takes a particular form to match:
// - double2hex copies its fixed results ("0x0p0", "inf", ...) with memcpy, which the compiler expands to byte
//   copies; strcpy stays a call.
// - double2hex passes __num2dec a copy of num made before the precision check: the original loads num there.
// - In double2hex, the exponent's mask is assigned back to snum: computed straight into exp, it lands in another
//   register.
#include <ansi_fp.h>
#include <ctype.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>
#include <stdio_api.h>
#include <critical_regions.h>

#define conversion_buff_size 512
#define conversion_max 509
#define bad_conversion 0xFF

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
	unsigned char conversion_char;
	int field_width;
	int precision;
} print_format;

static const char *parse_format(const char *format_string, va_list *arg, print_format *format)
{
	print_format f;
	const char *s = format_string;
	int c;
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
		while (isdigit(c)) {
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
			while (isdigit(c)) {
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
		if (f.argument_options == long_double_argument) {
			f.conversion_char = bad_conversion;
			break;
		}
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
		// fall through
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
		f.conversion_char = 'x';
		f.alternate_form = 1;
		f.argument_options = long_argument;
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
			f.conversion_char = bad_conversion;
		break;

	default:
		f.conversion_char = bad_conversion;
		break;
	}

	*format = f;
	return s + 1;
}

static char *long2str(long num, char *buff, print_format format)
{
	unsigned long unsigned_num, base;
	char *p;
	int n, digits;
	int minus = 0;

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

static char *longlong2str(long long num, char *buff, print_format format)
{
	unsigned long long unsigned_num, base;
	char *p;
	int n, digits;
	int minus = 0;

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

static char *double2hex(long double num, char *buff, print_format format)
{
	char *p;
	unsigned char *q;
	unsigned char working_byte;
	int expbits, expmask;
	unsigned long snum;
	long exp;
	print_format exp_format;
	int hex_precision;
	int offsetbits;
	int i;
	char c;
	decform form;
	decimal dec;
	long double ld;

	p = buff;
	ld = num;

	if (format.precision > conversion_max)
		return NULL;

	form.style = FLOATDECIMAL;
	form.digits = SIGDIGLEN;
	__num2dec(&form, ld, &dec);

	if (fabs(num) == 0.0) {
		p = buff - 6;
		memcpy(p, "0x0p0", 6);
		return p;
	}

	if (*dec.sig.text == 'I') {
		if (dec.sgn) {
			p = buff - 5;
			if (format.conversion_char == 'A')
				memcpy(p, "-INF", 5);
			else
				memcpy(p, "-inf", 5);
		} else {
			p = buff - 4;
			if (format.conversion_char == 'A')
				memcpy(p, "INF", 4);
			else
				memcpy(p, "inf", 4);
		}
		return p;
	} else if (*dec.sig.text == 'N') {
		if (dec.sgn) {
			p = buff - 5;
			if (format.conversion_char == 'A')
				memcpy(p, "-NAN", 5);
			else
				memcpy(p, "-nan", 5);
		} else {
			p = buff - 4;
			if (format.conversion_char == 'A')
				memcpy(p, "NAN", 4);
			else
				memcpy(p, "nan", 4);
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

	// num's bytes in big-endian order: the sign and exponent first, then the mantissa from its top
	{
		char *s = (char *)&num;
		for (i = 0; i < 4; i++) {
			c = s[i];
			s[i] = s[7 - i];
			s[7 - i] = c;
		}
	}

	expbits = 11;
	expmask = 0x7FF;
	snum = ((unsigned char *)&num)[0] << 25;
	snum |= ((unsigned char *)&num)[1] << 17;
	snum = (snum >> (32 - expbits)) & expmask;
	exp = snum - (expmask >> 1);

	p = long2str(exp, buff, exp_format);
	if (format.conversion_char == 'a')
		*--p = 'p';
	else
		*--p = 'P';

	q = (unsigned char *)&num;
	offsetbits = format.precision * 4 + expbits;
	for (hex_precision = format.precision; hex_precision >= 1; hex_precision--) {
		if (offsetbits < 64) {
			working_byte = q[offsetbits >> 3] >> (7 - (offsetbits & 7));
			if ((offsetbits & ~7) != ((offsetbits - 4) & ~7))
				working_byte |= (q[(offsetbits >> 3) - 1] << 8) >> (7 - (offsetbits & 7));
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
		offsetbits -= 4;
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

static char *float2str(long double num, char *buff, print_format format)
{
	decimal dec;
	decform form;
	char *p;
	char *q;
	int n, digits, sign;
	int int_digits, frac_digits;

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
			p = buff - 5;
			if (isupper(format.conversion_char))
				memcpy(p, "-INF", 5);
			else
				memcpy(p, "-inf", 5);
		} else {
			p = buff - 4;
			if (isupper(format.conversion_char))
				memcpy(p, "INF", 4);
			else
				memcpy(p, "inf", 4);
		}
		return p;

	case 'N':
		if (dec.sgn) {
			p = buff - 5;
			if (isupper(format.conversion_char))
				memcpy(p, "-NAN", 5);
			else
				memcpy(p, "-nan", 5);
		} else {
			p = buff - 4;
			if (isupper(format.conversion_char))
				memcpy(p, "NAN", 4);
			else
				memcpy(p, "nan", 4);
		}
		return p;
	}

	dec.exp += dec.sig.length - 1;
	p = buff;
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
		else {
			if ((format.precision = dec.sig.length - (dec.exp + 1)) < 0)
				format.precision = 0;
		}

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
			for (n = format.precision + 1 - dec.sig.length + 1; --n;)
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

	return p;
}

static int __pformatter(void *(*WriteProc)(void *, const char *, size_t), void *WriteProcArg,
                        const char *format_str, va_list arg)
{
	int num_chars, chars_written, field_width;
	const char *format_ptr;
	const char *curr_format;
	print_format format;
	long long_num;
	long long long_long_num;
	long double long_double_num;
	char buff[conversion_buff_size];
	char *buff_ptr;
	char *string_end;
	char fill_char = ' ';

	format_ptr = format_str;
	chars_written = 0;

	while (*format_ptr) {
		if (!(curr_format = strchr(format_ptr, '%'))) {
			num_chars = strlen(format_ptr);
			chars_written += num_chars;

			if (num_chars && !(*WriteProc)(WriteProcArg, format_ptr, num_chars))
				return -1;

			break;
		}

		num_chars = curr_format - format_ptr;
		chars_written += num_chars;

		if (num_chars && !(*WriteProc)(WriteProcArg, format_ptr, num_chars))
			return -1;

		format_ptr = curr_format;
		format_ptr = parse_format(format_ptr, &arg, &format);

		switch (format.conversion_char) {
		case 'd':
		case 'i':
			if (format.argument_options == long_argument)
				long_num = va_arg(arg, long);
			else if (format.argument_options == long_long_argument)
				long_long_num = va_arg(arg, long long);
			else if (format.argument_options == intmax_argument)
				long_long_num = va_arg(arg, long long);
			else if (format.argument_options == size_t_argument)
				long_num = va_arg(arg, size_t);
			else if (format.argument_options == ptrdiff_argument)
				long_num = va_arg(arg, ptrdiff_t);
			else
				long_num = va_arg(arg, int);

			if (format.argument_options == short_argument)
				long_num = (short)long_num;

			if (format.argument_options == char_argument)
				long_num = (signed char)long_num;

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
				long_long_num = va_arg(arg, long long);
			else if (format.argument_options == size_t_argument)
				long_num = va_arg(arg, size_t);
			else if (format.argument_options == ptrdiff_argument)
				long_num = va_arg(arg, ptrdiff_t);
			else
				long_num = va_arg(arg, unsigned int);

			if (format.argument_options == short_argument)
				long_num = (unsigned short)long_num;

			if (format.argument_options == char_argument)
				long_num = (unsigned char)long_num;

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
				wchar_t *wcs_ptr = va_arg(arg, wchar_t *);

				if (wcs_ptr == NULL)
					wcs_ptr = L"";

				if ((num_chars = wcstombs(buff, wcs_ptr, sizeof(buff))) < 0)
					goto conversion_error;

				buff_ptr = &buff[0];
			} else
				buff_ptr = va_arg(arg, char *);

			if (buff_ptr == NULL)
				buff_ptr = "";

			if (format.alternate_form) {
				num_chars = (unsigned char)*buff_ptr++;

				if (format.precision_specified && num_chars > format.precision)
					num_chars = format.precision;
			} else if (format.precision_specified) {
				num_chars = format.precision;

				if ((string_end = (char *)memchr(buff_ptr, 0, num_chars)) != 0)
					num_chars = string_end - buff_ptr;
			} else
				num_chars = strlen(buff_ptr);

			break;

		case 'n':
			buff_ptr = va_arg(arg, char *);

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
				*(long long *)buff_ptr = chars_written;
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
			*buff_ptr = va_arg(arg, int);
			num_chars = 1;
			break;

		case '%':
			buff_ptr = buff;
			*buff_ptr = '%';
			num_chars = 1;
			break;

		case bad_conversion:
		default:
		conversion_error:
			num_chars = strlen(curr_format);
			chars_written += num_chars;

			if (num_chars && !(*WriteProc)(WriteProcArg, curr_format, num_chars))
				return -1;

			return chars_written;
		}

		field_width = num_chars;

		if (format.justification_options != left_justification) {
			fill_char = (format.justification_options == zero_fill) ? '0' : ' ';

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
				char blank = ' ';

				if ((*WriteProc)(WriteProcArg, &blank, 1) == 0)
					return -1;

				++field_width;
			}
		}

		chars_written += field_width;
	}

	return chars_written;
}

void *__FileWrite(void *File, const char *Buffer, size_t NumChars)
{
	return __fwrite(Buffer, 1, NumChars, (FILE *)File) == NumChars ? File : NULL;
}

void *__StringWrite(void *osc, const char *Buffer, size_t NumChars)
{
	size_t CharsToBeWritten;
	__OutStrCtrl *Oscp = (__OutStrCtrl *)osc;

	CharsToBeWritten = (Oscp->CharsWritten + NumChars <= Oscp->MaxCharCount) ? NumChars
	                                                                         : Oscp->MaxCharCount - Oscp->CharsWritten;
	memcpy(Oscp->CharStr + Oscp->CharsWritten, Buffer, CharsToBeWritten);
	Oscp->CharsWritten += CharsToBeWritten;
	return (void *)1;
}

int printf(const char *format, ...)
{
	int result;
	va_list args;

	if (fwide(stdout, -1) >= 0)
		return -1;

	__begin_critical_region(stdout_access);
	va_start(args, format);
	result = __pformatter(&__FileWrite, stdout, format, args);
	__end_critical_region(stdout_access);

	return result;
}

int vprintf(const char *format, va_list arg)
{
	int result;

	if (fwide(stdout, -1) >= 0)
		return -1;

	__begin_critical_region(stdout_access);
	result = __pformatter(&__FileWrite, stdout, format, arg);
	__end_critical_region(stdout_access);

	return result;
}

int vfprintf(FILE *file, const char *format, va_list arg)
{
	int crtrgn;
	int result;

	if (fwide(file, -1) >= 0)
		return -1;

	if (file == stdout)
		crtrgn = stdout_access;
	else if (file == stderr)
		crtrgn = stderr_access;
	else
		crtrgn = files_access;

	__begin_critical_region(crtrgn);
	result = __pformatter(&__FileWrite, file, format, arg);
	__end_critical_region(crtrgn);

	return result;
}

int vsnprintf(char *s, size_t n, const char *format, va_list arg)
{
	int end;
	__OutStrCtrl osc;

	osc.CharStr = s;
	osc.MaxCharCount = n;
	osc.CharsWritten = 0;

	end = __pformatter(&__StringWrite, &osc, format, arg);

	if (s) {
		if (end < n)
			s[end] = '\0';
		else if (n > 0)
			s[n - 1] = '\0';
	}

	return end;
}

int vsprintf(char *s, const char *format, va_list arg)
{
	return vsnprintf(s, ULONG_MAX, format, arg);
}

int snprintf(char *s, size_t n, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	return vsnprintf(s, n, format, args);
}

int sprintf(char *s, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	return vsnprintf(s, ULONG_MAX, format, args);
}

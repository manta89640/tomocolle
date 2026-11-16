// MSL's conversion of text to floating point, as the C library of CodeWarrior for DS 2.0 has it (MSL_Common). This
// project's own decompilation of the game's code, written against that MSL's headers. The game links only __strtold,
// the scanner that strtod, strtof, atof and scanf's %f share: it reads the text through ReadProc one character at a
// time, as a state machine, and hands the decimal digits to __dec2num, or builds a hexadecimal number's bits itself.
//
// Measured against the archive's strtold.o:
// - The limit on hexadecimal digits is a variable set to 14 when "0x" is read, not a literal: only then does the
//   compiler compare against a register (`mov r0,#14; cmp r2,r0`), and only then are the zeroing stores at the top
//   the original's.
// - The mantissa is cleared with memset (inlined as byte stores), and the result's bytes are a union declared in
//   the block that builds them, which puts it, 8-aligned, below the other arrays as in the original's frame.
// Not matching yet: 64 of 1171 instructions, all register choices: j and p of the NaN state swap r6 and r7, and in
// the hexadecimal tail the result pointer and the mantissa's address swap r3 and r4, the bit loop's counter and
// byte index r2 and r7.

#include <ansi_fp.h>
#include <ctype.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <strtold.h>
#include <string.h>

enum {
	start               = 0x0001,
	sig_start           = 0x0002,
	leading_sig_zeroes  = 0x0004,
	int_digit_loop      = 0x0008,
	frac_start          = 0x0010,
	frac_digit_loop     = 0x0020,
	sig_end             = 0x0040,
	exp_start           = 0x0080,
	leading_exp_digit   = 0x0100,
	leading_exp_zeroes  = 0x0200,
	exp_digit_loop      = 0x0400,
	finished            = 0x0800,
	failure             = 0x1000,
	nan_state           = 0x2000,
	infin_state         = 0x4000,
	hex_state           = 0x8000
};

// The states of a hexadecimal number, after its "0x"
enum {
	not_hex             = 0x0000,
	hex_start           = 0x0001,
	hex_leading_zeroes  = 0x0002,
	hex_int_digits      = 0x0004,
	hex_frac_digits     = 0x0008,
	hex_exp_mark        = 0x0010,
	hex_exp_sign        = 0x0020,
	hex_leading_exp     = 0x0040,
	hex_exp_zeroes      = 0x0080,
	hex_exp_digits      = 0x0100
};

#define final_state(s)  ((s) & (finished | failure))
#define success(s)      ((s) & (leading_sig_zeroes | int_digit_loop | frac_digit_loop | \
                                leading_exp_zeroes | exp_digit_loop | finished))
#define hex_success(s)  ((s) & (hex_leading_zeroes | hex_int_digits | hex_frac_digits | \
                                hex_exp_zeroes | hex_exp_digits))

#define fetch()         (count++, (*ReadProc)(ReadProcArg, 0, __GetAChar))
#define unfetch(c)      (*ReadProc)(ReadProcArg, c, __UngetAChar)

long double __strtold(int max_width, int (*ReadProc)(void *, int, int), void *ReadProcArg, int *chars_scanned,
                      int *overflow)
{
	int scan_state = start;
	int hex_scan_state = not_hex;
	int count = 0;
	int spaces = 0;
	int c;
	decimal d = {0};
	int sig_negative = 0;
	int exp_negative = 0;
	long exp_value = 0;
	int exp_adjust = 0;
	long double result;
	int sign_detected = 0;
	unsigned char *chptr;
	unsigned char mantissa[8];
	long exp = 0;
	int expsign = 0;
	unsigned intdigits = 0;
	int nibble;
	unsigned max_digits;

	*overflow = 0;
	c = fetch();

	while (count <= max_width && c != EOF && !final_state(scan_state)) {
		switch (scan_state) {
		case start:
			if (isspace(c)) {
				c = (*ReadProc)(ReadProcArg, 0, __GetAChar);
				++spaces;
				break;
			}
			switch (toupper(c)) {
			case '-':
				sig_negative = 1;
			case '+':
				c = fetch();
				sign_detected = 1;
				break;
			case 'I':
				c = fetch();
				scan_state = infin_state;
				break;
			case 'N':
				c = fetch();
				scan_state = nan_state;
				break;
			default:
				scan_state = sig_start;
				break;
			}
			break;

		case infin_state: {
			int i = 1;
			char infinity[] = "INFINITY";
			char *p = &infinity[1];
			while (i < 8 && *p == toupper(c)) {
				++p;
				++i;
				c = fetch();
			}
			if (i == 3 || i == 8) {
				if (sig_negative)
					result = -INFINITY;
				else
					result = INFINITY;
				*chars_scanned = spaces + i + sign_detected;
				return result;
			}
			scan_state = failure;
			break;
		}

		case nan_state: {
			int i = 1;
			int j = 0;
			char nan_str[] = "NAN(";
			char nan_arg[32] = "";
			char *p = &nan_str[1];
			while (i < 4 && *p == toupper(c)) {
				++p;
				++i;
				c = fetch();
			}
			if (i == 3 || i == 4) {
				if (i == 4) {
					while (j < 32 && (isdigit(c) || isalpha(c) || c == '.')) {
						nan_arg[j++] = c;
						c = fetch();
					}
					if (c != ')') {
						scan_state = failure;
						break;
					}
					j++;
				}
				nan_arg[j] = '\0';
				if (sig_negative)
					result = -nan(nan_arg);
				else
					result = nan(nan_arg);
				*chars_scanned = spaces + i + j + sign_detected;
				return result;
			}
			scan_state = failure;
			break;
		}

		case sig_start:
			if (c == '.') {
				scan_state = frac_start;
				c = fetch();
				break;
			}
			if (!isdigit(c)) {
				scan_state = failure;
				break;
			}
			if (c == '0') {
				c = fetch();
				if (toupper(c) == 'X') {
					scan_state = hex_state;
					hex_scan_state = hex_start;
				} else
					scan_state = leading_sig_zeroes;
				break;
			}
			scan_state = int_digit_loop;
			break;

		case leading_sig_zeroes:
			if (c == '0') {
				c = fetch();
				break;
			}
			scan_state = int_digit_loop;
			break;

		case int_digit_loop:
			if (!isdigit(c)) {
				if (c == '.') {
					scan_state = frac_digit_loop;
					c = fetch();
				} else
					scan_state = sig_end;
				break;
			}
			if (d.sig.length < 20)
				d.sig.text[d.sig.length++] = c;
			else
				exp_adjust++;
			c = fetch();
			break;

		case frac_start:
			if (!isdigit(c)) {
				scan_state = failure;
				break;
			}
			scan_state = frac_digit_loop;
			break;

		case frac_digit_loop:
			if (!isdigit(c)) {
				scan_state = sig_end;
				break;
			}
			if (d.sig.length < 20) {
				if (c != '0' || d.sig.length)
					d.sig.text[d.sig.length++] = c;
				exp_adjust--;
			}
			c = fetch();
			break;

		case sig_end:
			if (toupper(c) == 'E') {
				scan_state = exp_start;
				c = fetch();
				break;
			}
			scan_state = finished;
			break;

		case exp_start:
			if (c == '+')
				c = fetch();
			else if (c == '-') {
				c = fetch();
				exp_negative = 1;
			}
			scan_state = leading_exp_digit;
			break;

		case leading_exp_digit:
			if (!isdigit(c)) {
				scan_state = failure;
				break;
			}
			if (c == '0') {
				scan_state = leading_exp_zeroes;
				c = fetch();
				break;
			}
			scan_state = exp_digit_loop;
			break;

		case leading_exp_zeroes:
			if (c == '0') {
				c = fetch();
				break;
			}
			scan_state = exp_digit_loop;
			break;

		case exp_digit_loop:
			if (!isdigit(c)) {
				scan_state = finished;
				break;
			}
			exp_value = exp_value * 10 + (c - '0');
			if (exp_value > SHRT_MAX)
				*overflow = 1;
			c = fetch();
			break;

		case hex_state:
			switch (hex_scan_state) {
			case hex_start:
				chptr = mantissa;
				memset(chptr, 0, 8);
				intdigits = 0;
				max_digits = 14;
				nibble = 0;
				hex_scan_state = hex_leading_zeroes;
				c = fetch();
				break;

			case hex_leading_zeroes:
				if (c == '0') {
					c = fetch();
					break;
				}
				hex_scan_state = hex_int_digits;
				break;

			case hex_int_digits:
				if (!isxdigit(c)) {
					if (c == '.') {
						hex_scan_state = hex_frac_digits;
						c = fetch();
					} else
						hex_scan_state = hex_exp_mark;
					break;
				}
				if (intdigits < max_digits) {
					unsigned char uch, digit;
					int ui;
					intdigits++;
					uch = chptr[nibble / 2];
					ui = toupper(c);
					if (ui >= 'A')
						ui = ui - 'A' + 10;
					else
						ui -= '0';
					digit = ui;
					if (nibble % 2)
						uch |= digit;
					else
						uch |= (unsigned char)(digit << 4);
					chptr[nibble++ / 2] = uch;
					c = fetch();
				} else
					c = fetch();
				break;

			case hex_frac_digits:
				if (!isxdigit(c)) {
					hex_scan_state = hex_exp_mark;
					break;
				}
				if (intdigits < max_digits) {
					unsigned char uch, digit;
					int ui;
					uch = chptr[nibble / 2];
					ui = toupper(c);
					if (ui >= 'A')
						ui = ui - 'A' + 10;
					else
						ui -= '0';
					digit = ui;
					if (nibble % 2)
						uch |= digit;
					else
						uch |= (unsigned char)(digit << 4);
					chptr[nibble++ / 2] = uch;
					c = fetch();
				} else
					c = fetch();
				break;

			case hex_exp_mark:
				if (toupper(c) == 'P') {
					hex_scan_state = hex_exp_sign;
					c = fetch();
				} else
					scan_state = finished;
				break;

			case hex_exp_sign:
				if (c == '-')
					expsign = 1;
				else if (c != '+') {
					unfetch(c);
					count--;
				}
				hex_scan_state = hex_leading_exp;
				c = fetch();
				break;

			case hex_leading_exp:
				if (!isdigit(c)) {
					scan_state = failure;
					break;
				}
				if (c == '0') {
					hex_scan_state = hex_exp_zeroes;
					c = fetch();
					break;
				}
				hex_scan_state = hex_exp_digits;
				break;

			case hex_exp_zeroes:
				if (c == '0') {
					c = fetch();
					break;
				}
				hex_scan_state = hex_exp_digits;
				break;

			case hex_exp_digits:
				if (!isdigit(c)) {
					scan_state = finished;
					break;
				}
				exp = exp * 10 + (c - '0');
				if (exp_value > SHRT_MAX)
					*overflow = 1;
				c = fetch();
				break;
			}
			break;
		}
	}

	if (scan_state != hex_state ? !success(scan_state) : (count - 1 <= 2 || !hex_success(hex_scan_state)))
		*chars_scanned = 0;
	else
		*chars_scanned = count - 1 + spaces;

	unfetch(c);

	if (hex_scan_state == not_hex) {
		if (exp_negative)
			exp_value = -exp_value;

		{
			int n = d.sig.length;
			unsigned char *p = &d.sig.text[n];
			while (n-- && *--p == '0')
				exp_adjust++;
			d.sig.length = n + 1;
			if (d.sig.length == 0)
				d.sig.text[d.sig.length++] = '0';
		}

		exp_value += exp_adjust;
		if (exp_value < SHRT_MIN || exp_value > SHRT_MAX)
			*overflow = 1;

		if (*overflow) {
			if (exp_negative)
				return 0.0;
			if (sig_negative)
				return -HUGE_VAL;
			return HUGE_VAL;
		}

		d.exp = exp_value;
		result = __dec2num(&d);

		if (result != 0.0 && result < LDBL_MIN)
			*overflow = 1;
		else if (result > LDBL_MAX) {
			*overflow = 1;
			result = HUGE_VAL;
		}

		if (sig_negative && success(scan_state))
			result = -result;
		return result;
	} else {
		union {
			long double ld;
			unsigned char uc[8];
		} uld;
		unsigned i;
		unsigned shift;
		int k;
		unsigned char *p = uld.uc;
		unsigned char carry;

		if (expsign)
			exp = -exp;
		exp += intdigits * 4;

		for (i = 0; i < 4 && !(mantissa[0] & (0x80 >> i)); i++)
			exp--;

		shift = i + 1;
		if (shift) {
			carry = 0;
			for (chptr = mantissa + 7; chptr >= mantissa; chptr--) {
				unsigned char uch = *chptr;
				*chptr = (uch << shift) | carry;
				carry = uch >> (8 - shift);
			}
		}

		memset(p, 0, 8);

		{
			unsigned bitpos = 12;
			k = 1;
			for (i = 0; i < 52; i += 8) {
				unsigned char uch = mantissa[i / 8];
				if (i + 8 > 52)
					uch &= 0xff << (52 - i);
				p[k] |= (unsigned char)(uch >> (bitpos % 8));
				k++;
				p[k] |= (unsigned char)(uch << (8 - bitpos % 8));
				bitpos += 8;
			}
		}

		exp += 1022;
		if (exp & ~0x7ff) {
			*overflow = 1;
			return 0.0;
		}
		p[1] |= (unsigned long)(exp << 21) >> 17;
		p[0] |= (unsigned long)(exp << 21) >> 25;
		if (sig_negative)
			p[0] |= 0x80;

		for (k = 0; k < 4; k++) {
			unsigned char t = p[k];
			p[k] = p[7 - k];
			p[7 - k] = t;
		}
		return *(long double *)p;
	}
}

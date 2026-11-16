// MSL's helpers for math.h, as the C library of CodeWarrior for DS 2.0 has them (MSL_Common). This project's own
// decompilation of the game's code, written against that MSL's headers. The game links three of the file's nine
// functions: __msl_generic_count_bits64, __signbitd and __fpclassifyd.
#include <math.h>
#include <math_api.h>

unsigned int __msl_generic_count_bits64(unsigned long long x)
{
	x -= (x >> 1) & 0x5555555555555555ULL;
	x = (x & 0x3333333333333333ULL) + ((x >> 2) & 0x3333333333333333ULL);
	x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0FULL;
	x += x >> 8;
	x += x >> 16;
	x += x >> 32;
	return (unsigned int)x & 0xFF;
}

int __signbitd(double x)
{
	return __HI(x) & 0x80000000;
}

int __fpclassifyd(double x)
{
	switch (__HI(x) & 0x7FF00000) {
	case 0x7FF00000:
		if ((__HI(x) & 0x000FFFFF) || __LO(x))
			return FP_NAN;
		else
			return FP_INFINITE;
	case 0:
		if ((__HI(x) & 0x000FFFFF) || __LO(x))
			return FP_SUBNORMAL;
		else
			return FP_ZERO;
	}
	return FP_NORMAL;
}

// MSL's memory fill, as the C library of CodeWarrior for DS 2.0 has it: memset's worker, the one function of its file
// the game links. This project's own decompilation of the game's code, written against that MSL's headers.
#include <mem_funcs.h>

void __fill_mem(void *dst, int val, unsigned long n)
{
	unsigned char *cpd = (unsigned char *)dst;
	unsigned long *lpd;
	unsigned long v = (unsigned char)val;
	unsigned long i;

	if (n >= 32) {
		i = (-(unsigned long)cpd) & 3;
		if (i) {
			n -= i;
			do
				*cpd++ = (unsigned char)v;
			while (--i);
		}
		if (v)
			v |= v << 24 | v << 16 | v << 8;
		lpd = (unsigned long *)cpd;
		i = n >> 5;
		if (i) {
			do {
				lpd[0] = v;
				lpd[1] = v;
				lpd[2] = v;
				lpd[3] = v;
				lpd[4] = v;
				lpd[5] = v;
				lpd[6] = v;
				lpd[7] = v;
				lpd += 8;
			} while (--i);
		}
		i = (n & 31) >> 2;
		if (i) {
			do
				*lpd++ = v;
			while (--i);
		}
		cpd = (unsigned char *)lpd;
		n &= 3;
	}
	if (n) {
		do
			*cpd++ = (unsigned char)v;
		while (--n);
	}
}

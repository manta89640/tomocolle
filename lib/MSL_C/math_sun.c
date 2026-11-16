// MSL's math functions over fdlibm's, as the C library of CodeWarrior for DS 2.0 has them (MSL_Common_Embedded/Math).
// This project's own decompilation of the game's code, written against that MSL's headers. The game links only
// scalbn.
#include <math.h>

double scalbn(double x, int n)
{
	int e;
	double f = frexp(x, &e);
	e += n;
	return ldexp(f, e);
}

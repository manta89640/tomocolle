// MSL's ARM math functions, as the C library of CodeWarrior for DS 2.0 has them (MSL_ARM). This project's own
// decompilation of the game's code, written against that MSL's headers. The game links only nan.
#include <math.h>

double nan(const char *tagp)
{
	return NAN;
}

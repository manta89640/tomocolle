// MSL's integer arithmetic, as the C library of CodeWarrior for DS 2.0 has it: abs, the one function of the file the
// game links. This project's own decompilation of the game's code, written against that MSL's headers.
//
// abs and labs compile to the same three instructions, and the game links one of them, from the place of arith.o's
// first function: it is taken to be abs, the name it already had.
#include <stdlib.h>

int abs(int n)
{
	if (n < 0)
		return -n;
	return n;
}

// MSL's errno, as the C library of CodeWarrior for DS 2.0 has it: the archive's errno.o holds this one variable and
// no code. This project's own decompilation of the game's data, written against that MSL's headers.
#include <errno.h>

int errno;

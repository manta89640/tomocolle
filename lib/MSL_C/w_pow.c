/* @(#)w_pow.c 1.3 95/01/18 */
/*
 * ====================================================
 * Copyright (C) 1993 by Sun Microsystems, Inc. All rights reserved.
 *
 * Developed at SunSoft, a Sun Microsystems, Inc. business.
 * Permission to use, copy, modify, and distribute this
 * software is freely granted, provided that this notice
 * is preserved.
 * ====================================================
 */

// fdlibm's wrapper of pow, as MSL's C library of CodeWarrior for DS 2.0 has it (MSL_Common_Embedded/Math). Written for
// this project from fdlibm, to the game's code: MSL builds fdlibm as _IEEE_LIBM, so the wrapper only calls
// __ieee754_pow.

/*
 * wrapper pow(x,y) return x**y
 */

#include <fdlibm.h>

double pow(double x, double y)
{
	return __ieee754_pow(x, y);
}

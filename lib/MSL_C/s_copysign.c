/* @(#)s_copysign.c 1.3 95/01/18 */
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

// fdlibm's copysign, as MSL's C library of CodeWarrior for DS 2.0 has it (MSL_Common_Embedded/Math). Written for this
// project from fdlibm, to the game's code: the same as fdlibm's.

/*
 * copysign(double x, double y)
 * copysign(x,y) returns a value with the magnitude of x and
 * with the sign bit of y.
 */

#include <fdlibm.h>

double copysign(double x, double y)
{
	__HIp(&x) = (__HIp(&x)&0x7fffffff)|(__HIp(&y)&0x80000000);
	return x;
}

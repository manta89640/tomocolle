/* @(#)s_fabs.c 1.3 95/01/18 */
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

// fdlibm's fabs, as MSL's C library of CodeWarrior for DS 2.0 has it (MSL_Common_Embedded/Math). Written for this
// project from fdlibm, to the game's code: the same as fdlibm's.

/*
 * fabs(x) returns the absolute value of x.
 */

#include <fdlibm.h>

double fabs(double x)
{
	__HIp(&x) &= 0x7fffffff;
	return x;
}

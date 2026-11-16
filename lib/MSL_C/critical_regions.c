// MSL's critical regions for NITRO, as the C library of CodeWarrior for DS 2.0 has them: the mutexes behind the
// inline functions of critical_regions.NITRO.h, and who holds each and how often. The archive's member is named
// "critical_regions", its name cut to sixteen characters; it has this data and no code. This project's own
// decompilation of the game's data, written against that MSL's headers.
#include <critical_regions.h>

// Declared in this order so that they come out in the original's: __cs_id, __cs_ref, __cs
int __cs_ref[num_critical_regions];
int __cs_id[num_critical_regions];
OSMutex __cs[num_critical_regions];

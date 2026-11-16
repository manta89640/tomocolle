#ifndef OWNERINFO_H
#define OWNERINFO_H

#include <nitro.h>

// The DS's owner, as the system settings have them, read once at start: the owner information, the colour of the
// favourite colour, and the MAC address. main() initialises it (OwnerInfo_Init) and asks it for the time of day to
// seed gRandom.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class OwnerInfo {
public:
    // Empty, and inline: the file's static initialiser is a lone return (0x020B0518)
    OwnerInfo() {}
    void Init();
    void GetTime(int *hour, int *minute, int *second);

    /* 0x00 */ OSOwnerInfo mInfo;
    /* 0x54 */ GXRgb mFavoriteColor;
    /* 0x56 */ u8 mMacAddress[6];
}; // 0x5C

extern OwnerInfo gOwnerInfo;

#endif // OWNERINFO_H

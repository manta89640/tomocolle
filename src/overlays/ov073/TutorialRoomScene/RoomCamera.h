#ifndef OV073_ROOMCAMERA_H
#define OV073_ROOMCAMERA_H

#include <nitro.h>
#include <nnsys.h>

#include "Room.h"
#include "RoomCharacter.h"

// The camera of the apartment room as the tutorial (overlay 73) has it: overlay 74's `RoomCamera` without its
// first flag and without its timers, 0x5C bytes instead of 0x64 (the scene's constructor puts the next member 0x5C
// after it). Its functions are group b2's; this is the part of it that the scene's functions use, with overlay
// 74's names for the members.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class RoomCamera {
public:
    enum Mode {
        MODE_NONE,
        MODE_ROOM,      // Look at the middle of the room
        MODE_CHARACTER, // Look at the character that is present
        MODE_BOTH,      // Look at the middle between both characters
    };

    RoomCamera();           // 0x0214e41c
    void Reset();           // 0x0214e48c
    void SetMode(int mode); // 0x0214e57c
    void Update();          // 0x0214e590

    /* 0x00 */ u8 mUnknown00[0x24];
    /* 0x24 */ VecFx32 mOrbit;      // x: elevation, y: direction, as angle indices times 4096
    /* 0x30 */ VecFx32 mOrbitSpeed;
    /* 0x3C */ VecFx32 mInput;      // Added to mOrbitSpeed each frame: the scene sets it from the + Control Pad
    /* 0x48 */ BOOL mIgnoreInput;
    /* 0x4C */ RoomCharacter *mCharacters[2];
    /* 0x54 */ ApartmentRoom *mRoom;
    /* 0x58 */ u8 mUnknown58[4];
}; // 0x5C

#endif // OV073_ROOMCAMERA_H

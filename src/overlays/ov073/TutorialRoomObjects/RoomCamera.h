// Before the guard, so that RoomCharacter is whole here whichever of the two headers is included first:
// RoomCharacter.h includes this one at its end.
#include "RoomCharacter.h"

#ifndef OV073_ROOMCAMERA_H
#define OV073_ROOMCAMERA_H

#include <nitro.h>
#include <nnsys.h>

#include "FloorPos.h"
#include "Room.h"

// The main program's camera (data_02122764; func_0202a640 resets it, func_0202a748 turns its position around
// its target by its angles). Not decompiled: a stand-in with the fields the room camera uses.
struct Camera {
    void SetAngles(u16 x, u16 y, u16 z) {
        mAngleX = x;
        mAngleY = y;
        mAngleZ = z;
    }

    /* 0x00 */ u8 mUnknown00[0x14];
    /* 0x14 */ VecFx32 mPosition;
    /* 0x20 */ VecFx32 mUnknown20;
    /* 0x2C */ VecFx32 mTarget;
    /* 0x38 */ u8 mUnknown38[0xC];
    /* 0x44 */ u16 mAngleX;
    /* 0x46 */ u16 mAngleY;
    /* 0x48 */ u16 mAngleZ;
};

// What the camera's destructor leaves for the next scene (data_0213ba74)
struct SavedCamera {
    void SetOrbit(s16 x, s16 y, s16 z) {
        mOrbitX = x;
        mOrbitY = y;
        mOrbitZ = z;
    }

    /* 0x00 */ VecFx32 mPosition;
    /* 0x0C */ VecFx32 mTarget;
    /* 0x18 */ s16 mOrbitX;
    /* 0x1A */ s16 mOrbitY;
    /* 0x1C */ s16 mOrbitZ;
};

extern "C" {
extern Camera data_02122764;
extern SavedCamera data_0213ba74;
void func_0202a640(Camera *camera);
// The absolute value of each component
void func_02028e0c(const VecFx32 *in, VecFx32 *out);
}

// The camera of the apartment room as the tutorial (overlay 73) has it: it circles the room, a character or the
// middle between both, and can be turned by hand. It is overlay 74's `RoomCamera` without the state that turns it
// to a new random angle now and then: no mState (so every member from mMode on is four bytes earlier), no
// mUnknown60 and mWaitFrames at the end, no SetState and no MODE_CHARACTER_CLOSE. 0x5C bytes instead of 0x64.
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

    RoomCamera();
    ~RoomCamera();
    void Reset();
    void SetMode(int mode);
    void Update();
    void UpdateOrbit();
    void ApplyInput(fx32 *speed, fx32 input, fx32 friction, fx32 max);
    VecFx32 GetOrbitPosition(VecFx32 radius, VecFx32 center);
    VecFx32 Approach(VecFx32 to, VecFx32 from, int shift);
    void Clamp(fx32 *value, fx32 limit);

    // Each component shifted right
    static VecFx32 Shift(const VecFx32 &v, u32 shift) {
        VecFx32 result = { v.x >> shift, v.y >> shift, v.z >> shift };
        return result;
    }
    void ApplyToCamera() {
        mCamera->mPosition = mPosition;
        mCamera->mTarget = mTarget;
    }
    // Where the camera is, on the floor: the characters turn to it
    FloorPos GetFloorPos() {
        FloorPos pos;
        pos.x = mPosition.x;
        pos.z = mPosition.z;
        return pos;
    }
    RoomCharacter *GetPresentCharacter() {
        RoomCharacter *character = mCharacters[0];
        if (!character->mPresent) {
            character = mCharacters[1];
        }
        return character;
    }

    /* 0x00 */ Camera *mCamera;
    /* 0x04 */ int mMode;
    /* 0x08 */ BOOL mSnap;          // Go to the wanted position at once instead of approaching it
    /* 0x0C */ VecFx32 mPosition;
    /* 0x18 */ VecFx32 mTarget;
    /* 0x24 */ VecFx32 mOrbit;      // x: elevation, y: direction, as angle indices times 4096; z unused
    /* 0x30 */ VecFx32 mOrbitSpeed;
    /* 0x3C */ VecFx32 mInput;      // Added to mOrbitSpeed each frame, then cleared
    /* 0x48 */ BOOL mIgnoreInput;
    /* 0x4C */ RoomCharacter *mCharacters[2];
    /* 0x54 */ ApartmentRoom *mRoom;
    /* 0x58 */ s16 mUnknown58;      // Only cleared, by SetMode (overlay 74's mUnknown5C)
    /* 0x5A */ s16 mFollowFrames;   // Frames since the last snap, up to 80: it follows faster after that
}; // 0x5C

#endif // OV073_ROOMCAMERA_H

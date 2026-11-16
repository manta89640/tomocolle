// Before the guard, so that RoomCharacter is whole here whichever of the two headers is included first:
// RoomCharacter.h includes this one at its end, for its inline functions that look at the camera.
#include "RoomCharacter.h"

#ifndef OV074_ROOMCAMERA_H
#define OV074_ROOMCAMERA_H

#include <nitro.h>
#include <nnsys.h>

#include "character/FloorPos.h"
#include "Room.h"

// Shared by the units of overlay 74 (it began as RoomObjects' RoomCamera.h).

// The main program's camera (data_02122764; func_0202a640 resets it, func_0202a748 turns its position around
// its target by its angles). Not decompiled: a stand-in with the fields the room camera uses.
struct Camera; // The main program's camera (MainProgram.h)

class RoomCamera {
public:
    enum State {
        STATE_NONE,
        STATE_TURN_RIGHT, // Pick a new angle to the right of this one
        STATE_TURN_LEFT,
        STATE_TURNED,     // The frame after picking
        STATE_WAIT,       // Count mWaitFrames down, then turn again
    };

    enum Mode {
        MODE_NONE,
        MODE_ROOM,       // Look at the middle of the room
        MODE_CHARACTER,  // Look at the character that is present
        MODE_BOTH,       // Look at the middle between both characters
        MODE_CHARACTER_CLOSE,
    };

    RoomCamera();
    ~RoomCamera();
    void Reset();
    void SetMode(int mode);
    void SetState(int state, s16 waitFrames);
    void Update();
    void UpdateOrbit();
    void ApplyInput(fx32 *speed, fx32 input, fx32 friction, fx32 max);
    VecFx32 GetOrbitPosition(VecFx32 radius, VecFx32 center);
    VecFx32 Approach(VecFx32 to, VecFx32 from, int shift);
    void Clamp(fx32 *value, fx32 limit);

    // Each component shifted right
    static VecFx32 MakeVec(fx32 x, fx32 y, fx32 z) {
        VecFx32 v;
        v.x = x;
        v.y = y;
        v.z = z;
        return v;
    }
    static VecFx32 Shift(const VecFx32 &v, u32 shift) {
        VecFx32 result = { v.x >> shift, v.y >> shift, v.z >> shift };
        return result;
    }
    void ApplyToCamera(); // Inline, in MainProgram.h (where the main program's Camera is)
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
    /* 0x04 */ int mState;
    /* 0x08 */ int mMode;
    /* 0x0C */ BOOL mSnap;          // Go to the wanted position at once instead of approaching it
    /* 0x10 */ VecFx32 mPosition;
    /* 0x1C */ VecFx32 mTarget;
    /* 0x28 */ VecFx32 mOrbit;      // x: elevation, y: direction, as angle indices times 4096; z unused
    /* 0x34 */ VecFx32 mOrbitSpeed;
    /* 0x40 */ VecFx32 mInput;      // Added to mOrbitSpeed each frame, then cleared
    /* 0x4C */ BOOL mIgnoreInput;
    /* 0x50 */ RoomCharacter *mCharacters[2];
    /* 0x58 */ ApartmentRoom *mRoom;
    /* 0x5C */ s16 mUnknown5C;
    /* 0x5E */ s16 mFollowFrames;   // Frames since the last snap, up to 80: it follows faster after that
    /* 0x60 */ s16 mUnknown60;
    /* 0x62 */ s16 mWaitFrames;
}; // 0x64

#endif // OV074_ROOMCAMERA_H

#ifndef OV073_TUTORIALROOMCAMERA_H
#define OV073_TUTORIALROOMCAMERA_H

#include <nitro.h>
#include <nnsys.h>

#include "FloorPos.h"
#include "MainProgram.h"
#include "Room.h"
#include "TutorialRoomCharacter.h"

// The camera of the tutorial's apartment room: it circles the room, a character or the middle between both, and
// can be turned by hand. It is overlay 74's `RoomCamera` without the state that turns it to a new random angle now
// and then: no mState (so every member from mMode on is four bytes earlier), no mUnknown60 and mWaitFrames at the
// end, no SetState and no MODE_CHARACTER_CLOSE. 0x5C bytes instead of 0x64 (the scene's constructor puts the next
// member 0x5C after it).
//
// The name is descriptive, not the original: no name of this class survives in the ROM. The prefix tells it from
// overlay 74's: the program is linked as one, so the two cannot have one name.
class TutorialRoomCamera {
public:
    enum Mode {
        MODE_NONE,
        MODE_ROOM,      // Look at the middle of the room
        MODE_CHARACTER, // Look at the character that is present
        MODE_BOTH,      // Look at the middle between both characters
    };

    TutorialRoomCamera();
    ~TutorialRoomCamera();
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
    TutorialRoomCharacter *GetPresentCharacter() {
        TutorialRoomCharacter *character = mCharacters[0];
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
    /* 0x3C */ VecFx32 mInput;      // Added to mOrbitSpeed each frame, then cleared: the scene sets it from the
                                    // + Control Pad
    /* 0x48 */ BOOL mIgnoreInput;
    /* 0x4C */ TutorialRoomCharacter *mCharacters[2];
    /* 0x54 */ TutorialApartmentRoom *mRoom;
    /* 0x58 */ s16 mUnknown58;      // Only cleared, by SetMode (overlay 74's mUnknown5C)
    /* 0x5A */ s16 mFollowFrames;   // Frames since the last snap, up to 80: it follows faster after that
}; // 0x5C

// The character's helpers that need the camera.

// The head's yaw towards the camera
inline u16 TutorialRoomCharacter::GetHeadYawToCamera() {
    FloorPos pos;
    pos.x = mCamera->mPosition.x;
    pos.z = mCamera->mPosition.z;
    return GetHeadYawTo(pos, 0x3800);
}

// Turns the head to look into the camera
inline void TutorialRoomCharacter::TurnHeadToCamera(u8 frames) {
    FloorPos pos;
    pos.x = mCamera->mPosition.x;
    pos.z = mCamera->mPosition.z;
    u16 angle = mCamera->mOrbit.x >> 12;
    SetHeadTurn(-(angle >> 2), GetHeadYawTo(pos, 0x3800), 0, frames);
}

#endif // OV073_TUTORIALROOMCAMERA_H

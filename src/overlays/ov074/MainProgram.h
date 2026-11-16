#ifndef OV074_MAINPROGRAM_H
#define OV074_MAINPROGRAM_H

// What overlay 74 uses of the main program that has no shared header yet, private to the overlay: only
// RoomScene.cpp includes it (the scene factory, src/scene/SceneManager.cpp, includes RoomScene.h).

#include <nitro.h>
#include <nnsys.h>

#include "SceneMemory.h"
#include "RoomScene.h"

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

// The scene memory's camera angles (what the camera's destructor leaves for the next scene)
static inline void SetMemoryOrbit(SceneMemory *memory, s16 x, s16 y, s16 z) {
    memory->mOrbitX = x;
    memory->mOrbitY = y;
    memory->mOrbitZ = z;
}

extern "C" {
extern Camera gCamera;
void Camera_Reset(Camera *camera);
void Camera_UpdateEye(Camera *camera);
// The absolute value of each component
void AbsVec(const VecFx32 *in, VecFx32 *out);
}

// The camera of the apartment room: it circles the room, a character or the middle between both, turning to a
// new random angle now and then, and can be turned by hand.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
// One of the main program's 3D lights (gLights is an array of them; Light_Reset resets one).
struct Light {
    /* 0x0 */ VecFx16 mDirection;
    /* 0x6 */ u16 mColor;
    /* 0x8 */ BOOL mEnabled;
}; // 0xC

extern "C" {

// --- The head icon (0x02080f64 to 0x020810e4) ---
int func_02081024(HeadIcon *icon, HeadIconFrame *frame); // frame ? frame->mValue : 0; does not use the icon

} // extern "C"

inline void RoomCamera::ApplyToCamera() {
    mCamera->mPosition = mPosition;
    mCamera->mTarget = mTarget;
}

#endif // OV074_MAINPROGRAM_H

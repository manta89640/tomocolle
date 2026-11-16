#ifndef SCENEMEMORY_H
#define SCENEMEMORY_H

#include <nitro.h>

// What the scenes leave for each other (gSceneMemory): the camera as a room scene left it, how far the tutorial
// is, and the characters the next scene shows. Its constructor (0x0204D824) is run by the static initialiser of
// src/scene/EmptyScene.cpp, the file that defines it. The overlays' sources have stand-ins of it with the fields
// each uses.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
struct SceneMemory {
    // Not inlined: the static initialiser calls it
    SceneMemory() __attribute__((never_inline)) {
        mCameraPosition.x = 0;
        mCameraPosition.y = 0;
        mCameraPosition.z = 0;
        mCameraTarget.x = 0;
        mCameraTarget.y = 0;
        mCameraTarget.z = 0;
        mOrbitX = 0;
        mOrbitY = 0;
        mOrbitZ = 0;
        mTutorialStep = 0;
        MI_CpuFill8(mCharacters, 0xFF, sizeof(mCharacters));
        MI_CpuFill8(mUnknown2C, 0xFF, sizeof(mUnknown2C));
    }

    /* 0x00 */ VecFx32 mCameraPosition;
    /* 0x0C */ VecFx32 mCameraTarget;
    /* 0x18 */ s16 mOrbitX; // The room camera's angles
    /* 0x1A */ s16 mOrbitY;
    /* 0x1C */ s16 mOrbitZ;
    /* 0x1E */ u8 mTutorialStep;
    /* 0x1F */ u8 mUnknown1F;
    /* 0x20 */ u8 mCharacters[12]; // The characters (saved character indices) the next scene shows, 0xFF for none
    /* 0x2C */ u8 mUnknown2C[16];  // 0xFF at the start
}; // 0x3C

extern SceneMemory gSceneMemory;

#endif // SCENEMEMORY_H

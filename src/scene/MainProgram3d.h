#ifndef SCENE_MAINPROGRAM3D_H
#define SCENE_MAINPROGRAM3D_H

#include "MainProgram.h"

// STAND-INS for what the main program's test scenes (DrawManTestScene.cpp to WirelessTestScene.cpp) and
// SceneHelpers.cpp use of the rest of the main program: the models, the characters, the camera, the lights and the
// pad. Most are overlay 96's and G3SpriteTestScene.h's stand-ins under the same names, with the fields these files
// touch; they give way to the shared definitions in include/ as those are written.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

#include "Model.h"
#include "character/Character.h"

#define RESOURCE_BTX0 0x30585442 // Textures

extern "C" {
// The models' resource table (gModelResources, include/Model.h)
void ModelResources_SetModel(void *resources, int resourceId, const char *modelPath); // Binds a model file to an id
void ModelResources_BeginDraw(void *resources);
// Binds the resources of a type (textures, ...) of the model's resource id to the model
void Model_BindResource(Model *model, u32 resType, int resourceId, int unk);
void ResourceSet_SetArchive(void *resourceSet, u32 resType, void *archive); // Gives a resource set an archive's animations
NNSG3dResFileHeader *func_020295a4(void *resourceSet, u32 resType); // The file of a type in a resource set
void func_02029758(void *resourceSet, u32 resType);
void NNS_GfdDumpFrmPlttVramManager(void);
// Queues a transfer of palette colours to the model's palette in VRAM (a NNS_GFD_DST_3D_TEX_PLTT task): size bytes
// at offset bytes into it, each taken from the palette's VRAM key when negative
void Model_QueuePalette(Model *model, const void *pltt, int offset, int size);
void NNS_GfdDumpLnkPlttVramManager(void);
}

// --- Characters: include/character/Character.h ---

// --- The camera and the lights ---

struct Camera {
    /* 0x00 */ u8 mUnknown00[0x14];
    /* 0x14 */ VecFx32 mPosition;
    /* 0x20 */ VecFx32 mEye;
    /* 0x2C */ VecFx32 mTarget;
    /* 0x38 */ u8 mUnknown38[0xC];
    /* 0x44 */ u16 mAngleX;
    /* 0x46 */ u16 mAngleY;
    /* 0x48 */ u16 mAngleZ;
};

// One of the main program's 3D lights (gLights is an array of them; Light_Reset resets one).
struct Light {
    /* 0x0 */ VecFx16 mDirection;
    /* 0x6 */ u16 mColor;
    /* 0x8 */ BOOL mEnabled;
}; // 0xC

extern "C" {
extern Camera gCamera;
extern Light gLights[];
}

// --- Input ---

// The pad: what was pressed this frame, and what is held
struct PadInput {
    /* 0x00 */ u32 mTrigger;
    /* 0x04 */ u32 mHeld;
    /* 0x08 */ u8 mUnknown08[0x18];
    /* 0x20 */ BOOL mUnknown20;
};

// The touch screen
struct TouchInput {
    /* 0x00 */ u8 mUnknown00[0x28];
    /* 0x28 */ TPData mTp;             // As the SDK reads it; mTp.touch is set while the screen is touched
    /* 0x30 */ ScreenPoint mLastPoint; // Where it was touched last
    /* 0x38 */ ScreenPoint mPoint;     // Where it is touched
    /* 0x40 */ u8 mUnknown40[8];
    /* 0x48 */ BOOL mTouched;          // The stylus went down this frame
    /* 0x4C */ BOOL mUnknown4C;
};

extern "C" {
extern PadInput gPadInput;
extern TouchInput gTouchInput;
}

#endif // SCENE_MAINPROGRAM3D_H

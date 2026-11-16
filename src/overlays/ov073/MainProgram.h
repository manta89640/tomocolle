#ifndef OV073_MAINPROGRAM_H
#define OV073_MAINPROGRAM_H

#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "Sprite.h"
#include "island/Sky.h"
#include "SceneManager.h"
#include "SceneMemory.h"
#include "ScreenControl.h"
#include "TutorialRoomScene.h"

// STAND-INS and declarations for what the tutorial's apartment uses of the main program that is not decompiled
// yet: its global objects with the fields the overlay touches, and its functions by the names symbols.txt has for
// them. Most are the same as overlay 74's (src/overlays/ov074/RoomScene/prelude.h).

// The archive loader (gArchiveManager): archives are asked for by name and path, and load on a thread.
struct ArchiveLoader;

// The screens: which planes are shown, and their brightness.
struct PadInput {
    /* 0x00 */ u32 mUnknown00;
    /* 0x04 */ u32 mHeld;
};

// The touch screen.
struct TouchInput {
    /* 0x00 */ u8 mUnknown00[0x2C];
    /* 0x2C */ u16 mTouching;
    /* 0x2E */ u8 mUnknown2E[2];
    /* 0x30 */ ScreenPoint mLastPoint; // Where it was touched last
    /* 0x38 */ ScreenPoint mPoint;     // Where it is touched
    /* 0x40 */ u8 mUnknown40[8];
    /* 0x48 */ BOOL mTouched;          // The stylus went down this frame
};

// Graphics resources, loaded by file name into VRAM.
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;
struct Renderer3d;

// The scene manager.
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

// One of the main program's 3D lights (data_021227cc is an array of them; func_0202a96c resets one).
struct Light {
    /* 0x0 */ VecFx16 mDirection;
    /* 0x6 */ u16 mColor;
    /* 0x8 */ BOOL mEnabled;
}; // 0xC

struct Unknown02114d78Entry {
    u32 mUnknown0 : 8;
    int mUnknown4;
    int mUnknown8;
};

extern "C" {

// --- Archives ---
extern ArchiveLoader gArchiveManager;
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void *ArchiveManager_GetFileByName(ArchiveLoader *loader, const char *path);
BOOL ArchiveManager_IsIdle(ArchiveLoader *loader); // Whether no archive is being loaded

// --- The screens ---
void ScreenControl_ClearBlend(ScreenControl *screens, int mask);
void ScreenControl_SetBlendAlpha(ScreenControl *screens, int a, int b, int c, int d);
// Something of the planes in the mask that the scene sets to -10 before a part and to 0 when the message window
// comes (a brightness, by the values)
void ScreenControl_SetBlendBrightness(ScreenControl *screens, int mask, int value);
// The offset (the plane's number shifted by 11) of a plane's screen in VRAM
int func_020253d4(u32 *graphicsManager, int plane); // (gGraphicsManager is declared in text/TextCanvas.h)

// --- Input ---
extern PadInput gPadInput;
extern TouchInput gTouchInput;

// --- The game's random number generator: every use is the inline code of the SDK's MATH_Rand16 ---
extern MATHRandContext16 gRandom;

// --- Graphics resources ---
extern CellLoader gObjResourceManager;
extern PaletteLoader gPaletteLoader;
extern ScreenLoader gScreenLoader;
extern CharLoader gVramTransferQueue;
extern Renderer3d data_02122824;
void func_0202b0e8(Renderer3d *renderer);
void ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
// The same without the characters. It returns something (what ObjResourceManager_Load leaves in r0) that the
// scene's Init does not use: declared void, Init's instructions come out in another order.
int func_02027290(CellLoader *loader, const char *cellName, int plane, int a);
void *PaletteLoader_LoadFile(PaletteLoader *loader, const char *name, int plane, int a, int b, int c); // Loads a palette
void PaletteLoader_SetBackdrop(PaletteLoader *loader, u16 color, int unknown); // Sets colours of a loaded palette
u16 *ScreenLoader_LoadFile(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void ScreenLoader_SetPriority(ScreenLoader *loader, int plane, int priority);
// Sets the scroll of the planes of the mask
void ScreenLoader_SetScrollY(ScreenLoader *loader, int planes, int y);
// Queues a character file for transfer to VRAM
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void CreateMainFrameHeap(int size);
void DestroyMainFrameHeap();
// Splits a 15-bit colour
void func_02028b98(int color, int *r, int *g, int *b);
// The absolute value of each component
void func_02028e0c(const VecFx32 *in, VecFx32 *out);

// --- 3D ---
extern Camera data_02122764;
void func_0202a640(Camera *camera);
extern Light data_021227cc[];
// Resets a light: direction straight down, colour white, enabled
void func_0202a96c(Light *light);
// Sets a light's direction: the vector, rotated by the three angles and normalized
void func_0202ab2c(Light *light, u16 angleX, u16 angleY, u16 angleZ, fx32 x, fx32 y, fx32 z);

// --- The saved game ---
BOOL func_0202c3dc(GameData *game, int index); // Whether the saved character with that index exists
int func_0202c414(GameData *game, int index);  // Its room number (SavedCharacter::mRoom)
struct SavedCharacter;
void func_0202c264(GameData *game, int index, SavedCharacter *character); // Reads one
void func_0202c28c(GameData *game, int index, SavedCharacter *character); // Writes one
// What is known of a character by its index
int func_0202c468(GameData *game, int character);
u8 func_0202c53c(GameData *game, int character);
int func_0202c5b4(GameData *game, int character);
int func_0202c58c(GameData *game, int character);
extern const Unknown02114d78Entry *const data_02114d78[];

// --- The clock ---
// The current time with the game's offset applied, as seconds and as date and time
s64 func_02035870(int offset, RTCDate *date, RTCTime *time);
extern int data_02123440[];

// --- Scenes ---

} // extern "C"

extern "C" {


// Copies the saved character's name out as a string
void MiiData_GetName(SavedCharacter *character, wchar_t *name);
// For each kind of relation, how much each reaction changes the affinity by, halved
extern const s8 *data_021135c8[];
// Binds the joint animations of an archive to a model's resource
void func_02029660(void *resource, u32 resType, void *archive);

// --- The particle system and the head icon (0x02080e80 to 0x020810e4) ---
void func_02080e98(ParticleSystem *particles, void *file, int a, int b, int c, int d, int e);
void func_02080ee4(ParticleSystem *particles);
void func_02080f00(ParticleSystem *particles, int unknown); // Draws
void func_0207b138(void *manager);                          // Updates
int func_02081024(HeadIcon *icon, HeadIconFrame *frame); // frame ? frame->mValue : 0; does not use the icon

} // extern "C"

// The screens: hides planes (as ScreenControl's Set and Show)
static inline void HideScreens(ScreenControl *screens, u32 planes) {
    screens->mPlanes &= ~planes;
    screens->mPlanesChanged = TRUE;
}

inline void TutorialRoomCamera::ApplyToCamera() {
    mCamera->mPosition = mPosition;
    mCamera->mTarget = mTarget;
}

// A joint's matrix of a character's body (as the stand-in CharacterModel had it, an inline)
static inline MtxFx43 *GetJointMatrix(CharacterModel &model, int joint) { return &model.mJointMatrices[joint]; }

// The scene memory's camera angles
static inline void SetMemoryOrbit(SceneMemory *memory, s16 x, s16 y, s16 z) {
    memory->mOrbitX = x;
    memory->mOrbitY = y;
    memory->mOrbitZ = z;
}

#endif // OV073_MAINPROGRAM_H

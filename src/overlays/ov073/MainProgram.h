#ifndef OV073_MAINPROGRAM_H
#define OV073_MAINPROGRAM_H

#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "Sprite.h"

// STAND-INS and declarations for what the tutorial's apartment uses of the main program that is not decompiled
// yet: its global objects with the fields the overlay touches, and its functions by the names symbols.txt has for
// them. Most are the same as overlay 74's (src/overlays/ov074/RoomScene/prelude.h).

// The archive loader (gArchiveManager): archives are asked for by name and path, and load on a thread.
struct ArchiveLoader;

// The screens: which planes are shown, and their brightness.
struct ScreenControl {
    void Set(u32 planes) {
        mPlanes = planes;
        mPlanesChanged = TRUE;
    }
    void Show(u32 planes) {
        mPlanes |= planes;
        mPlanesChanged = TRUE;
    }
    void Hide(u32 planes) {
        mPlanes &= ~planes;
        mPlanesChanged = TRUE;
    }

    /* 0x00 */ u32 mPlanes; // Of the main screen; bit 1 is set while a TutorialTimedMessage is shown
    /* 0x04 */ u8 mUnknown04[0x44];
    /* 0x48 */ int mUnknown48;
    /* 0x4C */ u8 mUnknown4C[0x88];
    /* 0xD4 */ BOOL mPlanesChanged;
    /* 0xD8 */ u8 mUnknownD8[0x14];
    /* 0xEC */ int mUnknownEC;
};

// The buttons.
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
class Scene;
struct SceneManager {
    /* 0x0 */ Scene *mScene;
    /* 0x4 */ int mNextSceneId;
    /* 0x8 */ int mPreviousSceneId;
};

// What the scenes leave for each other (data_0213ba74): the camera as the last room camera left it, and how far
// the tutorial is.
struct SceneMemory {
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
    /* 0x1E */ u8 mTutorialStep; // The tutorial's scene copies it into its mTutorialStep
};

// The main program's camera (data_02122764; func_0202a640 resets it, func_0202a748 turns its position around
// its target by its angles). A stand-in with the fields the room camera uses.
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

// The main program's time-of-day tracker (func_020a612c sets it up from a date and time, func_020a61cc updates
// it about every two minutes: it reloads its counter with 0x1C42 frames). A stand-in with the fields the sky uses.
struct DayPhase {
    /* 0x00 */ u32 mChanged; // Not 0 in the frame the phase changed
    /* 0x04 */ u8 mUnknown04[0x10];
    /* 0x14 */ int mFramesToCheck;
    /* 0x18 */ int mPhase;       // func_020a6214 returns it, plus 16 when mUnknown1C is 0
    /* 0x1C */ int mUnknown1C;
    /* 0x20 */ u8 mUnknown20[8];
}; // 0x28

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
extern ScreenControl data_0211d604;
void func_02023eac(ScreenControl *screens, int mask);
void func_02023ee0(ScreenControl *screens, int a, int b, int c, int d);
// Something of the planes in the mask that the scene sets to -10 before a part and to 0 when the message window
// comes (a brightness, by the values)
void func_02023f4c(ScreenControl *screens, int mask, int value);
// The offset (the plane's number shifted by 11) of a plane's screen in VRAM
int func_020253d4(u32 *graphicsManager, int plane); // (gGraphicsManager is declared in text/TextCanvas.h)

// --- Input ---
extern PadInput data_0211f9d4;
extern TouchInput gTouchInput;

// --- The game's random number generator: every use is the inline code of the SDK's MATH_Rand16 ---
extern MATHRandContext16 gRandom;

// --- Graphics resources ---
extern CellLoader gObjResourceManager;
extern PaletteLoader data_021213b8;
extern ScreenLoader data_02121834;
extern CharLoader gVramTransferQueue;
extern Renderer3d data_02122824;
void func_0202b0e8(Renderer3d *renderer);
void ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
// The same without the characters. It returns something (what ObjResourceManager_Load leaves in r0) that the
// scene's Init does not use: declared void, Init's instructions come out in another order.
int func_02027290(CellLoader *loader, const char *cellName, int plane, int a);
void *func_02027534(PaletteLoader *loader, const char *name, int plane, int a, int b, int c); // Loads a palette
void func_020278f8(PaletteLoader *loader, u16 color, int unknown); // Sets colours of a loaded palette
u16 *func_02027d7c(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028280(ScreenLoader *loader, int plane, int priority);
// Sets the scroll of the planes of the mask
void func_02028154(ScreenLoader *loader, int planes, int y);
// Queues a character file for transfer to VRAM
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void func_0202680c(int size);
void func_0202683c();
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
BOOL func_02099c94(int unknown);
extern const Unknown02114d78Entry *const data_02114d78[];

// --- The clock ---
// The current time with the game's offset applied, as seconds and as date and time
s64 func_02035870(int offset, RTCDate *date, RTCTime *time);
extern int data_02123440[];
void func_020a612c(DayPhase *phase, RTCDate *date, RTCTime *time);
void func_020a61cc(DayPhase *phase, RTCDate *date, RTCTime *time);
int func_020a6214(DayPhase *phase);

// --- Scenes ---
extern SceneManager gSceneManager;
void func_0203c02c(SceneManager *manager, int sceneId); // Asks for the next scene
extern SceneMemory data_0213ba74;

} // extern "C"

#endif // OV073_MAINPROGRAM_H

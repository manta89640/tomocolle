#ifndef OV110_MAINPROGRAM_H
#define OV110_MAINPROGRAM_H

// What overlay 110 uses of the main program that has no shared header yet, private to the overlay: only
// ItemTradeScene.cpp includes it (the scene factory, src/scene/SceneManager.cpp, includes ItemTradeScene.h).

#include "ItemTradeScene.h"

extern "C" {
void ModelResources_SetModel(void *resources, int resourceId, const char *modelPath);    // Binds a model file to an id
// The id of an animation of a resource set by its name
int ResourceSet_FindIndexByName(void *resourceSet, u32 resType, const char *name);
// Gives a resource set the animations of an archive
void func_02029660(void *resourceSet, u32 resType, void *archive);
}

extern "C" {
// The personalities' kinds (0 to 3), by personality
extern const int gPersonalityKinds[];
int MiiData_GetFavoriteColor(SavedCharacter *character);
int MiiData_GetSex(SavedCharacter *character);
GXRgb NFL_GetFavoriteColor(int index); // lib/NFL
}

// Plays the mannequin's body animation of that name (an inline member of the stand-in Mannequin)
static inline void SetMannequinAnimation(Mannequin *mannequin, const char *name) {
    mannequin->SetAnimation(RESOURCE_BCA0,
                            ResourceSet_FindIndexByName((void *)mannequin->mAnim.mUnknown04, RESOURCE_BCA0, name));
}

// --- The main program's touch buttons (include/SpriteButton.h) ------------------------------------------------

extern "C" {
void func_02031e88(SpriteButton *button, BOOL disabled); // Sets mUnknownB8, and draws the button darkened while it is set
void func_02032134(TextButton *button, int palette);     // Sets the overwrite palette of the button and its text
}

// --- Other main-program things --------------------------------------------------------------------------------

// The main program's camera
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

// One of the main program's 3D lights
struct Light {
    /* 0x0 */ u8 mUnknown0[6];
    /* 0x6 */ u16 mColor;
    /* 0x8 */ u8 mUnknown8[4];
}; // 0xC

// What holds the lights (data_021226e4): this scene sets their colours through it
struct Lighting {
    /* 0x00 */ u8 mUnknown00[0xE8];
    /* 0xE8 */ Light mLights[2];
};

struct ArchiveLoader;
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;
struct Renderer3d;

extern "C" {
extern Camera data_02122764;
extern Light data_021227cc[];
extern Light data_021227d8[];
extern Lighting data_021226e4;
extern ArchiveLoader gArchiveManager;
extern CellLoader gObjResourceManager;
extern PaletteLoader gPaletteLoader;
extern ScreenLoader data_02121834;
extern CharLoader gVramTransferQueue;
extern Renderer3d data_02122824;
extern const VecFx32 gMannequinHeadScale;
extern const VecFx32 gMannequinBodyScale;

void func_0202a640(Camera *camera);
void func_0202a748(Camera *camera);
void func_0202a96c(Light *light);
void func_0202ab2c(Light *light, u16 angleX, u16 angleY, u16 angleZ, fx32 x, fx32 y, fx32 z);
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
BOOL ArchiveManager_IsIdle(ArchiveLoader *loader);
void func_0202b0e8(Renderer3d *renderer);
int func_02027290(CellLoader *loader, const char *cellName, int plane, int a);
void *PaletteLoader_LoadFile(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
void PaletteLoader_SetBackdrop(PaletteLoader *loader, u16 color, int unknown);
void *func_02027d7c(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028280(ScreenLoader *loader, int plane, int priority);
void func_02028108(ScreenLoader *loader, int plane, int x);
void func_02028154(ScreenLoader *loader, int plane, int y);
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void func_02023ee0(ScreenControl *screens, int a, int b, int c, int d);

// --- The saved game ---
void func_0202c264(GameData *game, int index, SavedCharacter *character); // Reads one
BOOL func_0202cd48(GameData *game); // Starts writing the save data; FALSE when it cannot
BOOL func_0202cd38(GameData *game); // Not 0 while the save data is being written
}

// --- The message window's functions this scene calls besides MessageWindow.h's ---
extern "C" {
// Adds a button: (window, number, unknown, sequence, sequence when on, position, size, its sounds, unknown)
void func_02033448(MessageWindow *window, int button, int unk1, u8 sequence, u8 onSequence, WindowPoint position,
                   WindowPoint size, const int *sounds, int unk2);
// Adds a button with a text: as func_02033448, with the text's message file and message, font, plane, and
// character offset
void func_02033844(MessageWindow *window, int button, int unk1, u8 sequence, u8 onSequence, WindowPoint position,
                   WindowPoint size, MessageFile *file, u32 message, const NNSG2dFont *font, int plane,
                   int *charOffset, const int *sounds, int unk2);
// Adds the window's message text: (window, file, message, font, plane, area, position, character offset)
void func_020338d0(MessageWindow *window, MessageFile *file, u32 message, const NNSG2dFont *font, int plane,
                   WindowPoint area, WindowPoint position, int *charOffset);
}

// --- Wireless (main program) ------------------------------------------------------------------------------------

// WH, the SDK's wireless sample code, compiled into the main program (lib/WH; its header is not on the game's
// include path, so what this scene uses is declared again here)
extern "C" {
// The SSID of the child that connected (WH_StateOutStartParent copies it in); this scene shows it as the
// partner's name
extern u16 gWHChildSsid[WM_SIZE_CHILD_SSID / sizeof(u16)];

// WH's states (WH_SYSSTATE_*)
enum {
    WH_SYSSTATE_STOP,
    WH_SYSSTATE_IDLE,
    WH_SYSSTATE_SCANNING,
    WH_SYSSTATE_BUSY,
    WH_SYSSTATE_CONNECTED,
    WH_SYSSTATE_DATASHARING,
    WH_SYSSTATE_KEYSHARING,
    WH_SYSSTATE_MEASURECHANNEL,
    WH_SYSSTATE_CONNECT_FAIL,
    WH_SYSSTATE_ERROR,
    WH_SYSSTATE_FATAL,
};
#define WH_BITMAP_EMPTY 1

typedef void (*WHReceiverFunc)(u16 aid, u16 *data, u16 size);
typedef void (*WHSendCallbackFunc)(BOOL result);

int WH_GetLinkLevel(void);
u16 WH_GetBitmap(void);
int WH_GetSystemState(void);
u16 WH_GetMeasureChannel(void);
BOOL WH_Initialize(void);
BOOL WH_ParentConnect(int mode, u16 tgid, u16 channel);
void WH_SetReceiver(WHReceiverFunc proc);
BOOL WH_SendData(const void *data, u16 size, WHSendCallbackFunc callback);
void WH_Reset(void);
void WH_Finalize(void);
}

// The main program's layer over WH that finds and connects the other DS
#include "wireless/WirelessSession.h"

#endif // OV110_MAINPROGRAM_H

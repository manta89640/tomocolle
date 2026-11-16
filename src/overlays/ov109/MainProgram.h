#ifndef OV109_MAINPROGRAM_H
#define OV109_MAINPROGRAM_H

// What overlay 109 calls of the main program that has no shared header yet: declarations and stand-ins private to
// the overlay, which only TushinScene.cpp includes (the scene factory, src/scene/SceneManager.cpp, includes
// TushinScene.h with the other scenes' headers).

#include "TushinScene.h"

// --- What the scene uses of the main program (STAND-INS, not decompiled) ----------------------------------------

// The archive loader (gArchiveManager): archives are asked for by name and path, and load on a thread.
struct ArchiveLoader;

// Graphics resources, loaded by file name into VRAM.
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;
struct Renderer3d;

extern "C" {
extern ArchiveLoader gArchiveManager;
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void *ArchiveManager_GetFileByName(ArchiveLoader *loader, const char *path);

extern CellLoader gObjResourceManager;
extern PaletteLoader gPaletteLoader;
extern ScreenLoader gScreenLoader;
extern CharLoader gVramTransferQueue;
extern Renderer3d gVramManagerWork;
void VramManagers_Reset(Renderer3d *renderer);
int ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
void *PaletteLoader_LoadFile(PaletteLoader *loader, const char *name, int plane, int a, int b, int c); // Loads a palette
void PaletteLoader_SetBackdrop(PaletteLoader *loader, u16 color, int unknown); // Sets colours of a loaded palette
u16 *ScreenLoader_LoadFile(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void ScreenLoader_SetPriority(ScreenLoader *loader, int plane, int priority);
void ScreenLoader_SetScrollX(ScreenLoader *loader, int plane, int offset);
// Queues a character file for transfer to VRAM
void VramTransferQueue_LoadFile(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void ScreenControl_SetBlendAlpha(ScreenControl *screens, int a, int b, int c, int d);
}


// --- The message window's parts ---

extern "C" {
// Adds a button with a text: (.., number, .., two sprite sequences, position, size, the text's message file and
// message, font, plane, character offset, style, ..)
void MessageWindow_AddTextButtonEx(MessageWindow *window, int number, int a, u8 sequence, u8 sequence2, WindowPoint position,
                   WindowPoint size, MessageFile *file, u32 message, const NNSG2dFont *font, int plane,
                   int *charOffset, const ButtonSounds *style, int b);
// Adds a button without a text
void MessageWindow_AddSpriteButtonEx(MessageWindow *window, int number, int a, int sequence, u8 sequence2, WindowPoint position,
                   WindowPoint size, const ButtonSounds *style, int b);
// Sets the window's message text up
void MessageWindow_AddTextInArea(MessageWindow *window, MessageFile *file, u32 message, const NNSG2dFont *font, int plane,
                   WindowPoint position, WindowPoint margin, int *charOffset);
void MessageWindow_AddSprite(MessageWindow *window, int a, int sequence, WindowPoint position);
}


// --- Characters (main program, include/character/) ---

// A Mii's data, as the Mii exchange sends it (the first 0x4A bytes of a CharacterRecord)
struct MiiData {
    /* 0x00 */ u8 mBytes[0x4A];
}; // 0x4A

extern "C" {
extern const s16 gStandAnimations[4][5]; // Animation ids, by a row and by mood: standing
void func_0209decc(SceneCharacter *character);
void GameData_ReadCharacter(GameData *game, int index, CharacterRecord *record); // Reads a saved character
BOOL GameData_IsCharacterSlotUsed(GameData *game, int index);                          // Whether there is one with that index
void NFL_ClearMiiCreateId(MiiData *mii);
void ResourceSet_SetArchive(void *resourceSet, u32 resType, void *archive);
}


extern "C" {
void func_02024234(ScreenControl *screens, int planes, int a);
void ScreenControl_SetBlendBrightness(ScreenControl *screens, int planes, int a);
void ScreenControl_ClearBlend(ScreenControl *screens, int planes);
void DestroyMainFrameHeap(void);
void func_0201f508(wchar_t *text);
void MiiData_GetCreatorName(CharacterRecord *character, wchar_t *nickname);
int NFL_GetCharInfo(void *buffer);
}

// --- 3D ---

// The main program's camera (gCamera; Camera_Reset resets it).
struct Camera {
    /* 0x00 */ u8 mUnknown00[0x14];
    /* 0x14 */ VecFx32 mPosition;
    /* 0x20 */ VecFx32 mUnknown20;
    /* 0x2C */ VecFx32 mTarget;
    /* 0x38 */ u8 mUnknown38[0xC];
    /* 0x44 */ u16 mAngleX;
    /* 0x46 */ u16 mAngleY;
    /* 0x48 */ u16 mAngleZ;
};

// One of the main program's 3D lights (Light_Reset resets one).
struct Light {
    /* 0x0 */ VecFx16 mDirection;
    /* 0x6 */ u16 mColor;
    /* 0x8 */ BOOL mEnabled;
}; // 0xC

extern "C" {
extern Camera gCamera;
void Camera_Reset(Camera *camera);
extern Light gLights[];
extern Light gLight1[]; // The second light, referred to by its own address (as overlay 110 does)
void Light_Reset(Light *light);
void Light_SetRotatedDirection(Light *light, u16 angleX, u16 angleY, u16 angleZ, fx32 x, fx32 y, fx32 z);
}

// --- Particles (main program 0x0207ab20 to 0x0207b364) ---

struct ParticleEmitter {
    /* 0x00 */ u8 mUnknown00[0x24];
    /* 0x24 */ u32 mFlags;
};
struct ParticleSystem;

extern "C" {
void *CreateMainFrameHeap(int size);
void *AllocFromMainFrameHeap(int size); // An allocator the particle system is given
ParticleSystem *func_0207ab20(void *(*alloc)(int), int a, int b, int c, int d, int e);
void func_0207ac78(ParticleSystem *system, void *file);
void func_0207b110(ParticleSystem *system);
void func_0207b124(ParticleSystem *system);
ParticleEmitter *func_0207b2fc(ParticleSystem *system, int resource, VecFx32 *position);
void func_0207b364(ParticleSystem *system, ParticleEmitter *emitter);
void func_0207b258(ParticleSystem *system, void *a);
void func_0207b138(ParticleSystem *system);
void ModelResources_BeginDraw(void *resources);
}

// --- The resident list (main program 0x020a03e8 to 0x020a380c) ---


extern "C" {
}



extern "C" {
void func_020a2104(ListView *view, int a, int b);
void func_020a26fc(ListView *view, int a, int b, int c, int d, int e, int f, WindowPoint g, ScreenRect h);
void func_020a27dc(ListView *view, int a, int b, int c);
}

// --- Window buttons (the others are in SpriteButton.h) ---

extern "C" {
void SpriteButton_SetDisabled(void *button, int a);
void func_02031228(void *sprite);
}

// --- Items (main program) ---

// --- Songs (main program) ---

extern "C" {
u8 func_0202ca70(GameData *game);                                  // A free song, 0xFF when none
void GameData_GetSong(GameData *game, int song, void *buffer);
void GameData_SetSong(GameData *game, int song, void *buffer);
}

// --- Saving ---

extern "C" {
void GameData_WriteCharacter(GameData *game, int index, CharacterRecord *record); // Writes a saved character
void func_0202c328(GameData *game, int index, int a);
int GameData_GetCharacterSex(GameData *game, int index);
BOOL GameData_IsSaving(GameData *game); // Not 0 while the save data is being written
BOOL GameData_StartSave(GameData *game); // Starts writing the save data; FALSE when it cannot
int NFL_IsMiiCreateIdBit7Clear(CharacterRecord *character);
int NFL_AreMiiCreateIdsEqual(CharacterRecord *a, CharacterRecord *b);
int MiiData_GetSex(CharacterRecord *character);
void MiiData_GetName(CharacterRecord *character, wchar_t *name); // Its name, as a string
void func_0209e01c(SavedCharacter *character);
void MiiTransfer_SendMii(int index);
}

// --- WH, the SDK's wireless sample code (main program) ---

extern "C" {
typedef void (*WHSendCallbackFunc)(BOOL result);
int WH_GetSystemState(void);
BOOL WH_SendData(const void *data, u16 size, WHSendCallbackFunc callback);
}

// --- WH (continued) ---

extern "C" {
void WH_Reset(void);
void WH_Finalize(void);
u16 WH_GetBitmap(void);
typedef void (*WHReceiverFunc)(u16 aid, u16 *data, u16 size);
void WH_SetReceiver(WHReceiverFunc proc);
u16 WH_GetMeasureChannel(void);
int WH_GetLinkLevel(void);
BOOL WH_Initialize(void);
BOOL WH_ParentConnect(int mode, u16 tgid, u16 channel);
}

// --- The Wii's Mii Channel connection (main program 0x02077000 to 0x0207a000) ---

extern "C" {
int MiiTransfer_GetState(void);
int MiiTransfer_GetParentCount(void);
BOOL MiiTransfer_HasParent(int i);
int MiiTransfer_GetParentAge(int i);
WMBssDesc *MiiTransfer_GetParent(int i);
u16 MiiTransfer_GetParentUserGameInfoLength(int i);
const u16 *MiiTransfer_GetParentUserGameInfo(int i);
}

// --- Wireless (main program) ---



extern "C" {
void MiiTransfer_Start(void);
void MiiTransfer_Cut(void);
void MiiTransfer_Connect(u16 id);
BOOL func_0202c37c(GameData *game);
void func_0202d110(GameData *game, int index);
void MiiTransfer_Update(void);
extern u8 gMiiIdSource[];
void MiiTransfer_Init(void *a, void *miiData, int count);
void MiiTransfer_SetReceiveCallback(void (*callback)(int, int));
void MiiTransfer_SetSendResultCallback(void (*callback)(int, int));
void MiiTransfer_SetAcceptSpecial(int a);
extern u16 gWHChildSsid[12]; // WH copies the connecting child's SSID here: the scene shows it as its name
}

class TushinScene;
extern "C" {
void TushinScene_OnWiiSendResult(int, int);
void TushinScene_OnWiiMiiReceived(int, int);
void TushinScene_SendCallback(BOOL result);
void TushinScene_EntryCallback(void *arg);
void TushinScene_Receive(u16 aid, u16 *data, u16 size);
}

#endif // OV109_MAINPROGRAM_H

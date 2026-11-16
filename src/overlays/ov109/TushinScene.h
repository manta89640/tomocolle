#ifndef OV109_TUSHINSCENE_H
#define OV109_TUSHINSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "MessageWindow.h"
#include "Scene.h"
#include "Sprite.h"
#include "sound/SoundManager.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"

#include <wchar.h>

// Resource types, as the files have them (the four letters in memory order)
#define RESOURCE_BCA0 0x30414342 // Joint animation

// --- What the scene uses of the main program (STAND-INS, not decompiled) ----------------------------------------

// The archive loader (gArchiveManager): archives are asked for by name and path, and load on a thread.
struct ArchiveLoader;

// The screens: which planes are shown.
struct ScreenControl {
    /* 0x00 */ u32 mPlanes;
    /* 0x04 */ u8 mUnknown04[0xD0];
    /* 0xD4 */ BOOL mPlanesChanged;
};

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

extern ScreenControl data_0211d604;
extern CellLoader gObjResourceManager;
extern PaletteLoader data_021213b8;
extern ScreenLoader data_02121834;
extern CharLoader gVramTransferQueue;
extern Renderer3d data_02122824;
void func_0202b0e8(Renderer3d *renderer);
int ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
void *func_02027534(PaletteLoader *loader, const char *name, int plane, int a, int b, int c); // Loads a palette
void func_020278f8(PaletteLoader *loader, u16 color, int unknown); // Sets colours of a loaded palette
u16 *func_02027d7c(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028280(ScreenLoader *loader, int plane, int priority);
void func_02028108(ScreenLoader *loader, int plane, int offset);
// Queues a character file for transfer to VRAM
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void func_02023ee0(ScreenControl *screens, int a, int b, int c, int d);
}

// The scene manager.
struct SceneManager {
    /* 0x0 */ Scene *mScene;
    /* 0x4 */ int mNextSceneId;
    /* 0x8 */ int mPreviousSceneId;
};

extern "C" {
extern SceneManager gSceneManager;
void func_0203c02c(SceneManager *manager, int sceneId); // Asks for the next scene
}

// --- The message window's parts ---

// A style of a window button (what the four numbers are is not known)
struct WindowButtonStyle {
    /* 0x0 */ int mUnknown0;
    /* 0x4 */ int mUnknown4;
    /* 0x8 */ int mUnknown8;
    /* 0xC */ int mUnknownC;
};

extern "C" {
// Adds a button with a text: (.., number, .., two sprite sequences, position, size, the text's message file and
// message, font, plane, character offset, style, ..)
void func_02033844(MessageWindow *window, int number, int a, u8 sequence, u8 sequence2, WindowPoint position,
                   WindowPoint size, MessageFile *file, u32 message, const NNSG2dFont *font, int plane,
                   int *charOffset, const WindowButtonStyle *style, int b);
// Adds a button without a text
void func_02033448(MessageWindow *window, int number, int a, int sequence, u8 sequence2, WindowPoint position,
                   WindowPoint size, const WindowButtonStyle *style, int b);
// Sets the window's message text up
void func_020338d0(MessageWindow *window, MessageFile *file, u32 message, const NNSG2dFont *font, int plane,
                   WindowPoint position, WindowPoint margin, int *charOffset);
void func_02033bb4(MessageWindow *window, int a, int sequence, WindowPoint position);
}


// --- Characters (main program 0x0208eb18 to 0x0209e310). STAND-INS with the fields the scene uses ---

// What the save data holds of a character: the 0x2A4 bytes that func_0202c264 copies out (the scene reads one into
// a buffer of this size). The members are as the implicit copy assignment that the scene inlines copies them.
// A Mii's data, as the Mii exchange sends it.
struct MiiData {
    /* 0x00 */ u8 mBytes[0x4A];
}; // 0x4A

struct CharacterRecord {
    /* 0x000 */ MiiData mMii;
    /* 0x04A */ u8 mUnknown04A[5];
    /* 0x04F */ u8 mUnknown04F;
    /* 0x050 */ u32 mUnknown050Bit0 : 1;
    /*       */ u32 mUnknown050Bits1 : 28;
    /*       */ u32 mUnknown050Bits29 : 3;
    /* 0x054 */ u32 mUnknown054Bits0 : 4;
    /*       */ u32 mInterior : 7;
    /*       */ u32 mUnknown054Bits11 : 10;
    /*       */ u32 mUnknown054Bits21 : 4;
    /*       */ u32 mUnknown054Bits25 : 5;
    /*       */ u32 mUnknown054Bits30 : 2;
    /* 0x058 */ wchar_t mName[11];
    /* 0x06E */ wchar_t mUnknown06E[11];
    /* 0x084 */ wchar_t mSpeechDefault[17];
    /* 0x0A6 */ wchar_t mSpeechByMood[4][9];
    /* 0x0EE */ s16 mVoice[6];
    /* 0x0FA */ u8 mUnknown0FA;
    /* 0x0FB */ u8 mUnknown0FB;
    /* 0x0FC */ u8 mAffinity[100];
    /* 0x160 */ u8 mRelation[100];
    /* 0x1C4 */ u32 mUnknown1C4;
    /* 0x1C8 */ struct { u32 m[8]; } mUnknown1C8;
    /* 0x1E8 */ struct { u32 m[10]; } mUnknown1E8;
    /* 0x210 */ struct { u32 m[3]; } mUnknown210;
    /* 0x21C */ s64 mUnknown21C;
    /* 0x224 */ u8 mUnknown224;
    /* 0x225 */ u8 mUnknown225;
    /* 0x226 */ u8 mUnknown226;
    /* 0x227 */ u8 mUnknown227;
    /* 0x228 */ s64 mUnknown228;
    /* 0x230 */ s64 mUnknown230;
    /* 0x238 */ u16 mUnknown238;
    /* 0x23A */ u8 mUnknown23A;
    /* 0x23B */ u8 mUnknown23B;
    /* 0x23C */ s64 mUnknown23C;
    /* 0x244 */ u8 mUnknown244;
    /* 0x245 */ u8 mUnknown245;
    /* 0x246 */ u8 mUnknown246;
    /* 0x247 */ u8 mUnknown247;
    /* 0x248 */ u8 mUnknown248;
    /* 0x249 */ u8 mUnknown249;
    /* 0x24A */ u8 mUnknown24A;
    /* 0x24B */ u8 mUnknown24B;
    /* 0x24C */ u8 mUnknown24C;
    /* 0x24D */ u8 mUnknown24D;
    /* 0x24E */ u8 mUnknown24E;
    /* 0x24F */ u8 mUnknown24F;
    /* 0x250 */ u16 mUnknown250[32];
    /* 0x290 */ u8 mUnknown290;
    /* 0x291 */ u8 mUnknown291;
    /* 0x292 */ u8 mUnknown292[2];
    /* 0x294 */ u8 mUnknown294;
    /* 0x295 */ u8 mMood;
    /* 0x296 */ u8 mUnknown296;
    /* 0x297 */ u8 mUnknown297;
    /* 0x298 */ u8 mUnknown298;
    /* 0x299 */ u8 mUnknown299;
    /* 0x29A */ u8 mSongSlots[6];   // Which of the saved songs (lyrics) each of the character's six is
    /* 0x2A0 */ u8 mUnknown2A0;
    /* 0x2A1 */ u8 mUnknown2A1;
    /* 0x2A2 */ u8 mUnknown2A2;
    /* 0x2A3 */ u8 mUnknown2A3;
}; // 0x2A4

// One saved character (a resident): the record and what the game keeps beside it.
struct SavedCharacter : public CharacterRecord {
    SavedCharacter(); // func_020979c0
    u16 GetIndex() { return mIndex; }

    /* 0x2A4 */ u16 mIndex;        // Which of the saved characters this is
    /* 0x2A6 */ u8 mUnknown2A6[0x16];
    /* 0x2BC */ s8 mPersonality;   // What func_02098f2c computes
    /* 0x2BD */ u8 mUnknown2BD[3];
}; // 0x2C0

// The model of a character (func_0208eb18 constructs it).
// The main program's model (its base classes Model and AnimModel, as overlay 96's header has them).
struct ModelBase {
    BOOL IsVisible() { return mVisible; }

    /* 0x000 */ void *mVtable;
    /* 0x004 */ void *mResourceSet;
    /* 0x008 */ int mUnknown008;
    /* 0x00C */ NNSG3dResMdl *mResMdl;
    /* 0x010 */ u8 mUnknown010[0xA0];
    /* 0x0B0 */ BOOL mVisible;
    /* 0x0B4 */ u8 mUnknown0B4[0x2C];
}; // 0xE0

struct CharacterModel : public ModelBase {
    /* 0x0E0 */ VecFx32 mPos;
    /* 0x0EC */ u8 mUnknown0EC[0x18];
    /* 0x104 */ int mAnimId; // The current 'BCA0' animation
    /* 0x108 */ u8 mUnknown108[0x3C0];
    /* 0x4C8 */ char mArchiveName[4];    // Name its archive is mounted under (func_02091048 sets both)
    /* 0x4CC */ char mArchivePath[0x1C]; // and the archive's path
    /* 0x4E8 */ u8 mUnknown4E8[0x4C];
}; // 0x534 (to the end of the character: where the model ends is not known)

// The character as the main program draws it (func_0208fcf8 is its constructor).
struct CharacterBase : public SavedCharacter {
    /* 0x02C0 */ u8 mUnknown02C0[8];
    /* 0x02C8 */ int mUnknown02C8;
    /* 0x02CC */ u8 mUnknown02CC[0x2318];
}; // 0x25E4

// A character: the class that several scenes hold (its constructor and destructor are the main program's
// func_0203e168 and func_0203e188, which the constructors of those scenes give __cxa_vec_ctor).
//
// The name is descriptive, not the original.
class SceneCharacter : public CharacterBase {
public:
    SceneCharacter();
    ~SceneCharacter();

    /* 0x25E4 */ CharacterModel mModel;
}; // 0x2B18

extern "C" {
extern const s16 data_020fb870[4][5]; // Animation ids, by a row and by mood: standing
struct ModelAnimation {
    /* 0x00 */ u8 mUnknown00[0x14];
    /* 0x14 */ BOOL mFinished;
};
ModelAnimation *AnimModel_FindAnimation(CharacterModel *model, u32 resType, int id);
void func_02090fdc(SceneCharacter *character, int slot, int param2, int param3);
void func_02091048(SceneCharacter *character, const char *archiveName);
void func_02091968(SceneCharacter *character); // Draws it
void Character_SetAnimation(SceneCharacter *character, u32 resType, int id); // Starts an animation
void func_020919e8(SceneCharacter *character, u32 resType, int id);
void func_020919a0(SceneCharacter *character, fx32 speed);
void func_02091a14(SceneCharacter *character, BOOL visible);
void func_0208fefc(SceneCharacter *character);
void func_02091a24(SceneCharacter *character);
void func_02091a88(SceneCharacter *character);
s8 func_02098f2c(SavedCharacter *character);
void func_0209decc(SceneCharacter *character);
void func_0208ef00(CharacterModel *model, int a, int b);
void func_0208ef5c(CharacterModel *model, int resourceId, int a);
void func_02065abc(NNSG3dResMdl *model, int a);
void func_020979e0(SavedCharacter *character);
void func_0202c264(GameData *game, int index, CharacterRecord *record); // Reads a saved character
BOOL func_0202c354(GameData *game, int index);                          // Whether there is one with that index
struct MiiData;
void func_02073574(MiiData *mii);
// The models' resource table (0x1C bytes per resource id)
extern u8 data_021223e4[];
void *func_0202ad04(void *resources, int resourceId, const char *archiveName, int unk);
void func_0202aecc(void *resources, int resourceId);
void func_02029660(void *resourceSet, u32 resType, void *archive);
}

// The main program's object that decides the scene to go back to (0x2C bytes)
struct SceneCheck {
    /* 0x00 */ u8 mUnknown00[0x2C];
};

extern "C" {
void func_020af224(SceneCheck *check, int a, int b);
void func_020af260(SceneCheck *check);
int func_020af4d8(SceneCheck *check, int a);
void func_0203206c(void *button);
void func_02024234(ScreenControl *screens, int planes, int a);
void func_02023f4c(ScreenControl *screens, int planes, int a);
void func_02023eac(ScreenControl *screens, int planes);
void func_0202683c(void);
void func_0201f508(wchar_t *text);
void func_02036bb4(wchar_t *text);
void MiiData_GetCreatorName(CharacterRecord *character, wchar_t *nickname);
int func_020733a4(void *buffer);
void func_02097c64(SavedCharacter *character);
void Character_ResetSoundPhase(SceneCharacter *character, int slot);
void Character_PlaySoundAtAnimPhase(SceneCharacter *character, int slot, int player, int seqArcNo, int index,
                                    int phaseMask);
}

// --- 3D ---

// The main program's camera (data_02122764; func_0202a640 resets it).
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

// One of the main program's 3D lights (func_0202a96c resets one).
struct Light {
    /* 0x0 */ VecFx16 mDirection;
    /* 0x6 */ u16 mColor;
    /* 0x8 */ BOOL mEnabled;
}; // 0xC

extern "C" {
extern Camera data_02122764;
void func_0202a640(Camera *camera);
extern Light data_021227cc[];
extern Light data_021227d8[]; // The second light, referred to by its own address (as overlay 110 does)
void func_0202a96c(Light *light);
void func_0202ab2c(Light *light, u16 angleX, u16 angleY, u16 angleZ, fx32 x, fx32 y, fx32 z);
}

// --- Particles (main program 0x0207ab20 to 0x0207b364) ---

struct ParticleEmitter {
    /* 0x00 */ u8 mUnknown00[0x24];
    /* 0x24 */ u32 mFlags;
};
struct ParticleSystem;

extern "C" {
void *func_0202680c(int size);
void *func_02026824(int size); // An allocator the particle system is given
ParticleSystem *func_0207ab20(void *(*alloc)(int), int a, int b, int c, int d, int e);
void func_0207ac78(ParticleSystem *system, void *file);
void func_0207b110(ParticleSystem *system);
void func_0207b124(ParticleSystem *system);
ParticleEmitter *func_0207b2fc(ParticleSystem *system, int resource, VecFx32 *position);
void func_0207b364(ParticleSystem *system, ParticleEmitter *emitter);
void func_0207b258(ParticleSystem *system, void *a);
void func_0207b138(ParticleSystem *system);
void func_0202ad84(void *resources);
extern u8 data_02142438[];
}

// --- The resident list (main program 0x020a03e8 to 0x020a380c) ---

struct ListView {
    /* 0x0000 */ u8 mUnknown0000[0x9464];
    /* 0x9464 */ int mUnknown9464;
    /* 0x9468 */ u8 mUnknown9468[0x10];
}; // 0x9478

extern "C" {
void func_020a3634(ListView *view);
void func_020a3658(ListView *view, BOOL a);
}

struct ListPages {
    /* 0x0000 */ u8 mUnknown0000[0x5158];
}; // 0x5158

struct ResidentList {
    SavedCharacter *GetSelected() { return &mResidents[mSelected]; }

    /* 0x00000 */ SavedCharacter mResidents[100];
    /* 0x11300 */ u8 mUnknown11300[6];
    /* 0x11306 */ u16 mUnknown11306;
    /* 0x11308 */ u8 mUnknown11308[0x88];
    /* 0x11390 */ int mSelected; // The resident chosen, an index of mResidents
    /* 0x11394 */ u8 mUnknown11394[4];
    /* 0x11398 */ int mUnknown11398;
    /* 0x1139C */ int mUnknown1139C;
    /* 0x113A0 */ u8 mUnknown113A0[8];
}; // 0x113A8

extern "C" {
void func_020a03e8(ResidentList *list, ListView *view, ListPages *pages);
void func_020a05dc(ResidentList *list);
void func_020a0454(ResidentList *list);
void func_020a05b8(ResidentList *list); // Draws it
void func_020a123c(ResidentList *list, int a, int b);
void func_020a1fe0(ListView *view, int a);
void func_020a2104(ListView *view, int a, int b);
void func_020a2118(ListView *view, int a, int b, int c, WindowPoint d, WindowPoint e);
void func_020a23cc(ListView *view, int a, int b, WindowPoint c);
void func_020a2280(ListView *view, int plane, WindowPoint a, int *charOffset);
void func_020a258c(ListView *view, int a, int b, int c, WindowPoint d, WindowPoint e);
void func_020a2644(ListView *view, int a, int b, int c, WindowPoint d, WindowPoint e);
void func_020a26fc(ListView *view, int a, int b, int c, int d, int e, int f, WindowPoint g, ScreenRect h);
void func_020a27dc(ListView *view, int a, int b, int c);
void func_020a380c(ListPages *pages, int a, int b);
}

// --- Window buttons ---

extern "C" {
void func_02031260(WindowButton *button, int a);
void func_02031278(WindowButton *button, int a, int b);
void func_02031eb0(WindowButton *button, int x, int y);
void func_02032158(WindowButton *button, const WindowButtonStyle *style);
void func_0203119c(WindowButton *button);
int func_02031f58(WindowButton *button, MessageFile *file, u32 message, const NNSG2dFont *font, int plane,
                  int charOffset);
void func_02031e88(void *button, int a);
void func_02031228(void *sprite);
BOOL func_0203133c(void *button);
}

// --- Items (main program) ---

class InventoryItem {
public:
    BOOL Add(int count); // func_0209ed84

    /* 0x00 */ void *mVtable;
    /* 0x04 */ u16 mId;
    /* 0x08 */ BOOL mOwned;
    /* 0x0C */ int mUnknown0C;
    /* 0x10 */ int mCount;
}; // 0x14

// One kind of clothes the player owns or not (func_0209e168 constructs it). That these are the clothes is
// inferred: the wireless scene gives item 0x113 or 0x114 with the messages "got a DS T-shirt" and "got a Wii
// T-shirt" (etc messages 0x124 and 0x125).
class ClothesItem : public InventoryItem {
public:
    ClothesItem(); // func_0209e168

    /* 0x14 */ int mUnknown14;
}; // 0x18

// The clothes, loaded from and saved to the save data together (func_0209e248 and func_0209e310).
struct ClothesInventory {
    /* 0x0000 */ int mCount;
    /* 0x0004 */ ClothesItem mItems[300];
    /* 0x1C24 */ ClothesItem *mList[300];
}; // 0x20D4

extern "C" {
void func_0209e248(ClothesInventory *inventory); // Loads it
void func_0209e310(ClothesInventory *inventory); // Saves it
}

// --- Songs (main program) ---

// A song's object of the main program (func_020af058 sets it up)
struct SongEntry {
    /* 0x00 */ u32 mUnknown00[0x10];
    /* 0x40 */ u8 mSong;
}; // 0x44

extern "C" {
int func_0209c718(CharacterRecord *character, int slot);          // The saved song of a slot, 0xFF for none
void func_0209c73c(CharacterRecord *character, int slot, int song);
u8 func_0202ca70(GameData *game);                                  // A free song, 0xFF when none
void func_0202c920(GameData *game, int song, void *buffer);
void func_0202c934(GameData *game, int song, void *buffer);
void func_020af058(SongEntry *entry);
void func_020af06c(SongEntry *entry, u8 a);
void func_020af084(SongEntry *entry, u8 a);
void func_020af0b8(SongEntry *entry, int a, int b);
}

// --- Saving ---

extern "C" {
void func_0202c28c(GameData *game, int index, CharacterRecord *record); // Writes a saved character
void func_0202c328(GameData *game, int index, int a);
int func_0202c604(GameData *game, int index);
BOOL func_0202cd38(GameData *game); // Not 0 while the save data is being written
BOOL func_0202cd48(GameData *game); // Starts writing the save data; FALSE when it cannot
int func_02073520(CharacterRecord *character);
int func_02073560(CharacterRecord *a, CharacterRecord *b);
int func_02098584(CharacterRecord *character);
int MiiData_GetSex(CharacterRecord *character);
void MiiData_GetName(CharacterRecord *character, wchar_t *name); // Its name, as a string
void func_02097cc0(SavedCharacter *character);
void func_0209e01c(SavedCharacter *character);
void func_020a0664(ResidentList *list);
void func_02078db8(int index);
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
int func_02078f04(void);
int func_0207808c(void);
BOOL func_0207800c(int i);
int func_02078040(int i);
WMBssDesc *func_02078064(int i);
u16 func_0207809c(int i);
const u16 *func_020780b4(int i);
}

// --- Wireless (main program) ---

struct WirelessManager {
    /* 0x00 */ int mUnknown00;
    /* 0x04 */ int mUnknown04;
    /* 0x08 */ int mUnknown08;
    /* 0x0C */ int mUnknown0C;
    /* 0x10 */ int mUnknown10;
    /* 0x14 */ int mUnknown14;
    /* 0x18 */ int mUnknown18;
    /* 0x1C */ int mParent; // The partner chosen, an index of the ones found
};

extern "C" {
extern WirelessManager *data_0213b700;
void func_0203b76c(WirelessManager *manager, int a);
int func_0203b800(WirelessManager *manager);
void func_0203b7e8(WirelessManager *manager);
void func_0203b87c(WirelessManager *manager);
void func_0203b900(WirelessManager *manager, int a);
void func_0203b940(WirelessManager *manager, int a);
void func_0203b968(WirelessManager *manager, int a);
int func_0203b9c4(WirelessManager *manager);
void func_0203bac4(WirelessManager *manager, int parent);
void func_02078d04(void);
void func_02078d48(void);
void func_02078da4(u16 id);
BOOL func_0202c37c(GameData *game);
void func_0202d110(GameData *game, int index);
void func_0203b8b4(WirelessManager *manager, int a);
void func_020777f4(void);
void func_0203b7d4(WirelessManager *manager, void *buffer, int a);
void func_0203b7e0(WirelessManager *manager, void *buffer);
void func_0203b898(WirelessManager *manager);
void func_0203b908(WirelessManager *manager);
void func_0203b948(WirelessManager *manager);
BOOL func_0203b970(WirelessManager *manager);
BOOL func_0203b9b4(WirelessManager *manager);
void func_0203b9fc(WirelessManager *manager);
int func_0203ba10(WirelessManager *manager);
void func_0203ba20(WirelessManager *manager, wchar_t *name);
extern u8 data_0212349c[];
void func_0207777c(void *a, void *miiData, int count);
void func_02077710(void (*callback)(int, int));
void func_02077720(void (*callback)(int, int));
void func_02077804(int a);
extern u16 gWHChildSsid[12]; // WH copies the connecting child's SSID here: the scene shows it as its name
}

class TushinScene;
extern "C" {
void func_ov109_02151fcc(int, int);
void func_ov109_021523a4(int, int);
void func_ov109_02151eb4(BOOL result);
void func_ov109_0215245c(void *arg);
void func_ov109_02151ee0(u16 aid, u16 *data, u16 size);
}

// --- The scene ----------------------------------------------------------------------------------------------------

// The layout of one kind of window: which of its four buttons it has
struct WindowLayout {
    /* 0x0 */ BOOL mHasButton[3];
    /* 0xC */ BOOL mHasBackButton;
};

// Overlay 109, scene 0x29: wireless play (通信), the Mii exchange. The scene save screen (overlay 69, scene 0x2D)
// comes here after saving. Its pages (mPage, an index of sPageMessages and sPageLayouts) show etc messages 0xF7 to
// 0x125: what to exchange (Miis, or food, clothes and treasure, which is scene 0xB1, overlay 110), with what (another
// Tomodachi Collection, through WH, or the Wii's Mii Channel, through the main program's Mii transfer code), and on
// a DS whether to recruit a partner (the parent) or to find one (a child). A resident is chosen in the list on the
// lower screen and sent: the 0x2C0-byte character (sTransfer) in 0x80-byte chunks, then the songs of its six slots
// (sSongs); the receiver checks it (IsInvalid, the NG words of HasNgWord, a resident with the same Mii), stores it
// as a new resident or updates the one it has, and answers with a 2-byte code (data_ov109_02153200). The Wii sends Miis
// only: ReceiveWiiMii adds one from sMiiData. A DS or Wii T-shirt is given the first time (GiveShirt,
// GiveWiiShirt). The scene factory (main CreateScene, case 41) loads overlay 109 and allocates 0x3F0B4 bytes; the
// constructor is the main program's (func_02040644).
//
// The name is descriptive, not the original: no name of this class survives in the ROM. It is after the scene's
// archive, "Scn/Scene/SceneTushin_LZ.bin".
class TushinScene : public Scene {
public:
    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    SavedCharacter *GetSelectedResident() { return &mResidents.mResidents[mResidents.mSelected]; }

    void SetUpYesNoWindow(int *charOffset, MessageFile *file, u32 message);
    void SetUpWindow(int page);
    void SetUpList(int *charOffset);
    void InitCharacters();
    void InitParticles();
    void UpdateMiiData();
    void ResetExchange();
    void SetUpCharacter(int i);
    int GetStandAnimation(int i);
    void ReloadCharacter(int i);
    void LoadCharacters();
    void ShowNextCharacter();
    void LoadSavedCharacter(int i, int index);
    void LoadCharacter(int i);
    void ReloadCharacterFromResident(int i);
    void ReloadCharacterFromReceived(int i);
    void SendSongs();
    void Send(int kind);
    void UpdateSending();
    void SendFoundIndex();
    void UpdateSaving();
    int FindResident(CharacterRecord *record);
    void RefreshRecord(CharacterRecord *record);
    int ReceiveCharacter();
    int GetMode();
    BOOL IsNgWord(const wchar_t *text);
    BOOL IsBlank(const wchar_t *text);
    BOOL HasNgWord(CharacterRecord *record);
    BOOL HasNgSongReceived();
    BOOL HasNgSong(CharacterRecord *record);
    BOOL IsMiiBroken(CharacterRecord *record);
    BOOL IsNameBlank(CharacterRecord *record);
    BOOL IsNicknameBlank(CharacterRecord *record);
    BOOL IsInvalid(CharacterRecord *record);
    int ReceiveWiiMii();
    void UpdateCharacters();
    void DrawCharacters();
    void UpdateSignal();
    void Disconnect(int kind);
    void Leave();
    void UpdateListButtons();
    void UpdateListWindow();
    void UpdateConnection();
    void UpdateWiiConnection();
    void SetStandAnimation(u8 i);
    void SetOtherStandAnimation();
    void StartSendAnimation();
    void StartReceiveAnimation();
    void UpdateMusic();
    void ResetList();
    void ShowReceived();
    void CloseWindow();
    void StartEmitter(int i);
    void CountFrame();
    void DrawParticles();
    BOOL func_ov109_02151d74(CharacterRecord *record);
    BOOL GiveItem(int item);
    BOOL GiveShirt();
    BOOL GiveWiiShirt();
    void SetEntry(BOOL enabled);
    void ClearParentNames();
    void Save();
    void SetUnknown3F098();
    int NextPageAfterReceive();

    /* 0x00010 */ int mPage;     // What the window shows: an index of sPageMessages and sPageLayouts
    /* 0x00014 */ int mNextPage;
    /* 0x00018 */ int mUnknown18;
    /* 0x0001C */ int mUnknown1C;
    /* 0x00020 */ int mWait;
    /* 0x00024 */ int mUnknown24;
    /* 0x00028 */ int mUnknown28;
    /* 0x0002C */ MessageFile mMessageFiles[2]; // Etc messages, common messages
    /* 0x0005C */ int mUnknown5C;
    /* 0x00060 */ MessageWindow mWindow;
    /* 0x0B004 */ ResidentList mResidents;
    /* 0x1C3AC */ ListView mList;
    /* 0x25824 */ ListPages mPages;
    /* 0x2A97C */ MessageWindow mListWindow;
    /* 0x35920 */ u8 mUnknown35920;
    /* 0x35924 */ int mUnknown35924;
    /* 0x35928 */ WindowButton mButtons[3];
    /* 0x36DF8 */ Sprite mSprite;
    /* 0x36EA4 */ int mCharOffset;
    /* 0x36EA8 */ int mUnknown36EA8;
    /* 0x36EAC */ void *mPalette;
    /* 0x36EB0 */ Camera *mCamera;
    /* 0x36EB4 */ SceneCharacter mCharacters[3];
    /* 0x3EFFC */ u8 mCurrent;
    /* 0x3F000 */ int mUnknown3F000;
    /* 0x3F004 */ ParticleSystem *mParticles;
    /* 0x3F008 */ ParticleEmitter *mEmitters[2];
    /* 0x3F010 */ void *mParticleFile;
    /* 0x3F014 */ VecFx32 mEmitterPositions[2];
    /* 0x3F02C */ int mParticleResource;
    /* 0x3F030 */ int mUnknown3F030;
    /* 0x3F034 */ int mUnknown3F034;
    /* 0x3F038 */ wchar_t mParentNames[3][11]; // Of the Wiis (or DSs) found
    /* 0x3F07A */ u16 mParentIds[3];
    /* 0x3F080 */ int mUnknown3F080;
    /* 0x3F084 */ int mUnknown3F084;
    /* 0x3F088 */ int mUnknown3F088;
    /* 0x3F08C */ int mUnknown3F08C;
    /* 0x3F090 */ int mUnknown3F090;
    /* 0x3F094 */ int mUnknown3F094;
    /* 0x3F098 */ int mUnknown3F098;
    /* 0x3F09C */ int mUnknown3F09C;
    /* 0x3F0A0 */ int mUnknown3F0A0;
    /* 0x3F0A4 */ int mUnknown3F0A4;
    /* 0x3F0A8 */ int mUnknown3F0A8;
    /* 0x3F0AC */ int mUnknown3F0AC;
    /* 0x3F0B0 */ int mUnknown3F0B0;
}; // 0x3F0B4

#endif // OV109_TUSHINSCENE_H

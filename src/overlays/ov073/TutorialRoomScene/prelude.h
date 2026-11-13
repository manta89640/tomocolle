#pragma once
#include <nitro.h>
#include <nnsys.h>

#include "TutorialRoomScene.h"

extern "C" {

// --- Main program, not named yet (the declarations are those of overlay 74's RoomScene unit where it has them) ---

// The saved game
struct SavedGame;
extern SavedGame gGameData;
BOOL func_0202c3dc(SavedGame *game, int index); // Whether the saved character with that index exists
int func_0202c414(SavedGame *game, int index);  // Its room number (SavedCharacter::mRoom)

// The main program's character (CharacterBase)
void func_02091048(CharacterBase *character, const char *archiveName); // Names its archive
void func_02090fdc(CharacterBase *character, int slot, int unknown2, int unknown3); // Sets it up (overlay 74: from RoomCharacter::Init)
void func_0208fefc(CharacterBase *character);
void func_02091a24(CharacterBase *character);
void func_02091a6c(CharacterBase *character);
void func_020910d4(CharacterBase *character);
void func_02091a88(CharacterBase *character);
void Character_SetAngle(CharacterBase *character, u16 angle); // Turns it (sets both of its angles)
s8 func_02098f2c(SavedCharacter *character);   // Computes its personality (0..15) from two pairs of its values
void func_02099088(SavedCharacter *character, int index, int change); // Adds to its affinity for another
void func_020990b0(SavedCharacter *character, int index, u8 relation, int affinity); // Sets its relation to another
void func_02029660(void *resource, u32 resType, void *archive);
VecFx32 CharacterModel_GetJointPosition(CharacterModel *model, int joint); // Position of one of the model's joints
// For each kind of relation, how much each reaction changes the affinity by, halved
extern const s8 *data_021135c8[];

// The archive loader: archives are asked for by name and path, and load on a thread
struct ArchiveLoader;
extern ArchiveLoader gArchiveManager;
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void *ArchiveManager_GetFileByName(ArchiveLoader *loader, const char *path);

// The screens: which planes are shown, and their brightness
struct ScreenControl {
    void Set(u32 planes) {
        mPlanes = planes;
        mPlanesChanged = TRUE;
    }
    void Show(u32 planes) {
        mPlanes |= planes;
        mPlanesChanged = TRUE;
    }

    /* 0x00 */ u32 mPlanes; // Of the main screen
    /* 0x04 */ u8 mUnknown04[0x44];
    /* 0x48 */ int mUnknown48;
    /* 0x4C */ u8 mUnknown4C[0x88];
    /* 0xD4 */ BOOL mPlanesChanged;
    /* 0xD8 */ u8 mUnknownD8[0x14];
    /* 0xEC */ int mUnknownEC;
};
extern ScreenControl data_0211d604;
void func_02023eac(ScreenControl *screens, int mask);
void func_02023ee0(ScreenControl *screens, int a, int b, int c, int d);
// Something of the planes in the mask that the scene sets to -10 before a part and to 0 when the message window
// comes (a brightness, by the values)
void func_02023f4c(ScreenControl *screens, int mask, int value);

// The buttons
struct PadInput {
    /* 0x00 */ u32 mUnknown00;
    /* 0x04 */ u32 mHeld;
};
extern PadInput data_0211f9d4;

// The game's random number generator: every use is the inline code of the SDK's MATH_Rand16
extern MATHRandContext16 gRandom;

// Graphics resources, loaded by file name into VRAM
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;
struct Renderer3d;
extern CellLoader gObjResourceManager;
extern PaletteLoader data_021213b8;
extern ScreenLoader data_02121834;
extern CharLoader gVramTransferQueue;
extern Renderer3d data_02122824;
void func_0202b0e8(Renderer3d *renderer);
void ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
// The same without the characters. It returns something (what ObjResourceManager_Load leaves in r0) that Init
// does not use: declared void, Init's instructions come out in another order.
int func_02027290(CellLoader *loader, const char *cellName, int plane, int a);
void *func_02027534(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
void func_020278f8(PaletteLoader *loader, int a, int b);
u16 *func_02027d7c(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028280(ScreenLoader *loader, int plane, int priority);
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void func_0202680c(int size);
void func_0202683c();

// The particle system and its emitters (HeadIcon)
void func_02080e98(ParticleSystem *particles, void *file, int a, int b, int c, int d, int e);
void func_02080ee4(ParticleSystem *particles);
void func_02080f00(ParticleSystem *particles, int unknown); // Draws
void func_0207b138(void *manager);                          // Updates
void func_020810b4(HeadIcon *icon, ParticleSystem *particles, int iconId);
void func_020810c8(HeadIcon *icon);
void HeadIcon_SetMode(HeadIcon *icon, int mode);
void func_020810e4(HeadIcon *icon);
void func_02080fc4(HeadIcon *icon);

// The room model's base class, and its parts
void func_020a54c4(Room *room, const char *archiveName, int unknown);
void func_020a56bc(Room *room, const char *archiveName, int unknown);
void func_020a5514(Room *room, int slot);
void func_020a5760(Room *room, int slot);
void func_020a4acc(RoomParts *parts, int slot);
void func_020a4a50(RoomParts *parts); // Draws

// The message window
void func_02032fb8(MessageWindow *window, ScreenRect rect);                     // Sets it up, with its rectangle
void func_02033174(MessageWindow *window, const char *screenName, int plane);   // The screen file of its background
void func_020331bc(MessageWindow *window, int planes, ScreenRect rect);
// Adds a button with a text: (.., position, size, the text's message file and message, font, .., character offset)
void func_020336b4(MessageWindow *window, int a, int b, int c, u8 d, WindowPoint position, WindowPoint size,
                   MessageFile *file, int message, const NNSG2dFont *font, int plane, int *charOffset, int e, int f,
                   int g, int h);
// Adds a message text: (.., message file, message, font, plane, position, character offset)
void func_02033a58(MessageWindow *window, MessageFile *file, int message, const NNSG2dFont *font, int plane,
                   WindowPoint position, int *charOffset);
void func_020341d4(MessageWindow *window);                                      // Updates
void func_020345d8(MessageWindow *window);                                      // Draws
void func_02034720(MessageWindow *window, int flags, int unknown);              // Opens it
void func_020349cc(MessageWindow *window, int flags);                           // Closes it
void func_02034a3c(MessageWindow *window, int text, int argIndex, const wchar_t *string); // SetStringArg and DrawAll of a text
void func_02034f38(MessageWindow *window, int text, u32 message);               // Sets the message of a text
void func_02034f4c(MessageWindow *window, u8 hAlign);                           // Of every text
void func_02034f94(MessageWindow *window, u8 vAlign);

// The scene manager
struct SceneManager {
    /* 0x0 */ Scene *mScene;
    /* 0x4 */ int mNextSceneId;
    /* 0x8 */ int mPreviousSceneId;
};
extern SceneManager gSceneManager;
void func_0203c02c(SceneManager *manager, int sceneId); // Asks for the next scene

// What the scenes remember for each other
struct SceneMemory {
    /* 0x00 */ u8 mUnknown00[0x1E];
    /* 0x1E */ u8 mTutorialStep; // The scene copies it into its mTutorialStep
};
extern SceneMemory data_0213ba74;

} // extern "C"

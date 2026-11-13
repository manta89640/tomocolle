#include <nitro.h>
#include <nnsys.h>

#include "RoomScene.h"
#pragma once

extern "C" {

// --- Main program, not named yet -------------------------------------------------------------------------------

// The saved game (the listing calls it gGameData; the sound code reads the volume settings from it)
struct SavedGame {
    u8 GetBuilding() { return mBuilding & 3; }

    /* 0x00 */ u8 mUnknown00[0x4C];
    /* 0x4C */ u8 mBuilding; // Its low two bits: which building the plain apartment scene (id 0x11) shows
};
extern SavedGame gGameData;
BOOL func_0202c3dc(SavedGame *game, int index); // Whether the saved character with that index exists
int func_0202c414(SavedGame *game, int index);  // Its room number (SavedCharacter::mRoom), 0xFF without save data
u8 func_0202c468(SavedGame *game, int index);   // Its event (SavedCharacter::mEvent)
int func_0202c490(SavedGame *game, int index);  // Its event's partner (SavedCharacter::mEventPartner)
s64 func_0202c4b8(SavedGame *game, int index);  // The time its next event starts at
s64 func_0202c4e4(SavedGame *game, int index);  // The time its event ends at (SavedCharacter::mEventEnd)

// Tests of an event number (SavedCharacter::mEvent), and of a character's
BOOL func_02099c94(u8 event); // An event with a partner (func_02099d2c) or one of 0x13..0x16: the character is out
BOOL func_02099cec(u8 event); // An event with a partner, of either kind (func_02099d2c or func_02099db8)
BOOL func_02099d2c(u8 event); // 0x17, 0x19, ... 0x2d
BOOL func_02099db8(u8 event); // 0x18, 0x1a, ... 0x2e
BOOL func_02099c84(SavedCharacter *character); // func_02099c94 of its event
BOOL func_02099cdc(SavedCharacter *character); // func_02099cec
BOOL func_02099d1c(SavedCharacter *character); // func_02099d2c
BOOL func_02099da8(SavedCharacter *character); // func_02099db8
BOOL func_02099e64(SavedCharacter *character); // func_02099e74
BOOL func_0209a444(SavedCharacter *character);
s8 func_02098f2c(SavedCharacter *character);   // Computes its personality (0..15) from two pairs of its values
int func_02098ec4(SavedCharacter *character);  // A value from 0 to 200
void func_02099088(SavedCharacter *character, int index, int change); // Adds to its affinity for another
// Starts the event that is due for a character; the other two are the characters in the room, if they are others
void func_02099c28(SavedCharacter *character, s64 now, RTCTime *time, SavedCharacter *first,
                   SavedCharacter *second);
void func_0209a588(SavedCharacter *character, s64 now, SavedCharacter *partner); // Ends its event
void func_0209b14c(SavedCharacter *character, s64 now);
BOOL func_0209b208(SavedCharacter *character, s64 now, int force, SavedCharacter *partner, int unused);
void func_02099324(SavedCharacter *character, s64 now);
void func_0209b1d8(SavedCharacter *character);
// For each kind of relation, how much each reaction changes the affinity by, halved (a1)
extern const s8 *data_021135c8[];

// What the clock means for all the saved characters
void func_020ad568(s64 now);
void func_020ad514(s64 now, RTCTime *time);
void func_020ad294(s64 now, RTCTime *time);
BOOL func_020ad348(int index, s64 now);
int func_020adb44(int *force);  // How many characters could start a random event
int func_020adc08(u16 *indices); // Lists them, and returns how many
int func_020adfbc(EncounterJudge *judge, RoomCharacter *first, RoomCharacter *second);

// The main program's character (CharacterBase)
void func_02091048(CharacterBase *character, const char *archiveName); // Names its archive
void func_0208fefc(CharacterBase *character);
void func_02091a24(CharacterBase *character);
void func_02091a88(CharacterBase *character);
void func_02091a14(CharacterBase *character, int value);
void Character_SetAngle(CharacterBase *character, u16 angle); // Turns it (sets both of its angles)
void func_0209171c(CharacterBase *character, FloorPos pos);
void func_02029660(void *resource, u32 resType, void *archive);

// The archive loader: archives are asked for by name and path, and load on a thread
struct ArchiveLoader;
extern ArchiveLoader gArchiveManager;
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown); // Asks for one
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);                                // Lets go of one
BOOL ArchiveManager_IsIdle(ArchiveLoader *loader);                                                  // Nothing is loading
void *ArchiveManager_GetFileByName(ArchiveLoader *loader, const char *path);                               // A file of one

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
    void Hide2(u32 planes) {
        u32 x = mPlanes & ~planes;
        Set(x);
    }
    void Hide3(int plane) {
        mPlanes &= ~(1 << plane);
        mPlanesChanged = TRUE;
    }
    void Hide(u32 planes) {
        mPlanes &= ~planes;
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
void func_02023be8(ScreenControl *screens, int mask, fx32 brightness);
void func_02023c54(ScreenControl *screens, int mask, fx32 speed);
int func_02023c44(ScreenControl *screens, int mask); // The brightness now
void func_02023eac(ScreenControl *screens, int mask);
void func_02023ee0(ScreenControl *screens, int a, int b, int c, int d);

// The buttons
struct PadInput {
    /* 0x00 */ u32 mUnknown00;
    /* 0x04 */ u32 mHeld;
    /* 0x08 */ u8 mUnknown08[0x18];
    /* 0x20 */ BOOL mUnknown20; // Has to be set for a let-go on a button to count
};
extern PadInput data_0211f9d4;

// The touch screen
struct TouchInput {
    /* 0x00 */ u8 mUnknown00[0x2C];
    /* 0x2C */ u16 mTouching;
    /* 0x2E */ u8 mUnknown2E[2];
    /* 0x30 */ TouchPoint mLastPoint; // data_02121e88: where it was touched last
    /* 0x38 */ TouchPoint mPoint;     // data_02121e90: where it is touched
    /* 0x40 */ u8 mUnknown40[8];
    /* 0x48 */ BOOL mTouched;         // The stylus went down this frame
};
extern TouchInput data_02121e58;

// The game's random number generator: every use is the inline code of the SDK's MATH_Rand16
extern MATHRandContext16 gRandom;
// The SDK's sine and cosine table (FX_SinCosTable_)
extern const fx16 data_020b0810[];

// Graphics resources, loaded by file name into VRAM
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;
struct TextureArchives;
struct Renderer3d;
extern CellLoader gObjResourceManager;
extern PaletteLoader data_021213b8;
extern ScreenLoader data_02121834;
extern CharLoader gVramTransferQueue;
extern TextureArchives data_021223e4;
extern Renderer3d data_02122824;
void func_0202b0e8(Renderer3d *renderer);
void func_0202ae38(TextureArchives *archives);
void *func_0202ad04(TextureArchives *archives, int slot, const char *archiveName, int unknown);
void func_0202aecc(TextureArchives *archives, int slot);
void ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
void *func_02027534(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
void func_020278f8(PaletteLoader *loader, int a, int b);
void *func_02027d7c(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028280(ScreenLoader *loader, int plane, int priority);
void func_02028108(ScreenLoader *loader, int plane, int x);
void func_02028154(ScreenLoader *loader, int plane, int y);
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void func_0202680c(int size);
void func_0202683c();
void func_02060c1c();

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

// The room model's base class, and its door
void func_020a54c4(RoomModel *room, const char *archiveName, int unknown);
void func_020a56bc(RoomModel *room, const char *archiveName, int unknown);
void func_020a5514(RoomModel *room, int slot);
void func_020a5760(RoomModel *room, int slot);
void func_020a5948(RoomModel *room);
void func_020a4acc(void *door, int slot);
void func_020a4a50(void *door);
void func_020a4b74(void *door, int unknown);
void func_020a4ab8(void *door, int motion);

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
    /* 0x00 */ u8 mUnknown00[0x20];
    /* 0x20 */ u8 mCharacter; // The saved character whose room was shown last
};
extern SceneMemory data_0213ba74;

// --- The overlay's other groups, not named in the notes --------------------------------------------------------

void func_ov074_02152500(RoomCharacter *character);
void func_ov074_021686a4(RoomCharacter *character);
void func_ov074_021686dc(RoomCharacter *character);
int func_ov074_021686f4(RoomCharacter *character);
void func_ov074_02168a20(RoomCharacter *character, FloorPos *out);
void func_ov074_02169890(RoomCharacter *character, int transition);
void func_ov074_02169c6c(RoomCharacter *character);
void func_ov074_02152308(RoomCharacter *character, ParticleSystem *particles);
void func_ov074_0215243c(RoomCharacter *character);
void func_ov074_02152268(RoomCharacter *character, int slot, int a, int b);
void func_ov074_02152370(RoomCharacter *character, int state);

void func_ov074_02169e4c(HeldItem *item, const char *archiveName, u8 kind);
void func_ov074_02169e84(HeldItem *item, int slot);
void func_ov074_02169ebc(HeldItem *item);
void func_ov074_02169fc8(HeldItem *item, int transition);

void func_ov074_0216a2c4(RoomModel *room);
void func_ov074_0216a2d0(RoomModel *room, u16 interior);
void func_ov074_0216a2dc(RoomModel *room, u16 interior);
void func_ov074_0216a330(RoomModel *room);
void func_ov074_0216a354(RoomModel *room, int transition);

void func_ov074_0216a514(RoomCamera *camera);
void func_ov074_0216a618(RoomCamera *camera, int mode);
void func_ov074_0216a62c(RoomCamera *camera, int state, s16 timer);
void func_ov074_0216a638(RoomCamera *camera);

void func_ov074_0216b55c(CameraFollower *follower, RoomCamera *camera);
void func_ov074_0216b5bc(CameraFollower *follower);
void func_ov074_0216b8a8(CameraFollower *follower, int state);

BOOL func_ov074_0216b9c8(BuildingPanel *panel, TouchPoint point);
void func_ov074_0216bb18(PanelButton *button, int state);
BOOL func_ov074_0216bb20(PanelButton *button, TouchPoint point);
void func_ov074_0216bc08(BuildingPanel *panel, u8 building);
void func_ov074_0216bc18(BuildingPanel *panel, u8 room);
void func_ov074_0216bd2c(BuildingPanel *panel, TouchPoint point);
void func_ov074_0216bd60(BuildingPanel *panel);
void func_ov074_0216be1c(BuildingPanel *panel);
void func_ov074_0216be3c(BuildingPanel *panel, int room, int unknown);
void func_ov074_0216c1f0(BuildingPanel *panel, int room);
void func_ov074_0216c214(BuildingPanel *panel, int unknown, int room);

void func_ov074_0216c3cc(WindowView *view, RoomClock *clock, u8 building);
void func_ov074_0216c404(WindowView *view);
void func_ov074_0216c410(WindowView *view);

void func_ov074_0216c618(NamePlate *plate, int charOffset, MessageFile *a, MessageFile *b);
void func_ov074_0216c7a4(NamePlate *plate);
void func_ov074_0216c8f0(NamePlate *plate);
void func_ov074_0216c918(NamePlate *plate, int state);
void func_ov074_0216c92c(NamePlate *plate, void *a, wchar_t *name);

BOOL func_ov074_0216ca14(TouchButton *button, BOOL enabled);
void func_ov074_0216ca4c(TouchButton *button, s16 action);
void func_ov074_0216cb34(TouchButton *button);
void func_ov074_0216cc2c(TouchButton *button);
BOOL func_ov074_0216cc38(TouchButton *button, TouchPoint point);

void func_ov074_0216ccac(RoomClock *clock);
void func_ov074_0216ccd4(RoomClock *clock);
void func_ov074_0216cd10(ClockDisplay *display, RoomClock *clock);
void func_ov074_0216cdc8(ClockDisplay *display);
void func_ov074_0216cfc0(ClockDisplay *display);
void func_ov074_0216cfe8(ClockDisplay *display, int state);

void func_ov074_0216d070(SpeechText *text);
void func_ov074_0216d1a8(SpeechText *text, int state);

} // extern "C"


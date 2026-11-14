#include <nitro.h>
#include <nnsys.h>

#include "sound/SoundManager.h"

#include "RoomScene.h"

// ===================================================================================================================
// The main program's functions and data that the overlay uses, not decompiled yet

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
BOOL func_02099c94(int event); // An event with a partner (func_02099d2c) or one of 0x13..0x16: the character is out
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

// The main program's character (CharacterBase)
void func_02091048(CharacterBase *character, const char *archiveName); // Names its archive
void func_0208fefc(CharacterBase *character);
void func_02091a24(CharacterBase *character);
void func_02091a88(CharacterBase *character);
void func_02091a14(CharacterBase *character, int value);
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
    /* 0x30 */ ScreenPoint mLastPoint; // data_02121e88: where it was touched last
    /* 0x38 */ ScreenPoint mPoint;    // data_02121e90: where it is touched
    /* 0x40 */ u8 mUnknown40[8];
    /* 0x48 */ BOOL mTouched;         // The stylus went down this frame
};
extern TouchInput gTouchInput;

// The game's random number generator: every use is the inline code of the SDK's MATH_Rand16
extern MATHRandContext16 gRandom;
// The SDK's sine and cosine table (FX_SinCosTable_)
extern const fx16 data_020b0810[];

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
void func_0202ae38(void *resources); // Of the models' resource table, data_021223e4 (Model.h)
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

// The particle system
void func_02080e98(ParticleSystem *particles, void *file, int a, int b, int c, int d, int e);
void func_02080ee4(ParticleSystem *particles);
void func_02080f00(ParticleSystem *particles, int unknown); // Draws
void func_0207b138(void *manager);                          // Updates

// The room (Room.h has the others)
void func_020a54c4(Room *room, const char *archiveName, int unknown);
void func_020a56bc(Room *room, const char *archiveName, int unknown);
void func_020a5514(Room *room, int slot);
void func_020a5760(Room *room, int slot);
void func_020a4acc(RoomParts *parts, int slot);
void func_020a4a50(RoomParts *parts); // Draws them

// The scene manager
struct SceneManager {
    /* 0x0 */ Scene *mScene;
    /* 0x4 */ int mNextSceneId;
    /* 0x8 */ int mPreviousSceneId;
};
extern SceneManager gSceneManager;
void func_0203c02c(SceneManager *manager, int sceneId); // Asks for the next scene


} // extern "C"


extern "C" {
// --- Main program, not named yet ---

// Tests of a character's event (mEvent, 0x224); prelude.h has the others
BOOL func_02099d2c(u8 event); // The event is one with a partner (0x17, 0x19, ... 0x2d)
// Sets what the character is to the character with that index: mRelation[index] and the byte at 0xfc + index
void func_020990b0(SavedCharacter *character, int index, u8 relation, int affinity);
// Sets the character's mood from the result of an encounter, unless its mood is locked
void func_02098fe0(SavedCharacter *character, int result, int relation);
// Chooses the character's reaction (0..11) to its partner
int func_0209d5ac(SavedCharacter *character, s8 relationKind, s8 partnerReaction, SavedCharacter *partner);
// Resets the character to nothing, and sets its appearance (the scene does both when the saved game has no such one)
void func_020979e0(SavedCharacter *character);
void func_02098c40(SavedCharacter *character, int kind, int value);

// A distance multiplied by the character's 0x2af0
fx32 func_02091174(RoomCharacter *character, fx32 distance);
// The point at that distance in front of the character
FloorPos func_02091abc(RoomCharacter *character, fx32 distance);
void func_02090fdc(RoomCharacter *character, int param1, int param2, int param3);
// Moves the character and its model for this frame
void func_020910a8(RoomCharacter *character);

// The saved game's characters, by index
void func_0202c264(SavedGame *savedGame, int index, SavedCharacter *character); // Reads one
void func_0202c28c(SavedGame *savedGame, int index, SavedCharacter *character); // Writes one

// Copies the character's name out as a string
void func_0207a80c(SavedCharacter *character, wchar_t *name);
// The speech engine's state: 1 while it speaks
int Shtts_GetState(void *engine);
}


// ===================================================================================================================
// The overlay's data

// The rectangle of the floor the characters walk on, and its corners. The static initialiser 0x0216d67c computes
// the size and the corners from the origin. The corners are in .data although nothing of them is constant, which
// this compiler only does for a const object; and the initialiser reaches both through one pointer, which it does
// for the static objects of one file (it sorts them by size: these two, then HeldItem's sHeldItemDigits).
static FloorRect gWalkArea = { -FX32_CONST(2.5), -FX32_CONST(2.5), -gWalkArea.x * 2, -gWalkArea.z * 2 };
static const FloorPos gWalkAreaCorners[4] = {
    { -gWalkArea.x, -gWalkArea.z },
    { -gWalkArea.x, gWalkArea.z },
    { gWalkArea.x, gWalkArea.z },
    { gWalkArea.x, -gWalkArea.z },
};

// ===================================================================================================================
// RoomScene: loading, input, the building front's buttons, room changes and the events of the room

typedef char CheckSavedCharacterSize[sizeof(SavedCharacter) == 0x2C0 ? 1 : -1];
typedef char CheckRoomSceneSize[sizeof(RoomScene) == 0x1D7C8 ? 1 : -1];

// The archive of each kind of thing a character can hold, by number (data_ov074_0216d754, used by group a7)

// Counts the saved characters, chooses the two to show, and asks for the scene's archives.
void RoomScene::StartLoading() {
    u8 last;
    u8 building;
    u8 first;
    u8 second;

    mClock.Update();
    gSoundManager.StartSeqIfNotPlaying(0, 11);
    gSoundManager.SetSeqTrackMuteOnly(0, 0x1C1, TRUE);
    gSoundManager.SetSeqTrackModDepth(0, 0xFFFF, 0);
    func_020ad568(mClock.mSeconds);
    func_020ad514(mClock.mSeconds, &mClock.mTime);
    func_020ad294(mClock.mSeconds, &mClock.mTime);

    for (int i = 0; i < 100; i++) {
        if (func_0202c3dc(&gGameData, i)) {
            mResidents[mResidentCount] = i;
            mResidentCount++;
            mFront.mResidents[func_0202c414(&gGameData, i)] = i;
        }
    }

    last = gSceneMemory.mCharacter;
    if (last < 100 && func_0202c3dc(&gGameData, last)) {
        ChooseCharacters(last);
    } else {
        ChooseCharacters(0);
        gSceneMemory.mCharacter = mCharacters[0].mIndex;
    }
    LoadCharacters();
    func_02091048(&mCharacters[0], "3HM");
    func_02091048(&mCharacters[1], "BI1");
    mRoom.SetStyle(mCharacters[0].mInterior);
    mRoom.mRoomIndex = mCharacters[0].mRoomNumber;

    if (mSceneId == 0x11) {
        building = gGameData.mBuilding & 3;
    } else {
        building = mSceneId - 0x12;
    }
    mFront.SetBuildingSize(building);
    mSky.Init(&mClock, building);

    ArchiveManager_MountCompressed(&gArchiveManager, "MNU", "Scn/Scene/ScenePlayRoom2_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "BIC", "Mdl/Ica/ica_mii_room_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "EFT", "Mdl/Spl/SplRoomEffect_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TMM", "Msg/TmdMainRoomMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TEM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "ASP", "Scn/AFwBG/FwBgSkyPlt_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "APX", mFront.mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "ASX", mSky.mArchivePath, 0);
    func_020a54c4(&mRoom, "CO0", 0);
    func_020a56bc(&mRoom, "RM?", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[0].mArchiveName, mCharacters[0].mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[1].mArchiveName, mCharacters[1].mArchivePath, 0);

    GetEventItems(&first, &second);
    mItems[0].SetArchive("HI0", first);
    mItems[1].SetArchive("HI1", second);
    mItems[2].SetArchive("HI2", first);
    mItems[3].SetArchive("HI3", second);
    for (int i = 0; i < 4; i++) {
        HeldItem *item = &mItems[i];
        ArchiveManager_MountCompressed(&gArchiveManager, item->mArchiveName, item->mArchivePath, 0);
    }
}

// Which two things the resident holds for its event (the numbers of their archives), 0 for none.
void RoomScene::GetEventItems(u8 *first, u8 *second) {
    u32 event = mCharacters[0].mEvent;

    *first = 0;
    *second = 0;
    switch (event) {
    case 1:
    case 0x22:
        *first = 9;
        break;
    case 4:
        *first = 6;
        break;
    case 5:
    case 0x2A:
        *first = 2;
        *second = 3;
        break;
    case 8:
    case 0x26:
        *first = 7;
        break;
    case 9:
    case 0x20:
        *first = 1;
        break;
    case 10:
    case 0x1E:
        *first = 8;
        break;
    case 12:
    case 0x24:
        *first = 9;
        *second = 5;
        break;
    case 16:
    case 0x28:
        *first = 0;
        break;
    case 18:
        *first = 10;
        break;
    }
}

// Makes the saved character with that index the room's resident, and chooses the second character: the partner
// of the resident's event, or else the first other character there is.
void RoomScene::ChooseCharacters(int resident) {
    int other;

    if (mResidentCount == 0) {
        mCharacters[0].mIndex = 0;
        mCharacters[1].mIndex = 1;
    } else if (mResidentCount == 1) {
        mCharacters[0].mIndex = resident;
        if (resident != 1) {
            mCharacters[1].mIndex = 1;
        } else {
            mCharacters[1].mIndex = 2;
        }
    } else {
        if (func_02099cec(func_0202c468(&gGameData, resident))) {
            other = func_0202c490(&gGameData, resident);
        } else {
            other = mResidents[0];
            if (other == resident) {
                other = mResidents[1];
            }
        }
        mCharacters[0].mIndex = resident;
        mCharacters[1].mIndex = other;
    }
}

// Sets up the screens, the room, the two characters and the touch screen, when the archives are loaded.
// @0x0214a02c
void RoomScene::Init() {
    int i;
    void *file;
    void *animations;
    int charOffset;

    func_0202b0e8(&data_02122824);
    func_0202ae38(data_021223e4);
    data_0211d604.mUnknown48 = 1;
    data_0211d604.mUnknownEC = 1;
    G3X_SetClearColor(GX_RGB(1, 1, 1), 0, 0x7FFF, 63, FALSE);
    func_0202680c(0x4000);

    ObjResourceManager_Load(&gObjResourceManager, "MNU:/Obj_down.nce", "MNU:/Obj_down.ncg", 0x1000, 0, -1);
    func_02027534(&data_021213b8, "MNU:/Obj_down.ncl", 0x1000, -1, -1, -1);
    ObjResourceManager_Load(&gObjResourceManager, "MNU:/Obj_up.nce", "MNU:/Obj_up.ncg", 0x10, 1, -1);
    func_02027534(&data_021213b8, "MNU:/Obj_up.ncl", 0x10, -1, -1, -1);
    mFront.mScreen = (u16 *)func_02027d7c(&data_02121834, "APX:/BG_window.nsc", 0x200, 0, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, "APX:/BG_wall.nsc", 0x400, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "APX:/BG_down.ncg", 0x600, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "APX:/BG_down.ncl", 0x600, -1, -1, -1);
    func_02027d7c(&data_02121834, "MNU:/BG_dialogue.nsc", 2, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "MNU:/BG_sub.ncg", 2, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "MNU:/BG_sub.ncl", 2, -1, -1, -1);
    mSky.mColors = (u16 *)func_02027534(&data_021213b8, "ASP:/Fw_BG_sky.ncl", 0x2F00, 0, 0, 2);
    func_02028f24(&gVramTransferQueue, mSky.mCharPath, 0x400, 0, 0x1000, 0x3000, 0);
    mSky.mPalette = func_02027534(&data_021213b8, mSky.mPalettePath, 0x400, 0x80, 0x80, 0xC0);
    func_020278f8(&data_021213b8, 0, 1);
    func_02028280(&data_02121834, 1, 2);
    func_02028280(&data_02121834, 2, 1);
    func_02028108(&data_02121834, 2, 0);
    func_02028154(&data_02121834, 2, 0);
    func_02028108(&data_02121834, 0x200, 0);
    func_02028154(&data_02121834, 0x200, -0xC0);
    func_02028108(&data_02121834, 0x400, 0);
    func_02028154(&data_02121834, 0x400, -0xC0);
    data_0211d604.Set(0x1600);
    data_0211d604.Show(0x11);
    func_02023ee0(&data_0211d604, 2, 0x11, 8, 8);
    SetInputState(INPUT_IDLE);

    file = ArchiveManager_GetFileByName(&gArchiveManager, "EFT:/SplRoomEffect.spa");
    func_02080e98(&mParticles, file, 3, 0x50, 0x15, 0x16, 0x1E);
    func_02080ee4(&mParticles);
    mIcon[0].Load(&mParticles, 0);
    mRoom.Refresh();
    func_020a5514(&mRoom, 7);
    func_020a5760(&mRoom, 8);
    func_020a4acc(&mRoom.mParts, 10);
    mRoom.mParts.mDoor.mUnknown13C = 2;
    for (i = 0; i < 4; i++) {
        HeldItem *item = &mItems[i];
        func_0202ad04(data_021223e4, i + 2, item->mArchiveName, 0);
        item->Init(i + 2);
    }
    mTtsPlayer.InitEngine();

    func_0202ad04(data_021223e4, 0, mCharacters[0].mArchiveName, 0);
    func_0202ad04(data_021223e4, 1, mCharacters[1].mArchiveName, 0);
    mCharacters[0].Init(0, 0, 0);
    mCharacters[1].Init(1, 0, 1);
    mCharacters[0].mPartner = &mCharacters[1];
    mCharacters[1].mPartner = &mCharacters[0];
    mCharacters[0].mHeldItems[0] = &mItems[0];
    mCharacters[0].mHeldItems[1] = &mItems[1];
    mCharacters[1].mHeldItems[0] = &mItems[2];
    mCharacters[1].mHeldItems[1] = &mItems[3];
    mCharacters[0].mSoundHandle = 4;
    mCharacters[1].mSoundHandle = 5;
    mCharacters[0].mPresent = FALSE;
    mCharacters[1].mPresent = FALSE;
    animations = func_0202ad04(data_021223e4, 6, "BIC", 0);
    for (int i = 0; i < 2; i++) {
        LoadCharacter(&mCharacters[i]);
        func_0208fefc(&mCharacters[i]);
        func_02091a24(&mCharacters[i]);
        mCharacters[i].mPersonality = func_02098f2c(&mCharacters[i]);
        mCharacters[i].ResetBody();
        func_02091a88(&mCharacters[i]);
        mCharacters[i].CopyName();
        mCharacters[i].mTtsPlayer = &mTtsPlayer;
        mCharacters[i].RefreshSpeechTexts();
        func_02029660((void *)mCharacters[i].mModel.mUnknown04, RESOURCE_BCA0, animations);
        mCharacters[i].SetState(1);
        mCharacters[i].LoadHeadIcon(&mParticles);
        mCharacters[i].mRoom = &mRoom;
        mCharacters[i].mCamera = &mCamera;
    }
    PlaceCharacters();
    StartCharacterEvents();

    mCamera.Reset();
    mCamera.mCharacters[0] = &mCharacters[0];
    mCamera.mCharacters[1] = &mCharacters[1];
    mCamera.mRoom = &mRoom;
    mLight.Init(&mCamera, &mRoom);
    AimCamera();
    StartRoomEvent();

    mRoomMessages.Open("TMM:/tmd_mainroom_msg_JP.bmg");
    mEtcMessages.Open("TEM:/tmd_etc_msg_JP.bmg");
    charOffset = 0xC0;
    mMessage.InitObjForMessage(&mRoomMessages, 3, &gFonts[0].mFont, 0x10, charOffset, 0, 1, 0, 0, 15, NULL);
    mMessage.mSprite.SetPosition(0x10000, 0xA0000);
    mMessage.mVAlign = 1;
    charOffset += GetCanvasCharCount(&mMessage);
    if (mFront.mResidents[mCharacters[0].mRoomNumber] != 0xFF) {
        ShowEventMessage();
    }
    mNamePanel.SetNames(mRoom.mNumberText, mCharacters[0].mNameText);
    mNamePanel.Init(charOffset, &mRoomMessages, &mRoomMessages);

    mButtonLabel[0].InitObjForMessage(&mEtcMessages, 0xB3, &gFonts[0].mFont, 0x1000, 0x300, 0, 1, 0, 0, 15, NULL);
    mButtonLabel[0].mVAlign = 1;
    mButtonLabel[0].mShadowDirections = 0xAA;
    mButtonLabel[0].DrawAll();
    mUnknownClearedByInit = 0;
    mButtons[0].mLabel = &mButtonLabel[0];
    for (s16 j = 0; j < 2; j++) {
        mButtons[j].Init(j);
    }
    mFront.Init(mCharacters[0].mRoomNumber);
    mSky.Refresh();
    mClockDisplay.Init(&mClock);

    // Coming from the character distribution scene with a resident that has just moved in
    if (gSceneManager.mPreviousSceneId == 0x74 && GetEventEnd(&mCharacters[0]) == 0x7FFFFFFFFFFFFFFFLL) {
        SetInputState(INPUT_OFF);
        SetEventState(EVENT_AFTER_DISTRIBUTION, 60);
        mCamera.SetState(2, 0);
        mMessage.SetState(4);
    }
    UpdateButtons();
    mMessage.Update();
    NNS_GfdDumpLnkTexVramManager();
    func_02060c1c();
    gSoundManager.LoadGroup(0x4C);
}

void RoomScene::Update() {
    if ((data_0211f9d4.mHeld & 0x2FFF) != 0 || gTouchInput.mTouching != 0) {
        mIdleFrames = 0;
        mCamera.SetState(4, 0x474);
    }
    if (data_0211f9d4.mHeld & PAD_KEY_RIGHT) {
        mCamera.mInput.y = 0x10000;
    } else if (data_0211f9d4.mHeld & PAD_KEY_LEFT) {
        mCamera.mInput.y = -0x10000;
    }
    if (data_0211f9d4.mHeld & PAD_KEY_UP) {
        mCamera.mInput.x = 0xE000;
    } else if (data_0211f9d4.mHeld & PAD_KEY_DOWN) {
        mCamera.mInput.x = -0xE000;
    }
    UpdateInput();
    UpdateEvent();
    UpdateObjects();
}

#define ABS(x) ((x) < 0 ? -(x) : (x))

void RoomScene::Draw() {
    switch (mFadeState) {
    case 0:
        break;
    case 1:
        mFadeStep++;
        if (mFadeStep >= 10 || mPhase > PHASE_FADE_IN) {
            SetFadeState(0);
        } else if (mFadeStep >= 6) {
            if (mFadeStep == 6) {
                func_02023c54(&data_0211d604, 0x3F3F, 0x2AE1);
            }
            UpdateObjects();
        } else {
            func_02023be8(&data_0211d604, 0x3F3F, 0x10000);
            UpdateObjects();
        }
        break;
    case 2:
        mFadeStep++;
        if (ABS(func_02023c44(&data_0211d604, 0x3F)) > 10) {
            SetFadeState(0);
        } else if (mFadeStep == 1) {
            func_02023c54(&data_0211d604, 0x3F3F, 0x2000);
        } else if (mFadeStep > 1) {
            UpdateObjects();
        }
        break;
    }

    func_020a4a50(&mRoom.mParts);
    for (int i = 0; i < 2; i++) {
        mCharacters[i].UpdateBodyIfPresent();
    }
    for (int i = 0; i < 4; i++) {
        mItems[i].Draw();
    }
    func_02080f00(&mParticles, 0);
    mMessage.Draw();
    mButtonLabel[0].Draw();
    mNamePanel.Draw();
    for (int i = 0; i < 2; i++) {
        mButtons[i].Draw();
    }
    mFront.Draw();
    mClockDisplay.Draw();
}

void RoomScene::Exit() {
    func_02023eac(&data_0211d604, 0x3F);
    FinishEncounter();
    SaveCharacters();
    func_0202683c();
    func_0202aecc(data_021223e4, 0);
    func_0202aecc(data_021223e4, 1);
    func_0202aecc(data_021223e4, 2);
    func_0202aecc(data_021223e4, 3);
    func_0202aecc(data_021223e4, 4);
    func_0202aecc(data_021223e4, 5);
    func_0202aecc(data_021223e4, 6);
    func_020a5948(&mRoom);
    ArchiveManager_Unmount(&gArchiveManager, mCharacters[0].mArchiveName);
    ArchiveManager_Unmount(&gArchiveManager, "MNU");
    ArchiveManager_Unmount(&gArchiveManager, "BIC");
    ArchiveManager_Unmount(&gArchiveManager, "EFT");
    ArchiveManager_Unmount(&gArchiveManager, "TMM");
    ArchiveManager_Unmount(&gArchiveManager, "TEM");
    ArchiveManager_Unmount(&gArchiveManager, "APX");
    ArchiveManager_Unmount(&gArchiveManager, "ASP");
    ArchiveManager_Unmount(&gArchiveManager, "ASX");
    ArchiveManager_Unmount(&gArchiveManager, mCharacters[1].mArchiveName);
    for (int i = 0; i < 4; i++) {
        HeldItem *item = &mItems[i];
        ArchiveManager_Unmount(&gArchiveManager, item->mArchiveName);
    }
}

// Reads the clock and updates everything in the room and on the touch screen.
void RoomScene::UpdateObjects() {
    mClock.UpdateIfArchiveIdle();
    func_0207b138(mParticles.mManager);
    for (int i = 0; i < 2; i++) {
        mCharacters[i].Update();
    }
    for (int i = 0; i < 4; i++) {
        mItems[i].Update();
    }
    mIcon[0].Update();
    mCamera.Update();
    mRoom.Update();
    mLight.Update();
    mFront.Update();
    mSky.Update();
    mMessage.Update();
    mNamePanel.Update();
    for (int i = 0; i < 2; i++) {
        mButtons[i].Update();
    }
    mClockDisplay.Update();
    mTtsPlayer.Update();
}

// Puts the two characters at random places, on opposite sides of the room's middle, facing at random.
void RoomScene::PlaceCharacters() {
    fx32 radius = (MATH_Rand16(&gRandom, 9) << 9) + 0x1000;
    fx32 otherRadius = 0x3000 - radius;
    if (otherRadius > 0x2000) {
        otherRadius = 0x2000;
    }

    u16 angle = MATH_Rand16(&gRandom, 0x100) << 8;
    mCharacters[0].mModel.mPos.x = FX_Mul(radius, FX_CosIdx(angle));
    mCharacters[0].mModel.mPos.z = FX_Mul(radius, FX_SinIdx(angle));

    angle = angle + 0x4000 + (MATH_Rand16(&gRandom, 0x100) << 7);
    mCharacters[1].mModel.mPos.x = FX_Mul(otherRadius, FX_CosIdx(angle));
    mCharacters[1].mModel.mPos.z = FX_Mul(otherRadius, FX_SinIdx(angle));

    // The directions go through an int: that is what leaves two conversions to u16 behind
    int direction = MATH_Rand16(&gRandom, 0);
    Character_SetAngle(&mCharacters[0], direction);
    direction = MATH_Rand16(&gRandom, 0);
    Character_SetAngle(&mCharacters[1], direction);
}

// Starts what the resident's event has the characters do, unless the resident is out.
void RoomScene::StartCharacterEvents() {
    u8 event = mCharacters[0].mEvent;

    if (func_02099c94(event)) {
        mCharacters[0].mPresent = FALSE;
        return;
    }
    if (func_02099db8(event)) {
        StartPairEvent(1);
        mCharacters[0].mPresent = TRUE;
        mCharacters[1].mPresent = TRUE;
        return;
    }
    StartEvent(&mCharacters[0], 1);
    mCharacters[0].mPresent = TRUE;
}

// Points the camera at as many characters as are in the room.
void RoomScene::AimCamera() {
    int count = CountPresent();

    if (count == 0) {
        mCamera.SetMode(1);
        mLight.SetState(3);
    } else if (count == 1) {
        mCamera.SetMode(2);
        if (mCharacters[0].mEvent == 18) {
            mLight.SetState(3);
        } else {
            mLight.SetState(1);
        }
    } else {
        mCamera.SetMode(3);
        mLight.SetState(1);
    }
}

// Starts the event state for as many characters as are in the room.
void RoomScene::StartRoomEvent() {
    int count = CountPresent();

    if (count == 0) {
        SetEventState(EVENT_NOBODY_HOME, -1);
    } else if (count == 1) {
        SetEventState(EVENT_ALONE, -1);
    } else {
        SetEventState(EVENT_TOGETHER, -1);
    }
}

// Does what a button is for, when the stylus is let go on it.
void RoomScene::PressButton(int button) {
    switch (mButtons[button].mKind) {
    case 0:
        EnterRoom();
        break;
    case 1:
        SetInputState(INPUT_WAIT_LEAVE);
        SetEventState(EVENT_NONE);
        mTtsPlayer.Stop();
        mNextSceneId = 0x5F;
        gSceneMemory.mCharacter = mCharacters[0].mIndex;
        break;
    }
}

BOOL RoomScene::IsTouchEnabled() {
    switch (mInputState) {
    case INPUT_OFF:
        return FALSE;
    }
    switch (mEventState) {
    case EVENT_CHANGE_ROOM:
        return FALSE;
    }
    return TRUE;
}

BOOL RoomScene::CanEnterRoom() {
    if (!IsTouchEnabled()) {
        return FALSE;
    }
    switch (mEventState) {
    case EVENT_RESIDENT_ARRIVES:
    case EVENT_VISITOR_ARRIVES:
    case EVENT_LAST_LEAVES:
    case EVENT_VISITOR_LEAVES:
        return FALSE;
    }
    return TRUE;
}

// Leaves for the scene of the resident's room (scene 0x40), if there is somebody in.
void RoomScene::EnterRoom() {
    if (CountPresent() == 0) {
        return;
    }
    if (!CanEnterRoom()) {
        return;
    }
    if (mCharacters[0].mEvent == 18 || mCharacters[0].mEvent == 15 || mCharacters[0].mEvent == 17) {
        SetInputState(INPUT_WAIT_LEAVE);
    } else {
        SetInputState(INPUT_ENTER_ROOM);
        gSoundManager.StartSeqArc(2, 9, 0);
    }
    SetEventState(EVENT_NONE);
    mTtsPlayer.Stop();
    mNextSceneId = 0x40;
}

// Leaves for the information scene (scene 0xAE).
void RoomScene::GoToInformation() {
    SetInputState(INPUT_WAIT_LEAVE);
    SetEventState(EVENT_NONE);
    mTtsPlayer.Stop();
    mNextSceneId = 0xAE;
    gSceneMemory.mCharacter = mCharacters[0].mIndex;
}

// Enables the two buttons as the state allows.
void RoomScene::UpdateButtons() {
    if (!IsTouchEnabled()) {
        mButtons[1].SetReleased(FALSE);
    } else {
        mButtons[1].SetReleased(TRUE);
    }
    if (!CanEnterRoom() || CountPresent() == 0) {
        if (mButtons[0].SetReleased(FALSE)) {
            mButtons[0].mOverwritePriority = 11;
        }
    } else {
        if (mButtons[0].SetReleased(TRUE)) {
            mButtons[0].mOverwritePriority = -1;
        }
    }
}

void RoomScene::SetInputState(int state) {
    mInputState = state;
    mInputStep = 0;
}

// The touch screen: the two buttons, the building panel and its information button, and leaving the scene.
void RoomScene::UpdateInput() {
    if (mInputState != INPUT_WAIT_LEAVE && mInputState != INPUT_LEAVING) {
        UpdateButtons();
    }

    switch (mInputState) {
    case INPUT_OFF:
        break;

    case INPUT_IDLE:
        if (gTouchInput.mTouched) {
            ScreenPoint point = gTouchInput.mPoint;
            BOOL handled = FALSE;

            for (int i = 0; i < 2; i++) {
                if (mButtons[i].Contains(point)) {
                    handled = TRUE;
                    if (mButtons[i].mReleased) {
                        SetInputState(INPUT_TOUCH_BUTTON);
                        mPressedButton = i;
                        mButtons[i].mState = 1;
                        gSoundManager.StartSeqArc(2, 1, 0);
                    }
                    break;
                }
            }
            if (!handled) {
                if (mFront.mCursor.Contains(point)) {
                    if (mButtons[0].mReleased) {
                        EnterRoom();
                        handled = TRUE;
                    }
                } else if (mFront.mButton.Contains(point) && mButtons[1].mReleased) {
                    SetInputState(INPUT_INFO_BUTTON);
                    gSoundManager.StartSeqArc(2, 1, 0);
                    handled = TRUE;
                }
            }
            if (!handled) {
                SetInputState(INPUT_PANEL);
            }
        } else {
            if (gTouchInput.mTouching != 0) {
                return;
            }
            // After a minute of nothing, the panel goes to a random resident's room
            mIdleFrames++;
            if (mIdleFrames > 3600 && mResidentCount >= 2) {
                int count = mResidentCount;
                int room = func_0202c414(&gGameData, mResidents[MATH_Rand16(&gRandom, count)]);
                if ((u16)room != mFront.mSelectedRoom) {
                    mIdleFrames = 0;
                    mCamera.SetState(4, 0x474);
                    mFront.SelectRoom(room, -1);
                }
            }
            ChangeRoom();
        }
        break;

    case INPUT_TOUCH_BUTTON:
        if (gTouchInput.mTouching != 0) {
            if (mButtons[mPressedButton].Contains(gTouchInput.mPoint)) {
                mButtons[mPressedButton].mState = 1;
            } else {
                mButtons[mPressedButton].mState = 2;
            }
        } else {
            if (mButtons[mPressedButton].Contains(gTouchInput.mLastPoint) &&
                data_0211f9d4.mUnknown20) {
                mButtons[mPressedButton].mState = 3;
                PressButton(mPressedButton);
            } else {
                SetInputState(INPUT_IDLE);
                mButtons[mPressedButton].mState = 2;
            }
        }
        break;

    case INPUT_INFO_BUTTON:
        if (gTouchInput.mTouching != 0) {
            if (mFront.mButton.Contains(gTouchInput.mPoint)) {
                mFront.mButton.SetState(1);
            } else {
                mFront.mButton.SetState(2);
            }
        } else {
            if (mFront.mButton.Contains(gTouchInput.mLastPoint) && data_0211f9d4.mUnknown20) {
                mFront.mButton.SetState(3);
                GoToInformation();
            } else {
                SetInputState(INPUT_IDLE);
                mFront.mButton.SetState(2);
            }
        }
        break;

    case INPUT_PANEL:
        if (gTouchInput.mTouching != 0) {
            mFront.Touch(gTouchInput.mPoint);
            ChangeRoom();
        } else {
            SetInputState(INPUT_IDLE);
        }
        break;

    case INPUT_ENTER_ROOM:
        if (mInputStep == 0) {
            mInputStep++;
            mCharacters[0].SetState(0x113);
            if (mCharacters[1].mPresent) {
                mCharacters[1].SetState(0x113);
            }
            mCamera.SetState(0, 0);
            mCamera.mIgnoreInput = TRUE;
            mLight.SetState(2);
            mIcon[0].SetMode(1);
        } else if (mInputStep == 1) {
            if (mCharacters[0].mState != 0) {
                return;
            }
            mInputStep++;
            mCharacters[0].SetState(0x114);
        } else if (mInputStep == 2) {
            if (mCharacters[0].mState != 0) {
                return;
            }
            mInputStep++;
            if (CountPresent() >= 2) {
                mCharacters[0].SetState(0x116);
            } else {
                mCharacters[0].SetState(0x115);
            }
            // The resident walks half of the way to where the camera is
            VecFx32 target = mCamera.mPosition;
            VEC_Subtract(&target, &mCharacters[0].mModel.mPos, &target);
            target.x >>= 1;
            target.y >>= 1;
            target.z >>= 1;
            VEC_Set(&mCharacters[0].mTarget, target.x, 0, target.z);
        } else {
            if (mCharacters[0].mState != 0) {
                return;
            }
            SetInputState(INPUT_WAIT_LEAVE);
        }
        break;

    case INPUT_WAIT_LEAVE:
        if (!ArchiveManager_IsIdle(&gArchiveManager)) {
            return;
        }
        if (!IsTransitionDone()) {
            return;
        }
        SetInputState(INPUT_LEAVING);
        func_0203c02c(&gSceneManager, mNextSceneId);
        SetFadeState(2);
        if (mNextSceneId == 0x5F) {
            gSoundManager.StartSeqArc(2, 1, 13);
        } else if (mNextSceneId == 0xAE) {
            gSoundManager.StartSeqArc(2, 1, 10);
        } else {
            gSoundManager.StartSeqArc(2, 1, 11);
        }
        break;

    case INPUT_LEAVING:
        break;
    }
}

// Shows the room that was chosen on the building panel, if it is another: the characters are saved, the new
// room's are chosen and loaded, and everything is told to disappear and come back (EVENT_CHANGE_ROOM).
void RoomScene::ChangeRoom() {
    int i;
    u8 resident;

    if (mFront.mSelectedRoom == mRoom.mRoomIndex) {
        return;
    }
    if (!ArchiveManager_IsIdle(&gArchiveManager)) {
        return;
    }
    resident = mFront.mResidents[mFront.mSelectedRoom & 0xFF];
    mTtsPlayer.Stop();
    FinishEncounter();
    SaveCharacters();

    for (i = 0; i < 2; i++) {
        if (func_0202c3dc(&gGameData, mCharacters[i].mIndex)) {
            mFront.DrawWindow(mCharacters[i].mRoomNumber);
            if (func_02099d1c(&mCharacters[i])) {
                mFront.DrawWindow(func_0202c414(&gGameData, mCharacters[i].mEventPartner));
            }
        }
    }

    mRoom.mRoomIndex = mFront.mSelectedRoom;
    if (resident == 0xFF) {
        ChooseCharacters(0);
        LoadCharacters();
        mRoom.SetNextStyle(8);
        mNamePanel.SetState(2);
    } else {
        ChooseCharacters(resident);
        gSceneMemory.mCharacter = resident;
        LoadCharacters();
        mRoom.SetNextStyle(mCharacters[0].mInterior);
        mNamePanel.SetState(1);
    }

    mCharacters[0].mPresent = FALSE;
    mCharacters[1].mPresent = FALSE;
    mRoom.SetState(1);
    mCharacters[0].SetOverrideState(1);
    mCharacters[1].SetOverrideState(1);
    for (int i = 0; i < 4; i++) {
        mItems[i].SetLoadState(1);
    }
    SetEventState(EVENT_CHANGE_ROOM, 0);
    mIdleFrames = 0;
    mCamera.SetState(0, 0);
    mMessage.SetState(4);
    data_0211d604.Hide(1);
    mClockDisplay.SetState(1);
    mIcon[0].SetMode(1);
}

void RoomScene::SetEventState(int state) {
    mEventState = state;
    mEventStep = 0;
}

void RoomScene::SetEventState(int state, int timer) {
    SetEventState(state);
    mEventTimer = timer;
}

// Starts the event that is due for the saved character under mScanIndex. A character that is not one of the two
// in the room is loaded into `temp` for it. Returns the character.
SavedCharacter *RoomScene::StartDueEvent(s64 now, SavedCharacter *temp) {
    int events[2];
    SavedCharacter *first;
    SavedCharacter *second;

    SaveCharacters();
    for (int i = 0; i < 2; i++) {
        events[i] = mCharacters[i].mEvent;
    }

    if (mScanIndex == mCharacters[0].mIndex) {
        temp = &mCharacters[0];
        first = &mCharacters[1];
        second = NULL;
    } else if (mScanIndex == mCharacters[1].mIndex) {
        temp = &mCharacters[1];
        first = &mCharacters[0];
        second = NULL;
    } else {
        temp->mIndex = mScanIndex;
        LoadCharacter((RoomCharacter *)temp);
        temp->mPersonality = func_02098f2c(temp);
        first = &mCharacters[0];
        second = &mCharacters[1];
    }
    if (mResidentCount == 1) {
        second = NULL;
        first = second;
    }
    func_02099c28(temp, now, &mClock.mTime, first, second);

    for (int i = 0; i < 2; i++) {
        mCharacters[i].mPrevEvent = events[i];
    }
    LoadCharacters();
    return temp;
}

// Ends the event of the saved character under mScanIndex, whose time is up. Returns the index of the character
// it leaves its mood to, or -1.
int RoomScene::EndDueEvent() {
    int events[2];
    SavedCharacter *character;
    int partnerIndex;
    SavedCharacter *partner;

    SaveCharacters();
    for (int i = 0; i < 2; i++) {
        events[i] = mCharacters[i].mEvent;
    }
    SavedCharacter temp;

    if (mScanIndex == mCharacters[0].mIndex) {
        character = &mCharacters[0];
    } else if (mScanIndex == mCharacters[1].mIndex) {
        character = &mCharacters[1];
    } else {
        character = &temp;
        temp.mIndex = mScanIndex;
        LoadCharacter((RoomCharacter *)character);
        temp.mPersonality = func_02098f2c(character);
    }

    partnerIndex = -1;
    partner = NULL;
    if (character->mMoodLocked != 0 && func_0209a444(character) && mResidentCount > 1) {
        partnerIndex = character->mMoodPartner;
        if (partnerIndex == mCharacters[0].mIndex) {
            partner = &mCharacters[0];
        } else if (partnerIndex == mCharacters[1].mIndex) {
            partner = &mCharacters[1];
        }
    }
    func_0209a588(character, mClock.mSeconds, partner);

    for (int i = 0; i < 2; i++) {
        mCharacters[i].mPrevEvent = events[i];
    }
    LoadCharacters();
    return partnerIndex;
}

// For the saved character under mScanIndex when no event of its is due (func_020ad348 says when).
void RoomScene::UpdateAbsentCharacter(s64 now) {
    int events[2];
    SavedCharacter *character;

    SaveCharacters();
    for (int i = 0; i < 2; i++) {
        events[i] = mCharacters[i].mEvent;
    }
    SavedCharacter temp;

    if (mScanIndex == mCharacters[0].mIndex) {
        character = &mCharacters[0];
    } else if (mScanIndex == mCharacters[1].mIndex) {
        character = &mCharacters[1];
    } else {
        character = &temp;
        temp.mIndex = mScanIndex;
        LoadCharacter((RoomCharacter *)character);
        temp.mPersonality = func_02098f2c(character);
    }
    func_0209b14c(character, now);

    for (int i = 0; i < 2; i++) {
        mCharacters[i].mPrevEvent = events[i];
    }
    LoadCharacters();
}

// Once in each round of the saved characters: gives a random one of those that can have it an event. Returns its
// index, or -1.
int RoomScene::StartRandomEvent(s64 now) {
    int events[2];
    int force;
    int result;

    SaveCharacters();
    result = -1;
    for (int i = 0; i < 2; i++) {
        events[i] = mCharacters[i].mEvent;
    }

    if (func_020adb44(&force) > 0) {
        u16 indices[100];
        int count = func_020adc08(indices);
        u16 index = indices[MATH_Rand16(&gRandom, count)];
        SavedCharacter *character;
        SavedCharacter *partner = NULL;
        SavedCharacter temp;
        SavedCharacter partnerTemp;

        if (index == mCharacters[0].mIndex) {
            character = &mCharacters[0];
        } else if (index == mCharacters[1].mIndex) {
            character = &mCharacters[1];
        } else {
            temp.mIndex = index;
            LoadCharacter((RoomCharacter *)&temp);
            temp.mPersonality = func_02098f2c(&temp);
            character = &temp;
        }

        if (func_02099cdc(character)) {
            u8 partnerIndex = character->mEventPartner;
            if (partnerIndex == mCharacters[0].mIndex) {
                partner = &mCharacters[0];
            } else if (partnerIndex == mCharacters[1].mIndex) {
                partner = &mCharacters[1];
            } else {
                partnerTemp.mIndex = partnerIndex;
                LoadCharacter((RoomCharacter *)&partnerTemp);
                partnerTemp.mPersonality = func_02098f2c(&partnerTemp);
                partner = &partnerTemp;
            }
        }

        if (func_0209b208(character, now, force, partner, 0)) {
            SaveCharacter((RoomCharacter *)character);
            if (partner != NULL) {
                SaveCharacter((RoomCharacter *)partner);
            }
            result = character->mIndex;
        }
    }

    for (int i = 0; i < 2; i++) {
        mCharacters[i].mPrevEvent = events[i];
    }
    LoadCharacters();
    return result;
}

// Whether the room, the characters, the window view and the held things have all finished appearing or
// disappearing.
BOOL RoomScene::IsTransitionDone() {
    if (mRoom.mState != 0 || mCharacters[0].mOverrideState != 0 || mCharacters[1].mOverrideState != 0) {
        return FALSE;
    }
    if (mSky.mState == 2) {
        return FALSE;
    }
    for (int i = 0; i < 4; i++) {
        if (mItems[i].mLoadState != 0) {
            return FALSE;
        }
    }
    return TRUE;
}

// Starts a transition on the room, then on each character, then on the held things, each when the one before is
// done (mEventTimer counts the steps), and returns whether all are done.
BOOL RoomScene::StepTransition(int transition) {
    if (mEventTimer == 0) {
        if (IsTransitionDone()) {
            mEventTimer++;
            mRoom.SetState(transition);
        }
    } else if (mEventTimer == 1) {
        if (IsTransitionDone()) {
            mEventTimer++;
            mCharacters[0].SetOverrideState(transition);
        }
    } else if (mEventTimer == 2) {
        if (IsTransitionDone()) {
            mEventTimer++;
            mCharacters[1].SetOverrideState(transition);
        }
    } else if (mEventTimer == 3) {
        if (IsTransitionDone()) {
            mEventTimer++;
            for (int i = 0; i < 4; i++) {
                mItems[i].SetLoadState(transition);
            }
        }
    } else if (mEventTimer == 4) {
        if (IsTransitionDone()) {
            return TRUE;
        }
    }
    return FALSE;
}

// The numbers of the messages for what comes of an encounter, by what RoomCharacter::GetFlagOfPartner says of the
// resident: when it starts (0x0216d570) and when it ends (0x0216d59c).
extern "C" {
extern const int gEncounterMessages[];
extern const int gEncounterEndMessages[];
}

// What goes on in the room, each frame: the switch on mEventState. The states of a room with nobody, one or two
// characters in it (3 to 6) also look at one saved character each frame (mScanIndex), to start and end the events
// of all of them as the clock says, and at the end of each round give one of them a random event.
// @0x0214c4e8
void RoomScene::UpdateEvent() {
    mEventFrames++;

    switch (mEventState) {
    case EVENT_NONE:
        break;

    case EVENT_CHANGE_ROOM: {
        u8 resident = mFront.mResidents[mRoom.mRoomIndex & 0xFF];

        if (mEventStep == 0) {
            if (StepTransition(2)) {
                mEventStep++;
                mEventTimer = 0;
            }
        } else if (mEventStep == 1) {
            if (StepTransition(3)) {
                mEventStep++;
            }
        } else if (mEventStep == 2) {
            mEventStep++;
            func_02091a14(&mCharacters[0], 1);
            func_02091a14(&mCharacters[1], 1);
            func_020a4b74(&mRoom.mParts, 1);
            if (resident != 0xFF) {
                mCharacters[0].SetState(1);
                mCharacters[0].mWait = 0;
                mCharacters[1].SetState(1);
                mCharacters[1].mWait = 0;
                PlaceCharacters();
                StartCharacterEvents();
            }
            AimCamera();
        } else if (mEventStep == 3) {
            mEventStep++;
            mEventTimer = 4;
            func_020a4ab8(&mRoom.mParts, 3);
            mCamera.SetState(1, 0);
        } else if (mEventStep == 4) {
            if (!IsTransitionDone()) {
                break;
            }
            if (--mEventTimer > 0) {
                break;
            }
            if (resident == 0xFF) {
                SetEventState(EVENT_EMPTY_ROOM, -1);
            } else {
                StartRoomEvent();
                ShowEventMessage();
            }
            data_0211d604.Show(1);
        }
        break;
    }

    case EVENT_CHOOSE:
        ChooseEncounter();
        break;

    case EVENT_EMPTY_ROOM: {
        s64 now = mClock.mSeconds;

        if (mScanIndex >= 100) {
            mScanIndex = 0;
            int index = StartRandomEvent(now);
            if (index >= 0) {
                mFront.DrawWindow(func_0202c414(&gGameData, index));
            }
        } else {
            if (func_0202c3dc(&gGameData, mScanIndex)) {
                if (now >= func_0202c4b8(&gGameData, mScanIndex)) {
                    SavedCharacter temp;
                    StartDueEvent(now, &temp);
                    if (func_02099d2c(func_0202c468(&gGameData, mScanIndex))) {
                        mFront.DrawWindow(func_0202c414(&gGameData, func_0202c490(&gGameData, mScanIndex)));
                    }
                } else if (now >= func_0202c4e4(&gGameData, mScanIndex)) {
                    int partner = EndDueEvent();
                    if (partner >= 0) {
                        mFront.DrawWindow(func_0202c414(&gGameData, partner));
                    }
                } else if (func_020ad348(mScanIndex, now)) {
                    UpdateAbsentCharacter(now);
                }
                mFront.DrawWindow(func_0202c414(&gGameData, mScanIndex));
            }
            mScanIndex++;
        }
        break;
    }

    case EVENT_NOBODY_HOME:
        if (mEventStep == 0) {
            mEventStep++;
            mEventTimer = 60;
        } else {
            s64 now = mClock.mSeconds;

            if (mScanIndex >= 100) {
                mScanIndex = 0;
                int index = StartRandomEvent(now);
                if (index >= 0) {
                    mFront.DrawWindow(func_0202c414(&gGameData, index));
                }
            } else {
                if (func_0202c3dc(&gGameData, mScanIndex)) {
                    if (now >= func_0202c4b8(&gGameData, mScanIndex)) {
                        SavedCharacter temp;
                        StartDueEvent(now, &temp);
                        // The resident's new event has it at home
                        if (mScanIndex == mCharacters[0].mIndex && !func_02099c84(&mCharacters[0])) {
                            SetEventState(EVENT_CHOOSE);
                        }
                        if (func_02099d2c(func_0202c468(&gGameData, mScanIndex))) {
                            mFront.DrawWindow(func_0202c414(&gGameData, func_0202c490(&gGameData, mScanIndex)));
                        }
                    } else if (now >= func_0202c4e4(&gGameData, mScanIndex)) {
                        int partner = EndDueEvent();
                        if (partner >= 0) {
                            mFront.DrawWindow(func_0202c414(&gGameData, partner));
                        }
                    } else if (func_020ad348(mScanIndex, now)) {
                        UpdateAbsentCharacter(now);
                    }
                    mFront.DrawWindow(func_0202c414(&gGameData, mScanIndex));
                }
                mScanIndex++;
            }
            if (--mEventTimer <= 0) {
                SetEventState(EVENT_CHOOSE);
            }
        }
        break;

    case EVENT_ALONE:
        if (mEventStep == 0) {
            mEventStep++;
            if (mEventTimer < 0) {
                mEventTimer = 600;
            } else if (mEventTimer == 0) {
                SetEventState(EVENT_CHOOSE);
            }
        } else if (mEventStep == 1) {
            BOOL refresh;
            s64 now = mClock.mSeconds;

            if (mScanIndex >= 100) {
                mScanIndex = 0;
                int index = StartRandomEvent(now);
                if (index >= 0) {
                    mFront.DrawWindow(func_0202c414(&gGameData, index));
                }
            } else {
                if (func_0202c3dc(&gGameData, mScanIndex)) {
                    refresh = TRUE;

                    if (now >= func_0202c4b8(&gGameData, mScanIndex)) {
                        SavedCharacter temp;
                        SavedCharacter *character = StartDueEvent(now, &temp);

                        if (character->mIndex == mCharacters[0].mIndex) {
                            mEventStep++;
                            if (func_02099c84(&mCharacters[0])) {
                                refresh = FALSE;
                                if (mCharacters[0].mPrevEvent == 18) {
                                    mFront.DrawWindowOfKind(0, mCharacters[0].mRoomNumber);
                                }
                            } else if (func_02099da8(&mCharacters[0])) {
                                mCharacters[0].mWait = 0;
                                mCharacters[0].SetState(0x117);
                            }
                        } else if (func_02099d1c(character) && character->mEventPartner == mCharacters[0].mIndex) {
                            mEventStep++;
                            mCharacters[0].mWait = 0;
                            mCharacters[0].SetState(0x117);
                            refresh = FALSE;
                        } else if (func_02099da8(character) && character->mEventPartner == mCharacters[0].mIndex) {
                            mEventStep++;
                            mCharacters[0].mWait = 0;
                            mCharacters[0].SetState(0x117);
                            refresh = FALSE;
                        }

                        if (mScanIndex != mCharacters[0].mIndex) {
                            if (func_02099d2c(func_0202c468(&gGameData, mScanIndex))) {
                                mFront.DrawWindow(func_0202c414(&gGameData, func_0202c490(&gGameData, mScanIndex)));
                            }
                        } else if (!func_02099c84(&mCharacters[0]) && func_02099da8(&mCharacters[0])) {
                            mFront.DrawWindow(func_0202c414(&gGameData, mCharacters[0].mEventPartner));
                        }
                    } else if (now >= func_0202c4e4(&gGameData, mScanIndex)) {
                        int partner = EndDueEvent();
                        if (partner >= 0) {
                            mFront.DrawWindow(func_0202c414(&gGameData, partner));
                        }
                    } else if (func_020ad348(mScanIndex, now)) {
                        UpdateAbsentCharacter(now);
                    }
                    if (refresh) {
                        mFront.DrawWindow(func_0202c414(&gGameData, mScanIndex));
                    }
                }
                mScanIndex++;
            }

            if (mEventStep == 2) {
                break;
            }
            if (func_02099e64(&mCharacters[0])) {
                ChooseReactionsQuiet();
            }
            if (--mEventTimer <= 0) {
                SetEventState(EVENT_CHOOSE);
            }
        } else {
            SetEventState(EVENT_CHOOSE);
            StartEvent(&mCharacters[0], 0);
        }
        break;

    case EVENT_TOGETHER:
        if (mEventStep == 0) {
            mEventStep++;
            if (mEventTimer < 0) {
                mEventTimer = 600;
            } else if (mEventTimer == 0) {
                SetEventState(EVENT_CHOOSE);
            }
        } else if (mEventStep == 1) {
            BOOL refresh;
            s64 now = mClock.mSeconds;

            if (mScanIndex >= 100) {
                mScanIndex = 0;
                int index = StartRandomEvent(now);
                if (index >= 0) {
                    mFront.DrawWindow(func_0202c414(&gGameData, index));
                }
            } else {
                if (func_0202c3dc(&gGameData, mScanIndex)) {
                    refresh = TRUE;

                    if (now >= func_0202c4b8(&gGameData, mScanIndex)) {
                        SavedCharacter temp;
                        StartDueEvent(now, &temp);

                        BOOL withPartner = func_02099d1c(&mCharacters[1]);
                        if (!withPartner || (withPartner && mCharacters[1].mEventPartner != mCharacters[0].mIndex)) {
                            mEventStep++;
                            mCharacters[0].SetState(0x117);
                            mCharacters[1].SetState(0x117);
                            refresh = FALSE;
                            mCharacters[0].mWait = 0;
                            mCharacters[1].mWait = 0;
                        } else if (withPartner && mCharacters[1].mEventPartner == mCharacters[0].mIndex) {
                            if (mScanIndex == mCharacters[0].mIndex || mScanIndex == mCharacters[1].mIndex) {
                                mEventStep++;
                            }
                        }

                        if (mScanIndex != mCharacters[0].mIndex && mScanIndex != mCharacters[1].mIndex) {
                            if (func_02099d2c(func_0202c468(&gGameData, mScanIndex))) {
                                mFront.DrawWindow(func_0202c414(&gGameData, func_0202c490(&gGameData, mScanIndex)));
                            }
                        }
                    } else if (now >= func_0202c4e4(&gGameData, mScanIndex)) {
                        int partner = EndDueEvent();
                        if (partner >= 0) {
                            mFront.DrawWindow(func_0202c414(&gGameData, partner));
                        }
                    } else if (func_020ad348(mScanIndex, now)) {
                        UpdateAbsentCharacter(now);
                    }
                    if (refresh) {
                        mFront.DrawWindow(func_0202c414(&gGameData, mScanIndex));
                    }
                }
                mScanIndex++;
            }

            if (mEventStep == 2) {
                break;
            }
            if (func_02099e64(&mCharacters[0])) {
                UpdateMeeting();
                ChooseReactions();
            }
            if (--mEventTimer <= 0) {
                SetEventState(EVENT_CHOOSE);
            }
        } else {
            SetEventState(EVENT_CHOOSE);
            if (func_02099da8(&mCharacters[0])) {
                StartPairEvent(0);
            }
        }
        break;

    case EVENT_APPROACH:
        if (AreCharactersNear(0, 1, 0x2333, 0x2666)) {
            SetEventState(EVENT_MEET);
            mCharacters[0].SetState(0xF3);
            mCharacters[0].SetTargetFloorPos(mCharacters[1].GetFloorPos());
            mCharacters[1].SetState(0xF3);
            mCharacters[1].SetTargetFloorPos(mCharacters[0].GetFloorPos());
        }
        break;

    case EVENT_MEET:
        if (mEventStep == 0) {
            if (mCharacters[0].mState != 0 || mCharacters[1].mState != 0) {
                break;
            }
            int index = MATH_Rand16(&gRandom, 0) & 1;
            mEventStep++;
            RoomCharacter *partner = mCharacters[index].mPartner;
            func_02099088(&mCharacters[index], partner->mIndex,
                          data_021135c8[mCharacters[index].mRelation[partner->mIndex]][1] * 2);
            StartTalkReaction(index, 1);
            mCharacters[index].mReaction = 1;
            partner->mReaction = 1;
            mCharacters[index].mWait = 0x8E;
            partner->mWait = 0x8C;
            mEncounterPending = TRUE;
            mCharacters[index].SetHeadIconMode(1);
            partner->SetHeadIconMode(1);
        } else if (mEventStep == 1) {
            UpdateEncounter();
            if (mCharacters[0].mState != 0xF6 || mCharacters[1].mState != 0xF6) {
                break;
            }
            mEventStep++;
            mEncounterPending = FALSE;
            mCharacters[0].mReaction = 0;
            mCharacters[1].mReaction = 0;
            mCharacters[0].ResetBody();
            mCharacters[1].ResetBody();
            SaveCharacters();
        } else {
            if (JudgeEncounterEnd(mActor, mActorOther) != 10) {
                SetEventState(EVENT_ENCOUNTER_END);
                mCharacters[0].mWait = 20;
                mCharacters[1].mWait = 20;
            } else if (func_02099e64(&mCharacters[0])) {
                mCharacters[0].SetState(0x103);
                mCharacters[1].SetState(0x103);
                SetEventState(EVENT_CHOOSE_WALKER);
            } else {
                StartPairEvent(0);
                SetEventState(EVENT_TOGETHER, -1);
            }
        }
        break;

    case EVENT_CHOOSE_WALKER:
        if (mEventStep == 0) {
            FloorPos target;
            if (mCharacters[0].mState != 0 || mCharacters[1].mState != 0) {
                break;
            }
            mEventStep++;
            if (mCharacters[0].GetFloorPos().LengthSq() <= mCharacters[1].GetFloorPos().LengthSq()) {
                mActor = &mCharacters[0];
                mActorOther = &mCharacters[1];
            } else {
                mActor = &mCharacters[1];
                mActorOther = &mCharacters[0];
            }
            mActor->SetState(0xF5);
            mActor->GetRandomPos(&target);
            mActor->SetTargetFloorPos(target);
            mActor->mWait = MATH_Rand16(&gRandom, 10) * 2 + 60 + ((200 - func_02098ec4(mActor)) >> 2);
            mActorOther->SetState(4);
            mActorOther->mWait = 30;
        } else {
            if (mActor->mState == 0 || (mCharacters[0].mWait <= 0 && mCharacters[1].mWait <= 0)) {
                SetEventState(EVENT_TOGETHER, 240);
                mActor->SetState(1);
                mActor->mWait = MATH_Rand16(&gRandom, 10) + 10;
                mActorOther->SetState(1);
                mActorOther->mWait = MATH_Rand16(&gRandom, 15) + 30;
            } else if (AreCharactersNear(0, 1, 0x2333, 0x2666)) {
                func_0209171c(mActor, mActorOther->GetFloorPos());
            }
        }
        break;

    case EVENT_ENCOUNTER:
        if (mEventStep == 0) {
            if (mCharacters[0].mWait > 0 || mCharacters[1].mWait > 0) {
                break;
            }
            mEventStep++;
            mEventTimer = 40;
            ApplyEncounterResult(&mCharacters[0], &mCharacters[1],
                                 mEncounterJudge.Judge(&mCharacters[0], &mCharacters[1]), 1);
            mCharacters[0].SetState(0x104);
            mCharacters[1].SetState(0x104);
            mIcon[0].mIconId = 4;
            // The mark goes half way between the two
            VecFx32 middle = CharacterModel_GetJointPosition(&mCharacters[0].mModel, 3);
            VecFx32 other = CharacterModel_GetJointPosition(&mCharacters[1].mModel, 3);
            VEC_Add(&middle, &other, &middle);
            middle.x >>= 1;
            middle.y >>= 1;
            middle.z >>= 1;
            mIcon[0].SetPos(middle.x, middle.y, middle.z);
            mMessage.SetState(4);
        } else if (mEventStep == 1) {
            if (--mEventTimer > 0) {
                break;
            }
            mEventStep++;
            mIcon[0].Show();
            mIcon[0].Reload();
            mIcon[0].SetMode(2);
            int kind = mCharacters[0].GetFlagOfPartner();
            ShowMessage(&mCharacters[0], &mCharacters[1], gEncounterMessages[kind], 2);
            if (kind == 4) {
                gSoundManager.StartSeqArc(1, 0x26, 2);
            } else {
                gSoundManager.StartSeqArc(1, 0x26, 0);
            }
        } else {
            if (mCharacters[0].mState != 0 || mCharacters[1].mState != 0) {
                break;
            }
            SetEventState(EVENT_MEET);
            mIcon[0].SetMode(1);
        }
        break;

    case EVENT_ENCOUNTER_END:
        if (mEventStep == 0) {
            if (mCharacters[0].mWait > 0 || mCharacters[1].mWait > 0) {
                break;
            }
            mEventStep++;
            mEventTimer = 30;
            ApplyEncounterResult(&mCharacters[0], &mCharacters[1], JudgeEncounterEnd(mActor, mActorOther), 2);
            mCharacters[0].SetState(0x105);
            mCharacters[1].SetState(0x105);
            mMessage.SetState(4);
        } else if (mEventStep == 1) {
            if (--mEventTimer > 0) {
                break;
            }
            mEventStep++;
            ShowMessage(&mCharacters[0], &mCharacters[1],
                        gEncounterEndMessages[mCharacters[0].GetFlagOfPartner()], 2);
            gSoundManager.StartSeqArc(1, 0x26, 1);
        } else {
            if (mCharacters[0].mState != 0 || mCharacters[1].mState != 0) {
                break;
            }
            SetEventState(EVENT_TOGETHER, -1);
        }
        break;

    case EVENT_LAST_LEAVES:
        if (mEventStep == 0) {
            mEventStep++;
            mLight.SetState(2);
        } else {
            if (mActor->mPresent) {
                break;
            }
            SetEventState(EVENT_NOBODY_HOME);
            mCamera.SetMode(1);
            mLight.SetState(4);
            mFront.DrawWindow(func_0202c414(&gGameData, mActor->mIndex));
            if (func_02099d1c(mActor)) {
                mFront.DrawWindow(func_0202c414(&gGameData, mActor->mEventPartner));
            }
            SaveCharacter(mActor);
        }
        break;

    case EVENT_VISITOR_LEAVES:
        if (mEventStep == 0) {
            if (mActorOther->mState != 0) {
                break;
            }
            mEventStep++;
            mActor->SetState(0x107);
            mActorOther->SetState(0x109);
        } else {
            if (CountPresent() != 1) {
                break;
            }
            if (func_02099d1c(mActorOther)) {
                SetEventState(EVENT_CHOOSE, 60);
            } else {
                SetEventState(EVENT_ALONE, 60);
                StartEvent(mActorOther, 0);
            }
            mCamera.SetMode(2);
            mFront.DrawWindow(func_0202c414(&gGameData, mActor->mIndex));
            if (func_02099d1c(mActor)) {
                mFront.DrawWindow(func_0202c414(&gGameData, mActor->mEventPartner));
            }
            SaveCharacter(mActor);
        }
        break;

    case EVENT_RESIDENT_ARRIVES:
        if (mEventStep == 0) {
            mEventStep++;
            mCharacters[0].mPresent = TRUE;
            mCharacters[0].SetState(0x10C);
            mCamera.SetMode(2);
            mCamera.SetState(4, 0x474);
            mLight.SetState(2);
            if (mCharacters[0].mEvent == 18) {
                mFront.DrawWindowOfKind(0, mCharacters[0].mRoomNumber);
            } else {
                mFront.DrawWindow(mCharacters[0].mRoomNumber);
            }
            ShowMessage(&mCharacters[0], NULL, 9, 2);
        } else {
            if (mCharacters[0].mState != 0) {
                break;
            }
            SetEventState(EVENT_ALONE, 60);
            if (func_02099e64(&mCharacters[0])) {
                mCharacters[0].SetState(1);
            } else {
                StartEvent(&mCharacters[0], 0);
            }
            mCharacters[0].mWait = MATH_Rand16(&gRandom, 20) + 30;
            mCharacters[0].mReaction = 0;
        }
        break;

    case EVENT_VISITOR_ARRIVES:
        if (mEventStep == 0) {
            mEventStep++;
            mLight.SetState(2);
        } else if (mEventStep == 1) {
            if (mActorOther->mState != 0) {
                break;
            }
            if (!ArchiveManager_IsIdle(&gArchiveManager)) {
                break;
            }
            if (!IsTransitionDone()) {
                break;
            }
            mEventStep++;
            mActor->mIndex = GetCharacterCountForDisplay();
            LoadCharacter(mActor);
            mActor->SetOverrideState(1);
        } else if (mEventStep == 2) {
            if (!IsTransitionDone()) {
                break;
            }
            mEventStep++;
            mActor->SetOverrideState(2);
        } else if (mEventStep == 3) {
            if (!IsTransitionDone()) {
                break;
            }
            mEventStep++;
            mActor->SetOverrideState(3);
        } else if (mEventStep == 4) {
            if (!IsTransitionDone()) {
                break;
            }
            mEventStep++;
            mEventTimer = 0;
            mFront.DrawWindow(mActor->mRoomNumber);
            mMessage.SetState(4);
            gSoundManager.StartSeqArc(1, 0x1D, 0x10);
        } else if (mEventStep == 5) {
            if (++mEventTimer <= 40) {
                break;
            }
            mEventStep++;
            mEventTimer = 0;
            func_02091a14(mActor, 1);
            mActor->mPresent = TRUE;
            mActor->SetState(0x10D);
            mActorOther->SetState(0x110);
            mCamera.SetMode(3);
        } else if (mEventStep == 6) {
            if (++mEventTimer <= 60) {
                break;
            }
            mEventStep++;
            mEventTimer = 0;
            mActor->SetState(0x10E);
            ShowMessage(mActor, NULL, 9, 2);
        } else if (mEventStep == 7) {
            if (mActor->mState != 0) {
                break;
            }
            mEventStep++;
            mActor->SetState(1);
            mActor->mWait = MATH_Rand16(&gRandom, 20) + 30;
        } else {
            int nearer;
            int farther;
            if (mCharacters[0].GetFloorPos().LengthSq() <= mCharacters[1].GetFloorPos().LengthSq()) {
                nearer = 0;
                farther = 1;
            } else {
                nearer = 1;
                farther = 0;
            }
            StartEncounter(nearer, farther);
        }
        break;

    case EVENT_AFTER_DISTRIBUTION:
        if (mEventStep == 0) {
            if (--mEventTimer > 0) {
                break;
            }
            mEventStep++;
            mEventTimer = 60;
            func_02099324(&mCharacters[0], mClock.mSeconds);
            func_0209b1d8(&mCharacters[0]);
            SaveCharacter(&mCharacters[0]);
            mFront.DrawWindow(mCharacters[0].mRoomNumber);
            mLight.SetState(2);
        } else {
            if (--mEventTimer > 0) {
                break;
            }
            SetInputState(INPUT_IDLE);
            SetEventState(EVENT_CHOOSE);
        }
        break;
    }
}

// ===================================================================================================================
// What two characters in a room do to each other, and RoomCharacter's constructor, state setter and Update

#define RAND(max) MATH_Rand16(&gRandom, max)

// The same as a function: its int result makes a comparison of it signed
static inline int Rand(int max) {
    return MATH_Rand16(&gRandom, max);
}


// 0x0214e5f8
void RoomScene::ChooseEncounter() {
    int count = CountPresent();
    if (count == 0) {
        if (func_02099c84(&mCharacters[0])) {
            SetEventState(4);
        } else {
            SetEventState(12);
        }
    } else if (count == 1) {
        RoomCharacter *character = mCharacters[0].mPresent ? &mCharacters[0] : &mCharacters[1];
        if (func_02099c84(character)) {
            SetEventState(14);
            character->SetState(0x106);
            mActor = character;
        } else if (func_02099da8(character)) {
            if (mResidentCount != 1) {
                FloorPos pos = character->GetFloorPos();
                if (pos.z < -0x1800) {
                    if (pos.x < 0) {
                        character->mTarget.x = -0x2666;
                        character->mTarget.z = -0x1999;
                    } else {
                        character->mTarget.x = 0x2666;
                        character->mTarget.z = -0x1999;
                    }
                    character->SetState(0x112);
                } else {
                    character->SetState(0x111);
                }
                SetEventState(13);
                mActor = character->mPartner;
                mActorOther = character;
            } else {
                SetEventState(5, -1);
            }
        } else {
            SetEventState(5, -1);
        }
    } else if (count == 2) {
        int chance = RAND(1000);
        int withPartner = func_02099d1c(&mCharacters[1]);
        if (!withPartner || (withPartner && mCharacters[1].mEventPartner != mCharacters[0].mIndex)) {
            FloorPos pos1 = mCharacters[1].GetFloorPos();
            FloorPos pos0 = mCharacters[0].GetFloorPos();
            if (pos0.z < -0x1800) {
                if (pos1.x <= 0 && pos1.x < pos0.x && pos0.x < 0x2000) {
                    mCharacters[0].mTarget.x = 0x2666;
                    mCharacters[0].mTarget.z = -0x2000;
                    mCharacters[0].SetState(0x10b);
                } else if (pos1.x >= 0 && pos1.x > pos0.x && pos0.x > -0x2000) {
                    mCharacters[0].mTarget.x = -0x2666;
                    mCharacters[0].mTarget.z = -0x2000;
                    mCharacters[0].SetState(0x10b);
                } else if (pos0.x > -0x2000 && pos0.x < 0x2000) {
                    if (pos1.x < 0) {
                        mCharacters[0].mTarget.x = 0x2666;
                        mCharacters[0].mTarget.z = -0x2000;
                    } else {
                        mCharacters[0].mTarget.x = -0x2666;
                        mCharacters[0].mTarget.z = -0x2000;
                    }
                    mCharacters[0].SetState(0x10b);
                } else {
                    mCharacters[0].SetState(0x10a);
                }
            } else {
                mCharacters[0].SetState(0x10a);
            }
            RoomCharacter *other = &mCharacters[1];
            other->SetState(1);
            SetEventState(15);
            mActor = other;
            mActorOther = &mCharacters[0];
        } else if (chance < 150 && func_02099e64(&mCharacters[0])) {
            mEventFrames = 0;
            int nearer;
            int farther;
            if (LengthSq(mCharacters[0].GetFloorPos()) <= LengthSq(mCharacters[1].GetFloorPos())) {
                nearer = 0;
                farther = 1;
            } else {
                nearer = 1;
                farther = 0;
            }
            StartEncounter(nearer, farther);
        } else {
            SetEventState(6, -1);
        }
    } else {
        SetEventState(6, -1);
    }
}

// 0x0214ea60
void RoomScene::ChooseReactionsQuiet() {
    for (int i = 0; i < 2; i++) {
        if (!mCharacters[i].mPresent) {
            continue;
        }
        if (mCharacters[i].mWait > 0 && mCharacters[i].mState != 0) {
            continue;
        }
        int patience = 200 - func_02098ec4(&mCharacters[i]);
        mCharacters[i].mWait = 240 - RAND(50) + (patience >> 2);
        int reaction = mCharacters[i].PickReaction();
        mCharacters[i].mReaction = reaction;
        switch (reaction) {
        case 0: {
            int chance = RAND(100);
            if (chance < 50) {
                mCharacters[i].SetState(1);
            } else if (chance < 55) {
                mCharacters[i].SetState(7);
                mCharacters[i].mWait = 10000;
            } else {
                mCharacters[i].SetState(2);
            }
            break;
        }
        case 1: {
            FloorPos pos;
            mCharacters[i].GetRandomPosInArea(&pos);
            if (!func_02091edc(&mCharacters[i], pos, func_02091174(&mCharacters[i], 0x800))) {
                mCharacters[i].SetState(8);
                mCharacters[i].SetTarget(pos.x, 0, pos.z);
            } else {
                pos = func_02091abc(&mCharacters[i], 0x1000);
                mCharacters[i].ClampToWalkArea(&pos);
                FloorPos back;
                back = func_02091abc(&mCharacters[i], -0x1000);
                mCharacters[i].ClampToWalkArea(&back);
                if (pos.LengthSq() <= back.LengthSq()) {
                    mCharacters[i].SetTargetXZ(pos.x, pos.z);
                } else {
                    mCharacters[i].SetTargetXZ(back.x, back.z);
                }
                mCharacters[i].SetState(8);
            }
            break;
        }
        case 5:
            mCharacters[i].SetState(12);
            break;
        case 2:
            mCharacters[i].mReaction = 0;
            mCharacters[i].SetState(1);
            break;
        case 6:
            mCharacters[i].SetState(14);
            mCharacters[i].mWait = 1000;
            break;
        case 3:
            mCharacters[i].SetState(15);
            mCharacters[i].mWait = 1000;
            break;
        case 4: {
            int chance = RAND(100);
            if (chance < 50) {
                mCharacters[i].SetState(16);
            } else {
                mCharacters[i].SetState(17);
            }
            break;
        }
        case 7:
            mCharacters[i].SetState(21);
            break;
        case 8:
            mCharacters[i].SetState(22);
            break;
        case 9:
            mCharacters[i].SetState(23);
            break;
        case 10:
            mCharacters[i].SetState(24);
            break;
        case 11:
            mCharacters[i].SetState(25);
            break;
        case 12:
            mCharacters[i].SetState(26);
            break;
        case 13:
            mCharacters[i].mReaction = 0;
            mCharacters[i].SetState(1);
            break;
        case 14:
            mCharacters[i].SetState(28);
            mCharacters[i].mWait += 30;
            break;
        default:
            mCharacters[i].mReaction = 0;
            mCharacters[i].mWait = 10;
            break;
        }
    }
}

// Not matching yet: same size (0x65c), same instructions and stack frame; only the callee-saved registers are
// named differently throughout (95 instructions). Original: r4 = &gRandom, r5 = i, r6 = partner and the generator's
// temporary, r7 = this + i * 0x2d98, r8 = &mCharacters[i]; mine: r4 = this + ..., r5 = &mCharacters[i], r6 = &gRandom,
// r7 = i, r8 = partner. Tried: RAND macro instead of Rand(), a local pointer to gRandom, `partner`, `chance`,
// `patience` and `i` declared at the top of the function, no `patience` local, a `character` local (changes size).
// @0x0214ee90
void RoomScene::ChooseReactions() {
    for (int i = 0; i < 2; i++) {
        if (!mCharacters[i].mPresent) {
            continue;
        }
        if (mCharacters[i].mWait > 0 && mCharacters[i].mState != 0) {
            continue;
        }
        int patience = 200 - func_02098ec4(&mCharacters[i]);
        mCharacters[i].mWait = 240 - Rand(50) + (patience >> 2);
        int reaction = mCharacters[i].PickReaction();
        mCharacters[i].mReaction = reaction;
        switch (reaction) {
        case 0: {
            int chance = Rand(100);
            if (chance < 30) {
                mCharacters[i].SetState(1);
            } else if (chance < 45) {
                mCharacters[i].SetState(2);
            } else {
                int chance2 = Rand(100);
                RoomCharacter *partner = mCharacters[i].mPartner;
                if (mCharacters[i].IsFacing(partner) && partner->IsBeside(&mCharacters[i], 0x1000) &&
                    (partner->IsOneTwoOrFour(partner->mState) || partner->IsState10To12()) && chance2 < 40) {
                    mCharacters[i].SetState(5);
                    mCharacters[i].mWait >>= 1;
                    partner->mWait = mCharacters[i].mWait + Rand(10);
                } else {
                    mCharacters[i].SetState(4);
                }
            }
            break;
        }
        case 1: {
            FloorPos pos;
            if (Rand(100) < 85 || mCharacters[i].ClampToRect4E4(NULL) ||
                mCharacters[i].mPartner->ClampToWalkArea(NULL)) {
                mCharacters[i].GetRandomPos(&pos);
            } else {
                FloorPos middle = Half(mCharacters[0].GetFloorPos() + mCharacters[1].GetFloorPos());
                pos = middle;
            }
            if (!func_02091edc(&mCharacters[i], pos, func_02091174(&mCharacters[i], 0x800))) {
                if (Rand(100) < 40) {
                    mCharacters[i].SetState(8);
                } else {
                    mCharacters[i].SetState(9);
                }
                mCharacters[i].SetTarget(pos.x, 0, pos.z);
            } else {
                mCharacters[i].SetState(11);
            }
            break;
        }
        case 5:
            mCharacters[i].SetState(12);
            break;
        case 2:
            mCharacters[i].mReaction = 0;
            mCharacters[i].SetState(1);
            break;
        case 6:
            mCharacters[i].SetState(14);
            mCharacters[i].mWait = 1000;
            break;
        case 3:
            mCharacters[i].SetState(15);
            mCharacters[i].mWait = 1000;
            break;
        case 4: {
            int chance = Rand(100);
            if (chance < 27) {
                mCharacters[i].SetState(16);
            } else if (chance < 45) {
                mCharacters[i].SetState(17);
            } else {
                int chance2 = Rand(100);
                RoomCharacter *partner = mCharacters[i].mPartner;
                if (mCharacters[i].IsFacing(partner) && partner->IsBeside(&mCharacters[i], 0x1000) &&
                    (partner->IsOneTwoOrFour(partner->mState) || partner->IsState10To12()) && chance2 < 40) {
                    mCharacters[i].SetState(19);
                    mCharacters[i].mWait >>= 1;
                    partner->mWait = mCharacters[i].mWait + Rand(10);
                } else {
                    mCharacters[i].SetState(18);
                }
            }
            break;
        }
        case 7:
            mCharacters[i].SetState(21);
            break;
        case 8:
            mCharacters[i].SetState(22);
            break;
        case 9:
            mCharacters[i].SetState(23);
            break;
        case 10:
            mCharacters[i].SetState(24);
            break;
        case 11:
            mCharacters[i].SetState(25);
            break;
        case 12:
            mCharacters[i].SetState(26);
            break;
        case 13:
            mCharacters[i].mReaction = 0;
            mCharacters[i].SetState(1);
            break;
        case 14:
            mCharacters[i].SetState(28);
            mCharacters[i].mWait += 30;
            break;
        default:
            mCharacters[i].mReaction = 0;
            mCharacters[i].mWait = 10;
            break;
        }
    }
}

// 0x0214f4ec
void RoomScene::ShowMessage(RoomCharacter *first, RoomCharacter *second, int message, int drawMode) {
    if (first != NULL) {
        mMessage.SetStringArg(first->mNameText, 0);
    }
    if (second != NULL) {
        mMessage.SetStringArg(second->mNameText, 1);
    }
    mMessage.mColor = 1;
    mMessage.SetState(drawMode);
    mMessage.mMessage = message;
}

// 0x0214f564
void RoomScene::ShowEventMessage() {
    if (mCharacters[1].mPresent) {
        ShowMessage(&mCharacters[0], &mCharacters[1], 4, 1);
    } else if (mCharacters[0].mPresent) {
        ShowMessage(&mCharacters[0], NULL, 3, 1);
    } else {
        u8 event = mCharacters[0].mEvent;
        if (func_02099d2c(event)) {
            ShowMessage(&mCharacters[0], &mCharacters[1], 5, 1);
        } else {
            switch (event) {
            case 20:
                ShowMessage(&mCharacters[0], NULL, 6, 1);
                break;
            case 19:
                ShowMessage(&mCharacters[0], NULL, 7, 1);
                break;
            case 21:
                ShowMessage(&mCharacters[0], NULL, 8, 1);
                break;
            }
        }
    }
}

// 0x0214f67c
void RoomScene::ApplyEncounterResult(RoomCharacter *first, RoomCharacter *second, int result, int kind) {
    u8 relation = first->mRelation[second->mIndex];
    u16 firstIndex = first->mIndex;
    func_020990b0(first, second->mIndex, result, 100);
    func_020990b0(second, firstIndex, result, 100);
    func_02098fe0(first, result, relation);
    func_02098fe0(second, result, relation);
    if (kind == 1) {
        mEncounterJudge.Apply(first, second, relation, mClock.mSeconds);
    } else if (kind == 2) {
        mEncounterJudge.ApplyEnd(first, second, relation, mClock.mSeconds, 0);
    }
    SaveCharacter(first);
    SaveCharacter(second);
}

// 0x0214f788
int RoomScene::JudgeEncounterEnd(RoomCharacter *first, RoomCharacter *second) {
    if (first->mPrevState == 0xf9 || second->mPrevState == 0xf9) {
        return mEncounterJudge.JudgeEnd(first, second, FALSE);
    } else {
        return mEncounterJudge.JudgeEnd(first, second, TRUE);
    }
}

// 0x0214f7cc
void RoomScene::StartEncounter(int nearer, int farther) {
    BOOL near = AreCharactersNear(0, 1, 0x2333, 0x2666);
    if (mCharacters[nearer].IsAhead(&mCharacters[farther], 0x1b33) &&
        mCharacters[farther].IsAhead(&mCharacters[nearer], 0x3bae) &&
        !mCharacters[nearer].IsNearerThan(&mCharacters[farther], 0x8333) && !near) {
        mCharacters[farther].SetState(0xf2);
        mCharacters[nearer].SetState(0xf4);
    } else if (near) {
        mCharacters[0].SetState(1);
        mCharacters[1].SetState(1);
    } else {
        mCharacters[0].SetState(0xf2);
        mCharacters[1].SetState(0xf2);
    }
    SetEventState(7);
    mCharacters[0].mWait = 10000;
    mCharacters[1].mWait = 10000;
}

// 0x0214f950
void RoomScene::UpdateEncounter() {
    for (int i = 0; i < 2; i++) {
        if (mCharacters[i].mWait <= 0) {
            ChooseTalkReaction(i);
            if (mEncounterJudge.Judge(&mCharacters[i], mCharacters[i].mPartner) != 10) {
                SetEventState(10);
            }
        }
    }
}

// 0x0214f9d0
void RoomScene::FinishEncounter() {
    if (mEncounterPending && CountPresent() >= 2) {
        for (int i = 0; i < 20; i++) {
            ChooseTalkReaction(mCharacters[0].mWait > mCharacters[1].mWait ? 1 : 0);
            if (mCharacters[0].mState == 0xf6 && mCharacters[1].mState == 0xf6) {
                break;
            }
        }
        int result = mEncounterJudge.Judge(&mCharacters[0], &mCharacters[1]);
        if (result != 10) {
            ApplyEncounterResult(&mCharacters[0], &mCharacters[1], result, 1);
        } else {
            result = JudgeEncounterEnd(mActor, mActorOther);
            if (result != 10) {
                ApplyEncounterResult(&mCharacters[0], &mCharacters[1], result, 2);
            }
        }
    }
    mEncounterPending = FALSE;
}

// 0x0214fad4
void RoomScene::ChooseTalkReaction(int index) {
    u16 partnerIndex;
    RoomCharacter *partner = mCharacters[index].mPartner;
    partnerIndex = partner->mIndex;
    u8 relation = mCharacters[index].mRelation[partnerIndex];
    int reaction = func_0209d5ac(&mCharacters[index], relation, partner->mReaction, partner);
    int patience = (200 - func_02098ec4(&mCharacters[index])) >> 2;
    mCharacters[index].mReaction = reaction;
    mCharacters[index].mWait = patience + 160;
    partner->mWait = patience + 150;
    func_02099088(&mCharacters[index], partnerIndex, data_021135c8[relation][reaction] * 2);
    mActor = &mCharacters[index];
    mActorOther = mCharacters[index].mPartner;
    StartTalkReaction(index, reaction);
}

// 0x0214fbb0
void RoomScene::StartTalkReaction(int index, int reaction) {
    switch (reaction) {
    case 0:
        if (!mCharacters[index].mMoodLocked) {
            int state = mCharacters[index].mState;
            if (state == 0xf9) {
                mCharacters[index].mMood = 2;
            } else if (state == 0xf8 || state == 0xff) {
                mCharacters[index].mMood = 1;
            } else if (state == 0x100) {
                mCharacters[index].mMood = 0;
            }
        }
        mCharacters[index].SetState(0xf6);
        break;
    case 1:
        mCharacters[index].SetState(0xf7);
        break;
    case 2:
        mCharacters[index].SetState(0xf8);
        break;
    case 3:
        mCharacters[index].SetState(0xf9);
        break;
    case 4:
        mCharacters[index].SetState(0xfa);
        break;
    case 5:
        mCharacters[index].SetState(0xfb);
        break;
    case 6:
        mCharacters[index].SetState(0xfc);
        break;
    case 7:
        mCharacters[index].SetState(0xfd);
        break;
    case 8:
        mCharacters[index].SetState(0xfe);
        break;
    case 9:
        mCharacters[index].SetState(0xff);
        break;
    case 10:
        mCharacters[index].SetState(0x100);
        break;
    case 11:
        mCharacters[index].SetState(0x101);
        break;
    default:
        mCharacters[index].mReaction = 0;
        break;
    }
}

// @0x0214fd84
void RoomScene::StartEvent(RoomCharacter *character, int phase) {
    if (character->mPrevEvent != character->mEvent) {
        character->SetHeldItemsVisible(FALSE);
    }
    switch (character->mEvent) {
    case 0:
        if (phase == 0) {
            mLight.SetState(2);
            character->SetState(0x117);
        }
        character->mReaction = 0;
        character->mWait = 0;
        break;
    case 1:
        if (phase == 0) {
            character->SetState(0x1d);
            mLight.SetState(2);
        } else {
            character->SetState(Rand(3) + 0x1e);
        }
        break;
    case 2:
        if (phase == 0) {
            character->SetState(0x27);
            mLight.SetState(2);
        } else {
            character->SetState(0x28);
            character->mUnknown25D4 = 0xd100;
        }
        break;
    case 3:
        if (phase == 0) {
            character->SetState(0x29);
            mLight.SetState(2);
        } else {
            character->SetState(Rand(2) + 0x2a);
        }
        break;
    case 4:
        if (phase == 0) {
            character->SetState(0x2e);
            mLight.SetState(2);
        } else {
            FloorPos pos;
            character->GetRandomEdgePos(&pos);
            character->SetFloorPos(pos.x, pos.z);
            u16 angle = character->SnapAngle(FX_Atan2Idx(pos.z, pos.x));
            int state = Rand(2) + 0x2f;
            if (state == 0x2f) {
                character->SetTargetXZ(pos.x, pos.z);
            } else {
                FloorPos next;
                character->GetWalkAreaCorner(pos, &next);
                character->SetTargetXZ(next.x, next.z);
                angle -= 0x4000;
            }
            Character_SetAngle(character, angle);
            character->SetState(state);
        }
        break;
    case 5:
        if (phase == 0) {
            character->SetState(0x34);
            mLight.SetState(2);
        } else {
            character->mModel.mPos.x = 0;
            character->mModel.mPos.z = -0x800;
            Character_SetAngle(character, 0x4000);
            character->SetState(Rand(3) + 0x35);
        }
        break;
    case 6:
        if (phase == 0) {
            character->SetState(0x3d);
            mLight.SetState(2);
        } else {
            character->SetState(0x3e);
            FloorPos pos;
            character->GetRandomCorner(&pos);
            character->mModel.mPos.x = pos.x;
            character->mModel.mPos.z = pos.z;
            Character_SetAngle(character, FX_Atan2Idx(pos.z, pos.x));
        }
        break;
    case 7:
        if (phase == 0) {
            character->SetState(0x40);
            mLight.SetState(2);
        } else {
            character->SetState(Rand(3) + 0x41);
        }
        break;
    case 8:
        if (phase == 0) {
            character->SetState(0x44);
            mLight.SetState(2);
        } else {
            character->SetState(Rand(2) + 0x45);
        }
        break;
    case 9:
        if (phase == 0) {
            character->SetState(0x48);
            mLight.SetState(2);
        } else {
            character->SetState(0x4c);
        }
        break;
    case 10:
        if (phase == 0) {
            character->SetState(0x22);
            mLight.SetState(2);
        } else {
            character->SetState(Rand(3) + 0x23);
        }
        break;
    case 11:
        if (phase == 0) {
            character->SetState(0x4f);
            mLight.SetState(2);
        } else {
            character->SetState(Rand(3) + 0x50);
        }
        break;
    case 12:
        if (phase == 0) {
            character->SetState(0x54);
            mLight.SetState(2);
        } else {
            character->SetState(0x55);
        }
        break;
    case 13:
        if (phase == 0) {
            character->SetState(0x58);
            mLight.SetState(2);
        } else {
            character->SetState(0x59);
        }
        break;
    case 14:
        if (phase == 0) {
            character->SetState(0x5b);
            mLight.SetState(2);
        } else {
            character->SetState(Rand(2) + 0x5c);
        }
        break;
    case 15:
        if (phase == 0) {
            character->SetState(0x5e);
            mLight.SetState(2);
        } else {
            character->SetState(0x5f);
        }
        break;
    case 16:
        if (phase == 0) {
            character->SetState(0x61);
            mLight.SetState(2);
        } else {
            character->SetState(Rand(2) + 0x62);
        }
        break;
    case 17:
        if (phase == 0) {
            character->SetState(0x65);
            mLight.SetState(2);
        } else {
            character->SetState(0x66);
        }
        break;
    case 18:
        if (phase == 0) {
            character->SetState(0x68);
            mLight.SetState(4);
            mFront.DrawWindow(character->mRoomNumber);
        } else {
            character->SetState(0x6a);
        }
        break;
    }
}

// Not matching yet: same size (0xee8) and stack frame, 15 instructions differ, all in case 0x2a: the original loads
// gRoomPoints[1] after the four stores of the two places (interleaved with reading the first character's
// place back: pos.x, point.x, pos.z, point.z, in r6, r3, r5, r2), mine loads it between those stores, which also
// moves the place constants from r2/r3 to r5/r6. The original keeps the point's x and z across the first
// FX_Atan2Idx in a slot at sp+8, as this form does. Tried: the table not const (it is read again after the call,
// frame 8 bytes smaller), a named FloorPos copy in the case or at the top of the function (the copy lands at the top
// of the frame, not at sp+8), a const copy, a copy filled field by field, a by-value inline wrapper, a pointer and a
// reference to the entry, GetOffsetTo taking the point by value.
// @0x02150538
void RoomScene::StartPairEvent(int phase) {
    RoomCharacter *pair[2];
    for (int i = 0; i < 2; i++) {
        pair[i] = &mCharacters[i];
        if (pair[i]->mPrevEvent != pair[i]->mEvent) {
            pair[i]->SetHeldItemsVisible(FALSE);
        }
    }
    RoomCharacter *second = pair[1];
    if (pair[0]->mEventPartner != second->mIndex) {
        return;
    }
    switch (pair[0]->mEvent) {
    case 0x18:
        if (phase == 0) {
            pair[0]->SetState(0x117);
            second->SetState(0x117);
            mLight.SetState(2);
        }
        pair[0]->mReaction = 0;
        second->mReaction = 0;
        pair[0]->mWait = 0;
        second->mWait = 0;
        break;
    case 0x1a:
        if (phase == 0) {
            pair[0]->SetState(0x70);
            second->SetState(0x70);
            mLight.SetState(2);
        } else {
            pair[0]->SetFloorPos(0x1666, -0x199);
            second->SetFloorPos(-0x1666, -0x199);
            int kind = Rand(100);
            if (Rand(100) < 30) {
                Character_SetAngle(pair[0], 0x8000);
                Character_SetAngle(second, 0);
                if (kind < 35) {
                    pair[0]->SetState(0x72);
                } else if (kind < 70) {
                    pair[0]->SetState(0x74);
                } else {
                    pair[0]->SetState(0x75);
                }
            } else {
                Character_SetAngle(pair[0], 0x4000);
                Character_SetAngle(second, 0x4000);
                if (kind < 40) {
                    pair[0]->SetState(0x71);
                } else if (kind < 75) {
                    pair[0]->SetState(0x73);
                } else {
                    pair[0]->SetState(0x75);
                }
            }
            second->SetState(pair[0]->mState);
        }
        break;
    case 0x1c:
        if (phase == 0) {
            pair[0]->SetState(0x7f);
            second->SetState(0x7f);
            mLight.SetState(2);
        } else {
            pair[0]->SetFloorPos(0x1666, -0x199);
            second->SetFloorPos(-0x1666, -0x199);
            Character_SetAngle(pair[0], 0x4400);
            Character_SetAngle(second, 0x3c00);
            int state = Rand(2) + 0x80;
            pair[0]->SetState(state);
            second->SetState(state);
        }
        break;
    case 0x1e:
        if (phase == 0) {
            pair[0]->SetState(0x8b);
            second->SetState(0x8b);
            mLight.SetState(2);
        } else {
            pair[0]->SetFloorPos(0x1666, pair[0]->mModel.mPos.z >> 1);
            second->SetFloorPos(-0x1666, second->mModel.mPos.z >> 1);
            for (int i = 0; i < 2; i++) {
                RoomCharacter *character = pair[i];
                FloorPos pos = character->GetFloorPos();
                Character_SetAngle(character, FX_Atan2Idx(pos.z, pos.x) + 0x8000);
                int chance = Rand(100);
                if (chance < 70) {
                    character->SetState(0x8c);
                } else if (chance < 90) {
                    character->SetState(0x8d);
                } else {
                    character->SetState(0x8e);
                }
            }
        }
        break;
    case 0x20:
        if (phase == 0) {
            pair[0]->SetState(0x93);
            second->SetState(0x93);
            mLight.SetState(2);
        } else {
            pair[0]->SetFloorPos(0x1666, -0x199);
            second->SetFloorPos(-0x1666, -0x266);
            for (int i = 0; i < 2; i++) {
                RoomCharacter *character = pair[i];
                u16 angle;
                character->GetAngleAroundPartner(&angle, 0x1000);
                Character_SetAngle(character, angle);
            }
            second->mUnknown25D6 = second->GetHeadYawTo(pair[0]->GetFloorPos(), 0x3000);
            pair[0]->SetState(0x95);
            second->SetState(0x97);
        }
        break;
    case 0x22:
        if (phase == 0) {
            pair[0]->SetState(0x9f);
            second->SetState(0x9f);
            mLight.SetState(2);
        } else {
            pair[0]->SetFloorPos(0x1333, 0x266);
            second->SetFloorPos(-0x1333, -0x333);
            for (int i = 0; i < 2; i++) {
                RoomCharacter *character = pair[i];
                u16 angle;
                character->GetAngleAroundPartner(&angle, 0xa00);
                Character_SetAngle(character, angle);
            }
            second->mUnknown25D6 = second->GetHeadYawTo(pair[0]->GetFloorPos(), 0x3800);
            pair[0]->SetState(0xa0);
            second->SetState(0xa1);
        }
        break;
    case 0x24:
        if (phase == 0) {
            pair[0]->SetState(0xa8);
            second->SetState(0xa8);
            mLight.SetState(2);
        } else {
            pair[0]->SetFloorPos(0x1333, 0x266);
            second->SetFloorPos(-0x1333, -0x333);
            fx32 extra = Rand(16) << 9;
            for (int i = 0; i < 2; i++) {
                RoomCharacter *character = pair[i];
                u16 angle;
                character->GetAngleAroundPartner(&angle, extra + 0x1000);
                Character_SetAngle(character, angle);
            }
            second->mUnknown25D6 = second->GetHeadYawTo(pair[0]->GetFloorPos(), 0x3800);
            pair[0]->SetState(0xa9);
            second->SetState(0xab);
        }
        break;
    case 0x26:
        if (phase == 0) {
            pair[0]->SetState(0xb3);
            second->SetState(0xb3);
            mLight.SetState(2);
        } else {
            pair[0]->SetFloorPos(0x1666, -0x199);
            second->SetFloorPos(-0x1666, -0x266);
            fx32 distance = Rand(100) < 65 ? 0x1000 : 0x4000;
            for (int i = 0; i < 2; i++) {
                RoomCharacter *character = pair[i];
                u16 angle;
                character->GetAngleAroundPartner(&angle, distance);
                Character_SetAngle(character, angle);
            }
            int state = Rand(100) < 55 ? 0xb4 : 0xb5;
            pair[0]->SetState(state);
            second->SetState(state);
        }
        break;
    case 0x28:
        if (phase == 0) {
            pair[0]->SetState(0xbf);
            second->SetState(0xbf);
            mLight.SetState(2);
        } else {
            u16 angle = Rand(256) << 8;
            second->SetFloorPos(FX_Mul(FX_CosIdx(angle), 0xf33), FX_Mul(FX_SinIdx(angle), 0xf33));
            angle = angle + 0x4000 + (Rand(256) << 7);
            pair[0]->SetFloorPos(FX_Mul(FX_CosIdx(angle), 0x24cc), FX_Mul(FX_SinIdx(angle), 0x24cc));
            second->mUnknown25D6 = second->GetHeadYawTo(pair[0]->GetFloorPos(), 0x3800);
            pair[0]->SetState(Rand(100) < 50 ? 0xc0 : 0xc1);
            second->SetState(0xc2);
        }
        break;
    case 0x2a:
        if (phase == 0) {
            pair[0]->SetState(0xca);
            second->SetState(0xca);
            mLight.SetState(2);
        } else {
            pair[0]->SetFloorPos(0x1666, -0x800);
            second->SetFloorPos(-0x1666, -0x8cc);
            Character_SetAngle(pair[0], Angle(pair[0]->GetOffsetTo(gRoomPoints[1])));
            Character_SetAngle(second, Angle(second->GetOffsetTo(gRoomPoints[1])));
            int state = Rand(3) + 0xcb;
            pair[0]->SetState(state);
            second->SetState(state);
        }
        break;
    case 0x2c:
        if (phase == 0) {
            pair[0]->SetState(0xe1);
            second->SetState(0xe1);
            mLight.SetState(2);
        } else {
            pair[0]->SetFloorPos(0x1451, 0xccc);
            second->SetFloorPos(-0x1451, 0xe66);
            u16 angles[2];
            int state;
            if (Rand(100) < 67) {
                angles[0] = Angle(pair[0]->GetOffsetTo(second->GetFloorPos()));
                angles[1] = Angle(second->GetOffsetTo(pair[0]->GetFloorPos()));
                state = Rand(100) < 70 ? 0xe2 : 0xe3;
            } else {
                pair[0]->GetAngleAroundPartner(&angles[0], 0x8000);
                second->GetAngleAroundPartner(&angles[1], 0x8000);
                state = 0xe3;
            }
            Character_SetAngle(pair[0], angles[0]);
            Character_SetAngle(second, angles[1]);
            pair[0]->SetState(state);
            second->SetState(state);
        }
        break;
    case 0x2e:
        if (phase == 0) {
            pair[0]->SetState(0xe9);
            second->SetState(0xe9);
            mLight.SetState(2);
        } else {
            pair[0]->SetFloorPos(0x1451, 0xccc);
            second->SetFloorPos(-0x1451, 0xe66);
            u16 angle0 = Angle(pair[0]->GetOffsetTo(second->GetFloorPos()));
            u16 angle1 = Angle(second->GetOffsetTo(pair[0]->GetFloorPos()));
            Character_SetAngle(pair[0], angle0);
            Character_SetAngle(second, angle1);
            pair[0]->SetState(0xea);
            second->SetState(0xea);
        }
        break;
    }
}

// 0x02151420
int RoomScene::CountPresent() {
    int count = 0;
    for (int i = 0; i < 2; i++) {
        if (mCharacters[i].mPresent) {
            count++;
        }
    }
    return count;
}

// 0x02151454
int RoomScene::GetCharacterCountForDisplay() {
    if (mResidentCount == 0) {
        if (CountPresent() == 0) {
            return 0;
        }
        int index = mCharacters[0].mPresent ? 0 : 1;
        return mCharacters[index].mIndex != 1 ? 1 : 2;
    } else if (mResidentCount == 1) {
        return mResidents[0] != 1 ? 1 : 2;
    } else {
        if (CountPresent() == 0) {
            return mCharacters[0].mIndex;
        } else {
            return mCharacters[0].mEventPartner;
        }
    }
}

// 0x021514f0
void RoomScene::UpdateMeeting() {
    if (CountPresent() != 2) {
        return;
    }
    if (!AreCharactersNear(0, 1, 0x2333, 0x2666)) {
        return;
    }
    StartMeeting(0, 1);
}

// Not matching yet: same size (0x6a0), 75 instructions differ, of two kinds. (1) Stack: the original has the place
// that the first loop clamps at sp+0x78, above posFirst (0x70) and posSecond (0x68); here it is below them (as the
// local of ClampOwnPosToRect4E4, or as a local of the loop's body). Declared at the top of the function and assigned
// in the loop, it is where it should be but costs a third copy (frame 0xa8). (2) Registers: the original has
// first * 0x2d98 in r9 and the two characters in r7 and r8; mine r7, and r8 and r9.
// @0x02151554
void RoomScene::StartMeeting(int first, int second) {
    RoomCharacter *pair[2];
    FloorPos posFirst = mCharacters[first].GetFloorPos();
    FloorPos posSecond = mCharacters[second].GetFloorPos();
    fx32 distanceFirst = posFirst.LengthSq();
    fx32 distanceSecond = posSecond.LengthSq();
    if (distanceFirst - distanceSecond <= 0) {
        pair[0] = &mCharacters[first];
        pair[1] = &mCharacters[second];
    } else {
        pair[0] = &mCharacters[second];
        pair[1] = &mCharacters[first];
    }
    if (mCharacters[first].mState == 10 && mCharacters[second].mState == 10) {
        func_02091598(&mCharacters[first], mCharacters[second].GetFloorPos());
        func_02091598(&mCharacters[second], mCharacters[first].GetFloorPos());
        for (int i = 0; i < 2; i++) {
            RoomCharacter *character = pair[i];
            if (character->ClampOwnPosToRect4E4()) {
                character->SetState(1);
            }
        }
    } else if (mCharacters[first].mState == 10) {
        ApproachWaiting(first, second, distanceFirst - distanceSecond);
    } else if (mCharacters[second].mState == 10) {
        ApproachWaiting(second, first, distanceSecond - distanceFirst);
    } else if (mCharacters[first].IsState8Or9() && mCharacters[second].IsState8Or9()) {
        int chance;
        if (!mCharacters[first].ClampToRect4E4(NULL) && !mCharacters[second].ClampToRect4E4(NULL)) {
            chance = Rand(100);
        } else {
            chance = Rand(70);
        }
        if (chance < 40) {
            RoomCharacter *other = pair[1];
            func_0209171c(pair[0], other->GetFloorPos());
            int random = Rand(40);
            int patience = 200 - func_02098ec4(pair[0]);
            pair[0]->mWait = 100 - random + (patience >> 2);
            pair[0]->SetState(10);
            pair[0]->mReaction = 1;
            if (Rand(100) < 40) {
                other->SetState(1);
            } else {
                other->SetState(4);
            }
            other->mWait = 120;
            other->mReaction = 0;
        } else if (chance < 70) {
            RoomCharacter *other;
            if (Rand(100) < 70 && (other = pair[1], pair[0]->IsBeside(other, 0x1000)) &&
                other->IsBeside(pair[0], 0x1000)) {
                if (Rand(0) & 1) {
                    pair[0]->SetState(5);
                    other->SetState(3);
                } else {
                    other->SetState(5);
                    pair[0]->SetState(3);
                }
            } else {
                mCharacters[first].SetState(3);
                mCharacters[second].SetState(3);
            }
            mCharacters[first].mReaction = 0;
            mCharacters[second].mReaction = 0;
            int wait = Rand(30) + 60;
            pair[0]->mWait = wait;
            pair[1]->mWait = wait + 20;
        } else {
            func_02091598(&mCharacters[first], mCharacters[second].GetFloorPos());
            mCharacters[first].SetSavedTarget(mCharacters[first].mTarget);
            mCharacters[first].mWait = Rand(60) + 90;
            mCharacters[first].SetState(10);
            mCharacters[first].mReaction = 1;
            func_02091598(&mCharacters[second], mCharacters[first].GetFloorPos());
            mCharacters[second].SetSavedTarget(mCharacters[second].mTarget);
            mCharacters[second].mWait = Rand(60) + 90;
            mCharacters[second].SetState(10);
            mCharacters[second].mReaction = 1;
        }
    }
    if (mCharacters[first].IsState8Or9()) {
        AnswerWalker(first, second);
    } else if (mCharacters[second].IsState8Or9()) {
        AnswerWalker(second, first);
    }
}

// @0x02151bf4
void RoomScene::ApproachWaiting(int index, int other, int distanceDifference) {
    BOOL near = AreCharactersNear(index, other, 0x1800, 0x1b33);
    if (mCharacters[index].ClampToRect534(NULL)) {
        if (distanceDifference <= 0) {
            mCharacters[index].mTarget.x = 0;
            mCharacters[index].mTarget.z = 0;
        } else {
            mCharacters[index].SetState(11);
        }
    } else if (mCharacters[index].ClampToRect4E4(NULL)) {
        if (distanceDifference <= 0 || near) {
            func_02091598(&mCharacters[index], mCharacters[other].GetFloorPos());
        } else {
            func_0209171c(&mCharacters[index], mCharacters[other].GetFloorPos());
        }
    } else {
        if (near) {
            func_02091598(&mCharacters[index], mCharacters[other].GetFloorPos());
        } else {
            func_0209171c(&mCharacters[index], mCharacters[other].GetFloorPos());
        }
    }
}

// @0x02151d78
void RoomScene::AnswerWalker(int index, int walker) {
    int chance = Rand(100);
    BOOL facing;
    BOOL sitting;
    BOOL standing = mCharacters[walker].IsOneTwoOrFour(mCharacters[walker].mState);
    sitting = mCharacters[walker].IsState10To12();
    if (mCharacters[index].IsBeside(&mCharacters[walker], 0x1000) &&
        mCharacters[walker].IsBeside(&mCharacters[index], 0x1000)) {
        facing = TRUE;
        if (!standing) {
            chance -= 10;
        }
    } else {
        chance -= 10;
        facing = FALSE;
    }
    int prevState = mCharacters[index].mPrevState;
    if (prevState == 3 || prevState == 5 || prevState == 6 || chance < 50) {
        func_0209171c(&mCharacters[index], mCharacters[walker].GetFloorPos());
        int random = Rand(60);
        int patience = 200 - func_02098ec4(&mCharacters[index]);
        mCharacters[index].mWait = random + 90 + (patience >> 2);
        mCharacters[index].SetState(10);
        mCharacters[index].mReaction = 1;
        return;
    }
    if ((standing || sitting) && facing && Rand(100) < 50) {
        mCharacters[index].SetState(5);
        mCharacters[index].mReaction = 0;
        if (!sitting) {
            mCharacters[walker].SetState(3);
            mCharacters[walker].mReaction = 0;
        }
        mCharacters[index].mWait = Rand(60) + 60;
        mCharacters[walker].mWait = mCharacters[index].mWait + Rand(20);
    } else {
        mCharacters[index].mWait = 60;
        mCharacters[index].SetState(1);
        mCharacters[index].mReaction = 0;
    }
}

// 0x0215206c
BOOL RoomScene::AreCharactersNear(int first, int second, fx32 reach, fx32 maxReach) {
    if (mCharacters[first].IsWithinReach(&mCharacters[second], reach, maxReach) ||
        mCharacters[second].IsWithinReach(&mCharacters[first], reach, maxReach)) {
        return TRUE;
    }
    return FALSE;
}

// 0x02152124
void RoomScene::SetFadeState(int state) {
    mFadeState = state;
    mFadeStep = 0;
}

// 0x0215213c
void RoomScene::SaveCharacter(RoomCharacter *character) {
    if (character == NULL) {
        return;
    }
    u16 index = character->mIndex;
    if (func_0202c3dc(&gGameData, index)) {
        func_0202c28c(&gGameData, index, character);
    }
}

// 0x0215217c
void RoomScene::SaveCharacters() {
    SaveCharacter(&mCharacters[0]);
    SaveCharacter(&mCharacters[1]);
}

// 0x021521a0
void RoomScene::LoadCharacter(RoomCharacter *character) {
    if (character == NULL) {
        return;
    }
    u16 index = character->mIndex;
    if (func_0202c3dc(&gGameData, index)) {
        func_0202c264(&gGameData, index, character);
    } else {
        func_020979e0(character);
        func_02098c40(character, 40, index & 7);
        character->mIndex = index;
    }
}

// 0x02152204
void RoomScene::LoadCharacters() {
    LoadCharacter(&mCharacters[0]);
    LoadCharacter(&mCharacters[1]);
}

// 0x02152228
RoomCharacter::RoomCharacter() {
    SetOverrideState(0);
    mSoundArmed = 0;
}

// 0x02152268
void RoomCharacter::Init(int slot, int param2, int param3) {
    func_02090fdc(this, slot, param2, param3);
    mState = mPrevState = 0;
    mSlot = slot;
    mWait = 0;
    mStep = 0;
    mPrevEvent = 0;
    mSubSubStep = 0;
    mSubStep = mSubSubStep;
    mKeepSound = FALSE;
    ClearSpeechTexts();
    mSpeechText[0] = mSpeechTextDefault;
    mSpeechParam[0] = mSpeechDefault;
    for (int i = 0; i < 4; i++) {
        mSpeechText[i + 1] = mSpeechTextByMood[i];
        mSpeechParam[i + 1] = mSpeechByMood[i];
    }
}

// 0x02152308
void RoomCharacter::LoadHeadIcon(ParticleSystem *particles) {
    mHeadIcon.Load(particles, gMoodHeadIcons[mMood]);
}

// 0x0215232c
void RoomCharacter::PlaceHeadIcon() {
    mHeadIcon.mAngle = 0x4000 - mAngle + mUnknown25D6;
    PositionedNode *node = GetNode(0);
    mHeadIcon.SetPos(node->mPos.x, node->mPos.y, node->mPos.z);
}

// 0x02152370
void RoomCharacter::SetState(int state) {
    if (mState == 7 && Shtts_GetState(mTtsPlayer->mEngine) == 1) {
        mTtsPlayer->Stop();
    }
    mPrevState = mState;
    mState = state;
    mStep = 0;
    mSubSubStep = 0;
    mSubStep = mSubSubStep;
    MI_CpuClear16(&mCounter, 2);
    if (mKeepSound) {
        mKeepSound = FALSE;
        return;
    }
    gSoundManager.StopSeqArc(mSoundHandle, 0);
}

// 0x02152418
void RoomCharacter::SetStateWithPartner(int state, int partnerState) {
    SetState(state);
    mPartner->SetState(partnerState);
}

// 0x0215243c
void RoomCharacter::CopyName() {
    func_0207a80c(this, mNameText);
}

// 0x02152450
void RoomCharacter::SetEvent(u8 event) {
    mPrevEvent = mEvent;
    mEvent = event;
}

// 0x02152464
void RoomCharacter::ShowHeadIcon() {
    if (mMood == 0) {
        mHeadIcon.SetMode(1);
        return;
    }
    mHeadIcon.mAngle = 0x4000 - mAngle + mUnknown25D6;
    s16 iconId = gMoodHeadIcons[mMood];
    if (mHeadIcon.mIconId != iconId) {
        mHeadIcon.Show();
        mHeadIcon.mIconId = iconId;
        mHeadIcon.Reload();
        mHeadIcon.SetMode(3);
    } else {
        mHeadIcon.Restart();
        mHeadIcon.SetMode(3);
    }
}

// 0x02152500
void RoomCharacter::Update() {
    UpdateOverrideState();
    if (mPresent && mOverrideState == 0) {
        int state = mState;
        if (state != 0) {
            if (state <= 0x1c) {
                UpdateTalkStates();
            } else if (state <= 0x3f) {
                UpdateSoloStates1DTo3F();
            } else if (state <= 0x67) {
                UpdateSoloStates40To67();
            } else if (state <= 0x6f) {
                UpdateSoloStates68To6F();
            } else if (state <= 0x92) {
                UpdatePairStates70To92();
            } else if (state <= 0xb2) {
                UpdatePairStates93ToB2();
            } else if (state <= 0xc9) {
                UpdateStatesB3ToC9();
            } else if (state <= 0xe0) {
                UpdateStatesCAToE0();
            } else if (state <= 0xf1) {
                UpdateStatesE1ToF1();
            } else if (state <= 0x103) {
                UpdateStatesF2To103();
            } else if (state <= 0x105) {
                UpdateStates104To105();
            } else if (state <= 0x10b) {
                UpdateStates106To10B();
            } else if (state <= 0x112) {
                UpdateStates10CTo112();
            } else if (state <= 0x116) {
                UpdateStates113To116();
            } else {
                UpdateState117();
            }
        }
        if (mWait > 0) {
            mWait--;
        }
        func_020910a8(this);
    }
    mHeadIcon.Update();
}

// ===================================================================================================================
// RoomCharacterTalkStates

// Only for state 24 of UpdateTalkStates, see there.
struct RoomCharacterAlias : RoomCharacter {};

// An angle as a signed number, -0x7fff..0x8000.
static inline int SignedAngle(u16 angle) {
    int v = angle;
    if (v > 0x8000) {
        v -= 0x10000;
    }
    return v;
}

void RoomCharacter::UpdateTalkStates() {
    switch (mState) {
    case 1:
    case 2:
    case 3:
    case 4:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            Character_SetFace(this, GetFace());
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            {
                u16 yaw;
                u16 pitch;
                if (mState == 2) {
                    yaw = (0x20 - Random(0x40)) << 8;
                    if (Random(10) < 4) {
                        pitch = (0x10 - Random(0x20)) << 8;
                    } else {
                        pitch = 0;
                    }
                } else {
                    pitch = 0;
                    yaw = SignedAngle(mHeadYaw) >> 1;
                    func_02090c78(this, &yaw, 0x3000);
                }
                SetHeadTurn(pitch, yaw, 0, 5);
            }
        } else if (mStep == 1 && mState == 4) {
            if (!func_02091b80(this, mPartner->mModel.mPos.x, mPartner->mModel.mPos.z, 0x3000) &&
                mPartner->mState != 8 && mPartner->mState != 10 && mPartner->mState != 0x18 &&
                mPartner->mState != 0x19) {
                break;
            }
            {
                u16 yaw = GetHeadYawTo(mPartner->GetFloorPos(), 0x3000);
                u16 pitch;
                if (mMood != 3 && Random(100) < 50) {
                    pitch = GetHeadPitchToPartner(0, 0x1800);
                } else {
                    pitch = 0;
                }
                SetHeadTurn(pitch, yaw, 0, 5);
                if (mPartner->mState == 0x18) {
                    if (mFace != 0x1a) {
                        Character_SetFace(this, 0x1a);
                    }
                } else {
                    int face = mFace;
                    if (face != GetFace()) {
                        Character_SetFace(this, GetFace());
                    }
                }
            }
        }
        break;

    case 5:
        if (mStep == 0) {
            if (StepIdleThenAnimation55()) {
                mStep++;
                if (mPartner->IsState10To12()) {
                    mPartner->SetState(0x14);
                } else {
                    mPartner->SetState(6);
                }
            }
        } else {
            StepIdleThenAnimation55();
        }
        break;

    case 6:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StepRandomAnimationFacingPartner();
        } else {
            StepRandomAnimationFacingPartner();
        }
        break;

    case 7:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            Character_SetFace(this, GetFace());
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (mSpeechParam[mMood] && mTtsPlayer->IsEngineIdle()) {
                mStep++;
                func_0209d898(this, mTtsPlayer);
                mTtsPlayer->Speak(mSpeechText[mMood], 0);
            } else {
                SetState(0);
            }
        } else {
            if (mTtsPlayer->IsEngineIdle()) {
                Character_SetFace(this, GetFace());
                SetState(0);
            } else {
                func_02090a58(this, mTtsPlayer->GetPhonemeName(), GetFace());
            }
        }
        break;

    case 8:
    case 9:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            RestartStepSound();
            SetAnimation(RESOURCE_BCA0, GetWalkAnim());
            Character_SetFace(this, GetFace());
            {
                VecFx32 target = mTarget;
                mSavedTarget.x = target.x;
                mSavedTarget.y = target.y;
                mSavedTarget.z = target.z;
            }
            SetHeldItemsShowState(1);
            SetHeadTurn(0, 0, 0, 5);
            ShowHeadIcon();
        } else {
            int speed;
            if (mState == 9) {
                speed = 0x8000;
                SetHeadTurn(0, GetHeadYawTo(mPartner->GetFloorPos(), 0x3000), 0, 4);
            } else {
                speed = 0x3800;
            }
            if (StepWalkLooking(speed, 0, 0x199, 0x22, 6)) {
                SetState(0);
            }
        }
        break;

    case 10:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                RestartStepSound();
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                Character_SetFace(this, GetFace());
                SetHeldItemsShowState(1);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 5);
            }
        } else {
            if (StepWalkLooking(0x8000, 0, 0x199, 0x22, 6)) {
                SetState(0);
            }
        }
        Character_SetWalkTarget(this, mSavedTarget);
        break;

    case 11:
        if (mStep == 0) {
            mStep++;
            {
                u16 turn = Random(0x2000) + 0x1800;
                u16 target;
                if (Random(100) < 50) {
                    target = mAngle - turn;
                } else {
                    target = mAngle + turn;
                }
                mTargetAngle = target;
            }
            StepTurnWithAnimation(GetWalkAnim());
        } else if (mStep == 1) {
            if (StepTurnWithAnimation(GetWalkAnim())) {
                mStep++;
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(0);
        }
        break;

    case 12:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound0UnlessGroupA();
            SetAnimation(RESOURCE_BCA0, 0x17);
            Character_SetFace(this, 6);
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            if (mModel.mBlendFrames > 0) {
                Character_ResetSoundPhase(this, 0);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x22, 9, 4);
        }
        break;

    case 13:
    case 27:
        break;

    case 14:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (IsAnimOfGroupB(mModel.mAnimId)) {
                StartSound0UnlessGroupA();
                SetAnimation(RESOURCE_BCA0, 0x1a);
            }
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            SetAnimation(RESOURCE_BCA0, 0x28);
            func_0208ef28(&mModel, TRUE, 0, FALSE);
            Character_SetFace(this, 6);
            Character_ResetSoundPhase(this, 0);
        } else if (mStep == 2) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (Random(0) & 1) {
                Character_SetFace(this, 7);
            }
        } else if (mStep == 3) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1f, 5, 2);
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                if (Random(10) < 8) {
                    Character_SetFace(this, 6);
                } else {
                    Character_SetFace(this, 4);
                }
                gSoundManager.StartSeqArc(1, 0x1f, 6);
            }
        } else if (mStep == 4) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            mTimer = 0x32;
            Character_SetFace(this, 8);
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0);
            }
        }
        break;

    case 15:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, 0x29);
            func_0208ef28(&mModel, TRUE, 0, FALSE);
            Character_SetFace(this, 1);
            SetHeldItemsShowState(1);
            SetHeadTurn(0, 0, 0, 5);
            mHeadIcon.Show();
            mHeadIcon.mIconId = 0xf;
            mHeadIcon.Reload();
            mHeadIcon.SetMode(3);
        } else if (mStep == 1) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            mTimer = 0;
            Character_SetFace(this, 7);
            gSoundManager.StartSeqArc(1, 0x22, 8);
        } else if (mStep == 2) {
            if (mTimer == 0) {
                ModelAnimation *anim = AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId);
                if (anim->mAnmObj->frame >= ((((NNSG3dResAnmCommon *)anim->mAnmObj->resAnm)->numFrame << 12) * 5) >> 3) {
                    mTimer++;
                    if (Random(10) < 8) {
                        Character_SetFace(this, 6);
                    } else {
                        Character_SetFace(this, 4);
                    }
                }
            }
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                mTimer = 0x3c;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                Character_SetFace(this, 8);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0);
            }
        }
        break;

    case 16:
    case 17:
    case 18:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound0UnlessGroupA();
            if (IsAnimOfGroupA(mModel.mAnimId) && (Random(0) & 1)) {
                SetAnimation(RESOURCE_BCA0, 0x1b);
            } else {
                SetAnimation(RESOURCE_BCA0, 0x19);
            }
            Character_SetFace(this, GetFace());
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            {
                u16 yaw;
                u16 pitch;
                if (mState == 0x11) {
                    yaw = (0x20 - Random(0x40)) << 8;
                    pitch = (0 - Random(0x10)) << 8;
                } else {
                    pitch = 0;
                    yaw = 0;
                }
                SetHeadTurn(pitch, yaw, 0, 5);
            }
        } else if (mState == 0x12) {
            if (!func_02091c80(this, mPartner->mModel.mPos.x, mPartner->mModel.mPos.z) && mPartner->mState != 8 &&
                mPartner->mState != 10 && mPartner->mState != 0x18 && mPartner->mState != 0x19) {
                break;
            }
            {
                u16 yaw = GetHeadYawTo(mPartner->GetFloorPos(), 0x3000);
                u16 pitch = 0;
                if (Random(10) < 5) {
                    pitch = GetHeadPitchToPartner(0, 0x1800);
                }
                SetHeadTurn(pitch, yaw, 0, 5);
                if (mPartner->mState == 0x18) {
                    if (mFace != 0x11) {
                        Character_SetFace(this, 0x11);
                    }
                } else {
                    int face = mFace;
                    if (face != GetFace()) {
                        Character_SetFace(this, GetFace());
                    }
                }
            }
        }
        break;

    case 19:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound0UnlessGroupA();
            if (Random(10) < 4) {
                if (IsAnimOfGroupA(mModel.mAnimId) && (Random(0) & 1)) {
                    SetAnimation(RESOURCE_BCA0, 0x1b);
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x19);
                }
            } else {
                SetAnimation(RESOURCE_BCA0, 0x1d);
            }
            Character_SetFace(this, 0x15);
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            if (mModel.mBlendFrames > 0) {
                Character_ResetSoundPhase(this, 0);
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mPartner->IsState10To12()) {
                mPartner->SetState(0x14);
            } else {
                mPartner->SetState(6);
            }
        } else {
            u16 yaw = GetHeadYawTo(mPartner->GetFloorPos(), 0x3800);
            SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), yaw, 0, 3);
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1a, 3, 1);
        }
        break;

    case 20:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound0UnlessGroupA();
            if (Random(10) < 4) {
                if (Random(0) & 1) {
                    SetAnimation(RESOURCE_BCA0, 0x19);
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x1b);
                }
            } else {
                SetAnimation(RESOURCE_BCA0, 0x1c);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
            }
            Character_SetFace(this, GetFace());
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            {
                u16 yaw = GetHeadYawTo(mPartner->GetFloorPos(), 0x3800);
                SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), yaw, 0, 4);
            }
        } else {
            if (mModel.mAnimId == 0x1c) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1a, 7, 2);
            }
        }
        break;

    case 21:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound0UnlessGroupA();
            SetAnimation(RESOURCE_BCA0, 0x18);
            Character_SetFace(this, 6);
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        }
        break;

    case 22:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            if (data_020fba28[mPersonality] == 0) {
                SetAnimation(RESOURCE_BCA0, 0x23);
            } else if (data_020fba28[mPersonality] == 1) {
                SetAnimation(RESOURCE_BCA0, 0x23);
            } else {
                SetAnimation(RESOURCE_BCA0, 0x24);
            }
            Character_SetFace(this, 6);
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            if (mModel.mBlendFrames > 0) {
                func_02091fa0(this);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x20, 1, 2);
            Character_PlaySoundAtAnimPhase(this, 1, 1, 0x20, 0, 8);
        }
        break;

    case 23:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, 0x2b);
            Character_SetFace(this, GetFace());
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            if (mModel.mBlendFrames > 0) {
                Character_ResetSoundPhase(this, 0);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x22, 0xf, 2);
        }
        break;

    case 24:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            // Both arms start the same animation, and the original has both calls. The compiler folds two arms that
            // are the same in the syntax tree into one, so they differed in the source in some way that leaves no
            // trace in the code. What that was is not known; the cast stands in for it.
            if (data_020fba28[mPersonality] == 0) {
                SetAnimation(RESOURCE_BCA0, 0x26);
            } else {
                ((RoomCharacterAlias *)this)->SetAnimation(RESOURCE_BCA0, 0x26);
            }
            func_0208ef28(&mModel, TRUE, 0, FALSE);
            if (Random(2) == 0) {
                Character_SetFace(this, 6);
            } else {
                Character_SetFace(this, 4);
            }
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            mTimer = 0x46;
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            ModelAnimation *anim = AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId);
            if (anim->mAnmObj->frame >= (((NNSG3dResAnmCommon *)anim->mAnmObj->resAnm)->numFrame << 12) >> 1) {
                int voice;
                mStep++;
                mTimer = 0x32;
                voice = func_02098e88(this, 6);
                if (voice == 0) {
                    gSoundManager.StartSeqArc(1, 0x22, 0);
                } else if (voice == 1) {
                    gSoundManager.StartSeqArc(1, 0x22, 1);
                } else if (voice == 2) {
                    gSoundManager.StartSeqArc(1, 0x22, 2);
                } else if (voice == 3) {
                    gSoundManager.StartSeqArc(1, 0x22, 3);
                } else if (voice == 4) {
                    gSoundManager.StartSeqArc(1, 0x22, 4);
                } else {
                    gSoundManager.StartSeqArc(1, 0x22, 5);
                }
            }
        } else if (mStep == 2) {
            mTimer--;
            if (mTimer <= 0) {
                mStep++;
                mTimer = 0x3c;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                Character_SetFace(this, 8);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0);
            }
        }
        break;

    case 25:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, 0x25);
            func_0208ef28(&mModel, TRUE, 0, FALSE);
            Character_SetFace(this, 4);
            SetHeldItemsShowState(1);
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            ModelAnimation *anim = AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId);
            if (anim->mAnmObj->frame >= (((NNSG3dResAnmCommon *)anim->mAnmObj->resAnm)->numFrame << 12) >> 2) {
                mStep++;
                gSoundManager.StartSeqArc(1, 0x22, 6);
            }
        } else if (mStep == 2) {
            ModelAnimation *anim = AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId);
            if (anim->mAnmObj->frame >= (((NNSG3dResAnmCommon *)anim->mAnmObj->resAnm)->numFrame << 12) >> 1) {
                mStep++;
                Character_SetFace(this, 7);
                mHeadIcon.Show();
                mHeadIcon.mIconId = 0xb;
                mHeadIcon.Reload();
                mHeadIcon.SetMode(3);
                gSoundManager.StartSeqArc(1, 0x22, 7);
            }
        } else if (mStep == 3) {
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                mTimer = 0x3c;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                Character_SetFace(this, 8);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0);
            }
        }
        break;

    case 26:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            if (data_020fba28[mPersonality] == 0) {
                SetAnimation(RESOURCE_BCA0, 0x15);
            } else if (data_020fba28[mPersonality] == 1) {
                SetAnimation(RESOURCE_BCA0, 0x15);
            } else {
                SetAnimation(RESOURCE_BCA0, 0x16);
            }
            func_0208ef28(&mModel, TRUE, 0, FALSE);
            Character_SetFace(this, 4);
            SetHeldItemsShowState(1);
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            ModelAnimation *anim = AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId);
            if (anim->mAnmObj->frame >= (((NNSG3dResAnmCommon *)anim->mAnmObj->resAnm)->numFrame << 12) >> 1) {
                mStep++;
                Character_SetFace(this, 7);
                mHeadIcon.Show();
                mHeadIcon.mIconId = 8;
                mHeadIcon.Reload();
                mHeadIcon.SetMode(3);
                gSoundManager.StartSeqArc(1, 0x20, 2);
            }
        } else if (mStep == 2) {
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                mTimer = 0x14;
            }
        } else if (mStep == 3) {
            mTimer--;
            if (mTimer <= 0) {
                mStep++;
                mTimer = 0x3c;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                Character_SetFace(this, 8);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0);
            }
        }
        break;

    case 28:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0x47);
                Character_SetFace(this, GetFace());
                SetHeldItemsShowState(1);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 5);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                Character_SetFace(this, 6);
                mTimer = Random(0x32) + 0x50;
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                mTimer = Random(0x1e) + 0x28;
                if (Random(0) & 1) {
                    Character_SetFace(this, 6);
                } else {
                    Character_SetFace(this, GetFace());
                }
            }
        }
        if (mModel.mAnimId == 0x47) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1f, 7, 0xe);
        }
        break;

    default:
        SetState(0);
        break;
    }
    PlaceHeadIcon();
}

// ===================================================================================================================
// RoomCharacterSoloStates1D

// The states 0x1d to 0x3f: what a character does on its own (docs/notes/ov074.md has the notes on the
// states). Each family has a chooser state (0x1d, 0x22, 0x29, 0x2e, 0x34) that picks the next
// state at random when the previous state was of the same family, and otherwise resets the animation and goes to
// the family's entry state.
//   0x1d..0x21 and 0x22..0x26: the same activity with held item 9 and with held item 8 (animations 0x3f..0x42)
//   0x27, 0x28: animation 6
//   0x29..0x2d: animations 0x5b/0x5c, or StepWalkToRandomPos
//   0x2e..0x33: with held item 6, placed by HeldItem::SetJoint every frame, standing and walking about
//   0x34..0x3c: with held items 2 and 3, after walking to data_ov074_0216d47c
//   0x3d..0x3f: walks to the corner of the walk area away from where it is and stays there (animation 0x17)
void RoomCharacter::UpdateSoloStates1DTo3F() {
    switch (mState) {
    case 0x1d:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if ((u32)(mPrevState - 0x1e) <= 2) {
                int r = Random(100);
                if (r < 60) {
                    SetState(0x1e);
                } else if (r < 80) {
                    SetState(0x1f);
                } else {
                    SetState(0x20);
                }
            } else {
                SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(0x21);
        }
        break;

    case 0x1e:
    case 0x23:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(mState == 0x1e ? 9 : 8, 0xff) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound0UnlessGroupA();
                SetAnimation(RESOURCE_BCA0, 0x3f);
                Character_SetFace(this, 8);
                mTimer = Random(100) + 0x1f4;
                mTimer2 = Random(0x3c) + 0x96;
                if (mState == 0x23) {
                    gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x25, 4);
                }
            }
        } else {
            mTimer--;
            if (mModel.mBlendFrames <= 0) {
                if (mTimer <= 0) {
                    if (mState == 0x1e) {
                        SetState(0x1d);
                    } else {
                        SetState(0x22);
                    }
                } else if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished || --mTimer2 <= 0) {
                    if (mState == 0x1e) {
                        mTimer2 = Random(60) + 0xaa;
                    } else {
                        mTimer2 = Random(60) + 0x104;
                    }
                    if (Random(100) < 70) {
                        SetAnimation(RESOURCE_BCA0, 0x3f);
                        Character_SetFace(this, GetFaceOfMood(Random(5)));
                    } else {
                        SetAnimation(RESOURCE_BCA0, 0x40);
                        func_0208ef00(&mModel, TRUE, FALSE);
                        Character_SetFace(this, 8);
                        Character_ResetSoundPhase(this, 0);
                    }
                }
            }
            if (mState == 0x1e && mModel.mAnimId == 0x40) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1c, 4, 2);
            }
        }
        PlaceHeldItemBetweenHands(0xcc);
        break;

    case 0x1f:
    case 0x24:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            RestartStepSound();
        } else if (mStep == 1) {
            if (StepChangeHeldItems(mState == 0x1f ? 9 : 8, 0xff) && mModel.mBlendFrames <= 0) {
                FloorPos pos;
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0x42);
                Character_SetFace(this, 8);
                GetRandomPosInArea(&pos);
                SetTarget(pos.x, 0, pos.z);
                mHeadTurnFrames = 5;
                mTimer = Random(100) + 0x1f4;
                if (mState == 0x24) {
                    gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x25, 4);
                }
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (mState == 0x1f) {
                    SetState(0x1d);
                } else {
                    SetState(0x22);
                }
            }
            if (StepWalkLooking(0x1000, 0, 0x199, 0x22, 6)) {
                FloorPos pos;
                if (Random(100) < 70) {
                    GetRandomEdgePos(&pos);
                } else {
                    GetRandomPosInArea(&pos);
                }
                SetTarget(pos.x, 0, pos.z);
                Character_SetFace(this, GetFaceOfMood(Random(5)));
            }
        }
        PlaceHeldItemBetweenHands(0xcc);
        break;

    case 0x20:
    case 0x25:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(mState == 0x20 ? 9 : 8, 0xff) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0x41);
                Character_SetFace(this, 8);
                mTimer = Random(100) + 0x190;
                if (mState == 0x25) {
                    gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x25, 4);
                }
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (mState == 0x20) {
                    SetState(0x1d);
                } else {
                    SetState(0x22);
                }
            }
        }
        PlaceHeldItemBetweenHands(0xcc);
        break;

    case 0x21:
    case 0x26:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x3b, mState == 0x21 ? 9 : 8, 0xff)) {
                mStep++;
            }
        } else {
            if (mModel.mBlendFrames <= 0 && AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                if (mState == 0x21) {
                    SetState(0x20);
                } else {
                    SetState(0x25);
                }
            }
        }
        PlaceHeldItemBetweenHands(0);
        break;

    case 0x22:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if ((u32)(mPrevState - 0x23) <= 2) {
                int r = Random(100);
                if (r < 60) {
                    SetState(0x23);
                } else if (r < 80) {
                    SetState(0x24);
                } else {
                    SetState(0x25);
                }
            } else {
                SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            SetState(0x26);
        }
        break;

    case 0x27:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x28) {
                SetState(0x28);
            } else if (IsAnimOfGroupB(mModel.mAnimId)) {
                SetAnimation(RESOURCE_BCA0, 0x1a);
            }
            SetHeldItemsShowState(1);
        } else {
            SetState(0x28);
        }
        break;

    case 0x28:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            mTimer2 = 100;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, 6);
            Character_SetFace(this, 8);
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0xd100, 0, 0, 5);
        } else {
            if (mTimer2-- <= 0) {
                if (Random(0) & 1) {
                    Character_SetFace(this, 1);
                    mTimer2 = 0x50;
                } else {
                    Character_SetFace(this, 6);
                    mTimer2 = 0x3c;
                }
            }
        }
        break;

    case 0x29:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            {
                int next;
                if (Random(100) < 10) {
                    next = 0x2c;
                } else {
                    next = Random(2) + 0x2a;
                }
                if (next == mPrevState || mPrevState == 0x2d) {
                    SetState(next);
                } else {
                    if (IsAnimOfGroupB(mModel.mAnimId)) {
                        SetAnimation(RESOURCE_BCA0, 0x1a);
                    }
                    SetState(0x2d);
                }
            }
            SetHeldItemsShowState(1);
        }
        break;

    case 0x2a:
    case 0x2b:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound0UnlessGroup();
            if (mModel.mAnimId >= 0) {
                if (mState == 0x2a) {
                    if (mModel.mAnimId != 0x5b) {
                        SetAnimation(RESOURCE_BCA0, 0x1a);
                    }
                } else {
                    if (mModel.mAnimId != 0x5c) {
                        SetAnimation(RESOURCE_BCA0, 0x1a);
                    }
                }
            }
            {
                int r = Random(3);
                if (r < 1) {
                    Character_SetFace(this, 8);
                } else if (r < 2) {
                    Character_SetFace(this, 0xc);
                } else {
                    Character_SetFace(this, 0xb);
                }
            }
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            mTimer = Random(0x50) + 0x12c;
            if (mState == 0x2a) {
                SetAnimation(RESOURCE_BCA0, 0x5b);
            } else {
                SetAnimation(RESOURCE_BCA0, 0x5c);
            }
            func_0208ef00(&mModel, TRUE, FALSE);
            if (mModel.mBlendFrames > 0) {
                Character_ResetSoundPhase(this, 0);
            }
        } else {
            mTimer--;
            if (mState == 0x2a) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 1, 4);
            } else {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 2, 2);
            }
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                if (mTimer <= 0) {
                    SetState(0x29);
                    Character_SetFace(this, 8);
                }
                func_0208ef28(&mModel, TRUE, 0, FALSE);
                Character_ResetSoundPhase(this, 0);
            }
        }
        break;

    case 0x2c:
        if (StepWalkToRandomPos()) {
            SetState(0x29);
        }
        break;

    case 0x2d:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (IsAnimOfGroupB(mModel.mAnimId)) {
                SetAnimation(RESOURCE_BCA0, 0x1a);
            }
            Character_SetFace(this, GetFace());
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (Random(2) >= 1) {
                mTimer = 1;
            } else {
                mTimer = Random(0x3c) + 0x28;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 7);
            }
        } else {
            mTimer--;
            if (mTimer <= 0 && mModel.mBlendFrames <= 0) {
                SetState(0x29);
            }
        }
        break;

    case 0x2e:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if ((u32)(mPrevState - 0x2f) <= 1) {
                if (Random(100) < 25) {
                    SetState(0x33);
                } else {
                    SetState(Random(2) + 0x2f);
                }
            } else if ((u32)(mPrevState - 0x32) <= 1) {
                SetState(0x32);
            } else {
                SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(0x31);
        }
        break;

    case 0x2f:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(6, 0xff) && mModel.mBlendFrames <= 0) {
                u16 angle;
                StartSound3IfAnimOfGroup();
                FloorPos here = GetFloorPos();
                angle = SnapAngle(FX_Atan2Idx(here.z, here.x));
                if (angle != mAngle) {
                    mStep++;
                    RestartStepSound();
                    if (mModel.mAnimId != 0x34 && mModel.mAnimId != 0x35) {
                        Character_ResetSoundPhase(this, 1);
                    }
                    if (mModel.mAnimId != 0x35) {
                        SetAnimation(RESOURCE_BCA0, 0x14);
                    } else {
                        SetAnimation(RESOURCE_BCA0, 0x35);
                    }
                    Character_SetFace(this, 8);
                    mTargetAngle = angle;
                } else {
                    mStep += 2;
                    SetAnimation(RESOURCE_BCA0, 0x34);
                    if (mModel.mBlendFrames > 0) {
                        Character_ResetSoundPhase(this, 0);
                    }
                }
                mHeldItems[0]->SetRotation(0xe000, 0, 0xf000);
            }
        } else if (mStep == 2) {
            u16 yaw;
            func_02091fbc(this, 1, 0, 0x1a);
            if (Character_TurnToTargetAngle(this, 0xe0, 0)) {
                mStep++;
                yaw = 0;
            } else {
                yaw = GetHeadYaw(-mWalkAngle, 0x2000);
            }
            SetHeadTurn(0, yaw, 0, 5);
        } else if (mStep == 3) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x34);
                Character_SetFace(this, 8);
                mHeldItems[0]->SetRotation(0xe000, 0, 0xf000);
            }
        } else if (mStep == 4) {
            mStep++;
            mTimer = Random(100) + 0xfa;
            {
                int r = Random(3);
                if (r == 0) {
                    Character_SetFace(this, 8);
                } else if (r == 1) {
                    Character_SetFace(this, 0x12);
                } else {
                    Character_SetFace(this, 0x10);
                }
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 25) {
                    SetState(0x33);
                } else {
                    SetState(Random(2) + 0x2f);
                }
            }
        }
        if (mModel.mAnimId == 0x34 || mModel.mAnimId == 0x35) {
            Character_PlaySoundAtAnimPhase(this, 1, 1, 0x1c, 1, 0x14);
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x30:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(6, 0xff) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                RestartStepSound();
                if (mModel.mAnimId != 0x34 && mModel.mAnimId != 0x35) {
                    Character_ResetSoundPhase(this, 1);
                }
                SetAnimation(RESOURCE_BCA0, 0x35);
                Character_SetFace(this, 8);
                mHeldItems[0]->SetRotation(0xe000, 0, 0xf000);
            }
        } else if (mStep == 2) {
            mStep++;
            mTimer = Random(0x3c) + 0x118;
            StepWalkLooking(0x8000, 0, 0x199, 0x22, 6);
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 25) {
                    SetState(0x33);
                } else {
                    SetState(Random(2) + 0x2f);
                }
            }
            if (StepWalkLooking(0x2400, -0x1800, 0x199, 0x22, 5)) {
                FloorPos target;
                FloorPos pos;
                target.x = mTarget.x;
                target.z = mTarget.z;
                GetWalkAreaCorner(target, &pos);
                SetTargetXZ(pos.x, pos.z);
            }
        }
        if (mModel.mAnimId == 0x34 || mModel.mAnimId == 0x35) {
            Character_PlaySoundAtAnimPhase(this, 1, 1, 0x1c, 1, 0x14);
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x31:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x20, 6, 0xff)) {
                mStep++;
                mHeldItems[0]->mRotX = 0xe000;
            }
        } else {
            if (mModel.mBlendFrames <= 0 && AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                SetState(0x32);
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x32:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                FloorPos pos;
                mStep++;
                StartSound3IfAnimOfGroup();
                RestartStepSound();
                SetAnimation(RESOURCE_BCA0, 0x14);
                Character_SetFace(this, 8);
                mHeldItems[0]->SetRotation(0xe000, 0, 0);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 5);
                GetRandomEdgePos(&pos);
                SetTarget(pos.x, 0, pos.z);
            }
        } else {
            if (StepWalkLooking(0x3800, 0, 0x199, 0x22, 6)) {
                SetState(0x2f);
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x33:
        if (mStep == 0) {
            mStep++;
            mHeldItems[0]->SetRotation(0xe000, 0, 0);
            FloorPos here = GetFloorPos();
            mTargetAngle = FX_Atan2Idx(here.z, here.x) + 0x8000;
            ShowHeadIcon();
        } else if (mStep == 1) {
            if (StepTurnWithAnimation(0x14)) {
                mStep++;
            }
            mHeldItems[0]->SetShowState(0);
        } else if (mStep == 2) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = Random(0x3c) + 0x78;
                SetAnimation(RESOURCE_BCA0, 7);
            }
        } else {
            if (mTimer-- <= 0) {
                SetState(0x32);
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x34:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mPrevState > 0x34 && mPrevState <= 0x3a) {
                int r = Random(100);
                if (r < 35) {
                    SetState(0x35);
                } else if (r < 70) {
                    SetState(0x36);
                } else {
                    SetState(0x38);
                }
            } else {
                SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(0x3c);
        }
        break;

    case 0x35:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 4);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(2, 3) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                if (Random(0) & 1) {
                    SetAnimation(RESOURCE_BCA0, 0x59);
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x5a);
                }
                func_0208ef00(&mModel, TRUE, FALSE);
                Character_SetFace(this, 8);
                PlaceHeldItemsAtStart();
                mTimer = Random(300) + 0x5dc;
                mTimer2 = Random(2) + 1;
                mTimer3 = Random(100) + 200;
            }
        } else {
            mTimer--;
            if (mModel.mBlendFrames <= 0) {
                if (mTimer <= 0) {
                    SetState(0x39);
                    Character_SetFace(this, 8);
                    SetAnimation(RESOURCE_BCA0, 0x59);
                    func_0208ef00(&mModel, FALSE, FALSE);
                } else if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                    mTimer2--;
                    if (mTimer2 <= 0) {
                        int face;
                        mTimer2 = Random(2) + 1;
                        face = Random(100);
                        if (Random(100) < 50) {
                            SetAnimation(RESOURCE_BCA0, 0x59);
                            Character_SetFace(this, 8);
                        } else {
                            SetAnimation(RESOURCE_BCA0, 0x5a);
                            if (face < 50) {
                                Character_SetFace(this, 0x15);
                            } else {
                                Character_SetFace(this, 8);
                            }
                        }
                        func_0208ef28(&mModel, TRUE, 0, FALSE);
                    } else {
                        func_0208ef28(&mModel, TRUE, 0, FALSE);
                    }
                } else {
                    mTimer3--;
                    if (mTimer3 <= 0) {
                        mTimer3 = Random(0x118) + 0x1e;
                        mTimer3 += ((0x1000 - mModel.mUnknownD8) * 0x3c) >> 12;
                        if (mModel.mAnimId == 0x5a) {
                            SetAnimation(RESOURCE_BCA0, 0x59);
                            Character_SetFace(this, 8);
                            func_0208ef00(&mModel, TRUE, FALSE);
                        }
                    }
                }
            }
        }
        if (mModel.mAnimId == 0x5a) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x25, 1, 4);
        } else {
            Character_ResetSoundPhase(this, 0);
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x36:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 4);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(2, 3) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                if (Random(0) & 1) {
                    SetAnimation(RESOURCE_BCA0, 0x2f);
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x30);
                }
                Character_SetFace(this, 8);
                PlaceHeldItemsAtStart();
                mTimer = Random(300) + 0x5dc;
                mTimer2 = Random(200) + 0x32;
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (mModel.mBlendFrames <= 0) {
                    SetState(0x39);
                    SetAnimation(RESOURCE_BCA0, 0x2f);
                    Character_SetFace(this, 8);
                }
            } else if (mModel.mBlendFrames <= 0) {
                mTimer2--;
                if (mTimer2 <= 0) {
                    int face;
                    mTimer2 = Random(0x96) + 0x78;
                    mTimer2 += ((0x1000 - mModel.mUnknownD8) * 0x32) >> 12;
                    face = Random(100);
                    if (Random(100) < 50) {
                        SetAnimation(RESOURCE_BCA0, 0x2f);
                        Character_SetFace(this, 8);
                    } else {
                        SetAnimation(RESOURCE_BCA0, 0x30);
                        if (face < 20) {
                            Character_SetFace(this, 0x15);
                        } else if (face < 40) {
                            Character_SetFace(this, 0xb);
                        } else if (face < 60) {
                            Character_SetFace(this, 0x10);
                        } else {
                            Character_SetFace(this, 8);
                        }
                    }
                }
            }
        }
        if (mModel.mAnimId == 0x30) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x25, 3, 10);
        } else {
            Character_ResetSoundPhase(this, 0);
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x37:
    case 0x38:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 4);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(2, 3) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0x2d);
                Character_SetFace(this, 8);
                PlaceHeldItemsAtStart();
                if (mState == 0x37) {
                    mTimer = Random(4) + 0xe;
                } else {
                    mTimer = 0;
                }
                mTimer2 = Random(0x3c) + 0x3c;
            }
        } else if (mStep == 2) {
            if (mModel.mBlendFrames <= 0) {
                mTimer2--;
                if (mTimer2 <= 0) {
                    mStep++;
                    mTimer2 = 0xfa;
                    SetAnimation(RESOURCE_BCA0, 0x2d);
                    Character_SetFace(this, 8);
                }
            }
        } else if (mStep == 3) {
            if (mModel.mBlendFrames <= 0) {
                mTimer2--;
                if (mTimer2 <= 0 || gSoundManager.CountPlayingSeqArcEntry(0x25, 2) == 0) {
                    mStep++;
                    mTimer2 = Random(0x32) + 0x3c;
                    func_0208ef00(&mModel, TRUE, FALSE);
                    Character_SetFace(this, 8);
                }
            }
        } else if (mStep == 4) {
            if (mModel.mBlendFrames <= 0 && AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mTimer2--;
                if (mTimer2 <= 0) {
                    mStep++;
                    mTimer2 = Random(0x1e) + 0x32;
                    SetAnimation(RESOURCE_BCA0, 0x2e);
                    func_0208ef28(&mModel, TRUE, 0, FALSE);
                    mTimer++;
                }
            }
        } else if (mStep == 5) {
            if (mModel.mBlendFrames <= 0 && AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mTimer2--;
                if (mTimer2 <= 0) {
                    if (mTimer >= 0x15 || (mTimer >= 0xb && Random(100) < (mTimer * 3) >> 3)) {
                        mStep++;
                        mTimer = 300;
                        mTimer2 = 0;
                        SetAnimation(RESOURCE_BCA0, 0x2d);
                        Character_SetFace(this, 8);
                    } else {
                        int r;
                        mStep = 3;
                        mTimer2 = Random(0x32) + 0xfa;
                        SetAnimation(RESOURCE_BCA0, 0x2d);
                        r = Random(100);
                        if (r < 15) {
                            Character_SetFace(this, 2);
                        } else if (r < 30) {
                            Character_SetFace(this, 4);
                        } else if (r < 45) {
                            Character_SetFace(this, 6);
                        } else if (r < 60) {
                            Character_SetFace(this, 5);
                        } else {
                            Character_SetFace(this, 8);
                        }
                    }
                }
            }
        } else {
            mTimer2++;
            if (mTimer2 > 0x3c) {
                mTimer--;
                if (mTimer <= 0 || gSoundManager.CountPlayingSeqArcEntry(0x25, 2) == 0) {
                    SetState(0x39);
                }
            }
        }
        if (mModel.mAnimId == 0x2e) {
            Character_PlaySoundAtAnimPhase(this, 0, mSoundHandle, 0x25, 2, 4);
        } else {
            Character_ResetSoundPhase(this, 0);
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x39:
        if (mStep == 0) {
            mStep++;
            mTimer = Random(0x14) + 0x32;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 4);
            Character_SetFace(this, 8);
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mTimer--;
                if (mTimer <= 0) {
                    int r;
                    gSoundManager.StopSeqArc(1, 0);
                    Character_ResetSoundPhase(this, 0);
                    mStep++;
                    r = Random(100);
                    if (r < 25) {
                        SetAnimation(RESOURCE_BCA0, 0x2a);
                        Character_SetFace(this, 2);
                        mTimer2 = Random(2) + 2;
                        Character_ResetSoundPhase(this, 0);
                    } else if (r < 50) {
                        SetAnimation(RESOURCE_BCA0, 0x16);
                        Character_SetFace(this, 4);
                        mTimer2 = 1;
                    } else if (r < 75) {
                        SetAnimation(RESOURCE_BCA0, 0x54);
                        mTimer2 = Random(3) + 1;
                        Character_ResetSoundPhase(this, 0);
                        if (mTimer2 == 1) {
                            Character_SetFace(this, 5);
                        } else {
                            Character_SetFace(this, 6);
                        }
                    } else {
                        SetAnimation(RESOURCE_BCA0, 0x43);
                        mTimer2 = 1;
                        Character_SetFace(this, 1);
                    }
                    func_0208ef00(&mModel, TRUE, FALSE);
                    ShowHeadIcon();
                    SetHeadTurn(0, 0, 0, 5);
                }
            }
        } else if (mStep == 2) {
            if (mModel.mBlendFrames <= 0 && AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mTimer2--;
                if (mTimer2 <= 0) {
                    mStep++;
                    if (mModel.mAnimId == 0x16) {
                        mTimer = 0x32;
                    } else if (mModel.mAnimId == 0x43) {
                        mTimer = 0x6e;
                    } else {
                        mTimer = 0;
                    }
                } else {
                    func_0208ef28(&mModel, TRUE, 0, FALSE);
                }
            }
            if (mModel.mAnimId == 0x2a) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1f, 0, 2);
            } else if (mModel.mAnimId == 0x54) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x20, 3, 2);
            }
        } else if (mStep == 3) {
            if (mModel.mBlendFrames <= 0) {
                mTimer--;
                if (mTimer <= 0) {
                    mStep++;
                    mTimer = Random(0x50) + 0x78;
                    SetAnimation(RESOURCE_BCA0, 7);
                    Character_SetFace(this, 8);
                }
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x3a);
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x3a:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                mTimer = Random(0x3c) + 0x96;
                SetAnimation(RESOURCE_BCA0, 0x4d);
                Character_SetFace(this, 8);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 4);
                gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x25, 0);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                int r = Random(100);
                if (r < 35) {
                    SetState(0x35);
                } else if (r < 70) {
                    SetState(0x36);
                } else {
                    SetState(0x38);
                }
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x3b:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x20, 2, 3)) {
                mStep++;
                AnimModel_FinishAnimations(mHeldItems[1]);
                PlaceHeldItemsAtStart();
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0 && AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                mTimer = 0x5a;
                SetAnimation(RESOURCE_BCA0, 0x4d);
                gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x25, 0);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                int r = Random(100);
                if (r < 35) {
                    SetState(0x35);
                } else if (r < 70) {
                    SetState(0x36);
                } else {
                    SetState(0x38);
                }
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x3c:
        if (mStep == 0) {
            if (StepWalkTo(data_ov074_0216d47c)) {
                mStep += 2;
                mSubStep = 0;
                mTargetAngle = 0x4000;
            }
        } else if (mStep == 2) {
            u16 yaw;
            func_02091fbc(this, 1, 0, 0x1a);
            if (Character_TurnToTargetAngle(this, 0xa0, 0)) {
                mStep++;
                yaw = 0;
            } else {
                yaw = GetHeadYaw(-mTargetAngle, 0x3800);
            }
            SetHeadTurn(0, yaw, 0, 4);
            Character_MoveAlongAngle(this);
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(0x3b);
            SetAnimation(RESOURCE_BCA0, 7);
        }
        break;

    case 0x3d:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x3e) {
                SetState(0x3e);
            } else if (IsAnimOfGroupB(mModel.mAnimId)) {
                SetAnimation(RESOURCE_BCA0, 0x1a);
            }
            SetHeldItemsShowState(1);
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(0x3f);
        }
        break;

    case 0x3e:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound0UnlessGroupA();
            SetAnimation(RESOURCE_BCA0, 0x17);
            func_0208ef28(&mModel, TRUE, 0x24000, TRUE);
            Character_SetFace(this, 8);
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            mTimer = 500;
        } else {
            mTimer--;
            if (mTimer <= 0) {
                mTimer = 500;
                ShowHeadIcon();
            }
        }
        break;

    case 0x3f:
        if (mStep == 0) {
            fx32 x;
            fx32 z;
            mStep++;
            FloorRect *area = &gWalkArea;
            x = mModel.mPos.x < 0 ? area->x : -area->x;
            z = mModel.mPos.z < 0 ? area->z : -area->z;
            SetTarget(x, 0, z);
        } else if (mStep == 1) {
            if (StepWalkTo(GetTargetFloorPos())) {
                mStep++;
                mSubStep = 0;
            }
        } else if (mStep == 2) {
            mStep++;
            FloorPos twice = GetTargetFloorPos();
            twice.x *= 2;
            twice.z *= 2;
            mTargetAngle = FX_Atan2Idx(twice.z, twice.x);
            func_02091fbc(this, 1, 0, 0x1a);
        } else {
            u16 yaw;
            func_02091fbc(this, 1, 0, 0x1a);
            if (Character_TurnToTargetAngle(this, 0xa0, 0)) {
                SetState(0x3e);
                yaw = 0;
            } else {
                yaw = GetHeadYaw(-mWalkAngle, 0x3800);
                mHeadTurnActive = TRUE;
            }
            SetHeadTurn(0, yaw, 0, 5);
            Character_MoveAlongAngle(this);
        }
        break;
    }
    PlaceHeadIcon();
}

// ===================================================================================================================
// RoomCharacterSoloStates40

// The states 0x40 to 0x67: ten activities of one character on its own. Each has a hub state (0x40, 0x44, 0x48,
// 0x4f, 0x54, 0x58, 0x5b, 0x5e, 0x61, 0x65) that waits for the animation change and then either goes on with the
// activity, when the state before was one of its own, or resets the animation (0x1a) and enters it.
void RoomCharacter::UpdateSoloStates40To67() {
    switch (mState) {
    case 0x40:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x41 || mPrevState == 0x42 || mPrevState == 0x43) {
                SetState(Random(3) + 0x41);
            } else if (IsAnimOfGroupB(mModel.mAnimId)) {
                SetAnimation(RESOURCE_BCA0, 0x1A);
            }
            SetHeldItemsShowState(1);
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(Random(3) + 0x41);
        }
        break;

    case 0x41:
    case 0x42:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            mTimer = Random(100) + 600;
            mTimer2 = 0;
            mTimer3 = 200;
            if (mState == 0x41) {
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 2);
            } else {
                StartSound0UnlessGroupA();
                SetAnimation(RESOURCE_BCA0, 0x19);
            }
            Character_SetFace(this, 0x12);
            SetHeldItemsShowState(1);
            SetHeadTurn(0, 0, 0, 5);
            ShowHeadIcon();
        } else {
            if (mTimer2-- <= 0) {
                mTimer2 = 150;
                if (Random(2) == 0) {
                    Character_SetFace(this, 9);
                } else {
                    Character_SetFace(this, 10);
                }
            }
            if (mTimer3-- <= 0) {
                mTimer3 = 1000;
                if (Random(100) < 30) {
                    SetHeadTurn(0, (0x28 - Random(0x50)) << 8, 0, 5);
                    mHeadTurnActive = 1;
                }
            }
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x40);
                Character_SetFace(this, 8);
            }
        }
        break;

    case 0x43:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            mTimer = Random(100) + 600;
            mTimer2 = 0;
            StartSound3IfAnimOfGroup();
            RestartStepSound();
            SetAnimation(RESOURCE_BCA0, 0xC);
            Character_SetFace(this, 0x12);
            SetHeldItemsShowState(1);
            SetHeadTurn(0, 0, 0, 5);
            ShowHeadIcon();
            FloorPos pos;
            GetRandomPosInArea(&pos);
            SetTargetOnFloor(pos.x, pos.z);
        } else {
            if (StepWalkLooking(0x3000, 0, 0x199, 0x22, 6)) {
                FloorPos pos;
                if (Random(100) < 60) {
                    GetRandomEdgePos(&pos);
                } else {
                    GetRandomPosInArea(&pos);
                }
                SetTargetOnFloor(pos.x, pos.z);
            }
            if (mTimer2-- <= 0) {
                mTimer2 = 150;
                if (Random(2) == 0) {
                    Character_SetFace(this, 9);
                } else {
                    Character_SetFace(this, 10);
                }
            }
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x40);
                Character_SetFace(this, 8);
            }
        }
        break;

    case 0x44:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x45 || mPrevState == 0x46) {
                if (Random(10) < 7) {
                    SetState(0x45);
                } else {
                    SetState(0x46);
                }
            } else {
                SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1A);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(0x47);
        }
        break;

    case 0x45:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(7, 0xFF) && mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = Random(100) + 700;
                int r = Random(100);
                if (r < 30) {
                    StartSound3IfAnimOfGroup();
                    SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                } else if (r < 33) {
                    StartSound3IfAnimOfGroup();
                    mTimer >>= 2;
                    SetAnimation(RESOURCE_BCA0, 0x4B);
                } else {
                    StartSound0UnlessGroupA();
                    if (mMood == 3) {
                        SetAnimation(RESOURCE_BCA0, 0x18);
                    } else if (Random(100) < 50) {
                        SetAnimation(RESOURCE_BCA0, 0x19);
                    } else {
                        SetAnimation(RESOURCE_BCA0, 0x1B);
                    }
                }
                Character_SetFace(this, GetFace());
                mHeldItems[0]->mRotZ = 0x4000;
                gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x1C, 5);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                mKeepSound = 1;
                if (Random(10) < 7) {
                    SetState(0x45);
                } else {
                    SetState(0x46);
                }
            }
        }
        PlaceHeldItemAtHead();
        break;

    case 0x46:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            RestartStepSound();
        } else if (mStep == 1) {
            if (StepChangeHeldItems(7, 0xFF) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                Character_SetFace(this, GetFace());
                mHeldItems[0]->mRotZ = 0x4000;
                FloorPos pos;
                GetRandomPosInArea(&pos);
                SetTargetOnFloor(pos.x, pos.z);
                mTimer = Random(100) + 300;
                gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x1C, 5);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                mKeepSound = 1;
                if (Random(10) < 7) {
                    SetState(0x45);
                } else {
                    SetState(0x46);
                }
            }
            if (StepWalkLooking(0x3800, 0, 0x199, 0x22, 6)) {
                FloorPos pos;
                if (Random(100) < 70) {
                    GetRandomEdgePos(&pos);
                } else {
                    GetRandomPosInArea(&pos);
                }
                SetTargetOnFloor(pos.x, pos.z);
            }
        }
        PlaceHeldItemAtHead();
        break;

    case 0x47:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x20, 7, 0xFF)) {
                mStep++;
                mHeldItems[0]->mRotZ = 0x4000;
            }
        } else if (mModel.mBlendFrames <= 0) {
            if (IsAnimationFinished()) {
                SetState(0x45);
            }
        }
        mHeldItems[0]->mRotX = mUnknown25D6;
        mHeldItems[0]->SetJointOffset(mModel.GetJointMatrix(2), mModel.GetJointRotation(0), 0x666, 0, 0);
        break;

    case 0x48:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x49 || mPrevState == 0x4D || mPrevState == 0x4E) {
                SetState(0x49);
            } else if (mPrevState == 0x4B || mPrevState == 0x4C) {
                SetState(0x4C);
            } else {
                SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1A);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(0x4A);
        }
        break;

    case 0x49:
    case 0x4C:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(1, 0xFF) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSoundUnlessGroupB(0);
                SetAnimation(RESOURCE_BCA0, 0x3D);
                Character_SetFace(this, 8);
                mHeldItems[0]->mRotY = 0xC000 - mAngle;
                if (mState == 0x4C) {
                    VecFx32 pos;
                    GetMidpoint(GetNode(2), GetNode(1), &pos);
                    pos.y = 0xCC;
                    mHeldItems[0]->SetPosition(pos);
                } else {
                    gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x1C, 2);
                }
            }
        } else if (mStep == 2) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
            }
            if (mState == 0x4C) {
                VecFx32 pos;
                GetMidpoint(GetNode(2), GetNode(1), &pos);
                pos.y = 0xCC;
                mHeldItems[0]->SetPosition(pos);
                mTimer = 1;
            } else {
                mTimer = Random(100) + 300;
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (mState == 0x4C) {
                    SetState(0x49);
                } else {
                    int r = Random(100);
                    if (r < 70) {
                        SetState(0x49);
                    } else if (r < 90) {
                        SetState(0x4D);
                    } else {
                        SetState(0x4E);
                    }
                }
            }
        }
        break;

    case 0x4D:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, 0x28);
            func_0208ef00(&mModel, 1, 0);
            Character_SetFace(this, 6);
            AnimModel_FinishAnimations(mHeldItems[0]);
            SetHeadTurn(0, 0, 0, 5);
            ShowHeadIcon();
            Character_ResetSoundPhase(this, 0);
        } else if (mStep == 1) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (Random(0) & 1) {
                Character_SetFace(this, 7);
            }
        } else if (mStep == 2) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 5, 2);
            if (IsAnimationFinished()) {
                mStep++;
                mTimer = 20;
                Character_SetFace(this, 6);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x49);
                Character_SetFace(this, 8);
                gSoundManager.StartSeqArc(1, 0x1F, 6);
            }
        }
        break;

    case 0x4E:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            SetAnimation(RESOURCE_BCA0, 0x3D);
            func_0208ef00(&mModel, 1, 0);
            SetHeadTurn(0, 0, 0, 5);
            ShowHeadIcon();
        } else if (mStep == 1) {
            if (IsAnimationFinished()) {
                mStep++;
                mTimer = 110;
                Character_SetFace(this, 5);
                AnimModel_FinishAnimations(mHeldItems[0]);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x49);
                Character_SetFace(this, 8);
            }
        }
        break;

    case 0x4A:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x20, 1, 0xFF)) {
                mStep++;
                mHeldItems[0]->mRotY = 0xC000 - mAngle;
                AnimModel_FinishAnimations(mHeldItems[0]);
            }
        } else if (mModel.mBlendFrames <= 0) {
            if (IsAnimationFinished()) {
                SetState(0x4B);
            }
        }
        {
            VecFx32 pos;
            GetMidpoint(GetNode(2), GetNode(1), &pos);
            pos.y = mModel.GetJointMatrix(2)->m[3][1];
            mHeldItems[0]->SetPosition(pos);
        }
        break;

    case 0x4B:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x3C);
                func_0208ef28(&mModel, 1, 0, 0);
                Character_SetFace(this, 8);
                mHeldItems[0]->mRotY = 0xC000 - mAngle;
                MTX_Identity33(&mHeldItems[0]->mRotation);
                SetHeadTurn(0, 0, 0, 5);
                ShowHeadIcon();
            }
        } else {
            if (IsAnimationFinished()) {
                SetState(0x4C);
            }
        }
        {
            VecFx32 pos;
            GetMidpoint(GetNode(2), GetNode(1), &pos);
            pos.y = mModel.GetJointMatrix(2)->m[3][1];
            mHeldItems[0]->SetPosition(pos);
        }
        break;

    case 0x4F:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            int next;
            if (Random(100) < 10) {
                next = 0x53;
            } else {
                next = Random(3) + 0x50;
            }
            if (next == mPrevState) {
                SetState(next);
            } else {
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1A);
                }
                mCounter = next;
            }
            SetHeadTurn(0, 0, 0, 5);
            SetHeldItemsShowState(1);
        } else if (mStep == 1) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (Random(2) == 0) {
                mTimer = 1;
                break;
            }
            mTimer = Random(30) + 60;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, 7);
        } else {
            mTimer--;
            if (mTimer <= 0 && mModel.mBlendFrames <= 0) {
                SetState(mCounter);
            }
        }
        break;

    case 0x50:
        if (mStep == 0) {
            if (StepAlternateAnimations(0x4E, 0x4F)) {
                SetState(0x4F);
            }
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 0, 2);
        }
        break;

    case 0x51:
        if (mStep == 0) {
            if (StepAlternateAnimations(0x50, 0x51)) {
                SetState(0x4F);
            }
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 5, 2);
        }
        break;

    case 0x52:
        if (mStep == 0) {
            if (StepAlternateAnimations(0x52, 0x52)) {
                SetState(0x4F);
            }
            ModelAnimation *anim = GetCurrentAnimation();
            if (anim != NULL) {
                if (anim->mAnmObj->frame < (((NNSG3dResAnmCommon *)anim->mAnmObj->resAnm)->numFrame << FX32_SHIFT) >> 1) {
                    Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 3, 2);
                } else {
                    Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 4, 8);
                }
            }
        }
        break;

    case 0x53:
        if (StepWalkToRandomPos()) {
            SetState(0x4F);
        }
        break;

    case 0x54:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x55 || mPrevState == 0x56) {
                SetState(0x55);
            } else {
                SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1A);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(0x57);
        }
        break;

    case 0x55:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 4);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(9, 5) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound0UnlessGroupA();
                SetAnimation(RESOURCE_BCA0, 0x53);
                Character_SetFace(this, 8);
                mHeldItems[1]->mRotZ = 0x8000;
                mTimer = Random(100) + 500;
                mTimer2 = Random(60) + 140;
                gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x1C, 3);
                UpdateVoiceOfSound();
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 80 || ClampToRect4E4(NULL)) {
                    SetState(0x55);
                } else {
                    SetState(0x56);
                }
            } else if (mModel.mBlendFrames <= 0) {
                mTimer2--;
                if (mTimer2 <= 0) {
                    mTimer2 = Random(60) + 140;
                    if (IsAnimationFinished()) {
                        func_0208ef00(&mModel, 0, 0);
                        SetHeadTurn(0, 0, 0, 4);
                        Character_SetFace(this, 8);
                        gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x1C, 3);
                        UpdateVoiceOfSound();
                    } else if (Random(100) < 20) {
                        func_0208ef00(&mModel, 1, 1);
                        SetHeadTurn(0xE800, 0, 0, 4);
                        Character_SetFace(this, 0x12);
                        gSoundManager.StopSeqArc(mSoundHandle, 0);
                    }
                }
            }
        }
        PlaceHeldItemsInHands();
        break;

    case 0x56:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(9, 5) && mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x46);
                Character_SetFace(this, 6);
                mHeldItems[1]->mRotZ = 0x8000;
                mTimer = Random(60) + 330;
                if (Random(0) & 1) {
                    Character_SetFace(this, 7);
                }
                gSoundManager.StartSeqArc(1, 0x1E, 1);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x55);
            } else if (mTimer == 160) {
                Character_SetFace(this, 8);
            } else if (mTimer == 270) {
                Character_SetFace(this, 6);
            }
        }
        PlaceHeldItemsInHands();
        break;

    case 0x57:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x3B, 9, 5)) {
                mStep++;
                AnimModel_FinishAnimations(mHeldItems[1]);
                mHeldItems[1]->mRotZ = 0x8000;
            }
        } else if (mModel.mBlendFrames <= 0) {
            if (IsAnimationFinished()) {
                SetState(0x55);
            }
        }
        PlaceHeldItemsInHands();
        break;

    case 0x58:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x59) {
                SetState(0x59);
            } else if (IsAnimOfGroupB(mModel.mAnimId)) {
                SetAnimation(RESOURCE_BCA0, 0x1A);
            }
            SetHeldItemsShowState(1);
        } else {
            SetState(0x5A);
        }
        break;

    case 0x59:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            mTimer = Random(100) + 550;
            BOOL wasGroupB = IsAnimOfGroupB(mModel.mAnimId);
            StartSoundUnlessGroupB(1);
            int r = Random(4);
            if (r == 0) {
                SetAnimation(RESOURCE_BCA0, 0x44);
            } else if (r == 1) {
                SetAnimation(RESOURCE_BCA0, 0x45);
            } else if (r == 2) {
                SetAnimation(RESOURCE_BCA0, 0x46);
            } else {
                SetAnimation(RESOURCE_BCA0, 0x27);
            }
            if (mModel.mBlendFrames > 0 && wasGroupB) {
                gSoundManager.StartSeqArc(1, 0x1E, 2);
            }
            Character_SetFace(this, 0x12);
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x59);
            }
        }
        break;

    case 0x5A: {
        if (GetFloorPos().LengthSq() < 0x4000) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(0x59);
            SetAnimation(RESOURCE_BCA0, 0x1A);
        } else {
            mTarget.x = 0;
            mTarget.z = 0;
            StepWalkTo(GetTargetFloorPos());
        }
        break;
    }

    case 0x5B:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x5C || mPrevState == 0x5D) {
                if (Random(100) < 60) {
                    SetState(0x5C);
                } else {
                    SetState(0x5D);
                }
            } else if (IsAnimOfGroupB(mModel.mAnimId)) {
                SetAnimation(RESOURCE_BCA0, 0x1A);
            }
            SetHeldItemsShowState(1);
        } else {
            SetState(0x5C);
        }
        break;

    case 0x5C:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            mTimer = Random(100) + 500;
            mTimer2 = Random(60) + 120;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, 5);
            Character_SetFace(this, 0x10);
            SetHeldItemsShowState(1);
            SetHeadTurn(0, 0, 0, 5);
            ShowHeadIcon();
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x5B);
            }
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mTimer2--;
            if (mTimer2 > 0) {
                break;
            }
            mTimer2 = Random(60) + 140;
            if (Random(100) < 30) {
                Character_SetFace(this, 0x10);
                SetAnimation(RESOURCE_BCA0, 4);
            } else {
                Character_SetFace(this, 4);
                SetAnimation(RESOURCE_BCA0, 5);
            }
        }
        break;

    case 0x5D:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            FloorPos pos;
            GetRandomPosInArea(&pos);
            SetTargetOnFloor(pos.x, pos.z);
            RestartStepSound();
            mStep++;
            mTimer = Random(100) + 200;
            mTimer2 = Random(60) + 120;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, 0xF);
            Character_SetFace(this, 0x10);
            SetHeldItemsShowState(1);
            SetHeadTurn(0, 0, 0, 5);
            ShowHeadIcon();
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x5B);
            }
            mTimer2--;
            if (mTimer2 <= 0) {
                mTimer2 = Random(60) + 150;
                if (Random(100) < 40) {
                    Character_SetFace(this, 0x10);
                } else {
                    Character_SetFace(this, 4);
                }
            }
            if (StepWalkLooking(0x1000, 0, 0x199, 0x22, 6)) {
                FloorPos pos;
                if (Random(100) < 70) {
                    GetRandomEdgePos(&pos);
                } else {
                    GetRandomPosInArea(&pos);
                }
                SetTargetOnFloor(pos.x, pos.z);
            }
        }
        break;

    case 0x5E:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x5F) {
                SetState(0x5F);
            } else if (IsAnimOfGroupB(mModel.mAnimId)) {
                SetAnimation(RESOURCE_BCA0, 0x1A);
            }
            SetHeldItemsShowState(1);
        } else {
            SetState(0x60);
        }
        break;

    case 0x5F:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            mTimer = Random(100) + 700;
            mTimer2 = 120;
            BOOL wasGroupB = IsAnimOfGroupB(mModel.mAnimId);
            StartSoundUnlessGroupB(1);
            int r = Random(100);
            if (r < 60) {
                SetAnimation(RESOURCE_BCA0, 0x27);
            } else if (r < 80) {
                SetAnimation(RESOURCE_BCA0, 0x44);
            } else {
                SetAnimation(RESOURCE_BCA0, 0x45);
            }
            if (mModel.mBlendFrames > 0 && wasGroupB) {
                gSoundManager.StartSeqArc(1, 0x1E, 2);
            }
            Character_SetFace(this, 6);
            SetHeldItemsShowState(1);
            SetHeadTurn(0, 0, 0, 5);
            ShowHeadIconOfRecord();
            mHeadIcon.SetMode(3);
        } else {
            if (func_02081024(&mHeadIcon, mHeadIcon.mShown->mFrame) == 8) {
                PlayRandomSoundOfFour();
            }
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x5F);
                break;
            }
            mTimer2--;
            if (mTimer2 > 0) {
                break;
            }
            if (Random(100) < 80) {
                mTimer2 = 200;
                Character_SetFace(this, 6);
            } else {
                mTimer2 = 60;
                Character_SetFace(this, 2);
            }
        }
        break;

    case 0x60: {
        if (GetFloorPos().LengthSq() < 0x4000) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(0x5F);
            SetAnimation(RESOURCE_BCA0, 0x1A);
        } else {
            mTarget.x = 0;
            mTarget.z = 0;
            StepWalkTo(GetTargetFloorPos());
        }
        break;
    }

    case 0x61:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x62 || mPrevState == 0x63) {
                SetState(Random(2) + 0x62);
            } else {
                SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1A);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(0x64);
        }
        break;

    case 0x62:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(0, 0xFF) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0x21);
                Character_SetFace(this, 8);
                mHeldItems[0]->SetRotation(0, 0x7000, 0x1000);
                mTimer = Random(200) + 800;
                mTimer2 = 60;
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
            }
        } else {
            if (mTimer2-- <= 0) {
                mTimer2 = 100;
                if (Random(100) < 90) {
                    if (Random(2) == 0) {
                        Character_SetFace(this, 8);
                    } else {
                        Character_SetFace(this, 0x14);
                    }
                } else {
                    if (Random(2) == 0) {
                        Character_SetFace(this, 0xE);
                    } else {
                        Character_SetFace(this, 0x16);
                    }
                }
            }
            if (mFace == 0x14 || mFace == 0x16) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 3, 10);
            } else {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 7, 2);
            }
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 40) {
                    SetState(0x62);
                } else {
                    SetState(0x63);
                }
                Character_SetFace(this, 8);
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x63:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            RestartStepSound();
        } else if (mStep == 1) {
            if (StepChangeHeldItems(0, 0xFF) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0x22);
                Character_SetFace(this, 0x15);
                mHeldItems[0]->SetRotation(0, 0x7800, 0x1000);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 1);
                }
                FloorPos pos;
                GetRandomPosInArea(&pos);
                SetTargetOnFloor(pos.x, pos.z);
                mHeadTurnFrames = 5;
                mTimer = Random(200) + 800;
                mTimer2 = 0;
            }
        } else {
            if (mTimer2-- <= 0) {
                mTimer2 = 100;
                if (Random(100) < 70) {
                    Character_SetFace(this, 0x15);
                } else {
                    Character_SetFace(this, 0x16);
                }
            }
            Character_PlaySoundAtAnimPhase(this, 1, 1, 0x1A, 3, 10);
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 60) {
                    SetState(0x62);
                } else {
                    SetState(0x63);
                }
                Character_SetFace(this, 8);
            }
            if (StepWalkLooking(0x1800, 0, 0x199, 0x22, 6)) {
                FloorPos pos;
                if (Random(100) < 60) {
                    GetRandomCorner(&pos);
                } else {
                    GetRandomPosInArea(&pos);
                }
                SetTargetOnFloor(pos.x, pos.z);
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x64:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x20, 0, 0xFF)) {
                mStep++;
                mTimer = 300;
                Character_SetFace(this, GetFace());
                mHeldItems[0]->mRotY = 0x7000;
                gSoundManager.StartSeqArc(mSoundHandle, 0x1C, 0);
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0 && IsAnimationFinished()) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x21);
                mHeldItems[0]->mRotZ = 0x1000;
            }
        } else {
            mTimer--;
            if (mTimer <= 0 || gSoundManager.CountPlayingSeqArcEntry(0x1C, 0) == 0) {
                SetState(0x62);
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0x65:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x66 || mPrevState == 0x67) {
                SetState(0x66);
            } else if (IsAnimOfGroupB(mModel.mAnimId)) {
                SetAnimation(RESOURCE_BCA0, 0x1A);
            }
            SetHeldItemsShowState(1);
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(0x67);
        }
        break;

    case 0x66:
    case 0x67:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                if (mState == 0x67) {
                    mTimer = 10;
                } else {
                    mTimer = Random(100) + 300;
                }
                StartSound3IfAnimOfGroup();
                if (mModel.mAnimId != 0x31) {
                    Character_ResetSoundPhase(this, 0);
                }
                SetAnimation(RESOURCE_BCA0, 0x31);
                Character_SetFace(this, 9);
                SetHeldItemsShowState(1);
                ShowHeadIcon();
            }
        } else {
            if (mModel.mBlendFrames <= 0) {
                mTimer--;
                if (mTimer <= 0) {
                    if (mState == 0x67) {
                        mTimer = 0;
                    } else {
                        mTimer = Random(60) + 120;
                        int faceRoll = Random(100);
                        int animRoll = Random(100);
                        if (animRoll < 60) {
                            SetAnimation(RESOURCE_BCA0, 0x31);
                            if (faceRoll < 28) {
                                Character_SetFace(this, 8);
                            } else {
                                Character_SetFace(this, 9);
                            }
                        } else if (animRoll < 92) {
                            SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                            if (faceRoll < 60) {
                                Character_SetFace(this, 8);
                            } else {
                                Character_SetFace(this, GetFace());
                            }
                        } else {
                            SetAnimation(RESOURCE_BCA0, 0x54);
                            if (faceRoll < 60) {
                                Character_SetFace(this, 6);
                            } else {
                                Character_SetFace(this, 4);
                            }
                        }
                        if (mModel.mBlendFrames > 0) {
                            Character_ResetSoundPhase(this, 0);
                        }
                    }
                }
            }
            SetHeadTurn(GetHeadPitch(CharacterModel_GetJointPosition(&mModel, 0), mCamera->mPosition, 0x1800), 0, 0, 2);
        }
        if (mModel.mAnimId == 0x31) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 3, 10);
        } else if (mModel.mAnimId == 0x54) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x20, 3, 2);
        }
        if (mState == 0x67) {
            Character_SetWalkTarget(this, mCamera->mPosition);
            if (Character_TurnToWalkTarget(this, 0x300, 0)) {
                SetState(0x66);
            }
        } else {
            Character_SetAngle(this, AngleTo(mCamera->GetFloorPos()));
        }
        break;
    }
    PlaceHeadIcon();
}

// The states 0x68 to 0x6f: one more activity of the same kind (hub 0x68), with held item 0 on the floor where
// the character stands, scaled by the model's factors.
void RoomCharacter::UpdateSoloStates68To6F() {
    switch (mState) {
    case 0x68:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mPrevState == 0x69 || mPrevState == 0x6B || mPrevState == 0x6F || mPrevState == 0x6A ||
                mPrevState == 0x6E) {
                SetState(0x69);
            } else {
                SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1A);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            SetState(0x6C);
        }
        break;

    case 0x69:
    case 0x6A:
    case 0x6B:
        if (mStep == 0) {
            if (StepChangeHeldItems(10, 0xFF) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSoundUnlessGroupB(1);
                SetAnimation(RESOURCE_BCA0, 0x1F);
                if (mState == 0x6B) {
                    mTimer = Random(500) + 100;
                    mTimer2 = Random(100) + 200;
                    Character_SetFace(this, 0x12);
                    mHeadIcon.SetMode(1);
                } else {
                    mTimer = Random(400) + 2500;
                    mTimer2 = Random(100) + 800;
                    Character_SetFace(this, 6);
                    ShowHeadIconOfRecord();
                    mHeadIcon.SetMode(3);
                }
                if (mState == 0x6A) {
                    mUnknown25D6 = Random(0x4000) - 0x2000;
                } else {
                    SetHeadTurn(0, 0, 0, 5);
                }
                mHeldItems[0]->SetRotation(0, 0x4000 - mAngle, 0);
                mHeldItems[0]->SetPosition(mModel.mPos.x, 0, mModel.mPos.z);
            }
            mHeldItems[0]->SetScale(func_0208fc34(&mModel));
        } else {
            if (mState != 0x6B && func_02081024(&mHeadIcon, mHeadIcon.mShown->mFrame) == 8) {
                PlayRandomSoundOfFour();
            }
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x69);
                break;
            }
            mTimer2--;
            if (mTimer2 > 0) {
                break;
            }
            mTimer2 = Random(300) + 1100;
            SetHeadTurn(0, Random(0x4000) - 0x2000, Random(0x1200) - 0x900, 5);
        }
        break;

    case 0x6C: {
        if (GetFloorPos().LengthSq() < 0x4000) {
            SetState(0x6D);
        } else {
            mTarget.x = 0;
            mTarget.z = 0;
            StepWalkTo(GetTargetFloorPos());
        }
        break;
    }

    case 0x6D:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x20, 10, 0xFF)) {
                mStep++;
                mHeldItems[0]->mRotY = 0x4000 - mAngle;
                mHeldItems[0]->SetScale(func_0208fc34(&mModel));
                gSoundManager.StartSeqArc(1, 0x1C, 6);
            }
        } else if (mModel.mBlendFrames <= 0) {
            if (IsAnimationFinished()) {
                SetState(0x6E);
            }
        }
        PlaceHeldItemBetweenHands(0);
        break;

    case 0x6E:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x3C);
                func_0208ef28(&mModel, 1, 0, 0);
                mHeldItems[0]->mRotY = 0x4000 - mAngle;
                MTX_Identity33(&mHeldItems[0]->mRotation);
                SetHeadTurn(0, 0, 0, 5);
                ShowHeadIcon();
                Character_ResetSoundPhase(this, 0);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1C, 7, 4);
            if (IsAnimationFinished()) {
                SetState(0x6F);
            }
        }
        PlaceHeldItemBetweenHands(0);
        break;

    case 0x6F:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x1A);
                ShowHeadIcon();
            }
        } else if (mModel.mBlendFrames <= 0) {
            SetState(0x6B);
        }
        break;
    }
    PlaceHeadIcon();
}

// ===================================================================================================================
// RoomCharacterPairStates70

// The states 0x70 to 0x92: three things the two characters of the room do together. Each group has the same shape:
// a hub state (0x70, 0x7f, 0x8b) in which slot 0 picks the next state for both, the activity states, and a way out
// (walk to the partner, 0x7b/0x86/0x8f, then a pair of closing states). mStep is the step inside a state.
//   0x70..0x7e: both play the same pair of animations (0x4e/0x4f, 0x50/0x51 or 0x52) in turns, with sounds of
//               sequence archive 0x24
//   0x7f..0x8a: animations 0x5b/0x5c with sounds of the same archive
//   0x8b..0x92: with held item 8 (StepChangeHeldItems(8, 0xff)): animations 0x3f..0x42, sound 4 of archive 0x25, and
//               in 0x8e walking about to random places
void RoomCharacter::UpdatePairStates70To92() {
    int state = mState;
    switch (state) {
    case 0x70:
        if (mStep == 0 && mModel.mBlendFrames <= 0) {
            mStep++;
            if (mPrevState > 0x70 && mPrevState <= 0x7e && mPrevState != 0x7b) {
                if (mSlot == 0) {
                    int diff = (s16)mAngle - (s16)mPartner->mAngle;
                    if (diff < 0) {
                        diff = -diff;
                    }
                    int r = Random(100);
                    u16 other;
                    if (r < 25) {
                        if (diff < 0x4000) {
                            mCounter = 0x71;
                        } else {
                            mCounter = 0x72;
                        }
                        other = mCounter;
                    } else if (r < 50) {
                        if (diff < 0x4000) {
                            mCounter = 0x73;
                        } else {
                            mCounter = 0x74;
                        }
                        other = mCounter;
                    } else if (r < 70) {
                        mCounter = 0x75;
                        other = mCounter;
                    } else if (r < 85) {
                        mCounter = 0x7c;
                        other = 0x7d;
                    } else {
                        other = 0x7e;
                        mCounter = other;
                    }
                    SetStateWithPartner(mCounter, other);
                }
            } else {
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
                SetState(0x7b);
            }
            SetHeadTurn(0, 0, 0, 5);
            SetHeldItemsShowState(1);
        }
        break;
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
        if (mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0 && mSlot != 0) {
            if (state == 0x71) {
                SetStateWithPartner(0x76, 0x76);
            } else if (state == 0x72) {
                SetStateWithPartner(0x77, 0x77);
            } else if (state == 0x73) {
                SetStateWithPartner(0x78, 0x78);
            } else if (state == 0x74) {
                SetStateWithPartner(0x79, 0x79);
            } else {
                SetStateWithPartner(0x7a, 0x7a);
            }
        }
        break;
    case 0x76:
    case 0x77:
    case 0x78:
    case 0x79:
    case 0x7a:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mCounter = 0;
                StartSound3IfAnimOfGroup();
                int prevAnim = mModel.mAnimId;
                mTimer = Random(80) + 0xfa;
                if (mState == 0x7a) {
                    SetAnimation(RESOURCE_BCA0, 0x52);
                } else {
                    if (mState == 0x78 || mState == 0x79) {
                        mTimer2 = 0x50;
                        mTimer3 = 0x51;
                    } else {
                        mTimer2 = 0x4e;
                        mTimer3 = 0x4f;
                    }
                    if (mSlot == 0) {
                        if (Random(0) & 1) {
                            SetAnimation(RESOURCE_BCA0, mTimer2);
                        } else {
                            SetAnimation(RESOURCE_BCA0, mTimer3);
                        }
                    } else if (mState == 0x76 || mState == 0x78) {
                        SetAnimation(RESOURCE_BCA0, mPartner->mModel.mAnimId);
                    } else if (mTimer2 == mPartner->mModel.mAnimId) {
                        SetAnimation(RESOURCE_BCA0, mTimer3);
                    } else {
                        SetAnimation(RESOURCE_BCA0, mTimer2);
                    }
                }
                if (Random(0) & 1) {
                    Character_SetFace(this, 8);
                } else {
                    Character_SetFace(this, 10);
                }
                if (prevAnim != mModel.mAnimId) {
                    func_0208ef28(&mModel, 1, 0, 0);
                    Character_ResetSoundPhase(this, 0);
                } else {
                    func_0208ef00(&mModel, 1, 0);
                }
                u16 look;
                if (Random(100) < 25) {
                    look = GetHeadYawTo(mPartner->GetFloorPos(), 0x3000);
                } else {
                    look = 0;
                }
                SetHeadTurn(0, look, 0, 4);
                SetHeldItemsShowState(1);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            mTimer--;
            if (mState == 0x7a) {
                ModelAnimation *anim = GetCurrentAnimation();
                if (anim != NULL) {
                    if (anim->mAnmObj->frame < (NNS_G3dAnmObjGetNumFrame(anim->mAnmObj) >> 1)) {
                        Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 3, 2);
                    } else {
                        Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 4, 8);
                    }
                }
            } else if (mState == 0x78 || mState == 0x79) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 5, 2);
            } else {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 0, 2);
            }
            if (GetCurrentAnimation()->mFinished) {
                mCounter++;
                if (mCounter >= 2) {
                    mCounter = 0;
                    if (mTimer > 0) {
                        if (mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                            Character_ResetSoundPhase(this, 0);
                            if (mState == 0x7a) {
                                SetAnimation(RESOURCE_BCA0, 0x52);
                                mPartner->SetAnimation(RESOURCE_BCA0, 0x52);
                            } else if (mTimer2 == mModel.mAnimId) {
                                SetAnimation(RESOURCE_BCA0, mTimer3);
                                if (mState == 0x76 || mState == 0x78) {
                                    mPartner->SetAnimation(RESOURCE_BCA0, mTimer3);
                                } else {
                                    mPartner->SetAnimation(RESOURCE_BCA0, mTimer2);
                                }
                            } else {
                                SetAnimation(RESOURCE_BCA0, mTimer2);
                                if (mState == 0x76 || mState == 0x78) {
                                    mPartner->SetAnimation(RESOURCE_BCA0, mTimer2);
                                } else {
                                    mPartner->SetAnimation(RESOURCE_BCA0, mTimer3);
                                }
                            }
                        } else {
                            mCounter = 2;
                        }
                    } else {
                        SetStateWithPartner(0x70, 0x70);
                        Character_SetFace(this, 8);
                    }
                } else {
                    Character_ResetSoundPhase(this, 0);
                }
                func_0208ef28(&mModel, 1, 0, 0);
            }
        }
        break;
    case 0x7b:
        StepWalkTo(mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0x7c, 0x7d);
        }
        break;
    case 0x7c:
        if (mStep == 0) {
            if (StepIdleThenAnimation55()) {
                mStep++;
                mTimer = Random(60) + 0x46;
            }
        } else {
            StepIdleThenAnimation55();
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0x70, 0x70);
            }
        }
        break;
    case 0x7d:
        StepRandomAnimationFacingPartner();
        break;
    case 0x7e:
        if (mStep == 0) {
            mStep++;
            mTimer = Random(60) + 60;
        } else {
            StepAnimationFacingPartner(GetIdleAnim());
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0x70, 0x70);
            }
        }
        break;
    case 0x7f:
        if (mStep == 0 && mModel.mBlendFrames <= 0) {
            mStep++;
            if (mPrevState > 0x7f && mPrevState <= 0x8a && mPrevState != 0x86) {
                if (mSlot == 0) {
                    int next = Random(2) + 0x80;
                    if (mPrevState == 0x82 || mPrevState == 0x84) {
                        if (next != 0x80) {
                            next = 0x87;
                        }
                    } else if (mPrevState == 0x83 || mPrevState == 0x85) {
                        if (next != 0x81) {
                            next = 0x87;
                        }
                    } else if (mPrevState == 0x80 || mPrevState == 0x81 || mPrevState == 0x87) {
                    } else if ((u32)(mPrevState - 0x88) <= 2) {
                        next = 0x88;
                    } else {
                        next = 0x86;
                    }
                    SetStateWithPartner(next, next);
                }
            } else {
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
                SetState(0x86);
                SetHeldItemsShowState(1);
            }
        }
        break;
    case 0x80:
    case 0x81:
        if (mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0 && mSlot != 0) {
            if (state == 0x80) {
                SetStateWithPartner(0x82, 0x82);
            } else if (state == 0x81) {
                SetStateWithPartner(0x83, 0x83);
            } else {
                SetStateWithPartner(0x7f, 0x7f);
            }
        }
        break;
    case 0x82:
    case 0x83:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound0UnlessGroup();
                if (mModel.mAnimId >= 0) {
                    if (mState == 0x82) {
                        if (mModel.mAnimId != 0x5b) {
                            SetAnimation(RESOURCE_BCA0, 0x1a);
                        }
                    } else if (mModel.mAnimId != 0x5c) {
                        SetAnimation(RESOURCE_BCA0, 0x1a);
                    }
                }
                int r = Random(3);
                if (r < 1) {
                    Character_SetFace(this, 8);
                } else if (r < 2) {
                    Character_SetFace(this, 0xc);
                } else {
                    Character_SetFace(this, 0xb);
                }
                SetHeldItemsShowState(1);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = Random(80) + 400;
                if (mState == 0x82) {
                    SetAnimation(RESOURCE_BCA0, 0x5b);
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x5c);
                }
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                func_0208ef00(&mModel, 1, 0);
                u16 look;
                if (Random(100) < 15) {
                    look = GetHeadYawTo(mPartner->GetFloorPos(), 0x3000);
                } else {
                    look = 0;
                }
                SetHeadTurn(0, look, 0, 4);
            }
        } else {
            mTimer--;
            if (mState == 0x82) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 1, 4);
            } else {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 2, 2);
            }
            if (GetCurrentAnimation()->mFinished) {
                if (mTimer <= 0) {
                    if (Random(100) < 50) {
                        SetStateWithPartner(0x7f, 0x7f);
                    } else if (mState == 0x82) {
                        SetStateWithPartner(0x84, 0x84);
                    } else {
                        SetStateWithPartner(0x85, 0x85);
                    }
                }
                func_0208ef28(&mModel, 1, 0, 0);
                Character_ResetSoundPhase(this, 0);
            }
        }
        break;
    case 0x84:
    case 0x85:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = Random(60) + 0xfa;
                u16 look = GetHeadYawTo(mPartner->GetFloorPos(), 0x3000);
                if (mSlot == 0) {
                    func_0208ef00(&mModel, 1, 0);
                    Character_SetFace(this, 0x15);
                } else {
                    if (Random(10) < 4) {
                        func_0208ef00(&mModel, 1, 0);
                    } else {
                        func_0208ef00(&mModel, 0, 0);
                    }
                    if (Random(100) < 65) {
                        look = 0;
                    }
                }
                SetHeadTurn(0, look, 0, 4);
            }
        } else {
            SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), mHeadYaw, 0, 4);
            if (mState == 0x84) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 1, 4);
            } else {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x24, 2, 2);
            }
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0x7f, 0x7f);
            }
        }
        break;
    case 0x86:
        StepWalkTo(mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0x89, 0x8a);
        }
        break;
    case 0x87:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
                if (Random(100) < 25) {
                    Character_SetFace(this, 0x15);
                } else {
                    Character_SetFace(this, GetFace());
                }
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = Random(60) + 60;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 7);
                u16 look;
                if (Random(100) < 40) {
                    look = GetHeadYawTo(mPartner->GetFloorPos(), 0x3000);
                } else {
                    look = 0;
                }
                SetHeadTurn(0, look, 0, 4);
            }
        } else {
            if (mSlot == 0 && mPartner->mState == 0x87 && mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                mTimer--;
                if (mTimer <= 0) {
                    SetStateWithPartner(0x7f, 0x7f);
                    Character_SetFace(this, GetFace());
                }
            }
        }
        break;
    case 0x88:
        if (mStep == 0) {
            u16 angle;
            mStep++;
            GetAngleAroundPartner(&angle, 0);
            mTargetAngle = angle;
        } else if (StepTurnWithAnimation(GetWalkAnim())) {
            SetHeadTurn(0, 0, 0, 4);
            SetState(0x87);
        }
        break;
    case 0x89:
        if (mStep == 0) {
            if (StepIdleThenAnimation55()) {
                mStep++;
                mTimer = Random(60) + 0x46;
            }
        } else {
            StepIdleThenAnimation55();
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0x88, 0x88);
            }
        }
        break;
    case 0x8a:
        StepRandomAnimationFacingPartner();
        break;
    case 0x8b:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                if ((u32)(mPrevState - 0x8c) <= 2) {
                    int r = Random(100);
                    if (r < 65) {
                        SetState(0x8c);
                    } else if (r < 85) {
                        SetState(0x8d);
                    } else {
                        SetState(0x8e);
                    }
                } else {
                    SetHeldItemsShowState(1);
                    if (IsAnimOfGroupB(mModel.mAnimId)) {
                        SetAnimation(RESOURCE_BCA0, 0x1a);
                    }
                }
            }
        } else if (mModel.mBlendFrames <= 0) {
            SetState(0x8f);
        }
        break;
    case 0x8c:
    case 0x8d:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(8, 0xff) && mModel.mBlendFrames <= 0) {
                mStep++;
                if (mState == 0x8c) {
                    StartSound0UnlessGroupA();
                    SetAnimation(RESOURCE_BCA0, 0x3f);
                } else {
                    StartSound3IfAnimOfGroup();
                    SetAnimation(RESOURCE_BCA0, 0x41);
                }
                Character_SetFace(this, 8);
                mTimer = Random(100) + 500;
                mTimer2 = Random(60) + 0x96;
                gSoundManager.StartSeqArcLimited(mSoundHandle, 0x25, 4, 2);
            }
        } else {
            mTimer--;
            if (mModel.mBlendFrames <= 0) {
                if (mTimer <= 0) {
                    int r = Random(100);
                    if (r < 65) {
                        SetState(0x8c);
                    } else if (r < 85) {
                        SetState(0x8d);
                    } else {
                        SetState(0x8e);
                    }
                } else if (GetCurrentAnimation()->mFinished || --mTimer2 <= 0) {
                    u16 look;
                    if (Random(10) < 3) {
                        look = GetHeadYawTo(mPartner->GetFloorPos(), 0x2000);
                    } else {
                        look = 0;
                    }
                    SetHeadTurn(0, look, 0, 4);
                    mTimer2 = Random(60) + 0x104;
                    if (mState == 0x8c) {
                        if (Random(100) < 70) {
                            SetAnimation(RESOURCE_BCA0, 0x3f);
                            Character_SetFace(this, GetFaceOfMood(Random(5)));
                        } else {
                            SetAnimation(RESOURCE_BCA0, 0x40);
                            func_0208ef00(&mModel, 1, 0);
                            Character_SetFace(this, 8);
                        }
                    }
                    if (Random(100) < 10) {
                        Character_SetFace(this, 0x15);
                    }
                }
            }
        }
        PlaceHeldItemBetweenHands(0xcc);
        break;
    case 0x8e:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(8, 0xff) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0x42);
                Character_SetFace(this, 8);
                if (mModel.mBlendFrames <= 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                FloorPos p;
                GetRandomPosInArea(&p);
                SetTarget(p.x, 0, p.z);
                mHeadTurnFrames = 5;
                mTimer = Random(100) + 500;
                gSoundManager.StartSeqArcLimited(mSoundHandle, 0x25, 4, 2);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                int r = Random(100);
                if (r < 65) {
                    SetState(0x8c);
                } else if (r < 85) {
                    SetState(0x8d);
                } else {
                    SetState(0x8e);
                }
            } else {
                if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
                    FloorPos here = GetFloorPos();
                    if (ClampToWalkArea(&here)) {
                        SetState(0x8d);
                    } else {
                        func_02091598(this, mPartner->GetFloorPos());
                    }
                }
                if (StepWalkLooking(0x1000, 0, 0x199, 0x22, 6)) {
                    FloorPos p;
                    GetRandomPosInArea(&p);
                    SetTarget(p.x, 0, p.z);
                    Character_SetFace(this, GetFaceOfMood(Random(5)));
                }
            }
        }
        PlaceHeldItemBetweenHands(0xcc);
        break;
    case 0x8f:
        StepWalkTo(mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0x90, 0x91);
        }
        break;
    case 0x90:
        if (mStep == 0) {
            if (StepIdleThenAnimation55()) {
                mStep++;
                mTimer = Random(60) + 0x46;
            }
        } else {
            StepIdleThenAnimation55();
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0x92, 0x92);
            }
        }
        break;
    case 0x91:
        StepRandomAnimationFacingPartner();
        break;
    case 0x92:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x3b, 8, 0xff)) {
                mStep++;
            }
        } else if (mModel.mBlendFrames <= 0 && GetCurrentAnimation()->mFinished) {
            SetState(0x8d);
        }
        PlaceHeldItemBetweenHands(0);
        break;
    }
    PlaceHeadIcon();
}

// The states 0x93 to 0xb2: three more of the same shape, with the hubs 0x93, 0x9f and 0xa8.
//   0x93..0x9e: with held item 1, which is kept between the two hands (animations 0x3d, 0x28, 0x3c)
//   0x9f..0xa7: with held item 9 (animations 0x3f, 0x40) while the partner watches (0xa1, 0xa2)
//   0xa8..0xb2: with held items 9 and 5 (animations 0x53, 0x46), sound 3 of archive 0x1c
void RoomCharacter::UpdatePairStates93ToB2() {
    int state = mState;
    switch (state) {
    case 0x93:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                int prev = mPrevState;
                if (prev == 0x94 || prev == 0x96) {
                    SetState(0x94);
                } else if ((u32)(prev - 0x97) <= 1) {
                    SetState(0x97);
                } else if (prev == 0x9e || prev == 0x95) {
                    SetState(0x95);
                } else if (prev == 0x9a || prev == 0x9b || prev == 0x9d) {
                    SetState(0x9c);
                } else {
                    SetHeldItemsShowState(1);
                    if (IsAnimOfGroupB(mModel.mAnimId)) {
                        SetAnimation(RESOURCE_BCA0, 0x1a);
                    }
                }
            }
        } else if (mModel.mBlendFrames <= 0) {
            SetState(0x99);
        }
        break;
    case 0x94:
    case 0x95:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(1, 0xff) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSoundUnlessGroupB(0);
                SetAnimation(RESOURCE_BCA0, 0x3d);
                Character_SetFace(this, 8);
                mHeldItems[0]->mRotY = 0xc000 - mAngle;
                if (mState == 0x95) {
                    VecFx32 mid;
                    GetMidpoint(GetNode(2), GetNode(1), &mid);
                    mid.y = 0xcc;
                    mHeldItems[0]->SetPosition(mid);
                } else {
                    gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x1c, 2);
                }
            }
        } else if (mStep == 2) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                if (mState == 0x94) {
                    if (Random(100) < 15) {
                        u16 look = GetHeadYawTo(mPartner->GetFloorPos(), 0x3800);
                        SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), look, 0, 4);
                    }
                    if (Random(100) < 10) {
                        Character_SetFace(this, 0x15);
                    }
                }
            }
            if (mState == 0x95) {
                VecFx32 mid;
                GetMidpoint(GetNode(2), GetNode(1), &mid);
                mid.y = 0xcc;
                mHeldItems[0]->SetPosition(mid);
                mTimer = 1;
            } else {
                mTimer = Random(100) + 300;
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 15 && mState != 0x95) {
                    SetState(0x96);
                } else {
                    SetState(0x94);
                }
            }
        }
        break;
    case 0x96:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0x28);
                func_0208ef28(&mModel, 1, 0, 0);
                Character_SetFace(this, 6);
                AnimModel_FinishAnimations(mHeldItems[0]);
                SetHeadTurn(0, 0, 0, 5);
                ShowHeadIcon();
                Character_ResetSoundPhase(this, 0);
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                if (Random(0) & 1) {
                    Character_SetFace(this, 7);
                }
            }
        } else if (mStep == 2) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1f, 5, 2);
            if (GetCurrentAnimation()->mFinished) {
                mStep++;
                mTimer = 0x14;
                Character_SetFace(this, 6);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0x94);
                Character_SetFace(this, 8);
                gSoundManager.StartSeqArc(1, 0x1f, 6);
            }
        }
        break;
    case 0x97:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound0UnlessGroupA();
                int r = Random(100);
                if (r < 60) {
                    SetAnimation(RESOURCE_BCA0, 0x19);
                } else if (r < 85) {
                    SetAnimation(RESOURCE_BCA0, 0x1b);
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x33);
                }
                Character_SetFace(this, GetFace());
                SetHeldItemsShowState(1);
                ShowHeadIcon();
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = Random(100) + 300;
                if (Random(100) < 70 || mModel.mAnimId == 0x33) {
                    HeldItem *item = mPartner->mHeldItems[0];
                    u16 look = GetHeadYawTo(item->GetFloorPos(), 0x3000);
                    SetHeadTurn(GetHeadPitch(CharacterModel_GetJointPosition(&mModel, 0), item->mPosition, 0x1800), look, 0, 4);
                } else {
                    SetHeadTurn(0, 0, 0, 5);
                }
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 70) {
                    SetState(0x97);
                } else {
                    SetState(0x98);
                }
            }
            if (mModel.mAnimId == 0x33) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1f, 8, 1);
            }
        }
        break;
    case 0x98:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StepAnimationThenFacePartner(0x1d);
                mTimer = Random(60) + 100;
            }
        } else if (StepAnimationThenFacePartner(0x1d) && mTimer <= 0) {
            SetState(0x97);
        } else {
            mTimer--;
        }
        break;
    case 0x99:
        StepWalkTo(mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0x9a, 0x9b);
        }
        break;
    case 0x9a:
        if (mStep == 0) {
            if (StepIdleThenAnimation55()) {
                mStep++;
                mTimer = Random(60) + 0x46;
            }
        } else {
            StepIdleThenAnimation55();
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0x9c, 0x9c);
            }
        }
        break;
    case 0x9b:
        StepRandomAnimationFacingPartner();
        break;
    case 0x9c:
        if (mStep == 0) {
            u16 angle;
            mStep++;
            GetAngleAroundPartner(&angle, 0x1000);
            mTargetAngle = angle;
        } else if (StepTurnWithAnimation(GetWalkAnim())) {
            SetHeadTurn(0, 0, 0, 4);
            if (mSlot == 0) {
                SetState(0x9d);
            } else {
                SetState(0x97);
            }
        }
        break;
    case 0x9d: {
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x20, 1, 0xff)) {
                mStep++;
                mHeldItems[0]->mRotY = 0xc000 - mAngle;
                AnimModel_FinishAnimations(mHeldItems[0]);
            }
        } else if (mModel.mBlendFrames <= 0 && GetCurrentAnimation()->mFinished) {
            SetState(0x9e);
        }
        VecFx32 mid;
        GetMidpoint(GetNode(2), GetNode(1), &mid);
        mid.y = GetNode(2)->mPos.y;
        mHeldItems[0]->SetPosition(mid);
        break;
    }
    case 0x9e: {
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x3c);
                func_0208ef28(&mModel, 1, 0, 0);
                Character_SetFace(this, 8);
                mHeldItems[0]->mRotY = 0xc000 - mAngle;
                MTX_Identity33(&mHeldItems[0]->mRotation);
                SetHeadTurn(0, 0, 0, 5);
                ShowHeadIcon();
            }
        } else if (GetCurrentAnimation()->mFinished) {
            SetState(0x95);
        }
        VecFx32 mid;
        GetMidpoint(GetNode(2), GetNode(1), &mid);
        mid.y = GetNode(2)->mPos.y;
        mHeldItems[0]->SetPosition(mid);
        break;
    }
    case 0x9f:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                int prev = mPrevState;
                if (prev == 0xa0) {
                    SetState(0xa0);
                } else if ((u32)(prev - 0xa1) <= 1) {
                    SetState(0xa1);
                } else if ((u32)(prev - 0xa4) <= 2) {
                    SetState(0xa4);
                } else {
                    SetHeldItemsShowState(1);
                    if (IsAnimOfGroupB(mModel.mAnimId)) {
                        SetAnimation(RESOURCE_BCA0, 0x1a);
                    }
                }
            }
        } else if (mModel.mBlendFrames <= 0) {
            SetState(0xa3);
        }
        break;
    case 0xa0:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(9, 0xff) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound0UnlessGroupA();
                SetAnimation(RESOURCE_BCA0, 0x3f);
                Character_SetFace(this, 8);
                mTimer = Random(100) + 500;
                mTimer2 = Random(60) + 0x96;
            }
        } else {
            mTimer--;
            if (mModel.mBlendFrames <= 0) {
                if (mTimer <= 0) {
                    SetState(0xa0);
                } else if (GetCurrentAnimation()->mFinished || --mTimer2 <= 0) {
                    mTimer2 = Random(60) + 0xaa;
                    if (Random(100) < 70) {
                        SetAnimation(RESOURCE_BCA0, 0x3f);
                        Character_SetFace(this, GetFaceOfMood(Random(5)));
                    } else {
                        SetAnimation(RESOURCE_BCA0, 0x40);
                        func_0208ef00(&mModel, 1, 0);
                        Character_SetFace(this, 8);
                        Character_ResetSoundPhase(this, 0);
                    }
                    if (Random(100) < 8) {
                        SetHeadTurn(0, GetHeadYawTo(mPartner->GetFloorPos(), 0x2800), 0, 4);
                    }
                    if (Random(100) < 30) {
                        Character_SetFace(this, 0x15);
                    }
                }
            }
            if (mModel.mAnimId == 0x40) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1c, 4, 2);
            }
        }
        PlaceHeldItemBetweenHands(0xcc);
        break;
    case 0xa1:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound0UnlessGroupA();
                mTimer = Random(100) + 400;
                int r = Random(100);
                if (r < 65) {
                    SetAnimation(RESOURCE_BCA0, 0x19);
                    Character_SetFace(this, GetFace());
                } else if (r < 85) {
                    SetAnimation(RESOURCE_BCA0, 0x1b);
                    Character_SetFace(this, GetFace());
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x17);
                    Character_SetFace(this, 6);
                    if (mModel.mBlendFrames > 0) {
                        Character_ResetSoundPhase(this, 0);
                    }
                }
                SetHeldItemsShowState(1);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                if (Random(100) < 70) {
                    if (mModel.mAnimId != 0x17) {
                        HeldItem *item = mPartner->mHeldItems[0];
                        u16 look = GetHeadYawTo(item->GetFloorPos(), 0x3000);
                        SetHeadTurn(GetHeadPitch(CharacterModel_GetJointPosition(&mModel, 0), item->mPosition, 0x1800), look, 0, 4);
                    }
                } else {
                    SetHeadTurn(0, 0, 0, 5);
                }
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 20 && mModel.mAnimId != 0x17) {
                    SetState(0xa2);
                } else {
                    SetState(0xa1);
                }
            }
        }
        if (mModel.mAnimId == 0x17) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x22, 9, 4);
        }
        break;
    case 0xa2:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = Random(60) + 100;
            }
        } else if (StepAnimationThenFacePartner(0x1d) && mTimer <= 0) {
            SetState(0xa1);
        } else {
            mTimer--;
        }
        break;
    case 0xa3:
        StepWalkTo(mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0xa5, 0xa6);
        }
        break;
    case 0xa4:
        if (mStep == 0) {
            u16 angle;
            mStep++;
            GetAngleAroundPartner(&angle, 0x1000);
            mTargetAngle = angle;
        } else if (StepTurnWithAnimation(GetWalkAnim())) {
            SetHeadTurn(0, 0, 0, 4);
            if (mSlot == 0) {
                SetState(0xa7);
            } else {
                SetState(0xa1);
            }
        }
        break;
    case 0xa5:
        if (mStep == 0) {
            if (StepIdleThenAnimation55()) {
                mStep++;
                mTimer = Random(60) + 0x46;
            }
        } else {
            StepIdleThenAnimation55();
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0xa4, 0xa4);
            }
        }
        break;
    case 0xa6:
        StepRandomAnimationFacingPartner();
        break;
    case 0xa7:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x3b, 9, 0xff)) {
                mStep++;
            }
        } else if (mModel.mBlendFrames <= 0 && GetCurrentAnimation()->mFinished) {
            SetState(0xa0);
        }
        PlaceHeldItemBetweenHands(0);
        break;
    case 0xa8:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                int prev = mPrevState;
                if ((u32)(prev - 0xa9) <= 1) {
                    SetState(0xa9);
                } else if ((u32)(prev - 0xab) <= 2) {
                    SetState(0xab);
                } else if ((u32)(prev - 0xaf) <= 2) {
                    SetState(0xaf);
                } else {
                    SetHeldItemsShowState(1);
                    if (IsAnimOfGroupB(mModel.mAnimId)) {
                        SetAnimation(RESOURCE_BCA0, 0x1a);
                    }
                }
            }
        } else if (mModel.mBlendFrames <= 0) {
            SetState(0xae);
        }
        break;
    case 0xa9:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 4);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(9, 5) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound0UnlessGroupA();
                SetAnimation(RESOURCE_BCA0, 0x53);
                Character_SetFace(this, 8);
                mHeldItems[1]->mRotZ = 0x8000;
                mTimer = Random(100) + 500;
                mTimer2 = Random(60) + 0x8c;
                gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x1c, 3);
                UpdateVoiceOfSound();
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 80 || ClampToRect4E4(NULL)) {
                    SetState(0xa9);
                } else {
                    SetState(0xaa);
                }
            } else if (mModel.mBlendFrames <= 0) {
                mTimer2--;
                if (mTimer2 <= 0) {
                    mTimer2 = Random(60) + 0x8c;
                    if (GetCurrentAnimation()->mFinished) {
                        func_0208ef00(&mModel, 0, 0);
                        SetHeadTurn(0, 0, 0, 4);
                        gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x1c, 3);
                        UpdateVoiceOfSound();
                    } else if (Random(100) < 30) {
                        u16 lookA;
                        u16 lookB;
                        if (Random(100) < 60) {
                            lookA = -((Random(9) << 9) + 0x600);
                            lookB = 0;
                        } else {
                            lookA = 0xf700;
                            lookB = GetHeadYawTo(mPartner->GetFloorPos(), 0x2000);
                        }
                        SetHeadTurn(lookA, lookB, 0, 4);
                        func_0208ef00(&mModel, 1, 1);
                        gSoundManager.StopSeqArc(mSoundHandle, 0);
                    }
                }
            }
        }
        PlaceHeldItemsInHands();
        break;
    case 0xaa:
        if (mStep == 0) {
            mStep++;
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(9, 5) && mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x46);
                Character_SetFace(this, 6);
                mHeldItems[1]->mRotZ = 0x8000;
                mTimer = Random(60) + 0x14a;
                if (Random(0) & 1) {
                    Character_SetFace(this, 7);
                }
                gSoundManager.StartSeqArc(1, 0x1e, 1);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0xa9);
            } else if (mTimer == 0x96) {
                Character_SetFace(this, 8);
            } else if (mTimer == 0x10e) {
                Character_SetFace(this, 6);
            }
        }
        PlaceHeldItemsInHands();
        break;
    case 0xab:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound0UnlessGroupA();
                mTimer = Random(100) + 400;
                int r = Random(100);
                if (r < 65) {
                    SetAnimation(RESOURCE_BCA0, 0x19);
                    Character_SetFace(this, GetFace());
                } else if (r < 85) {
                    SetAnimation(RESOURCE_BCA0, 0x1b);
                    Character_SetFace(this, GetFace());
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x17);
                    Character_SetFace(this, 6);
                    if (mModel.mBlendFrames > 0) {
                        Character_ResetSoundPhase(this, 0);
                    }
                }
                SetHeldItemsShowState(1);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                if (Random(100) < 70) {
                    if (mModel.mAnimId != 0x17) {
                        u16 look = GetHeadYawTo(mPartner->mHeldItems[0]->GetFloorPos(), 0x3000);
                        SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), look, 0, 4);
                    }
                } else {
                    SetHeadTurn(0, 0, 0, 5);
                }
            }
        } else {
            if (mPartner->mState == 0xaa) {
                mTimer--;
            }
            mTimer--;
            if (mTimer <= 0) {
                if (mModel.mAnimId == 0x17) {
                    SetState(0xab);
                } else {
                    int r = Random(100);
                    if (mPartner->mState == 0xaa) {
                        r -= 60;
                    }
                    if (r < 20) {
                        SetState(0xac);
                    } else if (r < 85) {
                        SetState(0xab);
                    } else {
                        SetState(0xad);
                    }
                }
            }
        }
        if (mModel.mAnimId == 0x17) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x22, 9, 4);
        }
        break;
    case 0xac:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = Random(60) + 0x78;
            }
        } else if (StepAnimationThenFacePartner(0x1d) && mTimer <= 0) {
            SetState(0xab);
        } else {
            mTimer--;
        }
        break;
    case 0xad:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = Random(60) + 0xb4;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0);
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), mUnknown25D6, 0, 4);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (Random(10) < 5) {
                    SetState(0xab);
                } else {
                    SetState(0xad);
                }
            }
        }
        break;
    case 0xae:
        StepWalkTo(mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0xb0, 0xb1);
        }
        break;
    case 0xaf:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                RestartStepSound();
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                SetTargetFloorPos(mPartner->GetFloorPos());
                ShowHeadIcon();
                SetHeldItemsShowState(1);
            }
        } else if (StepTurnLooking(0x100, 0, 0x3800)) {
            SetHeadTurn(0, 0, 0, 4);
            if (mSlot == 0) {
                SetState(0xb2);
            } else {
                SetState(0xab);
            }
        }
        break;
    case 0xb0:
        if (mStep == 0) {
            if (StepIdleThenAnimation55()) {
                mStep++;
                mTimer = Random(60) + 0x46;
            }
        } else {
            StepIdleThenAnimation55();
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0xaf, 0xaf);
            }
        }
        break;
    case 0xb1:
        StepRandomAnimationFacingPartner();
        break;
    case 0xb2:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x3b, 9, 5)) {
                mStep++;
                AnimModel_FinishAnimations(mHeldItems[1]);
                mHeldItems[1]->mRotZ = 0x8000;
            }
        } else if (mModel.mBlendFrames <= 0 && GetCurrentAnimation()->mFinished) {
            SetState(0xa9);
        }
        PlaceHeldItemsInHands();
        break;
    }
    PlaceHeadIcon();
}

// ===================================================================================================================
// RoomCharacterStatesB3

// States 0xb3 to 0xc9, two characters and the thing in the first hand (mHeldItems[0], which follows joint 2).
// 0xb3..0xbe: both take it out (0xb4/0xb5), use it together for a random time with sequence 0x1c playing (0xb6/0xb7),
// or one walks up to the other first (0xb8..0xbb). 0xbf..0xc9: one uses it (0xc0 standing, 0xc1 walking about)
// while the other watches (0xc2); 0xc4..0xc9 bring the two to the middle of the room first.
// Only the character with mSlot == 0 chooses the next states, for both.
void RoomCharacter::UpdateStatesB3ToC9() {
    // Declared here and not in their case: that is where the original has them on its stack
    FloorPos own;
    FloorPos other;

    switch (mState) {
    case 0xb3:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mSlot != 0) {
                break;
            }
            if (mPrevState > 0xb3 && mPrevState <= 0xb7) {
                if (Random(100) < 70) {
                    SetStateWithPartner(0xb4, 0xb4);
                } else {
                    SetStateWithPartner(0xb5, 0xb5);
                }
            } else if (mPrevState == 0xb9 || mPrevState == 0xba || mPrevState == 0xbb) {
                SetStateWithPartner(0xb9, 0xb9);
            } else {
                SetHeldItemsShowState(1);
                mPartner->SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
                if (mPartner->IsAnimOfGroupB(mPartner->mModel.mAnimId)) {
                    mPartner->SetAnimation(RESOURCE_BCA0, 0x1a);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            if (mSlot != 0) {
                break;
            }
            SetStateWithPartner(0xb8, 0xb8);
        }
        break;

    case 0xb4:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            StepChangeHeldItems(7, 0xff);
        } else {
            if (StepChangeHeldItems(7, 0xff) && mModel.mBlendFrames <= 0) {
                SetState(0xb6);
                Character_SetFace(this, 8);
                mHeldItems[0]->mRotZ = 0x4000;
            }
        }
        PlaceHeldItemAtHead();
        break;

    case 0xb6:
        if (mStep == 0) {
            if (mSlot == 0) {
                if (mPartner->mState == 0xb6 && mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                    mStep++;
                    StartSound3IfAnimOfGroup();
                    mTimer = Random(200) + 550;
                    int r = Random(100);
                    if (r < 55) {
                        SetAnimation(RESOURCE_BCA0, 9);
                    } else if (r < 70) {
                        SetAnimation(RESOURCE_BCA0, 2);
                    } else if (r < 83) {
                        mTimer >>= 2;
                        SetAnimation(RESOURCE_BCA0, 7);
                    } else if (r < 92) {
                        mTimer >>= 1;
                        SetAnimation(RESOURCE_BCA0, 0x58);
                    } else if (r < 96) {
                        mTimer >>= 2;
                        SetAnimation(RESOURCE_BCA0, 0x4b);
                    } else {
                        mTimer >>= 3;
                        SetAnimation(RESOURCE_BCA0, 0x2a);
                        if (mModel.mBlendFrames > 0) {
                            Character_ResetSoundPhase(this, 0);
                            Character_ResetSoundPhase(mPartner, 0);
                        }
                    }
                    mPartner->SetAnimation(RESOURCE_BCA0, mModel.mAnimId);
                    if (Random(100) < 50) {
                        Character_SetFace(this, 8);
                    } else {
                        Character_SetFace(this, 10);
                    }
                    Character_SetFace(mPartner, mFace);
                    u16 yaw;
                    if (Random(100) < 25) {
                        yaw = GetHeadYawTo(mPartner->GetFloorPos(), 0x2000);
                    } else {
                        yaw = 0;
                    }
                    SetHeadTurn(0, yaw, 0, 4);
                    gSoundManager.StartSeqArcLimited(mSoundHandle, 0x1c, 5, 1);
                }
            } else {
                mStep++;
                u16 yaw;
                if (Random(100) < 25) {
                    yaw = GetHeadYawTo(mPartner->GetFloorPos(), 0x2000);
                } else {
                    yaw = 0;
                }
                SetHeadTurn(0, yaw, 0, 4);
            }
        } else {
            if (mSlot == 0) {
                mTimer--;
                if (mTimer <= 0 && mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                    mKeepSound = TRUE;
                    if (Random(100) < 90) {
                        SetStateWithPartner(0xb6, 0xb6);
                    } else {
                        SetStateWithPartner(0xb7, 0xb7);
                    }
                }
            }
            if (mModel.mAnimId == 0x2a) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1f, 0, 2);
            }
        }
        PlaceHeldItemAtHead();
        break;

    case 0xb5:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            StepChangeHeldItems(7, 0xff);
        } else {
            if (StepChangeHeldItems(7, 0xff) && mModel.mBlendFrames <= 0) {
                SetState(0xb7);
                Character_SetFace(this, 8);
                mHeldItems[0]->mRotZ = 0x4000;
            }
        }
        PlaceHeldItemAtHead();
        break;

    case 0xb7:
        if (mStep == 0) {
            if (mSlot == 0) {
                if (mPartner->mState == 0xb7 && mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                    mStep++;
                    StartSound0UnlessGroupA();
                    mTimer = Random(200) + 450;
                    int r = Random(100);
                    if (r < 50) {
                        SetAnimation(RESOURCE_BCA0, 0x19);
                    } else if (r < 70) {
                        SetAnimation(RESOURCE_BCA0, 0x1b);
                    } else if (r < 85) {
                        mTimer >>= 1;
                        SetAnimation(RESOURCE_BCA0, 0x1c);
                    } else {
                        mTimer >>= 1;
                        SetAnimation(RESOURCE_BCA0, 0x33);
                    }
                    mPartner->SetAnimation(RESOURCE_BCA0, mModel.mAnimId);
                    if (mModel.mBlendFrames > 0) {
                        Character_ResetSoundPhase(this, 0);
                        Character_ResetSoundPhase(mPartner, 0);
                    }
                    if (Random(100) < 50) {
                        Character_SetFace(this, 8);
                    } else {
                        Character_SetFace(this, 10);
                    }
                    Character_SetFace(mPartner, mFace);
                    u16 yaw;
                    if (Random(100) < 25) {
                        yaw = GetHeadYawTo(mPartner->GetFloorPos(), 0x2000);
                    } else {
                        yaw = 0;
                    }
                    SetHeadTurn(0, yaw, 0, 4);
                    gSoundManager.StartSeqArcLimited(mSoundHandle, 0x1c, 5, 1);
                }
            } else {
                mStep++;
                u16 yaw;
                if (Random(100) < 25) {
                    yaw = GetHeadYawTo(mPartner->GetFloorPos(), 0x2000);
                } else {
                    yaw = 0;
                }
                SetHeadTurn(0, yaw, 0, 4);
            }
        } else {
            if (mSlot == 0) {
                mTimer--;
                if (mTimer <= 0 && mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                    mKeepSound = TRUE;
                    if (Random(100) < 80) {
                        SetStateWithPartner(0xb7, 0xb7);
                    } else {
                        SetStateWithPartner(0xb6, 0xb6);
                    }
                }
            }
            if (mModel.mBlendFrames <= 0 && mModel.mAnimId == 0x33) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1f, 8, 1);
            }
        }
        PlaceHeldItemAtHead();
        break;

    case 0xb8:
        StepWalkTo(mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0xba, 0xbb);
        }
        break;

    case 0xb9:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            RestartStepSound();
            SetAnimation(RESOURCE_BCA0, GetWalkAnim());
            SetTargetFloorPos(mPartner->GetFloorPos());
            ShowHeadIcon();
            SetHeldItemsShowState(1);
        } else {
            if (StepTurnLooking(0x100, 0, 0x3800)) {
                SetHeadTurn(0, 0, 0, 4);
                SetState(0xbe);
            }
        }
        break;

    case 0xba:
        if (mStep == 0) {
            if (StepIdleThenAnimation55()) {
                mStep++;
                mTimer = Random(60) + 70;
            }
        } else {
            StepIdleThenAnimation55();
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0xb9, 0xb9);
            }
        }
        break;

    case 0xbb:
        StepRandomAnimationFacingPartner();
        break;

    case 0xbc:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 7);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 5);
            }
        } else {
            if (mModel.mBlendFrames <= 0) {
                SetState(0xbd);
            }
        }
        PlaceHeldItemAtHead();
        break;

    case 0xbd:
        if (mSlot == 0 && mPartner->mState == 0xbd) {
            SetStateWithPartner(0xb4, 0xb4);
        }
        PlaceHeldItemAtHead();
        break;

    case 0xbe:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x20, 7, 0xff)) {
                mStep++;
                mHeldItems[0]->mRotZ = 0x4000;
            }
        } else {
            if (mModel.mBlendFrames <= 0) {
                if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                    SetState(0xbc);
                }
            }
        }
        mHeldItems[0]->mRotX = mUnknown25D6;
        mHeldItems[0]->SetJointOffset(mModel.GetJointMatrix(2), mModel.GetJointRotation(0), 0x666, 0, 0);
        break;

    case 0xbf:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mSlot != 0) {
                break;
            }
            if (mPrevState > 0xbf && mPrevState <= 0xc1) {
                if (Random(100) < 50) {
                    SetStateWithPartner(0xc0, 0xc2);
                } else {
                    SetStateWithPartner(0xc1, 0xc2);
                }
            } else {
                SetHeldItemsShowState(1);
                mPartner->SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
                if (mPartner->IsAnimOfGroupB(mPartner->mModel.mAnimId)) {
                    mPartner->SetAnimation(RESOURCE_BCA0, 0x1a);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            if (mSlot != 0) {
                break;
            }
            SetStateWithPartner(0xc4, 0xc4);
        }
        break;

    case 0xc0:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        } else if (mStep == 1) {
            if (StepChangeHeldItems(0, 0xff) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0x21);
                Character_SetFace(this, 8);
                HeldItem *item = mHeldItems[0];
                item->mRotX = 0;
                item->mRotY = 0x7000;
                item->mRotZ = 0x1000;
                mTimer = Random(200) + 700;
                mTimer2 = 60;
                mTimer3 = 100;
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
            }
        } else {
            if (mTimer2-- <= 0) {
                mTimer2 = 100;
                if (Random(100) < 90) {
                    if (Random(2) == 0) {
                        Character_SetFace(this, 8);
                    } else {
                        Character_SetFace(this, 0x14);
                    }
                } else {
                    if (Random(2) == 0) {
                        Character_SetFace(this, 0xe);
                    } else {
                        Character_SetFace(this, 0x16);
                    }
                }
            }
            if (mFace == 0x14 || mFace == 0x16) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1a, 3, 10);
            } else {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1a, 7, 2);
            }
            if (mTimer3-- <= 0) {
                mTimer3 = Random(100) + 200;
                u16 yaw;
                if (Random(100) < 30) {
                    yaw = GetHeadYawTo(mPartner->GetFloorPos(), 0x1800);
                } else {
                    yaw = 0;
                }
                SetHeadTurn(0, yaw, 0, 5);
            }
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 40) {
                    SetState(0xc0);
                } else {
                    SetState(0xc1);
                }
                Character_SetFace(this, 8);
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xc1:
        if (mStep == 0) {
            mStep++;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            RestartStepSound();
        } else if (mStep == 1) {
            if (StepChangeHeldItems(0, 0xff) && mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0x22);
                Character_SetFace(this, 0x15);
                HeldItem *item = mHeldItems[0];
                item->mRotX = 0;
                item->mRotY = 0x7800;
                item->mRotZ = 0x1000;
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 1);
                }
                FloorPos pos;
                GetRandomPosInArea(&pos);
                VEC_Set(&mTarget, pos.x, 0, pos.z);
                mTimer = Random(200) + 700;
                mTimer2 = 0;
                mCounter = 0;
            }
        } else {
            if (mTimer2-- <= 0) {
                mTimer2 = 100;
                if (Random(100) < 70) {
                    Character_SetFace(this, 0x15);
                } else {
                    Character_SetFace(this, 0x16);
                }
            }
            Character_PlaySoundAtAnimPhase(this, 1, 1, 0x1a, 3, 10);
            mTimer--;
            if (mTimer <= 0) {
                if (Random(100) < 60) {
                    SetState(0xc0);
                } else {
                    SetState(0xc1);
                }
                Character_SetFace(this, 8);
            } else {
                if (IsWithinReach(mPartner->GetFloorPos(), 0x2666, 0x2666)) {
                    func_0209171c(this, mPartner->GetFloorPos());
                }
                if (StepWalkLooking(0x1800, 0, 0x199, 0x22, 6) || ++mCounter > 200) {
                    mCounter = 0;
                    FloorPos pos;
                    if (Random(100) < 50) {
                        GetRandomCorner(&pos);
                    } else {
                        GetRandomPosInArea(&pos);
                    }
                    VEC_Set(&mTarget, pos.x, 0, pos.z);
                }
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xc2:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound0UnlessGroupA();
                mTimer = Random(100) + 400;
                int r = Random(100);
                if (r < 70) {
                    SetAnimation(RESOURCE_BCA0, 0x19);
                    Character_SetFace(this, GetFace());
                } else if (r < 90) {
                    SetAnimation(RESOURCE_BCA0, 0x1b);
                    Character_SetFace(this, GetFace());
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x17);
                    Character_SetFace(this, 6);
                    if (mModel.mBlendFrames > 0) {
                        Character_ResetSoundPhase(this, 0);
                    }
                }
                SetHeldItemsShowState(1);
                ShowHeadIcon();
                mCounter = 0;
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                int r = Random(100);
                if (r < 55) {
                    if (mModel.mAnimId != 0x17) {
                        mCounter = 1;
                    } else {
                        SetHeadTurn(0, 0, 0, 5);
                    }
                } else if (r < 90) {
                    if (mModel.mAnimId != 0x17) {
                        u16 yaw = (0x20 - Random(0x40)) << 8;
                        u16 pitch = (0x10 - Random(0x20)) << 8;
                        SetHeadTurn(pitch, yaw, 0, 5);
                    } else {
                        SetHeadTurn(0, 0, 0, 5);
                    }
                } else {
                    SetHeadTurn(0, 0, 0, 5);
                }
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0xc2);
            } else if (mCounter == 1) {
                u16 yaw = GetHeadYawTo(mPartner->GetFloorPos(), 0x3000);
                SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), yaw, 0, 4);
            }
        }
        if (mModel.mAnimId == 0x17) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x22, 9, 4);
        }
        break;

    case 0xc3:
        if (mStep == 0 && mModel.mBlendFrames <= 0) {
            mStep++;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            Character_SetFace(this, GetFace());
            SetHeldItemsShowState(1);
            ShowHeadIcon();
        }
        break;

    case 0xc4:
        if (mStep != 0) {
            break;
        }
        if (mSlot == 0) {
            if (mPartner->mState != 0xc4) {
                break;
            }
            mStep++;
            own = GetFloorPos();
            other = mPartner->GetFloorPos();
            fx32 ownDistance = own.LengthSq();
            fx32 otherDistance = other.LengthSq();
            if (otherDistance < 0x1000) {
                SetStateWithPartner(0xc9, 0xc8);
            } else if (ownDistance <= 0x6000) {
                SetStateWithPartner(0xc5, 0xc3);
            } else if (ownDistance < otherDistance && own.Dot(other) > 0) {
                SetStateWithPartner(0xc6, 0xc3);
            } else {
                SetStateWithPartner(0xc3, 0xc7);
            }
        } else {
            mStep++;
        }
        break;

    case 0xc5:
        if (mStep == 0 && mSlot == 0) {
            FloorPos own = GetFloorPos();
            if (own.LengthSq() <= 0x6000) {
                func_02091598(this, mPartner->GetFloorPos());
                StepWalkTo(GetTargetFloorPos());
            } else {
                SetStateWithPartner(0xc3, 0xc7);
            }
        }
        break;

    case 0xc6:
        if (mStep == 0 && mSlot == 0) {
            FloorPos own = GetFloorPos();
            if (own.LengthSq() > 0x4000) {
                mTarget.x = 0;
                mTarget.z = 0;
                StepWalkTo(GetTargetFloorPos());
            } else {
                SetStateWithPartner(0xc5, 0xc3);
            }
        }
        break;

    case 0xc7:
        if (mSlot == 0) {
            break;
        }
        if (mStep == 0) {
            mStep++;
            mTarget.x = 0;
            mTarget.z = 0;
            mTargetAngle = AngleTo(GetTargetFloorPos());
        } else if (mStep == 1) {
            if (StepTurnWithAnimation(GetWalkAnim())) {
                mStep++;
                SetHeadTurn(0, 0, 0, 4);
            }
        } else {
            if (GetFloorPos().LengthSq() < 0x1000) {
                SetStateWithPartner(0xc8, 0xc9);
            } else {
                mTarget.x = 0;
                mTarget.z = 0;
                StepWalkTo(GetTargetFloorPos());
            }
        }
        break;

    case 0xc8:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            mTimer = 150;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            Character_SetFace(this, GetFace());
            SetHeldItemsShowState(1);
            ShowHeadIcon();
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0xc2);
            }
        }
        break;

    case 0xc9:
        if (mStep == 0) {
            if (StepChangeItemsThenAnimation(0x20, 0, 0xff)) {
                mStep++;
                mTimer = 300;
                Character_SetFace(this, GetFace());
                mHeldItems[0]->mRotY = 0x7000;
                gSoundManager.StartSeqArc(mSoundHandle, 0x1c, 0);
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0 && AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x21);
                mHeldItems[0]->mRotZ = 0x1000;
            }
        } else {
            mTimer--;
            if (mTimer <= 0 || gSoundManager.CountPlayingSeqArcEntry(0x1c, 0) == 0) {
                SetState(0xc0);
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;
    }
    PlaceHeadIcon();
}

// States 0xca to 0xe0, two characters taking turns: in 0xcf and 0xd1/0xd2 mCounter says whose turn it is (100 and
// up: the partner's) and how far it is; 0xd3/0xd4 end a round, 0xd5..0xd7 bring the two together, 0xd8..0xde walk
// them to either side of the room (by the sign of x) and to a fixed place, 0xdf/0xe0 start again from there.
void RoomCharacter::UpdateStatesCAToE0() {
    // Declared here and not in their case: that is where the original has them on its stack
    FloorPos own;
    FloorPos other;

    switch (mState) {
    case 0xca:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mSlot != 0) {
                break;
            }
            if (mPrevState > 0xca && mPrevState <= 0xd4) {
                int r = Random(100);
                if (r < 35) {
                    SetStateWithPartner(0xcb, 0xcb);
                } else if (r < 70) {
                    SetStateWithPartner(0xcc, 0xcc);
                } else {
                    SetStateWithPartner(0xce, 0xce);
                }
            } else {
                SetHeldItemsShowState(1);
                mPartner->SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
                if (mPartner->IsAnimOfGroupB(mPartner->mModel.mAnimId)) {
                    mPartner->SetAnimation(RESOURCE_BCA0, 0x1a);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            if (mSlot != 0) {
                break;
            }
            SetStateWithPartner(0xd5, 0xd5);
        }
        break;

    case 0xcb:
        if (StepPutHeldItemsDown(0x59)) {
            SetState(0xcf);
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xcf:
        if (mStep == 0) {
            if (mSlot == 0 && mPartner->mState == 0xcf && mModel.mBlendFrames <= 0 &&
                mPartner->mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = Random(300) + 2000;
                mTimer2 = Random(60) + 90;
                mTimer3 = 0;
                mCounter = 0;
            }
        } else if (mModel.mBlendFrames <= 0 && mSlot == 0) {
            RoomCharacter *who;
            u16 phase = mCounter;
            if (phase < 100) {
                who = this;
            } else {
                phase -= 100;
                who = mPartner;
            }
            mTimer--;
            if (who->mModel.mBlendFrames <= 0) {
                if (phase == 0) {
                    mTimer2--;
                    if (mTimer2 <= 0) {
                        mCounter++;
                        who->SetAnimation(RESOURCE_BCA0, 0x5a);
                        func_0208ef00(&who->mModel, TRUE, FALSE);
                        Character_ResetSoundPhase(who, 0);
                    }
                } else if (phase == 1) {
                    if (Character_PlaySoundAtAnimPhase(who, 0, 1, 0x25, 1, 4) == 2 ||
                        AnimModel_FindAnimation(&who->mModel, RESOURCE_BCA0, who->mModel.mAnimId)->mFinished) {
                        mCounter++;
                        mTimer2 = Random(15) + 2;
                        if (Random(100) < 60) {
                            Character_SetFace(who, 0x15);
                        } else {
                            Character_SetFace(who, 8);
                        }
                    }
                } else {
                    mTimer2--;
                    if (mTimer2 <= 0 || AnimModel_FindAnimation(&who->mModel, RESOURCE_BCA0, who->mModel.mAnimId)->mFinished) {
                        who->SetAnimation(RESOURCE_BCA0, 0x59);
                        Character_SetFace(who, 8);
                        if (Random(100) < 9) {
                            mTimer2 = Random(30) + 150;
                            mCounter = 0;
                            mTimer3++;
                        } else {
                            ModelAnimation *anim = AnimModel_FindAnimation(&who->mModel, RESOURCE_BCA0, who->mModel.mAnimId);
                            if (anim != NULL) {
                                int frames = (((NNSG3dResAnmCommon *)anim->mAnmObj->resAnm)->numFrame << FX32_SHIFT) >> FX32_SHIFT;
                                frames = (frames * who->mPartner->mAnimSpeed) >> FX32_SHIFT;
                                mTimer2 = (Random(frames) >> 3) + (frames >> 4);
                            } else {
                                mTimer2 = Random(18) + 2;
                            }
                            if (mCounter < 100) {
                                mCounter = 100;
                            } else {
                                mCounter = 0;
                            }
                        }
                        if (mTimer <= 0 && mTimer3 >= 4) {
                            SetStateWithPartner(0xd3, 0xd3);
                        }
                    }
                }
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xcc:
        if (StepPutHeldItemsDown(0x2f)) {
            SetState(0xd0);
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xd0:
        if (mStep == 0) {
            if (mPartner->mState == 0xd0) {
                mStep++;
                mTimer = Random(300) + 1700;
                mTimer2 = Random(60) + 90;
            }
        } else if (mModel.mBlendFrames <= 0) {
            mTimer--;
            if (mTimer <= 0) {
                if (mPartner->mModel.mBlendFrames <= 0) {
                    SetStateWithPartner(0xd3, 0xd3);
                    SetAnimation(RESOURCE_BCA0, 0x2f);
                    mPartner->SetAnimation(RESOURCE_BCA0, 0x2f);
                }
            } else {
                mTimer2--;
                if (mTimer2 <= 0) {
                    mTimer2 = Random(110) + 120;
                    mTimer2 += ((FX32_ONE - mAnimSpeed) * 50) >> FX32_SHIFT;
                    int r = Random(100);
                    if (mModel.mAnimId == 0x2f) {
                        r += 10;
                    } else {
                        r -= 10;
                    }
                    if (r < 50) {
                        SetAnimation(RESOURCE_BCA0, 0x2f);
                    } else {
                        SetAnimation(RESOURCE_BCA0, 0x30);
                    }
                    r = Random(100);
                    if (r < 20) {
                        Character_SetFace(this, 0x15);
                    } else if (r < 40) {
                        Character_SetFace(this, 0xb);
                    } else if (r < 60) {
                        Character_SetFace(this, 0x10);
                    } else {
                        Character_SetFace(this, 8);
                    }
                }
            }
        }
        if (mModel.mAnimId == 0x30) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x25, 3, 10);
        } else {
            Character_ResetSoundPhase(this, 0);
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xcd:
    case 0xce:
        if (StepPutHeldItemsDown((mSlot == 0 && mState == 0xcd) ? 0x2d : 7)) {
            if (mState == 0xcd) {
                SetState(0xd1);
            } else {
                SetState(0xd2);
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xd1:
    case 0xd2:
        if (mStep == 0) {
            if (mSlot == 0) {
                switch (mPartner->mState) {
                case 0xd1:
                case 0xd2:
                    if (mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                        mStep++;
                        mCounter = 0;
                        mTimer2 = Random(60) + 90;
                        mTimer3 = 0;
                        if (mState != 0xd1) {
                            mTimer = 0;
                        } else {
                            mTimer = Random(3) * 2 + 12;
                        }
                    }
                    break;
                }
            }
        } else if (mSlot == 0) {
            RoomCharacter *who;
            u16 phase = mCounter;
            if (phase < 100) {
                who = this;
            } else {
                phase -= 100;
                who = mPartner;
            }
            if (who->mModel.mBlendFrames <= 0) {
                if (phase == 0) {
                    mTimer2--;
                    if (mTimer2 <= 0) {
                        mCounter++;
                        mTimer2 = Random(30) + 80;
                        who->SetAnimation(RESOURCE_BCA0, 0x2d);
                        Character_SetFace(who, 8);
                        who->SetHeadTurn(0, 0, 0, 4);
                    }
                } else if (phase == 1) {
                    mTimer2--;
                    if (mTimer2 <= 0) {
                        mCounter++;
                        mTimer3++;
                        who->SetAnimation(RESOURCE_BCA0, 0x2e);
                        func_0208ef00(&who->mModel, TRUE, FALSE);
                        Character_ResetSoundPhase(who, 0);
                    }
                } else if (phase == 2) {
                    Character_PlaySoundAtAnimPhase(who, 0, mSoundHandle, 0x25, 2, 4);
                    if (AnimModel_FindAnimation(&who->mModel, RESOURCE_BCA0, who->mModel.mAnimId)->mFinished) {
                        mCounter++;
                        mTimer2 = 250;
                        who->SetAnimation(RESOURCE_BCA0, 0x2d);
                        int r = Random(100);
                        if (r < 15) {
                            Character_SetFace(who, 2);
                        } else if (r < 30) {
                            Character_SetFace(who, 4);
                        } else if (r < 45) {
                            Character_SetFace(who, 6);
                        } else if (r < 60) {
                            Character_SetFace(who, 5);
                        } else {
                            Character_SetFace(who, 8);
                        }
                        if (Random(100) < 20) {
                            u16 yaw = who->mPartner->GetHeadYawTo(who->GetFloorPos(), 0x2000);
                            who->mPartner->SetHeadTurn(who->mPartner->GetHeadPitchToPartner(0, 0x1800), yaw, 0, 5);
                        }
                    }
                } else if (phase == 3) {
                    if (gSoundManager.CountPlayingSeqArcEntry(0x25, 2) == 0 || --mTimer2 <= 0) {
                        mCounter++;
                        Character_SetFace(who, 8);
                        who->mPartner->SetHeadTurn(0, 0, 0, 5);
                    }
                } else {
                    mTimer2 = Random(60) + 60;
                    if (mTimer3 >= 2 || Random(100) < 20) {
                        mTimer3 = 0;
                        mTimer++;
                        who->SetAnimation(RESOURCE_BCA0, 7);
                        if (mCounter < 100) {
                            mCounter = 100;
                        } else {
                            mCounter = 0;
                        }
                    } else {
                        if (mCounter < 100) {
                            mCounter = 0;
                        } else {
                            mCounter = 100;
                        }
                    }
                    if (mTimer >= 20) {
                        SetStateWithPartner(0xd3, 0xd3);
                    }
                }
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xd3:
        if (mStep == 0) {
            mStep++;
            mTimer = Random(20) + 50;
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 4);
            Character_SetFace(this, 8);
        } else if (mStep == 1) {
            if (mSlot == 0) {
                if (mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0 && mState == mPartner->mState) {
                    mTimer--;
                    if (mTimer <= 0) {
                        gSoundManager.StopSeqArc(1, 0);
                        mStep++;
                        mTimer = Random(30) + 150;
                        RoomCharacter *first;
                        RoomCharacter *second;
                        if (Random(2) < 1) {
                            first = this;
                            second = mPartner;
                        } else {
                            first = mPartner;
                            second = this;
                        }
                        int r = Random(100);
                        if (r < 35) {
                            first->SetAnimation(RESOURCE_BCA0, 0x2a);
                            Character_ResetSoundPhase(first, 0);
                        } else if (r < 70) {
                            first->SetAnimation(RESOURCE_BCA0, 8);
                        } else {
                            first->SetAnimation(RESOURCE_BCA0, 0x4c);
                        }
                        if (Random(100) < 60) {
                            Character_SetFace(first, 2);
                        } else {
                            Character_SetFace(first, 8);
                        }
                        r = Random(100);
                        if (r < 35) {
                            second->SetAnimation(RESOURCE_BCA0, 6);
                        } else if (r < 70) {
                            second->SetAnimation(RESOURCE_BCA0, 1);
                        } else {
                            second->SetAnimation(RESOURCE_BCA0, 0x43);
                        }
                        if (Random(100) < 60) {
                            Character_SetFace(second, 4);
                        } else {
                            Character_SetFace(second, 8);
                        }
                    }
                }
            } else {
                mStep++;
            }
        } else if (mStep == 2) {
            if (mSlot == 0 && mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                mTimer--;
                if (mTimer <= 0) {
                    mStep++;
                    mTimer = Random(30) + 120;
                    SetAnimation(RESOURCE_BCA0, 7);
                    mPartner->SetAnimation(RESOURCE_BCA0, 7);
                }
            }
            if (mModel.mAnimId == 0x2a) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1f, 0, 2);
            }
        } else {
            if (mSlot == 0) {
                mTimer--;
                if (mTimer <= 0) {
                    SetStateWithPartner(0xd4, 0xd4);
                }
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xd4:
        if (mStep == 0) {
            if (mSlot == 0) {
                if (mModel.mBlendFrames <= 0) {
                    mStep++;
                    mTimer = Random(60) + 150;
                    StartSound3IfAnimOfGroup();
                    SetAnimation(RESOURCE_BCA0, 0x4d);
                    ShowHeadIcon();
                    SetHeadTurn(0, 0, 0, 4);
                    gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x25, 0);
                }
            } else {
                mStep++;
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 4);
            }
        } else if (mSlot == 0) {
            mTimer--;
            if (mTimer <= 0) {
                int r = Random(100);
                if (r < 35) {
                    SetStateWithPartner(0xcb, 0xcb);
                } else if (r < 70) {
                    SetStateWithPartner(0xcc, 0xcc);
                } else {
                    SetStateWithPartner(0xce, 0xce);
                }
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xd5:
        StepWalkTo(mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x3000)) {
            SetStateWithPartner(0xd6, 0xd7);
        }
        break;

    case 0xd6:
        if (mStep == 0) {
            if (StepIdleThenAnimation55()) {
                mStep++;
                mTimer = Random(60) + 70;
            }
        } else {
            StepIdleThenAnimation55();
            mTimer--;
            if (mTimer <= 0) {
                SetStateWithPartner(0xd8, 0xd8);
            }
        }
        break;

    case 0xd7:
        StepRandomAnimationFacingPartner();
        break;

    case 0xd8:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            }
        } else if (mSlot == 0) {
            own = GetFloorPos();
            other = mPartner->GetFloorPos();
            if (FX_Mul(own.x, other.x) < 0) {
                SetStateWithPartner(0xdb, 0xdb);
            } else if (own.x == other.x) {
                if (own.x > 0) {
                    SetStateWithPartner(0xda, 0xdb);
                } else if (own.x < 0) {
                    SetStateWithPartner(0xd9, 0xdb);
                } else {
                    SetStateWithPartner(0xd9, 0xda);
                }
            } else if (own.x > other.x) {
                if (own.x > 0) {
                    SetStateWithPartner(0xdb, 0xda);
                } else {
                    SetStateWithPartner(0xd9, 0xdb);
                }
            } else {
                if (other.x > 0) {
                    SetStateWithPartner(0xda, 0xdb);
                } else {
                    SetStateWithPartner(0xdb, 0xd9);
                }
            }
        }
        break;

    case 0xd9: {
        FloorPos target = GetFloorPos();
        target.x = 0x3000;
        StepWalkTo(target);
        if (mModel.mPos.x >= 0x800) {
            SetState(0xdb);
        }
        break;
    }

    case 0xda: {
        FloorPos target = GetFloorPos();
        target.x = -0x3000;
        StepWalkTo(target);
        if (mModel.mPos.x <= -0x800) {
            SetState(0xdb);
        }
        break;
    }

    case 0xdb:
        if (mStep == 0) {
            if (mPartner->mState == 0xdb) {
                if (mModel.mPos.x >= 0) {
                    SetStateWithPartner(0xdc, 0xdd);
                } else {
                    SetStateWithPartner(0xdd, 0xdc);
                }
            } else if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            }
        } else if (mPartner->mState == 0xdb) {
            if (mModel.mPos.x >= 0) {
                SetStateWithPartner(0xdc, 0xdd);
            } else {
                SetStateWithPartner(0xdd, 0xdc);
            }
        }
        break;

    case 0xdc:
    case 0xdd: {
        FloorPos target = gRoomPoints[11];
        if (mState == 0xdd) {
            target.x = -target.x;
        }
        if (StepWalkTo(target)) {
            SetState(0xde);
        }
        break;
    }

    case 0xde:
        if (mStep == 0) {
            mStep++;
            mTargetAngle = AngleTo(gRoomPoints[12]);
        } else if (mStep == 1) {
            if (StepTurnWithAnimation(GetWalkAnim())) {
                mStep++;
                SetHeadTurn(0, 0, 0, 4);
                if (mPartner->mModel.mPos.z < 0x1800) {
                    SetState(0xdf);
                }
            }
        } else if (mStep == 2) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            }
        } else {
            if (mPartner->mModel.mPos.z < 0x1800) {
                SetState(0xdf);
            }
        }
        break;

    case 0xdf:
        if (mStep == 0) {
            if (mSlot == 0) {
                if (StepChangeItemsThenAnimation(0x20, 2, 3)) {
                    mStep++;
                    AnimModel_FinishAnimations(mHeldItems[1]);
                    PlaceHeldItemsAtStart();
                }
            } else {
                if (StepChangeItemsThenAnimation(0x20, 2, 0xff)) {
                    mStep++;
                    mHeldItems[0]->mRotY = 0x8000;
                }
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0 && AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                mTimer = 150;
                SetAnimation(RESOURCE_BCA0, 0x4d);
                gSoundManager.StartSeqArcIfNotPlaying(mSoundHandle, 0x25, 0);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                mKeepSound = TRUE;
                SetState(0xe0);
            }
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;

    case 0xe0:
        if (mPartner->mState == 0xe0) {
            SetHeadTurn(0, 0, 0, 4);
            int r = Random(100);
            if (r < 35) {
                SetStateWithPartner(0xcb, 0xcb);
            } else if (r < 70) {
                SetStateWithPartner(0xcc, 0xcc);
            } else {
                SetStateWithPartner(0xce, 0xce);
            }
        } else if (mPartner->mState == 0xdf) {
            SetHeadTurn(0, 0, 0, 4);
        } else {
            SetHeadTurn(0, GetHeadYawTo(mPartner->GetFloorPos(), 0x3000), 0, 4);
        }
        mHeldItems[0]->SetJoint(mModel.GetJointMatrix(2), mModel.GetJointRotation(2));
        break;
    }
    PlaceHeadIcon();
}

// States 0xe1 to 0xf1, two characters side by side: 0xe2/0xe3 stay there, changing face and looking at each other
// now and then; 0xe4..0xe8 and 0xed..0xf1 are the same walk up to the partner, ending in 0xe2 or 0xea; 0xea looks at
// the room's target, then gives both characters their next event (0x18 and 0x17) and saves them; 0xeb/0xec turn
// away and hand both back to state 2.
void RoomCharacter::UpdateStatesE1ToF1() {
    int state = mState;

    switch (state) {
    case 0xe1:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mSlot != 0) {
                break;
            }
            if (mPrevState > 0xe1 && mPrevState <= 0xe3) {
                SetStateWithPartner(mPrevState, mPrevState);
            } else {
                SetHeldItemsShowState(1);
                mPartner->SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
                if (mPartner->IsAnimOfGroupB(mPartner->mModel.mAnimId)) {
                    mPartner->SetAnimation(RESOURCE_BCA0, 0x1a);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            if (mSlot != 0) {
                break;
            }
            SetStateWithPartner(0xe4, 0xe4);
        }
        break;

    case 0xe2:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            StartAnimationWithIcon(GetIdleAnim());
            SetHeadTurn(0, 0, 0, 5);
            Character_SetFace(this, 8);
            mTimer2 = 1;
            mTimer3 = 2;
        } else {
            mTimer2--;
            if (mTimer2 <= 0) {
                mTimer2 = Random(100) + 300;
                if (mSlot == 0) {
                    if (Random(100) < 35) {
                        Character_SetFace(this, 8);
                    } else {
                        Character_SetFace(this, 2);
                    }
                    Character_SetFace(mPartner, mFace);
                }
            }
            mTimer3--;
            if (mTimer3 <= 0) {
                mTimer3 = Random(100) + 400;
                if (mSlot == 0) {
                    SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), 0, 0, 4);
                    mPartner->SetHeadTurn(mPartner->GetHeadPitchToPartner(0, 0x1800), 0, 0, 4);
                }
            }
        }
        break;

    case 0xe3:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound0UnlessGroupA();
            StartAnimationWithIcon(0x19);
            SetHeadTurn(0, 0, 0, 5);
            Character_SetFace(this, 2);
            mTimer2 = 200;
            mTimer3 = 300;
        } else {
            mTimer2--;
            if (mTimer2 <= 0) {
                mTimer2 = Random(100) + 300;
                if (mSlot == 0) {
                    if (Random(100) < 35) {
                        Character_SetFace(this, 8);
                    } else {
                        Character_SetFace(this, 2);
                    }
                    Character_SetFace(mPartner, mFace);
                }
            }
            mTimer3--;
            if (mTimer3 <= 0) {
                mTimer3 = Random(100) + 500;
                if (mSlot == 0) {
                    if (Random(100) < 20) {
                        SetHeadTurn(0, GetHeadYawTo(mPartner->GetFloorPos(), 0x3000), 0, 5);
                        mPartner->SetHeadTurn(0, mPartner->GetHeadYawTo(GetFloorPos(), 0x3000), 0, 5);
                    } else {
                        SetHeadTurn(0, 0, 0, 5);
                        mPartner->SetHeadTurn(0, 0, 0, 5);
                    }
                }
            }
        }
        break;

    case 0xe4:
    case 0xed:
        StepWalkTo(mPartner->GetFloorPos());
        if (IsWithinReach(mPartner->GetFloorPos(), 0x2800, 0x2999)) {
            if (mState == 0xe4) {
                SetStateWithPartner(0xe5, 0xe6);
            } else {
                SetStateWithPartner(0xee, 0xef);
            }
        }
        break;

    case 0xe5:
    case 0xee:
        if (mStep == 0) {
            if (StepIdleThenAnimation55()) {
                mStep++;
                mTimer = Random(60) + 70;
            }
        } else {
            StepIdleThenAnimation55();
            mTimer--;
            if (mTimer <= 0) {
                if (mState == 0xe5) {
                    SetStateWithPartner(0xe7, 0xe7);
                } else {
                    SetStateWithPartner(0xf0, 0xf0);
                }
            }
        }
        break;

    case 0xe6:
    case 0xef:
        StepRandomAnimationFacingPartner();
        break;

    case 0xe7:
    case 0xf0:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            RestartStepSound();
            SetAnimation(RESOURCE_BCA0, GetWalkAnim());
            Character_SetFace(this, 8);
            SetTargetFloorPos(mPartner->GetFloorPos());
            ShowHeadIcon();
            SetHeldItemsShowState(1);
        } else {
            if (StepTurnLooking(0x100, 0, 0x3800)) {
                SetHeadTurn(0, 0, 0, 4);
                if (mState == 0xe7) {
                    SetState(0xe8);
                } else {
                    SetState(0xf1);
                }
            }
        }
        break;

    case 0xe8:
    case 0xf1:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, GetIdleAnim());
        } else if (mSlot == 0) {
            if (state == 0xe8) {
                if (mPartner->mState == 0xe8) {
                    SetStateWithPartner(0xe2, 0xe2);
                }
            } else {
                if (mPartner->mState == 0xf1) {
                    SetStateWithPartner(0xea, 0xea);
                }
            }
        }
        break;

    case 0xe9:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            if (mSlot != 0) {
                break;
            }
            if (mPrevState > 0xe9 && mPrevState <= 0xea) {
                SetStateWithPartner(mPrevState, mPrevState);
            } else {
                SetHeldItemsShowState(1);
                mPartner->SetHeldItemsShowState(1);
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1a);
                }
                if (mPartner->IsAnimOfGroupB(mPartner->mModel.mAnimId)) {
                    mPartner->SetAnimation(RESOURCE_BCA0, 0x1a);
                }
            }
        } else {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            if (mPartner->mModel.mBlendFrames > 0) {
                break;
            }
            if (mSlot != 0) {
                break;
            }
            SetStateWithPartner(0xed, 0xed);
        }
        break;

    case 0xea:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            StartSound3IfAnimOfGroup();
            StartAnimationWithIcon(GetIdleAnim());
            SetHeadTurn(0, 0, 0, 5);
            Character_SetFace(this, 2);
            mTimer = 80;
        } else if (mStep == 1) {
            mTimer--;
            if (mTimer <= 0) {
                mStep++;
                mTimer = 100;
                Character_SetFace(this, 5);
                mHeadIcon.SetMode(1);
                if (mSlot == 0) {
                    mWait = 10000;
                    mPartner->mWait = 10000;
                    SetEvent(0x18);
                    mPartner->SetEvent(0x17);
                    Save();
                    mPartner->Save();
                }
            } else {
                SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), 0, 0, 4);
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                if (mSlot == 0) {
                    SetStateWithPartner(0xeb, 0xeb);
                }
            } else {
                u16 pitch = GetHeadPitch(CharacterModel_GetJointPosition(&mModel, 0), mCamera->mPosition, 0x1800);
                u16 yaw = GetHeadYawTo(mCamera->GetFloorPos(), 0x3800);
                SetHeadTurn(pitch, yaw, 0, 2);
            }
        }
        break;

    case 0xeb:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            RestartStepSound();
            SetAnimation(RESOURCE_BCA0, GetWalkAnim());
            Character_SetFace(this, 8);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 3);
            mTargetAngle = mAngle + 0x5000 + Random(0x1000) * 6;
        } else {
            func_02091fbc(this, 1, 0, 0x1a);
            if (Character_TurnToTargetAngle(this, 0x200, 0)) {
                SetState(0xec);
            }
        }
        break;

    case 0xec:
        if (mStep == 0) {
            if (mModel.mBlendFrames > 0) {
                break;
            }
            mStep++;
            SetAnimation(RESOURCE_BCA0, GetIdleAnim());
        } else if (mPartner->mState == 0xec) {
            SetStateWithPartner(2, 2);
            mReaction = 0;
            mPartner->mReaction = 0;
            mWait = 150;
            mPartner->mWait = Random(30) + 160;
        }
        break;
    }
    PlaceHeadIcon();
}

// ===================================================================================================================
// RoomCharacterStatesF2

void RoomCharacter::UpdateStatesF2To103() {
    switch (mState) {
    case 0xF2:
        if (mStep == 0) {
            if (StepWalkTo(mPartner->GetFloorPos())) {
                mStep++;
            }
        }
        break;
    case 0xF3:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                RestartStepSound();
                if (Character_TurnToWalkTarget(this, 0x100, 0)) {
                    mStep += 2;
                } else {
                    mStep++;
                    StartSound3IfAnimOfGroup();
                    SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                }
                Character_SetFace(this, GetFace());
                SetHeldItemsShowState(1);
                SetHeadTurn(0, 0, 0, 3);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            mHeadPitch = GetHeadPitchToPartner(0, 0x1800);
            if (StepTurnLooking(0x100, 0, 0x3800)) {
                mStep++;
            }
        } else {
            if (mModel.mBlendFrames <= 0) {
                SetState(0);
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), 0, 0, 3);
            } else {
                func_02091fbc(this, 1, 0, 0x1A);
            }
        }
        Character_MoveAlongAngle(this);
        break;
    case 0xF4:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0x31);
                Character_SetFace(this, GetFace());
                mTimer = 70;
                SetHeldItemsShowState(1);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 5);
                Character_ResetSoundPhase(this, 0);
            }
        } else if (mStep == 1) {
            SetTargetFloorPos(mPartner->GetFloorPos());
            Character_TurnToWalkTarget(this, 0x100, 0);
            Character_MoveAlongAngle(this);
            mTimer--;
            if (mTimer <= 0 && mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            }
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 3, 10);
        } else {
            if (mModel.mBlendFrames <= 0) {
                SetState(0);
            }
        }
        break;
    case 0xF5:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                RestartStepSound();
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                Character_SetFace(this, GetFace());
                mTimer = ((200 - func_02098ec4(this)) >> 3) + 25;
                SetHeldItemsShowState(1);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 5);
            }
        } else if (mStep == 1) {
            mTimer--;
            if (StepTurnLooking(0x180, 4, 0x8000) || mTimer <= 0) {
                mStep++;
            }
        } else if (mStep == 2) {
            if (ClampToWalkArea(NULL) || StepWalkLooking(0x8000, 0, 0x23D, 0x1B, 6)) {
                mStep++;
            }
        } else {
            if (mModel.mBlendFrames <= 0) {
                SetState(0);
            }
        }
        break;
    case 0xF6:
        StepAnimationFacingPartner(GetIdleAnim());
        break;
    case 0xF7:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x55);
                Character_SetFace(this, 0x15);
                SetHeldItemsShowState(1);
                SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), 0, 0, 4);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                SetHeadIcon(13);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 3, 1);
        }
        break;
    case 0xF8:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x56);
                Character_SetFace(this, 0x16);
                SetHeldItemsShowState(1);
                SetHeadTurn(0, 0, 0, 5);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                SetHeadIcon(5);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 3, 1);
        }
        break;
    case 0xF9:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 3);
                Character_SetFace(this, 0x17);
                SetHeldItemsShowState(1);
                SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), 0, 0, 4);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                SetHeadIcon(0);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 3, 1);
        }
        break;
    case 0xFA:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x2C);
                Character_SetFace(this, 0x18);
                SetHeldItemsShowState(1);
                SetHeadTurn(0, 0, 0, 5);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                mHeadIcon.SetMode(1);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 3, 1);
        }
        break;
    case 0xFB:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x37);
                Character_SetFace(this, 0x19);
                SetHeldItemsShowState(1);
                SetHeadTurn(0, 0, 0, 5);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                SetHeadIcon(2);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 3, 1);
        }
        break;
    case 0xFC:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x36);
                Character_SetFace(this, 0x15);
                SetHeldItemsShowState(1);
                SetHeadTurn(0, 0, 0, 5);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                mHeadIcon.SetMode(1);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 3, 1);
        }
        break;
    case 0xFD:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x3E);
                Character_SetFace(this, 0x15);
                SetHeldItemsShowState(1);
                SetHeadTurn(0, 0, 0, 5);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                mHeadIcon.SetMode(1);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 3, 1);
        }
        break;
    case 0xFE:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x57);
                Character_SetFace(this, 0x18);
                SetHeldItemsShowState(1);
                SetHeadTurn(0, 0, 0, 5);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                mHeadIcon.SetMode(1);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 3, 1);
        }
        break;
    case 0xFF:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x48);
                func_0208ef28(&mModel, 1, 0, 0);
                Character_SetFace(this, 0x13);
                SetHeldItemsShowState(1);
                SetHeadTurn(0, 0, 0, 5);
                mHeadIcon.SetMode(1);
                Character_ResetSoundPhase(this, 0);
            }
        } else if (mStep == 1) {
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x49);
                SetHeadIcon(10);
                Character_ResetSoundPhase(this, 0);
            } else {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 5, 2);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 6, 1);
        }
        break;
    case 0x100:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x4A);
                Character_SetFace(this, 2);
                SetHeldItemsShowState(1);
                SetHeadTurn(0, 0, 0, 5);
                mHeadIcon.SetMode(1);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 4, 2);
        }
        break;
    case 0x101:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x38);
                Character_SetFace(this, 0x10);
                SetHeldItemsShowState(1);
                SetHeadTurn(0, 0, 0, 5);
                mHeadIcon.SetMode(1);
            }
        }
        break;
    case 0x102:
        if (mStep == 0) {
            mStep++;
        }
        break;
    case 0x103:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = 16;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                Character_SetFace(this, GetFace());
                SetHeldItemsShowState(1);
                SetHeadTurn(0, 0, 0, 5);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mTimer--;
                if (mTimer <= 0) {
                    mStep++;
                    SetAnimation(RESOURCE_BCA0, 0x39);
                    func_0208ef28(&mModel, 1, 0, 0);
                    Character_ResetSoundPhase(this, 0);
                }
            }
        } else if (mStep == 2) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 4, 2);
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                mTimer = 16;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            }
        } else {
            if (mModel.mBlendFrames <= 0) {
                mTimer--;
                if (mTimer <= 0) {
                    SetState(0);
                }
            }
        }
        break;
    }
    PlaceHeadIcon();
}

void RoomCharacter::UpdateStates104To105() {
    switch (mState) {
    case 0x104:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = 30;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 7);
                Character_SetFace(this, 8);
                SetHeldItemsShowState(1);
                mHeadIcon.SetMode(1);
                SetHeadTurn(0, 0, 0, 5);
            }
        } else if (mStep == 1) {
            mTimer--;
            if (mTimer <= 0 && mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, 0x3A);
                func_0208ef28(&mModel, 1, 0, 0);
                Character_ResetSoundPhase(this, 0);
            }
        } else if (mStep == 2) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 2, 2);
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                mTimer = 140;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0);
            }
        }
        break;
    case 0x105:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = 30;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                Character_SetFace(this, GetFace());
                SetHeldItemsShowState(1);
                SetHeadTurn(0, 0, 0, 5);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            mTimer--;
            if (mTimer <= 0 && mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = 60;
            }
        } else if (mStep == 2) {
            mTimer--;
            if (mTimer <= 0) {
                mStep++;
                mTimer = 100;
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0);
            }
        }
        break;
    }
    PlaceHeadIcon();
}

void RoomCharacter::UpdateStates106To10B() {
    switch (mState) {
    case 0x106:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                RestartStepSound();
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                Character_SetFace(this, GetFace());
                SetTargetFloorPos(gRoomPoints[4]);
                SetHeldItemsShowState(1);
                SetHeadTurn(0, 0, 0, 5);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            if (StepWalkLooking(0x3800, 0, 0x199, 0x22, 6)) {
                mStep++;
                SetTargetFloorPos(gRoomPoints[6]);
                mHeadTurnFrames = 3;
            }
        } else if (mStep == 2) {
            if (StepTurnLooking(0x100, 0, 0x3800)) {
                mStep++;
                SetHeadTurn(0, 0, 0, 5);
            }
        } else if (mStep == 3) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            }
        } else if (mStep == 4) {
            mStep++;
            func_020a4144(&mRoom->mParts, 1);
        } else if (mStep == 5) {
            if (mRoom->mParts.mDoor.mMotion == 0 && mModel.mBlendFrames <= 0) {
                mStep++;
                RestartStepSound();
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                SetTargetFloorPos(gRoomPoints[5]);
            }
        } else if (mStep == 6) {
            if (StepWalkLooking(0x8000, 0, 0x28F, 0x51, 6)) {
                mStep++;
                SetTargetFloorPos(gRoomPoints[10]);
            }
        } else if (mStep == 7) {
            mStep++;
            func_020a4144(&mRoom->mParts, 2);
        } else {
            StepWalkLooking(0x8000, 0, 0x28F, 0x28, 6);
            if (mRoom->mParts.mDoor.mMotion == 0) {
                mPresent = FALSE;
                SetState(0);
                mHeadIcon.SetMode(1);
            }
        }
        break;
    case 0x107:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                RestartStepSound();
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                Character_SetFace(this, GetFace());
                Character_SetWalkTarget(this, data_ov074_0216d4d8);
                mSavedTarget = GetTarget();
                SetHeldItemsShowState(1);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 4);
            }
        } else if (mStep == 1) {
            BOOL near = FALSE;
            if (IsWithinReach(mPartner->GetFloorPos(), 0x1800, 0x2333)) {
                SetHeadTurn(0, GetHeadYawTo(GetTargetFloorPos(), 0x3800), 0, mHeadTurnFrames);
                func_0209171c(this, mPartner->GetFloorPos());
                near = TRUE;
            }
            if (StepWalkLooking(0x8000, 0, 0x199, 0x22, 6)) {
                mStep++;
                if (mMood != 2) {
                    SetTargetFloorPos(mPartner->GetFloorPos());
                } else {
                    Character_SetWalkTarget(this, data_ov074_0216d490);
                }
                mSavedTarget = GetTarget();
                mHeadTurnFrames = 3;
            }
            Character_SetWalkTarget(this, mSavedTarget);
            if (!near) {
                SetHeadTurn(0, GetHeadYaw(-mWalkAngle, 0x3800), 0, mHeadTurnFrames);
            }
        } else if (mStep == 2) {
            if (StepTurnLooking(0x100, 0, 0x3800)) {
                mStep++;
                SetHeadTurn(0, 0, 0, mHeadTurnFrames);
            }
        } else if (mStep == 3) {
            if (mPartner->mModel.mBlendFrames <= 0 && mModel.mBlendFrames <= 0) {
                mStep++;
                if (mMood != 2) {
                    if (!GetFlagOfPartnerInline()) {
                        SetAnimation(RESOURCE_BCA0, 0x3A);
                    } else {
                        SetAnimation(RESOURCE_BCA0, 0x39);
                    }
                    func_0208ef28(&mModel, 1, 0, 0);
                    mPartner->SetState(0x108);
                    Character_ResetSoundPhase(this, 0);
                }
            }
        } else if (mStep == 4) {
            if (mMood != 2) {
                if (mModel.mAnimId == 0x3A) {
                    Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 2, 2);
                } else if (mModel.mAnimId == 0x39) {
                    Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 4, 2);
                }
                if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                    if (mPartner->mModel.mBlendFrames <= 0) {
                        mStep++;
                        SetTargetFloorPos(gRoomPoints[0]);
                        RestartStepSound();
                        SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                        mPartner->SetState(0x10A);
                    }
                }
            } else {
                mStep++;
                SetTargetFloorPos(gRoomPoints[2]);
                mPartner->SetState(0x10A);
            }
        } else if (mStep == 5) {
            if (StepTurnLooking(0x180, 4, 0x3800)) {
                mStep++;
                SetHeadTurn(0, 0, 0, mHeadTurnFrames);
            }
        } else if (mStep == 6) {
            mStep++;
            SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            func_020a4144(&mRoom->mParts, 1);
        } else if (mStep == 7) {
            if (mRoom->mParts.mDoor.mMotion == 0 && mModel.mBlendFrames <= 0) {
                mStep++;
                RestartStepSound();
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                SetTargetFloorPos(gRoomPoints[3]);
            }
        } else if (mStep == 8) {
            if (StepWalkLooking(0x8000, 0, 0x28F, 0x51, 6)) {
                mStep++;
                SetTargetFloorPos(gRoomPoints[7]);
            }
        } else if (mStep == 9) {
            mStep++;
            func_02091fbc(this, 1, 0, 0x1A);
            func_020a4144(&mRoom->mParts, 2);
        } else {
            StepWalkLooking(0x8000, 0, 0x28F, 0x28, 6);
            if (mRoom->mParts.mDoor.mMotion == 0) {
                mPresent = FALSE;
                SetState(0);
                mHeadIcon.SetMode(1);
            }
        }
        break;
    case 0x108:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                if (!GetFlagOfPartnerInline()) {
                    SetAnimation(RESOURCE_BCA0, 0x3A);
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x39);
                }
                func_0208ef28(&mModel, 1, 0, 0);
                SetHeldItemsShowState(1);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 5);
                Character_ResetSoundPhase(this, 0);
            }
        } else {
            if (mModel.mAnimId == 0x3A) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 2, 2);
            } else if (mModel.mAnimId == 0x39) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 4, 2);
            }
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                SetState(0);
            }
        }
        break;
    case 0x109:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                RestartStepSound();
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                SetHeldItemsShowState(1);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 3);
            }
        } else if (mStep == 1) {
            SetTargetFloorPos(mPartner->GetFloorPos());
            if (StepTurnLooking(0xD0, 0, 0x3800)) {
                if (mModel.mBlendFrames <= 0) {
                    mStep++;
                    SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                    ShowHeadIcon();
                }
                SetHeadTurn(0, 0, 0, 3);
            }
        } else if (mStep == 2) {
            SetTargetFloorPos(mPartner->GetFloorPos());
            Character_TurnToWalkTarget(this, 0x100, 0);
            Character_MoveAlongAngle(this);
        }
        break;
    case 0x10A:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                Character_SetFace(this, GetFace());
                SetHeldItemsShowState(1);
                SetHeadTurn(0, 0, 0, 5);
                ShowHeadIcon();
            }
        } else {
            if (mModel.mBlendFrames <= 0) {
                SetState(0);
            }
        }
        break;
    case 0x10B:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                RestartStepSound();
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                Character_SetFace(this, GetFace());
                mSavedTarget = GetTarget();
                SetHeldItemsShowState(1);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 5);
            }
        } else if (mStep == 1) {
            if (IsWithinReach(mPartner->GetFloorPos(), 0x1B33, 0x2000)) {
                func_0209171c(this, mPartner->GetFloorPos());
            }
            if (StepWalkLooking(0x3800, 0, 0x23D, 0x28, 5)) {
                mStep++;
                mHeadTurnFrames = 3;
            }
            Character_SetWalkTarget(this, mSavedTarget);
        } else {
            SetTargetFloorPos(mPartner->GetFloorPos());
            if (StepTurnLooking(0x120, 0, 0x3800)) {
                if (mModel.mBlendFrames <= 0) {
                    SetState(0x10A);
                }
            }
        }
        break;
    }
    PlaceHeadIcon();
}

void RoomCharacter::UpdateStates10CTo112() {
    switch (mState) {
    case 0x10C:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mSoundArmed = 1;
                RestartStepSound();
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mAnmObj->frame = 0;
                Character_SetFace(this, GetFace());
                SetPos(data_ov074_0216d4a8);
                Character_SetAngle(this, 0x4000);
                SetHeadTurn(0, 0, 0, 5);
                mUnknown25D4 = 0;
                mUnknown25D6 = 0;
                mUnknown25D8 = 0;
                SetHeldItemsShowState(1);
            }
        } else if (mStep == 1) {
            mStep++;
            func_020a4144(&mRoom->mParts, 1);
            SetTargetFloorPos(gRoomPoints[8]);
            ShowHeadIcon();
        } else if (mStep == 2) {
            if (StepWalkLooking(0x8000, 0, 0x23D, 0x28, 6)) {
                mStep++;
                func_020a4144(&mRoom->mParts, 2);
            }
        } else if (mStep == 3) {
            mStep++;
            FloorPos pos;
            pos.x = MATH_Rand16(&gRandom, 16) * 0x400 - 0x1E66;
            pos.z = MATH_Rand16(&gRandom, 16) * 0x385 - 0x1999;
            SetTargetFloorPos(pos);
            func_02091fbc(this, 1, 0, 0x1A);
            mHeadTurnFrames = 5;
        } else {
            if (StepWalkLooking(0x2000, 0, 0x23D, 0x25, 6)) {
                SetState(0x111);
            }
        }
        break;
    case 0x10D:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mSoundArmed = 1;
                RestartStepSound();
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                Character_SetFace(this, GetFace());
                SetPos(data_ov074_0216d4c0);
                Character_SetAngle(this, 0x4000);
                SetHeadTurn(0, 0, 0, 5);
                mUnknown25D4 = 0;
                mUnknown25D6 = 0;
                mUnknown25D8 = 0;
                SetHeldItemsShowState(1);
            }
        }
        break;
    case 0x10E:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mSoundArmed = 1;
                RestartStepSound();
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                Character_SetFace(this, GetFace());
                SetPos(data_ov074_0216d4cc);
                Character_SetAngle(this, 0x4000);
                SetHeadTurn(0, 0, 0, 5);
                mUnknown25D4 = 0;
                mUnknown25D6 = 0;
                mUnknown25D8 = 0;
                SetHeldItemsShowState(1);
            }
        } else if (mStep == 1) {
            mStep++;
            func_020a4144(&mRoom->mParts, 1);
            SetTargetFloorPos(gRoomPoints[9]);
            ShowHeadIcon();
        } else if (mStep == 2) {
            if (StepWalkLooking(0x8000, 0, 0x23D, 0x33, 6)) {
                mStep++;
                SetTargetFloorPos(gRoomPoints[14]);
                mHeadTurnFrames = 4;
            }
        } else if (mStep == 3) {
            if (StepWalkLooking(0x8000, 0, 0x23D, 0x33, 6)) {
                mStep++;
                SetTargetFloorPos(mPartner->GetFloorPos());
                func_020a4144(&mRoom->mParts, 2);
                mHeadTurnFrames = 3;
            }
            SetHeadTurn(0, GetHeadYawTo(mPartner->GetFloorPos(), 0x3800), 0, mHeadTurnFrames);
        } else if (mStep == 4) {
            if (StepTurnLooking(0x120, 0, 0x3800)) {
                if (mModel.mBlendFrames <= 0) {
                    mStep++;
                    SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                    SetHeadTurn(0, 0, 0, mHeadTurnFrames);
                }
            }
        } else if (mStep == 5) {
            if (mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                mStep++;
                if (!GetFlagOfPartnerInline()) {
                    SetAnimation(RESOURCE_BCA0, 0x3A);
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x39);
                }
                func_0208ef28(&mModel, 1, 0, 0);
                mPartner->SetState(0x10F);
                Character_ResetSoundPhase(this, 0);
            }
        } else {
            if (mModel.mAnimId == 0x3A) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 2, 2);
            } else if (mModel.mAnimId == 0x39) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 4, 2);
            }
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                if (mPartner->mModel.mBlendFrames <= 0) {
                    SetState(0x111);
                    mPartner->SetState(0x111);
                }
            }
        }
        break;
    case 0x112:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                RestartStepSound();
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                Character_SetFace(this, GetFace());
                mSavedTarget = GetTarget();
                SetHeldItemsShowState(1);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 5);
            }
        } else {
            if (StepWalkLooking(0x8000, 0, 0x23D, 0x22, 5)) {
                SetState(0x111);
            }
            Character_SetWalkTarget(this, mSavedTarget);
        }
        break;
    case 0x10F:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                if (!GetFlagOfPartnerInline()) {
                    SetAnimation(RESOURCE_BCA0, 0x3A);
                } else {
                    SetAnimation(RESOURCE_BCA0, 0x39);
                }
                func_0208ef28(&mModel, 1, 0, 0);
                SetHeldItemsShowState(1);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 5);
                Character_ResetSoundPhase(this, 0);
            }
        } else {
            if (mModel.mAnimId == 0x3A) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 2, 2);
            } else if (mModel.mAnimId == 0x39) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 4, 2);
            }
            if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
                SetState(0);
            }
        }
        break;
    case 0x110:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                RestartStepSound();
                SetAnimation(RESOURCE_BCA0, GetWalkAnim());
                SetHeldItemsShowState(1);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 3);
            }
        } else if (mStep == 1) {
            SetTargetFloorPos(mPartner->GetFloorPos());
            if (StepTurnLooking(0x140, 0, 0x3800)) {
                if (mModel.mBlendFrames <= 0) {
                    mStep++;
                    SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                }
                SetHeadTurn(0, 0, 0, mHeadTurnFrames);
            }
        } else {
            SetTargetFloorPos(mPartner->GetFloorPos());
            Character_TurnToWalkTarget(this, 0x60, 0);
            Character_MoveAlongAngle(this);
        }
        break;
    case 0x111:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, GetIdleAnim());
                Character_SetFace(this, GetFace());
                SetHeldItemsShowState(1);
                SetHeadTurn(0, 0, 0, 5);
                ShowHeadIcon();
            }
        } else {
            if (mModel.mBlendFrames <= 0) {
                SetState(0);
            }
        }
        break;
    }
    PlaceHeadIcon();
}

void RoomCharacter::UpdateStates113To116() {
    switch (mState) {
    case 0x113:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                if (IsAnimOfGroupB(mModel.mAnimId)) {
                    SetAnimation(RESOURCE_BCA0, 0x1A);
                }
                Character_SetFace(this, GetFaceOfMood(0));
                mHeadTurnActive = 1;
                SetHeldItemsShowState(1);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = 90;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 0);
                if (func_02099e34(mEvent)) {
                    Character_SetFace(this, 5);
                }
            }
        } else if (mStep == 2) {
            mTimer--;
            if (mTimer <= 0) {
                Character_SetFace(this, GetFaceOfMood(0));
                if (mSlot == 0) {
                    SetState(0);
                } else {
                    mStep++;
                }
            }
        }
        LookAtCamera(3);
        break;
    case 0x114:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                RestartStepSound();
                SetAnimation(RESOURCE_BCA0, 10);
                mTargetAngle = mAngle - mUnknown25D6;
                ShowHeadIcon();
            }
        } else {
            mHeadTurnActive = 1;
            func_02091fbc(this, 1, 0, 0x1A);
            if (Character_TurnToTargetAngle(this, 0xE0, 0x10)) {
                SetState(0);
            }
            Character_MoveAlongAngle(this);
        }
        LookAtCamera(2);
        break;
    case 0x115:
    case 0x116:
        if (mStep == 0) {
            mStep++;
            if (mState == 0x115) {
                if (mUnknown2AF0 < 0x1000) {
                    mTimer = 40;
                } else {
                    mTimer = 30;
                }
            } else {
                mTimer = 10;
            }
        } else {
            mTimer--;
            if (mTimer <= 0 && mModel.mBlendFrames <= 0) {
                SetState(0);
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, 7);
            }
            mHeadTurnActive = 1;
            StepWalkLooking(0x8000, 0, 0x28F, 0x1D, 5);
        }
        LookAtCamera(2);
        break;
    }
    PlaceHeadIcon();
}

void RoomCharacter::UpdateState117() {
    if (mState == 0x117) {
        if (mStep == 0 && mModel.mBlendFrames <= 0) {
            mStep++;
            if (IsAnimOfGroupB(mModel.mAnimId)) {
                SetAnimation(RESOURCE_BCA0, 0x1A);
            }
            SetHeadTurn(0, 0, 0, 5);
        }
    } else {
        SetState(0);
    }
    PlaceHeadIcon();
}

int RoomCharacter::StepWalkToRandomPos() {
    int done = 0;
    if (mSubStep == 0) {
        if (mModel.mBlendFrames <= 0) {
            FloorPos pos;
            GetRandomPosInArea(&pos);
            SetTarget(pos.x, 0, pos.z);
            StartSound3IfAnimOfGroup();
            RestartStepSound();
            mSubStep++;
            SetAnimation(RESOURCE_BCA0, 0x14);
            Character_SetFace(this, 8);
            SetHeldItemsShowState(1);
            mHeadTurnFrames = 5;
            ShowHeadIcon();
        }
    } else {
        done = StepWalkLooking(0x3800, 0, 0x199, 0x22, 6);
    }
    return done;
}

int RoomCharacter::StepWalkTo(FloorPos pos) {
    int done = 0;
    if (mSubStep == 0) {
        if (mModel.mBlendFrames <= 0) {
            mSubStep++;
            StartSound3IfAnimOfGroup();
            RestartStepSound();
            SetAnimation(RESOURCE_BCA0, GetWalkAnim());
            Character_SetFace(this, GetFace());
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 4);
        }
    } else {
        SetTargetFloorPos(pos);
        if (StepWalkLooking(0x3800, 0, 0x23D, 0x22, 5)) {
            done = 1;
        }
    }
    return done;
}

int RoomCharacter::StepTurnWithAnimation(int anim) {
    int done = 0;
    if (mSubStep == 0) {
        if (mModel.mBlendFrames <= 0) {
            mSubStep++;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, anim);
            if (mModel.mBlendFrames > 0) {
                Character_ResetSoundPhase(this, 0);
            }
            Character_SetFace(this, GetFace());
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 4);
        }
    } else if (mSubStep == 1) {
        func_02091fbc(this, 1, 0, 0x1A);
        if (Character_TurnToTargetAngle(this, 0x120, 0)) {
            mSubStep++;
            done = 1;
        } else {
            SetHeadTurn(0, GetHeadYaw(-mTargetAngle, 0x3800), 0, 4);
        }
        Character_MoveAlongAngle(this);
    } else {
        done = 1;
    }
    return done;
}

int RoomCharacter::StepAlternateAnimations(int animA, int animB) {
    int done = 0;
    if (mSubStep == 0) {
        if (mModel.mBlendFrames <= 0) {
            mSubStep++;
            mCounter = 0;
            StartSound3IfAnimOfGroup();
            mTimer4 = MATH_Rand16(&gRandom, 80) + 250;
            if (MATH_Rand16(&gRandom, 0) & 1) {
                SetAnimation(RESOURCE_BCA0, animA);
            } else {
                SetAnimation(RESOURCE_BCA0, animB);
            }
            if (MATH_Rand16(&gRandom, 0) & 1) {
                Character_SetFace(this, 8);
            } else {
                Character_SetFace(this, 10);
            }
            SetHeldItemsShowState(1);
            SetHeadTurn(0, 0, 0, 5);
            ShowHeadIcon();
            if (mModel.mBlendFrames > 0) {
                func_0208ef28(&mModel, 1, 0, 0);
                Character_ResetSoundPhase(this, 0);
            } else {
                func_0208ef00(&mModel, 1, 0);
            }
        }
    } else {
        mTimer4--;
        if (AnimModel_FindAnimation(&mModel, RESOURCE_BCA0, mModel.mAnimId)->mFinished) {
            mCounter++;
            if (mCounter >= 2) {
                mCounter = 0;
                if (mTimer4 > 0) {
                    if (animA == mModel.mAnimId) {
                        SetAnimation(RESOURCE_BCA0, animB);
                    } else {
                        SetAnimation(RESOURCE_BCA0, animA);
                    }
                    Character_ResetSoundPhase(this, 0);
                } else {
                    Character_SetFace(this, 8);
                    done = 1;
                }
            } else {
                Character_ResetSoundPhase(this, 0);
            }
            func_0208ef28(&mModel, 1, 0, 0);
        }
    }
    return done;
}

int RoomCharacter::StepAnimationFacingPartner(int anim) {
    int done = 0;
    if (mSubStep == 0) {
        if (mModel.mBlendFrames <= 0) {
            mSubStep++;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, anim);
            Character_SetFace(this, GetFace());
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
            if (mModel.mAnimId == 0x32 && mModel.mBlendFrames > 0) {
                Character_ResetSoundPhase(this, 0);
            }
        }
    } else if (mSubStep == 1) {
        mSubStep++;
        u16 angle = GetHeadYawTo(mPartner->GetFloorPos(), 0x3800);
        SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), angle, 0, 4);
    } else {
        if (mModel.mAnimId == 0x32) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 7, 2);
        }
        done = 1;
    }
    return done;
}

int RoomCharacter::StepIdleThenAnimation55() {
    int done = 0;
    if (mSubStep == 0) {
        if (mModel.mBlendFrames <= 0) {
            mSubStep++;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, GetIdleAnim());
            Character_SetFace(this, GetFace());
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        }
    } else if (mSubStep == 1) {
        if (mModel.mBlendFrames <= 0) {
            mSubStep++;
            SetAnimation(RESOURCE_BCA0, 0x55);
            Character_SetFace(this, 0x15);
            u16 angle = GetHeadYawTo(mPartner->GetFloorPos(), 0x3800);
            SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), angle, 0, 3);
            Character_ResetSoundPhase(this, 1);
        }
    } else {
        Character_PlaySoundAtAnimPhase(this, 1, 1, 0x1A, 3, 1);
        done = 1;
    }
    return done;
}

int RoomCharacter::StepRandomAnimationFacingPartner() {
    int anim = MATH_Rand16(&gRandom, 100);
    if (anim >= 50) {
        anim = 0x32;
    } else {
        anim = GetIdleAnim();
    }
    return StepAnimationFacingPartner(anim);
}

int RoomCharacter::StepAnimationThenFacePartner(int anim) {
    int done = 0;
    if (mSubStep == 0) {
        if (mModel.mBlendFrames <= 0) {
            mSubStep++;
            StartSound0UnlessGroupA();
            SetAnimation(RESOURCE_BCA0, anim);
            Character_SetFace(this, 0x15);
            SetHeldItemsShowState(1);
            ShowHeadIcon();
            Character_ResetSoundPhase(this, 1);
        }
    } else if (mSubStep == 1) {
        if (mModel.mBlendFrames <= 0) {
            mSubStep++;
            u16 angle = GetHeadYawTo(mPartner->GetFloorPos(), 0x3000);
            SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), angle, 0, 4);
            Character_PlaySoundAtAnimPhase(this, 1, 1, 0x1A, 3, 1);
        }
    } else {
        Character_PlaySoundAtAnimPhase(this, 1, 1, 0x1A, 3, 1);
        done = 1;
    }
    return done;
}

void RoomCharacter::StartAnimationWithIcon(int anim) {
    SetAnimation(RESOURCE_BCA0, anim);
    SetHeadIcon(6);
    SetHeldItemsShowState(1);
}

int RoomCharacter::StepChangeItemsThenAnimation(int anim, int itemA, int itemB) {
    int done = 0;
    if (mSubStep == 0) {
        if (mModel.mBlendFrames <= 0) {
            mSubStep++;
            StartSound3IfAnimOfGroup();
            SetAnimation(RESOURCE_BCA0, 7);
            StepChangeHeldItems(itemA, itemB);
            ShowHeadIcon();
            SetHeadTurn(0, 0, 0, 5);
        }
        mHeldItems[0]->mVisible = 0;
        mHeldItems[1]->mVisible = 0;
    } else if (mSubStep == 1) {
        int changed = StepChangeHeldItems(itemA, itemB);
        if (mModel.mBlendFrames <= 0 && changed) {
            mSubStep++;
            SetAnimation(RESOURCE_BCA0, anim);
            func_0208ef28(&mModel, 1, 0, 0);
            Character_SetFace(this, 8);
        }
        mHeldItems[0]->mVisible = 0;
        mHeldItems[1]->mVisible = 0;
    } else if (mSubStep == 2) {
        mSubStep++;
        if (itemA != 0xFF) {
            mHeldItems[0]->mVisible = 1;
        }
        if (itemB != 0xFF) {
            mHeldItems[1]->mVisible = 1;
        }
        done = 1;
    } else {
        done = 1;
    }
    return done;
}

int RoomCharacter::StepChangeHeldItems(int itemA, int itemB) {
    u8 items[2];
    items[0] = itemA;
    items[1] = itemB;
    int done = 0;
    if (mSubSubStep == 0) {
        if (AreHeldItemsLoaded()) {
            mSubSubStep++;
            for (int i = 0; i < 2; i++) {
                if (items[i] != 0xFF) {
                    mHeldItems[i]->SetKind(items[i]);
                } else {
                    mHeldItems[i]->SetShowState(0);
                    mHeldItems[i]->mVisible = 0;
                }
            }
        }
    } else if (mSubSubStep == 1) {
        if (AreHeldItemsLoaded()) {
            int ready = 0;
            mSubSubStep++;
            for (int i = 0; i < 2; i++) {
                GXRgb color = func_02073514(func_0207a6c0(this));
                NNS_G3dMdlSetMdlDiff(mHeldItems[i]->mResMdl, 0, color);
                mHeldItems[i]->SetRotation(0, 0, 0);
                MTX_Identity33(&mHeldItems[i]->mRotation);
                if (items[i] != 0xFF) {
                    if (mHeldItems[i]->mVisible) {
                        ready++;
                    }
                } else {
                    if (!mHeldItems[i]->mVisible) {
                        ready++;
                    }
                }
            }
            if (ready == 2) {
                done = 1;
            }
        }
    } else if (mSubSubStep == 2) {
        mSubSubStep++;
        for (int i = 0; i < 2; i++) {
            if (items[i] != 0xFF) {
                mHeldItems[i]->mVisible = 1;
            } else {
                mHeldItems[i]->SetShowState(0);
                mHeldItems[i]->mVisible = 0;
            }
        }
        done = 1;
    } else {
        done = 1;
    }
    return done;
}

void RoomCharacter::UpdateBodyIfPresent() {
    if (mPresent) {
        func_02091968(this);
    }
}

int RoomCharacter::PickReaction() {
    return func_0209d804(this, mReaction);
}

void RoomCharacter::ResetBody() {
    func_02091a6c(this);
    func_020910d4(this);
}

u8 RoomCharacter::GetFlagOfPartner() {
    return GetFlagOfPartnerInline();
}

int RoomCharacter::GetFaceOfMood(int variant) {
    return gMoodFaces[variant];
}

int RoomCharacter::GetFace() {
    return gMoodFaces[mMood];
}

void RoomCharacter::GetMidpoint(PositionedNode *a, PositionedNode *b, VecFx32 *out) {
    out->x = (a->mPos.x + b->mPos.x) >> 1;
    out->y = (a->mPos.y + b->mPos.y) >> 1;
    out->z = (a->mPos.z + b->mPos.z) >> 1;
}

void RoomCharacter::SetAnimation(u32 resType, int id) {
    func_020919e8(this, resType, id);
    func_0208ef00(&mModel, 0, 0);
}

BOOL RoomCharacter::IsOneTwoOrFour(int value) {
    switch (value) {
    case 1:
    case 2:
    case 4:
        return TRUE;
    }
    return FALSE;
}

BOOL RoomCharacter::IsState8Or9() {
    switch (mState) {
    case 8:
    case 9:
        return TRUE;
    }
    return FALSE;
}

BOOL RoomCharacter::IsState10To12() {
    switch (mState) {
    case 0x10:
    case 0x11:
    case 0x12:
        return TRUE;
    }
    return FALSE;
}

BOOL RoomCharacter::IsAnimOfGroupB(int anim) {
    switch (anim) {
    case 0x1E:
    case 0x1F:
    case 0x27:
    case 0x3D:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x5B:
    case 0x5C:
        return TRUE;
    }
    return FALSE;
}

BOOL RoomCharacter::IsWalkAnim() {
    int anim = mModel.mAnimId;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            if (anim == data_020fb898[i][j]) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

BOOL RoomCharacter::IsMovingAnim() {
    switch (mModel.mAnimId) {
    case 0x22:
    case 0x35:
    case 0x42:
        return TRUE;
    }
    return IsWalkAnim();
}

BOOL RoomCharacter::IsAnimOfGroupA(int anim) {
    switch (anim) {
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x24:
    case 0x33:
    case 0x3F:
    case 0x40:
    case 0x53:
        return TRUE;
    }
    return FALSE;
}

void RoomCharacter::GetRandomPosInArea(FloorPos *out) {
    out->x = gWalkArea.x + MATH_Rand16(&gRandom, gWalkArea.w);
    out->z = gWalkArea.z + MATH_Rand16(&gRandom, gWalkArea.h);
}

void RoomCharacter::GetRandomPos(FloorPos *out) {
    out->x = MATH_Rand16(&gRandom, 0x4000) - 0x2000;
    out->z = MATH_Rand16(&gRandom, 0x4800) - 0x2800;
}

BOOL RoomCharacter::ClampToRect(FloorPos *pos, const FloorRect *rect) {
    fx32 left = rect->x;
    fx32 top = rect->z;
    fx32 right = left + rect->w;
    fx32 bottom = top + rect->h;
    BOOL clamped = FALSE;
    if (pos->x < left) {
        pos->x = left;
        clamped = TRUE;
    } else if (pos->x > right) {
        pos->x = right;
        clamped = TRUE;
    }
    if (pos->z < top) {
        pos->z = top;
        clamped = TRUE;
    } else if (pos->z > bottom) {
        pos->z = bottom;
        clamped = TRUE;
    }
    return clamped;
}

BOOL RoomCharacter::ClampToWalkArea(FloorPos *pos) {
    if (pos == NULL) {
        FloorPos own = GetFloorPos();
        return ClampToRect(&own, &gWalkArea);
    }
    return ClampToRect(pos, &gWalkArea);
}

BOOL RoomCharacter::ClampToRect4E4(FloorPos *pos) {
    if (pos == NULL) {
        FloorPos own = GetFloorPos();
        return ClampToRect(&own, &data_ov074_0216d4e4);
    }
    return ClampToRect(pos, &data_ov074_0216d4e4);
}

BOOL RoomCharacter::ClampToRect534(FloorPos *pos) {
    if (pos == NULL) {
        FloorPos own = GetFloorPos();
        return ClampToRect(&own, &data_ov074_0216d534);
    }
    return ClampToRect(pos, &data_ov074_0216d534);
}

void RoomCharacter::GetRandomCorner(FloorPos *out) {
    *out = gWalkAreaCorners[MATH_Rand16(&gRandom, 0) % 4u];
}

// Not matching yet: 0xf8 bytes against 0xfc; the instructions are the same but for register allocation in the
// two tails. The original loads the area's w/h into r2 and the generator's x/mul/add into r4/ip/lr (which in the
// second tail splits the `ldm` into `ldr` + `ldm`, the extra instruction); this loads w/h into lr/r4 and x into r2.
// Tried: the area as a direct global, a local pointer, a reference, an fx32 array, {min, size}; u32/int fields;
// if/else instead of the ternary; x read before the second random number; locals for the range and the result;
// int and u16 wrappers around MATH_Rand16; inline helpers for the two halves; the arms swapped; a switch.
void RoomCharacter::GetRandomEdgePos(FloorPos *out) {
    FloorRect *area = &gWalkArea;
    if (MATH_Rand16(&gRandom, 0) & 1) {
        out->x = (MATH_Rand16(&gRandom, 0) & 1) ? area->x : -area->x;
        out->z = MATH_Rand16(&gRandom, area->h) - (area->h >> 1);
    } else {
        out->z = (MATH_Rand16(&gRandom, 0) & 1) ? area->z : -area->z;
        out->x = MATH_Rand16(&gRandom, area->w) - (area->w >> 1);
    }
}

// ===================================================================================================================
// RoomObjects

extern "C" {
// --- The main program's, not decompiled or not named yet ---

// The offset (the plane's number shifted by 11) of a plane's screen in VRAM
int func_020253d4(void *graphicsManager, int plane);

// Splits a 15-bit colour
void func_02028b98(int color, int *r, int *g, int *b);
// Resets a light: direction straight down... colour white, enabled
void func_0202a96c(Light *light);
// Sets a light's direction: the vector, rotated by the three angles and normalized
void func_0202ab2c(Light *light, u16 angleX, u16 angleY, u16 angleZ, fx32 x, fx32 y, fx32 z);
extern Light data_021227cc[];

// The saved game: what is known of a character by its index
u8 func_0202c53c(SavedGame *game, int character);
int func_0202c5b4(SavedGame *game, int character);
int func_0202c58c(SavedGame *game, int character);
struct Unknown02114d78Entry {
    u32 mUnknown0 : 8;
    int mUnknown4;
    int mUnknown8;
};
extern const Unknown02114d78Entry *const data_02114d78[];

// The time-of-day tracker
void func_020a612c(DayPhase *phase, RTCDate *date, RTCTime *time);
void func_020a61cc(DayPhase *phase, RTCDate *date, RTCTime *time);
int func_020a6214(DayPhase *phase);

// The current time with the game's offset applied, as seconds and as date and time
s64 func_02035870(int offset, RTCDate *date, RTCTime *time);
extern int data_02123440[];

// Turns to the walk target and sets the speed; TRUE when the character is within reach of it
BOOL func_02091194(RoomCharacter *character, fx32 reach, int speed, int turnShift);
// Takes the model's animations of a resource type away
void func_0208f0f8(CharacterModel *model, u32 resourceType);
// Writes the name and path of the character's archive (mArchiveName, mArchivePath)
void func_0209107c(RoomCharacter *character);
// Makes the model from the resources of an id
void func_0208ef5c(CharacterModel *model, int resourceId, int unk);
// Copies a text, of at most that many bytes
void func_020372d0(wchar_t *dest, const wchar_t *src, int size);
}

// @0x02168d24
void RoomCharacter::GetWalkAreaCorner(FloorPos pos, FloorPos *corner) {
    *corner = gWalkAreaCorners[GetWalkAreaSide(pos.x, pos.z)];
}

// @0x02168d64
int RoomCharacter::GetWalkAreaSide(fx32 x, fx32 z) {
    if (z == x && z == 0) {
        return 0;
    }
    if (z > x && z >= -x) {
        return 0;
    }
    if (z <= x && z > -x) {
        return 1;
    }
    if (z < x && z <= -x) {
        return 2;
    }
    return 3;
}

// @0x02168dc4
u16 RoomCharacter::SnapAngle(u16 angle) {
    if (angle < 0x2000) {
        angle = 0;
    } else if (angle < 0x6000) {
        angle = 0x4000;
    } else if (angle < 0xA000) {
        angle = 0x8000;
    } else if (angle < 0xE000) {
        angle = 0xC000;
    } else {
        angle = 0;
    }
    return angle;
}

// Not matching yet: 0x32c bytes against 0x348. The stack frame's 21 FloorPos slots are all where the original has
// them (so the inline helpers and their order are right), and the arithmetic is the same; what differs is register
// allocation after the call of FX_Atan2Idx: the original keeps offset.x and offset.z in r5/r4 across the call (and
// spills the high words of the four sines and cosines to five more stack words), this reloads them from the
// stack. Tried: Rotate taking a reference, a pointer, the value, two scalars, or the sine and cosine; the sines and
// cosines as locals; offset.x/z as locals before the call (keeps z in a register, not x); the angle through an
// inline; the two distances right after each point (this) or after both (0x340 bytes) or in the conditions.
// @0x02168dfc
BOOL RoomCharacter::GetAngleAround(u16 *angle, FloorPos from, FloorPos to, int margin) {
    FloorPos middle;
    middle = Half(Add(from, to));
    FloorPos offset = Sub(from, to);
    u16 direction = FX_Atan2Idx(offset.z, offset.x);
    FloorPos right = Add(Rotate(offset, 0x4000), middle);
    fx32 rightDistance = LengthSq(right);
    FloorPos left = Add(Rotate(offset, 0xC000), middle);
    fx32 leftDistance = LengthSq(left);
    if (rightDistance == leftDistance || (middle.x == 0 && middle.z == 0)) {
        *angle = direction;
        return FALSE;
    }
    if (rightDistance > leftDistance) {
        *angle = direction + 0xC000 + margin;
    } else {
        *angle = direction + 0x4000 - margin;
    }
    return TRUE;
}

// @0x02169144
BOOL RoomCharacter::GetAngleAroundPartner(u16 *angle, int margin) {
    BOOL found = GetAngleAround(angle, mPartner->GetFloorPos(), GetFloorPos(), margin);
    if (!found) {
        *angle = mAngle;
    }
    return found;
}

// @0x021691b0
u16 RoomCharacter::GetHeadYaw(u16 angle, int limit) {
    u16 yaw = angle + mAngle;
    func_02090c78(this, &yaw, limit);
    return yaw;
}

// @0x021691d4
void RoomCharacter::SetHeadTurn(u16 pitch, u16 yaw, u16 roll, u8 frames) {
    mHeadPitch = pitch;
    mHeadYaw = yaw;
    mHeadRoll = roll;
    mHeadTurnFrames = frames;
    mHeadTurnActive = TRUE;
}

// @0x021691fc
u16 RoomCharacter::GetHeadYawTo(FloorPos pos, int limit) {
    FloorPos offset = Sub(pos, GetFloorPos());
    return GetHeadYaw(-FX_Atan2Idx(offset.z, offset.x), limit);
}

// @0x02169280
u16 RoomCharacter::GetHeadPitch(VecFx32 from, VecFx32 to, u16 limit) {
    VecFx32 offset;
    VEC_Subtract(&to, &from, &offset);
    fx32 z = MATH_ABS(offset.z);
    fx32 x = MATH_ABS(offset.x);
    u16 pitch = -FX_Atan2Idx(offset.y, FX_Mul((x + z) >> 1, 0x2CCC));
    func_02090c78(this, &pitch, limit);
    return pitch;
}

// @0x02169318
u16 RoomCharacter::GetHeadPitchToPartner(int node, u16 limit) {
    return GetHeadPitch(CharacterModel_GetJointPosition(&mModel, 0),
                        CharacterModel_GetJointPosition(&mPartner->mModel, node), limit);
}

// @0x02169388
BOOL RoomCharacter::StepWalkLooking(u32 lookLimit, int lookAngle, fx32 reach, int speed, int turnShift) {
    BOOL arrived = FALSE;
    func_02091fbc(this, 1, 0, 0x1A);
    if (func_02091194(this, func_02091174(this, reach), speed, turnShift)) {
        arrived = TRUE;
    }
    Character_MoveAlongAngle(this);
    if (lookLimit < 0x4000) {
        SetHeadTurn(mHeadPitch, GetHeadYaw(lookAngle - mWalkAngle, lookLimit), mHeadRoll, mHeadTurnFrames);
    }
    return arrived;
}

// @0x0216943c
BOOL RoomCharacter::StepTurnLooking(int speed, int accel, u32 lookLimit) {
    BOOL facing = FALSE;
    func_02091fbc(this, 1, 0, 0x1A);
    if (Character_TurnToWalkTarget(this, speed, accel)) {
        facing = TRUE;
    }
    if (accel != 0) {
        Character_MoveAlongAngle(this);
    }
    if (lookLimit < 0x4000) {
        SetHeadTurn(mHeadPitch, GetHeadYaw(-mWalkAngle, lookLimit), mHeadRoll, mHeadTurnFrames);
    }
    return facing;
}

// @0x021694e8
void RoomCharacter::RestartStepSound() {
    if (IsMovingAnim()) {
        return;
    }
    Character_ResetSoundPhase(this, 0);
}

// @0x0216950c
void RoomCharacter::SetHeldItemsShowState(int state) {
    for (int i = 0; i < 2; i++) {
        mHeldItems[i]->SetShowState(state);
    }
}

// @0x02169540
void RoomCharacter::SetHeldItemsVisible(BOOL visible) {
    for (int i = 0; i < 2; i++) {
        mHeldItems[i]->mVisible = visible;
    }
}

// @0x02169564
BOOL RoomCharacter::AreHeldItemsLoaded() {
    for (int i = 0; i < 2; i++) {
        if (mHeldItems[i]->mLoadState != HeldItem::LOAD_IDLE) {
            return FALSE;
        }
    }
    return TRUE;
}

// @0x02169598
void RoomCharacter::PlaceHeldItemBetweenHands(fx32 height) {
    VecFx32 middle;
    mHeldItems[0]->mRotY = 0x4000 - mAngle;
    GetMidpoint(GetNode(2), GetNode(1), &middle);
    middle.y += FX_Mul(height, mModel.mScale);
    mHeldItems[0]->SetPosition(middle);
}

// @0x02169634
void RoomCharacter::PlaceHeldItemsInHands() {
    mHeldItems[1]->SetJointOffset(mModel.GetJointMatrix(2), mModel.GetJointRotation(2), 0, 0x800, 0);
    mHeldItems[0]->mRotY = 0x4000 - mAngle;
    MtxFx43 *hand = mModel.GetJointMatrix(1);
    HeldItem *item = mHeldItems[0];
    item->SetPosition(hand->m[3][0], hand->m[3][1], hand->m[3][2]);
}

// @0x021696b8
void RoomCharacter::PlaceHeldItemAtHead() {
    mHeldItems[0]->mRotX = mUnknown25D6;
    mHeldItems[0]->SetJointOffset(mModel.GetJointMatrix(0), mModel.GetJointRotation(0), 0xBD7, 0, 0);
}

// @0x02169708
void RoomCharacter::PlaceHeldItemsAtStart() {
    mHeldItems[0]->mRotY = 0x8000;
    mHeldItems[1]->SetPosition(0, 0, 0x3000);
    mHeldItems[1]->mRotY = 0x8000;
}

// @0x0216973c
BOOL RoomCharacter::StepPutHeldItemsDown(int anim) {
    BOOL done = FALSE;
    if (mSubStep == 0) {
        mSubStep++;
        ShowHeadIcon();
        SetHeadTurn(0, 0, 0, 4);
    } else if (mSubStep == 1) {
        if (mSlot == 0) {
            if (StepChangeHeldItems(2, 3) && mModel.mBlendFrames <= 0) {
                mSubStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, anim);
                Character_SetFace(this, 8);
                PlaceHeldItemsAtStart();
                done = TRUE;
            }
        } else {
            if (StepChangeHeldItems(2, 0xFF) && mModel.mBlendFrames <= 0) {
                mSubStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(RESOURCE_BCA0, anim);
                Character_SetFace(this, 8);
                mHeldItems[0]->mRotY = 0x8000;
                done = TRUE;
            }
        }
    } else {
        done = TRUE;
    }
    return done;
}

// @0x02169890
void RoomCharacter::SetOverrideState(int state) {
    mOverrideState = state;
    mOverrideStep = 0;
}

// @0x021698a8
void RoomCharacter::UpdateOverrideState() {
    switch (mOverrideState) {
    case OVERRIDE_NONE:
        break;
    case OVERRIDE_UNLOAD:
        SetOverrideState(OVERRIDE_NONE);
        SetState(0);
        mReaction = 0;
        mPrevEvent = 0;
        func_02091a14(this, 0);
        CopyName();
        func_0208f0f8(&mModel, RESOURCE_BCA0);
        mHeadIcon.SetMode(1);
        SetHeldItemsShowState(HeldItem::SHOW_HIDE);
        ClearSpeechTexts();
        func_02091fa0(this);
        mSoundArmed = FALSE;
        func_0202aecc(data_021223e4, mSlot);
        ArchiveManager_Unmount(&gArchiveManager, mArchiveName);
        break;
    case OVERRIDE_LOAD_ARCHIVE:
        if (mOverrideStep == 0) {
            mOverrideStep++;
            SetState(0);
            func_0208fefc(this);
            func_0209107c(this);
            ArchiveManager_MountCompressed(&gArchiveManager, mArchiveName, mArchivePath, 0);
        } else if (mOverrideStep == 1) {
            if (ArchiveManager_IsIdle(&gArchiveManager)) {
                SetOverrideState(OVERRIDE_NONE);
                func_02091a24(this);
                mPersonality = func_02098f2c(this);
                RefreshSpeechTexts();
            }
        }
        break;
    case OVERRIDE_MAKE_MODEL:
        if (mOverrideStep == 0) {
            func_0202ad04(data_021223e4, mSlot, mArchiveName, 0);
            func_0208ef5c(&mModel, mSlot, 0);
            func_02091a88(this);
            ResetBody();
            mUnknown25D4 = 0;
            mUnknown25D6 = 0;
            mUnknown25D8 = 0;
            mHeadPitch = 0;
            mHeadYaw = 0;
            mHeadRoll = 0;
            func_02091a14(this, 0);
            SetOverrideState(OVERRIDE_NONE);
        }
        break;
    }
}

// @0x02169a98
void RoomCharacter::UpdateVoiceOfSound() {
    int volume = (func_02098ec4(this) * 0x333) >> 12;
    int pitch = func_02098ec4(this) * 4 - 60;
    if (pitch < 0) {
        pitch = 0;
    }
    gSoundManager.SetSeqArcVolume(mSoundHandle, volume + 90);
    gSoundManager.SetSeqArcTrackPitch(mSoundHandle, 0xFFFF, pitch);
}

// @0x02169b00
void RoomCharacter::StartSound3IfAnimOfGroup() {
    if (mSoundArmed) {
        if (IsAnimOfGroupA(mModel.mAnimId) || IsAnimOfGroupB(mModel.mAnimId)) {
            gSoundManager.StartSeqArc(1, 0x1E, 3);
        }
    } else {
        mSoundArmed = TRUE;
    }
}

// @0x02169b68
void RoomCharacter::StartSound0UnlessGroupA() {
    if (mSoundArmed) {
        if (!IsAnimOfGroupA(mModel.mAnimId)) {
            gSoundManager.StartSeqArc(1, 0x1E, 0);
        }
    } else {
        mSoundArmed = TRUE;
    }
}

// @0x02169bb4
void RoomCharacter::StartSoundUnlessGroupB(int index) {
    if (mSoundArmed) {
        if (!IsAnimOfGroupB(mModel.mAnimId)) {
            gSoundManager.StartSeqArc(1, 0x1E, index);
        }
    } else {
        mSoundArmed = TRUE;
    }
}

// @0x02169c04
void RoomCharacter::StartSound0UnlessGroup() {
    if (mSoundArmed) {
        if (!IsAnimOfGroupB(mModel.mAnimId) && !IsAnimOfGroupA(mModel.mAnimId)) {
            gSoundManager.StartSeqArc(1, 0x1E, 0);
        }
    } else {
        mSoundArmed = TRUE;
    }
}

// @0x02169c6c
void RoomCharacter::RefreshSpeechTexts() {
    ConvertSpeechTexts();
}

// @0x02169c78
void RoomCharacter::ConvertSpeechTexts() {
    wchar_t text[0x80];
    if (mSpeechDefault[0] == L'\0') {
        MI_CpuFill8(mSpeechTextDefault, 0, 4);
    } else {
        func_020372d0(text, mSpeechDefault, sizeof(mSpeechTextDefault));
        STD_ConvertStringUnicodeToSjis(mSpeechTextDefault, NULL, (const u16 *)text, NULL, NULL);
    }
    for (int i = 0; i < 4; i++) {
        const wchar_t *source = mSpeechByMood[i];
        char *dest = mSpeechTextByMood[i];
        if (source[0] == L'\0') {
            MI_CpuFill8(dest, 0, 4);
        } else {
            func_020372d0(text, source, sizeof(mSpeechTextByMood[i]));
            STD_ConvertStringUnicodeToSjis(dest, NULL, (const u16 *)text, NULL, NULL);
        }
    }
}

// @0x02169d5c
void RoomCharacter::ClearSpeechTexts() {
    MI_CpuFill8(mSpeechTextDefault, 0, sizeof(mSpeechTextDefault));
    MI_CpuFill8(mSpeechTextByMood, 0, sizeof(mSpeechTextByMood));
}

// @0x02169d90
void RoomCharacter::Save() {
    u16 index = mIndex;
    if (func_0202c3dc(&gGameData, index)) {
        func_0202c28c(&gGameData, index, this);
    }
}

// --- HeldItem ---

// By kind: whether the model has a texture pattern animation, and the digits of its archive
static const u8 sHeldItemHasPattern[12] = { FALSE, TRUE, FALSE, TRUE, FALSE, FALSE, FALSE, FALSE, TRUE, FALSE, FALSE };
static const char *sHeldItemDigits[11] = { "00", "01", "02", "03", "04", "05", "06", "07", "08", "09", "10" };

// @0x02169dcc
HeldItem::HeldItem() {
    SetShowState(SHOW_IDLE);
    SetLoadState(LOAD_IDLE);
    STD_CopyString(mArchivePath, "Mdl/Goods/gd00_3D_LZ.bin");
    mKind = 0;
}

// @0x02169e4c
void HeldItem::SetArchive(const char *name, int kind) {
    STD_CopyString(mArchiveName, name);
    mKind = kind;
    MI_CpuCopy8(sHeldItemDigits[kind & 0xFF], &mArchivePath[12], 2);
}

// @0x02169e84
void HeldItem::Init(int resourceId) {
    mResourceId = resourceId;
    AnimModel::Load(resourceId, 0);
    StartPattern();
    mVisible = FALSE;
}

// @0x02169eac
void HeldItem::SetShowState(int state) {
    mShowState = state;
    mShowStep = 0;
}

// @0x02169ebc
void HeldItem::Update() {
    UpdateLoad();
    if (mLoadState == LOAD_IDLE) {
        UpdateShow();
    }
}

// @0x02169ee0
void HeldItem::Draw() {
    if (mVisible) {
        AnimModel::Draw();
    }
}

// @0x02169ef8
void HeldItem::UpdateShow() {
    switch (mShowState) {
    case SHOW_IDLE:
        break;
    case SHOW_HIDE:
        SetShowState(SHOW_IDLE);
        mVisible = FALSE;
        break;
    case SHOW_SHOW:
        SetShowState(SHOW_IDLE);
        mVisible = TRUE;
        break;
    }
}

// @0x02169f48
void HeldItem::SetJoint(const MtxFx43 *joint, const MtxFx33 *rotation) {
    VEC_Set(&mPosition, joint->_30, joint->_31, joint->_32);
    MI_Copy36B(rotation, &mRotation);
}

// @0x02169f78
void HeldItem::SetJointOffset(const MtxFx43 *joint, const MtxFx33 *rotation, fx32 x, fx32 y, fx32 z) {
    SetJoint(joint, rotation);
    VecFx32 offset;
    offset.x = x;
    offset.y = y;
    offset.z = z;
    MTX_MultVec33(&offset, rotation, &offset);
    AddPosition(&offset);
}

// @0x02169fc8
void HeldItem::SetLoadState(int state) {
    mLoadState = state;
    mLoadStep = 0;
}

// @0x02169fd8
void HeldItem::SetKind(int kind) {
    if (mKind != kind || mLoadState != LOAD_IDLE) {
        mKind = kind;
        SetLoadState(LOAD_CHANGE);
    } else {
        if (sHeldItemHasPattern[mKind]) {
            RestartAnims();
        }
        mScale.x = FX32_ONE;
        mScale.y = FX32_ONE;
        mScale.z = FX32_ONE;
    }
}

// @0x0216a034
void HeldItem::UpdateLoad() {
    switch (mLoadState) {
    case LOAD_IDLE:
        break;
    case LOAD_UNLOAD:
        SetLoadState(LOAD_IDLE);
        Unload();
        break;
    case LOAD_LOAD_ARCHIVE:
        if (mLoadStep == 0) {
            mLoadStep++;
            MI_CpuCopy8(sHeldItemDigits[mKind], &mArchivePath[12], 2);
            ArchiveManager_MountCompressed(&gArchiveManager, mArchiveName, mArchivePath, 0);
        } else if (mLoadStep == 1) {
            if (ArchiveManager_IsIdle(&gArchiveManager)) {
                SetLoadState(LOAD_IDLE);
            }
        }
        break;
    case LOAD_RELOAD:
        SetLoadState(LOAD_IDLE);
        Reload();
        StartPattern();
        break;
    case LOAD_CHANGE:
        if (mLoadStep == 0) {
            mLoadStep++;
            Unload();
        } else if (mLoadStep == 1) {
            if (ArchiveManager_IsIdle(&gArchiveManager)) {
                mLoadStep++;
                MI_CpuCopy8(sHeldItemDigits[mKind], &mArchivePath[12], 2);
                ArchiveManager_MountCompressed(&gArchiveManager, mArchiveName, mArchivePath, 0);
            }
        } else if (mLoadStep == 2) {
            if (ArchiveManager_IsIdle(&gArchiveManager)) {
                Reload();
                StartPattern();
                SetLoadState(LOAD_IDLE);
            }
        }
        break;
    }
}

// @0x0216a1ac
void HeldItem::Unload() {
    SetShowState(SHOW_IDLE);
    mVisible = FALSE;
    RemoveAnims();
    func_0202aecc(data_021223e4, mResourceId);
    ArchiveManager_Unmount(&gArchiveManager, mArchiveName);
}

// @0x0216a1f0
void HeldItem::Reload() {
    func_0202ad04(data_021223e4, mResourceId, mArchiveName, 0);
    AnimModel::Load(mResourceId, 0);
    mVisible = FALSE;
}

// @0x0216a22c
void HeldItem::StartPattern() {
    BOOL hasPattern = sHeldItemHasPattern[mKind];
    if (hasPattern) {
        func_0202ad04(data_021223e4, mResourceId, mArchiveName, hasPattern);
        SetAnimation(RESOURCE_BTP0, 0);
    }
}

// --- ApartmentRoom ---

// @0x0216a278
ApartmentRoom::ApartmentRoom() {
    SetState(STATE_IDLE);
    func_020a4ab8(&mParts, 3);
    MI_CpuClear16(mNumberText, sizeof(mNumberText));
}

// @0x0216a2c4
void ApartmentRoom::Refresh() {
    UpdateNumberText();
}

// @0x0216a2d0
void ApartmentRoom::SetNextStyle(u16 style) {
    mNextStyle = style;
}

// @0x0216a2dc
void ApartmentRoom::SetStyle(u16 style) {
    mNextStyle = style;
    mStyle = style;
}

// @0x0216a2ec
void ApartmentRoom::UpdateNumberText() {
    int room = (mRoomIndex & 7) + 1;
    int floor = (mRoomIndex >> 3) + 1;
    func_0203673c(floor * 100 + room, mNumberText, sizeof(mNumberText), 4, FALSE);
}

// @0x0216a330
void ApartmentRoom::Update() {
    UpdateState();
    if (mState == STATE_IDLE) {
        func_020a5960(this);
    }
}

// @0x0216a354
void ApartmentRoom::SetState(int state) {
    mState = state;
    mStep = 0;
}

// @0x0216a368
void ApartmentRoom::UpdateState() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_RESET:
        SetState(STATE_IDLE);
        func_020a4ab8(&mParts, 0);
        func_020a4144(&mParts, 0);
        func_020a4b74(&mParts, 0);
        func_020a5948(this);
        UpdateNumberText();
        break;
    case STATE_CHANGE_STYLE:
        if (mStep == 0) {
            mStep++;
            mStyle = mNextStyle;
            func_020a54e4(this, 0);
        } else if (mStep == 1) {
            if (ArchiveManager_IsIdle(&gArchiveManager)) {
                mStep++;
                func_020a56dc(this, 0);
            }
        } else if (mStep == 2) {
            if (ArchiveManager_IsIdle(&gArchiveManager)) {
                SetState(STATE_IDLE);
            }
        }
        break;
    case STATE_UNKNOWN3:
        if (mStep == 0) {
            mStep++;
            func_020a5530(this, 7);
        } else if (mStep == 1) {
            SetState(STATE_IDLE);
            func_020a5854(this, 8);
        }
        break;
    }
}

// --- RoomCamera ---

// @0x0216a4a4
RoomCamera::RoomCamera() {
    mCamera = &data_02122764;
}

// @0x0216a4b4
RoomCamera::~RoomCamera() {
    VEC_Set(&data_0213ba74.mPosition, mCamera->mPosition.x, mCamera->mPosition.y, mCamera->mPosition.z);
    VEC_Set(&data_0213ba74.mTarget, mCamera->mTarget.x, mCamera->mTarget.y, mCamera->mTarget.z);
    data_0213ba74.SetOrbit(mOrbit.x >> FX32_SHIFT, mOrbit.y >> FX32_SHIFT, mOrbit.z >> FX32_SHIFT);
}

// @0x0216a514
void RoomCamera::Reset() {
    SetMode(MODE_NONE);
    SetState(STATE_TURN_RIGHT, 0);
    mSnap = TRUE;
    mIgnoreInput = FALSE;
    func_0202a640(mCamera);
    mPosition.x = 0;
    mPosition.y = FX32_CONST(1);
    mPosition.z = FX32_CONST(16);
    mTarget.x = 0;
    mTarget.y = FX32_CONST(1.5);
    mTarget.z = 0;
    mOrbit.x = 0x1200000;
    mOrbit.y = 0x1600000;
    mOrbit.z = 0;
    VecFx32 zero = { 0, 0, 0 };
    mInput = zero;
    mOrbitSpeed = mInput;
    ApplyToCamera();
    mCamera->SetAngles(0, 0, 0);
}

// @0x0216a618
void RoomCamera::SetMode(int mode) {
    mMode = mode;
    mUnknown5C = mFollowFrames = 0;
}

// @0x0216a62c
void RoomCamera::SetState(int state, s16 waitFrames) {
    mState = state;
    mWaitFrames = waitFrames;
}

// Goes to the wanted position and target: at once after a turn, otherwise a part of the way each frame.
// Written out in each of the four places: as an inline function its temporaries come in another order on the stack.
#define MOVE_TO(position, target)                                  \
    if (mSnap) {                                                   \
        mSnap = FALSE;                                             \
        mFollowFrames = 10000;                                     \
    } else {                                                       \
        target = Approach(target, mTarget, 5);                     \
        if (mFollowFrames < 80) {                                  \
            mFollowFrames++;                                       \
            position = Approach(position, mPosition, 5);           \
        } else {                                                   \
            position = Approach(position, mPosition, 4);           \
        }                                                          \
    }                                                              \
    mPosition = position;                                          \
    mTarget = target;                                              \
    ApplyToCamera();

// Not matching yet: 16 instructions of 792 differ, all in the two modes that follow one character (MODE_CHARACTER,
// MODE_CHARACTER_CLOSE): the copy of the character's position is on the stack where the original has it only as
// far as "center = GetPos()" goes; the original then reads it in the order x, z, y and stores radius and target in
// another order. Everything else (both switches, the four MOVE_TO blocks, the stack frame) is identical. Tried:
// center as an initialized local, as a reference to the temporary, as the result of an inline function.
// @0x0216a638
void RoomCamera::Update() {
    switch (mState) {
    case STATE_NONE:
        break;
    case STATE_TURN_RIGHT:
    case STATE_TURN_LEFT:
        if (mState == STATE_TURN_RIGHT) {
            mOrbit.y += (MATH_Rand16(&gRandom, 7) + 1) * 0x2000000;
        } else {
            mOrbit.y -= MATH_Rand16(&gRandom, 2) * 0x2400000;
        }
        SetState(STATE_TURNED, 0);
        mSnap = TRUE;
        VEC_Set(&mInput, 0, 0, 0);
        VEC_Set(&mOrbitSpeed, mInput.x, mInput.y, mInput.z);
        mOrbit.x = (MATH_Rand16(&gRandom, 7) + 4) * 0x200000 + 0xA00000;
        if (mOrbit.x > 0x3000000) {
            mOrbit.x = 0x3000000;
        }
        func_020a4ab8(&mRoom->mParts, mRoom->mParts.mState);
        break;
    case STATE_TURNED:
        SetState(STATE_WAIT, 1140);
        break;
    case STATE_WAIT:
        mWaitFrames--;
        if (mWaitFrames <= 0) {
            if (!mCharacters[0]->mPresent && !mCharacters[1]->mPresent) {
                SetState(STATE_WAIT, 100);
            } else {
                SetState(STATE_TURN_RIGHT, 0);
            }
        }
        break;
    }

    switch (mMode) {
    case MODE_NONE:
        break;
    case MODE_ROOM: {
        UpdateOrbit();
        VecFx32 target;
        target.x = 0;
        target.y = 0x1E66;
        target.z = 0;
        VecFx32 radius;
        radius.x = FX32_CONST(19);
        radius.y = FX32_CONST(9);
        radius.z = FX32_CONST(19);
        VecFx32 position = GetOrbitPosition(radius, Shift(target, 3));
        MOVE_TO(position, target);
        break;
    }
    case MODE_CHARACTER: {
        UpdateOrbit();
        VecFx32 center;
        center = GetPresentCharacter()->GetPos();
        VecFx32 target;
        VecFx32 radius;
        radius.x = center.x;
        radius.y = center.y;
        radius.z = center.z;
        target.x = center.x >> 1;
        target.z = center.z >> 1;
        target.y = 0x1999;
        Clamp(&target.x, 0x3800);
        Clamp(&target.z, 0x3800);
        func_02028e0c(&radius, &radius);
        radius.x = (radius.x * 9 >> 2) + 0xA800;
        radius.z = (radius.z * 9 >> 2) + 0xA800;
        radius.y = 0x8000;
        if (radius.x > 0x12000) {
            radius.x = 0x12000;
        }
        if (radius.z > 0x12000) {
            radius.z = 0x12000;
        }
        VecFx32 position = GetOrbitPosition(radius, Shift(target, 4));
        MOVE_TO(position, target);
        break;
    }
    case MODE_BOTH: {
        UpdateOrbit();
        VecFx32 target;
        VEC_Add(&mCharacters[0]->mModel.mPos, &mCharacters[1]->mModel.mPos, &target);
        target.x >>= 1;
        target.z >>= 1;
        target.y = 0x1C00;
        Clamp(&target.x, 0x3800);
        Clamp(&target.z, 0x3800);
        VecFx32 radius;
        VEC_Subtract(&mCharacters[0]->mModel.mPos, &mCharacters[1]->mModel.mPos, &radius);
        func_02028e0c(&radius, &radius);
        radius.x = (radius.x * 9 >> 2) + 0x9000;
        radius.z = (radius.z * 9 >> 2) + 0x9000;
        radius.y = 0x8000;
        if (radius.x > 0x12000) {
            radius.x = 0x12000;
        }
        if (radius.z > 0x12000) {
            radius.z = 0x12000;
        }
        VecFx32 position = GetOrbitPosition(radius, Shift(target, 4));
        MOVE_TO(position, target);
        break;
    }
    case MODE_CHARACTER_CLOSE: {
        VecFx32 center;
        center = GetPresentCharacter()->GetPos();
        VecFx32 target;
        VecFx32 radius;
        target.x = center.x;
        target.z = center.z;
        radius.x = center.x;
        radius.y = center.y;
        radius.z = center.z;
        target.y = 0x1999;
        Clamp(&target.x, 0x3800);
        Clamp(&target.z, 0x3800);
        func_02028e0c(&radius, &radius);
        radius.x = (radius.x * 9 >> 2) + 0xA800;
        radius.z = (radius.z * 9 >> 2) + 0xA800;
        radius.y = 0x8000;
        if (radius.x > 0x12000) {
            radius.x = 0x12000;
        }
        if (radius.z > 0x12000) {
            radius.z = 0x12000;
        }
        VecFx32 position = GetOrbitPosition(radius, Shift(target, 3));
        MOVE_TO(position, target);
        break;
    }
    }

    if (mState == STATE_TURNED) {
        func_0202a748(mCamera);
    }
}

// @0x0216b298
void RoomCamera::UpdateOrbit() {
    if ((mInput.x > 0 && mOrbit.x > 0x3000000) || (mInput.x < 0 && mOrbit.x < 0xA00000)) {
        mInput.x = 0;
    }
    if (mIgnoreInput) {
        mInput.x = 0;
        mInput.y = 0;
        mInput.z = 0;
    }
    ApplyInput(&mOrbitSpeed.x, mInput.x, 0x4000, 0x40000);
    ApplyInput(&mOrbitSpeed.y, mInput.y, 0x7000, 0x100000);
    VEC_Add(&mOrbit, &mOrbitSpeed, &mOrbit);
    mInput.x = 0;
    mInput.y = 0;
    mInput.z = 0;
}

// @0x0216b34c
void RoomCamera::ApplyInput(fx32 *speed, fx32 input, fx32 friction, fx32 max) {
    *speed += input;
    if (*speed > 0) {
        *speed -= friction;
    } else if (*speed < 0) {
        *speed += friction;
    }
    if (*speed > max) {
        *speed = max;
    } else if (*speed < -max) {
        *speed = -max;
    } else if (MATH_ABS(*speed) <= (friction + 1) >> 1) {
        *speed = 0;
    }
}

// @0x0216b3b0
VecFx32 RoomCamera::GetOrbitPosition(VecFx32 radius, VecFx32 center) {
    u16 elevation = mOrbit.x >> FX32_SHIFT;
    u16 direction = mOrbit.y >> FX32_SHIFT;
    VecFx32 position;
    position.x = center.x + FX_Mul(radius.z, FX_SinIdx(direction));
    position.z = center.z + FX_Mul(radius.x, FX_CosIdx(direction));
    position.y = FX_Mul(radius.y, FX_SinIdx(elevation));
    return position;
}

// @0x0216b488
VecFx32 RoomCamera::Approach(VecFx32 to, VecFx32 from, int shift) {
    VecFx32 step;
    VEC_Subtract(&to, &from, &step);
    step = Shift(step, shift);
    VEC_Add(&step, &from, &step);
    return step;
}

// @0x0216b524
void RoomCamera::Clamp(fx32 *value, fx32 limit) {
    if (*value > limit) {
        *value = limit;
    } else if (*value < -limit) {
        *value = -limit;
    }
}

// --- RoomLight ---

// @0x0216b544
RoomLight::RoomLight() {
    SetState(STATE_IDLE);
}

// @0x0216b55c
void RoomLight::Init(RoomCamera *camera, ApartmentRoom *room) {
    mCamera = camera;
    mRoom = room;
    for (int i = 0; i < 2; i++) {
        mLights[i] = &data_021227cc[i];
        func_0202a96c(mLights[i]);
        mLights[i]->mColor = data_020f72f8[mRoom->mStyle].mLightColor;
    }
}

// @0x0216b5bc
void RoomLight::Update() {
    switch (mState) {
    case STATE_SET_STYLE_COLOR:
        SetColor(data_020f72f8[mRoom->mStyle].mLightColor);
        SetState(STATE_IDLE);
        break;
    case STATE_FADE_STYLE_COLOR:
        if (mStep == 0) {
            mStep++;
            mFade = 0;
        } else {
            int targetR, targetG, targetB;
            int r, g, b;
            u16 color;
            mFade += 0x80;
            func_02028b98(data_020f72f8[mRoom->mStyle].mLightColor, &targetR, &targetG, &targetB);
            BlendColor(&targetR, &targetG, &targetB, &r, &g, &b);
            if (r >= targetR || g >= targetG || b >= targetB) {
                color = data_020f72f8[mRoom->mStyle].mLightColor;
                SetState(STATE_IDLE);
            } else {
                color = r | (g << 5) | (b << 10);
            }
            SetColor(color);
        }
        break;
    case STATE_SET_ROOM_COLOR:
        SetColor(func_020a4d90(mRoom));
        SetState(STATE_IDLE);
        break;
    case STATE_FADE_ROOM_COLOR:
        if (mStep == 0) {
            mStep++;
            mFade = 0;
        } else {
            int targetR, targetG, targetB;
            int r, g, b;
            u16 color;
            mFade += 0x40;
            func_020a4dcc(mRoom, &targetR, &targetG, &targetB);
            BlendColor(&targetR, &targetG, &targetB, &r, &g, &b);
            if (r <= targetR || g <= targetG || b <= targetB) {
                color = targetR | (targetG << 5) | (targetB << 10);
                SetState(STATE_IDLE);
            } else {
                color = r | (g << 5) | (b << 10);
            }
            SetColor(color);
        }
        break;
    }
    func_0202ab2c(mLights[0], mCamera->mOrbit.x >> 12, mCamera->mOrbit.y >> 12, mCamera->mOrbit.z >> 12, 1600,
                  -600, -4000);
    func_0202ab2c(mLights[1], mCamera->mOrbit.x >> 12, mCamera->mOrbit.y >> 12, mCamera->mOrbit.z >> 12, 0,
                  -4000, 1500);
}

// @0x0216b8a8
void RoomLight::SetState(int state) {
    mState = state;
    mStep = 0;
}

// @0x0216b8b8
void RoomLight::SetColor(u16 color) {
    for (int i = 0; i < 2; i++) {
        mLights[i]->mColor = color;
    }
}

// @0x0216b8d4
void RoomLight::BlendColor(int *targetR, int *targetG, int *targetB, int *r, int *g, int *b) {
    func_02028b98(mLights[0]->mColor, r, g, b);
    fx32 weight = FX_Mul(mFade, mFade);
    *r = *r + ((weight * (*targetR - *r)) >> FX32_SHIFT);
    *g = *g + ((weight * (*targetG - *g)) >> FX32_SHIFT);
    *b = *b + ((weight * (*targetB - *b)) >> FX32_SHIFT);
}

// --- RoomCursor ---

// The rectangles around the cursor and the button that take a touch
static const ScreenRect sCursorTouchRect = { -16, -12, 34, 26 };
static const ScreenRect sButtonTouchRect = { -22, -18, 46, 38 };

// @0x0216b970
RoomCursor::RoomCursor() {
    SetState(STATE_IDLE);
}

// @0x0216b988
void RoomCursor::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_MOVE:
        SetState(STATE_IDLE);
        mPosition.x = mTarget.x;
        mPosition.y = mTarget.y;
        break;
    }
}

// @0x0216b9c0
void RoomCursor::SetState(int state) {
    mState = state;
}

// @0x0216b9c8
BOOL RoomCursor::Contains(ScreenPoint point) {
    ScreenRect rect = sCursorTouchRect;
    ScreenPoint position = GetPixelPosition();
    rect.x += position.x;
    rect.y += position.y;
    return func_02028d88(&rect, &point) != 0;
}

// --- BuildingButton ---

// @0x0216ba4c
BuildingButton::BuildingButton() {
    SetState(STATE_IDLE);
}

// @0x0216ba64
void BuildingButton::Init() {
    Sprite_Init(this, 0, 9, 219, 0, TRUE);
}

// @0x0216ba90
void BuildingButton::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_PRESS:
        SetState(STATE_IDLE);
        Sprite_SetSequence(this, 10, TRUE);
        mAnim.animCtrl.bActive = FALSE;
        break;
    case STATE_RELEASE:
        SetState(STATE_IDLE);
        Sprite_SetSequence(this, 9, TRUE);
        mAnim.animCtrl.bActive = FALSE;
        break;
    case STATE_PLAY:
        SetState(STATE_IDLE);
        Sprite_SetPlaying(this, TRUE);
        break;
    }
}

// @0x0216bb18
void BuildingButton::SetState(int state) {
    mState = state;
}

// @0x0216bb20
BOOL BuildingButton::Contains(ScreenPoint point) {
    ScreenRect rect = sButtonTouchRect;
    ScreenPoint position = GetPixelPosition();
    rect.x += position.x;
    rect.y += position.y;
    return func_02028d88(&rect, &point) != 0;
}

// --- ApartmentFront ---

// How many rooms a building of each size has, and how far the view scrolls per floor
static const u8 sRoomCounts[4] = { 24, 48, 72, 100 };
static const fx32 sScrollPerFloor[4] = { 0x16000, 0x16000, 0x19000, 0x1C000 };

// @0x0216bba4
ApartmentFront::ApartmentFront() {
    SetState(STATE_IDLE);
    mScrollTarget = 0;
    mUnknownClearedByConstructor = 0;
    mScroll = 0;
    mSelectedRoom = 0;
    mBuildingSize = 0;
    STD_CopyString(mArchivePath, "Scn/AFwBG/Apart0_LZ.bin");
    MI_CpuFill8(mResidents, 0xFF, sizeof(mResidents));
}

// @0x0216bc08
void ApartmentFront::SetBuildingSize(int size) {
    mBuildingSize = size;
    mArchivePath[15] = size + '0';
}

// @0x0216bc18
void ApartmentFront::Init(int room) {
    for (u8 i = 0; i < sRoomCounts[mBuildingSize]; i++) {
        DrawWindow(i);
    }
    mScreen = (u16 *)(func_020253d4(gGraphicsManager, 0x200) + HW_DB_BG_VRAM);
    mSelectedRoom = room;
    int column;
    int floor;
    GetRoomCell(room, &column, &floor);
    mScrollTarget = floor * sScrollPerFloor[mBuildingSize];
    ClampScrollTarget();
    mScroll = mScrollTarget;
    UpdateCursorTarget();
    ScreenPoint target = mCursor.GetPixelTarget();
    Sprite_Init(&mCursor, 0, 8, target.x, target.y, TRUE);
    mButton.Init();
    UpdateButtonPosition();
}

// @0x0216bd2c
void ApartmentFront::Touch(ScreenPoint point) {
    SelectRoom(GetRoomAt(point), point.y);
}

// @0x0216bd60
void ApartmentFront::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_SCROLL: {
        fx32 target = mScrollTarget;
        fx32 step = (target - mScroll) >> 4;
        if (MATH_ABS(step) >= 0xF5) {
            mScroll += step;
        } else {
            SetState(STATE_IDLE);
            mScroll = target;
        }
        UpdateCursorTarget();
        mCursor.SetState(RoomCursor::STATE_MOVE);
        break;
    }
    }
    int y = 320 - (mScroll >> FX32_SHIFT);
    func_02028154(&data_02121834, 0x200, y);
    func_02028154(&data_02121834, 0x400, y);
    mCursor.Update();
    mButton.Update();
    UpdateButtonPosition();
}

// @0x0216be1c
void ApartmentFront::Draw() {
    Sprite_Draw(&mCursor);
    Sprite_Draw(&mButton);
}

// @0x0216be34
void ApartmentFront::SetState(int state) {
    mState = state;
}

// @0x0216be3c
void ApartmentFront::SelectRoom(int room, int touchY) {
    int column;
    int floor;
    GetRoomCell(room, &column, &floor);
    if (mBuildingSize > 3) {
        return;
    }
    if (room < 0) {
        SetState(STATE_SCROLL);
    } else if (room >= sRoomCounts[mBuildingSize]) {
        SetState(STATE_SCROLL);
    } else if (mSelectedRoom != room) {
        mSelectedRoom = room;
        SetState(STATE_SCROLL);
        gSoundManager.StopSeqArc(2, 0);
        switch (GetWindowKind(mSelectedRoom)) {
        case WINDOW_0:
        case WINDOW_1:
        case WINDOW_7:
            gSoundManager.StartSeqArc(2, 9, 3);
            break;
        case WINDOW_2:
            gSoundManager.StartSeqArc(2, 9, 2);
            break;
        case WINDOW_3:
        case WINDOW_5:
        case WINDOW_6:
            gSoundManager.StartSeqArc(2, 9, 4);
            break;
        case WINDOW_EMPTY:
            gSoundManager.StartSeqArc(2, 9, 1);
            break;
        }
    }
    if (touchY >= 40 && touchY <= 144) {
        mScrollTarget = mScroll;
    } else {
        mScrollTarget = floor * sScrollPerFloor[mBuildingSize];
    }
    ClampScrollTarget();
}

// @0x0216bfb8
int ApartmentFront::GetRoomAt(ScreenPoint point) {
    int x = point.x;
    int y = 191 - point.y;
    int floor = (y - 72 + (mScroll >> FX32_SHIFT)) >> 5;
    int column = x >> 5;
    s16 room = column + floor * 8;
    if (floor == 12) {
        if (column <= 1) {
            return room + 4;
        }
        if (column <= 5) {
            return room - 2;
        }
    }
    return room;
}

// @0x0216c014
void ApartmentFront::GetRoomCell(int room, int *column, int *floor) {
    *column = room & 7;
    *floor = room >> 3;
    if (*floor == 12) {
        if (*column <= 3) {
            *column += 2;
        } else if (*column <= 5) {
            *column -= 4;
        }
    }
}

// @0x0216c050
void ApartmentFront::ClampScrollTarget() {
    fx32 max = sScrollPerFloor[mBuildingSize] * (mBuildingSize * 3 + 2);
    if (mScrollTarget < 0) {
        mScrollTarget = 0;
    } else if (mScrollTarget > max) {
        mScrollTarget = max;
    }
}

// Not matching yet: same instructions, another order (11 of 25 differ): the original loads mScroll before the
// column, and stores each of the two locals as soon as it is computed. Tried: plain statements, the scroll in a
// local, compound assignments, the arithmetic in an inline function taking pointers (CellToPosition, kept).
// @0x0216c08c
void ApartmentFront::UpdateCursorTarget() {
    int x;
    int y;
    GetRoomCell(mSelectedRoom, &x, &y);
    CellToPosition(&x, &y, mScroll >> FX32_SHIFT);
    mCursor.SetTarget(x << FX32_SHIFT, y << FX32_SHIFT);
}

// @0x0216c0f0
void ApartmentFront::UpdateButtonPosition() {
    mButton.mPosition.y = ((mScroll >> FX32_SHIFT) + 151) << FX32_SHIFT;
}

// @0x0216c108
int ApartmentFront::GetWindowKind(int room) {
    u8 character = mResidents[room];
    if (character == 0xFF) {
        return WINDOW_EMPTY;
    }
    int unknown1 = func_0202c468(&gGameData, character);
    u8 unknown2 = func_0202c53c(&gGameData, character);
    int unknown3 = func_0202c5b4(&gGameData, character);
    int unknown4 = data_02114d78[unknown3][func_0202c58c(&gGameData, character)].mUnknown0;
    if (func_02099c94(unknown1)) {
        return WINDOW_2;
    }
    if (unknown1 == 18) {
        return WINDOW_1;
    }
    if (unknown1 == 17) {
        return WINDOW_7;
    }
    if (unknown2 == 1 || unknown2 == 2) {
        if (unknown3 == 3 && (unknown4 == 5 || unknown4 == 6)) {
            return WINDOW_5;
        }
        if (unknown3 == 4 && unknown4 == 11) {
            return WINDOW_6;
        }
        return WINDOW_3;
    }
    return WINDOW_0;
}

// @0x0216c1f0
void ApartmentFront::DrawWindow(int room) {
    DrawWindowOfKind(GetWindowKind(room), room);
}

// @0x0216c214
void ApartmentFront::DrawWindowOfKind(int kind, int room) {
    int column;
    int floor;
    GetRoomCell(room, &column, &floor);
    DrawWindowTiles(kind, column, floor);
}

// @0x0216c258
void ApartmentFront::DrawWindowTiles(int kind, u8 column, u8 floor) {
    u16 *screen = mScreen + (column * 4 + 0x660 - floor * 0x80);
    u16 tile;
    int i;
    int palette;
    switch (kind) {
    case WINDOW_0:
        palette = 0x0000;
        tile = 1;
        break;
    case WINDOW_1:
    case WINDOW_2:
        palette = 0x1000;
        tile = 5;
        break;
    case WINDOW_3:
        palette = 0x2000;
        tile = 9;
        break;
    case WINDOW_EMPTY:
        palette = 0x3000;
        tile = 13;
        break;
    case WINDOW_5:
        palette = 0x2000;
        tile = 17;
        break;
    case WINDOW_6:
        palette = 0x2000;
        tile = 21;
        break;
    case WINDOW_7:
        palette = 0x0000;
        tile = 25;
        break;
    default:
        return;
    }
    for (i = 0; i < 4; i++) {
        screen[0] = palette + ((screen[0] & 0xC00) + tile);
        screen[1] = palette + ((screen[1] & 0xC00) + (tile + 1));
        screen[2] = palette + ((screen[2] & 0xC00) + (tile + 2));
        screen[3] = palette + ((screen[3] & 0xC00) + (tile + 3));
        tile += 0x20;
        screen += 0x20;
    }
}

// --- SkyBackground ---

// Which of the seven pictures of the sky each time of day shows
static const s8 sSkyPictures[32] = {
    0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 0,
};

// @0x0216c374
SkyBackground::SkyBackground() {
    STD_CopyString(mArchivePath, "Scn/AFwBG/A0FwFBG0_LZ.bin");
    STD_CopyString(mCharPath, "ASX:/Fw_BG0.ncg");
    STD_CopyString(mPalettePath, "ASX:/Fw_BG0.ncl");
    SetState(STATE_WATCH);
    mPalette = NULL;
    mColors = NULL;
}

// @0x0216c3cc
void SkyBackground::Init(Clock *clock, int buildingSize) {
    mClock = clock;
    func_020a612c(this, &clock->mDate, &clock->mTime);
    mBuildingSize = buildingSize;
    mArchivePath[11] = buildingSize + '0';
    UpdatePaths();
}

// @0x0216c404
void SkyBackground::Refresh() {
    ApplyPalette();
}

// @0x0216c410
void SkyBackground::Update() {
    func_020a61cc(this, &mClock->mDate, &mClock->mTime);
    UpdateState();
}

// @0x0216c434
void SkyBackground::SetState(int state) {
    mState = state;
    mStep = 0;
}

// @0x0216c444
void SkyBackground::UpdateState() {
    switch (mState) {
    case STATE_WATCH:
        if (mChanged != 0) {
            SetState(STATE_RELOAD);
        }
        break;
    case STATE_RELOAD:
        if (mStep == 0) {
            if (ArchiveManager_IsIdle(&gArchiveManager)) {
                mStep++;
                UpdatePaths();
                ArchiveManager_Unmount(&gArchiveManager, "ASX");
                ArchiveManager_MountCompressed(&gArchiveManager, "ASX", mArchivePath, 0);
            }
        } else if (mStep == 1) {
            if (ArchiveManager_IsIdle(&gArchiveManager)) {
                SetState(STATE_WATCH);
                func_02028f24(&gVramTransferQueue, mCharPath, 0x400, 0, 0x1000, 0x3000, 0);
                mPalette = func_02027534(&data_021213b8, mPalettePath, 0x400, 0x80, 0x80, 0xC0);
                ApplyPalette();
            }
        }
        break;
    }
}

// @0x0216c584
void SkyBackground::UpdatePaths() {
    s8 digit = sSkyPictures[func_020a6214(this) & 0x1F] + '0';
    mArchivePath[17] = digit;
    mCharPath[10] = digit;
    mPalettePath[10] = digit;
}

// @0x0216c5bc
void SkyBackground::ApplyPalette() {
    func_020278f8(&data_021213b8, mColors[func_020a6214(this)], 0x100);
}

// --- NamePanel ---

// @0x0216c5e8
NamePanel::NamePanel() {
    SetState(STATE_IDLE);
}

// Not matching yet: 17 of 99 instructions differ, between the second InitObjForMessage and the first DrawAll: the
// original computes the second text's character count with other registers and after loading the position's
// constants, and orders the dead stores of SetPosition differently. The calls and their arguments are identical.
// Tried: SetPosition on the sprite and on the canvas, the constants in locals, the count written out.
// @0x0216c618
int NamePanel::Init(int charOffset, MessageFile *file1, MessageFile *file2) {
    Sprite_Init(this, 1, 4, 128, 96, FALSE);
    mText1.InitObjForMessage(file1, 1, &gFonts[0].mFont, 0x10, charOffset, 0, 1, 0, 0, 15, NULL);
    charOffset += mText1.GetCharCount();
    mText2.InitObjForMessage(file2, 2, &gFonts[0].mFont, 0x10, charOffset, 0, 1, 0, 0, 15, NULL);
    int count = mText2.GetCharCount();
    mText1.SetPosition(FX32_CONST(38), FX32_CONST(8));
    mText1.DrawAll();
    mText2.SetPosition(FX32_CONST(113), FX32_CONST(8));
    mText2.DrawAll();
    SetState(STATE_SHOWN);
    return charOffset + count;
}

// @0x0216c7a4
void NamePanel::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_SHOW_BOTH:
    case STATE_SHOW_FIRST:
        if (mStep == 0) {
            mStep++;
            SetTextVisible(FALSE);
        } else if (mStep == 1) {
            mStep++;
            NNS_G2dCharCanvasClear(&mText1.mCanvas, 0);
            mText1.DrawAll();
        } else {
            NNS_G2dCharCanvasClear(&mText2.mCanvas, 0);
            SetVisible(TRUE);
            if (mState == STATE_SHOW_FIRST) {
                mText2.mSprite.mVisible = FALSE;
            } else {
                mText2.DrawAll();
            }
            SetState(STATE_SHOWN);
        }
        break;
    case STATE_SHOWN:
        if (mStep == 0) {
            mStep++;
            mFrames = 180;
        } else {
            mFrames--;
            if (mFrames <= 0) {
                SetState(STATE_IDLE);
                SetVisible(FALSE);
            }
        }
        break;
    case STATE_HIDE:
        SetVisible(FALSE);
        break;
    }
}

// @0x0216c8f0
void NamePanel::Draw() {
    mText1.Draw();
    mText2.Draw();
    Sprite_Draw(this);
}

// @0x0216c918
void NamePanel::SetState(int state) {
    mState = state;
    mStep = 0;
}

// @0x0216c92c
void NamePanel::SetNames(const wchar_t *name1, const wchar_t *name2) {
    mText1.SetStringArg(name1, 0);
    mText2.SetStringArg(name2, 0);
}

// @0x0216c95c
void NamePanel::SetTextVisible(BOOL visible) {
    mText1.mSprite.mVisible = visible;
    mText2.mSprite.mVisible = visible;
}

// @0x0216c968
void NamePanel::SetVisible(BOOL visible) {
    SetTextVisible(visible);
    mVisible = visible;
}

// --- TouchSprite, TouchButton ---

struct ButtonPosition {
    int x;
    int y;
};

struct ButtonSequences {
    u16 mNormal;
    u16 mPressed;
};

// By kind of button
static const ButtonSequences sButtonSequences[3] = { { 2, 3 }, { 0, 1 }, { 0, 0 } };
static const ButtonPosition sButtonPositions[2] = { { 128, 177 }, { 242, 13 } };
static const ScreenRect sButtonRect0 = { -34, -12, 70, 26 };
static const ScreenRect sButtonRect1 = { -16, -16, 34, 34 };
static const ScreenRect sButtonRectOther = { -32, -24, 66, 56 };

// @0x0216c980
TouchSprite::TouchSprite() {
    mEnabled = TRUE;
    mReleased = TRUE;
}

// @0x0216c990
BOOL TouchSprite::Contains(ScreenPoint point) {
    if (!mEnabled) {
        return FALSE;
    }
    ScreenRect rect = *mTouchRect;
    rect.x += mX;
    rect.y += mY;
    return func_02028d88(&rect, &point) != 0;
}

// @0x0216ca14
BOOL TouchSprite::SetReleased(BOOL released) {
    BOOL changed = mReleased != released;
    mReleased = released;
    return changed;
}

// @0x0216ca30
TouchButton::TouchButton() {
    mLabel = NULL;
}

// @0x0216ca4c
void TouchButton::Init(int kind) {
    mKind = kind;
    mX = sButtonPositions[kind].x;
    mY = sButtonPositions[kind].y;
    Sprite_Init(this, 0, sButtonSequences[kind].mNormal, mX, mY, FALSE);
    mState = STATE_IDLE;
    if (kind == 0) {
        mTouchRect = &sButtonRect0;
        MessageText *label = mLabel;
        label->SetPositionFromVec((mX - label->mCanvas.areaWidth * 4) << FX32_SHIFT,
                                   (mY - label->mCanvas.areaHeight * 4) << FX32_SHIFT);
    } else if (kind == 1) {
        mTouchRect = &sButtonRect1;
    } else {
        mTouchRect = &sButtonRectOther;
    }
}

// @0x0216cb34
void TouchButton::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_PRESS:
        mState = STATE_IDLE;
        Sprite_SetSequence(this, sButtonSequences[mKind].mPressed, TRUE);
        mAnim.animCtrl.bActive = FALSE;
        if (mLabel != NULL) {
            mLabel->mSprite.mPosition.y = (mY - mLabel->mCanvas.areaHeight * 4 + 2) << FX32_SHIFT;
        }
        break;
    case STATE_RELEASE:
        mState = STATE_IDLE;
        Sprite_SetSequence(this, sButtonSequences[mKind].mNormal, TRUE);
        mAnim.animCtrl.bActive = FALSE;
        if (mLabel != NULL) {
            mLabel->mSprite.mPosition.y = (mY - mLabel->mCanvas.areaHeight * 4) << FX32_SHIFT;
        }
        break;
    case STATE_PLAY:
        mState = STATE_IDLE;
        Sprite_SetPlaying(this, TRUE);
        break;
    }
}

// @0x0216cc2c
void TouchButton::Draw() {
    Sprite_Draw(this);
}

// @0x0216cc38
BOOL TouchButton::Contains(ScreenPoint point) {
    if (!mVisible) {
        return FALSE;
    }
    return TouchSprite::Contains(point);
}

// --- Clock ---

// @0x0216cc70
Clock::Clock() {
    MI_CpuClear32(&mDate, sizeof(mDate));
    MI_CpuClear32(&mTime, sizeof(mTime));
    mSeconds = 0;
}

// @0x0216ccac
void Clock::Update() {
    mSeconds = func_02035870(data_02123440[0], &mDate, &mTime);
}

// @0x0216ccd4
void Clock::UpdateIfArchiveIdle() {
    if (ArchiveManager_IsIdle(&gArchiveManager)) {
        mSeconds = func_02035870(data_02123440[0], &mDate, &mTime);
    }
}

// --- ClockDisplay ---

// The animation sequence and the position of each sprite
static const u16 sClockSequences[ClockDisplay::SPRITE_COUNT] = { 0, 0, 2, 0, 0, 3, 0, 0, 1, 0, 0 };
static const ButtonPosition sClockPositions[ClockDisplay::SPRITE_COUNT] = {
    { 160, 8 }, { 167, 8 }, { 174, 8 }, { 181, 8 }, { 188, 8 }, { 205, 8 },
    { 222, 8 }, { 229, 8 }, { 236, 8 }, { 243, 8 }, { 250, 8 },
};

// @0x0216cd10
void ClockDisplay::Init(Clock *clock) {
    mClock = clock;
    SetState(STATE_HIDE);
    for (int i = 0; i < SPRITE_COUNT; i++) {
        Sprite *sprite = &mSprites[i];
        Sprite_Init(sprite, 1, sClockSequences[i], 128, 96, TRUE);
        mSprites[i].SetPositionFromVec(sClockPositions[i].x << FX32_SHIFT, sClockPositions[i].y << FX32_SHIFT);
        sprite->mAnim.animCtrl.bActive = FALSE;
    }
    Refresh(TRUE);
}

// @0x0216cdc8
void ClockDisplay::Update() {
    switch (mState) {
    case STATE_SHOWN:
        break;
    case STATE_HIDE:
        SetState(STATE_HIDDEN);
        SetVisible(FALSE);
        break;
    case STATE_HIDDEN:
        mHiddenFrames++;
        if (mHiddenFrames > 190) {
            SetState(STATE_SHOWN);
            SetVisible(TRUE);
        }
        break;
    }
    Refresh(FALSE);
}

// Not matching yet: 2 instructions are swapped in the hour's part ("add r0, r0, #0x400" for &mSprites[7] comes
// after the subtraction in the original, before it here). Everything else is identical.
// @0x0216ce48
void ClockDisplay::Refresh(BOOL all) {
    Clock *clock = mClock;
    if (mMonth != clock->mDate.month || all) {
        u32 tens = clock->mDate.month / 10;
        Sprite_SetFrame(&mSprites[SPRITE_MONTH_TENS], tens);
        Sprite_SetFrame(&mSprites[SPRITE_MONTH_ONES], clock->mDate.month - tens * 10);
        mMonth = clock->mDate.month;
    }
    if (mDay != clock->mDate.day || all) {
        u32 tens = clock->mDate.day / 10;
        Sprite_SetFrame(&mSprites[SPRITE_DAY_TENS], tens);
        Sprite_SetFrame(&mSprites[SPRITE_DAY_ONES], clock->mDate.day - tens * 10);
        mDay = clock->mDate.day;
    }
    if (mHour != clock->mTime.hour || all) {
        u32 half = clock->mTime.hour / 12;
        Sprite_SetFrame(&mSprites[SPRITE_HALF_OF_DAY], half);
        u32 hour = clock->mTime.hour - half * 12;
        u32 tens = hour / 10;
        Sprite_SetFrame(&mSprites[SPRITE_HOUR_TENS], tens);
        Sprite_SetFrame(&mSprites[SPRITE_HOUR_ONES], hour - tens * 10);
        mHour = clock->mTime.hour;
    }
    if (mMinute != clock->mTime.minute || all) {
        u32 tens = clock->mTime.minute / 10;
        Sprite_SetFrame(&mSprites[SPRITE_MINUTE_TENS], tens);
        Sprite_SetFrame(&mSprites[SPRITE_MINUTE_ONES], clock->mTime.minute - tens * 10);
        mMinute = clock->mTime.minute;
    }
}

// @0x0216cfc0
void ClockDisplay::Draw() {
    for (int i = 0; i < SPRITE_COUNT; i++) {
        Sprite_Draw(&mSprites[i]);
    }
}

// @0x0216cfe8
void ClockDisplay::SetState(int state) {
    mState = state;
    mHiddenFrames = 0;
}

// @0x0216cffc
void ClockDisplay::SetVisible(BOOL visible) {
    for (int i = 0; i < SPRITE_COUNT; i++) {
        mSprites[i].mVisible = visible;
    }
}

// --- TimedMessage ---

// @0x0216d018
TimedMessage::TimedMessage() {
    SetState(STATE_IDLE);
    mFrames = 0;
}

// @0x0216d070
void TimedMessage::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_SHOW:
        NNS_G2dCharCanvasClear(&mCanvas, 0);
        mDrawMode = 1;
        mStarted = FALSE;
        SetVisible(TRUE);
        mFrames = 180;
        SetState(STATE_WAIT);
        break;
    case STATE_SHOW_BY_LINE:
        if (mStep == 0) {
            mStep++;
            NNS_G2dCharCanvasClear(&mCanvas, 0);
            mDrawMode = 3;
            mStarted = FALSE;
        } else if (mDrawMode == 0) {
            SetVisible(TRUE);
            mFrames = 180;
            SetState(STATE_WAIT);
        }
        break;
    case STATE_WAIT:
        if (mFrames > 0) {
            mFrames--;
        } else {
            SetState(STATE_HIDE);
        }
        break;
    case STATE_HIDE:
        SetState(STATE_IDLE);
        SetVisible(FALSE);
        break;
    }
    UpdateDraw();
}

// @0x0216d1a8
void TimedMessage::SetState(int state) {
    mState = state;
    mStep = 0;
}

// @0x0216d1bc
void TimedMessage::SetVisible(BOOL visible) {
    mSprite.mVisible = visible;
    if (visible == TRUE) {
        data_0211d604.Show(2);
    } else {
        data_0211d604.Hide(2);
    }
}

// --- Emitted by the compiler ---

// HeldItem's destructor is implicit. The copy that only destroys is the main program's func_0203ef44; the
// deleting one is here.
// @0x0216d1fc:D0

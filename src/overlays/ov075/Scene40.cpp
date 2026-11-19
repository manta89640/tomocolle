#include "Scene40.h"
#include "MainProgram.h"
#include "../../scene/MainProgram3d.h"
#include "GameData.h"
#include "SceneManager.h"
#include "SceneMemory.h"
#include "ScreenControl.h"
#include "sound/SoundManager.h"
#include <wchar.h>
#include "save/SaveBlocks.h"
#include "Scene40Pool.h"
#include "text/TextUtil.h"
#include "Scene40Context.h"
#include "Scene40SpeechWindow.h"
#include "Scene40Camera.h"


extern "C" {
void ParticleSystem_Draw(ParticleSystem *particles, int unknown); // Draws the particles
void GameData_WriteCharacter(GameData *game, int index, SavedCharacter *character); // Writes a saved character
void DestroyMainFrameHeap(void);
BOOL ArchiveManager_IsIdle(ArchiveLoader *loader);
extern int gGameClock[];
s64 Clock_GetDateTime(int offset, RTCDate *date, RTCTime *time); // The date and time now, and the same in seconds

BOOL SpriteButton_IsTouched(SpriteButton *button);                // Whether the stylus came down on it
void SpriteButton_SetDisabled(SpriteButton *button, BOOL disabled);
BOOL func_02031d00(SpriteButton *button);                         // Whether it is selected (mUnknownB0)

// Not decompiled yet
}

// What this scene keeps in the main program's gSceneScratch (an array of bytes there) for the scenes it goes to
struct Scene40Scratch {
    /* 0x00 */ union {
        u16 mShopItem;
        struct { u8 mUnknown00; u8 mUnknown01; };
    };
    /* 0x02 */ u8 mUnknown02[2];
    /* 0x04 */ BOOL mMenuAvailable[5]; // The menu's first five buttons, as Scene40MenuBar::Refresh takes them back
    /* 0x18 */ u8 mUnknown18[0x58];
};

// 0x70

extern Scene40Scratch gSceneScratch;

// Overlay 75: scenes 0x40 and 0x6C, the scene of touching a Mii ("Scn/Scene/SceneMiiTouch_LZ.bin"). One source
// file by its one static initialiser. Most of it is not decompiled yet.

// An archive's name, as ArchiveManager_MountCompressed takes it: three characters
struct ArchiveName {
    char mName[4];
};

#define sItemArchiveName SCENE40_POOL(ArchiveName, 0x14) // TEMP pool: "IM?", the item models' archives with a digit

// The saved game's flags and counters that the scene changes (defined at their place, 0x02150540)
static void SetGameFlags(u32 flags, BOOL set);

static u32 GetGameFlags(u32 flags);

static void AddTroublesSolved(int count);

static void ProjectTouch(const ScreenPoint *point, VecFx32 *out, const VecFx32 *planePoint);

static void ProjectTouchBetween(const ScreenPoint *point, VecFx32 *out, fx32 targetWeight, fx32 eyeWeight);

// The scenes after which the visit starts with the pages' entry 23 (CameFromSceneOfTables)
static const int sReturnScenesA[3] = {89, 59, 124};

static const int sReturnScenesB[9] = {130, 131, 132, 133, 134, 135, 183, 185, 184};

static const Scene40SceneEntry sReturnScenesC[7] = {{83, 0xA01}, {117, 0xA01}, {122, 0x501}, {137, 0x501}, {138, 0xA01}, {146, 0xA01}, {147, 0xA03}};

// The planes each change of layout fades, by the new layout and the old
static const int sFadePlanes[4][4] = {{0, 16191, 16191, 16191}, {16191, 0, 16128, 16128}, {16191, 16128, 0, 16128}, {16191, 16128, 16128, 0}};

// The scenes after which the visit starts with the pages' entry 24, and what the touch page does for each
static const Scene40PresentEntry sPresentScenes[20] = {
    {24, 2, 0, 9, 0, 1, {0, 0}},
    {28, 2, 0, 166, 0, 1, {0, 0}},
    {43, 2, 1, 270, 0, 1, {0, 0}},
    {50, 4, 0, 218, 0, 1, {0, 0}},
    {54, 1, 1, 280, 0, 1, {0, 0}},
    {55, 3, 0, 169, 0, 1, {0, 0}},
    {58, 1, 0, 35, 0, 1, {0, 0}},
    {61, 2, 1, 104, 0, 1, {0, 0}},
    {76, 2, 0, 167, 0, 1, {0, 0}},
    {77, 2, 0, 151, 0, 1, {0, 0}},
    {80, 1, 0, 0, 1, 1, {0, 0}},
    {82, 1, 0, 164, 0, 1, {0, 0}},
    {118, 1, 0, 125, 0, 1, {0, 0}},
    {123, 3, 0, 220, 0, 1, {0, 0}},
    {136, 1, 0, 219, 0, 1, {0, 0}},
    {139, 1, 0, 189, 0, 1, {0, 0}},
    {141, 1, 0, 153, 0, 1, {0, 0}},
    {143, 5, 1, 0, 1, 1, {0, 0}},
    {144, 5, 1, 0, 1, 1, {0, 0}},
    {145, 2, 0, 0, 1, 1, {0, 0}},
};

// The scene's two message windows' rectangles
static const ScreenRect sWindowRect0 = {20, 20, 216, 152};

static const ScreenRect sWindowArea0 = {20, 20, 216, 152};

static const ScreenRect sWindowRect1 = {20, 20, 216, 152};

static const ScreenRect sWindowArea1 = {20, 20, 216, 152};

#define sWindowButtonPosition0 SCENE40_POOL(WindowPoint, 0xA4) // TEMP pool

#define sWindowButtonSize0 SCENE40_POOL(WindowPoint, 0x18C)    // TEMP pool

#define sWindowTextPosition0 SCENE40_POOL(WindowPoint, 0x9C)   // TEMP pool

#define sWindowButtonPosition1 SCENE40_POOL(WindowPoint, 0x1B4) // TEMP pool

#define sWindowButtonSize1 SCENE40_POOL(WindowPoint, 0x84)     // TEMP pool

#define sWindowTextPosition1 SCENE40_POOL(WindowPoint, 0x3C)   // TEMP pool

// The chains of messages of the two windows (Scene40MessageSequence)
static const Scene40MessageSequence::Entry sMessageEntries[21] = {
    {0, 0, 103, 60, 0, 0, 21},
    {1, 0, 106, 30, 30, 1, 21},
    {2, 0, 111, 60, 30, 0, 21},
    {3, 0, 119, 30, 30, 1, 21},
    {4, 0, 124, 60, 0, 0, 21},
    {5, 0, 125, 60, 0, 1, 21},
    {6, 0, 126, 60, 0, 0, 7},
    {7, 0, 127, 0, 0, 0, 8},
    {8, 0, 128, 0, 30, 0, 21},
    {9, 0, 133, 45, 0, 0, 21},
    {10, 0, 155, 45, 0, 1, 21},
    {11, 0, 177, 45, 0, 0, 21},
    {12, 0, 180, 45, 0, 0, 21},
    {13, 0, 134, 30, 30, 1, 21},
    {14, 1, 135, 45, 0, 0, 21},
    {15, 1, 136, 0, 0, 0, 21},
    {16, 1, 173, 0, 0, 0, 21},
    {17, 1, 174, 0, 0, 0, 21},
    {18, 1, 179, 0, 0, 0, 21},
    {19, 0, 181, 30, 0, 0, 21},
    {20, 0, 182, 30, 0, 0, 21},
};

// A trouble (CharacterRecord::mTroubleCategory and mTroubleId index data_02114d78), as src/character/SavedCharacter.cpp
// has it
struct TroubleInfo {
    u32 kind : 8;
    u32 minutes : 16;         // How long it lasts at most
    u32 weight : 8;           // Its weight in the random choice (ChooseTrouble)
    u32 unk4Bits0 : 8;
    u32 mood : 8;             // The mood it puts the character in (SetTroubleMood), 0 to 4
    u32 cooldownMinutes : 12; // Added to mTroubleEnd once it is over
    u32 hasTarget : 4;        // Not 0: about a character or an item, which StartTrouble chooses (mTroubleTarget)
    u32 flags;                // Food troubles: the kinds of food; clothes troubles: the kinds of clothes
}; // 0xC
extern "C" const TroubleInfo *const data_02114d78[];

// The trouble the character has
static inline const TroubleInfo *GetTrouble(SavedCharacter *character) {
    return &data_02114d78[character->mTroubleCategory][character->mTroubleId];
}

static inline BOOL IsInTroubleOfKind(SavedCharacter *character, int kind) {
    if (character->IsInTrouble() && GetTrouble(character)->kind == kind) {
        return TRUE;
    }
    return FALSE;
}

// Sets a vector field by field, from a copy
static inline void SetVecFields(VecFx32 *dst, VecFx32 src) {
    dst->x = src.x;
    dst->y = src.y;
    dst->z = src.z;
}

static inline const ItemInfo *GetGoodsInfo(u16 id) {
    return &gGoodsTable[id];
}

static inline const ClothesInfo *GetClothesInfo(u16 id) {
    return &gClothesTable[id];
}


// --- The scene -----------------------------------------------------------------------------------------------------

// The character to visit, its room, and the archives of the scene; after the apartment (scene 0x11) the residents'
// day goes on first, as the island's scenes do it
void Scene40::StartLoading() {
    RTCDate date;
    RTCTime time;
    s64 now = Clock_GetDateTime(gGameClock[0], &date, &time);
    if (gSceneManager.mPreviousSceneId == 0x11 && mSceneId != 0x6C) {
        LimitEventTimes(now);
        UpdateResidents(now, &time);
        StartRandomEvents(now);
        ChooseTouchTalkers(&date, &time);
    }

    int index = GetVisitedIndex();
    GameData_ReadCharacter(&gGameData, index, &mCharacter);
    mCharacter.mIndex = index;
    if (gSceneManager.mPreviousSceneId == 0xB0) {
        mCharacter.CharacterModel::SetArchive("3HM", 0x91, MiiData_GetSex(&mCharacter));
    } else {
        mCharacter.Character::SetArchive("3HM");
    }
    if ((int)gGameData.mUnknownB2[0] < (int)mCharacter.mSatisfyLevel) {
        gGameData.mUnknownB2[0] = mCharacter.mSatisfyLevel;
    }

    mRoom.mId = mCharacter.mInterior;
    mRoom.Mount("CO0", 0);
    mRoom.MountAll("RM?", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TCM", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TTM", "Msg/TmdTroubleMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TIM", "Msg/TmdItemMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TDM", "Msg/TmdCostumeMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TRM", "Msg/TmdRoomMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TEM", "Msg/TmdMiitouchMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3PM", "Mdl/Misc/Tch/mii_tch_presM_3D_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3HB", "Mdl/Misc/Tch/mii_tch_balloon_3D_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3MH", "Mdl/Misc/headManikin_3D_LZ.bin", 0);
    if (mCharacter.mLongActState == 18) {
        ArchiveManager_MountCompressed(&gArchiveManager, "3HT", "Mdl/Goods/gd10_3D_LZ.bin", 0);
    } else {
        ArchiveManager_MountCompressed(&gArchiveManager, "3HT", "Mdl/Misc/Tch/mii_tch_curtain_3D_LZ.bin", 0);
    }
    ArchiveManager_MountCompressed(&gArchiveManager, "EFT", "Mdl/Spl/SplMiiTouchEffect_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "KBD", "Cmn/Kbd/SoftKeyboard_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "SMT", "Scn/Scene/SceneMiiTouch_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "LTL", "Scn/List/List_TouchList_LZ.bin", 0);

    mCharacter.mPersonality = mCharacter.GetPersonality();
    switch (gPersonalityKinds[mCharacter.mPersonality]) {
    case 0:
        ArchiveManager_MountCompressed(&gArchiveManager, "TLM", "Msg/TmdTalkCoolMsgJP_LZ.bin", 0);
        ArchiveManager_MountCompressed(&gArchiveManager, "3HA", "Mdl/Ica/ica_mii_mii_touch_cool_LZ.bin", 0);
        break;
    case 1:
        ArchiveManager_MountCompressed(&gArchiveManager, "TLM", "Msg/TmdTalkDryMsgJP_LZ.bin", 0);
        ArchiveManager_MountCompressed(&gArchiveManager, "3HA", "Mdl/Ica/ica_mii_mii_touch_dry_LZ.bin", 0);
        break;
    case 2:
        ArchiveManager_MountCompressed(&gArchiveManager, "TLM", "Msg/TmdTalkNagomiMsgJP_LZ.bin", 0);
        ArchiveManager_MountCompressed(&gArchiveManager, "3HA", "Mdl/Ica/ica_mii_mii_touch_nagomi_LZ.bin", 0);
        break;
    case 3:
        ArchiveManager_MountCompressed(&gArchiveManager, "TLM", "Msg/TmdTalkNoriMsgJP_LZ.bin", 0);
        ArchiveManager_MountCompressed(&gArchiveManager, "3HA", "Mdl/Ica/ica_mii_mii_touch_nori_LZ.bin", 0);
        break;
    }

    // The three item models' archives: "IM0" to "IM2"
    ArchiveName name;
    ArchiveName archiveName = sItemArchiveName;
    for (int i = 0; i < 3; i++) {
        name = archiveName;
        name.mName[2] = '0' + i;
        Scene40ItemModel *model = &mItemModels[i];
        model->SetItem(name.mName, 0);
        ArchiveManager_MountCompressed(&gArchiveManager, model->mArchiveName, model->mArchivePath, 0);
    }
    mMannequin.CharacterModel::SetArchive("3MM", 0, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3MM", mMannequin.mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacter.mArchiveName, mCharacter.mArchivePath, 0);

    BOOL keepMusic = FALSE;
    if (gSceneManager.mPreviousSceneId == 0x38) {
        if (GetSceneMemory().mNextMode == 0) {
            keepMusic = TRUE;
        }
    } else if (CameFromSceneOfTable(gSceneManager.mPreviousSceneId)) {
        Scene40Scratch *scratch = &gSceneScratch;
        if (scratch->mUnknown00 == 2 && scratch->mUnknown01 == 0) {
            keepMusic = TRUE;
        }
    }
    if (keepMusic) {
        gSceneManager.SetSoundGroup(14);
        gSoundManager.StartSeqIfNotPlaying(0, 21);
        return;
    }
    int previous = gSceneManager.mPreviousSceneId;
    if (previous == 0x5A || previous == 0xB2 || previous == 0xB5 || previous == 0xB3) {
        gSoundManager.StopSeq(0, 1);
        return;
    }
    if (previous == 0x78 && mCharacter.IsInTrouble() && mCharacter.mTroubleCategory == 4) {
        gSoundManager.StopSeq(0, 1);
        return;
    }
    func_ov075_021793fc(func_ov075_02179290(&mCharacter));
}

void Scene40::Init() {
    mScreenState = LAYOUT_SMT_UP;
    mMainCharOffset = 0x100;
    mSubCharOffset = 0x190;
    mDrawState = 0;
    mDrawFrames = 0;
    gSoundManager.LoadGroup(77);
    Unused0214ca80();

    mMessageFiles[0].Open("TCM:tmd_common_msg_JP.bmg");
    mMessageFiles[1].Open("TTM:tmd_trouble_msg_JP.bmg");
    mMessageFiles[3].Open("TIM:tmd_item_msg_JP.bmg");
    mMessageFiles[4].Open("TDM:tmd_costume_msg_JP.bmg");
    mMessageFiles[5].Open("TRM:tmd_room_msg_JP.bmg");
    mMessageFiles[6].Open("TEM:tmd_miitouch_msg_JP.bmg");
    switch (gPersonalityKinds[mCharacter.mPersonality]) {
    case 0:
        mMessageFiles[2].Open("TLM:tmd_talk_cool_msg_JP.bmg");
        break;
    case 1:
        mMessageFiles[2].Open("TLM:tmd_talk_dry_msg_JP.bmg");
        break;
    case 2:
        mMessageFiles[2].Open("TLM:tmd_talk_nagomi_msg_JP.bmg");
        break;
    case 3:
        mMessageFiles[2].Open("TLM:tmd_talk_nori_msg_JP.bmg");
        break;
    }

    InitLowerScreen();
    Init3d();
    mClock.Update();
    mCharacter.SetUnknown23CFromFullness(mClock.mSeconds);
    mClothesList.Load();
    mClothesList.SortHeldFirst();
    mInventory.Load();
    mInventory.SortHeldFirst();
    mInteriors.Load();
    mInteriors.SortHeldFirst();
    mMoney = gGameData.mCappedCounter;
    mIslandProgress.Init(&mInventory, &mMoney);

    Sprite_Init(&mSprite013D0, 0, 26, 128, 96, TRUE);
    mSprite013D0.mOverwritePalette = 1;
    mSprite013D0.mVisible = FALSE;
    mTtsPlayer.InitEngine();
    mText.mMessageFile = &mMessageFiles[1];
    mMainCharOffset += mSpeechWindow.Init(mMainCharOffset, &mMessageFiles[1], 4, 16, &mTtsPlayer, TRUE);
    mSpeechWindow.mEnabled = TRUE;
    mSpeechWindow.mText[0].mHAlign = mSpeechWindow.mText[0].mVAlign = 1;
    mSpeechWindow.mText[0].mSprite.mPosition.y = FX32_CONST(159);
    mSpeechWindow.Hide();

    // Two message windows, one on each engine, each with a button and a text
    MessageWindow_Init(&mWindows[0], sWindowRect0);
    MessageWindow_SetScreen(&mWindows[0], "SMT:/Window_down.nsc", 2);
    func_020331bc(&mWindows[0], 0x3F, sWindowArea0);
    MessageWindow_AddTextButtonWithSoundIds(&mWindows[0], 0, 0, 23, 24, sWindowButtonPosition0,
                                            sWindowButtonSize0, &mMessageFiles[0], 4, &gFonts[2].mFont,
                                            16, &mMainCharOffset, 10, -1, 0, -1);
    MessageWindow_AddText(&mWindows[0], &mMessageFiles[6], 103, &gFonts[0].mFont, 16,
                          sWindowTextPosition0, &mMainCharOffset);
    MessageWindow_SetTextsVAlign(&mWindows[0], 1);
    MessageWindow_SetTextsHAlign(&mWindows[0], 1);
    MessageWindow_Init(&mWindows[1], sWindowRect1);
    MessageWindow_SetScreen(&mWindows[1], "SMT:/Window_down.nsc", 0x400);
    func_020331bc(&mWindows[1], 0x3F00, sWindowArea1);
    MessageWindow_AddTextButtonWithSoundIds(&mWindows[1], 0, 4, 2, 3, sWindowButtonPosition1,
                                            sWindowButtonSize1, &mMessageFiles[0], 4, &gFonts[2].mFont,
                                            0x1000, &mSubCharOffset, 10, -1, 0, -1);
    MessageWindow_AddText(&mWindows[1], &mMessageFiles[6], 103, &gFonts[0].mFont, 0x1000,
                          sWindowTextPosition1, &mSubCharOffset);
    MessageWindow_SetTextsVAlign(&mWindows[1], 1);
    MessageWindow_SetTextsHAlign(&mWindows[1], 1);

    mMessageSequence.Init(&mWindows[0], &mWindows[1]);
    mMessageSequence.SetCharacter(&mCharacter);
    mPhysics.Init();
    mPhysics.mCharacter = &mCharacter;
    for (int i = 0; i < 3; i++) {
        mPhysics.AddItem(&mItemModels[i]);
    }
    mTouch.Init();

    mContext.mSceneId = mSceneId;
    mContext.mClothesList = &mClothesList;
    mContext.mInventory = &mInventory;
    mContext.mInteriors = &mInteriors;
    mContext.mMoney = &mMoney;
    mContext.mCommonMessages = &mMessageFiles[0];
    mContext.mTroubleMessages = &mMessageFiles[1];
    mContext.mTalkMessages = &mMessageFiles[2];
    mContext.mItemMessages = &mMessageFiles[3];
    mContext.mCostumeMessages = &mMessageFiles[4];
    mContext.mRoomMessages = &mMessageFiles[5];
    mContext.mMiiTouchMessages = &mMessageFiles[6];
    mContext.mSprite = &mSprite013D0;
    mContext.mSpeechWindow = &mSpeechWindow;
    mContext.mTtsPlayer = &mTtsPlayer;
    mContext.mText = &mText;
    mContext.mIslandProgress = &mIslandProgress;
    mContext.mClock = &mClock;
    mContext.mTouch = &mTouch;
    mContext.mCamera = &mCamera;
    mContext.mLights = &mLights;
    mContext.mPhysics = &mPhysics;
    mContext.mCharacter = &mCharacter;
    mContext.mItemModels = mItemModels;
    mContext.mMannequin = &mMannequin;
    mContext.mRoom = &mRoom;
    mContext.mHeadIcon = &mHeadIcon;
    mContext.mModels[0] = &mModels[0];
    mContext.mModels[1] = &mModels[1];
    mContext.mModels[2] = &mModels[2];
    mContext.mWindows[0] = &mWindows[0];
    mContext.mWindows[1] = &mWindows[1];
    mContext.mMessageSequence = &mMessageSequence;
    mPages.Init(&mMainCharOffset, &mSubCharOffset, &mContext);

    mCharacter.SetContext(&mContext);
    mCharacter.SetVoiceOf(&mTtsPlayer);
    func_ov075_021753c8(&mCharacter);
    mCharacter.Update(FALSE);
    Unused0214a884();
    Start();
}

// Does nothing
void Scene40::Unused0214a884() {}

// The lower screen: its sprites, background, the windows' frame and the dialogue background, and the graphics
// state LoadLayout restores; and the upper screen's sprite cells
void Scene40::InitLowerScreen() {
    ObjResourceManager_LoadCell(&gObjResourceManager, "SMT:/Obj_down.nce", 16, 0);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "SMT:/Obj_down.ncg", 16, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "SMT:/Obj_down.ncl", 16, -1, -1, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "SMT:/BG_down.ncl", 47, -1, -1, -1);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "SMT:/BG_down.ncg", 6, 0, 0, 0, -1);
    ScreenLoader_LoadFile(&gScreenLoader, "SMT:/Window_down.nsc", 2, 0, 0, 0, 0, -1);
    ScreenLoader_LoadFile(&gScreenLoader, "SMT:/BG_dialogue.nsc", 4, 0, 0, 0, 0, -1);
    ScreenLoader_SetPriority(&gScreenLoader, 4, 0);
    ScreenLoader_SetPriority(&gScreenLoader, 2, 1);
    ScreenLoader_SetPriority(&gScreenLoader, 1, 3);
    ScreenControl_SetBlendAlpha(&gScreenControl, 4, 17, 8, 8);
    GraphicsManager_SaveState(gGraphicsManager, mSavedGraphics, 0x3F00);
    ObjResourceManager_LoadCell(&gObjResourceManager, "SMT:/Obj_up.nce", 0x1000, 1);
    ObjResourceManager_LoadCell(&gObjResourceManager, "SMT:/Obj_present.nce", 0x1000, 4);
    ObjResourceManager_LoadCell(&gObjResourceManager, "LTL:/Obj_Touch_list.nce", 0x1000, 2);
}

// Every frame: everything the scene holds, then the change of the upper screen's layout that the pages asked for:
// fade out, load, fade in
void Scene40::Update() {
    mClock.UpdateIfArchiveIdle();
    mCamera.Update();
    mLights.Update();
    mPhysics.Update();
    func_0207b138(mParticles.mManager);
    mHeadIcon.Update();
    mCharacter.Update(TRUE);
    mRoom.Update();
    MessageWindow_Update(&mWindows[0]);
    MessageWindow_Update(&mWindows[1]);
    mMessageSequence.Update();
    mTouch.Update();
    mPages.Update();
    mTtsPlayer.Update();

    switch (mScreenState) {
    case LAYOUT_SMT_UP:
    case LAYOUT_LIST:
    case LAYOUT_KEYBOARD:
    case LAYOUT_SMT_DOWN:
        if (mPages.mRequestedLayout == 0) {
            break;
        }
        mRequestedLayout = mPages.mRequestedLayout;
        mBrightness = 0;
        mLayout = mScreenState;
        mScreenState = SCREEN_FADE_OUT;
        mSpeechWindow.mEnabled = FALSE;
        mPages.SetActive(FALSE);
        break;
    case SCREEN_FADE_OUT:
        mBrightness += 2;
        mBrightness = MATH_CLAMP(mBrightness, 0, 16);
        mNextLayout = GetLayout(mRequestedLayout);
        ScreenControl_SetBlendBrightness(&gScreenControl, sFadePlanes[mNextLayout][mLayout], mBrightness);
        if (mBrightness >= 16) {
            mScreenState = SCREEN_LOAD;
        }
        break;
    case SCREEN_LOAD:
        LoadLayout(mRequestedLayout);
        mBrightness = 16;
        mScreenState = SCREEN_FADE_IN;
        mPages.mWaitingForLayout = FALSE;
        break;
    case SCREEN_FADE_IN:
        mBrightness -= 2;
        mBrightness = MATH_CLAMP(mBrightness, 0, 16);
        ScreenControl_SetBlendBrightness(&gScreenControl, sFadePlanes[mNextLayout][mLayout], mBrightness);
        if (mBrightness <= 0) {
            ScreenControl_SetBlendAlpha(&gScreenControl, 4, 17, 8, 8);
            mScreenState = mNextLayout;
            mSpeechWindow.mEnabled = TRUE;
            mPages.SetActive(TRUE);
        }
        break;
    }
    mSpeechWindow.Update();
}

// The layout that LoadLayout's request shows
int Scene40::GetLayout(int request) {
    int layout = LAYOUT_SMT_UP;
    switch (request) {
    case 0:
    case 1:
        layout = LAYOUT_SMT_UP;
        break;
    case 2:
        layout = LAYOUT_LIST;
        break;
    case 3:
        layout = LAYOUT_KEYBOARD;
        break;
    case 4:
        layout = LAYOUT_SMT_DOWN;
        break;
    }
    return layout;
}

// Loads the upper screen's backgrounds and sprites of a layout (request 1 to 4: LAYOUT_SMT_UP to LAYOUT_SMT_DOWN)
// over the state InitLowerScreen saved, and returns the layout
int Scene40::LoadLayout(int request) {
    int layout = LAYOUT_SMT_UP;
    switch (request) {
    case 0:
        break;
    case 1:
        GraphicsManager_RestoreState(gGraphicsManager, mSavedGraphics, 0x3F00);
        ScreenLoader_SetScrollX(&gScreenLoader, 0x100, 0);
        PaletteLoader_LoadFile(&gPaletteLoader, "SMT:/BG_up.ncl", 0x2F00, -1, -1, -1);
        VramTransferQueue_LoadFile(&gVramTransferQueue, "SMT:/BG_up.ncg", 0x2F00, 0, 0, 0, -1);
        ScreenLoader_LoadFile(&gScreenLoader, "SMT:/BG_up.nsc", 0x100, 0, 0, 0, 0, -1);
        gScreenControl.Set(0x1111);
        ScreenLoader_SetPriority(&gScreenLoader, 0x100, 3);
        ScreenLoader_SetScrollX(&gScreenLoader, 0x2F00, 0);
        ScreenLoader_SetScrollY(&gScreenLoader, 0x2F00, 0);
        PaletteLoader_SetBackdrop(&gPaletteLoader, 0, 0x101);
        ObjResourceManager_LoadCell(&gObjResourceManager, "SMT:/Obj_up.nce", 0x1000, 1);
        VramTransferQueue_LoadFile(&gVramTransferQueue, "SMT:/Obj_up.ncg", 0x1000, 0, 0, 0, -1);
        PaletteLoader_LoadFile(&gPaletteLoader, "SMT:/Obj_up.ncl", 0x1000, -1, -1, -1);
        gScreenControl.mUnknown44 = 0;
        gScreenControl.mUnknown44Changed = TRUE;
        layout = LAYOUT_SMT_UP;
        break;
    case 2:
        GraphicsManager_RestoreState(gGraphicsManager, mSavedGraphics, 0x3F00);
        ScreenLoader_SetScrollX(&gScreenLoader, 0x100, 0);
        PaletteLoader_LoadFile(&gPaletteLoader, "LTL:/BG_list.ncl", 0x2F00, -1, -1, -1);
        VramTransferQueue_LoadFile(&gVramTransferQueue, "LTL:/BG_list.ncg", 0x2F00, 0, 0, 0, -1);
        ScreenLoader_LoadFile(&gScreenLoader, "LTL:/BG_list.nsc", 0x100, 0, 0, 0, 0, -1);
        switch (mPages.mListFrame) {
        case 0:
            ScreenLoader_LoadFile(&gScreenLoader, "LTL:/BG_Touch_frame.nsc", 0x200, 0, 0, 0, 0, -1);
            break;
        case 1:
            ScreenLoader_LoadFile(&gScreenLoader, "LTL:/BG_Touchtxt_frame.nsc", 0x200, 0, 0, 0, 0, -1);
            break;
        }
        ScreenLoader_SetPriority(&gScreenLoader, 0x100, 3);
        ScreenLoader_SetPriority(&gScreenLoader, 0x200, 2);
        ScreenLoader_SetScrollX(&gScreenLoader, 0x2F00, 0);
        ScreenLoader_SetScrollY(&gScreenLoader, 0x2F00, 0);
        gScreenControl.Set(0x1311);
        PaletteLoader_LoadFile(&gPaletteLoader, "LTL:/Obj_list.ncl", 0x1000, -1, -1, -1);
        VramTransferQueue_LoadFile(&gVramTransferQueue, "LTL:/Obj_list.ncg", 0x1000, 0, 0, 0, -1);
        ObjResourceManager_LoadCell(&gObjResourceManager, "LTL:/Obj_Touch_list.nce", 0x1000, 2);
        gScreenControl.mUnknown44 = 1;
        gScreenControl.mUnknown44Changed = TRUE;
        layout = LAYOUT_LIST;
        break;
    case 3:
        GraphicsManager_RestoreState(gGraphicsManager, mSavedGraphics, 0x3F00);
        ScreenLoader_SetScrollX(&gScreenLoader, 0x100, 0);
        PaletteLoader_LoadFile(&gPaletteLoader, "KBD:/keyboardColor.ncl", 0x2F00, -1, -1, -1);
        VramTransferQueue_LoadFile(&gVramTransferQueue, "KBD:/bg_keyboard.ncg", 0x2F00, 0, 0, 0, -1);
        switch (mPages.mKeyboardLayout) {
        case 0:
            ScreenLoader_LoadFile(&gScreenLoader, "KBD:/bg_keyboard_10.nsc", 0x200, 0, 0, 0, 0, -1);
            break;
        case 1:
            ScreenLoader_LoadFile(&gScreenLoader, "KBD:/bg_keyboard_16.nsc", 0x200, 0, 0, 0, 0, -1);
            break;
        case 2:
            ScreenLoader_LoadFile(&gScreenLoader, "KBD:/bg_keyboard_6.nsc", 0x200, 0, 0, 0, 0, -1);
            break;
        case 3:
            ScreenLoader_LoadFile(&gScreenLoader, "KBD:/bg_keyboard_8.nsc", 0x200, 0, 0, 0, 0, -1);
            break;
        case 4:
            ScreenLoader_LoadFile(&gScreenLoader, "KBD:/bg_keyboard_Q.nsc", 0x200, 0, 0, 0, 0, -1);
            break;
        }
        ScreenLoader_SetPriority(&gScreenLoader, 0x100, 2);
        ScreenLoader_SetPriority(&gScreenLoader, 0x200, 3);
        G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x3000,
                          GX_BG_CHARBASE_0x08000, GX_BG_EXTPLTT_01);
        gScreenControl.Set(0x1311);
        gScreenControl.mUnknown44 = 1;
        gScreenControl.mUnknown44Changed = TRUE;
        layout = LAYOUT_KEYBOARD;
        break;
    case 4:
        GraphicsManager_RestoreState(gGraphicsManager, mSavedGraphics, 0x3F00);
        ScreenLoader_SetScrollX(&gScreenLoader, 0x100, 0);
        PaletteLoader_LoadFile(&gPaletteLoader, "SMT:/BG_down.ncl", 0x2F00, -1, -1, -1);
        VramTransferQueue_LoadFile(&gVramTransferQueue, "SMT:/BG_down.ncg", 0x2F00, 0, 0, 0, -1);
        ScreenLoader_LoadFile(&gScreenLoader, "SMT:/BG_down.nsc", 0x100, 0, 0, 0, 0, -1);
        gScreenControl.Set(0x1111);
        ScreenLoader_SetPriority(&gScreenLoader, 0x100, 3);
        ScreenLoader_SetScrollX(&gScreenLoader, 0x2F00, 0);
        ScreenLoader_SetScrollY(&gScreenLoader, 0x2F00, 0);
        PaletteLoader_LoadFile(&gPaletteLoader, "SMT:/Obj_present.ncl", 0x1000, -1, -1, -1);
        VramTransferQueue_LoadFile(&gVramTransferQueue, "SMT:/Obj_present.ncg", 0x1000, 0, 0, 0, -1);
        ObjResourceManager_LoadCell(&gObjResourceManager, "SMT:/Obj_present.nce", 0x1000, 4);
        gScreenControl.mUnknown44 = 1;
        gScreenControl.mUnknown44Changed = TRUE;
        layout = LAYOUT_SMT_DOWN;
        break;
    }
    return layout;
}

// For the first nine frames the room and the character are updated from here as well
void Scene40::Draw() {
    MessageWindow_Draw(&mWindows[0]);
    MessageWindow_Draw(&mWindows[1]);
    mPages.Draw();
    Sprite_Draw(&mSprite013D0);

    switch (mDrawState) {
    case 0:
        mRoom.Update();
        mCharacter.Update(FALSE);
        mDrawFrames++;
        if (mDrawFrames >= 9) {
            mDrawState = 1;
        }
        break;
    case 1:
        break;
    }

    ParticleSystem_Draw(&mParticles, 0);
    mCharacter.Draw();
    mFace.Draw();
    mMannequin.Draw();
    mRoom.RoomModels::Draw();
    for (int i = 0; i < 3; i++) {
        mItemModels[i].Draw();
    }
    mSpeechWindow.Draw();
}

// NOT MATCHING (the copy of the camera's eye and target only): the original keeps the camera's address in r5 and
// loads the words in pairs before storing them (x, y; store x; z, target x; store y and z together; ...), where
// this loads and stores one at a time through r1. Whole-vector and field-by-field assignments compile alike; a
// local pointer to the camera is worse.
// Saves what the scene changed (the character, with the date of this meeting, and the lists of clothes, goods and
// interiors), keeps the camera for the next scene, and unmounts the scene's archives
void Scene40::Exit() {
    RTCDate date;
    date.year = mClock.mDate.year;
    date.month = mClock.mDate.month;
    date.day = mClock.mDate.day;
    date.week = mClock.mDate.week;
    mCharacter.SetMeetDate(&date);
    GameData_WriteCharacter(&gGameData, mCharacter.mIndex, &mCharacter);
    if ((int)gGameData.mUnknownB2[0] < (int)mCharacter.mSatisfyLevel) {
        gGameData.mUnknownB2[0] = mCharacter.mSatisfyLevel;
    }
    mClothesList.Save();
    mInventory.Save();
    mInteriors.Save();
    gGameData.SetCappedCounter(mMoney);

    gSceneMemory.mCameraPosition = gCamera.mEye;
    gSceneMemory.mCameraTarget = gCamera.mTarget;

    DestroyMainFrameHeap();
    ArchiveManager_Unmount(&gArchiveManager, "KBD");
    ArchiveManager_Unmount(&gArchiveManager, "SMT");
    ArchiveManager_Unmount(&gArchiveManager, "LTL");
    ArchiveManager_Unmount(&gArchiveManager, "TCM");
    ArchiveManager_Unmount(&gArchiveManager, "TTM");
    ArchiveManager_Unmount(&gArchiveManager, "TLM");
    ArchiveManager_Unmount(&gArchiveManager, "TIM");
    ArchiveManager_Unmount(&gArchiveManager, "TDM");
    ArchiveManager_Unmount(&gArchiveManager, "TRM");
    ArchiveManager_Unmount(&gArchiveManager, "TEM");
    ModelResources_ReleaseModel(gModelResources, 0);
    ModelResources_ReleaseModel(gModelResources, 1);
    ModelResources_ReleaseModel(gModelResources, 2);
    ModelResources_ReleaseModel(gModelResources, 4);
    ArchiveManager_Unmount(&gArchiveManager, "3HA");
    ArchiveManager_Unmount(&gArchiveManager, "3HB");
    ArchiveManager_Unmount(&gArchiveManager, "3HT");
    ArchiveManager_Unmount(&gArchiveManager, "EFT");
    ModelResources_ReleaseModel(gModelResources, 12);
    ModelResources_ReleaseModel(gModelResources, 13);
    ArchiveManager_Unmount(&gArchiveManager, "3MM");
    ArchiveManager_Unmount(&gArchiveManager, "3MH");
    ModelResources_ReleaseModel(gModelResources, 17);
    ArchiveManager_Unmount(&gArchiveManager, "3PM");
    mRoom.ReleaseAll();
    ArchiveManager_Unmount(&gArchiveManager, "3HM");
    for (int i = 0; i < 3; i++) {
        ArchiveManager_Unmount(&gArchiveManager, mItemModels[i].mArchiveName);
        ModelResources_ReleaseModel(gModelResources, i + 14);
    }
}

// The first page, by the scene the game comes from (and in the tutorial's scene, by the tutorial's step); then the
// scene runs a first frame at once
void Scene40::Start() {
    if (gSceneManager.mPreviousSceneId == 0x68) {
        mScreenState = LAYOUT_LIST;
        mPages.mListFrame = 0;
        LoadLayout(2);
        mPages.Start(2);
    } else if (gSceneManager.mPreviousSceneId == 0x1E) {
        mScreenState = LAYOUT_LIST;
        mPages.mListFrame = 0;
        LoadLayout(2);
        mPages.Start(3);
    } else if (gSceneManager.mPreviousSceneId == 0x35) {
        mScreenState = LAYOUT_LIST;
        mPages.mListFrame = 0;
        LoadLayout(2);
        mPages.Start(4);
    } else if (gSceneManager.mPreviousSceneId == 0x1A) {
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(5);
        MI_CpuFill8(&gSceneScratch, 0, 100);
    } else if (gSceneManager.mPreviousSceneId == 0x5B) {
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(1);
    } else if (gSceneManager.mPreviousSceneId == 0xAF) {
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(18);
    } else if (gSceneManager.mPreviousSceneId == 0xB2) {
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(19);
    } else if (gSceneManager.mPreviousSceneId == 0x5A) {
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(20);
    } else if (gSceneManager.mPreviousSceneId == 0xB3) {
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(21);
    } else if (gSceneManager.mPreviousSceneId == 0xB5) {
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(22);
    } else if (gSceneManager.mPreviousSceneId == 0x34 || gSceneManager.mPreviousSceneId == 0x51 ||
               gSceneManager.mPreviousSceneId == 0x31) {
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(1);
    } else if (gSceneManager.mPreviousSceneId == 0x3C) {
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(1);
    } else if (gSceneManager.mPreviousSceneId == 0xBF) {
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(11);
    } else if (gSceneManager.mPreviousSceneId == 0xB0) {
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(12);
    } else if (gSceneManager.mPreviousSceneId == 0x54) {
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(13);
    } else if (gSceneManager.mPreviousSceneId == 0xA3) {
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(14);
    } else if (gSceneManager.mPreviousSceneId == 0xAA) {
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(16);
    } else if (gSceneManager.mPreviousSceneId == 0x22) {
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(15);
    } else if ((u32)(gSceneManager.mPreviousSceneId - 0x77) <= 0x79 - 0x77) {
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(17);
    } else if (CameFromSceneOfTables(gSceneManager.mPreviousSceneId)) {
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(23);
    } else if (CameFromSceneOfPresentTable(gSceneManager.mPreviousSceneId)) {
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(24);
    } else if (gSceneManager.mPreviousSceneId == 0x8E) {
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(0);
    } else if (gSceneManager.mPreviousSceneId == 0x38) {
        Scene40Scratch *scratch = &gSceneScratch;
        switch (scratch->mUnknown00) {
        case 0:
            switch (scratch->mUnknown01) {
            case 0:
                mScreenState = LAYOUT_SMT_UP;
                LoadLayout(1);
                mPages.Start(6);
                break;
            case 1:
                mScreenState = LAYOUT_SMT_DOWN;
                LoadLayout(4);
                mPages.Start(7);
                break;
            }
            break;
        case 1:
            switch (scratch->mUnknown01) {
            case 0:
            case 1:
                mScreenState = LAYOUT_SMT_UP;
                LoadLayout(1);
                mPages.Start(8);
                break;
            }
            break;
        }
    } else if (gSceneManager.mPreviousSceneId == 0x40) {
        switch (GetSceneMemory().mNextMode) {
        case 0:
            mScreenState = LAYOUT_SMT_UP;
            LoadLayout(1);
            mPages.Start(9);
            break;
        case 1:
            mScreenState = LAYOUT_SMT_UP;
            LoadLayout(1);
            mPages.Start(10);
            break;
        }
    } else if (mSceneId == 0x6C) {
        // The tutorial's visits
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.HideMenu(TRUE);
        if (gSceneMemory.mTutorialStep == 9) {
            mCharacter.mTroubleState = 1;
            mCharacter.mTroubleCategory = 0;
            mCharacter.mTroubleId = 0;
            mCharacter.mTroubleTarget = 0;
            mCharacter.mTroubleEnd = 0x7FFFFFFFFFFFFFFF;
            mPages.Start(25);
        } else if (gSceneMemory.mTutorialStep == 12) {
            mCharacter.mTroubleState = 3;
            mPages.Start(26);
            AddTroublesSolved(1);
        } else if (gSceneMemory.mTutorialStep == 17 || gSceneMemory.mTutorialStep == 22) {
            mCharacter.mTroubleState = 1;
            mCharacter.mTroubleCategory = 0;
            mCharacter.mTroubleId = 57;
            mCharacter.mTroubleTarget = 0;
            mCharacter.mTroubleEnd = 0x7FFFFFFFFFFFFFFF;
            mPages.Start(0);
        }
    } else if (gIslandState.GetUnknown44(4) == 0 && mCharacter.mIndex == 0 &&
               !mCharacter.IsTroubleOfCategory34OrKind20()) {
        mCharacter.ClearTrouble();
        mCharacter.mTroubleCategory = 9;
        mCharacter.mTroubleId = 0;
        mCharacter.mTroubleState = 1;
        mCharacter.mTroubleEnd = 0x7FFFFFFFFFFFFFFF;
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(0);
    } else {
        mScreenState = LAYOUT_SMT_UP;
        LoadLayout(1);
        mPages.Start(0);
    }
    Update();
    Draw();
}

// Which saved character the scene visits: in the tutorial's scene the first or the second, by the step; otherwise
// the first of the scene memory's, or the first saved character when there is none
int Scene40::GetVisitedIndex() {
    u8 index;
    if (mSceneId == 0x6C) {
        switch (gSceneMemory.mTutorialStep) {
        case 9:
        case 12:
            index = 0;
            break;
        case 17:
        case 22:
            index = 1;
            break;
        }
    } else {
        index = gSceneMemory.mCharacters[0];
        if (index == 0xFF) {
            index = 0;
        } else if (index >= 100) {
            index = 0;
        } else if (!GameData_IsCharacterSlotUsed(&gGameData, index)) {
            index = 0;
        }
    }
    return index;
}

// Whether the scene the game comes from is one of a set of twelve, or of the next table's
BOOL Scene40::CameFromSceneOfTables(int sceneId) {
    BOOL found = FALSE;
    for (int i = 0; i < 3; i++) {
        if (sceneId == sReturnScenesA[i]) {
            found = TRUE;
        }
    }
    for (int i = 0; i < 9; i++) {
        if (sceneId == sReturnScenesB[i]) {
            found = TRUE;
        }
    }
    return found | CameFromSceneOfTable(sceneId);
}

BOOL Scene40::CameFromSceneOfTable(int sceneId) {
    BOOL found = FALSE;
    for (int i = 0; i < 7; i++) {
        if (sceneId == sReturnScenesC[i].mSceneId) {
            found = TRUE;
        }
    }
    return found;
}

BOOL Scene40::CameFromSceneOfPresentTable(int sceneId) {
    BOOL found = FALSE;
    for (int i = 0; i < 20; i++) {
        if (sPresentScenes[i].mUnknown9 == 1 && sceneId == sPresentScenes[i].mSceneId) {
            found = TRUE;
        }
    }
    return found;
}

// Does nothing
void Scene40::Unused0214ca80() {}

// --- The scene's lower-screen pages (scene+0x30B50) ----------------------------------------------------------------

// Every page from the context. The pages of the sub engine all start their sprites at the same character offset
// (one is shown at a time); the offset left after them is the furthest any went, where the keyboard's go.
void Scene40Pages::Init(int *mainCharOffset, int *subCharOffset, Scene40Context *context) {
    int start = *subCharOffset;
    int end = 0;
    mContext = context;
    mSpeechWindow = context->mSpeechWindow;
    mTracker = context->mTouch;
    mCharacter = context->mCharacter;
    mWaitingForLayout = FALSE;
    mActive = TRUE;
    mRequestedLayout = 0;
    mListFrame = 0;
    mKeyboardLayout = 1;
    mTouch.Init(context->mTouch, context->mCharacter);

    mListPage.Init(context, subCharOffset);
    if (end < *subCharOffset) {
        end = *subCharOffset;
    }
    *subCharOffset = start;
    mOptionPage.Init(context, subCharOffset);
    if (end < *subCharOffset) {
        end = *subCharOffset;
    }
    *subCharOffset = start;
    mKeyboardPage.Init(context, subCharOffset);
    if (end < *subCharOffset) {
        end = *subCharOffset;
    }
    *subCharOffset = start;
    mProfilePage.Init(context, subCharOffset);
    mProfilePage.Refresh();
    if (end < *subCharOffset) {
        end = *subCharOffset;
    }
    *subCharOffset = start;

    mMenuBar.Init(context);
    mTouchPage.Init(context, &mTouch, &mMenuBar);
    mTalkPage.Init(context, mainCharOffset, &mMenuBar, &mProfilePage);
    mGiftPage.Init(context, &mTouch, &mProfilePage, &mCounter);
    mPresentPage.Init(context, &mTouch);
    mCounter.Init(context, mainCharOffset);
    mKeyboardPage.mObjCharOffset = end;
    *subCharOffset = end;
    mNextPage = PAGE_TOUCH;
    mPage = PAGE_TOUCH;
}

// The page asked for last, unless the scene is changing the upper screen; the menu every frame
void Scene40Pages::Update() {
    mRequestedLayout = 0;
    if (!mWaitingForLayout) {
        mPage = mNextPage;
        switch (mPage) {
        case PAGE_TOUCH:
            UpdateTouchPage();
            break;
        case PAGE_TALK:
            UpdateTalkPage();
            break;
        case PAGE_GIFT:
            UpdateGiftPage();
            break;
        case PAGE_PRESENT:
            UpdatePresentPage();
            break;
        case PAGE_LIST_GIVE:
            UpdateListGivePage();
            break;
        case PAGE_LIST_GIVE_FROM_SHOP:
            UpdateListGiveFromShopPage();
            break;
        case PAGE_LIST_DRESS:
            UpdateListDressPage();
            break;
        case PAGE_LIST_DRESS_2:
            UpdateListDress2Page();
            break;
        case PAGE_KEYBOARD:
            UpdateKeyboardPage();
            break;
        case PAGE_OPTIONS:
            UpdateOptionPage();
            break;
        }
    }
    mMenuBar.Update();
}

// The touch page: when it is done, what it asked for (its mResult)
void Scene40Pages::UpdateTouchPage() {
    mProfilePage.Update();
    mCounter.Update();
    mTouchPage.Update();
    if (mTouchPage.mLastState != Scene40TouchPage::STATE_DONE) {
        return;
    }
    switch (mTouchPage.mResult) {
    case 0:
        if (mContext->mSceneId == 0x40) {
            mTalkPage.mTopic = 0;
            mTalkPage.mNextState = Scene40TalkPage::STATE_START;
            mNextPage = PAGE_TALK;
        } else if (mContext->mSceneId == 0x6C) {
            mTalkPage.mTopic = 4;
            mTalkPage.mNextState = Scene40TalkPage::STATE_START;
            mNextPage = PAGE_TALK;
        }
        mTouchPage.SetMenuDisabled(TRUE);
        break;
    case 1:
        if (mContext->mSceneId == 0x40) {
            mTalkPage.mTopic = 0;
            mTalkPage.mNextState = Scene40TalkPage::STATE_START;
            mNextPage = PAGE_TALK;
        } else if (mContext->mSceneId == 0x6C) {
            mTalkPage.mTopic = 4;
            mTalkPage.mNextState = Scene40TalkPage::STATE_START;
            mNextPage = PAGE_TALK;
        }
        break;
    case 2:
        mProfilePage.ShowProfile();
        mProfilePage.SetSwitchable(FALSE);
        mTouchPage.SetMenuDisabled(TRUE);
        mGiftPage.mReaction = 4;
        mGiftPage.SetItem(mContext->mUnknown14, mContext->mUnknown18);
        mGiftPage.mNextState = 1;
        mGiftPage.mStep = 0;
        mGiftPage.mUnknown08 = 0;
        mNextPage = PAGE_GIFT;
        break;
    case 3:
        mProfilePage.ShowProfile();
        mProfilePage.SetSwitchable(FALSE);
        mTouchPage.SetMenuDisabled(TRUE);
        mGiftPage.mReaction = 6;
        mGiftPage.SetValue(mContext->mUnknown18);
        mGiftPage.mNextState = 1;
        mGiftPage.mStep = 0;
        mGiftPage.mUnknown08 = 0;
        mNextPage = PAGE_GIFT;
        break;
    case 4:
        mProfilePage.ShowProfile();
        mProfilePage.SetSwitchable(FALSE);
        mTouchPage.SetMenuDisabled(TRUE);
        mGiftPage.mReaction = 9;
        mGiftPage.SetValue(mContext->mUnknown18);
        mGiftPage.mNextState = 1;
        mGiftPage.mStep = 0;
        mGiftPage.mUnknown08 = 0;
        mNextPage = PAGE_GIFT;
        break;
    case 5:
        mListPage.mMode = Scene40ListPage::MODE_GIVE;
        mListPage.mTab = 0;
        mListPage.mNextState = Scene40ListPage::STATE_OPEN;
        mTouchPage.SetMenuDisabled(TRUE);
        mWaitingForLayout = TRUE;
        mRequestedLayout = 2;
        mListFrame = 0;
        mNextPage = PAGE_LIST_GIVE;
        break;
    case 6:
        mListPage.mMode = Scene40ListPage::MODE_DRESS;
        mListPage.mTab = 0;
        mListPage.mNextState = Scene40ListPage::STATE_OPEN;
        mTouchPage.SetMenuDisabled(TRUE);
        mWaitingForLayout = TRUE;
        mRequestedLayout = 2;
        mListFrame = 1;
        mNextPage = PAGE_LIST_DRESS;
        break;
    case 7:
        wcscpy(mKeyboardPage.mText2, mCharacter->mMotto);
        mKeyboardPage.mUnknown0008 = 0;
        mKeyboardPage.mNextState = 1;
        mTouchPage.SetMenuDisabled(TRUE);
        mWaitingForLayout = TRUE;
        mRequestedLayout = 3;
        mKeyboardLayout = 1;
        mNextPage = PAGE_KEYBOARD;
        break;
    case 8:
        mProfilePage.ShowProfile();
        mProfilePage.SetSwitchable(FALSE);
        mGiftPage.mReaction = 4;
        mGiftPage.SetItem(0, 30);
        mGiftPage.mNextState = 1;
        mGiftPage.mStep = 0;
        mGiftPage.mUnknown08 = 0;
        mTouchPage.SetMenuDisabled(TRUE);
        mNextPage = PAGE_GIFT;
        break;
    case 9:
        mProfilePage.ShowProfile();
        mProfilePage.SetSwitchable(FALSE);
        mGiftPage.mReaction = 4;
        mGiftPage.SetItem(3, -30);
        mGiftPage.mNextState = 1;
        mGiftPage.mStep = 0;
        mGiftPage.mUnknown08 = 0;
        mTouchPage.SetMenuDisabled(TRUE);
        mNextPage = PAGE_GIFT;
        break;
    case 10:
        mKeyboardPage.mNextState = 1;
        mTouchPage.SetMenuDisabled(TRUE);
        mWaitingForLayout = TRUE;
        mRequestedLayout = 3;
        mKeyboardLayout = 1;
        mNextPage = PAGE_KEYBOARD;
        break;
    case 11:
        mOptionPage.UpdateDone();
        mOptionPage.mUnknown000C = 0;
        mOptionPage.mNextState = 1;
        mTouchPage.SetMenuDisabled(TRUE);
        mWaitingForLayout = TRUE;
        mRequestedLayout = 4;
        mNextPage = PAGE_OPTIONS;
        break;
    case 12:
        mPresentPage.mUnknown124 = TRUE;
        mPresentPage.Start();
        mTouchPage.SetMenuDisabled(TRUE);
        mNextPage = PAGE_PRESENT;
        break;
    case 13:
        mProfilePage.ShowProfile();
        mProfilePage.SetSwitchable(FALSE);
        mGiftPage.mReaction = 28;
        mGiftPage.mNextState = 1;
        mGiftPage.mStep = 0;
        mGiftPage.mUnknown08 = 0;
        mTouchPage.SetMenuDisabled(TRUE);
        mNextPage = PAGE_GIFT;
        break;
    case 14:
        mProfilePage.ShowProfile();
        mProfilePage.SetSwitchable(FALSE);
        mGiftPage.mReaction = 17;
        mGiftPage.mNextState = 1;
        mGiftPage.mStep = 0;
        mGiftPage.mUnknown08 = 0;
        mTouchPage.SetMenuDisabled(TRUE);
        mNextPage = PAGE_GIFT;
        break;
    case 15:
        mProfilePage.ShowProfile();
        mProfilePage.SetSwitchable(FALSE);
        mGiftPage.mReaction = 29;
        mGiftPage.mNextState = 1;
        mGiftPage.mStep = 0;
        mGiftPage.mUnknown08 = 0;
        mTouchPage.SetMenuDisabled(TRUE);
        mNextPage = PAGE_GIFT;
        break;
    }
}

// The talk page: when it is done, what its answer leads to (its mResult)
void Scene40Pages::UpdateTalkPage() {
    mProfilePage.Update();
    mCounter.Update();
    mTalkPage.Update();
    if (mTalkPage.mState != Scene40TalkPage::STATE_DONE) {
        return;
    }
    switch (mTalkPage.mResult) {
    case 0:
        mTouchPage.SetMenuDisabled(FALSE);
        mTouchPage.mEntry = 0;
        mTouchPage.mState = Scene40TouchPage::STATE_START;
        mNextPage = PAGE_TOUCH;
        break;
    case 1:
        mListPage.mMode = Scene40ListPage::MODE_GIVE;
        mListPage.mTab = 0;
        mListPage.mNextState = Scene40ListPage::STATE_OPEN;
        mWaitingForLayout = TRUE;
        mRequestedLayout = 2;
        mListFrame = 0;
        mNextPage = PAGE_LIST_GIVE;
        break;
    case 2:
        // A word to teach: the keyboard by the trouble
        switch (mCharacter->mTroubleId) {
        case 0:
            mKeyboardPage.mUnknown0008 = 1;
            mKeyboardLayout = 1;
            break;
        case 1:
            mKeyboardPage.mUnknown0008 = 4;
            mKeyboardLayout = 3;
            break;
        case 2:
            mKeyboardPage.mUnknown0008 = 5;
            mKeyboardLayout = 3;
            break;
        case 3:
            mKeyboardPage.mUnknown0008 = 6;
            mKeyboardLayout = 3;
            break;
        case 4:
            mKeyboardPage.mUnknown0008 = 7;
            mKeyboardLayout = 3;
            break;
        }
        mKeyboardPage.mNextState = 1;
        mWaitingForLayout = TRUE;
        mRequestedLayout = 3;
        mNextPage = PAGE_KEYBOARD;
        break;
    case 5:
        mListPage.mMode = Scene40ListPage::MODE_DRESS_2;
        mListPage.mTab = 0;
        mListPage.mNextState = Scene40ListPage::STATE_OPEN;
        mWaitingForLayout = TRUE;
        mRequestedLayout = 2;
        mListFrame = 1;
        mNextPage = PAGE_LIST_DRESS_2;
        break;
    case 6:
        mProfilePage.ShowProfile();
        mProfilePage.SetSwitchable(FALSE);
        mGiftPage.mReaction = 4;
        mGiftPage.SetItem(mContext->mUnknown14, mContext->mUnknown18);
        mGiftPage.mNextState = 1;
        mGiftPage.mStep = 0;
        mGiftPage.mUnknown08 = 0;
        mNextPage = PAGE_GIFT;
        break;
    case 7:
        mProfilePage.ShowProfile();
        mProfilePage.SetSwitchable(FALSE);
        mGiftPage.mReaction = 5;
        mGiftPage.SetValue(mContext->mUnknown18);
        mGiftPage.mNextState = 1;
        mGiftPage.mStep = 0;
        mGiftPage.mUnknown08 = 0;
        mNextPage = PAGE_GIFT;
        break;
    case 8:
        mProfilePage.ShowProfile();
        mProfilePage.SetSwitchable(FALSE);
        mGiftPage.mReaction = 6;
        mGiftPage.SetValue(mContext->mUnknown18);
        mGiftPage.mNextState = 1;
        mGiftPage.mStep = 0;
        mGiftPage.mUnknown08 = 0;
        mNextPage = PAGE_GIFT;
        break;
    case 9:
        mProfilePage.ShowProfile();
        mProfilePage.SetSwitchable(FALSE);
        mGiftPage.mReaction = 8;
        mGiftPage.SetValue(mContext->mUnknown18);
        mGiftPage.mNextState = 1;
        mGiftPage.mStep = 0;
        mGiftPage.mUnknown08 = 0;
        mNextPage = PAGE_GIFT;
        break;
    case 10:
        mTouchPage.mEntry = 13;
        mTouchPage.mState = Scene40TouchPage::STATE_START;
        mNextPage = PAGE_TOUCH;
        break;
    }
}

// The gift page: when it is done, the option page (if the gift was the one asked for and the first panel is not
// all done), the reaction to finishing it, or back to the touch page
void Scene40Pages::UpdateGiftPage() {
    mProfilePage.Update();
    mCounter.Update();
    mGiftPage.Update();
    if (mGiftPage.mState != Scene40GiftPage::STATE_RESET) {
        return;
    }
    BOOL gotTarget = mGiftPage.mUnknown6C;
    BOOL allDone = FALSE;
    if (gotTarget) {
        mOptionPage.UpdateDone();
        allDone = mOptionPage.IsAllDone();
    }
    if (gotTarget) {
        if (!allDone) {
            mOptionPage.mUnknown000C = 0;
            mOptionPage.mNextState = 1;
            mWaitingForLayout = TRUE;
            mRequestedLayout = 4;
            mNextPage = PAGE_OPTIONS;
            return;
        }
        mGiftPage.mReaction = 7;
        mGiftPage.mNextState = 1;
        mGiftPage.mStep = 0;
        mGiftPage.mUnknown08 = 0;
        mNextPage = PAGE_GIFT;
        func_ov075_021793fc(func_ov075_02179290(mCharacter));
        gSoundManager.MoveSeqVolume(0, 127, 120);
        gSoundManager.LoadGroup(77);
        return;
    }
    mProfilePage.SetSwitchable(TRUE);
    mTouchPage.mEntry = 2;
    mTouchPage.mState = Scene40TouchPage::STATE_START;
    mNextPage = PAGE_TOUCH;
}

// The present page: when it is done, back to the touch page
void Scene40Pages::UpdatePresentPage() {
    mProfilePage.Update();
    mPresentPage.Update();
    if (mPresentPage.mState != Scene40PresentPage::STATE_DONE) {
        return;
    }
    mTouchPage.SetMenuDisabled(FALSE);
    mTouchPage.mEntry = 2;
    mTouchPage.mState = Scene40TouchPage::STATE_START;
    mNextPage = PAGE_TOUCH;
}

// The list of what to give: once chosen, back to the scene's own upper screen; once closed, the gift's reaction
void Scene40Pages::UpdateListGivePage() {
    mListPage.Update();
    if (mListPage.mState == Scene40ListPage::STATE_CHOSEN) {
        mRequestedLayout = 1;
        mWaitingForLayout = TRUE;
        return;
    }
    if (mListPage.mState != Scene40ListPage::STATE_CLOSE) {
        return;
    }
    if (mListPage.mChosen) {
        Scene40Gift gift;
        int reaction;
        int value;
        mListPage.GetGift(&gift);
        GetGiftReaction(&gift, &reaction, &value);
        mGiftPage.mReaction = 0;
        mGiftPage.mGift.Set(gift.mKind, gift.mItem.InventoryItem::mId);
        mGiftPage.SetItem(reaction, value);
        mGiftPage.mNextState = 1;
        mGiftPage.mStep = 0;
        mGiftPage.mUnknown08 = 0;
        mProfilePage.ShowProfile();
        mProfilePage.SetSwitchable(FALSE);
        mNextPage = PAGE_GIFT;
        return;
    }
    mTouchPage.SetMenuDisabled(FALSE);
    mTouchPage.mEntry = 0;
    mTouchPage.mState = Scene40TouchPage::STATE_START;
    mProfilePage.ShowProfile();
    mNextPage = PAGE_TOUCH;
}

void Scene40Pages::UpdateListGiveFromShopPage() {
    mListPage.Update();
    if (mListPage.mState == Scene40ListPage::STATE_CHOSEN) {
        mRequestedLayout = 1;
        mWaitingForLayout = TRUE;
        return;
    }
    if (mListPage.mState != Scene40ListPage::STATE_CLOSE) {
        return;
    }
    mTouchPage.SetMenuDisabled(FALSE);
    mTouchPage.mEntry = 1;
    mTouchPage.mState = Scene40TouchPage::STATE_START;
    mProfilePage.ShowProfile();
    mProfilePage.SetSwitchable(TRUE);
    mNextPage = PAGE_TOUCH;
}

void Scene40Pages::UpdateListDressPage() {
    mListPage.Update();
    if (mListPage.mState == Scene40ListPage::STATE_CHOSEN) {
        mRequestedLayout = 1;
        mWaitingForLayout = TRUE;
        return;
    }
    if (mListPage.mState != Scene40ListPage::STATE_CLOSE) {
        return;
    }
    if (mListPage.mChosen) {
        mTouchPage.mEntry = 1;
        mTouchPage.mState = Scene40TouchPage::STATE_START;
        mProfilePage.ShowProfile();
        mProfilePage.SetSwitchable(TRUE);
        mNextPage = PAGE_TOUCH;
        return;
    }
    mTouchPage.SetMenuDisabled(FALSE);
    mTouchPage.mEntry = 0;
    mTouchPage.mState = Scene40TouchPage::STATE_START;
    mProfilePage.ShowProfile();
    mProfilePage.SetSwitchable(TRUE);
    mNextPage = PAGE_TOUCH;
}

void Scene40Pages::UpdateListDress2Page() {
    mListPage.Update();
    if (mListPage.mState == Scene40ListPage::STATE_CHOSEN) {
        mRequestedLayout = 1;
        mWaitingForLayout = TRUE;
        return;
    }
    if (mListPage.mState != Scene40ListPage::STATE_CLOSE) {
        return;
    }
    mTouchPage.mEntry = 1;
    mTouchPage.mState = Scene40TouchPage::STATE_START;
    mProfilePage.ShowProfile();
    mProfilePage.SetSwitchable(FALSE);
    mNextPage = PAGE_TOUCH;
}

// The keyboard: once typed, back to the scene's upper screen (or the option page's); once closed, what was typed
// goes to the character: a motto (with a present for it) or a word it is taught
void Scene40Pages::UpdateKeyboardPage() {
    mKeyboardPage.Update();
    if (mKeyboardPage.mState == 3) {
        switch (mKeyboardPage.mUnknown0008) {
        case 0:
        case 1:
        case 3:
        case 4:
        case 5:
        case 6:
        case 7:
            mRequestedLayout = 1;
            mWaitingForLayout = TRUE;
            break;
        case 2:
            switch (mKeyboardPage.mUnknown0010) {
            case 0:
                mRequestedLayout = 1;
                mWaitingForLayout = TRUE;
                break;
            case 1:
                mRequestedLayout = 4;
                mWaitingForLayout = TRUE;
                break;
            }
            break;
        }
    } else if (mKeyboardPage.mState == 4) {
    switch (mKeyboardPage.mUnknown0008) {
    case 0:
        switch (mKeyboardPage.mUnknown0010) {
        case 0: {
            wcscpy(mCharacter->mMotto, mKeyboardPage.mText);
            Scene40Gift gift;
            gift.Set(4, 0);
            mGiftPage.mReaction = 1;
            mGiftPage.mGift.Set(gift.mKind, gift.mItem.InventoryItem::mId);
            mGiftPage.SetItem(0, 0);
            mGiftPage.mNextState = 1;
            mGiftPage.mStep = 0;
            mGiftPage.mUnknown08 = 0;
            mProfilePage.ShowProfile();
            mProfilePage.SetSwitchable(FALSE);
            mNextPage = PAGE_GIFT;
            break;
        }
        case 1:
            mTouchPage.SetMenuDisabled(FALSE);
            mTouchPage.mEntry = 0;
            mTouchPage.mState = Scene40TouchPage::STATE_START;
            mProfilePage.ShowProfile();
            mProfilePage.SetSwitchable(TRUE);
            mNextPage = PAGE_TOUCH;
            break;
        }
        break;
    case 1:
    case 4:
    case 5:
    case 6:
    case 7:
        switch (mKeyboardPage.mUnknown0010) {
        case 0:
            mCharacter->SetTaughtWord(mKeyboardPage.mText);
            mTalkPage.mTopic = 1;
            mTalkPage.mNextState = Scene40TalkPage::STATE_START;
            mProfilePage.ShowProfile();
            mProfilePage.SetSwitchable(TRUE);
            mNextPage = PAGE_TALK;
            break;
        case 1:
            mCharacter->SetState(0);
            mTouchPage.SetMenuDisabled(FALSE);
            mTouchPage.mEntry = 0;
            mTouchPage.mState = Scene40TouchPage::STATE_START;
            mProfilePage.ShowProfile();
            mProfilePage.SetSwitchable(TRUE);
            mNextPage = PAGE_TOUCH;
            break;
        }
        break;
    case 2:
        switch (mKeyboardPage.mUnknown0010) {
        case 0: {
            wcscpy(mCharacter->mMotto, mKeyboardPage.mText);
            Scene40Gift gift;
            gift.Set(4, 0);
            mGiftPage.mReaction = 2;
            mGiftPage.mGift.Set(gift.mKind, gift.mItem.InventoryItem::mId);
            mGiftPage.SetItem(0, 0);
            mGiftPage.mNextState = 1;
            mGiftPage.mStep = 0;
            mGiftPage.mUnknown08 = 0;
            mProfilePage.ShowProfile();
            mProfilePage.SetSwitchable(FALSE);
            mNextPage = PAGE_GIFT;
            break;
        }
        case 1:
            mOptionPage.mUnknown000C = 0;
            mOptionPage.mNextState = 1;
            mNextPage = PAGE_OPTIONS;
            break;
        }
        break;
    case 3:
        mTouchPage.SetMenuDisabled(FALSE);
        mTouchPage.mEntry = 0;
        mTouchPage.mState = Scene40TouchPage::STATE_START;
        mProfilePage.ShowProfile();
        mProfilePage.SetSwitchable(TRUE);
        mNextPage = PAGE_TOUCH;
        break;
    }
    }
}

// The option page: once chosen, back to the scene's upper screen; once closed, the present chosen (a reaction),
// or the keyboard for a motto
void Scene40Pages::UpdateOptionPage() {
    mOptionPage.Update();
    if (mOptionPage.mState == 10) {
        switch (mOptionPage.mResult) {
        case 0:
        case 1:
            mRequestedLayout = 1;
            mWaitingForLayout = TRUE;
            break;
        case 2:
            break;
        case 3:
            mRequestedLayout = 3;
            mKeyboardLayout = 1;
            mWaitingForLayout = TRUE;
            break;
        }
    } else if (mOptionPage.mState == 11) {
    switch (mOptionPage.mResult) {
    case 0:
    case 1: {
        Scene40Gift gift;
        gift.Set(mOptionPage.mGift.mKind, mOptionPage.mGift.mItem.InventoryItem::mId);
        mGiftPage.mReaction = 2;
        mGiftPage.mGift.Set(gift.mKind, gift.mItem.InventoryItem::mId);
        mGiftPage.SetItem(0, 0);
        mGiftPage.mNextState = 1;
        mGiftPage.mStep = 0;
        mGiftPage.mUnknown08 = 0;
        mProfilePage.ShowProfile();
        mProfilePage.SetSwitchable(FALSE);
        mNextPage = PAGE_GIFT;
        break;
    }
    case 2:
        break;
    case 3:
        mKeyboardPage.mUnknown0008 = 2;
        mKeyboardPage.mNextState = 1;
        mNextPage = PAGE_KEYBOARD;
        break;
    }
    }
}

// The pages take touches only while the scene is not changing the upper screen
void Scene40Pages::SetActive(BOOL active) {
    if (active == mActive) {
        return;
    }
    mActive = active;
    mTouchPage.mActive = active;
    mTalkPage.mActive = active;
    mGiftPage.mActive = active;
    mPresentPage.mActive = active;
    mListPage.mActive = active;
    mKeyboardPage.mActive = active;
    mOptionPage.mActive = active;
}

void Scene40Pages::HideMenu(BOOL hide) {
    if (!hide) {
        return;
    }
    mTouchPage.HideMenu(TRUE);
}

// Whether the gift is the goods the character's trouble asks for: the very item, or one of the kinds it names
BOOL Scene40Pages::IsWishedGoods(SavedCharacter *character, Scene40Gift *gift) {
    BOOL wished = FALSE;
    if (character->IsInTrouble() &&
        data_02114d78[character->mTroubleCategory][character->mTroubleId].kind == 1) {
        if (data_02114d78[character->mTroubleCategory][character->mTroubleId].hasTarget) {
            if (gift->mItem.InventoryItem::mId == character->mTroubleTarget) {
                wished = TRUE;
            }
        } else {
            u32 flags = GetGoodsInfo(gift->mItem.InventoryItem::mId)->mFlags;
            u32 wanted = data_02114d78[character->mTroubleCategory][character->mTroubleId].flags;
            u32 asked = flags & wanted;
            u32 missing = ~flags & wanted;
            if (asked != 0 && missing == 0) {
                wished = TRUE;
            } else if (asked != 0) {
                wished = TRUE;
            }
        }
    }
    return wished;
}

// The same for clothes, of the kinds the trouble names
BOOL Scene40Pages::IsWishedClothes(SavedCharacter *character, Scene40Gift *gift) {
    BOOL wished = FALSE;
    Scene40Character *visited = mCharacter;
    if (visited->IsInTrouble() && data_02114d78[visited->mTroubleCategory][visited->mTroubleId].kind == 3) {
        if (data_02114d78[character->mTroubleCategory][character->mTroubleId].hasTarget) {
            if (gift->mItem.InventoryItem::mId == character->mTroubleTarget) {
                wished = TRUE;
            }
        } else {
            u32 styles = GetClothesInfo(gift->mItem.InventoryItem::mId)->mStyles;
            u32 wanted = data_02114d78[mCharacter->mTroubleCategory][mCharacter->mTroubleId].flags;
            u32 asked = styles & wanted;
            u32 missing = ~styles & wanted;
            if (asked != 0 && missing == 0) {
                wished = TRUE;
            } else if (asked != 0) {
                wished = TRUE;
            }
        }
    }
    return wished;
}

// How the character reacts to a gift and what it is worth to it: its favourite goods are worth at least what the
// trouble promises when they are the goods it asks for; goods or clothes it asks for are worth that
void Scene40Pages::GetGiftReaction(Scene40Gift *gift, int *reaction, int *value) {
    switch (gift->mGiftKind) {
    case 0:
        if (mCharacter->mFavoriteItem == gift->mItem.InventoryItem::mId) {
            *reaction = mCharacter->GetGoodsReaction(gift->mItem.InventoryItem::mId);
            int satisfaction = mCharacter->GetGoodsSatisfaction(gift->mItem.InventoryItem::mId);
            int promised = 0;
            if (IsWishedGoods(mCharacter, gift)) {
                promised = GetTrouble(mCharacter)->unk4Bits0;
            }
            if (satisfaction < promised) {
                satisfaction = promised;
            }
            *value = satisfaction;
        } else if (mCharacter->mDislikedItem == gift->mItem.InventoryItem::mId) {
            *reaction = mCharacter->GetGoodsReaction(gift->mItem.InventoryItem::mId);
            *value = mCharacter->GetGoodsSatisfaction(gift->mItem.InventoryItem::mId);
        } else if (IsWishedGoods(mCharacter, gift)) {
            *reaction = 0;
            *value = GetTrouble(mCharacter)->unk4Bits0;
        } else {
            *reaction = mCharacter->GetGoodsReaction(gift->mItem.InventoryItem::mId);
            *value = mCharacter->GetGoodsSatisfaction(gift->mItem.InventoryItem::mId);
        }
        break;
    case 1:
        if (IsWishedClothes(mCharacter, gift)) {
            *reaction = 5;
            *value = GetTrouble(mCharacter)->unk4Bits0;
        } else {
            *reaction = mCharacter->GetClothesReaction(gift->mItem.InventoryItem::mId);
            *value = mCharacter->GetClothesSatisfaction(gift->mItem.InventoryItem::mId);
        }
        break;
    case 3:
    case 4:
    case 5:
    case 6:
    case 16:
        *reaction = 2;
        *value = 0;
        break;
    }
}

// Back from one of the games 0x82 to 0x87 that was played to the end: the character forgets which it was
BOOL Scene40Pages::TakeGameResult() {
    Scene40Scratch *const scratch = &gSceneScratch;
    BOOL taken = FALSE;
    switch (gSceneManager.mPreviousSceneId) {
    case 0x82:
    case 0x83:
    case 0x84:
    case 0x85:
    case 0x86:
    case 0x87:
        if (scratch->mUnknown01 == 0) {
            taken = TRUE;
            mCharacter->ResetUnknown24E();
        }
        break;
    }
    return taken;
}

// The page shown: the first four together with the touch page's, and the menu
void Scene40Pages::Draw() {
    mCounter.Draw();
    switch (mPage) {
    case PAGE_TOUCH:
    case PAGE_TALK:
    case PAGE_GIFT:
    case PAGE_PRESENT:
        mTouchPage.Draw();
        mTalkPage.Draw();
        mGiftPage.Draw();
        mPresentPage.Draw();
        mProfilePage.Draw();
        mMenuBar.Draw();
        break;
    case PAGE_LIST_GIVE:
    case PAGE_LIST_GIVE_FROM_SHOP:
    case PAGE_LIST_DRESS:
    case PAGE_LIST_DRESS_2:
        mListPage.Draw();
        break;
    case PAGE_KEYBOARD:
        mKeyboardPage.Draw();
        break;
    case PAGE_OPTIONS:
        mOptionPage.Draw();
        break;
    }
}

// --- What the stylus does to the character (the pages' +0x2C) --------------------------------------------------------

void Scene40CharacterTouch::Init(Scene40Touch *tracker, Scene40Character *character) {
    mTracker = tracker;
    mCharacter = character;
}

static inline ScreenPoint Add(ScreenPoint a, ScreenPoint b) {
    ScreenPoint sum;
    sum.x = a.x + b.x;
    sum.y = a.y + b.y;
    return sum;
}

static inline ScreenPoint Sub(ScreenPoint a, ScreenPoint b) {
    ScreenPoint difference;
    difference.x = a.x - b.x;
    difference.y = a.y - b.y;
    return difference;
}

static inline u32 LengthSq(ScreenPoint a) {
    return a.x * a.x + a.y * a.y;
}

static inline ScreenPoint ToFx(ScreenPoint a) {
    ScreenPoint fx;
    fx.x = a.x << FX32_SHIFT;
    fx.y = a.y << FX32_SHIFT;
    return fx;
}

// Where the stylus is in the room, and which way it strokes and is flicked
void Scene40CharacterTouch::Update() {
    if (mTracker->mPressed) {
        Clear();
        mStroke = 0;
        VecFx32 origin = mCharacter->CharacterModel::mPos;
        mDepths[0].mOrigin.x = origin.x;
        mDepths[0].mOrigin.y = origin.y;
        mDepths[0].mOrigin.z = origin.z;
        ScreenPoint point = mTracker->mPressPoint;
        mPartAtPress = HitTest(&point);
        ProjectTouch(&point, &mDepths[0].mPress, &mDepths[0].mOrigin);
        ProjectTouchBetween(&point, &mDepths[1].mPress, FX32_CONST(3), FX32_CONST(4));
        ProjectTouchBetween(&point, &mDepths[2].mPress, FX32_CONST(2), FX32_CONST(1));
    } else if (mTracker->mHeld) {
        ScreenPoint point = mTracker->mPoint;
        if (LengthSq(Sub(mTracker->mPoint, mTracker->mPressPoint)) > 20 * 20) {
            ScreenPoint move = ToFx(Sub(mTracker->mPoint, mTracker->mPressPoint));
            u16 angle = FX_Atan2Idx(move.y, move.x);
            if (angle >= 0xB000 && angle < 0xD000) {
                mStroke = 1;
            } else if (angle >= 0x3000 && angle < 0x5000) {
                mStroke = 2;
            } else if (angle >= 0x7000 && angle < 0x9000) {
                mStroke = 3;
            } else if (angle >= 0xF000 || angle < 0x1000) {
                mStroke = 4;
            } else if (angle >= 0x9000 && angle < 0xB000) {
                mStroke = 5;
            } else if (angle >= 0xD000 && angle < 0xF000) {
                mStroke = 6;
            } else if (angle >= 0x5000 && angle < 0x7000) {
                mStroke = 7;
            } else if (angle >= 0x1000 && angle < 0x3000) {
                mStroke = 8;
            }
        }
        mPart = HitTest(&point);
        mPartsTouched |= HitTest(&point);
        ProjectTouch(&point, &mDepths[0].mHold, &mDepths[0].mOrigin);
        ProjectTouchBetween(&point, &mDepths[1].mHold, FX32_CONST(3), FX32_CONST(4));
        ProjectTouchBetween(&point, &mDepths[2].mHold, FX32_CONST(2), FX32_CONST(1));
    } else if (mTracker->mReleased) {
        ScreenPoint point = mTracker->mReleasePoint;
        ProjectTouch(&point, &mDepths[0].mRelease, &mDepths[0].mOrigin);
        ProjectTouchBetween(&point, &mDepths[1].mRelease, FX32_CONST(3), FX32_CONST(4));
        ProjectTouchBetween(&point, &mDepths[2].mRelease, FX32_CONST(2), FX32_CONST(1));

        // Where the flick would have taken the stylus in a frame, and so which way and how fast it went
        ScreenPoint flicked = Add(mTracker->mReleasePoint, mTracker->mVelocity);
        VecFx32 flicked0, flicked1;
        ProjectTouch(&flicked, &flicked0, &mDepths[0].mOrigin);
        ProjectTouchBetween(&flicked, &flicked1, FX32_CONST(3), FX32_CONST(4));
        VEC_Subtract(&flicked0, &mDepths[0].mRelease, &mDepths[0].mFlick);
        VEC_Subtract(&flicked1, &mDepths[1].mRelease, &mDepths[1].mFlick);

        // Thrown away from the camera as well, by how high
        VecFx32 away0, away1;
        VEC_Subtract(&mDepths[0].mRelease, &gCamera.mEye, &away0);
        VEC_Subtract(&mDepths[1].mRelease, &gCamera.mEye, &away1);
        VEC_Normalize(&away0, &away0);
        VEC_Normalize(&away1, &away1);
        fx32 speed0 = MATH_CLAMP(mDepths[0].mFlick.y / 2, 0, 500);
        fx32 speed1 = MATH_CLAMP(mDepths[1].mFlick.y / 2, 1200, 1300);
        mDepths[0].mFlick.x += FX_Mul(speed0, away0.x);
        mDepths[0].mFlick.z += FX_Mul(speed0, away0.z);
        mDepths[1].mFlick.x += FX_Mul(speed1, away1.x);
        mDepths[1].mFlick.z += FX_Mul(speed1, away1.z);
    }
}

void Scene40CharacterTouch::Clear() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 5; j++) {
            VecFx32 zero = {0, 0, 0};
            (&mDepths[i].mOrigin)[j] = zero;
        }
    }
    mPartAtPress = 0;
    mPart = 0;
    mPartsTouched = 0;
}

// What of the character is under a point of the touch screen
int Scene40CharacterTouch::HitTest(const ScreenPoint *point) {
    int parts = 0;
    ModelResources_SetTouchLine(gModelResources, point->x, point->y);
    if (mCharacter->HitTestFace()) {
        parts |= PART_FACE;
    }
    if (mCharacter->HitTestJoint(3)) {
        parts |= PART_HEAD;
    }
    if (mCharacter->IsBalloonTouched()) {
        parts |= PART_BODY;
    }
    if (parts == 0) {
        parts = PART_NONE;
    }
    return parts;
}

// --- Where the stylus is in the room ---------------------------------------------------------------------------

// The point on the line into the room under a point of the touch screen that lies in the vertical plane through
// planePoint facing the camera; without one (the line in the plane), a fixed point
static void ProjectTouch(const ScreenPoint *point, VecFx32 *out, const VecFx32 *planePoint) {
    Camera *const camera = &gCamera;
    VecFx32 line, near, far, normal, side;
    NNS_G3dScrPosToWorldLine(point->x, point->y, &near, &far);
    VEC_Subtract(&far, &near, &line);
    const VecFx32 up = {0, FX32_ONE, 0};
    VecFx32 view, toPlane;
    VEC_Subtract(&camera->mTarget, &camera->mEye, &view);
    VEC_CrossProduct(&up, &view, &side);
    VEC_CrossProduct(&up, &side, &normal);
    VEC_Subtract(planePoint, &near, &toPlane);
    fx32 distance = VEC_DotProduct(&toPlane, &normal);
    fx32 step = VEC_DotProduct(&line, &normal);
    if (step != 0) {
        fx32 t = FX_Div(distance, step);
        out->x = FX_Mul(line.x, t) + near.x;
        out->y = FX_Mul(line.y, t) + near.y;
        out->z = FX_Mul(line.z, t) + near.z;
        return;
    }
    const VecFx32 fallback = {0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF};
    out->x = fallback.x;
    out->y = fallback.y;
    out->z = fallback.z;
}

// The same with the plane through a point between the camera's target and eye, by their weights
static void ProjectTouchBetween(const ScreenPoint *point, VecFx32 *out, fx32 targetWeight, fx32 eyeWeight) {
    Camera *const camera = &gCamera;
    VecFx32 planePoint, target, eye;
    ScaleVec(eyeWeight, &camera->mEye, &eye);
    ScaleVec(targetWeight, &camera->mTarget, &target);
    VEC_Add(&target, &eye, &planePoint);
    int sum = (targetWeight + eyeWeight) >> FX32_SHIFT;
    planePoint.x /= sum;
    planePoint.y /= sum;
    planePoint.z /= sum;
    ProjectTouch(point, out, &planePoint);
}

// A twelfth of the distance from the camera's eye
static fx32 GetCameraDistanceScale(const VecFx32 *pos) {
    VecFx32 offset;
    VEC_Subtract(&gCamera.mEye, pos, &offset);
    return FX_Mul(VEC_Mag(&offset), FX32_CONST(1) / 12);
}

// The camera's rotation, turned about x or y by an angle's sine and cosine: what faces the camera so turned
static void MakeFacingMatrixX(MtxFx33 *mtx, fx32 sin, fx32 cos) {
    MtxFx43 rot;
    const MtxFx43 *invView = NNS_G3dGlbGetInvV();
    MI_Copy36B(invView, mtx);
    MTX_RotX43(&rot, sin, cos);
    MTX_Concat43(&rot, invView, &rot);
    MI_Copy36B(&rot, mtx);
}

static void MakeFacingMatrixY(MtxFx33 *mtx, fx32 sin, fx32 cos) {
    MtxFx43 rot;
    const MtxFx43 *invView = NNS_G3dGlbGetInvV();
    MI_Copy36B(invView, mtx);
    MTX_RotY43(&rot, sin, cos);
    MTX_Concat43(&rot, invView, &rot);
    MI_Copy36B(&rot, mtx);
}

// --- The saved game's flags and counters that the scene changes --------------------------------------------------

// Sets or clears bits of gGameData's flags at 0xCC
static void SetGameFlags(u32 flags, BOOL set) {
    GameData *const game = &gGameData;
    if (set) {
        game->mUnknownCC |= flags;
    } else {
        game->mUnknownCC &= ~flags;
    }
}

static u32 GetGameFlags(u32 flags) {
    return gGameData.mUnknownCC & flags;
}

// The number of troubles solved, at most 9,999
static void AddTroublesSolved(int count) {
    u32 solved = gGameData.mTroublesSolved + count;
    if (solved > 9999) {
        solved = 9999;
    }
    gGameData.mTroublesSolved = solved;
}

// --- The chain of messages in the two windows (scene+0x2C908) -----------------------------------------------------

void Scene40MessageSequence::Init(MessageWindow *window0, MessageWindow *window1) {
    mEntry = 0;
    mState = STATE_IDLE;
    mLastState = STATE_IDLE;
    mWindow = 0;
    mFrames = 0;
    mDelay = 0;
    mWait = 0;
    mPrevious = ENTRY_NONE;
    mNext = ENTRY_NONE;
    mWindows[0] = window0;
    mWindows[1] = window1;
}

void Scene40MessageSequence::SetCharacter(SavedCharacter *character) {
    MiiData_GetName(character, mName);
}

void Scene40MessageSequence::Update() {
    int state = mState;
    mLastState = state;
    switch (state) {
    case STATE_IDLE:
        break;
    case STATE_START:
        mWindow = sMessageEntries[mEntry].mWindow;
        mDelay = sMessageEntries[mEntry].mDelay;
        mWait = sMessageEntries[mEntry].mWait;
        MessageWindow_SetTextMessage(mWindows[mWindow], 0, sMessageEntries[mEntry].mMessage);
        switch (sMessageEntries[mEntry].mArgKind) {
        case 0:
            break;
        case 1:
            MessageWindow_SetTextStringArg(mWindows[mWindow], 0, 0, mName);
            break;
        }
        MessageWindow_SetTextsVAlign(mWindows[mWindow], 1);
        if (mDelay <= 0) {
            if (mPrevious == ENTRY_NONE) {
                MessageWindow_Open(mWindows[mWindow], 18, 1);
            } else {
                MessageWindow_Open(mWindows[mWindow], 26, 1);
            }
            mState = STATE_OPENING;
        } else {
            mFrames = 0;
            mState = STATE_DELAY;
        }
        break;
    case STATE_DELAY:
        if (++mFrames < mDelay) {
            break;
        }
        if (mPrevious == ENTRY_NONE) {
            MessageWindow_Open(mWindows[mWindow], 18, 1);
        } else {
            MessageWindow_Open(mWindows[mWindow], 26, 1);
        }
        mState = STATE_OPENING;
        break;
    case STATE_OPENING:
        if (mWindows[mWindow]->mUnknownAF88) {
            mState = STATE_OPEN;
        }
        break;
    case STATE_OPEN:
        if (mWindows[mWindow]->mResult == -1) {
            break;
        }
        mNext = sMessageEntries[mEntry].mNext;
        if (mNext == ENTRY_NONE) {
            MessageWindow_Close(mWindows[mWindow], 20);
        } else {
            MessageWindow_Close(mWindows[mWindow], 28);
        }
        mState = STATE_CLOSING;
        break;
    case STATE_CLOSING:
        if (!mWindows[mWindow]->mTransitionDone) {
            break;
        }
        if (mWait <= 0) {
            mState = STATE_NEXT;
            break;
        }
        mFrames = 0;
        mState = STATE_WAIT;
        break;
    case STATE_WAIT:
        if (++mFrames >= mWait) {
            mState = STATE_NEXT;
        }
        break;
    case STATE_NEXT:
        if (mNext == ENTRY_NONE) {
            mPrevious = ENTRY_NONE;
            ScreenControl_SetBlendAlpha(&gScreenControl, 4, 17, 8, 8);
            mState = STATE_DONE;
            break;
        }
        mPrevious = mEntry;
        mEntry = mNext;
        mState = STATE_START;
        break;
    case STATE_DONE:
        mDelay = 0;
        mWait = 0;
        mState = STATE_IDLE;
        break;
    }
}

// --- The page the scene starts on (the pages' +0xFC) --------------------------------------------------------------

void Scene40TouchPage::Init(Scene40Context *context, Scene40CharacterTouch *touch, Scene40MenuBar *menu) {
    mContext = context;
    mCharacter = context->mCharacter;
    mTracker = mContext->mTouch;
    mTouch = touch;
    mState = 0;
    mLastState = 0;
    mStep = 0;
    mEntry = 0;
    mTouching = FALSE;
    mMenuHidden = FALSE;
    mActive = TRUE;
    mUnknown60 = 0;
    mUnknown64 = 0;
    mUnknown68 = 0;
    mMenu = menu;
    RefreshMenu();
}

// The state the controller gave, then one frame of it
void Scene40TouchPage::Update() {
    mTouch->Update();
    int state = mState;
    mLastState = state;
    switch (state) {
    case STATE_START:
        UpdateStart();
        break;
    case STATE_IDLE:
        UpdateIdle();
        break;
    case STATE_3:
        Update3();
        break;
    case STATE_4:
        Update4();
        break;
    case STATE_5:
        Update5();
        break;
    case STATE_WAIT_IDLE:
        UpdateWaitIdle();
        break;
    case STATE_7:
        Update7();
        break;
    case STATE_8:
        Update8();
        break;
    case STATE_WAIT_9:
        UpdateWait9();
        break;
    case STATE_10:
        Update10();
        break;
    case STATE_11:
        Update11();
        break;
    case STATE_12:
        Update12();
        break;
    case STATE_13:
        Update13();
        break;
    case STATE_14:
        Update14();
        break;
    case STATE_WAIT_MESSAGES:
        UpdateWaitMessages();
        break;
    case STATE_MESSAGES_0:
        UpdateMessages0();
        break;
    case STATE_MESSAGES_4:
        UpdateMessages4();
        break;
    case STATE_DONE:
        break;
    }
    mFrames++;
}

// The page forwards calling and availability to the menu it was given by Init.
void Scene40TouchPage::StartCall() {
    mMenu->StartCall();
}

void Scene40TouchPage::SetMenuDisabled(BOOL disabled) {
    mMenu->SetDisabled(disabled);
}

// Only a request to hide changes the page; showing it again is handled by its state updates.
void Scene40TouchPage::HideMenu(BOOL hide) {
    if (!hide) {
        return;
    }
    mMenuHidden = TRUE;
    mMenu->SetVisible(FALSE);
}

void Scene40TouchPage::RefreshMenu() {
    mMenu->Refresh();
    for (int i = 0; i < 5; i++) {
        mContext->mMenuAvailable[i] = mMenu->mAvailable[i];
    }
}

void Scene40TouchPage::UpdateMenu() {
    mMenu->UpdateAvailable();
}

// How the page starts, by the controller's entry: the menu, the character's state and the page's first state
void Scene40TouchPage::UpdateStart() {
    mTouching = FALSE;
    mCharacter->mBalloon.mUnknown08 = FALSE;
    switch (mEntry) {
    case 0:
        SetState(STATE_IDLE);
        break;
    case 1:
        mMenu->SetDisabled(TRUE);
        SetState(STATE_WAIT_IDLE);
        break;
    case 2:
        UpdateMenu();
        SetState(STATE_12);
        break;
    case 3:
        mCharacter->SetState(26);
        SetState(STATE_IDLE);
        break;
    case 4:
        mCharacter->SetState(0);
        SetState(STATE_IDLE);
        break;
    case 5:
        mMenu->SetLeftDisabled(TRUE);
        SetState(STATE_7);
        break;
    case 7:
        mMenu->SetLeftDisabled(TRUE);
        SetState(STATE_7);
        break;
    case 6:
        mMenu->SetDisabled(TRUE);
        mCharacter->SetState(18);
        SetState(STATE_WAIT_9);
        PlaceGift();
        break;
    case 8:
        mMenu->SetDisabled(TRUE);
        mCharacter->SetState(20);
        SetState(STATE_11);
        break;
    case 9: {
        // The room's interior counts as the character's own
        mCharacter->SetState(0);
        int interior = mCharacter->mInterior;
        mCharacter->mOwnedInteriors[interior >> 5] |= 1 << (interior & 31);
        UpdateMenu();
        SetState(STATE_12);
        break;
    }
    case 10:
        mMenu->SetDisabled(TRUE);
        mCharacter->SetState(0xA1);
        SetState(STATE_WAIT_IDLE);
        break;
    case 11:
        mMenu->SetDisabled(TRUE);
        mCharacter->SetState(0x6E);
        SetState(STATE_WAIT_IDLE);
        break;
    case 12:
        mCharacter->SetState(0xC6);
        mCharacter->mUnknown054Bit20 = (u8)(mCharacter->mUnknown054Bit20 + 1);
        mStep = 0;
        SetState(STATE_MESSAGES_0);
        break;
    case 13:
        mStep = 0;
        SetState(STATE_MESSAGES_4);
        break;
    }
}

// The menu and the stylus: a button of the menu, or the stylus down, held and up on the character
void Scene40TouchPage::UpdateIdle() {
    mMenu->SetEnabled(mActive);
    if (mMenu->mTouchedButton != -1) {
        UpdateMenuChoice();
    } else if (mContext->mTouch->mPressed) {
        mTouching = TRUE;
        UpdatePress();
    } else if (mContext->mTouch->mHeld && mTouching) {
        UpdateHold();
    }
    if (mContext->mTouch->mReleased && mTouching) {
        UpdateRelease();
        mTouching = FALSE;
    }
    Unused02151ab0();
}

// A button of the menu: the list to give (5), the clothes (6), the motto (7) for the controller, or another scene
void Scene40TouchPage::UpdateMenuChoice() {
    if (mMenu->mPressedButton == 0) {
        mMenu->SetEnabled(FALSE);
        mResult = 5;
        SetState(STATE_DONE);
    } else if (mMenu->mPressedButton == 1) {
        gSceneMemory.mCharacters[(u8)mCharacter->mIndex] = 0;
        gSceneManager.RequestScene(0x5B);
    } else if (mMenu->mPressedButton == 2) {
        mMenu->SetEnabled(FALSE);
        mResult = 6;
        SetState(STATE_DONE);
    } else if (mMenu->mPressedButton == 3) {
        mMenu->SetEnabled(FALSE);
        mResult = 7;
        SetState(STATE_DONE);
    } else if (mMenu->mPressedButton == 4) {
        int interior = mCharacter->mInterior;
        mCharacter->mOwnedInteriors[interior >> 5] |= 1 << (interior & 31);
        MI_CpuFill8(&gSceneScratch, 0, 100);
        gSceneScratch.mUnknown00 = 1;
        for (int i = 0; i < 5; i++) {
            gSceneScratch.mMenuAvailable[i] = mContext->mMenuAvailable[i];
        }
        gSceneMemory.mCharacters[0] = mCharacter->mIndex;
        gSceneManager.RequestScene(0x38);
    } else if (mMenu->mPressedButton == 5) {
        gSceneMemory.mCharacters[0] = mCharacter->mIndex;
        MI_CpuFill8(&gSceneScratch, 0, 100);
        gSceneScratch.mUnknown00 = 1;
        gSceneScratch.mUnknown01 = mCharacter->mIndex;
        gSceneManager.RequestScene(0x1A);
    } else if (mMenu->mPressedButton == 6) {
        if (mCharacter->IsLongActPaired() && mCharacter->mUnknown3094) {
            mCharacter->ClearLongActOfPairFrom(mContext->mClock->mSeconds);
        }
        gSceneManager.RequestScene(0x11);
    }
}

// The stylus comes down: on the balloon over the head (the character talks, or tells what it has to tell), on the
// face or the head (the sprite under the stylus), or beside the character (it turns its head there)
void Scene40TouchPage::UpdatePress() {
    if (mActive && (mTouch->mPartAtPress & Scene40CharacterTouch::PART_BODY) && mCharacter->mBalloon.mState == 1) {
        mCharacter->ShowBalloon(3);
        mCharacter->mBalloon.mUnknown08 = TRUE;
        gSceneMemory.mCharacters[0] = mCharacter->mIndex;
        BOOL inTrouble = mCharacter->IsInTrouble();
        BOOL hasNews = mCharacter->HasSomethingToTell();
        if (inTrouble) {
            gSoundManager.StartSeqArc(1, 26, 10);
            int category = mCharacter->mTroubleCategory;
            if (category == 0 || category == 1 || category == 2 || category == 8 ||
                (data_02114d78[category][mCharacter->mTroubleId].kind == 11 && mCharacter->mTroubleId != 1 &&
                 mCharacter->mTroubleId != 4)) {
                StartCall();
            }
            mCharacter->ChooseAfterTouch();
            if (mCharacter->mUnknown2ED0) {
                mMenu->SetEnabled(FALSE);
                mResult = 1;
                SetState(STATE_DONE);
            } else {
                mMenu->SetEnabled(FALSE);
                mResult = 0;
                SetState(STATE_DONE);
            }
        } else if (hasNews) {
            gSoundManager.StartSeqArc(1, 26, 9);
            mCharacter->SetState(28);
            mCharacter->mBalloon.mUnknown08 = FALSE;
        }
    } else if (mTouch->mPartAtPress & Scene40CharacterTouch::PART_FACE) {
        mContext->mSprite->mVisible = TRUE;
        Sprite_SetSequence(mContext->mSprite, 26, TRUE);
        ScreenPoint point = mContext->mTouch->mPressPoint;
        mContext->mSprite->SetPositionFromVec(point.x << FX32_SHIFT, point.y << FX32_SHIFT);
    } else if (mTouch->mPartAtPress & Scene40CharacterTouch::PART_HEAD) {
        mContext->mSprite->mVisible = TRUE;
        Sprite_SetSequence(mContext->mSprite, 26, TRUE);
        ScreenPoint point = mContext->mTouch->mPressPoint;
        mContext->mSprite->SetPositionFromVec(point.x << FX32_SHIFT, point.y << FX32_SHIFT);
    } else if (mTouch->mPartAtPress & Scene40CharacterTouch::PART_NONE) {
        if (mCharacter->mState == 0) {
            VecFx32 target;
            SetVecFields(&target, mTouch->mDepths[2].mPress);
            mCharacter->TurnHeadTo(&target, 0x2000, 0x3F00);
            mCharacter->SetState(10);
        } else if (mCharacter->mState == 6) {
            VecFx32 target;
            SetVecFields(&target, mTouch->mDepths[2].mPress);
            mCharacter->TurnHeadTo(&target, 0x2000, 0x3F00);
            mCharacter->SetState(13);
        }
    }
}

// The stylus held on the character: a stroke on the face (down: the character is patted and the camera turns to
// it; up: it is tickled), a long rub on the face, the scene's sprite following the stylus on the face or the head;
// beside the character, its head following the stylus
void Scene40TouchPage::UpdateHold() {
    BOOL handled = FALSE;
    if (mTouch->mPart & Scene40CharacterTouch::PART_FACE) {
        switch (mTouch->mStroke) {
        case 1: {
            VecFx32 pos = mCharacter->CharacterModel::mPos;
            if ((mTouch->mPartAtPress & Scene40CharacterTouch::PART_FACE) && pos.x > -0x3800 && pos.x < 0x3800 &&
                pos.z > -0x3800 && pos.z < 0x3800) {
                gSoundManager.StartSeqArc(1, 35, 2);
                mCharacter->SetState(1);
                mCharacter->FaceCamera();
                mCharacter->AddUnknown1C4Once0();
                mContext->mCamera->SetState(0);
                Sprite_SetSequence(mContext->mSprite, 27, TRUE);
                SetState(STATE_3);
                handled = TRUE;
            }
            break;
        }
        case 2:
            if (mTouch->mPartAtPress & Scene40CharacterTouch::PART_FACE) {
                gSoundManager.StartSeqArc(1, 35, 5);
                mCharacter->SetState(3);
                mCharacter->AddUnknown1C4Once0();
                SetState(STATE_4);
                handled = TRUE;
            }
            break;
        }
        if ((mTouch->mPartAtPress & Scene40CharacterTouch::PART_FACE) && !handled && mTracker->mDistance > 80) {
            if (mCharacter->mState == 0) {
                mCharacter->SetState(7);
                SetState(STATE_5);
                handled = TRUE;
            } else if (mCharacter->mState == 6) {
                mCharacter->SetState(8);
                SetState(STATE_5);
                handled = TRUE;
            }
        }
        mContext->mSprite->mVisible = TRUE;
        ScreenPoint point = mContext->mTouch->mPoint;
        mContext->mSprite->SetPositionFromVec(point.x << FX32_SHIFT, point.y << FX32_SHIFT);
    } else if (mTouch->mPart & Scene40CharacterTouch::PART_HEAD) {
        mContext->mSprite->mVisible = TRUE;
        ScreenPoint point = mContext->mTouch->mPoint;
        mContext->mSprite->SetPositionFromVec(point.x << FX32_SHIFT, point.y << FX32_SHIFT);
    } else if (mTouch->mPart & Scene40CharacterTouch::PART_BODY) {
        Sprite_SetSequence(mContext->mSprite, 26, TRUE);
        mContext->mSprite->mVisible = FALSE;
    } else if (mTouch->mPart & Scene40CharacterTouch::PART_NONE) {
        Sprite_SetSequence(mContext->mSprite, 26, TRUE);
        mContext->mSprite->mVisible = FALSE;
    }
    if (handled) {
        return;
    }
    if ((u32)(mCharacter->mState - 10) <= 12 - 10) {
        VecFx32 target;
        SetVecFields(&target, mTouch->mDepths[2].mHold);
        mCharacter->TurnHeadTo(&target, 0x2000, 0x3F00);
        if (mTracker->mHeldFrames < 10) {
            mCharacter->SetState(11);
        } else {
            mCharacter->SetState(12);
        }
    } else if ((u32)(mCharacter->mState - 13) <= 15 - 13) {
        VecFx32 target;
        SetVecFields(&target, mTouch->mDepths[2].mHold);
        mCharacter->TurnHeadTo(&target, 0x2000, 0x3F00);
        if (mTracker->mHeldFrames < 10) {
            mCharacter->SetState(14);
        } else {
            mCharacter->SetState(15);
        }
    }
}

// The stylus goes up: a flick across the face (or head) from beside the character in one of four ways makes it
// react (its states 4, 5, 22, 23); a tap without moving on the head or the face plays a game, by whether it faces
// the camera and its mood; otherwise its head stops following the stylus
void Scene40TouchPage::UpdateRelease() {
    BOOL handled = FALSE;
    if (((mTouch->mPartsTouched & Scene40CharacterTouch::PART_NONE) &&
         (mTouch->mPartsTouched & Scene40CharacterTouch::PART_FACE)) ||
        ((mTouch->mPartsTouched & Scene40CharacterTouch::PART_NONE) &&
         (mTouch->mPartsTouched & Scene40CharacterTouch::PART_HEAD))) {
        ScreenPoint velocity = mTracker->mVelocity;
        switch (mTouch->mStroke) {
        case 1:
            if (mCharacter->mState != 4 && velocity.y < -5) {
                mCharacter->SetState(4);
                mCharacter->AddUnknown1C4Once0();
                handled = TRUE;
            }
            break;
        case 2:
            if (velocity.y > 5 && mCharacter->mState != 6 && mCharacter->mState != 13 && mCharacter->mState != 14 &&
                mCharacter->mState != 15) {
                mCharacter->SetState(5);
                mCharacter->AddUnknown1C4Once0();
                handled = TRUE;
            }
            break;
        case 3:
            if (velocity.x < -5) {
                mCharacter->SetState(22);
                mCharacter->AddUnknown1C4Once0();
                handled = TRUE;
            }
            break;
        case 4:
            if (velocity.x > 5) {
                mCharacter->SetState(23);
                mCharacter->AddUnknown1C4Once0();
                handled = TRUE;
            }
            break;
        }
    } else if (mTouch->mPart & Scene40CharacterTouch::PART_HEAD) {
        ScreenPoint velocity = mTracker->mVelocity;
        if (MATH_ABS(velocity.x) + MATH_ABS(velocity.y) < 2 && !mMenuHidden) {
            mCharacter->UpdateCameraAngle();
            if (mCharacter->IsFacingCameraLoosely()) {
                if (IsCharacterNotIn2Or24Or25()) {
                    PlayGame3B();
                    handled = TRUE;
                }
            } else if (IsCharacterMood0Or1() && IsCharacterNotIn2Or24Or25()) {
                PlayGame87();
                handled = TRUE;
            }
        }
    } else if (mTouch->mPart & Scene40CharacterTouch::PART_FACE) {
        ScreenPoint velocity = mTracker->mVelocity;
        if (MATH_ABS(velocity.x) + MATH_ABS(velocity.y) < 1 && !mMenuHidden) {
            mCharacter->UpdateCameraAngle();
            if (mCharacter->IsFacingCameraLoosely()) {
                if (IsCharacterMood0Or1() && IsCharacterNotIn2Or24Or25()) {
                    PlayGame();
                    handled = TRUE;
                }
            } else if (IsCharacterMood0Or1() && IsCharacterNotIn2Or24Or25()) {
                PlayGame87();
                handled = TRUE;
            }
        }
    }
    if (!handled) {
        if (mCharacter->mState == 11) {
            mCharacter->SetState(10);
        } else if (mCharacter->mState == 12) {
            mCharacter->SetState(0);
        } else if (mCharacter->mState == 14) {
            mCharacter->SetState(13);
        } else if (mCharacter->mState == 15) {
            mCharacter->SetState(6);
        }
    }
    mContext->mSprite->mVisible = FALSE;
}

// Does nothing (the idle state calls it last)
void Scene40TouchPage::Unused02151ab0() {}

void Scene40TouchPage::UpdateWaitIdle() {
    if (mCharacter->mState != 0) {
        return;
    }
    mMenu->SetDisabled(FALSE);
    SetState(STATE_IDLE);
}

void Scene40TouchPage::UpdateWait9() {
    if (mCharacter->mState != 17) {
        return;
    }
    mCharacter->ClearUnknown246();
    if (mUnknown60) {
        SetState(STATE_10);
        return;
    }
    mMenu->SetRightDisabled(FALSE);
    SetState(STATE_7);
}

void Scene40TouchPage::UpdateWaitMessages() {
    Scene40MessageSequence *sequence = mContext->mMessageSequence;
    if (sequence->mLastState != Scene40MessageSequence::STATE_DONE) {
        return;
    }
    mState = sequence->mReturnState;
    mFrames = 0;
}

void Scene40TouchPage::UpdateMessages0() {
    switch (mStep) {
    case 0:
        if (mContext->mCharacter->mState != 0) {
            return;
        }
        mContext->mMessageSequence->mReturnState = STATE_MESSAGES_0;
        mState = STATE_WAIT_MESSAGES;
        mContext->mMessageSequence->mEntry = 0;
        mContext->mMessageSequence->mState = Scene40MessageSequence::STATE_START;
        mStep++;
        break;
    case 1:
        SetState(STATE_IDLE);
        break;
    }
}

void Scene40TouchPage::UpdateMessages4() {
    switch (mStep) {
    case 0:
        if (mContext->mCharacter->mState != 0) {
            return;
        }
        mMenu->Choose(0);
        mContext->mMessageSequence->mReturnState = STATE_MESSAGES_4;
        mState = STATE_WAIT_MESSAGES;
        mContext->mMessageSequence->mEntry = 4;
        mContext->mMessageSequence->mState = Scene40MessageSequence::STATE_START;
        mStep++;
        break;
    case 1:
        SetState(STATE_IDLE);
        break;
    }
}

void Scene40TouchPage::SetState(int state) {
    mState = state;
    mStep = 0;
    mFrames = 0;
    mUnknown10 = 0;
}

// Coming back from a scene that gives the character something (sPresentScenes): the gift, taken by one of the
// scene's item models, placed a little to the side of the character and in front of it, and held there
void Scene40TouchPage::PlaceGift() {
    int index = FindPresentScene(gSceneManager.mPreviousSceneId);
    mUnknown60 = FALSE;
    if (sPresentScenes[index].mGiftKind == 2) {
        return;
    }
    mUnknown60 = TRUE;
    int kind = 1;
    if (sPresentScenes[index].mGiftKind == 0) {
        kind = 0;
    }
    if (!sPresentScenes[index].mFromMemory) {
        mGift.Set(kind, sPresentScenes[index].mItem);
    } else {
        mGift.Set(kind, gSceneMemory.mUnknown2C[0]);
    }
    mContext->mPhysics->TakeFreeItem(&mContext->mItemModels);
    mContext->mItemModels->ChangeItem(mGift.mMessage);
    mContext->mItemModels->mVisible = TRUE;
    mContext->mItemModels->mMotion.mHeld = TRUE;
    mContext->mItemModels->mMotion.mOffered = TRUE;

    VecFx32 placed, sideUnit, front, side;
    const VecFx32 up = {0, FX32_ONE, 0};
    VecFx32 toCharacter;
    VecFx32 position = mCharacter->CharacterModel::mPos;
    VEC_Subtract(&position, &gCamera.mEye, &toCharacter);
    VEC_CrossProduct(&up, &toCharacter, &side);
    VEC_Normalize(&side, &sideUnit);
    ScaleVec(FX32_ONE, &sideUnit, &sideUnit);
    VEC_Add(&sideUnit, &position, &placed);
    VEC_Normalize(&toCharacter, &front);
    ScaleVec(-FX32_CONST(1.2), &front, &front);
    VEC_Add(&front, &placed, &placed);
    placed.y = 0;
    SetVecFields(&mContext->mItemModels->mPosition, placed);
}

BOOL Scene40TouchPage::IsCharacterMood0Or1() {
    return SavedCharacter::Is0Or1(mCharacter->mMood);
}

BOOL Scene40TouchPage::IsCharacterNotIn2Or24Or25() {
    if (mCharacter->mState == 2 || mCharacter->mState == 24 || mCharacter->mState == 25) {
        return FALSE;
    }
    return TRUE;
}

// One of the games: by the character's game (scenes 0x82 to 0x86), or 0xB7 or 0xB9 at even odds
void Scene40TouchPage::PlayGame() {
    MI_CpuFill8(&gSceneScratch, 0, 100);
    BOOL requested = FALSE;
    GetSceneMemory().mNextMode = 0;
    if (mCharacter->mUnknown24E != 0xFF) {
        requested = TRUE;
        switch (mCharacter->mUnknown24E) {
        case 0:
            gSceneManager.RequestScene(0x82);
            break;
        case 2:
            gSceneManager.RequestScene(0x83);
            break;
        case 3:
            gSceneManager.RequestScene(0x84);
            break;
        case 4:
            gSceneManager.RequestScene(0x85);
            break;
        case 5:
            gSceneManager.RequestScene(0x86);
            break;
        default:
            requested = FALSE;
            break;
        }
    }
    if (!requested) {
        if ((int)MATH_Rand16(&gRandom, 1000) < 500) {
            gSceneManager.RequestScene(0xB7);
        } else {
            gSceneManager.RequestScene(0xB9);
        }
    }
    mCharacter->AddUnknown1C4Once0();
}

void Scene40TouchPage::PlayGame3B() {
    MI_CpuFill8(&gSceneScratch, 0, 100);
    GetSceneMemory().mNextMode = 0;
    gSceneManager.RequestScene(0x3B);
    mCharacter->AddUnknown1C4Once0();
}

void Scene40TouchPage::PlayGame87() {
    MI_CpuFill8(&gSceneScratch, 0, 100);
    GetSceneMemory().mNextMode = 0;
    if (mCharacter->mUnknown24E == 1) {
        gSceneManager.RequestScene(0x87);
    } else {
        gSceneManager.RequestScene(0xB8);
    }
    mCharacter->AddUnknown1C4Once0();
}

// The index of a scene in sPresentScenes, or -1
int Scene40TouchPage::FindPresentScene(int sceneId) {
    int index = -1;
    for (int i = 0; i < 20; i++) {
        if (sceneId == sPresentScenes[i].mSceneId) {
            index = i;
        }
    }
    return index;
}

void Scene40TouchPage::Draw() {}

// --- The scene's character ------------------------------------------------------------------------------------------

// With speaking set, the lips follow the scene's speech
void Scene40Character::Update(BOOL speaking) {
    Character::Update();
    PlaceHeadIcons();
    mHeadIcons[0].Update();
    mHeadIcons[1].Update();
    mBalloon.Update();
    if (!speaking) {
        return;
    }
    UpdateState();
    if (mTtsPlayer->IsEngineIdle()) {
        return;
    }
    if (!mLipSync) {
        return;
    }
    LipSync(mTtsPlayer->GetPhonemeName(), 1);
}

// objdiff scores this 99.5% though its instructions and relocations equal the original's (checked with objdump
// against the delinked object; check_relocs lists nothing): the two functions it calls are byte for byte the same,
// and the report seems to pair one call with the other's.
void Scene40Character::Draw() {
    Character::Draw();
    mBalloon.Draw();
    mCover.Draw();
}

// --- The scene's list: the shops' item list with a mark beside each row ------------------------------------------

void Scene40ListView::Draw() {
    for (int i = 0; i < mRowCount; i++) {
        Sprite_Draw(&mMarks[i]);
    }
    ItemListView::Draw();
}

void Scene40ListView::SetVisible(BOOL visible) {
    ItemListView::SetVisible(visible);
    for (int i = 0; i < mRowCount; i++) {
        mMarks[i].mVisible = visible;
    }
}

// A row's mark shows that the character has had the row's item: a bit of its record for each
void Scene40ListView::DrawRow(int row, int item) {
    ItemListView::DrawRow(row, item);
    BOOL visible = TRUE;
    switch (mMarkKind) {
    case MARK_NONE:
        visible = FALSE;
        break;
    case MARK_ITEMS:
        visible = mCharacter->mUnknown1C8.m[mRowItemIds[row] >> 5] & (1 << (mRowItemIds[row] & 31));
        break;
    case MARK_CLOTHES:
        visible = mCharacter->mOwnedClothes.m[mRowItemIds[row] >> 5] & (1 << (mRowItemIds[row] & 31));
        break;
    case MARK_INTERIORS:
        visible = mCharacter->mOwnedInteriors[mRowItemIds[row] >> 5] & (1 << (mRowItemIds[row] & 31));
        break;
    }
    mMarks[row].mVisible = visible;
}

void Scene40ListView::ClearRow(int row) {
    ItemListView::ClearRow(row);
    mMarks[row].mVisible = FALSE;
}

// --- The scene's clock ------------------------------------------------------------------------------------------

Scene40Clock::Scene40Clock() {
    MI_CpuClear32(&mDate, sizeof(mDate));
    MI_CpuClear32(&mTime, sizeof(mTime));
    mSeconds = 0;
}

void Scene40Clock::Update() {
    mSeconds = Clock_GetDateTime(gGameClock[0], &mDate, &mTime);
}

void Scene40Clock::UpdateIfArchiveIdle() {
    if (!ArchiveManager_IsIdle(&gArchiveManager)) {
        return;
    }
    mSeconds = Clock_GetDateTime(gGameClock[0], &mDate, &mTime);
}

extern "C" {
void ParticleSystem_Draw(ParticleSystem *particles, int unknown); // Draws the particles
void GameData_WriteCharacter(GameData *game, int index, SavedCharacter *character); // Writes a saved character
void DestroyMainFrameHeap(void);
BOOL ArchiveManager_IsIdle(ArchiveLoader *loader);
extern int gGameClock[];
s64 Clock_GetDateTime(int offset, RTCDate *date, RTCTime *time); // The date and time now, and the same in seconds

BOOL SpriteButton_IsTouched(SpriteButton *button);                // Whether the stylus came down on it
void SpriteButton_SetDisabled(SpriteButton *button, BOOL disabled);
BOOL func_02031d00(SpriteButton *button);
void TextButton_SetOverwritePalette(TextButton *button, int palette); // Of the button and its text
int MiiData_GetFavoriteColor(const CharacterRecord *mii);
GXRgb NFL_GetFavoriteColor(int index);
void Camera_UpdateEye(Camera *camera);                         // Whether it is selected (mUnknownB0)

// Not decompiled yet
// Of ov075-c
// Of ov075-e
int func_ov075_02176454(Scene40Character *character); // Chooses one of gStr_S_ov075
}

extern MATHRandContext16 gRandom;



// A game the talk page can go to (gStr_S_ov075, 0x0217BBC0; 0x02176454 chooses one)
struct Scene40Game {
    /* 0x0 */ int mSceneId;
    /* 0x4 */ u8 mUnknown4[4];
};

// 0x8
extern "C" const Scene40Game gStr_S_ov075[];

// Where on the screen the mannequin and an item model are shown
static const ScreenPoint sMannequinScreenPoint = {190, 172};

static const ScreenPoint sItemScreenPoint = {190, 134};


void AddTroublesSolved(int count);

extern SaveHeader gIslandState;

// The body's place and size. The mannequin comes last: an inlined call binds its arguments last to first, and the
// original reads the pointer before it computes the size (as a member function, `this` binds last and does not match)
static inline void SetBodyPosition(Mannequin *body, VecFx32 position) {
    body->mPos.x = position.x;
    body->mPos.y = position.y;
    body->mPos.z = position.z;
}

static inline void SetBodyScaleX(fx32 x, Mannequin *body) {
    body->mScale.x = x;
    body->mInvScale.x = FX_Inv(x);
}

static inline void SetBodyScaleY(fx32 y, Mannequin *body) {
    body->mScale.y = y;
    body->mInvScale.y = FX_Inv(y);
}

static inline void SetBodyScaleZ(fx32 z, Mannequin *body) {
    body->mScale.z = z;
    body->mInvScale.z = FX_Inv(z);
}

// --- The talk page (pages+0x180) ----------------------------------------------------------------------------------

void Scene40TalkPage::Init(Scene40Context *context, int *charOffset, Scene40MenuBar *menuBar,
                         Scene40ProfilePage *profilePage) {
    mContext = context;
    mChoices.Init(charOffset, mContext->mTroubleMessages);
    mMenuBar = menuBar;
    mProfilePage = profilePage;
    mActive = TRUE;
    mState = mNextState = STATE_NONE;
    mChoice = 0;
    mUnknown1C10 = TRUE;
}

void Scene40TalkPage::Update() {
    switch (mState = mNextState) {
    case STATE_NONE:
        break;
    case STATE_START:
        UpdateStart();
        break;
    case STATE_ANSWER:
        UpdateAnswer();
        break;
    case STATE_SECOND_ANSWER:
        UpdateSecondAnswer();
        break;
    case STATE_FIRST_OF_TWO:
        UpdateFirstOfTwo();
        break;
    case STATE_SECOND_OF_TWO:
        UpdateSecondOfTwo();
        break;
    case STATE_YES_NO:
        UpdateYesNo();
        break;
    case STATE_TROUBLE_ANSWER:
        UpdateTroubleAnswer();
        break;
    case STATE_WAIT_QUESTION:
        UpdateWaitQuestion();
        break;
    case STATE_RELATION_ANSWER:
        UpdateRelationAnswer();
        break;
    case STATE_SOLVE_OR_NOT:
        UpdateSolveOrNot();
        break;
    case STATE_PLAY_OR_NOT:
        UpdatePlayOrNot();
        break;
    case STATE_FOUR_ANSWERS:
        UpdateFourAnswers();
        break;
    case STATE_WAIT_MESSAGES:
        UpdateWaitMessages();
        break;
    case STATE_WAIT_CHARACTER:
        UpdateWaitCharacter();
        break;
    case STATE_LEAVE_A:
        UpdateLeaveA();
        break;
    case STATE_LEAVE_B:
        UpdateLeaveB();
        break;
    case STATE_DONE:
        break;
    }
}

// The topic selects the character's response and the next set of answers.
void Scene40TalkPage::UpdateStart() {
    switch (mTopic) {
    case 0: {
        mContext->mCharacter->SetState(0x86);
        Scene40Character *character = mContext->mCharacter;
        if (character->IsInTrouble() && GetTrouble(character)->kind == 18) {
            if (!mContext->mCharacter->mUnknown2ED0) {
                mNextState = STATE_WAIT_QUESTION;
            } else {
                mResult = 0;
                mNextState = STATE_DONE;
            }
        } else {
            mChoices.SetChoicesForTrouble(mContext->mCharacter->mTroubleCategory, mContext->mCharacter->mTroubleId);
            if (!mContext->mCharacter->mUnknown2ED0) {
                mChoices.Open();
                mNextState = STATE_ANSWER;
            } else {
                mResult = 0;
                mNextState = STATE_DONE;
            }
        }
        break;
    }
    case 1:
        mContext->mCharacter->SetState(0xAF);
        mChoices.SetChoices(7);
        mChoices.Open();
        mNextState = STATE_SOLVE_OR_NOT;
        break;
    case 2:
        mContext->mCharacter->SetState(0xB3);
        mChoices.SetChoices(8);
        mChoices.Open();
        mNextState = STATE_PLAY_OR_NOT;
        break;
    case 3:
        mContext->mCharacter->SetState(0x68);
        mChoices.SetChoices(9);
        mChoices.Open();
        mNextState = STATE_FOUR_ANSWERS;
        break;
    case 4:
        switch (gSceneMemory.mTutorialStep) {
        case 9:
            mContext->mCharacter->SetState(0xC7);
            mStep = 0;
            mNextState = STATE_LEAVE_A;
            break;
        case 17:
            mContext->mCharacter->SetState(0xCB);
            mStep = 0;
            mNextState = STATE_LEAVE_B;
            break;
        case 22:
            mContext->mCharacter->ChooseAfterTouch();
            mContext->mCharacter->SetState(0xCE);
            if (!mUnknown1C10) {
                mResult = 0;
            } else {
                mResult = 10;
                mUnknown1C10 = FALSE;
            }
            mNextState = STATE_DONE;
            break;
        }
        break;
    }
}

void Scene40TalkPage::UpdateAnswer() {
    mChoices.Update();
    if (mChoices.mPressed != -1) {
        mChoices.mAnswer = mChoices.mPressed;
        mChoices.Close();
        mChoice = mChoices.mPressed;
        return;
    }
    if (!mChoices.mClosed) {
        return;
    }
    switch (mChoices.mTroubleKind) {
    case 1:
        switch (mChoices.mAnswer) {
        case 0:
            mContext->mCharacter->SetState(0);
            mContext->mSpeechWindow->Hide();
            mResult = 1;
            mNextState = STATE_DONE;
            break;
        case 1:
            mContext->mCharacter->SetState(0);
            mContext->mSpeechWindow->Hide();
            mResult = 0;
            mNextState = STATE_DONE;
            break;
        }
        break;
    case 3:
        switch (mChoices.mAnswer) {
        case 0:
            mContext->mCharacter->SetState(0);
            mContext->mSpeechWindow->Hide();
            mResult = 1;
            mNextState = STATE_DONE;
            break;
        case 1:
            mContext->mCharacter->SetState(0);
            mContext->mSpeechWindow->Hide();
            mResult = 0;
            mNextState = STATE_DONE;
            break;
        }
        break;
    case 4:
        switch (mChoices.mAnswer) {
        case 0:
            if ((int)MATH_Rand16(&gRandom, 200) < 100) {
                mChoices.SetChoices(18);
                mContext->mCharacter->SetState(0x90);
            } else {
                mChoices.SetChoices(19);
                mContext->mCharacter->SetState(0x91);
            }
            mChoices.Open();
            mNextState = STATE_SECOND_ANSWER;
            break;
        case 1:
            mContext->mCharacter->SetState(0);
            mContext->mSpeechWindow->Hide();
            mResult = 0;
            mNextState = STATE_DONE;
            break;
        }
        break;
    case 5:
        switch (mChoices.mAnswer) {
        case 0:
            mChoices.SetChoices(13);
            mChoices.Open();
            mContext->mCharacter->SetState(0x95);
            mNextState = STATE_FIRST_OF_TWO;
            break;
        case 1:
            mChoices.SetChoices(16);
            mChoices.Open();
            mContext->mCharacter->SetState(0x9C);
            mNextState = STATE_TROUBLE_ANSWER;
            break;
        }
        break;
    case 8:
        switch (mChoices.mAnswer) {
        case 0:
            mContext->mCharacter->SetState(0xA2);
            mContext->mCharacter->StartTroubleProgress();
            mMenuBar->StartCall();
            mResult = 0;
            mNextState = STATE_DONE;
            break;
        case 1:
            mContext->mCharacter->SetState(0);
            mContext->mSpeechWindow->Hide();
            mResult = 0;
            mNextState = STATE_DONE;
            break;
        }
        break;
    case 9:
        switch (mChoices.mAnswer) {
        case 0:
            mContext->mCharacter->SetState(0xA7);
            break;
        case 1:
            mContext->mCharacter->SetState(0xA8);
            break;
        }
        mProfilePage->SetSwitchable(FALSE);
        break;
    case 10:
        switch (mChoices.mAnswer) {
        case 0:
            mContext->mCharacter->SetState(0xA7);
            break;
        case 1:
            mContext->mCharacter->SetState(0xA8);
            break;
        }
        mProfilePage->SetSwitchable(FALSE);
        break;
    case 12:
        switch (mChoices.mAnswer) {
        case 0:
            mContext->mSpeechWindow->Hide();
            mResult = 2;
            mNextState = STATE_DONE;
            break;
        case 1:
            mContext->mCharacter->SetState(0);
            mContext->mSpeechWindow->Hide();
            mResult = 0;
            mNextState = STATE_DONE;
            break;
        }
        break;
    case 13:
        switch (mChoices.mAnswer) {
        case 0:
            mContext->mCharacter->SetState(0);
            mContext->mSpeechWindow->Hide();
            mResult = 1;
            mNextState = STATE_DONE;
            break;
        case 1:
            mContext->mCharacter->SetState(0);
            mContext->mSpeechWindow->Hide();
            mResult = 0;
            mNextState = STATE_DONE;
            break;
        }
        break;
    case 17:
        switch (mChoices.mAnswer) {
        case 0:
            MI_CpuFill8(&gSceneScratch, 0, 100);
            GetSceneMemory().mTalkAnswers[0] = 1;
            gSceneManager.RequestScene(sReturnScenesA[GetTrouble(mContext->mCharacter)->flags]);
            break;
        case 1:
            mContext->mCharacter->SetState(0);
            mContext->mSpeechWindow->Hide();
            mResult = 0;
            mNextState = STATE_DONE;
            break;
        }
        break;
    case 20:
        if (mChoices.mAnswer != 0) {
            return;
        }
        mContext->mCharacter->ClearTrouble();
        gIslandState.SetUnknown44(4, 3);
        gSceneManager.RequestScene(0x5F);
        break;
    }
}

void Scene40TalkPage::UpdateSecondAnswer() {
    mChoices.Update();
    if (mChoices.mPressed != -1) {
        MI_CpuFill8(&gSceneScratch, 0, 100);
        GetSceneMemory().mTalkAnswers[1] = mChoices.mPressed;
        mChoices.Close();
        mChoice = mChoices.mPressed;
        return;
    }
    if (!mChoices.mClosed) {
        return;
    }
    mContext->mCharacter->SetState(0x92);
    mResult = 3;
    mNextState = STATE_DONE;
}

void Scene40TalkPage::UpdateFirstOfTwo() {
    mChoices.Update();
    if (mChoices.mPressed != -1) {
        MI_CpuFill8(&gSceneScratch, 0, 100);
        GetSceneMemory().mTalkAnswers[0] = mChoices.mPressed;
        mChoices.Close();
        mChoice = mChoices.mPressed;
        return;
    }
    if (!mChoices.mClosed) {
        return;
    }
    mContext->mCharacter->SetState(0x96);
    mChoices.SetChoices(14);
    mChoices.Open();
    mNextState = STATE_SECOND_OF_TWO;
}

void Scene40TalkPage::UpdateSecondOfTwo() {
    mChoices.Update();
    if (mChoices.mPressed != -1) {
        GetSceneMemory().mTalkAnswers[1] = mChoices.mPressed;
        mChoices.Close();
        mChoice = mChoices.mPressed;
        return;
    }
    if (!mChoices.mClosed) {
        return;
    }
    mContext->mCharacter->SetState(0x97);
    mChoices.SetChoices(15);
    mChoices.Open();
    mNextState = STATE_YES_NO;
}

void Scene40TalkPage::UpdateYesNo() {
    mChoices.Update();
    if (mChoices.mPressed != -1) {
        mChoices.mAnswer = mChoices.mPressed;
        mChoices.Close();
        mChoice = mChoices.mPressed;
        return;
    }
    if (!mChoices.mClosed) {
        return;
    }
    switch (mChoices.mAnswer) {
    case 0:
        mResult = 5;
        mNextState = STATE_DONE;
        break;
    case 1:
        mContext->mCharacter->SetState(0x98);
        mResult = 4;
        mNextState = STATE_DONE;
        break;
    }
    mProfilePage->SetSwitchable(FALSE);
}

void Scene40TalkPage::UpdateTroubleAnswer() {
    mChoices.Update();
    if (mChoices.mPressed != -1) {
        mChoices.mAnswer = mChoices.mPressed;
        mChoices.Close();
        mChoice = mChoices.mPressed;
        return;
    }
    if (!mChoices.mClosed) {
        return;
    }
    int state = 0x9D;
    switch (mChoices.mAnswer) {
    case 0:
        state = 0x9D;
        break;
    case 1:
        state = 0x9E;
        break;
    case 2:
        state = 0x9F;
        break;
    case 3:
        state = 0xA0;
        break;
    }
    mContext->mCharacter->SetState(state);
    s64 now = mContext->mClock->mSeconds;
    u16 target = mContext->mCharacter->mTroubleTarget;
    mContext->mCharacter->ClearTrouble();
    mContext->mCharacter->mTroubleCategory = 4;
    mContext->mCharacter->mTroubleId = 3;
    mContext->mCharacter->mTroubleState = 1;
    mContext->mCharacter->mTroubleTarget = target;
    Scene40Character *character = mContext->mCharacter;
    character->mTroubleEnd = now + GetTrouble(character)->minutes * 60;
    mContext->mCharacter->mMood = 3;
    mContext->mCharacter->ResetTroubleProgress();
    mContext->mCharacter->ClearLongActOfPair();
    mContext->mUnknown18 = -100;
    mResult = 9;
    mNextState = STATE_DONE;
}

void Scene40TalkPage::UpdateWaitQuestion() {
    if (mContext->mCharacter->mState != 0xAC) {
        return;
    }
    mContext->mCharacter->SetState(0xAD);
    mChoices.SetSpecialChoices(mContext->mCharacter->mUnknown2EE0[0]);
    mChoices.Open();
    mNextState = STATE_RELATION_ANSWER;
}

// The answer moves the character's relation meter with the trouble's target up or down by 40
void Scene40TalkPage::UpdateRelationAnswer() {
    mChoices.Update();
    if (mChoices.mPressed != -1) {
        mChoices.mAnswer = mChoices.mPressed;
        mChoices.Close();
        mChoice = mChoices.mPressed;
        mContext->mCharacter->mUnknown2EE0[2] = mChoices.mPressed;
        Scene40Character *character = mContext->mCharacter;
        int answer = mChoices.mPressed;
        u16 target = character->mTroubleTarget;
        int meter = character->mRelationMeter[target];
        int value;
        switch (answer) {
        case 0:
            value = meter + 40;
            break;
        case 1:
            value = meter;
            break;
        case 2:
            value = meter - 40;
            break;
        }
        character->mRelationMeter[target] = value > 200 ? 200 : value < 0 ? 0 : value;
        mProfilePage->mRelationsChanged = TRUE;
        return;
    }
    if (!mChoices.mClosed) {
        return;
    }
    mContext->mCharacter->SetState(0xAE);
    Scene40Character *character = mContext->mCharacter;
    character->mTroubleEnd = mContext->mClock->mSeconds + GetTrouble(character)->cooldownMinutes * 60;
    mContext->mCharacter->mTroubleState = 3;
    AddTroublesSolved(1);
    mContext->mUnknown18 = GetTrouble(mContext->mCharacter)->unk4Bits0;
    mNextState = STATE_WAIT_CHARACTER;
}

void Scene40TalkPage::UpdateSolveOrNot() {
    mChoices.Update();
    if (mChoices.mPressed != -1) {
        mChoices.mAnswer = mChoices.mPressed;
        mChoices.Close();
        mChoice = mChoices.mPressed;
        return;
    }
    if (!mChoices.mClosed) {
        return;
    }
    switch (mChoices.mAnswer) {
    case 0: {
        mContext->mCharacter->SetState(0xB0);
        Scene40Character *character = mContext->mCharacter;
        character->mTroubleEnd = mContext->mClock->mSeconds + GetTrouble(character)->cooldownMinutes * 60;
        mContext->mCharacter->mTroubleState = 3;
        AddTroublesSolved(1);
        mContext->mUnknown18 = GetTrouble(mContext->mCharacter)->unk4Bits0;
        mNextState = STATE_WAIT_CHARACTER;
        break;
    }
    case 1:
        mContext->mSpeechWindow->Hide();
        mResult = 2;
        mNextState = STATE_DONE;
        break;
    case 2:
        mContext->mCharacter->SetState(0);
        mContext->mSpeechWindow->Hide();
        mResult = 0;
        mNextState = STATE_DONE;
        break;
    }
}

// Yes goes to one of the games, chosen by the character, with the character for it
void Scene40TalkPage::UpdatePlayOrNot() {
    mChoices.Update();
    if (mChoices.mPressed != -1) {
        mChoices.mAnswer = mChoices.mPressed;
        mChoices.Close();
        mChoice = mChoices.mPressed;
        return;
    }
    if (!mChoices.mClosed) {
        return;
    }
    switch (mChoices.mAnswer) {
    case 0:
        MI_CpuFill8(&gSceneScratch, 0, 100);
        GetSceneMemory().mTalkAnswers[0] = 2;
        GetSceneMemory().mCharacters[0] = mContext->mCharacter->mIndex;
        gSceneManager.RequestScene(gStr_S_ov075[func_ov075_02176454(mContext->mCharacter)].mSceneId);
        break;
    case 1:
        mContext->mCharacter->SetState(0);
        mContext->mSpeechWindow->Hide();
        mResult = 0;
        mNextState = STATE_DONE;
        break;
    }
}

void Scene40TalkPage::UpdateFourAnswers() {
    mChoices.Update();
    if (mChoices.mPressed != -1) {
        mChoices.mAnswer = mChoices.mPressed;
        mChoices.Close();
        mChoice = mChoices.mPressed;
        return;
    }
    if (!mChoices.mClosed) {
        return;
    }
    switch (mChoices.mAnswer) {
    case 0:
        mContext->mCharacter->SetState(0x69);
        break;
    case 1:
        mContext->mCharacter->SetState(0x6A);
        break;
    case 2:
        mContext->mCharacter->SetState(0x6B);
        break;
    case 3:
        mContext->mCharacter->SetState(0x6C);
        break;
    }
    Scene40Character *character = mContext->mCharacter;
    if (character->IsInTrouble() && GetTrouble(character)->kind == 13) {
        character = mContext->mCharacter;
        character->mTroubleEnd = mContext->mClock->mSeconds + GetTrouble(character)->cooldownMinutes * 60;
        mContext->mCharacter->mTroubleState = 3;
        mContext->mUnknown18 = GetTrouble(mContext->mCharacter)->unk4Bits0;
        AddTroublesSolved(1);
    } else {
        mContext->mUnknown18 = 30;
    }
    mResult = 7;
    mNextState = STATE_DONE;
}

void Scene40TalkPage::UpdateWaitMessages() {
    Scene40MessageSequence *sequence = mContext->mMessageSequence;
    if (sequence->mLastState == Scene40MessageSequence::STATE_DONE) {
        mNextState = sequence->mReturnState;
    }
}

void Scene40TalkPage::UpdateWaitCharacter() {
    if (mContext->mCharacter->mState != 0x33) {
        return;
    }
    mResult = 8;
    mNextState = STATE_DONE;
}

void Scene40TalkPage::UpdateLeaveA() {
    switch (mStep) {
    case 0:
        if (mContext->mCharacter->mState != 0xC9) {
            return;
        }
        mContext->mCharacter->SetState(0);
        mContext->mMessageSequence->mReturnState = STATE_LEAVE_A;
        mNextState = STATE_WAIT_MESSAGES;
        mContext->mMessageSequence->mEntry = 1;
        mContext->mMessageSequence->mState = Scene40MessageSequence::STATE_START;
        mStep++;
        break;
    case 1:
        gSceneManager.RequestScene(0x69);
        break;
    }
}

void Scene40TalkPage::UpdateLeaveB() {
    switch (mStep) {
    case 0:
        if (mContext->mCharacter->mState != 0xCD) {
            return;
        }
        mContext->mCharacter->SetState(0);
        mContext->mMessageSequence->mReturnState = STATE_LEAVE_B;
        mNextState = STATE_WAIT_MESSAGES;
        mContext->mMessageSequence->mEntry = 3;
        mContext->mMessageSequence->mState = Scene40MessageSequence::STATE_START;
        mStep++;
        break;
    case 1:
        gSceneManager.RequestScene(0x69);
        break;
    }
}

void Scene40TalkPage::Draw() {
    mChoices.Draw();
}

// --- The gift page (pages+0x1D94) ---------------------------------------------------------------------------------

void Scene40GiftPage::Init(Scene40Context *context, Scene40CharacterTouch *touch, Scene40ProfilePage *profile,
                           Scene40CounterDisplay *counter) {
    mContext = context;
    mCharacter = context->mCharacter;
    mTouch = touch;
    mProfilePage = profile;
    mCounter = counter;
    mLucky = FALSE;
    mGotTarget = FALSE;
    mUnknown68 = -1;
    mUnknown6C = 0;
    mUnknown70 = 0;
    mItem = -1;
    mValue = 0;
    mActive = TRUE;
    mNextState = STATE_NONE;
    mState = STATE_NONE;
    mUnknown08 = 0;
    mUnknown0C = 0;
    mStep = 0;
    mReaction = 0;
}

void Scene40GiftPage::Update() {
    switch (mState = mNextState) {
    case STATE_NONE:
        break;
    case STATE_REACT:
        UpdateReact();
        break;
    case STATE_HOLD_ITEM:
        UpdateHoldItem();
        break;
    case STATE_3:
        Update3();
        break;
    case STATE_4:
        Update4();
        break;
    case STATE_5:
        Update5();
        break;
    case STATE_6:
        Update6();
        break;
    case STATE_7:
        Update7();
        break;
    case STATE_8:
        Update8();
        break;
    case STATE_9:
        break;
    case STATE_WAIT_MESSAGES:
        UpdateWaitMessages();
        break;
    case STATE_LEAVE_A:
        UpdateLeaveA();
        break;
    case STATE_HOLD_THEN_B:
        UpdateHoldThenB();
        break;
    case STATE_LEAVE_B:
        UpdateLeaveB();
        break;
    case STATE_RESET:
        mNextState = STATE_NONE;
        mLucky = FALSE;
        mGotTarget = FALSE;
        mUnknown68 = -1;
        mUnknown70 = 0;
        mItem = -1;
        break;
    }
}

// The player holds the item out to the character: it follows the screen point until the stylus comes down
void Scene40GiftPage::UpdateHoldItem() {
    mTouch->Update();
    PlaceItemModel();
    if (!mContext->mTouch->mPressed) {
        return;
    }
    mItemModel->mMotion.mOffered = TRUE;
    gSoundManager.StartSeqArc(1, 10, 3);
    mNextState = STATE_3;
}

void Scene40GiftPage::UpdateWaitMessages() {
    if (mContext->mMessageSequence->mReturnState == STATE_HOLD_THEN_B) {
        mTouch->Update();
        PlaceItemModel();
    }
    Scene40MessageSequence *sequence = mContext->mMessageSequence;
    if (sequence->mLastState == Scene40MessageSequence::STATE_DONE) {
        mNextState = sequence->mReturnState;
        mUnknown08 = 0;
    }
}

void Scene40GiftPage::UpdateLeaveA() {
    switch (mStep) {
    case 0:
        mContext->mMessageSequence->mReturnState = STATE_LEAVE_A;
        mNextState = STATE_WAIT_MESSAGES;
        mContext->mMessageSequence->mEntry = 2;
        mContext->mMessageSequence->mState = Scene40MessageSequence::STATE_START;
        mStep++;
        break;
    case 1:
        mContext->mCharacter->SetState(0xD0);
        mStep++;
        break;
    case 2:
        if (mContext->mCharacter->mState != 0xD1) {
            return;
        }
        gSceneManager.RequestScene(0x69);
        break;
    }
}

void Scene40GiftPage::UpdateHoldThenB() {
    mTouch->Update();
    PlaceItemModel();
    switch (mStep) {
    case 0:
        mContext->mMessageSequence->mReturnState = STATE_HOLD_THEN_B;
        mNextState = STATE_WAIT_MESSAGES;
        mContext->mMessageSequence->mEntry = 5;
        mContext->mMessageSequence->mState = Scene40MessageSequence::STATE_START;
        mStep++;
        break;
    case 1:
        mNextState = STATE_HOLD_ITEM;
        break;
    }
}

void Scene40GiftPage::UpdateLeaveB() {
    switch (mStep) {
    case 0:
        mContext->mMessageSequence->mReturnState = STATE_LEAVE_B;
        mNextState = STATE_WAIT_MESSAGES;
        mContext->mMessageSequence->mEntry = 6;
        mContext->mMessageSequence->mState = Scene40MessageSequence::STATE_START;
        mStep++;
        break;
    case 1:
        gSceneManager.RequestScene(0x69);
        break;
    }
}

void Scene40GiftPage::Draw() {
}

void Scene40GiftPage::SetLucky(BOOL lucky) {
    SetLucky(lucky, 1000);
}

// Lucky with a chance of chance in a thousand
void Scene40GiftPage::SetLucky(BOOL lucky, int chance) {
    if (chance > MATH_Rand16(&gRandom, 1000) && lucky) {
        mLucky = TRUE;
        return;
    }
    mLucky = FALSE;
}

void Scene40GiftPage::SetValue(int value) {
    mValue = value;
}

void Scene40GiftPage::SetItem(int item, int value) {
    mItem = item;
    mValue = value;
}

// Whether the gift solves the character's trouble (kind 1 for goods, 3 for clothes); if so the trouble ends, and with
// a count above 0 it counts as solved and makes the character lucky with a chance of 400 in a thousand
BOOL Scene40GiftPage::SolveTrouble(Scene40Gift *gift, int count) {
    s64 now = mContext->mClock->mSeconds;
    BOOL solved = FALSE;
    switch (gift->mGiftKind) {
    case 0:
        solved = IsInTroubleOfKind(mCharacter, 1);
        break;
    case 1:
        solved = IsInTroubleOfKind(mCharacter, 3);
        break;
    }
    if (solved) {
        Scene40Character *character = mCharacter;
        const TroubleInfo *trouble = GetTrouble(character);
        if (trouble->hasTarget != 0 && gift->mItem.InventoryItem::mId == character->mTroubleTarget) {
            if (trouble->kind == 1 && gift->mGiftKind == 0) {
                mGotTarget = TRUE;
            } else if (trouble->kind == 3 && gift->mGiftKind == 1) {
                mGotTarget = TRUE;
            }
        }
        mCharacter->mTroubleEnd = now + GetTrouble(mCharacter)->cooldownMinutes * 60;
        mCharacter->mTroubleState = 3;
        if (count > 0) {
            AddTroublesSolved(1);
            SetLucky(TRUE, 400);
        }
    }
    return solved;
}

// The character's state for the item it was given (mItem: 0 to 19)
void Scene40GiftPage::StartCharacterReaction() {
    mCharacter->mUnknown2EB4 = mItem;
    switch (mItem) {
    case 18:
        mCharacter->SetState(0x36);
        break;
    case 19:
        mCharacter->SetState(0x37);
        break;
    case 0:
        mCharacter->SetState(0x38);
        break;
    case 1:
        mCharacter->SetState(0x39);
        break;
    case 2:
        mCharacter->SetState(0x3A);
        break;
    case 3:
        mCharacter->SetState(0x3B);
        break;
    case 4:
        mCharacter->SetState(0x3C);
        break;
    case 5:
        mCharacter->SetState(0x3D);
        break;
    case 6:
        mCharacter->SetState(0x3E);
        break;
    case 7:
        mCharacter->SetState(0x3F);
        break;
    case 8:
        mCharacter->SetState(0x40);
        break;
    case 9:
        mCharacter->SetState(0x41);
        break;
    case 10:
        mCharacter->SetState(0x42);
        break;
    case 11:
        mCharacter->SetState(0x43);
        break;
    case 12:
        mCharacter->SetState(0x34);
        break;
    case 17:
        mCharacter->SetState(0xCA);
        break;
    default:
        mCharacter->SetState(0x31);
        break;
    }
}

// The music of the reaction to the item given (mItem), in its sound group
void Scene40GiftPage::PlayReactionMusic() {
    int sequence;
    int group = 107;
    switch (mItem) {
    case 18:
        sequence = 118;
        break;
    case 0:
        sequence = 118;
        break;
    case 1:
        sequence = 119;
        break;
    case 2:
        sequence = 120;
        break;
    case 3:
        sequence = 121;
        break;
    case 4:
        sequence = 122;
        break;
    case 19:
        sequence = 123;
        break;
    case 5:
        sequence = 118;
        break;
    case 6:
        sequence = 119;
        break;
    case 7:
        sequence = 120;
        break;
    case 8:
        sequence = 121;
        break;
    case 9:
        sequence = 122;
        break;
    case 10:
        sequence = 125;
        break;
    case 11:
        sequence = 126;
        break;
    case 13:
        sequence = 119;
        break;
    case 14:
        sequence = 121;
        break;
    case 15:
        sequence = 119;
        break;
    case 16:
        sequence = 121;
        break;
    case 17:
        sequence = 119;
        break;
    default:
        sequence = 120;
        break;
    }
    if (sequence == 125) {
        group = 109;
    } else if (sequence == 126) {
        group = 110;
    }
    gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, 0);
    gSceneManager.SetSoundGroup(group);
    gSoundManager.StartSeq(0, sequence);
    gSoundManager.LoadGroup(77);
}

void Scene40GiftPage::PlaceItemModel() {
    Camera_UpdateEye(&gCamera);
    VecFx32 position;
    ProjectTouchBetween(&sItemScreenPoint, &position, 0x3000, 0x4000);
    mItemModel->SetPosition(position);
}

// --- The present page (pages+0x1E1C) -------------------------------------------------------------------------------

void Scene40PresentPage::Init(Scene40Context *context, Scene40CharacterTouch *touch) {
    mContext = context;
    mCharacter = context->mCharacter;
    mBoxes[0] = context->mModels[0];
    mBoxes[1] = context->mModels[1];
    mBoxes[2] = context->mModels[2];
    mActive = TRUE;
    Sprite_Init(&mSprite, 0, 30, 128, 96, TRUE);
    mSprite.mVisible = FALSE;
    mSprite.mOverwritePalette = 1;
    mTouch = touch;
    mUnknown124 = FALSE;
}

void Scene40PresentPage::Update() {
    mState = mNextState;
    mTouch->Update();
    switch (mState) {
    case STATE_NONE:
        break;
    case STATE_START:
        UpdateStart();
        break;
    case STATE_SHOW_BOXES:
        UpdateShowBoxes();
        break;
    case STATE_CHOOSE:
        UpdateChoose();
        break;
    case STATE_OPEN:
        UpdateOpen();
        break;
    case STATE_RISE:
        UpdateRise();
        break;
    case STATE_GIVE:
        UpdateGive();
        break;
    case STATE_END:
        UpdateEnd();
        break;
    case STATE_WAIT_MESSAGES:
        UpdateWaitMessages();
        break;
    case STATE_DONE:
        mUnknown124 = FALSE;
        break;
    }
}

// Puts an item of each group in the boxes
void Scene40PresentPage::Start() {
    mNextState = STATE_START;
    for (int i = 0; i < 3; i++) {
        mItems[i] = ChooseItemInGroup(1, 4, i);
        if (mContext->mPhysics->TakeFreeItem(&mItemModels[i])) {
            mItemModels[i]->ChangeItem(mItems[i]);
        }
    }
}

void Scene40PresentPage::UpdateStart() {
    mCharacter->SetState(0xB4);
    mNextState = STATE_SHOW_BOXES;
}

// The box layout and scale factors, measured from the scene's read-only data.
extern "C" {
extern const ScreenPoint sPresentCenterPoint;
extern const ScreenPoint sPresentBoxPoints[3];
extern const fx32 sPresentBoxScales[3];
BOOL LineMeetsSphere(const VecFx32 *origin, const VecFx32 *dir, fx32 radius);
extern VecFx32 gTouchLineDirection;
int func_ov075_02179290(Scene40Character *character);
void func_ov075_021793fc(int kind);
}

static inline void SetPresentPosition(Model *model, const VecFx32 &position) {
    model->mPosition.x = position.x;
    model->mPosition.y = position.y;
    model->mPosition.z = position.z;
}

static inline void SetPresentScale(Model *model, fx32 scale) {
    model->mScale.x = scale;
    model->mScale.y = scale;
    model->mScale.z = scale;
}

static inline void SetPresentAnimation(AnimModel *box, const char *name) {
    box->SetAnimation(RESOURCE_BCA0, ResourceSet_FindIndexByName((void *)box->mUnknown04, RESOURCE_BCA0, name));
}

static inline ModelAnimation *GetPresentBoxAnimation(AnimModel *box, const char *name) {
    return AnimModel_FindAnimation(box, RESOURCE_BCA0,
                                  ResourceSet_FindIndexByName((void *)box->mUnknown04, RESOURCE_BCA0, name));
}

// The character has set out the boxes. Project their screen positions into the room and keep their hit radii.
void Scene40PresentPage::UpdateShowBoxes() {
    if (mCharacter->mState != 0xB6) {
        return;
    }
    VecFx32 center;
    MtxFx33 rotation;
    VecFx32 position;
    ProjectTouchBetween(&sPresentCenterPoint, &center, 0x3000, 0x4000);
    MakeFacingMatrixX(&rotation, 0x2C7, 0xFC2);
    for (int i = 0; i < 3; i++) {
        ProjectTouchBetween(&sPresentBoxPoints[i], &position, 0x3000, 0x4000);
        mScale = GetCameraDistanceScale(&center);
        fx32 scale = FX_Mul(mScale, sPresentBoxScales[i]);
        mBoxTouchRadii[i] = FX_Mul(scale, 0x733);
        SetPresentPosition(mBoxes[i], position);
        MI_Copy36B(&rotation, &mBoxes[i]->mRotation);
        SetPresentScale(mBoxes[i], scale);
        SetPresentAnimation(mBoxes[i], "open");
        GetPresentBoxAnimation(mBoxes[i], "open")->mFinished = TRUE;
        mBoxes[i]->mVisible = TRUE;
    }
    gSoundManager.StartSeqArc(1, 10, 9);
    mNextState = STATE_CHOOSE;
}

void Scene40PresentPage::UpdateChoose() {
    if (!gTouchInput.mTouched) {
        return;
    }
    int chosen = -1;
    for (int i = 0; i < 3; i++) {
        VecFx32 offset;
        fx32 radius = mBoxTouchRadii[i];
        VecFx32 position = mBoxes[i]->mPosition;
        position.y += mBoxTouchRadii[i];
        VecFx32 direction = gTouchLineDirection;
        VEC_Subtract(&gCamera.mEye, &position, &offset);
        if (LineMeetsSphere(&offset, &direction, radius)) {
            chosen = i;
            break;
        }
    }
    if (chosen == -1) {
        return;
    }
    if (!mContext->mTtsPlayer->IsEngineIdle()) {
        mContext->mTtsPlayer->StopEngine();
        mCharacter->SetState(0xB7);
    }
    GetPresentBoxAnimation(mBoxes[chosen], "open")->mFinished = FALSE;
    GetPresentBoxAnimation(mBoxes[chosen], "open")->mPlayOnce = TRUE;
    mItemModels[chosen]->mMotion.mHeld = TRUE;
    SetPresentPosition(mItemModels[chosen], mBoxes[chosen]->mPosition);
    mItemModels[chosen]->SetScale(mScale);
    mItemModels[chosen]->mVisible = TRUE;
    gSoundManager.StartSeqArc(1, 10, 10);
    mChosenBox = chosen;
    mNextState = STATE_OPEN;
    mStep = 0;
}

// Open the chosen box first, then the other two; wait for the last opening animation before lifting the items.
void Scene40PresentPage::UpdateOpen() {
    switch (mStep) {
    case 0:
        if (!GetPresentBoxAnimation(mBoxes[mChosenBox], "open")->mFinished) {
            return;
        }
        for (int i = 0; i < 3; i++) {
            if (i == mChosenBox) {
                continue;
            }
            GetPresentBoxAnimation(mBoxes[i], "open")->mFinished = FALSE;
            GetPresentBoxAnimation(mBoxes[i], "open")->mPlayOnce = TRUE;
            mItemModels[i]->mMotion.mHeld = TRUE;
            SetPresentPosition(mItemModels[i], mBoxes[i]->mPosition);
            mItemModels[i]->SetScale(mScale);
            mItemModels[i]->mVisible = TRUE;
            mOpenedBox = i;
        }
        gSoundManager.StartSeqArc(1, 10, 10);
        mStep++;
        break;
    case 1:
        if (!GetPresentBoxAnimation(mBoxes[mOpenedBox], "open")->mFinished) {
            return;
        }
        mFrames = 0;
        mWaitFrames = 20;
        for (int i = 0; i < 3; i++) {
            mItemPoints[i].x = sPresentBoxPoints[i].x;
            mItemPoints[i].y = sPresentBoxPoints[i].y;
        }
        mNextState = STATE_RISE;
        gSoundManager.StartSeqArc(1, 10, 11);
        break;
    }
}

static inline void SetPresentMarkPosition(Sprite *sprite, ScreenPoint point, fx32 size) {
    point.y -= ((size * 5) >> FX32_SHIFT) + 10;
    sprite->SetPosition(FxVec2(point.x << FX32_SHIFT, point.y << FX32_SHIFT));
}

// NOT MATCHING: behavior is transcribed; register allocation and stack copies still differ from the original.
void Scene40PresentPage::UpdateRise() {
    mFrames++;
    VecFx32 position;
    for (int i = 0; i < 3; i++) {
        if (i == mChosenBox) {
            mItemPoints[i].y -= 2;
        } else {
            mItemPoints[i].y--;
        }
        ProjectTouchBetween(&mItemPoints[i], &position, 0x3000, 0x4000);
        SetPresentPosition(mItemModels[i], position);
    }
    int chosen = mChosenBox;
    if (mContext->mInventory->mItems[mItems[chosen]].mOwned == FALSE) {
        fx32 size = (gGoodsTable[mItemModels[chosen]->mId].mScale << FX32_SHIFT) / 10;
        mSprite.mVisible = TRUE;
        SetPresentMarkPosition(&mSprite, mItemPoints[chosen], size);
    }
    if (mFrames < 25) {
        return;
    }
    for (int i = 0; i < 3; i++) {
        mBoxes[i]->mVisible = FALSE;
    }
    mFrames = 0;
    mWaitFrames = 30;
    mNextState = STATE_GIVE;
}

void Scene40PresentPage::UpdateEnd() {
    mFrames++;
    switch (mStep) {
    case 0:
        if (gTouchInput.mTouched) {
            mFrames = 0;
            mStep++;
            gSoundManager.StartSeqArc(1, 26, 13);
            for (int i = 0; i < 3; i++) {
                if (i != mChosenBox) {
                    mItemModels[i]->mVisible = FALSE;
                }
            }
        }
        if (mFrames >= 0x7FFF) {
            mContext->mHeadIcon->SetMode(1);
        }
        break;
    case 1:
        if (mFrames < 35) {
            return;
        }
        gSoundManager.StartSeqArc(1, 26, 14);
        mSprite.mVisible = FALSE;
        for (int i = 0; i < 3; i++) {
            mItemModels[i]->Hide();
            mItemModels[i] = NULL;
        }
        mStep = 3;
        mFrames = 0;
        break;
    case 2:
        break;
    case 3:
        if (mFrames < 45) {
            return;
        }
        mContext->mSpeechWindow->Hide();
        func_ov075_021793fc(func_ov075_02179290(mCharacter));
        gSoundManager.MoveSeqVolume(0, 127, 120);
        gSoundManager.LoadGroup(77);
        mCharacter->SetState(0);
        if (GetGameFlags(0x400)) {
            mNextState = STATE_DONE;
        } else {
            SetGameFlags(0x400, TRUE);
            mContext->mMessageSequence->mReturnState = STATE_END;
            mNextState = STATE_WAIT_MESSAGES;
            mContext->mMessageSequence->mEntry = 20;
            mContext->mMessageSequence->mState = Scene40MessageSequence::STATE_START;
            mStep++;
        }
        break;
    case 4:
        if (mFrames < 30) {
            return;
        }
        mContext->mHeadIcon->SetMode(1);
        mNextState = STATE_DONE;
        break;
    }
}

void Scene40PresentPage::UpdateWaitMessages() {
    Scene40MessageSequence *sequence = mContext->mMessageSequence;
    if (sequence->mLastState == Scene40MessageSequence::STATE_DONE) {
        mNextState = sequence->mReturnState;
        mFrames = 0;
    }
}

void Scene40PresentPage::Draw() {
    for (int i = 0; i < 3; i++) {
        mBoxes[i]->Draw();
    }
    Sprite_Draw(&mSprite);
}

// --- The list page (pages+0x1F44) ----------------------------------------------------------------------------------


void Scene40ListPage::Init(Scene40Context *context, int *charOffset) {
    mContext = context;
    mNextState = STATE_NONE;
    mState = STATE_NONE;
    mMode = MODE_GIVE;
    mTab = 0;
    mActive = TRUE;
    mChosenId = 0;
    mChosen = FALSE;
    mItemCount = 0;
    mSortGoods = FALSE;
    mSortClothes = FALSE;
    mSortInteriors = FALSE;
    mClothesList = context->mClothesList;
    mInventory = context->mInventory;
    mInteriors = context->mInteriors;
    mMannequin = context->mMannequin;
    mGift.Set(0, 0);

    mTitle.InitObjForMessage(mContext->mMiiTouchMessages, 188, &gFonts[0].mFont, 0x1000, *charOffset, 0, 1, 0, 0, 15,
                             NULL);
    mTitle.mMessageFile = mContext->mMiiTouchMessages;
    mTitle.SetPositionFromVec((128 - mTitle.mCanvas.areaWidth * 8 / 2) << FX32_SHIFT,
                              (15 - mTitle.mCanvas.areaHeight * 8 / 2) << FX32_SHIFT);
    mTitle.mSprite.mOverwritePalette = 1;
    mTitle.mHAlign = mTitle.mMessageFile->GetAlignment(mTitle.mMessage);
    *charOffset += mTitle.GetCharCount();
    mTitle.mSprite.mVisible = FALSE;

    mList.Init();
    mList.SetBackground(0x100, 56);
    PaletteLoader_SetBackdrop(&gPaletteLoader, 0x77DC, 0x100);
    mList.InitRows(2, 8, 9, charOffset);
    mList.InitCounts(charOffset);
    mList.InitMarks(2, 36);
    mList.InitUpButton(2, 2, 3);
    mList.InitDownButton(2, 4, 5);
    mList.InitScrollBar(2, 6, 7);
    mList.SetUpScrollBar(2, 6, 7);
    mList.mCharacter = context->mCharacter;
    mList.SetMessageFile(context->mItemMessages);
    mList.SetMode(ItemListView::MODE_COUNT);
    mList.mMarkKind = Scene40ListView::MARK_NONE;
    mList.SetVisible(FALSE);

    mListTabs.Init(2);
    mGoodsTabs.Init(2);
    mClothesTabs.Init(2);

    Sprite_Init(&mOkButton, 2, 10, 128, 96, TRUE);
    SpriteButton_SetSequences(&mOkButton, 10, 11);
    mOkButton.SetPositionFromVec(192 << FX32_SHIFT, 173 << FX32_SHIFT);
    SpriteButton_SetTouchRect(&mOkButton, 72, 30);
    *charOffset += TextButton_SetMessage(&mOkButton, context->mTroubleMessages, 476, &gFonts[2].mFont, 0x1000, *charOffset);
    SpriteButton_Reset(&mOkButton);
    SpriteButton_SetKind(&mOkButton, 4);
    mOkButton.mUnknownDC = TRUE;
    mOkButton.mSounds.mSounds[0] = 10;
    mOkButton.mSounds.mSounds[2] = 0;
    TextButton_SetOverwritePalette(&mOkButton, 1);
    mOkButton.mVisible = FALSE;

    Sprite_Init(&mShopButton, 2, 10, 128, 96, TRUE);
    SpriteButton_SetSequences(&mShopButton, 10, 11);
    mShopButton.SetPositionFromVec(64 << FX32_SHIFT, 173 << FX32_SHIFT);
    SpriteButton_SetTouchRect(&mShopButton, 72, 30);
    *charOffset += TextButton_SetMessage(&mShopButton, context->mTroubleMessages, 478, &gFonts[2].mFont, 0x1000, *charOffset);
    SpriteButton_Reset(&mShopButton);
    SpriteButton_SetKind(&mShopButton, 4);
    mShopButton.mUnknownDC = TRUE;
    mShopButton.mSounds.mSounds[0] = 10;
    mShopButton.mSounds.mSounds[2] = 0;
    TextButton_SetOverwritePalette(&mShopButton, 1);
    mShopButton.mVisible = FALSE;

    Sprite_Init(&mCloseButton, 2, 0, 128, 96, TRUE);
    SpriteButton_SetSequences(&mCloseButton, 0, 1);
    mCloseButton.SetPositionFromVec(242 << FX32_SHIFT, 13 << FX32_SHIFT);
    SpriteButton_SetTouchRect(&mCloseButton, 24, 22);
    SpriteButton_Reset(&mCloseButton);
    SpriteButton_SetKind(&mCloseButton, 4);
    mCloseButton.mUnknownDC = TRUE;
    mCloseButton.mSounds.mSounds[0] = 12;
    mCloseButton.mSounds.mSounds[2] = 0;
    mCloseButton.mOverwritePalette = 1;
    mCloseButton.mVisible = FALSE;
}

void Scene40ListPage::Update() {
    switch (mState = mNextState) {
    case STATE_NONE:
        break;
    case STATE_OPEN:
        Open();
        break;
    case STATE_CHOOSE:
        Choose();
        break;
    case STATE_CHOSEN:
        mNextState = STATE_CLOSE;
        break;
    case STATE_CLOSE:
        Close();
        break;
    }
}

void Scene40ListPage::Open() {
    mContext->mPhysics->TakeFreeItem(&mItemModel);
    mContext->mSpeechWindow->Hide();
    PaletteLoader_SetBackdrop(&gPaletteLoader, 0x77DC, 0x100);
    if (mSortClothes) {
        mClothesList->SortHeldFirst();
        mSortClothes = FALSE;
    }
    if (mSortGoods) {
        mInventory->SortHeldFirst();
        mSortGoods = FALSE;
    }
    if (mSortInteriors) {
        mInteriors->SortHeldFirst();
        mSortInteriors = FALSE;
    }

    switch (mMode) {
    case MODE_GIVE:
    case MODE_GIVE_FROM_SHOP: {
        mTitle.mSprite.mVisible = FALSE;
        mCloseButton.mVisible = TRUE;
        mListTabs.UpdateAvailable(mContext);
        mListTabs.SetVisible(TRUE);
        mListTabs.Select(mTab);
        mOkButton.mVisible = TRUE;
        SpriteButton_SetDisabled(&mOkButton, TRUE);
        NNS_G2dCharCanvasClear(&mOkButton.mText.mCanvas, 0);
        mOkButton.mText.mMessage = 476;
        mOkButton.mText.DrawAll();
        NNS_G2dCharCanvasClear(&mShopButton.mText.mCanvas, 0);
        mShopButton.mText.DrawAll();
        mList.SetMode(ItemListView::MODE_COUNT);
        mList.SetVisible(TRUE);
        mList.mTouchable = TRUE;
        int tab = 0;
        int filterTab = 0;
        if (mMode == MODE_GIVE_FROM_SHOP) {
            mContext->mCharacter->SetState(0);
            u16 *bought = &gSceneScratch.mShopItem;
            int item = *bought;
            int shownTab = mTab;
            if (item != 0xFFFF) {
                switch (shownTab) {
                case 0:
                    filterTab = Scene40GoodsTabs::FindTab(GetGoodsInfo(item)->mFlags);
                    break;
                case 1:
                    break;
                case 2:
                    filterTab = Scene40ClothesTabs::FindTab(GetClothesInfo(item)->mKind);
                    break;
                case 3:
                    break;
                }
            }
            ShowList(shownTab, filterTab);
            if (item != 0xFFFF) {
                int index = mList.FindItem(item);
                if (index != -1) {
                    mList.Select(index);
                    mList.ScrollToSelected();
                    ShowItem(*bought);
                }
            }
        } else {
            Scene40Character *character = mContext->mCharacter;
            if (character->mTroubleState == 2) {
                switch (character->mTroubleCategory) {
                case 0:
                    break;
                case 1:
                    tab = 1;
                    break;
                case 2:
                    tab = 2;
                    if (MiiData_GetSex(character) != 0) {
                        filterTab = 1;
                    }
                    break;
                case 8:
                    tab = 3;
                    break;
                }
            }
            mTab = tab;
            mListTabs.Select(tab);
            ShowList(tab, filterTab);
        }
        break;
    }
    case MODE_DRESS:
    case MODE_DRESS_2: {
        wchar_t name[12];
        MiiData_GetName(mContext->mCharacter, name);
        mTitle.SetStringArg(name, 0);
        mTitle.mSprite.mVisible = TRUE;
        NNS_G2dCharCanvasClear(&mTitle.mCanvas, 0);
        mTitle.DrawAll();
        mShopButton.mVisible = FALSE;
        mCloseButton.mVisible = mMode == MODE_DRESS ? TRUE : FALSE;
        mListTabs.SetVisible(FALSE);
        mGoodsTabs.SetVisible(FALSE);
        mClothesTabs.SetVisible(TRUE);
        mOwnClothes.LoadOwnedBy(mContext->mCharacter);
        mOwnClothes.Sort();
        if (MiiData_GetSex(mContext->mCharacter) == 0) {
            mClothesTabs.Select(0);
            mOwnClothes.GetOwned(mItems, &mItemCount, 0);
        } else {
            mClothesTabs.Select(1);
            mOwnClothes.GetOwned(mItems, &mItemCount, 1);
        }
        mOkButton.mVisible = TRUE;
        SpriteButton_SetDisabled(&mOkButton, TRUE);
        NNS_G2dCharCanvasClear(&mOkButton.mText.mCanvas, 0);
        mOkButton.mText.mMessage = 479;
        mOkButton.mText.DrawAll();
        mList.SetMode(ItemListView::MODE_NAME);
        mList.SetMessageFile(mContext->mCostumeMessages);
        mList.mMarkKind = Scene40ListView::MARK_NONE;
        mList.SetVisible(TRUE);
        mList.SetItems(mItems, mItemCount);
        mList.mTouchable = TRUE;
        mGift.Set(1, 0);
        break;
    }
    }
    mNextState = STATE_CHOOSE;
}

void Scene40ListPage::Choose() {
    mListTabs.Update();
    if (mListTabs.mPressed != 0) {
        ShowList(mListTabs.mSelected, -1);
        SpriteButton_SetDisabled(&mOkButton, TRUE);
    }
    mGoodsTabs.Update();
    if (mGoodsTabs.mPressed != -1) {
        mInventory->GetOwned(mItems, &mItemCount, 1, mGoodsTabs.GetFilter());
        mList.SetItems(mItems, mItemCount);
        SpriteButton_SetDisabled(&mOkButton, TRUE);
    }
    mClothesTabs.Update();
    if (mClothesTabs.mPressed != -1) {
        switch (mMode) {
        case MODE_GIVE:
        case MODE_GIVE_FROM_SHOP:
            mClothesList->GetOwned(mItems, &mItemCount, mClothesTabs.GetKind());
            mList.SetItems(mItems, mItemCount);
            break;
        case MODE_DRESS:
        case MODE_DRESS_2:
            mOwnClothes.GetOwned(mItems, &mItemCount, mClothesTabs.GetKind());
            mList.SetItems(mItems, mItemCount);
            break;
        }
        SpriteButton_SetDisabled(&mOkButton, TRUE);
    }
    mList.Update();
    if (mList.mTouched && mList.mSelected != -1) {
        ShowItem(mList.mItems[mList.mSelected]->mId);
    }

    if (mActive && SpriteButton_Update(&mOkButton)) {
        if (mList.mSelected != -1) {
            Item *item = mList.mItems[mList.mSelected];
            mChosenId = item->mId;
            switch (mMode) {
            case MODE_GIVE:
            case MODE_GIVE_FROM_SHOP:
                item->Add(-1);
                if (item->mCount <= 0) {
                    switch (mListTabs.mSelected) {
                    case 0:
                    case 1:
                        mSortGoods = TRUE;
                        break;
                    case 2:
                        mSortClothes = TRUE;
                        break;
                    case 3:
                        mSortInteriors = TRUE;
                        break;
                    }
                }
                break;
            case MODE_DRESS:
                mContext->mCharacter->mClothesId = (u16)mChosenId;
                mContext->mCharacter->AddUnknown1C4Once2();
                mContext->mCharacter->SetState(0x55);
                break;
            case MODE_DRESS_2:
                mContext->mCharacter->mClothesId = (u16)mChosenId;
                mContext->mCharacter->AddUnknown1C4Once2();
                mContext->mCharacter->SetState(0x9A);
                break;
            }
            mChosen = TRUE;
        } else {
            mChosen = FALSE;
        }
        mNextState = STATE_CHOSEN;
    }
    if (mActive && SpriteButton_Update(&mCloseButton)) {
        mChosen = FALSE;
        mNextState = STATE_CHOSEN;
    }
    if (mActive && SpriteButton_Update(&mShopButton)) {
        gSceneMemory.mUnknown2C[0] = 0xFFFF;
        if (mList.mSelected != -1) {
            gSceneMemory.mUnknown2C[0] = mList.mItems[mList.mSelected]->mId;
        }
        switch (mListTabs.mSelected) {
        case 0:
            if (gIslandState.GetUnknown44(3) == 2) {
                gIslandState.SetUnknown44(3, 1);
            }
            gSceneManager.RequestScene(0x68);
            break;
        case 1:
            if (gIslandState.GetUnknown44(8) == 2) {
                gIslandState.SetUnknown44(8, 1);
            }
            gSceneManager.RequestScene(0x2F);
            break;
        case 2:
            if (gIslandState.GetUnknown44(10) == 2) {
                gIslandState.SetUnknown44(10, 1);
            }
            gSceneManager.RequestScene(0x1E);
            break;
        case 3:
            if (gIslandState.GetUnknown44(11) == 2) {
                gIslandState.SetUnknown44(11, 1);
            }
            gSceneManager.RequestScene(0x35);
            break;
        }
    }
    if (mMannequin->mHead.mVisible) {
        PlaceMannequin();
    }
    if (mItemModel->mVisible) {
        PlaceItemModel();
    }
}

void Scene40ListPage::Close() {
    mItemModel->Hide();
    mItemModel = NULL;
    mMannequin->SetVisible(FALSE);
    mOkButton.mVisible = FALSE;
    mShopButton.mVisible = FALSE;
    mCloseButton.mVisible = FALSE;
    mListTabs.SetVisible(FALSE);
    mGoodsTabs.SetVisible(FALSE);
    mClothesTabs.SetVisible(FALSE);
    mList.SetVisible(FALSE);
    mContext->mSpeechWindow->Hide();
    mNextState = STATE_NONE;
}

void Scene40ListPage::Draw() {
    mTitle.Draw();
    mList.Draw();
    TextButton_Draw(&mOkButton);
    TextButton_Draw(&mShopButton);
    SpriteButton_Draw(&mCloseButton);
    mListTabs.Draw();
    mGoodsTabs.Draw();
    mClothesTabs.Draw();
}

// The list of a tab: filterTab chooses the goods' filter or the kind of clothes, -1 for the first (for clothes, the
// character's sex's)
void Scene40ListPage::ShowList(int tab, int filterTab) {
    if (filterTab == -1) {
        filterTab = 0;
        if (tab == 2 && MiiData_GetSex(mContext->mCharacter) != 0) {
            filterTab = 1;
        }
    }
    switch (tab) {
    case 0:
        mGoodsTabs.SetVisible(TRUE);
        mGoodsTabs.Select(filterTab);
        mClothesTabs.SetVisible(FALSE);
        mInventory->GetOwned(mItems, &mItemCount, 1, mGoodsTabs.GetFilter());
        mList.SetMessageFile(mContext->mItemMessages);
        mList.mMarkKind = Scene40ListView::MARK_ITEMS;
        mList.SetItems(mItems, mItemCount);
        mShopButton.mVisible = mContext->mSceneId == 0x40 ? TRUE : FALSE;
        mGift.Set(0, 0);
        break;
    case 1:
        mGoodsTabs.SetVisible(FALSE);
        mClothesTabs.SetVisible(FALSE);
        mInventory->GetOwned(mItems, &mItemCount, 3);
        mList.SetMessageFile(mContext->mItemMessages);
        mList.mMarkKind = Scene40ListView::MARK_ITEMS;
        mList.SetItems(mItems, mItemCount);
        mShopButton.mVisible = FALSE;
        mGift.Set(0, 0);
        break;
    case 2:
        mGoodsTabs.SetVisible(FALSE);
        mClothesTabs.SetVisible(TRUE);
        mClothesTabs.Select(filterTab);
        mClothesList->GetOwned(mItems, &mItemCount, mClothesTabs.GetKind());
        mList.SetMessageFile(mContext->mCostumeMessages);
        mList.mMarkKind = Scene40ListView::MARK_CLOTHES;
        mList.SetItems(mItems, mItemCount);
        mShopButton.mVisible = gIslandState.GetUnknown44(10) ? TRUE : FALSE;
        mGift.Set(1, 0);
        break;
    case 3:
        mGoodsTabs.SetVisible(FALSE);
        mClothesTabs.SetVisible(FALSE);
        mInteriors->GetOwnedFrom8(mItems, &mItemCount);
        mList.SetMessageFile(mContext->mRoomMessages);
        mList.mMarkKind = Scene40ListView::MARK_INTERIORS;
        mList.SetItems(mItems, mItemCount);
        mShopButton.mVisible = gIslandState.GetUnknown44(11) ? TRUE : FALSE;
        mGift.Set(2, 0);
        break;
    }
}

// Shows the item touched: goods on an item model, clothes on the mannequin
void Scene40ListPage::ShowItem(u16 id) {
    SpriteButton_SetDisabled(&mOkButton, mList.mItems[mList.mSelected]->mCount <= 0);
    switch (mGift.mKind) {
    case 0:
        mItemModel->ChangeItem(id);
        mItemModel->mVisible = TRUE;
        mItemModel->mMotion.mHeld = TRUE;
        mMannequin->SetVisible(FALSE);
        mGift.Set(0, id);
        break;
    case 1:
        DressMannequin(id);
        mMannequin->SetVisible(TRUE);
        mItemModel->mVisible = FALSE;
        mGift.Set(1, id);
        break;
    case 2:
        mItemModel->mVisible = FALSE;
        mMannequin->SetVisible(FALSE);
        mGift.Set(2, id);
        break;
    }
}

void Scene40ListPage::DressMannequin(u16 clothes) {
    ModelResources_ReleaseModel(gModelResources, 12);
    ArchiveManager_Unmount(&gArchiveManager, "3MM");
    mMannequin->SetArchive("3MM", clothes, MiiData_GetSex(mContext->mCharacter));
    ArchiveManager_MountCompressed(&gArchiveManager, "3MM", mMannequin->mArchivePath, TRUE);
    ModelResources_SetModel(gModelResources, 12, mMannequin->mModelPath);
    mMannequin->MakeModel(12, 0);
    mMannequin->SetPaletteColors(NFL_GetFavoriteColor(MiiData_GetFavoriteColor(mContext->mCharacter)), 0x5FBE);
    mMannequin->SetAnimFlags(TRUE, TRUE);
}

// Puts the mannequin where the screen point shows, turned a little and sized by its distance from the camera
void Scene40ListPage::PlaceMannequin() {
    VecFx32 position;
    ProjectTouchBetween(&sMannequinScreenPoint, &position, 0x3000, 0x4000);
    fx32 distance = GetCameraDistanceScale(&position);
    MtxFx33 rotation;
    MakeFacingMatrixY(&rotation, -0x1F3, 0xFE1);
    MI_Copy36B(&rotation, &mMannequin->mAnim.mRotation);
    fx32 scale = FX_Mul(distance, 0xF33);
    SetBodyPosition(mMannequin, position);
    SetBodyScaleX(FX_Mul(scale, gMannequinBodyScale.x), mMannequin);
    SetBodyScaleY(FX_Mul(scale, gMannequinBodyScale.y), mMannequin);
    SetBodyScaleZ(FX_Mul(scale, gMannequinBodyScale.z), mMannequin);
    mMannequin->mHeadScale.x = FX_Mul(scale, gMannequinHeadScale.x);
    mMannequin->mHeadScale.y = FX_Mul(scale, gMannequinHeadScale.y);
    mMannequin->mHeadScale.z = FX_Mul(scale, gMannequinHeadScale.z);
}

void Scene40ListPage::PlaceItemModel() {
    Camera_UpdateEye(&gCamera);
    VecFx32 position;
    ProjectTouchBetween(&sItemScreenPoint, &position, 0x3000, 0x4000);
    mItemModel->SetPosition(position);
}

void Scene40ListPage::GetGift(Scene40Gift *gift) {
    gift->Set(mGift.mKind, mGift.mItem.InventoryItem::mId);
}

// --- Page 9 (pages+0x214B0) ---------------------------------------------------------------------------------------

void Scene40OptionPage::Draw() {
    mTitle.Draw();
    for (int i = 0; i < 4; i++) {
        Sprite_Draw(&mSpritesA[i]);
        mTextsA[i].Draw();
        SpriteButton_Draw(&mButtonsA[i]);
    }
    SpriteButton_Draw(&mBackButton);
    for (int i = 0; i < 6; i++) {
        Sprite_Draw(&mSpritesB[i]);
        mTextsB[i].Draw();
        SpriteButton_Draw(&mButtonsB[i]);
    }
    for (int i = 0; i < 8; i++) {
        Sprite_Draw(&mSpritesC[i]);
        mTextsC[i].Draw();
        SpriteButton_Draw(&mButtonsC[i]);
    }
}

// The goods of the third panel
static const int sOptionGoods[8] = {236, 241, 243, 242, 239, 240, 237, 238};

void Scene40OptionPage::UpdateChooseB() {
    if (SpriteButton_Update(&mBackButton)) {
        mNextState = 7;
        mFrames = 0;
        mNextPanel = 2;
    }
    int chosen = -1;
    for (int i = 0; i < 6; i++) {
        if (SpriteButton_Update(&mButtonsB[i])) {
            chosen = i;
        }
    }
    if (chosen == -1) {
        return;
    }
    mResult = 0;
    mGift.Set(3, chosen);
    mNextState = 10;
    mFrames = 0;
}

void Scene40OptionPage::UpdateChooseC() {
    if (SpriteButton_Update(&mBackButton)) {
        mNextState = 7;
        mFrames = 0;
        mNextPanel = 2;
    }
    int chosen = -1;
    for (int i = 0; i < 8; i++) {
        if (SpriteButton_Update(&mButtonsC[i])) {
            chosen = i;
        }
    }
    if (chosen == -1) {
        return;
    }
    mResult = 1;
    mGift.Set(0, sOptionGoods[chosen]);
    mNextState = 10;
    mFrames = 0;
}

void Scene40OptionPage::UpdateEndWithResult3() {
    mResult = 3;
    mNextState = 10;
    mFrames = 0;
}

void Scene40OptionPage::UpdateWaitMessages() {
    Scene40MessageSequence *sequence = mContext->mMessageSequence;
    if (sequence->mLastState == Scene40MessageSequence::STATE_DONE) {
        mNextState = sequence->mReturnState;
        mFrames = 0;
    }
}

// The goods of the third panel the marks are of
static const int sOptionDoneGoods[8] = {236, 241, 243, 242, 239, 240, 237, 238};

// Marks what the character has done
void Scene40OptionPage::UpdateDone() {
    Scene40Character *character = mContext->mCharacter;
    int count = 0;
    for (int i = 0; i < 6; i++) {
        if ((character->mUnknown299 >> (i & 31)) & 1) {
            mDoneB[i] = TRUE;
            count++;
        }
    }
    if (count >= 6) {
        mDoneA[0] = TRUE;
    }
    int had = 0;
    for (int i = 0; i < 8; i++) {
        int item = sOptionDoneGoods[i];
        if ((character->mUnknown1C8.m[item >> 5] >> (item & 31)) & 1) {
            mDoneC[i] = TRUE;
            had++;
        }
    }
    if (had >= 8) {
        mDoneA[1] = TRUE;
    }
    BOOL all = TRUE;
    for (int i = 0; i < 78; i++) {
        if (!(character->mOwnedInteriors[i >> 5] & (1 << (i & 31))) && gInteriorTable[i].mKind == 13) {
            all = FALSE;
            break;
        }
    }
    if (all) {
        mDoneA[2] = TRUE;
    }
    if ((wcslen(character->mMotto) != 0 ? TRUE : FALSE)) {
        mDoneA[3] = TRUE;
    }
}

BOOL Scene40OptionPage::IsAllDone() {
    BOOL all = TRUE;
    for (int i = 0; i < 4; i++) {
        if (!mDoneA[i]) {
            all = FALSE;
        }
    }
    return all;
}

void Scene40Button::SetSequences(u16 sequence, u16 pressedSequence, u16 doneSequence) {
    mSequence = sequence;
    mPressedSequence = pressedSequence;
    mDoneSequence = doneSequence;
    Sprite_SetSequence(this, mSequence, TRUE);
}

// A button marked done shows its done sequence
void Scene40Button::SetDone(BOOL done) {
    SpriteButton_Reset(this);
    if (done) {
        Sprite_SetSequence(this, mDoneSequence, TRUE);
    } else {
        Sprite_SetSequence(this, mSequence, TRUE);
    }
    mUnknownB8 = done;
}

// --- The list page's tabs ------------------------------------------------------------------------------------------

static const u16 sListTabSequences[Scene40ListTabs::TAB_COUNT] = {41, 47, 43, 45};

static const u16 sListTabPressedSequences[Scene40ListTabs::TAB_COUNT] = {42, 48, 44, 46};

static const ScreenPoint sListTabPositions[Scene40ListTabs::TAB_COUNT] = {{23, 13}, {158, 13}, {68, 13}, {113, 13}};

void Scene40ListTabs::Init(int resource) {
    mPressed = 0;
    mSelected = 0;
    for (int i = 0; i < TAB_COUNT; i++) {
        Sprite_Init(&mTabs[i], resource, sListTabSequences[i], 128, 96, TRUE);
        SpriteButton_SetSequences(&mTabs[i], sListTabSequences[i], sListTabPressedSequences[i]);
        mTabs[i].SetPositionFromVec(sListTabPositions[i].x << FX32_SHIFT, sListTabPositions[i].y << FX32_SHIFT);
        SpriteButton_SetTouchRect(&mTabs[i], 44, 22);
        SpriteButton_Reset(&mTabs[i]);
        SpriteButton_SetKind(&mTabs[i], 3);
        mTabs[i].mOverwritePalette = 1;
        mTabs[i].mUnknownDC = TRUE;
        mTabs[i].mSounds.mSounds[0] = 3;
    }
    SetVisible(FALSE);
}

// A list's tab is there when the player owns some of it or the island has the shop that sells it
void Scene40ListTabs::UpdateAvailable(Scene40Context *context) {
    u8 built = gIslandState.GetUnknown44(3);
    BOOL owned = context->mInventory->HasAnyOwnedKind1();
    mAvailable[0] = owned | (built != 0);
    u8 level = gGameData.mUnknownB2[0];
    owned = context->mInventory->HasAnyOwnedKind3();
    mAvailable[1] = owned | (level >= 5);
    built = gIslandState.GetUnknown44(10);
    owned = context->mClothesList->HasAnyOwned();
    mAvailable[2] = owned | (built != 0);
    built = gIslandState.GetUnknown44(11);
    owned = context->mInteriors->HasAnyOwned();
    mAvailable[3] = owned | (built != 0);
}

void Scene40ListTabs::Update() {
    mPressed = 0;
    if (SpriteButton_Update(&mTabs[0])) {
        mPressed = 1 << 0;
        SpriteButton_SetSelected(&mTabs[mSelected], FALSE);
        mSelected = 0;
    } else if (SpriteButton_Update(&mTabs[1])) {
        mPressed = 1 << 1;
        SpriteButton_SetSelected(&mTabs[mSelected], FALSE);
        mSelected = 1;
    } else if (SpriteButton_Update(&mTabs[2])) {
        mPressed = 1 << 2;
        SpriteButton_SetSelected(&mTabs[mSelected], FALSE);
        mSelected = 2;
    } else if (SpriteButton_Update(&mTabs[3])) {
        mPressed = 1 << 3;
        SpriteButton_SetSelected(&mTabs[mSelected], FALSE);
        mSelected = 3;
    }
}

void Scene40ListTabs::Draw() {
    for (int i = 0; i < TAB_COUNT; i++) {
        SpriteButton_Draw(&mTabs[i]);
    }
}

void Scene40ListTabs::SetVisible(BOOL visible) {
    for (int i = 0; i < TAB_COUNT; i++) {
        mTabs[i].mVisible = visible & mAvailable[i];
    }
}

void Scene40ListTabs::Select(int tab) {
    for (int i = 0; i < TAB_COUNT; i++) {
        SpriteButton_SetSelected(&mTabs[i], FALSE);
    }
    mSelected = tab;
    SpriteButton_SetSelected(&mTabs[tab], TRUE);
}

static const u16 sGoodsTabSequences[Scene40GoodsTabs::TAB_COUNT] = {37, 39, 20, 22};

static const u16 sGoodsTabPressedSequences[Scene40GoodsTabs::TAB_COUNT] = {38, 40, 21, 23};

static const ScreenPoint sGoodsTabPositions[Scene40GoodsTabs::TAB_COUNT] = {{32, 41}, {88, 41}, {144, 41}, {200, 41}};

static const u32 sGoodsTabFilters[Scene40GoodsTabs::TAB_COUNT] = {1, 2, 8, 16};

void Scene40GoodsTabs::Init(int resource) {
    for (int i = 0; i < TAB_COUNT; i++) {
        Sprite_Init(&mTabs[i], resource, sGoodsTabSequences[i], 128, 96, TRUE);
        SpriteButton_SetSequences(&mTabs[i], sGoodsTabSequences[i], sGoodsTabPressedSequences[i]);
        mTabs[i].SetPositionFromVec(sGoodsTabPositions[i].x << FX32_SHIFT, sGoodsTabPositions[i].y << FX32_SHIFT);
        SpriteButton_SetTouchRect(&mTabs[i], 52, 24);
        SpriteButton_Reset(&mTabs[i]);
        SpriteButton_SetKind(&mTabs[i], 3);
        mTabs[i].mOverwritePalette = 1;
        mTabs[i].mUnknownDC = TRUE;
        mTabs[i].mSounds.mSounds[0] = 3;
    }
    SetVisible(FALSE);
    Select(0);
}

void Scene40GoodsTabs::Update() {
    mPressed = -1;
    for (int i = 0; i < TAB_COUNT; i++) {
        if (SpriteButton_Update(&mTabs[i])) {
            SpriteButton_SetSelected(&mTabs[mSelected], FALSE);
            mSelected = i;
            mPressed = i;
        }
    }
}

void Scene40GoodsTabs::Draw() {
    for (int i = 0; i < TAB_COUNT; i++) {
        SpriteButton_Draw(&mTabs[i]);
    }
}

void Scene40GoodsTabs::SetVisible(BOOL visible) {
    for (int i = 0; i < TAB_COUNT; i++) {
        mTabs[i].mVisible = visible;
    }
}

void Scene40GoodsTabs::Select(int tab) {
    for (int i = 0; i < TAB_COUNT; i++) {
        SpriteButton_SetSelected(&mTabs[i], FALSE);
    }
    SpriteButton_SetSelected(&mTabs[tab], TRUE);
    mSelected = tab;
}

u32 Scene40GoodsTabs::GetFilter() {
    return sGoodsTabFilters[mSelected];
}

int Scene40GoodsTabs::FindTab(u32 filter) {
    int tab = 0;
    switch (filter & 0x1F) {
    case 1:
        tab = 0;
        break;
    case 2:
        tab = 1;
        break;
    case 8:
        tab = 2;
        break;
    case 16:
        tab = 3;
        break;
    }
    return tab;
}

static const u16 sClothesTabSequences[Scene40ClothesTabs::TAB_COUNT] = {24, 26, 28, 30, 32};

static const u16 sClothesTabPressedSequences[Scene40ClothesTabs::TAB_COUNT] = {25, 27, 29, 31, 33};

static const ScreenPoint sClothesTabPositions[Scene40ClothesTabs::TAB_COUNT] = {
    {26, 41}, {72, 41}, {118, 41}, {164, 41}, {210, 41},
};

static const int sClothesTabKinds[Scene40ClothesTabs::TAB_COUNT] = {0, 1, 2, 3, 4};

void Scene40ClothesTabs::Init(int resource) {
    for (int i = 0; i < TAB_COUNT; i++) {
        Sprite_Init(&mTabs[i], resource, sClothesTabSequences[i], 128, 96, TRUE);
        SpriteButton_SetSequences(&mTabs[i], sClothesTabSequences[i], sClothesTabPressedSequences[i]);
        mTabs[i].SetPositionFromVec(sClothesTabPositions[i].x << FX32_SHIFT, sClothesTabPositions[i].y << FX32_SHIFT);
        SpriteButton_SetTouchRect(&mTabs[i], 42, 24);
        SpriteButton_Reset(&mTabs[i]);
        SpriteButton_SetKind(&mTabs[i], 3);
        mTabs[i].mOverwritePalette = 1;
        mTabs[i].mUnknownDC = TRUE;
        mTabs[i].mSounds.mSounds[0] = 3;
    }
    SetVisible(FALSE);
    Select(0);
}

void Scene40ClothesTabs::Update() {
    mPressed = -1;
    for (int i = 0; i < TAB_COUNT; i++) {
        if (SpriteButton_Update(&mTabs[i])) {
            SpriteButton_SetSelected(&mTabs[mSelected], FALSE);
            mSelected = i;
            mPressed = i;
        }
    }
}

void Scene40ClothesTabs::Draw() {
    for (int i = 0; i < TAB_COUNT; i++) {
        SpriteButton_Draw(&mTabs[i]);
    }
}

void Scene40ClothesTabs::SetVisible(BOOL visible) {
    for (int i = 0; i < TAB_COUNT; i++) {
        mTabs[i].mVisible = visible;
    }
}

void Scene40ClothesTabs::Select(int tab) {
    for (int i = 0; i < TAB_COUNT; i++) {
        SpriteButton_SetSelected(&mTabs[i], FALSE);
    }
    SpriteButton_SetSelected(&mTabs[tab], TRUE);
    mSelected = tab;
}

int Scene40ClothesTabs::GetKind() {
    return sClothesTabKinds[mSelected];
}

int Scene40ClothesTabs::FindTab(int kind) {
    int tab = 0;
    switch (kind) {
    case 0:
        tab = 0;
        break;
    case 1:
        tab = 1;
        break;
    case 2:
        tab = 2;
        break;
    case 3:
        tab = 3;
        break;
    case 4:
        tab = 4;
        break;
    }
    return tab;
}

// --- The scene's speech window (scene+0x163B0) ---------------------------------------------------------------------

void Scene40SpeechWindow::Update() {
    SpeechWindow::Update();
    if (mEnabled && gScreenControl.mUnknown44 == 0) {
        return;
    }
    Hide();
}

void Scene40SpeechWindow::Show(u32 message, int mode, int frames) {
    if (!mEnabled) {
        return;
    }
    if (gScreenControl.mUnknown44 != 0) {
        return;
    }
    SpeechWindow::Show(message, mode, frames);
}

// --- The scene's menu (scene+0x6947C) ------------------------------------------------------------------------------

// Each button's sequence, pressed sequence, place and touch rectangle in pixels and sound; the first button's
// values are written out in Init as well
static const u16 sMenuSequences[Scene40MenuBar::BUTTON_COUNT] = {4, 7, 2, 11, 9, 28, 0};

static const u16 sMenuPressedSequences[Scene40MenuBar::BUTTON_COUNT] = {6, 8, 3, 12, 10, 29, 1};

static const ScreenPoint sMenuPositions[Scene40MenuBar::BUTTON_COUNT] = {
    {18, 13}, {18, 109}, {18, 37}, {18, 85}, {18, 61}, {207, 13}, {242, 13},
};

static const ScreenPoint sMenuTouchSizes[Scene40MenuBar::BUTTON_COUNT] = {
    {32, 22}, {32, 22}, {32, 22}, {32, 22}, {32, 22}, {32, 22}, {24, 22},
};

static const int sMenuSounds[Scene40MenuBar::BUTTON_COUNT] = {10, 10, 10, 10, 10, 10, 12};

void Scene40CallButton::Reset() {
    SpriteButton_Reset(this);
    mCallState = CALL_NONE;
    mCallFrames = 0;
}

void Scene40CallButton::SetSequences(u16 sequence, u16 pressedSequence, u16 callSequence) {
    SpriteButton_SetSequences(this, sequence, pressedSequence);
    mCallSequence = callSequence;
}

void Scene40CallButton::StartCall() {
    if (!mVisible) {
        return;
    }
    mCallState = CALL_WAITING;
    mCallFrames = 0;
}

// Whether it was pressed; a touch stops the call
BOOL Scene40CallButton::Update() {
    BOOL pressed = SpriteButton_Update(this);
    BOOL selected = func_02031d00(this);
    if (pressed | selected) {
        mCallState = CALL_NONE;
    }
    return pressed;
}

void Scene40CallButton::UpdateCall() {
    switch (mCallState) {
    case CALL_NONE:
        break;
    case CALL_WAITING:
        if (++mCallFrames < 120) {
            break;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            break;
        }
        Sprite_SetSequence(this, mCallSequence, TRUE);
        gSoundManager.StartSeqArc(1, 10, 14);
        mCallState = CALL_CALLING;
        mCallFrames = 0;
        break;
    case CALL_CALLING:
        if (++mCallFrames < 160) {
            if (mCallFrames % 20 == 0 && (mCallFrames / 20) % 2 == 0) {
                gSoundManager.StartSeqArc(1, 10, 15);
            }
        } else if (mCallFrames >= 160) {
            Sprite_SetSequence(this, mSequence, TRUE);
            mCallState = CALL_NONE;
            mCallFrames = 0;
        }
        break;
    }
}

void Scene40CallButton::SetDisabled(BOOL disabled) {
    SpriteButton_SetDisabled(this, disabled);
    if (disabled) {
        mCallState = CALL_NONE;
    }
}

void Scene40ButtonFlash::Update() {
    if (!mVisible) {
        return;
    }
    if (!mAnim.animCtrl.bActive) {
        mVisible = FALSE;
    }
}

void Scene40ButtonFlash::Play() {
    if (mVisible) {
        return;
    }
    mVisible = TRUE;
    Sprite_SetPlaying(this, TRUE);
}

void Scene40MenuBar::Init(Scene40Context *context) {
    mUnknown000 = 0;
    mUnknown004 = 0;
    mTouchedButton = -1;
    mPressedButton = -1;
    mContext = context;
    mCharacter = context->mCharacter;
    mEnabled = FALSE;

    Sprite_Init(&mCallButton, 0, 4, 128, 96, TRUE);
    mCallButton.SetSequences(4, 6, 5);
    mCallButton.mTtsPlayer = mContext->mTtsPlayer;
    mCallButton.SetPositionFromVec(18 << FX32_SHIFT, 13 << FX32_SHIFT);
    SpriteButton_SetTouchRect(&mCallButton, 32, 22);
    mCallButton.Reset();
    SpriteButton_SetKind(&mCallButton, 4);
    mCallButton.mOverwritePalette = 1;
    mCallButton.mUnknownDC = TRUE;
    mCallButton.mSounds.mSounds[0] = 10;
    mCallButton.mSounds.mSounds[2] = 0;
    mButtonList[0] = &mCallButton;

    for (int i = 1; i < BUTTON_COUNT; i++) {
        Sprite_Init(&mButtons[i], 0, sMenuSequences[i], 128, 96, TRUE);
        SpriteButton_SetSequences(&mButtons[i], sMenuSequences[i], sMenuPressedSequences[i]);
        mButtons[i].SetPositionFromVec(sMenuPositions[i].x << FX32_SHIFT, sMenuPositions[i].y << FX32_SHIFT);
        SpriteButton_SetTouchRect(&mButtons[i], sMenuTouchSizes[i].x, sMenuTouchSizes[i].y);
        SpriteButton_Reset(&mButtons[i]);
        SpriteButton_SetKind(&mButtons[i], 4);
        mButtons[i].mOverwritePalette = 1;
        mButtons[i].mUnknownDC = TRUE;
        mButtons[i].mSounds.mSounds[0] = sMenuSounds[i];
        mButtons[i].mSounds.mSounds[2] = 0;
        mButtonList[i] = &mButtons[i];
    }
    for (int i = 0; i < 5; i++) {
        mButtonList[i]->mVisible = FALSE;
    }

    Sprite_Init(&mFlash, 0, 31, 128, 96, TRUE);
    mFlash.mOverwritePalette = 1;
    mFlash.mVisible = FALSE;
}

void Scene40MenuBar::Update() {
    mTouchedButton = -1;
    mPressedButton = -1;
    mFlash.Update();
    mCallButton.UpdateCall();
    if (mEnabled && SpriteButton_IsTouched(&mCallButton)) {
        mTouchedButton = 0;
    }
    if (mEnabled && mCallButton.Update()) {
        mPressedButton = 0;
    }
    for (int i = 1; i < BUTTON_COUNT; i++) {
        if (mEnabled && SpriteButton_IsTouched(mButtonList[i])) {
            mTouchedButton = i;
        }
        if (mEnabled && SpriteButton_Update(mButtonList[i])) {
            mPressedButton = i;
        }
    }
}

void Scene40MenuBar::SetEnabled(BOOL enabled) {
    if (enabled != mEnabled) {
        mEnabled = enabled;
    }
}

// Shows the buttons whose offer is there; coming back from scene 0x38 or 0x40, the first five are as that scene
// left them
void Scene40MenuBar::Refresh() {
    UpdateAvailable();
    if (gSceneManager.mPreviousSceneId == 0x38) {
        for (int i = 0; i < 5; i++) {
            mAvailable[i] = gSceneScratch.mMenuAvailable[i];
        }
    } else if (gSceneManager.mPreviousSceneId == 0x40) {
        for (int i = 0; i < 5; i++) {
            mAvailable[i] = gSceneScratch.mMenuAvailable[i];
        }
    }
    for (int i = 0; i < BUTTON_COUNT; i++) {
        mButtonList[i]->mVisible = mAvailable[i] ? TRUE : FALSE;
    }
}

void Scene40MenuBar::UpdateAvailable() {
    mAvailable[0] = mContext->mSceneId != 0x6C;
    mAvailable[1] = MATH_CountPopulation(mCharacter->mUnknown299) != 0;
    mAvailable[2] = mCharacter->CountOwnedClothes() > 1;
    mAvailable[3] = wcslen(mCharacter->mMotto) != 0;
    mAvailable[4] = mCharacter->CountOwnedInteriors() > 1;
    mAvailable[5] = TRUE;
    mAvailable[6] = TRUE;
}

void Scene40MenuBar::Draw() {
    Sprite_Draw(&mFlash);
    for (int i = 0; i < BUTTON_COUNT; i++) {
        SpriteButton_Draw(mButtonList[i]);
    }
}

void Scene40MenuBar::StartCall() {
    mCallButton.StartCall();
}

// Shows the button chosen and plays the flash over it
void Scene40MenuBar::Choose(int button) {
    SetButtonVisible(button, TRUE);
    ScreenPoint position = mButtonList[button]->GetPixelPosition();
    mFlash.SetPositionFromVec(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
    mFlash.Play();
    gSoundManager.StartSeqArc(1, 10, 17);
}

void Scene40MenuBar::SetButtonVisible(int button, BOOL visible) {
    mButtonList[button]->mVisible = visible;
}

void Scene40MenuBar::SetVisible(BOOL visible) {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        mButtonList[i]->mVisible = visible;
    }
}

void Scene40MenuBar::SetDisabled(BOOL disabled) {
    mCallButton.SetDisabled(disabled);
    for (int i = 1; i < BUTTON_COUNT; i++) {
        SpriteButton_SetDisabled(mButtonList[i], disabled);
    }
}

void Scene40MenuBar::SetLeftDisabled(BOOL disabled) {
    mCallButton.SetDisabled(disabled);
    for (int i = 1; i < 5; i++) {
        SpriteButton_SetDisabled(mButtonList[i], disabled);
    }
}

void Scene40MenuBar::SetRightDisabled(BOOL disabled) {
    for (int i = 5; i <= 6; i++) {
        SpriteButton_SetDisabled(mButtonList[i], disabled);
    }
}





extern "C" {
void ParticleSystem_Draw(ParticleSystem *particles, int unknown); // Draws the particles
void GameData_WriteCharacter(GameData *game, int index, SavedCharacter *character); // Writes a saved character
void DestroyMainFrameHeap(void);
BOOL ArchiveManager_IsIdle(ArchiveLoader *loader);
extern int gGameClock[];
s64 Clock_GetDateTime(int offset, RTCDate *date, RTCTime *time); // The date and time now, and the same in seconds

void GameData_ReadCharacter(GameData *game, int index, SavedCharacter *character);
void MiiData_GetName(CharacterRecord *character, wchar_t *name);
void MiiData_GetCreatorName(CharacterRecord *character, wchar_t *name);
BOOL func_02031ccc(SpriteButton *button);
BOOL GameData_IsCharacterSlotUsed(GameData *game, int index);
struct ScreenLoader;
extern ScreenLoader gScreenLoader;
void *ScreenLoader_LoadFile(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e); // Whether the button's press has played out
void TextButton_SetOverwritePalette(TextButton *button, int palette);

// Not decompiled yet
extern const char *const sWaitAnimations[4][5]; // The standing animations ("wait00".."wait09"), by personality kind and mood
extern const char *const sWalkAnimations[4][5]; // The walking animations ("walk00".."walk10"), by personality kind and mood
extern MATHRandContext16 gRandom;
void func_ov075_02174a14(Scene40Character *character);
void func_ov075_02174b38(Scene40Character *character, int item);
}

static inline int Random(int max) {
    return MATH_Rand16(&gRandom, max);
}

// The head icon of each mood
extern const s16 sMoodHeadIcons[5];

// The answers of the talk page
extern const Scene40ChoiceSet sChoiceSets[];

// By set
extern const int sTroubleChoiceSets[];

// The set of each kind of trouble
extern const Scene40ChoiceSet sTroubleChoices[];

// Set 10's answers, by trouble id
extern const Scene40ChoiceSet sSpecialChoiceSets[];

// Puts a text canvas at a point in pixels (as src/shop/Shop.cpp's)
static inline void SetCanvasPosition(ObjTextCanvas *canvas, ScreenPoint position) {
    canvas->SetPosition(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
}

// Clears a message text's canvas
static inline void ClearMessageText(MessageText *text) { NNS_G2dCharCanvasClear(&text->mCanvas, 0); }

// A text button's text: cleared, and drawn again
static inline void ClearButtonText(TextButton *button) { NNS_G2dCharCanvasClear(&button->mText.mCanvas, 0); }

static inline void DrawButtonText(TextButton *button) { button->mText.DrawAll(); }

// A model's scale, given by value. The copy of the parameter is the original's: ShowBed has the call's result,
// the parameter and this copy on its stack.
static inline void SetModelScale(Model *model, VecFx32 scale) {
    VecFx32 s = scale;
    model->mScale.x = s.x;
    model->mScale.y = s.y;
    model->mScale.z = s.z;
}

// --- The talk page's answers --------------------------------------------------------------------------------------

void Scene40ChoiceList::Init(int *charOffset, MessageFile *file) {
    mState = STATE_HIDDEN;
    mCount = 0;
    mSet = 0;
    mPressed = -1;
    mFinished = -1;
    mOpened = FALSE;
    mClosed = FALSE;
    mTimer = 0;
    for (int i = 0; i < 4; i++) {
        Sprite_Init(&mButtons[i], 0, 15, 0x80, 0x60, TRUE);
        SpriteButton_SetSequences(&mButtons[i], 15, 16);
        mButtons[i].SetPosition(FxVec2(208 << FX32_SHIFT, (i * 24 + 64) << FX32_SHIFT));
        SpriteButton_SetTouchRect(&mButtons[i], 96, 20);
        *charOffset += TextButton_SetMessage(&mButtons[i], file, 480, &gFonts[0].mFont, 16, *charOffset);
        SpriteButton_Reset(&mButtons[i]);
        SpriteButton_SetKind(&mButtons[i], 4);
        TextButton_SetOverwritePalette(&mButtons[i], 1);
        mButtons[i].mVisible = FALSE;
        mButtons[i].mUnknownDC = TRUE;
        mButtons[i].mSounds.mSounds[0] = 10;
        mButtons[i].mSounds.mSounds[2] = 0;
    }
}

void Scene40ChoiceList::Update() {
    mPressed = -1;
    mFinished = -1;
    mOpened = FALSE;
    mClosed = FALSE;
    switch (mState) {
    case STATE_HIDDEN:
        break;
    case STATE_CHOOSING:
        for (int i = 0; i < mCount; i++) {
            if (SpriteButton_Update(&mButtons[i])) {
                mPressed = i;
            }
            if (func_02031ccc(&mButtons[i])) {
                mFinished = i;
            }
        }
        break;
    case STATE_OPENING:
        if (mTimer >= 100) {
            mState = STATE_CHOOSING;
            mOpened = TRUE;
            for (int i = 0; i < mCount; i++) {
                SpriteButton_Reset(&mButtons[i]);
                mButtons[i].mVisible = TRUE;
            }
        }
        mTimer++;
        break;
    case STATE_CLOSING:
        if (mTimer == 15) {
            for (int i = 0; i < mCount; i++) {
                mButtons[i].mVisible = FALSE;
            }
        } else if (mTimer > 15) {
            mState = STATE_HIDDEN;
            mClosed = TRUE;
        }
        mTimer++;
        break;
    }
}

void Scene40ChoiceList::Draw() {
    for (int i = 0; i < mCount; i++) {
        TextButton_Draw(&mButtons[i]);
    }
}

// The buttons are 24 pixels apart, the last at y 136 however many there are. The three functions that place them
// write the position each in its own way (SetPosition of an FxVec2 or SetPositionFromVec): each form leaves the
// position's dead copies in other stack slots, and these are the forms that give the original's.
void Scene40ChoiceList::SetChoices(int set) {
    mSet = set;
    mCount = sChoiceSets[set].mCount;
    for (int i = 0; i < mCount; i++) {
        ClearButtonText(&mButtons[i]);
        mButtons[i].mText.mMessage = sChoiceSets[mSet].mMessages[i];
        DrawButtonText(&mButtons[i]);
        mButtons[i].SetPosition(FxVec2(208 << FX32_SHIFT, ((i + (4 - mCount)) * 24 + 64) << FX32_SHIFT));
    }
}

// The answers to a trouble: the set of its kind; set 10 has answers of the trouble's own
void Scene40ChoiceList::SetChoicesForTrouble(int category, int id) {
    mTroubleKind = data_02114d78[category][id].kind;
    mSet = sTroubleChoiceSets[mTroubleKind];
    mCount = sChoiceSets[mSet].mCount;
    if (mSet == 10) {
        for (int i = 0; i < mCount; i++) {
            ClearButtonText(&mButtons[i]);
            mButtons[i].mText.mMessage = sTroubleChoices[id].mMessages[i];
            DrawButtonText(&mButtons[i]);
            mButtons[i].SetPositionFromVec(208 << FX32_SHIFT, ((i + (4 - mCount)) * 24 + 64) << FX32_SHIFT);
        }
    } else {
        for (int i = 0; i < mCount; i++) {
            ClearButtonText(&mButtons[i]);
            mButtons[i].mText.mMessage = sChoiceSets[mSet].mMessages[i];
            DrawButtonText(&mButtons[i]);
            mButtons[i].SetPositionFromVec(208 << FX32_SHIFT, ((i + (4 - mCount)) * 24 + 64) << FX32_SHIFT);
        }
    }
}

void Scene40ChoiceList::SetSpecialChoices(int index) {
    mSet = 23;
    mCount = sSpecialChoiceSets[index].mCount;
    for (int i = 0; i < mCount; i++) {
        ClearButtonText(&mButtons[i]);
        mButtons[i].mText.mMessage = sSpecialChoiceSets[index].mMessages[i];
        DrawButtonText(&mButtons[i]);
        mButtons[i].SetPosition(FxVec2(208 << FX32_SHIFT, ((i + (4 - mCount)) * 24 + 64) << FX32_SHIFT));
    }
}

void Scene40ChoiceList::Open() {
    mTimer = 0;
    mState = STATE_OPENING;
}

void Scene40ChoiceList::Close() {
    mTimer = 0;
    mState = STATE_CLOSING;
}

// --- The scene's list: the shops' item list with a mark beside each row ------------------------------------------

void Scene40ListView::Init() {
    ItemListView::Init();
    mMarkKind = MARK_NONE;
}

void Scene40ListView::InitMarks(int resource, int sequence) {
    for (int i = 0; i < mRowCount; i++) {
        Sprite_Init(&mMarks[i], resource, sequence, 0x80, 0x60, TRUE);
        mMarks[i].SetPosition(FxVec2(0, 0));
        mMarks[i].mOverwritePalette = 3;
    }
}

// --- The clothes the character owns, for the list page -------------------------------------------------------------

void Scene40ClothesList::LoadOwnedBy(SavedCharacter *character) {
    int i;
    mCount = 0;
    for (i = 0; i < CLOTHES_SIZE; i++) {
        mItems[i].Clear();
    }
    mCount = CLOTHES_SIZE;
    for (i = 0; i < mCount; i++) {
        mItems[i].mId = i;
        if ((character->mOwnedClothes.m[i >> 5] >> (i & 31)) & 1) {
            mItems[i].mOwned = TRUE;
            mItems[i].mUnknown0C = 1;
            mItems[i].mCount = 1;
        }
        mList[i] = &mItems[i];
    }
}

void Scene40ClothesList::GetOwned(Item **list, int *count, int kind) {
    *count = 0;
    int n = 0;
    for (int i = 0; i < mCount; i++) {
        ClothesItem *item = mList[i];
        if (item->mOwned && kind == gClothesTable[item->mId].mKind) {
            list[n] = item;
            (*count)++;
            n++;
        }
    }
}

static s32 CompareClothesSortKeys(void *a, void *b);

void Scene40ClothesList::Sort() {
    u32 buffer[200];
    MATH_QSort(mList, mCount, sizeof(ClothesItem *), CompareClothesSortKeys, buffer);
}

static s32 CompareClothesSortKeys(void *a, void *b) {
    return gClothesTable[(*(ClothesItem **)a)->mId].mSortKey - gClothesTable[(*(ClothesItem **)b)->mId].mSortKey;
}

// --- The meters of the character's status page -------------------------------------------------------------------

void Scene40Meter::Init() {
    mTarget = 0;
    mValue = 0;
    mFull = FALSE;
    mState = STATE_IDLE;
    mRiseStep = 1;
    mFallStep = 1;
    mSeqArc = 27;
    mRiseSound = 0;
    mFallSound = 1;
    mRiseEndSound = 2;
    mFallEndSound = 3;
}

// One frame of moving the value to the target
void Scene40Meter::Update() {
    mFull = FALSE;
    mEmpty = FALSE;
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_START:
        if (mValue < mTarget) {
            gSoundManager.StartSeqArc(1, mSeqArc, mRiseSound);
            mState = STATE_MOVING;
            mFalling = FALSE;
        } else if (mValue <= mTarget) {
            mState = STATE_ARRIVED;
        } else {
            gSoundManager.StartSeqArc(1, mSeqArc, mFallSound);
            mState = STATE_MOVING;
            mFalling = TRUE;
        }
        break;
    case STATE_MOVING:
        if (mValue == mTarget) {
            gSoundManager.StopSeqArc(1, 0);
            if (!mFalling) {
                gSoundManager.StartSeqArc(1, mSeqArc, mRiseEndSound);
            } else {
                gSoundManager.StartSeqArc(1, mSeqArc, mFallEndSound);
            }
            mState = STATE_ARRIVED;
            if (mValue == 100) {
                mFull = TRUE;
            }
            if (mValue == 0) {
                mEmpty = TRUE;
            }
        } else if (mValue < mTarget) {
            mValue += mRiseStep;
            if (mValue > mTarget) {
                mValue = mTarget;
            }
            Show(mValue);
            mFalling = FALSE;
        } else if (mTarget < mValue) {
            mValue -= mFallStep;
            if (mValue < mTarget) {
                mValue = mTarget;
            }
            Show(mValue);
            mFalling = TRUE;
        }
        break;
    case STATE_ARRIVED:
        mState = STATE_IDLE;
        break;
    }
}

void Scene40Meter::SetMoveSounds(int seqArc, int riseSound, int fallSound) {
    mSeqArc = seqArc;
    mRiseSound = riseSound;
    mFallSound = fallSound;
}

void Scene40Meter::SetArriveSounds(int seqArc, int riseSound, int fallSound) {
    mSeqArc = seqArc;
    mRiseEndSound = riseSound;
    mFallEndSound = fallSound;
}

void Scene40Meter::SetSteps(int riseStep, int fallStep) {
    mRiseStep = riseStep;
    mFallStep = fallStep;
}

// Shows a value at once
void Scene40Meter::SetValue(int value) {
    if (value > 100) {
        value = 100;
    } else if (value < 0) {
        value = 0;
    }
    mTarget = mValue = value;
    Show(value);
}

// Where Update moves the value to
void Scene40Meter::SetTarget(int value) {
    if (value > 100) {
        value = 100;
    } else if (value < 0) {
        value = 0;
    }
    mTarget = value;
}

#define sSegmentMeterPosition SCENE40_POOL(ScreenPoint, 0x6C) // TEMP pool

// Sets the meter's sprites up, 2 pixels a segment apart, at the default position, showing 0
void Scene40SegmentMeter::Init(int resource, int sequence) {
    Scene40Meter::Init();
    ScreenPoint position = sSegmentMeterPosition;
    mSpacing = 2;
    for (int i = 0; i < 10; i++) {
        Sprite_Init(&mSegments[i], resource, sequence, 0x80, 0x60, TRUE);
        int offset = mSpacing * (i * 10 - 50);
        mSegments[i].SetPosition(FxVec2((offset + position.x) << FX32_SHIFT, position.y << FX32_SHIFT));
        mSegments[i].mOverwritePalette = 1;
        Sprite_SetFrame(&mSegments[i], 0);
        NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&mSegments[i].mAnim));
    }
}

void Scene40SegmentMeter::Draw() {
    for (int i = 0; i < 10; i++) {
        Sprite_Draw(&mSegments[i]);
    }
}

void Scene40SegmentMeter::SetPosition(ScreenPoint position) {
    for (int i = 0; i < 10; i++) {
        int offset = mSpacing * (i * 10 - 50);
        mSegments[i].SetPosition(FxVec2((offset + position.x) << FX32_SHIFT, position.y << FX32_SHIFT));
    }
}

// Each segment shows up to 10 of the value, as its frame
void Scene40SegmentMeter::Show(int value) {
    value = MATH_CLAMP(value, 0, 100);
    for (int i = 0; i < 10; i++) {
        int frame = MATH_CLAMP(value - i * 10, 0, 10);
        Sprite_SetFrame(&mSegments[i], (u16)frame);
    }
}

void Scene40SegmentMeter::SetVisible(BOOL visible) {
    for (int i = 0; i < 10; i++) {
        mSegments[i].mVisible = visible;
    }
}

#define sLevelTextPosition SCENE40_POOL(ScreenPoint, 0x2C) // TEMP pool

static const ScreenPoint sLevelMeterPosition = {151, 33};

// Level 1 with no points; the level's text box and the segments (resource 1, sequence 1)
void Scene40LevelMeter::Init(int *charOffset) {
    mLevel = mTargetLevel = 1;
    mPoints = mTargetPoints = 0;
    mTotal = mTargetTotal = mLevel * 100 + mPoints;
    mDistance = 0;
    mLevelText.Init(0x1000, 8, 2, *charOffset, 0, 1, 0, 0, NULL);
    SetCanvasPosition(&mLevelText, sLevelTextPosition);
    mLevelText.mColor = 8;
    mLevelText.mSprite.mOverwritePalette = 0;
    mLevelText.mSprite.mVisible = TRUE;
    *charOffset += mLevelText.GetCharCount();
    Scene40SegmentMeter::Init(1, 1);
    Scene40SegmentMeter::SetPosition(sLevelMeterPosition);
}

// When the points arrive at the target, a full meter goes on to the next level
void Scene40LevelMeter::Update() {
    mLevelChanged = FALSE;
    Scene40Meter::Update();
    if (mState != STATE_ARRIVED) {
        return;
    }
    mPoints = mTarget;
    mTotal = mLevel * 100 + mPoints;
    if (mFull) {
        mLevelChanged = TRUE;
        mLevel++;
        ShowLevel(mLevel);
    }
}

void Scene40LevelMeter::Draw() {
    Scene40SegmentMeter::Draw();
    mLevelText.Draw();
}

// Shows a level and its points at once
void Scene40LevelMeter::SetLevel(int level, int points) {
    mLevel = mTargetLevel = level;
    mPoints = mTargetPoints = points;
    mTotal = mTargetTotal = mLevel * 100 + points;
    mDistance = 0;
    ShowLevel(mLevel);
    SetValue(points);
}

// Sets where the meter goes: to the end of the meter if the target is in another level, else to its points. With
// beyondTop, a character at level 99 may go on to 100.
void Scene40LevelMeter::SetTargetLevel(int level, int points, BOOL beyondTop) {
    mTargetLevel = level;
    mTargetPoints = points;
    mTargetTotal = mTargetLevel * 100 + points;
    if (beyondTop && mTargetLevel >= 99 && mLevel == 99) {
        mTargetLevel++;
        mTargetTotal += 100;
    }
    mDistance = mTargetTotal - mTotal;
    if (mDistance > 0) {
        if (mTargetPoints < mPoints || mTargetTotal - mTotal >= 100) {
            SetTarget(100);
        } else {
            SetTarget(mTargetPoints);
        }
    } else if (mDistance < 0) {
        if (mPoints < mTargetPoints || mTotal - mTargetTotal >= 100) {
            SetTarget(0);
        } else {
            SetTarget(mTargetPoints);
        }
    }
}

// Starts the meter moving; after a level went up it starts from empty
void Scene40LevelMeter::Start() {
    if (mTotal <= mTargetTotal) {
        SetValue(mPoints = 0);
        if (mTargetTotal - mTotal >= 100) {
            SetTarget(100);
        } else {
            SetTarget(mTargetPoints);
        }
    }
    mState = STATE_START;
}

// "Ｌｖ" and the level, at most 99
void Scene40LevelMeter::ShowLevel(int level) {
    wchar_t text[10] = L"Ｌｖ";
    wchar_t number[10];
    if (level > 99) {
        level = 99;
    } else if (level < 0) {
        level = 0;
    }
    FormatNumber(level, number, sizeof(number), 2, TRUE);
    wcscat(text, number);
    mLevelText.SetText((const NNSG2dChar *)text);
    mLevelText.Redraw();
}

void Scene40LevelMeter::SetVisible(BOOL visible) {
    Scene40SegmentMeter::SetVisible(visible);
    mLevelText.mSprite.mVisible = visible;
}

// Shows the frame of the value: the sprite has 33 of them
void Scene40SpriteMeter::Init(int resource, int sequence) {
    Sprite_Init(&mSprite, resource, sequence, 0x80, 0x60, TRUE);
    mSprite.mOverwritePalette = 1;
    Sprite_SetFrame(&mSprite, 0);
    NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&mSprite.mAnim));
}

void Scene40SpriteMeter::Draw() {
    Sprite_Draw(&mSprite);
}

void Scene40SpriteMeter::SetPosition(ScreenPoint position) {
    mSprite.SetPosition(FxVec2(position.x << FX32_SHIFT, position.y << FX32_SHIFT));
}

void Scene40SpriteMeter::Show(int value) {
    if (value > 100) {
        value = 100;
    } else if (value < 0) {
        value = 0;
    }
    int frame = value / 3;
    if (frame > 32) {
        frame = 32;
    } else if (frame < 0) {
        frame = 0;
    }
    Sprite_SetFrame(&mSprite, (u16)frame);
}

void Scene40SpriteMeter::SetVisible(BOOL visible) {
    mSprite.mVisible = visible;
}

// --- The present the character receives -------------------------------------------------------------------------

void Scene40Gift::Set(int kind, int id) {
    mKind = kind;
    switch (kind) {
    case 0:
        mItem.InventoryItem::mId = id;
        mMessage = id;
        switch (gGoodsTable[mItem.InventoryItem::mId].mKind) {
        case 1:
            mGiftKind = 0;
            return;
        case 2:
            mGiftKind = 4;
            return;
        case 3:
            switch (mItem.InventoryItem::mId) {
            case 206:
                mGiftKind = 6;
                return;
            case 212:
                mGiftKind = 7;
                return;
            case 205:
                mGiftKind = 8;
                return;
            case 204:
                mGiftKind = 9;
                return;
            case 214:
                mGiftKind = 10;
                return;
            case 213:
                mGiftKind = 11;
                return;
            case 207:
                mGiftKind = 12;
                return;
            case 137:
                mGiftKind = 13;
                return;
            case 215:
                mGiftKind = 14;
                return;
            case 208:
                mGiftKind = 15;
                return;
            default:
                mGiftKind = 16;
                return;
            }
        default:
            mGiftKind = 16;
            return;
        }
    case 1:
        mItem.InventoryItem::mId = id;
        mGiftKind = 1;
        mMessage = 0xF7;
        return;
    case 2:
        mItem.InventoryItem::mId = id;
        mGiftKind = 2;
        mMessage = 0xF9;
        return;
    case 3:
        mItem.InventoryItem::mId = id;
        mGiftKind = 3;
        mMessage = 0xF4;
        return;
    case 4:
        mItem.InventoryItem::mId = id;
        mGiftKind = 5;
        mMessage = 0xF5;
        return;
    }
}

// --- The visited character's profile -------------------------------------------------------------------------------

// The goods whose marks the profile shows when the character has had them (0x0217ba2c)
extern const int sHadItemMarkItems[8];

extern const u32 sRelationMessages[][7];

// By relation group; [6] is the one while in a trouble of kind
                                                // 9 or 10 (0x0217c618; dsd's symbol at 0x0217c630 is [0][6])
extern const int sRelationRankKeys[];

// By RelationRank (0x0217bb08)
extern const int sMeterGrades[];

extern const u32 sGroupTitleMessages[];

// By relation group (0x0217ba70)                // Falling thresholds of the relation meter (0x0217b8f4)

void Scene40ProfilePage::Update() {
    mSatisfactionMeter.Update();
    mTroubleMeter.Update();
    ShowFullness();
    switch (mScreen) {
    case SCREEN_PROFILE:
        if (!mSwitchable) {
            return;
        }
        if (gPadInput.mTrigger & PAD_BUTTON_R) {
            SwitchScreen();
            gSoundManager.StartSeqArc(1, 10, 13);
        } else if (gPadInput.mTrigger & PAD_BUTTON_L) {
            SwitchScreen();
            gSoundManager.StartSeqArc(1, 10, 12);
        }
        break;
    case SCREEN_REDRAW:
        UpdateRedraw();
        break;
    case SCREEN_RELATIONS:
        UpdateRelations();
        break;
    case SCREEN_NO_RELATIONS:
        UpdateNoRelations();
        break;
    }
}

void Scene40ProfilePage::Draw() {
    mTitle.Draw();
    Sprite_Draw(&mMeterLabel);
    switch (mMeterKind) {
    case METER_SATISFACTION:
        mSatisfactionMeter.Draw();
        break;
    case METER_TROUBLE:
        mTroubleMeter.Draw();
        break;
    }
    mFullnessMeter.Draw();
    for (int i = 0; i < 3; i++) {
        mFavoriteFoods[i].Draw();
    }
    mFavoriteItem.Draw();
    mDislikedItem.Draw();
    Sprite_Draw(&mPartnerSexIcon);
    mPartnerName.Draw();
    mBestFriendName.Draw();
    for (int i = 0; i < 8; i++) {
        Sprite_Draw(&mHadItemMarks[i]);
    }
    Sprite_Draw(&mSprite0020);
    Sprite_Draw(&mSprite00CC);
    Sprite_Draw(&mCompleteMark);
    for (int i = 0; i < 11; i++) {
        mRelationNames[i].Draw();
        mRelationTexts[i].Draw();
    }
    for (int i = 0; i < 5; i++) {
        mGroupTitles[i].Draw();
    }
    mText8920.Draw();
    mTextD060.Draw();
}

// The trouble meter while the character has a trouble that progresses, else the satisfaction meter
void Scene40ProfilePage::ChooseMeterKind() {
    mMeterKind = METER_SATISFACTION;
    if (mCharacter->IsProgressingTrouble()) {
        mMeterKind = METER_TROUBLE;
    }
}

void Scene40ProfilePage::ShowMeterLabel() {
    switch (mMeterKind) {
    case METER_SATISFACTION:
        Sprite_SetSequence(&mMeterLabel, 24, TRUE);
        break;
    case METER_TROUBLE:
        Sprite_SetSequence(&mMeterLabel, 25, TRUE);
        break;
    }
}

void Scene40ProfilePage::Refresh() {
    ShowMeters();
    ShowFullness();
    ShowFavoriteFoods();
    ShowFavoriteItem();
    ShowDislikedItem();
    ShowPartner();
    ShowBestFriend();
    ShowHadItemMarks();
    wchar_t creator[11];
    BOOL complete = TRUE;
    MIi_CpuClear16(0, creator, sizeof(creator));
    MiiData_GetCreatorName(mCharacter, creator);
    SavedCharacter *character = mCharacter;
    if (wcslen(character->mFamilyName) == 0 || wcslen(character->mName) == 0 || wcslen(creator) == 0 ||
        character->mBirthYear == 0xFFF || character->GetParameter(PARAM_BIRTH_MONTH) == 0 ||
        mCharacter->GetParameter(PARAM_BIRTH_DAY) == 0 || mCharacter->mBloodType == 4) {
        complete = FALSE;
    }
    mCompleteMark.mVisible = complete;
    RedrawTexts();
}

void Scene40ProfilePage::ShowMeters() {
    if (mMeterKind == METER_TROUBLE) {
        mTroubleMeter.SetValue(GetTroubleMeterValue());
    }
    mSatisfactionMeter.SetLevel(mCharacter->mSatisfyLevel, mCharacter->mSatisfyMeter);
}

// Starts the meters moving to the character's values now
void Scene40ProfilePage::MoveMeters(BOOL beyondTop) {
    if (mMeterKind == METER_TROUBLE) {
        mTroubleMeter.SetTarget(GetTroubleMeterValue());
        mTroubleMeter.mState = Scene40Meter::STATE_START;
    }
    mSatisfactionMeter.SetTargetLevel(mCharacter->mSatisfyLevel, mCharacter->mSatisfyMeter, beyondTop);
    mSatisfactionMeter.mState = Scene40Meter::STATE_START;
}

void Scene40ProfilePage::ShowFullness() {
    mFullnessMeter.SetValue(mCharacter->GetFullness(mContext->mClock->mSeconds));
}

// NOT MATCHING (4 bytes short): the original halves mTroubleProgress (a u8) as a signed int, with the sign fix-up
// (add r0, r0, lsr #31) that ours leaves out as the compiler knows the byte is not negative; casts, a local int,
// one variable for both branches and an inline taking int do not keep it.
// The trouble meter shows 100 less half the trouble's progress (0 to 200)
int Scene40ProfilePage::GetTroubleMeterValue() {
    int value = 100;
    if (mCharacter->IsProgressingTroubleInState2()) {
        value = 100 - (int)mCharacter->GetTroubleProgress(mContext->mClock->mSeconds) / 2;
    } else if (mCharacter->IsProgressingTrouble()) {
        value = 100 - mCharacter->mTroubleProgress / 2;
    }
    return value;
}

void Scene40ProfilePage::ShowFavoriteFoods() {
    for (int i = 0; i < 3; i++) {
        u8 food = mCharacter->GetUnknown292(i);
        if (food != 0xFF) {
            mFavoriteFoods[i].mMessageFile = mContext->mItemMessages;
            mFavoriteFoods[i].mMessage = food;
            ClearMessageText(&mFavoriteFoods[i]);
            mFavoriteFoods[i].DrawAll();
        } else if (mShowUnknown) {
            mFavoriteFoods[i].mMessageFile = mContext->mMiiTouchMessages;
            mFavoriteFoods[i].mMessage = 15;
            ClearMessageText(&mFavoriteFoods[i]);
            mFavoriteFoods[i].DrawAll();
        }
    }
}

void Scene40ProfilePage::ShowFavoriteItem() {
    u8 item = mCharacter->mFavoriteItem;
    if (mCharacter->mUnknown1C8.m[item >> 5] & (1 << (item & 31))) {
        mFavoriteItem.mMessageFile = mContext->mItemMessages;
        mFavoriteItem.mMessage = item;
        ClearMessageText(&mFavoriteItem);
        mFavoriteItem.DrawAll();
    } else if (mShowUnknown) {
        mFavoriteItem.mMessageFile = mContext->mMiiTouchMessages;
        mFavoriteItem.mMessage = 15;
        ClearMessageText(&mFavoriteItem);
        mFavoriteItem.DrawAll();
    }
}

void Scene40ProfilePage::ShowDislikedItem() {
    u8 item = mCharacter->mDislikedItem;
    if (mCharacter->mUnknown1C8.m[item >> 5] & (1 << (item & 31))) {
        mDislikedItem.mMessageFile = mContext->mItemMessages;
        mDislikedItem.mMessage = item;
        ClearMessageText(&mDislikedItem);
        mDislikedItem.DrawAll();
    } else if (mShowUnknown) {
        mDislikedItem.mMessageFile = mContext->mMiiTouchMessages;
        mDislikedItem.mMessage = 15;
        ClearMessageText(&mDislikedItem);
        mDislikedItem.DrawAll();
    }
}

// The lover or spouse's name, and for a spouse the icon of their sex
void Scene40ProfilePage::ShowPartner() {
    int partner = mCharacter->FindCouplePartner();
    if (partner != -1) {
        SavedCharacter other;
        wchar_t name[11];
        GameData_ReadCharacter(&gGameData, partner, &other);
        MiiData_GetName(&other, name);
        mPartnerName.SetText((const char *)name);
        mPartnerName.Redraw();
        u8 rank = mCharacter->mRelationRank[partner];
        if (rank != RELATION_SPOUSES && rank != RELATION_SPOUSES_FIGHTING) {
            return;
        }
        if (MiiData_GetSex(&other) == 0) {
            Sprite_SetSequence(&mPartnerSexIcon, 22, TRUE);
        } else {
            Sprite_SetSequence(&mPartnerSexIcon, 23, TRUE);
        }
    } else if (mShowUnknown) {
        mPartnerName.SetText((const char *)mContext->mMiiTouchMessages->GetText(12));
        mPartnerName.Redraw();
    }
}

void Scene40ProfilePage::ShowBestFriend() {
    int friendIndex = mCharacter->FindBestFriend();
    if (friendIndex != -1) {
        SavedCharacter other;
        wchar_t name[11];
        GameData_ReadCharacter(&gGameData, friendIndex, &other);
        MiiData_GetName(&other, name);
        mBestFriendName.SetText((const char *)name);
        mBestFriendName.Redraw();
    } else if (mShowUnknown) {
        mBestFriendName.SetText((const char *)mContext->mMiiTouchMessages->GetText(12));
        mBestFriendName.Redraw();
    }
}

void Scene40ProfilePage::ShowHadItemMarks() {
    for (int i = 0; i < 8; i++) {
        int item = sHadItemMarkItems[i];
        mHadItemMarks[i].mVisible = (mCharacter->mUnknown1C8.m[item >> 5] >> (item & 31)) & 1;
    }
}

void Scene40ProfilePage::RedrawTexts() {
    for (int i = 0; i < 3; i++) {
        ClearMessageText(&mFavoriteFoods[i]);
        mFavoriteFoods[i].DrawAll();
    }
    ClearMessageText(&mFavoriteItem);
    mFavoriteItem.DrawAll();
    ClearMessageText(&mDislikedItem);
    mDislikedItem.DrawAll();
    ShowMeters();
    ShowPartner();
    ShowBestFriend();
}

void Scene40ProfilePage::ShowTitle() {
    wchar_t name[11];
    mTitle.mMessage = 17;
    MiiData_GetName(mCharacter, name);
    mTitle.SetStringArg(name, 0);
    ClearMessageText(&mTitle);
    mTitle.DrawAll();
}

void Scene40ProfilePage::ShowRelationsTitle() {
    wchar_t name[11];
    mTitle.mMessage = 18;
    MiiData_GetName(mCharacter, name);
    mTitle.SetStringArg(name, 0);
    ClearMessageText(&mTitle);
    mTitle.DrawAll();
    mText8920.mMessage = 19;
    ClearMessageText(&mText8920);
    mText8920.DrawAll();
}

void Scene40ProfilePage::ShowProfile() {
    ShowProfileScreen();
    mScreenIndex = 0;
    if (mRelationsChanged) {
        FindRelations();
    }
}

void Scene40ProfilePage::SetSwitchable(BOOL switchable) {
    mSwitchable = switchable;
    if (switchable) {
        Sprite_SetSequence(&mSprite00CC, 27, TRUE);
    } else {
        Sprite_SetSequence(&mSprite00CC, 28, TRUE);
    }
}

// The profile: its parts shown, the relations' hidden, and the upper screen's background
void Scene40ProfilePage::ShowProfileScreen() {
    mScreen = SCREEN_REDRAW;
    ShowTitle();
    mSprite0020.mVisible = TRUE;
    mMeterLabel.mVisible = TRUE;
    switch (mMeterKind) {
    case METER_SATISFACTION:
        mSatisfactionMeter.SetVisible(TRUE);
        break;
    case METER_TROUBLE:
        mTroubleMeter.SetVisible(TRUE);
        break;
    }
    mFullnessMeter.SetVisible(TRUE);
    for (int i = 0; i < 3; i++) {
        mFavoriteFoods[i].mSprite.mVisible = TRUE;
    }
    mFavoriteItem.mSprite.mVisible = TRUE;
    mDislikedItem.mSprite.mVisible = TRUE;
    mPartnerSexIcon.mVisible = TRUE;
    mPartnerName.mSprite.mVisible = TRUE;
    mBestFriendName.mSprite.mVisible = TRUE;
    ShowHadItemMarks();
    for (int i = 0; i < 5; i++) {
        mGroupTitles[i].mSprite.mVisible = FALSE;
    }
    for (int i = 0; i < 11; i++) {
        mRelationNames[i].mSprite.mVisible = FALSE;
        mRelationTexts[i].mSprite.mVisible = FALSE;
    }
    mText8920.mSprite.mVisible = FALSE;
    mTextD060.mSprite.mVisible = FALSE;
    ScreenLoader_LoadFile(&gScreenLoader, "SMT:/BG_up.nsc", 0x100, 0, 0, 0, 0, -1);
    mFirstGroup = 0;
    mPage = 0;
}

// The relations (or, without any, a message): the profile's parts hidden
void Scene40ProfilePage::ShowRelationsScreen() {
    if (mHasRelations) {
        mScreen = SCREEN_RELATIONS;
    } else {
        mScreen = SCREEN_NO_RELATIONS;
    }
    ShowRelationsTitle();
    mSprite0020.mVisible = FALSE;
    mMeterLabel.mVisible = FALSE;
    switch (mMeterKind) {
    case METER_SATISFACTION:
        mSatisfactionMeter.SetVisible(FALSE);
        break;
    case METER_TROUBLE:
        mTroubleMeter.SetVisible(FALSE);
        break;
    }
    mFullnessMeter.SetVisible(FALSE);
    for (int i = 0; i < 3; i++) {
        mFavoriteFoods[i].mSprite.mVisible = FALSE;
    }
    mFavoriteItem.mSprite.mVisible = FALSE;
    mDislikedItem.mSprite.mVisible = FALSE;
    mPartnerSexIcon.mVisible = FALSE;
    mPartnerName.mSprite.mVisible = FALSE;
    mBestFriendName.mSprite.mVisible = FALSE;
    for (int i = 0; i < 8; i++) {
        mHadItemMarks[i].mVisible = FALSE;
    }
    for (int i = 0; i < 5; i++) {
        mGroupTitles[i].mSprite.mVisible = FALSE;
    }
    for (int i = 0; i < 11; i++) {
        mRelationNames[i].mSprite.mVisible = FALSE;
        mRelationTexts[i].mSprite.mVisible = FALSE;
    }
    mText8920.mSprite.mVisible = FALSE;
    ScreenLoader_LoadFile(&gScreenLoader, "SMT:/BG_relation.nsc", 0x100, 0, 0, 0, 0, -1);
}

void Scene40ProfilePage::UpdateRedraw() {
    mScreen = SCREEN_PROFILE;
    RedrawTexts();
}

#define sFirstGroupTitlePosition SCENE40_POOL(ScreenPoint, 0x13C) // TEMP pool

// Draws the relations' page mScreenIndex - 1: a title for each group, its relations' names and what they are
void Scene40ProfilePage::UpdateRelations() {
    mScreen = SCREEN_PROFILE;
    int titles = 0;
    mFirstGroup = mGroups[mPage];
    mText8920.mSprite.mVisible = TRUE;
    SetCanvasPosition(&mGroupTitles[titles], sFirstGroupTitlePosition);
    mGroupTitles[titles].mSprite.mVisible = TRUE;
    mGroupTitles[titles].mMessage = sGroupTitleMessages[mFirstGroup];
    mGroupTitles[titles].mColor = 8;
    ClearMessageText(&mGroupTitles[titles]);
    mGroupTitles[titles].DrawAll();
    titles++;
    BOOL groupEnded = FALSE;
    int lines = 0;
    int y = 36;
    while (y < 180 && mPageStarts[mScreenIndex - 1] + lines < mRelationCount) {
        if (groupEnded) {
            if (mGroups[mPage] <= mLastGroup) {
                mFirstGroup = mGroups[mPage];
                mGroupTitles[titles].mSprite.SetPosition(FxVec2(20 << FX32_SHIFT, (y - 14) << FX32_SHIFT));
                mGroupTitles[titles].mSprite.mVisible = TRUE;
                mGroupTitles[titles].mMessage = sGroupTitleMessages[mFirstGroup];
                mGroupTitles[titles].mColor = 8;
                ClearMessageText(&mGroupTitles[titles]);
                mGroupTitles[titles].DrawAll();
                titles++;
            }
            groupEnded = FALSE;
        }
        mRelationNames[lines].mSprite.SetPosition(FxVec2(28 << FX32_SHIFT, y << FX32_SHIFT));
        mRelationTexts[lines].mSprite.SetPosition(FxVec2(138 << FX32_SHIFT, y << FX32_SHIFT));
        int lineHeight;
        lines++;
        if (mPageStarts[mScreenIndex - 1] + lines == mGroupStarts[mFirstGroup] + mGroupCounts[mFirstGroup]) {
            mPage++;
            groupEnded = TRUE;
            lineHeight = 28;
        } else {
            lineHeight = 14;
        }
        y += lineHeight;
    }
    for (; titles < 5; titles++) {
        mGroupTitles[titles].mSprite.mVisible = FALSE;
    }
    int i = 0;
    for (int relation = mPageStarts[mScreenIndex - 1]; i < 11 && i < lines && relation < mRelationCount; relation++) {
        mRelationNames[i].SetText((const char *)mRelations[relation].mName);
        int group = GetSortKeyGroup(mRelations[relation].mSortKey);
        mRelationTexts[i].mMessage =
            GetRelationMessage(group, GetMeterGrade(GetSortKeyMeter(mRelations[relation].mSortKey)));
        mRelationTexts[i].mColor = 8;
        mRelationNames[i].Redraw();
        mRelationNames[i].mSprite.mVisible = TRUE;
        ClearMessageText(&mRelationTexts[i]);
        mRelationTexts[i].DrawAll();
        mRelationTexts[i].mSprite.mVisible = TRUE;
        i++;
    }
    if (i >= 11) {
        return;
    }
    for (; i < 11; i++) {
        mRelationNames[i].mSprite.mVisible = FALSE;
        mRelationTexts[i].mSprite.mVisible = FALSE;
    }
}

void Scene40ProfilePage::UpdateNoRelations() {
    mScreen = SCREEN_PROFILE;
    *(BOOL *)((u8 *)&mTextD060 + 0x64) = TRUE;
    ClearMessageText(&mTextD060);
    mTextD060.DrawAll();
}

// L and R: the next screen, after the last the profile again
void Scene40ProfilePage::SwitchScreen() {
    if (mScreenCount <= 0) {
        return;
    }
    if (mScreenIndex < mScreenCount - 1) {
        mScreenIndex = mScreenIndex + 1;
    } else {
        mScreenIndex = 0;
    }
    if (mScreenIndex < 1) {
        ShowProfileScreen();
    } else {
        ShowRelationsScreen();
    }
}

static s32 CompareRelations(void *a, void *b);

// Lists the characters the visited one has a relation with, sorted, grouped by rank and cut into pages
void Scene40ProfilePage::FindRelations() {
    mRelationCount = 0;
    mRelationsChanged = FALSE;
    for (int i = 0; i < 100; i++) {
        if (!GameData_IsCharacterSlotUsed(&gGameData, i)) {
            continue;
        }
        if (i == mCharacter->mIndex || mCharacter->mRelationRank[i] == RELATION_NONE) {
            continue;
        }
        MIi_CpuClear16(0, mRelations[mRelationCount].mName, sizeof(mRelations[0].mName));
        SavedCharacter other;
        GameData_ReadCharacter(&gGameData, i, &other);
        MiiData_GetName(&other, mRelations[mRelationCount].mName);
        mRelations[mRelationCount].mSortKey =
            GetRelationSortKey(mCharacter->mRelationRank[i], mCharacter->mRelationMeter[i]);
        mRelations[mRelationCount].mIndex = i;
        mRelationCount++;
    }
    u32 buffer[200];
    MATH_QSort(mRelations, mRelationCount, sizeof(Scene40Relation), CompareRelations, buffer);
    for (int i = 0; i < 9; i++) {
        mGroups[i] = 0;
        mGroupCounts[i] = 0;
        mGroupStarts[i] = 0;
    }
    int group = -1;
    int n = -1;
    for (int i = 0; i < mRelationCount; i++) {
        int g = GetSortKeyGroup(mRelations[i].mSortKey);
        if (group < g) {
            n++;
            while (group < g) {
                group++;
            }
            mGroups[n] = group;
            mGroupStarts[group] = i;
        }
        mGroupCounts[mGroups[n]]++;
    }
    mLastGroup = group;
    if (mRelationCount > 0) {
        mHasRelations = TRUE;
        mScreenCount = 1;
        int k = 0;
        int current = mGroups[0];
        mFirstGroup = current;
        int page = 0;
        for (int start = 0; start < mRelationCount;) {
            mPageStarts[page] = start;
            int lines = 0;
            int height = 36;
            do {
                int lineHeight;
                lines++;
                if (start + lines == mGroupStarts[current] + mGroupCounts[current]) {
                    k++;
                    if (mGroups[k] <= mLastGroup) {
                        current = mGroups[k];
                    }
                    lineHeight = 28;
                } else {
                    lineHeight = 14;
                }
                height += lineHeight;
            } while (height < 180);
            mScreenCount++;
            start += lines;
            page++;
        }
    } else {
        mHasRelations = FALSE;
        mScreenCount = 2;
    }
    mPage = 0;
}

// NOT MATCHING: the trouble table's address, category and id take r1, r2, r3 in the original, r2, r3, r1 here;
// a pointer to the entry, a pointer to the row, the character or the id in locals leave them so.
// The message of a relation group's line: groups 0 and 2 have their seventh while the character is in a trouble
// of kind 9 or 10
u32 Scene40ProfilePage::GetRelationMessage(int group, int index) {
    if ((group == 0 || group == 2) && mCharacter->IsInTrouble()) {
        switch (data_02114d78[mCharacter->mTroubleCategory][mCharacter->mTroubleId].kind) {
        case 9:
        case 10:
            return sRelationMessages[group][6];
        }
    }
    return sRelationMessages[group][index];
}

// Sorts the relations by rank (in the order of sRelationRankKeys) and, within a rank, by the meter, highest first
int Scene40ProfilePage::GetRelationSortKey(int rank, int meter) {
    return sRelationRankKeys[rank] - 1 - meter;
}

int Scene40ProfilePage::GetSortKeyGroup(int key) {
    return key >> 14;
}

// The meter a sort key was made of
int Scene40ProfilePage::GetSortKeyMeter(int key) {
    return ((GetSortKeyGroup(key) + 1) << 14) - key - 1;
}

// The grade of a meter: how many of sMeterGrades' (falling) thresholds it is below, at most 7
int Scene40ProfilePage::GetMeterGrade(int meter) {
    int grade = 0;
    for (;;) {
        if (meter >= sMeterGrades[grade]) {
            break;
        }
        grade++;
        if (grade >= 7) {
            break;
        }
    }
    return grade;
}

static s32 CompareRelations(void *a, void *b) {
    return ((Scene40Relation *)a)->mSortKey - ((Scene40Relation *)b)->mSortKey;
}

// --- The scene's counter ------------------------------------------------------------------------------------------

#define sCounterTextPosition SCENE40_POOL(ScreenPoint, 0x184) // TEMP pool

void Scene40CounterDisplay::Init(Scene40Context *context, int *charOffset) {
    mContext = context;
    mState = STATE_IDLE;
    mFalling = FALSE;
    mCounter = mContext->mMoney;
    mShown = *mCounter;
    mChange = 0;
    Sprite_Init(&mIcon, 0, 25, 0x80, 0x60, TRUE);
    mIcon.SetPosition(FxVec2(193 << FX32_SHIFT, 180 << FX32_SHIFT));
    mIcon.mOverwritePalette = 1;
    mText.mDrawFlags = NNS_G2D_VERTICALORIGIN_TOP | NNS_G2D_HORIZONTALORIGIN_RIGHT | NNS_G2D_HORIZONTALALIGN_LEFT;
    mText.mTextX = 64;
    mText.InitText(&gFonts[0].mFont, 16, 8, 2, *charOffset, -2, 1, 0, 0, 15, NULL, 2, NNS_G2D_CHARA_COLORMODE_16);
    mText.mColor = 1;
    NNS_G2dCharCanvasClear(&mText.mCanvas, 0);
    SetCanvasPosition(&mText, sCounterTextPosition);
    mText.mSprite.mOverwritePalette = 1;
    *charOffset += mText.GetCharCount();
    Redraw();
}

void Scene40CounterDisplay::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_START:
        if (mShown < *mCounter) {
            gSoundManager.StartSeqArc(1, 27, 4);
            mState = STATE_COUNTING;
            mFalling = FALSE;
        } else if (mShown <= *mCounter) {
            mState = STATE_ARRIVED;
        } else {
            gSoundManager.StartSeqArc(1, 27, 5);
            mState = STATE_COUNTING;
            mFalling = TRUE;
        }
        break;
    case STATE_COUNTING:
        if (mShown == *mCounter) {
            gSoundManager.StopSeqArc(1, 0);
            if (!mFalling) {
                gSoundManager.StartSeqArc(1, 27, 6);
            } else {
                gSoundManager.StartSeqArc(1, 27, 7);
            }
            mState = STATE_ARRIVED;
        } else if (mShown < *mCounter) {
            mShown += 50;
            if (mShown > *mCounter) {
                mShown = *mCounter;
            }
            Redraw();
            mFalling = FALSE;
        } else if (mShown > *mCounter) {
            mShown -= 50;
            if (mShown < *mCounter) {
                mShown = *mCounter;
            }
            Redraw();
            mFalling = TRUE;
        }
        break;
    case STATE_ARRIVED:
        mState = STATE_IDLE;
        break;
    }
}

void Scene40CounterDisplay::Draw() {
    if (mContext->mSpeechWindow->mShown) {
        return;
    }
    mText.Draw();
    Sprite_Draw(&mIcon);
}

// Changes the counter at once; the number shown follows
void Scene40CounterDisplay::Add(int amount) {
    *mCounter += amount;
    *mCounter = MATH_CLAMP(*mCounter, 0, 9999999);
    mChange = *mCounter - mShown;
    Redraw();
}

void Scene40CounterDisplay::AddTimes60(int count) {
    if (count < 0) {
        count = 0;
    }
    Add(count * 60);
}

void Scene40CounterDisplay::Redraw() {
    wchar_t text[10];
    FormatNumberWithCommas(mShown, text, sizeof(text), 0, FALSE);
    mText.mText = (const NNSG2dChar *)text;
    NNS_G2dCharCanvasClear(&mText.mCanvas, 0);
    mText.DrawText();
}

// --- The scene's character ------------------------------------------------------------------------------------------

void Scene40Character::Init(int resourceId, int unk, u32 defaultMii) {
    Character::Init(resourceId, unk, defaultMii);
    mMotion.Reset();
    mState = mNextState = 0;
    mEntered = TRUE;
    mHeldItem = NULL;
    mLipSync = TRUE;
    mUnknown30A0 = 0;
    mUnknown2ED0 = FALSE;
    mUnknown2ED4 = FALSE;
    mUnknown2ED8 = FALSE;
    mUnknown2EDC = FALSE;
    mUnknown1C4Added[0] = FALSE;
    mUnknown1C4Added[1] = FALSE;
    mUnknown1C4Added[2] = FALSE;
    mUnknown2EE0[0] = 0;
    mUnknown2EE0[1] = 0;
    mUnknown2EE0[2] = 0;
    mUnknown3094 = TRUE;
}

// Both head icons show the icon of the mood, over the head
void Scene40Character::LoadHeadIcons(ParticleSystem *particles) {
    for (int i = 0; i < 2; i++) {
        mHeadIcons[i].Load(particles, sMoodHeadIcons[mMood]);
        mHeadIconPlaces[i] = ICON_AT_HEAD;
    }
}

void Scene40Character::PlaceHeadIcons() {
    for (int i = 0; i < 2; i++) {
        switch (mHeadIconPlaces[i]) {
        case ICON_AT_HEAD:
            PlaceHeadIconAtHead(i);
            break;
        case ICON_AT_BODY:
            PlaceHeadIconAtBody(i);
            break;
        case ICON_AT_FEET:
            PlaceHeadIconAtFeet(i);
            break;
        case ICON_AT_HAND:
            PlaceHeadIconAtHand(i);
            break;
        }
    }
}

void Scene40Character::PlaceHeadIconAtHead(int icon) {
    mHeadIcons[icon].mAngle = 0x4000 - mAngle + mHeadYawNow;
    MtxFx43 *matrices = mJointMatrices;
    mHeadIcons[icon].SetPos(matrices[JOINT_NECK]._30, matrices[JOINT_NECK]._31, matrices[JOINT_NECK]._32);
}

void Scene40Character::PlaceHeadIconAtBody(int icon) {
    mHeadIcons[icon].mAngle = 0x4000 - mAngle;
    MtxFx43 *matrices = mJointMatrices;
    mHeadIcons[icon].SetPos(matrices[JOINT_BODY]._30, matrices[JOINT_BODY]._31, matrices[JOINT_BODY]._32);
}

void Scene40Character::PlaceHeadIconAtFeet(int icon) {
    mHeadIcons[icon].mAngle = 0x4000 - mAngle;
    VecFx32 *pos = &mPos;
    mHeadIcons[icon].SetPos(pos->x, pos->y, pos->z);
}

void Scene40Character::PlaceHeadIconAtHand(int icon) {
    mHeadIcons[icon].mAngle = 0x4000 - mAngle;
    MtxFx43 *matrices = mJointMatrices;
    mHeadIcons[icon].SetPos(matrices[JOINT_R_HAND]._30, matrices[JOINT_R_HAND]._31, matrices[JOINT_R_HAND]._32);
}

// The first head icon shows the mood's icon over the head, from its start; none in the first mood
void Scene40Character::ShowMoodHeadIcon() {
    mHeadIconPlaces[0] = ICON_AT_HEAD;
    if (mMood == 0) {
        mHeadIcons[0].SetMode(1);
        return;
    }
    mHeadIcons[0].mAngle = 0x4000 - mAngle + mHeadYawNow;
    s16 id = sMoodHeadIcons[mMood];
    if (mHeadIcons[0].mIconId != id) {
        mHeadIcons[0].Show();
        mHeadIcons[0].mIconId = id;
        mHeadIcons[0].Reload();
        mHeadIcons[0].SetMode(3);
    } else {
        mHeadIcons[0].Restart();
        mHeadIcons[0].SetMode(3);
    }
}

void Scene40Character::InitBalloon(int resourceId, int unk, CharacterBase *face) {
    mBalloon.Init(resourceId, unk, face, this);
}

// 0: the bed, 1: the dressing curtain
void Scene40Character::InitCover(int kind) {
    mCover.Init(kind);
}

// Puts the bed where the character lies down, at the character's scale
void Scene40Character::ShowBed(VecFx32 position, u16 angle, GXRgb color) {
    if (mCover.mKind != Scene40Cover::KIND_BED) {
        return;
    }
    mCover.SetPosition(position, angle);
    mCover.SetDiffuse(color);
    mCover.mModel.mVisible = TRUE;
    SetModelScale(&mCover.mModel, GetHeldItemScale());
}

void Scene40Character::SetContext(Scene40Context *context) {
    mContext = context;
    mTtsPlayer = mContext->mTtsPlayer;
    mSpeechWindow = (Scene40SpeechWindow *)mContext->mSpeechWindow;
}

// Runs the group function of the state's range
void Scene40Character::UpdateState() {
    mState = mNextState;
    if (mState <= 0x19) {
        UpdateStates00To19();
    } else if (mState <= 0x1C) {
        UpdateStates1ATo1C();
    } else if (mState <= 0x27) {
        UpdateStates1DTo27();
    } else if (mState <= 0x2D) {
        UpdateStates28To2D();
    } else if (mState <= 0x43) {
        UpdateStates2ETo43();
    } else if (mState <= 0x47) {
        UpdateStates44To47();
    } else if (mState <= 0x4E) {
        UpdateStates48To4E();
    } else if (mState <= 0x66) {
        UpdateStates4FTo66();
    } else if (mState <= 0x85) {
        UpdateStates67To85();
    } else if (mState <= 0xB2) {
        UpdateStates86ToB2();
    } else if (mState <= 0xBC) {
        UpdateStatesB3ToBC();
    } else if (mState <= 0xC5) {
        UpdateStatesBDToC5();
    } else if (mState <= 0xD1) {
        UpdateStatesC6ToD1();
    }
    mFrame++;
}

// Whether a character in a mood has a motto for it, and says it by a chance
static inline BOOL HasText(const wchar_t *text) { return wcslen(text) != 0; }

static inline BOOL HasMotto(Scene40Character *character, int mood, const wchar_t *motto) {
    return character->mMood == mood && HasText(motto);
}

static inline BOOL WillSayMotto(Scene40Character *character, int mood, const wchar_t *motto, int chance) {
    return HasMotto(character, mood, motto) && Random(100) < chance;
}

// 0x1A: stands, looks at the camera, and after 40 frames says its motto of the mood, if it has one and by a chance
// of the mood (the first mood rarely), else something chosen for a touch (ChooseTouchTalk); back to state 0 when
// the speech is over. 0x1B: walks round to face the camera, then 0x1C: faces it and waits for the speech (TellNews
// at frame 30, TellNews), then state 0.
void Scene40Character::UpdateStates1ATo1C() {
    Camera *camera = &gCamera;
    FloorPos pos;
    switch (mState) {
    case 0x1A:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mStep = mFrame = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame >= mWait) {
                mStep++;
            }
            break;
        case 1:
            if (WillSayMotto(this, 0, mMotto, 20)) {
                Speak(0, 0x11E, 8, 0, 0);
            } else if (WillSayMotto(this, 1, mMottos[0], 80)) {
                Speak(0, 0x11E, 9, 0, 0);
            } else if (WillSayMotto(this, 2, mMottos[1], 80)) {
                Speak(0, 0x11E, 10, 0, 0);
            } else if (WillSayMotto(this, 3, mMottos[2], 80)) {
                Speak(0, 0x11E, 11, 0, 0);
            } else if (WillSayMotto(this, 4, mMottos[3], 80)) {
                Speak(0, 0x11E, 12, 0, 0);
            } else {
                u32 message;
                int count;
                int types[2];
                int values[2];
                ChooseTouchTalk(&message, &count, types, values);
                Speak(1, message, count, types, values, 0);
            }
            mStep++;
            break;
        case 2:
            if (mTtsPlayer->IsEngineIdle()) {
                SetState(0);
            }
            break;
        }
        break;
    case 0x1B:
        if (mEntered) {
            mEntered = FALSE;
            ResetHeadTurn();
            BlendToNamedAnimation(sWalkAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            ResetSoundPhase(0);
            UpdateSpeedScales();
            pos.x = camera->mEye.x;
            pos.z = camera->mEye.z;
            SetTargetFloorPos(pos);
            return;
        }
        if (TurnToWalkTarget(0x280, 0)) {
            SetState(0x1C);
        }
        MoveAlongAngle();
        PlaySoundOnce(1, 0, 26);
        break;
    case 0x1C:
        if (mEntered) {
            mEntered = FALSE;
            UpdateCameraAngle();
            if (IsFacingCamera()) {
                BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
                UpdateAnimSpeed();
                TurnHeadToCamera();
            } else {
                SetState(0x1B);
            }
        } else if (mFrame == 30) {
            TellNews();
        }
        if (mTtsPlayer->IsEngineIdle() && mFrame >= 30) {
            SetState(0);
        }
        break;
    }
}

// 0x28: walks to the camera holding the food it was given; if it cannot eat (CanEat) 0x44, if it ate the same last
// and is not hungry (fullness 20 or more) 0x46, else 0x29: waits, says 0x100, marks the goods as had, and for food
// (gift kind 0) 0x2A eats ("i_eat00": bites over the animation's length, at most 80 frames), 0x2B finishes ("i_eat01",
// Eat), 0x2C walks back to where it stood (the camera's side) and stands, then 0x2E; other gifts go to 0x4F. 0x2D:
// greets ("greet01", says 0x102), then 0x30 when the speech is over.
void Scene40Character::UpdateStates28To2D() {
    Camera *camera = &gCamera;
    FloorPos pos;
    switch (mState) {
    case 0x28:
        if (mEntered) {
            mEntered = FALSE;
            ResetHeadTurn();
            BlendToNamedAnimation("i_walk00");
            UpdateAnimSpeed();
            ResetSoundPhase(0);
            UpdateSpeedScales();
            pos.x = camera->mEye.x;
            pos.z = camera->mEye.z;
            SetTargetFloorPos(pos);
            return;
        }
        PlaceHeldItem();
        MoveAlongAngle();
        PlaySoundOnce(1, 0, 26);
        if (!TurnToWalkTarget(0x280, 0)) {
            return;
        }
        if (gGoodsTable[mHeldItem->InventoryItem::mId].mKind == 1) {
            s64 now = mContext->mClock->mSeconds;
            if (!CanEat(now, mGift.mItem.InventoryItem::mId)) {
                SetState(0x44);
            } else if (mEatLastFood == mGift.mItem.InventoryItem::mId && GetFullness(now) >= 20) {
                SetState(0x46);
            } else {
                SetState(0x29);
            }
        } else {
            SetState(0x29);
        }
        break;
    case 0x29:
        if (mEntered) {
            mEntered = FALSE;
            BlendToNamedAnimation("i_wait00");
            GetAnimation()->mPlayOnce = FALSE;
            UpdateAnimSpeed();
            Speak(0, 0x100, 0);
            TurnHeadToCamera();
            AddUnknown1C4ByTrouble();
            mUnknown1C8.m[mGift.mItem.InventoryItem::mId >> 5] |= 1 << (mGift.mItem.InventoryItem::mId & 31);
            return;
        }
        PlaceHeldItem();
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        if (mGift.mGiftKind == 0) {
            SetState(0x2A);
        } else {
            SetState(0x4F);
        }
        break;
    case 0x2A:
        if (mEntered) {
            mEntered = FALSE;
            BlendToNamedAnimation("i_eat00");
            UpdateAnimSpeed();
            ResetHeadTurn();
            RankLikedItem(mGift.mItem.InventoryItem::mId);
            mEatLastFood = mGift.mItem.InventoryItem::mId;
            mWait = 80;
            mStep = 0;
            const ItemInfo *info = &gGoodsTable[mGift.mItem.InventoryItem::mId];
            if (info->mFlags & 0x10000) {
                if (Random(100) < 50) {
                    mUnknown24E = 0;
                }
            }
            return;
        }
        if (mStep == 0) {
            ModelAnimation *animation = GetAnimation();
            int frames = (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10 + 1;
            int wait = frames * 25 / 10;
            if (wait > 80) {
                mWait = wait;
                mUnknown3098 = 0;
                mUnknown309C = 3;
            } else {
                mUnknown3098 = 0;
                int bites = 80 / frames;
                mUnknown309C = bites + 1;
                mWait = bites * frames + frames / 2;
            }
            mStep++;
        }
        PlaceHeldItem();
        func_ov075_02174a14(this);
        UpdateMouthByAnimation();
        func_ov075_02174b38(this, mGift.mItem.InventoryItem::mId);
        if (mFrame < mWait) {
            return;
        }
        mHeldItem->Hide();
        mHeldItem = NULL;
        SetState(0x2B);
        break;
    case 0x2B:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(1);
            BlendToNamedAnimation("i_eat01");
            GetAnimation()->mPlayOnce = TRUE;
            return;
        }
        if (!GetAnimation()->mFinished) {
            return;
        }
        Eat(mContext->mClock->mSeconds, mGift.mItem.InventoryItem::mId);
        SetState(0x2C);
        break;
    case 0x2C:
        if (mEntered) {
            mEntered = FALSE;
            pos.x = camera->mEye.x;
            pos.z = camera->mEye.z;
            SetTargetFloorPos(pos);
            if (!TurnToWalkTarget(0x280, 0)) {
                BlendToNamedAnimation(sWalkAnimations[gPersonalityKinds[mPersonality]][mMood]);
                ResetSoundPhase(0);
                mStep = 0;
                return;
            }
            BlendToNamedAnimation("idle00");
            mStep = 2;
            return;
        }
        switch (mStep) {
        case 0:
            MoveAlongAngle();
            PlaySoundOnce(1, 0, 26);
            if (!TurnToWalkTarget(0x280, 0)) {
                return;
            }
            if (mBlendFrames > 0) {
                mStep = 1;
                return;
            }
            BlendToNamedAnimation("idle00");
            mStep = 2;
            return;
        case 1:
            if (mBlendFrames > 0) {
                return;
            }
            BlendToNamedAnimation("idle00");
            mStep++;
            break;
        case 2:
            if (mBlendFrames > 0) {
                return;
            }
            mUnknown30A0 = TRUE;
            SetState(0x2E);
            break;
        }
        break;
    case 0x2D:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(1);
            BlendToNamedAnimation("greet01");
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = TRUE;
            Speak(0, 0x102, 0);
            return;
        }
        if (GetAnimation()->mFinished) {
            BlendToNamedAnimation("idle00");
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            return;
        }
        if (mTtsPlayer->IsEngineIdle()) {
            SetState(0x30);
        }
        break;
    }
}

// --- The scene's clock, at 0x30A8C --------------------------------------------------------------------------------







extern "C" {
void ParticleSystem_Draw(ParticleSystem *particles, int unknown); // Draws the particles
void GameData_WriteCharacter(GameData *game, int index, SavedCharacter *character); // Writes a saved character
void DestroyMainFrameHeap(void);
BOOL ArchiveManager_IsIdle(ArchiveLoader *loader);
extern int gGameClock[];
s64 Clock_GetDateTime(int offset, RTCDate *date, RTCTime *time); // The date and time now, and the same in seconds

// Not decompiled yet
extern const u32 sSongGiftMessages[6];        // "You presented enka!" ...: the message of each song genre given (miitouch file)
extern const char *const sWaitAnimations[4][5]; // The standing animations ("wait00".."wait09"), by personality kind and mood
extern const char *const sWalkAnimations[4][5]; // The walking animations ("walk00".."walk10"), by personality kind and mood
extern MATHRandContext16 gRandom;
void MiiData_GetName(CharacterRecord *character, wchar_t *name);
void GameData_ReadCharacter(GameData *game, int index, SavedCharacter *character);
int func_ov075_0217520c(Scene40Character *character);
void func_ov075_02174a14(Scene40Character *character);
void func_ov075_02174c00(Scene40Character *character);
void ScreenControl_SetBlendAlpha(ScreenControl *screens, int a, int b, int c, int d);
void ScreenControl_SetBlendBrightness(ScreenControl *screens, int planes, int level);
}

// --- The scene's character's states 0x44 to 0xB2 (0x02168354 to 0x021722c8) ----------------------------------------

// The character gives back food it will not eat: "I am full" (0x44) or "I ate this a moment ago" (0x46), then
// waits (0x45, 0x47)
void Scene40Character::UpdateStates44To47() {
    Camera *camera = &gCamera;
    FloorPos target;

    switch (mState) {
    case 0x44:
        if (mEntered) {
            mEntered = FALSE;
            PlayAnimation("i_wait00", FALSE);
            UpdateAnimSpeed();
            Speak(0, 0x104, 0);
            TurnHeadToCamera();
            mStep = 0;
            return;
        }
        PlaceHeldItem();
        switch (mStep) {
        case 0:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            BlendToNamedAnimation("i_walk00");
            ResetHeadTurn();
            mStep++;
            break;
        case 1:
            MoveAlongAngle();
            if (!TurnToWalkTarget(0x280, 0)) {
                return;
            }
            SetAnimSpeed(0xA66);
            PlayAnimation("i_give00", TRUE);
            TurnHeadToCamera();
            mStep++;
            break;
        case 2:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mFrame = 0;
            mStep++;
            break;
        case 3:
            mWait = 31;
            if (mFrame < mWait) {
                return;
            }
            UpdateAnimSpeed();
            BlendToNamedAnimation("idle00");
            mHeldItem->Hide();
            mHeldItem = NULL;
            mFrame = 0;
            mStep++;
            break;
        case 4:
            mWait = 30;
            if (mFrame < mWait) {
                return;
            }
            SetState(0x45);
            break;
        }
        break;
    case 0x45:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x46:
        if (mEntered) {
            mEntered = FALSE;
            PlayAnimation("i_wait00", FALSE);
            UpdateAnimSpeed();
            TurnHeadToCamera();
            mStep = 0;
            mFrame = 0;
            return;
        }
        PlaceHeldItem();
        switch (mStep) {
        case 0:
            if (mFrame < 30) {
                return;
            }
            Speak(0, 0x106, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            BlendToNamedAnimation("i_walk00");
            ResetHeadTurn();
            mStep++;
            break;
        case 2:
            MoveAlongAngle();
            if (!TurnToWalkTarget(0x280, 0)) {
                return;
            }
            SetAnimSpeed(0xA66);
            PlayAnimation("i_give00", TRUE);
            TurnHeadToCamera();
            mStep++;
            break;
        case 3:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mFrame = 0;
            mStep++;
            break;
        case 4:
            if (mFrame >= 20) {
                mHeldItem->mVisible = FALSE;
            }
            if (mFrame < 40) {
                return;
            }
            UpdateAnimSpeed();
            BlendToNamedAnimation("idle00");
            mHeldItem->Hide();
            mHeldItem = NULL;
            mFrame = 0;
            mStep++;
            break;
        case 5:
            mWait = 30;
            if (mFrame < mWait) {
                return;
            }
            SetState(0x47);
            break;
        }
        break;
    case 0x47:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    }
}

// The character receives a present from the player: walks to face the camera (0x48), holds it up (0x49) and shows
// what it was in the speech window, takes it (0x4A, then waits in 0x4B), or waits (0x4C); 0x4D holds the last frame
// of holding up and then goes back to state 0; 0x4E speaks a message before taking it
void Scene40Character::UpdateStates48To4E() {
    Camera *camera = &gCamera;
    FloorPos target;

    switch (mState) {
    case 0x48:
        if (mEntered) {
            mEntered = FALSE;
            ResetHeadTurn();
            BlendToNamedAnimation("i_walk00");
            UpdateAnimSpeed();
            ResetSoundPhase(0);
            UpdateSpeedScales();
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            return;
        }
        PlaceHeldItem();
        MoveAlongAngle();
        PlaySoundOnce(1, 0, 26);
        if (!TurnToWalkTarget(0x280, 0)) {
            return;
        }
        SetState(0x49);
        break;
    case 0x49:
        if (mEntered) {
            mEntered = FALSE;
            PlayAnimation("i_hoist00", TRUE);
            mHeadIcons[1].Show();
            mHeadIcons[1].mIconId = 4;
            mHeadIcons[1].Reload();
            mHeadIcons[1].SetMode(3);
            mHeadIconPlaces[1] = 3;
            return;
        }
        PlaceHeldItemInHand();
        switch (mStep) {
        case 0:
            if (!GetAnimation()->mFinished) {
                return;
            }
            u32 message;
            int mode = SpeechWindow::MODE_FRAMES;
            switch (mGift.mGiftKind) {
            case 3:
                message = sSongGiftMessages[mGift.mItem.InventoryItem::mId];
                SetSongSlot(mGift.mItem.InventoryItem::mId, 0xFF);
                mUnknown299 |= 1 << (mGift.mItem.InventoryItem::mId & 0x1F);
                break;
            case 4:
                switch (mGift.mItem.InventoryItem::mId) {
                case 236:
                    message = 0xA5;
                    break;
                case 241:
                    message = 0xA6;
                    break;
                case 243:
                    message = 0xA7;
                    break;
                case 242:
                    message = 0xA8;
                    break;
                case 239:
                    message = 0xA9;
                    break;
                case 240:
                    message = 0xAA;
                    break;
                case 237:
                    message = 0xAB;
                    break;
                case 238:
                    message = 0xAC;
                    break;
                }
                mUnknown1C8.m[mGift.mItem.InventoryItem::mId >> 5] |= 1 << (mGift.mItem.InventoryItem::mId & 0x1F);
                StartLongActWithTool(mGift.mItem.InventoryItem::mId);
                break;
            case 5:
                message = 0xB2;
                mode = SpeechWindow::MODE_MANUAL;
                break;
            case 2:
                message = 0xB0;
                break;
            }
            gSoundManager.StartSeqIfNotPlaying(0, 22);
            mSpeechWindow->mMessageFile = mContext->mMiiTouchMessages;
            mSpeechWindow->mText[0].mHAlign = 1;
            mSpeechWindow->Show(message, mode, 120);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            mWait = 120;
            if (mFrame < mWait) {
                return;
            }
            switch (mGift.mGiftKind) {
            case 3:
            case 4:
                SetState(0x4A);
                break;
            case 5:
                SetState(0x4E);
                break;
            case 2:
                SetState(0x4C);
                break;
            }
            break;
        }
        break;
    case 0x4A:
        if (mEntered) {
            mEntered = FALSE;
            PlayAnimation("i_get01", TRUE);
            UpdateAnimSpeed();
            ResetHeadTurn();
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0: {
            ModelAnimation *animation = GetAnimation();
            int frames = (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10 + 1;
            int half = frames / 2;
            int threeQuarters = frames * 3 / 4;
            if (mFrame < half) {
                PlaceHeldItem();
                mHeldItem->SetScale(((half - mFrame) << FX32_SHIFT) / half);
            } else if (mFrame == half) {
                mHeldItem->Hide();
                mHeldItem = NULL;
            } else if (mFrame == threeQuarters) {
                gSoundManager.StartSeqArc(1, 26, 8);
            } else if (GetAnimation()->mFinished && mFrame > threeQuarters) {
                BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
                mStep++;
            }
            break;
        }
        case 1:
            if (mBlendFrames > 0) {
                return;
            }
            SetState(0x4B);
            break;
        }
        break;
    case 0x4B:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x4C:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x4D:
        if (mEntered) {
            mEntered = FALSE;
            SetNamedAnimation("i_hoist00");
            ModelAnimation *animation = GetAnimation();
            SetAnimFlagsAndFrame(TRUE, (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) - 1, TRUE);
            return;
        }
        if (mFrame < 30) {
            return;
        }
        SetState(0);
        break;
    case 0x4E:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            Speak(0, 0x11E, 8, 0, 0);
            return;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        SetState(0x4A);
        break;
    }
}

// The character receives clothes, an interior or goods it was given, or gives them back:
// 0x4F takes what it holds (as 0x4A) and waits in 0x50;
// 0x51 to 0x54 clothes: it faces the camera, and changes into them behind the curtain (0x52, then state 0x2E) or
// says "I already have these clothes" and gives them back (0x53, then waits in 0x54);
// 0x55, 0x56: it walks to the middle of the room and changes behind the curtain into what it wears;
// 0x57 to 0x61 an interior: it faces the camera and holds it up (0x58, then waits in 0x59), or says "I already have
// this interior" and gives it back (0x5A, 0x5B); 0x5C to 0x61 say what it thinks of its new room, from "I like it
// very much" to "personally it is a bit...", then state 0x33;
// 0x62 to 0x66 another present: it faces the camera, holds it up, speaks of it for 120 frames and takes it.
void Scene40Character::UpdateStates4FTo66() {
    Camera *camera = &gCamera;
    FloorPos target;

    switch (mState) {
    case 0x4F:
        if (mEntered) {
            mEntered = FALSE;
            PlayAnimation("i_get01", TRUE);
            UpdateAnimSpeed();
            ResetHeadTurn();
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0: {
            ModelAnimation *animation = GetAnimation();
            int frames = (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10 + 1;
            int half = frames / 2;
            int threeQuarters = frames * 3 / 4;
            if (mFrame < half) {
                PlaceHeldItem();
                mHeldItem->SetScale(((half - mFrame) << FX32_SHIFT) / half);
            } else if (mFrame == half) {
                mHeldItem->Hide();
                mHeldItem = NULL;
            } else if (mFrame == threeQuarters) {
                gSoundManager.StartSeqArc(1, 26, 8);
            } else if (GetAnimation()->mFinished && mFrame > threeQuarters) {
                BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
                mStep++;
            }
            break;
        }
        case 1:
            if (mBlendFrames > 0) {
                return;
            }
            SetState(0x50);
            break;
        }
        break;
    case 0x50:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x51:
        if (mEntered) {
            mEntered = FALSE;
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            if (!TurnToWalkTarget(0x280, 0)) {
                BlendToNamedAnimation("i_walk00");
                ResetSoundPhase(0);
                mStep = 0;
            } else {
                BlendToNamedAnimation("i_wait00");
                mStep = 2;
            }
            UpdateAnimSpeed();
            return;
        }
        PlaceHeldItem();
        switch (mStep) {
        case 0:
            PlaySoundOnce(1, 0, 26);
            if (TurnToWalkTarget(0x280, 0)) {
                if (mBlendFrames <= 0) {
                    BlendToNamedAnimation("i_wait00");
                    mStep = 2;
                } else {
                    mStep++;
                }
            }
            MoveAlongAngle();
            break;
        case 1:
            if (mBlendFrames > 0) {
                return;
            }
            BlendToNamedAnimation("i_wait00");
            mStep++;
            break;
        case 2:
            if (mBlendFrames > 0) {
                return;
            }
            if ((mOwnedClothes.m[mGift.mItem.InventoryItem::mId >> 5] >> (mGift.mItem.InventoryItem::mId & 0x1F)) & 1) {
                SetState(0x53);
            } else {
                SetState(0x52);
            }
            break;
        }
        break;
    case 0x52:
        if (mEntered) {
            mEntered = FALSE;
            BlendToNamedAnimation("i_wait00");
            UpdateAnimSpeed();
            mCover.SetKind(Scene40Cover::KIND_CURTAIN);
            mCover.SetPosition(mPos, mAnim.mRotY);
            mCover.Appear();
            gSoundManager.StartSeqArc(1, 10, 1);
            if (Random(100) < 50) {
                mUnknown24E = 1;
            }
            return;
        }
        switch (mStep) {
        case 0:
            PlaceHeldItem();
            if (!mCover.HasAppeared()) {
                return;
            }
            mHeldItem->Hide();
            mHeldItem = NULL;
            mCover.PlayWait();
            mFrame = 0;
            mStep++;
            break;
        case 1:
            mWait = 52;
            if (mFrame == 10 || mFrame == 36) {
                gSoundManager.StartSeqArc(1, 10, 2);
            }
            if (mFrame < mWait) {
                return;
            }
            mClothesId = mGift.mItem.InventoryItem::mId;
            OwnClothes(mGift.mItem.InventoryItem::mId);
            AddUnknown1C4ByTrouble();
            ModelResources_ReleaseModel(gModelResources, 0);
            ArchiveManager_Unmount(&gArchiveManager, mArchiveName);
            SetArchive(mArchiveName);
            ArchiveManager_MountCompressed(&gArchiveManager, mArchiveName, mArchivePath, 1);
            ModelResources_SetModel(gModelResources, 0, mModelPath);
            MakeModel(0, 0);
            UpdateBodyColors();
            BlendToNamedAnimation("idle00");
            UpdateAnimSpeed();
            mFrame = 0;
            mStep++;
            break;
        case 2:
            mWait = 52;
            if (mFrame == 10 || mFrame == 36) {
                gSoundManager.StartSeqArc(1, 10, 2);
            }
            if (mFrame < mWait) {
                return;
            }
            mCover.Disappear();
            gSoundManager.StartSeqArc(1, 10, 0);
            mFrame = 0;
            mStep++;
            break;
        case 3:
            if (!mCover.HasDisappeared()) {
                return;
            }
            mCover.mModel.mVisible = FALSE;
            mUnknown30A0 = TRUE;
            SetState(0x2E);
            break;
        }
        break;
    case 0x53:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x10A, 0);
            TurnHeadToCamera();
            BlendToNamedAnimation("i_wait00");
            mStep = 0;
            return;
        }
        PlaceHeldItem();
        switch (mStep) {
        case 0:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetAnimSpeed(0xA66);
            PlayAnimation("i_give00", TRUE);
            mStep++;
            break;
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mFrame = 0;
            mStep++;
            break;
        case 2:
            if (mFrame >= 20) {
                mHeldItem->mVisible = FALSE;
            }
            if (mFrame < 40) {
                return;
            }
            mFrame = 0;
            mStep++;
            UpdateAnimSpeed();
            BlendToNamedAnimation("idle00");
            mHeldItem->Hide();
            mHeldItem = NULL;
            break;
        case 3:
            mWait = 30;
            if (mFrame < mWait) {
                return;
            }
            SetState(0x54);
            break;
        }
        break;
    case 0x54:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x55:
        if (mEntered) {
            mEntered = FALSE;
            ShowBalloon(3);
            ResetHeadTurn();
            mUnknown2EA0.x = 0;
            mUnknown2EA0.z = 0;
            SetTargetFloorPos(mUnknown2EA0);
            if (!WalkToTarget(ScaleBySpeed(0x7A), 0x22, 4)) {
                ResetSoundPhase(0);
                BlendToNamedAnimation(sWalkAnimations[gPersonalityKinds[mPersonality]][mMood]);
                mStep = 0;
                MoveAlongAngle();
            } else {
                target.x = camera->mEye.x;
                target.z = camera->mEye.z;
                SetTargetFloorPos(target);
                MoveAlongAngle();
                if (!TurnToWalkTarget(0x280, 0)) {
                    ResetSoundPhase(0);
                    BlendToNamedAnimation(sWalkAnimations[gPersonalityKinds[mPersonality]][mMood]);
                    mStep = 1;
                } else {
                    BlendToNamedAnimation("idle00");
                    mStep = 3;
                }
                MoveAlongAngle();
            }
            return;
        }
        switch (mStep) {
        case 0:
            PlaySoundOnce(1, 0, 26);
            if (WalkToTarget(ScaleBySpeed(0x7A), 0x22, 4)) {
                target.x = camera->mEye.x;
                target.z = camera->mEye.z;
                SetTargetFloorPos(target);
                mStep++;
            }
            MoveAlongAngle();
            break;
        case 1:
            PlaySoundOnce(1, 0, 26);
            if (TurnToWalkTarget(0x280, 0)) {
                mStep++;
            }
            MoveAlongAngle();
            break;
        case 2:
            if (mBlendFrames > 0) {
                return;
            }
            BlendToNamedAnimation("idle00");
            mStep++;
            break;
        case 3:
            if (mBlendFrames > 0) {
                return;
            }
            SetState(0x56);
            break;
        }
        break;
    case 0x56:
        if (mEntered) {
            mEntered = FALSE;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 20;
            if (mFrame < mWait) {
                return;
            }
            mCover.SetKind(Scene40Cover::KIND_CURTAIN);
            mCover.SetPosition(mPos, mAnim.mRotY);
            mCover.Appear();
            gSoundManager.StartSeqArc(1, 10, 1);
            mStep++;
            break;
        case 1:
            if (!mCover.HasAppeared()) {
                return;
            }
            mCover.PlayWait();
            mFrame = 0;
            mStep++;
            break;
        case 2:
            mWait = 52;
            if (mFrame == 10 || mFrame == 36) {
                gSoundManager.StartSeqArc(1, 10, 2);
            }
            if (mFrame < mWait) {
                return;
            }
            ModelResources_ReleaseModel(gModelResources, 0);
            ArchiveManager_Unmount(&gArchiveManager, mArchiveName);
            SetArchive(mArchiveName);
            ArchiveManager_MountCompressed(&gArchiveManager, mArchiveName, mArchivePath, 1);
            ModelResources_SetModel(gModelResources, 0, mModelPath);
            MakeModel(0, 0);
            UpdateBodyColors();
            mFrame = 0;
            mStep++;
            break;
        case 3:
            mWait = 52;
            if (mFrame == 10 || mFrame == 36) {
                gSoundManager.StartSeqArc(1, 10, 2);
            }
            if (mFrame < mWait) {
                return;
            }
            mCover.Disappear();
            gSoundManager.StartSeqArc(1, 10, 0);
            mFrame = 0;
            mStep++;
            break;
        case 4:
            if (!mCover.HasDisappeared()) {
                return;
            }
            mCover.mModel.mVisible = FALSE;
            SetState(0);
            break;
        }
        break;
    case 0x57:
        if (mEntered) {
            mEntered = FALSE;
            ResetHeadTurn();
            BlendToNamedAnimation("i_walk00");
            UpdateAnimSpeed();
            ResetSoundPhase(0);
            UpdateSpeedScales();
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            return;
        }
        PlaceHeldItem();
        MoveAlongAngle();
        PlaySoundOnce(1, 0, 26);
        if (!TurnToWalkTarget(0x280, 0)) {
            return;
        }
        if (mOwnedInteriors[mGift.mItem.InventoryItem::mId >> 5] & (1 << (mGift.mItem.InventoryItem::mId & 0x1F))) {
            SetState(0x5A);
        } else {
            SetState(0x58);
        }
        break;
    case 0x58:
        if (mEntered) {
            mEntered = FALSE;
            PlayAnimation("i_hoist00", TRUE);
            AddUnknown1C4ByTrouble();
            return;
        }
        PlaceHeldItemInHand();
        switch (mStep) {
        case 0:
            if (!GetAnimation()->mFinished) {
                return;
            }
            gSoundManager.StartSeqArc(1, 10, 20);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            mWait = 60;
            if (mFrame < mWait) {
                return;
            }
            SetState(0x59);
            break;
        }
        break;
    case 0x59:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x5A:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x116, 0);
            TurnHeadToCamera();
            BlendToNamedAnimation("i_wait00");
            mStep = 0;
            return;
        }
        PlaceHeldItem();
        switch (mStep) {
        case 0:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetAnimSpeed(0xA66);
            PlayAnimation("i_give00", TRUE);
            mStep++;
            break;
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mFrame = 0;
            mStep++;
            break;
        case 2:
            if (mFrame >= 20) {
                mHeldItem->mVisible = FALSE;
            }
            if (mFrame < 40) {
                return;
            }
            mFrame = 0;
            mStep++;
            UpdateAnimSpeed();
            BlendToNamedAnimation("idle00");
            mHeldItem->Hide();
            mHeldItem = NULL;
            break;
        case 3:
            mWait = 30;
            if (mFrame < mWait) {
                return;
            }
            SetState(0x5B);
            break;
        }
        break;
    case 0x5B:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x5C:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x10C, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x5D:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x10C, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x5E:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x10E, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x5F:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x110, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x60:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x112, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x61:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x114, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x62:
        if (mEntered) {
            mEntered = FALSE;
            ResetHeadTurn();
            BlendToNamedAnimation("i_walk00");
            UpdateAnimSpeed();
            ResetSoundPhase(0);
            UpdateSpeedScales();
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            return;
        }
        PlaceHeldItem();
        MoveAlongAngle();
        PlaySoundOnce(1, 0, 26);
        if (!TurnToWalkTarget(0x280, 0)) {
            return;
        }
        SetState(0x63);
        break;
    case 0x63:
        if (mEntered) {
            mEntered = FALSE;
            PlayAnimation("i_hoist00", TRUE);
            return;
        }
        PlaceHeldItemInHand();
        switch (mStep) {
        case 0:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 1:
            mWait = 30;
            if (mFrame < mWait) {
                return;
            }
            SetState(0x64);
            break;
        }
        break;
    case 0x64:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x11E, 8, 0, 0);
            mFrame = 0;
            return;
        }
        if (mFrame < 120) {
            return;
        }
        SetState(0x65);
        break;
    case 0x65:
        if (mEntered) {
            mEntered = FALSE;
            PlayAnimation("i_get01", TRUE);
            UpdateAnimSpeed();
            ResetHeadTurn();
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0: {
            ModelAnimation *animation = GetAnimation();
            int frames = (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10 + 1;
            int half = frames / 2;
            int threeQuarters = frames * 3 / 4;
            if (mFrame < half) {
                PlaceHeldItem();
                mHeldItem->SetScale(((half - mFrame) << FX32_SHIFT) / half);
            } else if (mFrame == half) {
                mHeldItem->Hide();
                mHeldItem = NULL;
            } else if (mFrame == threeQuarters) {
                gSoundManager.StartSeqArc(1, 26, 8);
            } else if (GetAnimation()->mFinished && mFrame > threeQuarters) {
                BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
                mStep++;
            }
            break;
        }
        case 1:
            if (mBlendFrames > 0) {
                return;
            }
            SetState(0x66);
            break;
        }
        break;
    case 0x66:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    }
}

// What the character does with a present it has been given, by the present (the trouble messages it speaks):
// 0x67 the camera ("I'll go and take a picture": the screen fades out, it comes back holding the photo, scene
// 0xBF shows it); 0x68 "The title is ..."; 0x69 to 0x6C what it says to the player's opinion of the photo;
// 0x6D a present to try out ("Please try it", scene 0x55); 0x6E it wakes with a start ("What was I doing just now?");
// 0x6F the bath ("I'll go and take a bath", scene 0xB0) and 0x70, 0x71 "That was refreshing";
// 0x72 food: it eats, in bites, then 0x73 or 0x77 by what the food was; 0x74 to 0x7A medicine for a cold (the
// sound group changes and a message shows what it did, then "I feel the fever has gone down", "Quite bitter", "I
// feel much better", "A strange taste"); 0x7B, 0x7C the fan ("I'll cool off", scene 0x54, "Nicely cooled");
// 0x7D, 0x7E the music box (scene 0xA3); 0x7F, 0x80 cooking (scene 0xAA); 0x81, 0x82 an experience (scene 0x22);
// 0x83 to 0x85 an outing it is sent on, alone (scene 0x78) or with a partner (scene 0x79), "Here is a souvenir".
void Scene40Character::UpdateStates67To85() {
    Camera *camera = &gCamera;
    FloorPos target;
    int brightness;

    switch (mState) {
    case 0x67:
        if (mEntered) {
            mEntered = FALSE;
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            if (!TurnToWalkTarget(0x280, 0)) {
                BlendToNamedAnimation("i_walk00");
                ResetSoundPhase(0);
                mStep = 0;
            } else {
                BlendToNamedAnimation("i_wait00");
                mStep = 2;
            }
            UpdateAnimSpeed();
            return;
        }
        PlaceHeldItem();
        switch (mStep) {
        case 0:
            PlaySoundOnce(1, 0, 26);
            if (TurnToWalkTarget(0x280, 0)) {
                if (mBlendFrames <= 0) {
                    BlendToNamedAnimation("i_wait00");
                    mStep = 2;
                } else {
                    mStep++;
                }
            }
            MoveAlongAngle();
            break;
        case 1:
            if (mBlendFrames > 0) {
                return;
            }
            BlendToNamedAnimation("i_wait00");
            mStep++;
            break;
        case 2:
            if (mBlendFrames > 0) {
                return;
            }
            Speak(0, 0x12E, 0);
            TurnHeadToCamera();
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            gSoundManager.MoveSeqVolume(0, 0, 30);
            mStep++;
            mFrame = 0;
            break;
        case 4:
            brightness = mFrame * 2;
            if (brightness > 16) {
                brightness = 16;
            } else if (brightness < 0) {
                brightness = 0;
            }
            ScreenControl_SetBlendBrightness(&gScreenControl, 0x3F3F, brightness);
            if (mFrame < 8) {
                return;
            }
            mStep++;
            mFrame = 0;
            mHeldItem->Hide();
            mHeldItem = NULL;
            SetNamedAnimation("idle00");
            break;
        case 5:
            if (mFrame == 32) {
                gSoundManager.StartSeqArc(1, 10, 18);
            }
            if (mFrame == 170) {
                gSoundManager.MoveSeqVolume(0, 127, 30);
            }
            if (mFrame < 192) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 6:
            brightness = mFrame * 2;
            if (brightness > 16) {
                brightness = 16;
            } else if (brightness < 0) {
                brightness = 0;
            }
            ScreenControl_SetBlendBrightness(&gScreenControl, 0x3F3F, 16 - brightness);
            if (mFrame < 8) {
                return;
            }
            mStep++;
            mFrame = 0;
            ScreenControl_SetBlendAlpha(&gScreenControl, 4, 17, 8, 8);
            break;
        case 7:
            if (mFrame < 40) {
                return;
            }
            Speak(0, 0x130, 0);
            TurnHeadToCamera();
            mStep++;
            mFrame = 0;
            break;
        case 8:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            UpdateAnimSpeed();
            PlayAnimation("i_give01", TRUE);
            mContext->mPhysics->TakeFreeItem(&mHeldItem);
            mHeldItem->ChangeItem(250);
            mHeldItem->mVisible = FALSE;
            mStep++;
            mFrame = 0;
            break;
        case 9:
            if (!GetAnimation()->mFinished) {
                ModelAnimation *animation = GetAnimation();
                int frames = (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10 + 1;
                int quarter = frames / 4;
                int half = frames / 2;
                if (mFrame == quarter) {
                    gSoundManager.StartSeqArc(1, 26, 8);
                } else if (mFrame == half) {
                    fx32 scale = ((mFrame - half + 1) << FX32_SHIFT) / (frames - half);
                    mHeldItem->SetScale(scale);
                    mHeldItem->mVisible = TRUE;
                    PlaceHeldItem(scale);
                } else if (mFrame > half) {
                    fx32 scale = ((mFrame - half + 1) << FX32_SHIFT) / (frames - half);
                    mHeldItem->SetScale(scale);
                    PlaceHeldItem(scale);
                }
            } else {
                PlayAnimation("i_give00", TRUE);
                SetAnimSpeed(0xA66);
                mHeldItem->SetScale(FX32_ONE);
                PlaceHeldItem();
                mFrame = 0;
                mStep++;
            }
            break;
        case 10:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mFrame = 0;
            mStep++;
            gSoundManager.StartSeqArc(1, 10, 19);
            break;
        case 11:
            brightness = mFrame * 2;
            if (brightness > 16) {
                brightness = 16;
            } else if (brightness < 0) {
                brightness = 0;
            }
            ScreenControl_SetBlendBrightness(&gScreenControl, 0x3F3F, brightness);
            if (mFrame < 8) {
                return;
            }
            mFrame = 0;
            mStep++;
            break;
        case 12:
            if (mFrame < 32) {
                return;
            }
            gSceneManager.RequestScene(0xBF);
            gSceneMemory.mCharacters[0] = mIndex;
            break;
        }
        break;
    case 0x68:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x132, 7, 0, 2);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x69:
        if (mEntered) {
            mEntered = FALSE;
            mStep = 0;
            mFrame = 0;
            TurnHeadToCamera();
            SetFace(2);
            mContext->mTtsPlayer->StopEngine();
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 45) {
                return;
            }
            Speak(0, 0x134, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x6A:
        if (mEntered) {
            mEntered = FALSE;
            mStep = 0;
            mFrame = 0;
            TurnHeadToCamera();
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 20) {
                return;
            }
            Speak(0, 0x136, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x6B:
        if (mEntered) {
            mEntered = FALSE;
            mStep = 0;
            mFrame = 0;
            TurnHeadToCamera();
            SetFace(6);
            mContext->mTtsPlayer->StopEngine();
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 60) {
                return;
            }
            Speak(0, 0x138, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x6C:
        if (mEntered) {
            mEntered = FALSE;
            mStep = 0;
            mFrame = 0;
            TurnHeadToCamera();
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 20) {
                return;
            }
            Speak(0, 0x13A, 2, mIndex, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x6D:
        if (mEntered) {
            mEntered = FALSE;
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            if (!TurnToWalkTarget(0x280, 0)) {
                BlendToNamedAnimation("i_walk00");
                ResetSoundPhase(0);
                mStep = 0;
            } else {
                BlendToNamedAnimation("i_wait00");
                mStep = 2;
            }
            UpdateAnimSpeed();
            return;
        }
        PlaceHeldItem();
        switch (mStep) {
        case 0:
            PlaySoundOnce(1, 0, 26);
            if (TurnToWalkTarget(0x280, 0)) {
                if (mBlendFrames <= 0) {
                    BlendToNamedAnimation("i_wait00");
                    mStep = 2;
                } else {
                    mStep++;
                }
            }
            MoveAlongAngle();
            break;
        case 1:
            if (mBlendFrames > 0) {
                return;
            }
            BlendToNamedAnimation("i_wait00");
            mStep++;
            break;
        case 2:
            if (mBlendFrames > 0) {
                return;
            }
            mFrame = 0;
            mStep++;
            break;
        case 3:
            if (mFrame < 30) {
                return;
            }
            mFrame = 0;
            mStep++;
            mHeadTurn = 1;
            mHeadRoll = 0xF500;
            break;
        case 4:
            if (mFrame < 70) {
                return;
            }
            TurnHeadToCamera();
            mFrame = 0;
            mStep++;
            break;
        case 5:
            if (mFrame < 50) {
                return;
            }
            Speak(0, 0x124, 0);
            mStep++;
            break;
        case 6:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            gSceneManager.RequestScene(0x55);
            for (u8 i = 0; i < 12; i++) {
                gSceneMemory.mCharacters[i] = 0xFF;
            }
            gSceneMemory.mCharacters[0] = mIndex;
            break;
        }
        break;
    case 0x6E:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(6);
            BlendToNamedAnimation("idle00");
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            ResetHeadTurn();
            StopHeadTurn();
            mUnknown24A = 0;
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 120;
            if (mFrame < mWait) {
                return;
            }
            SetFace(5);
            SetFace(5);
            SetAnimFlags(TRUE, TRUE);
            mStep += 2;
            mLipSync = FALSE;
            Speak(0, 0x126, 0);
            mFrame = 0;
            break;
        case 2:
            if (mFrame < 80) {
                return;
            }
            mLipSync = TRUE;
            SetFace(8);
            SetAnimFlags(FALSE, FALSE);
            mStep++;
            mFrame = 0;
            break;
        case 3:
            if (mFrame < 15) {
                return;
            }
            TurnHeadToCamera();
            mStep++;
            mFrame = 0;
            break;
        case 4:
            mWait = 60;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x128, 0);
            mStep++;
            break;
        case 5:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0);
            break;
        }
        break;
    case 0x6F:
        if (mEntered) {
            mEntered = FALSE;
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            if (!TurnToWalkTarget(0x280, 0)) {
                BlendToNamedAnimation("i_walk00");
                ResetSoundPhase(0);
                mStep = 0;
            } else {
                BlendToNamedAnimation("i_wait00");
                mStep = 2;
            }
            UpdateAnimSpeed();
            return;
        }
        PlaceHeldItem();
        switch (mStep) {
        case 0:
            PlaySoundOnce(1, 0, 26);
            if (TurnToWalkTarget(0x280, 0)) {
                if (mBlendFrames <= 0) {
                    BlendToNamedAnimation("i_wait00");
                    mStep = 2;
                } else {
                    mStep++;
                }
            }
            MoveAlongAngle();
            break;
        case 1:
            if (mBlendFrames > 0) {
                return;
            }
            BlendToNamedAnimation("i_wait00");
            mStep++;
            break;
        case 2:
            if (mBlendFrames > 0) {
                return;
            }
            Speak(0, 0x12A, 0);
            TurnHeadToCamera();
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            ClearMoodIfTroubleOfKind8();
            gSceneManager.RequestScene(0xB0);
            gSceneMemory.mCharacters[0] = mIndex;
            break;
        }
        break;
    case 0x70:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x12C, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0);
            break;
        }
        break;
    case 0x71:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x12C, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x72:
        if (mEntered) {
            mEntered = FALSE;
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            if (!TurnToWalkTarget(0x280, 0)) {
                BlendToNamedAnimation("i_walk00");
                ResetSoundPhase(0);
                mStep = 0;
            } else {
                BlendToNamedAnimation("i_wait00");
                mStep = 2;
            }
            UpdateAnimSpeed();
            ShowBalloon(3);
            return;
        }
        PlaceHeldItem();
        switch (mStep) {
        case 0:
            PlaySoundOnce(1, 0, 26);
            if (TurnToWalkTarget(0x280, 0)) {
                if (mBlendFrames <= 0) {
                    BlendToNamedAnimation("i_wait00");
                    mStep = 2;
                } else {
                    mStep++;
                }
            }
            MoveAlongAngle();
            break;
        case 1:
            if (mBlendFrames > 0) {
                return;
            }
            BlendToNamedAnimation("i_wait00");
            mStep++;
            break;
        case 2:
            if (mBlendFrames > 0) {
                return;
            }
            Speak(0, 0x100, 0);
            TurnHeadToCamera();
            mStep++;
            break;
        case 3: {
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            BlendToNamedAnimation("i_eat00");
            UpdateAnimSpeed();
            ResetHeadTurn();
            ModelAnimation *animation = GetAnimation();
            int frames = (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10 + 1;
            int length = frames * 25 / 10;
            if (length > 80) {
                mWait = length;
                mUnknown3098 = 0;
                mUnknown309C = 3;
            } else {
                mWait = 80;
                mUnknown3098 = 0;
                mUnknown309C = 80 / frames + 1;
                mWait = 80 / frames * frames + frames / 2;
            }
            mFrame = 0;
            mStep++;
            break;
        }
        case 4:
            PlaceHeldItem();
            func_ov075_02174a14(this);
            UpdateMouthByAnimation();
            func_ov075_02174c00(this);
            if (mFrame < mWait) {
                return;
            }
            mHeldItem->Hide();
            mHeldItem = NULL;
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][0]);
            mStep++;
            break;
        case 5:
            if (mBlendFrames > 0) {
                return;
            }
            switch (mGift.mGiftKind) {
            case 8:
                SetState(0x73);
                break;
            case 9:
                SetState(0x77);
                break;
            }
            break;
        }
        break;
    case 0x73:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x74:
        if (mEntered) {
            mEntered = FALSE;
            TurnHeadToCamera();
            gSoundManager.StopSeq(0, 1);
            mHeadIcons[0].SetMode(1);
            mHeadIcons[1].SetMode(1);
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 90) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x75);
            gSoundManager.StartSeq(0, 0x85);
            gSoundManager.LoadGroup(0x4D);
            PlayAnimation("happy00", TRUE);
            SetAnimSpeed(FX32_ONE);
            SetFace(2);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->Show(0x1B0, SpeechWindow::MODE_MANUAL, 120);
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (mFrame < 150) {
                return;
            }
            Speak(0, 0x14C, 0);
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x75:
        if (mEntered) {
            mEntered = FALSE;
            TurnHeadToCamera();
            gSoundManager.StopSeq(0, 1);
            mHeadIcons[0].SetMode(1);
            mHeadIcons[1].SetMode(1);
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 0) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x75);
            gSoundManager.StartSeq(0, 0x85);
            gSoundManager.LoadGroup(0x4D);
            PlayAnimation("happy00", TRUE);
            SetAnimSpeed(FX32_ONE);
            SetFace(2);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->Show(0x1B1, SpeechWindow::MODE_MANUAL, 120);
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (mFrame < 150) {
                return;
            }
            Speak(0, 0x14C, 0);
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x76:
        if (mEntered) {
            mEntered = FALSE;
            TurnHeadToCamera();
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 90) {
                return;
            }
            Speak(0, 0x14E, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x77:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x78:
        if (mEntered) {
            mEntered = FALSE;
            TurnHeadToCamera();
            gSoundManager.StopSeq(0, 1);
            mHeadIcons[0].SetMode(1);
            mHeadIcons[1].SetMode(1);
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 90) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x76);
            gSoundManager.StartSeq(0, 0x86);
            gSoundManager.LoadGroup(0x4D);
            PlayAnimation("happy00", TRUE);
            SetAnimSpeed(FX32_ONE);
            SetFace(2);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->Show(0x1B2, SpeechWindow::MODE_MANUAL, 120);
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (mFrame < 180) {
                return;
            }
            Speak(0, 0x150, 0);
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x79:
        if (mEntered) {
            mEntered = FALSE;
            TurnHeadToCamera();
            gSoundManager.StopSeq(0, 1);
            mHeadIcons[0].SetMode(1);
            mHeadIcons[1].SetMode(1);
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 0) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x76);
            gSoundManager.StartSeq(0, 0x86);
            gSoundManager.LoadGroup(0x4D);
            PlayAnimation("happy00", TRUE);
            SetAnimSpeed(FX32_ONE);
            SetFace(2);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->Show(0x1B3, SpeechWindow::MODE_MANUAL, 120);
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (mFrame < 180) {
                return;
            }
            Speak(0, 0x150, 0);
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x7A:
        if (mEntered) {
            mEntered = FALSE;
            TurnHeadToCamera();
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 90) {
                return;
            }
            Speak(0, 0x152, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x7B:
        if (mEntered) {
            mEntered = FALSE;
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            if (!TurnToWalkTarget(0x280, 0)) {
                BlendToNamedAnimation("i_walk00");
                ResetSoundPhase(0);
                mStep = 0;
            } else {
                BlendToNamedAnimation("i_wait00");
                mStep = 2;
            }
            UpdateAnimSpeed();
            return;
        }
        PlaceHeldItem();
        switch (mStep) {
        case 0:
            PlaySoundOnce(1, 0, 26);
            if (TurnToWalkTarget(0x280, 0)) {
                if (mBlendFrames <= 0) {
                    BlendToNamedAnimation("i_wait00");
                    mStep = 2;
                } else {
                    mStep++;
                }
            }
            MoveAlongAngle();
            break;
        case 1:
            if (mBlendFrames > 0) {
                return;
            }
            BlendToNamedAnimation("i_wait00");
            mStep++;
            break;
        case 2:
            if (mBlendFrames > 0) {
                return;
            }
            Speak(0, 0x13C, 0);
            TurnHeadToCamera();
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            gSceneManager.RequestScene(0x54);
            gSceneMemory.mCharacters[0] = mIndex;
            break;
        }
        break;
    case 0x7C:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x13E, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x7D:
        if (mEntered) {
            mEntered = FALSE;
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            if (!TurnToWalkTarget(0x280, 0)) {
                BlendToNamedAnimation("i_walk00");
                ResetSoundPhase(0);
                mStep = 0;
            } else {
                BlendToNamedAnimation("i_wait00");
                mStep = 2;
            }
            UpdateAnimSpeed();
            return;
        }
        PlaceHeldItem();
        switch (mStep) {
        case 0:
            PlaySoundOnce(1, 0, 26);
            if (TurnToWalkTarget(0x280, 0)) {
                if (mBlendFrames <= 0) {
                    BlendToNamedAnimation("i_wait00");
                    mStep = 2;
                } else {
                    mStep++;
                }
            }
            MoveAlongAngle();
            break;
        case 1:
            if (mBlendFrames > 0) {
                return;
            }
            BlendToNamedAnimation("i_wait00");
            mStep++;
            break;
        case 2:
            if (mBlendFrames > 0) {
                return;
            }
            Speak(0, 0x144, 0);
            TurnHeadToCamera();
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            ClearMoodIfTroubleOfKind8();
            gSceneManager.RequestScene(0xA3);
            gSceneMemory.mCharacters[0] = mIndex;
            break;
        }
        break;
    case 0x7E:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x146, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x7F:
        if (mEntered) {
            mEntered = FALSE;
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            if (!TurnToWalkTarget(0x280, 0)) {
                BlendToNamedAnimation("i_walk00");
                ResetSoundPhase(0);
                mStep = 0;
            } else {
                BlendToNamedAnimation("i_wait00");
                mStep = 2;
            }
            UpdateAnimSpeed();
            return;
        }
        PlaceHeldItem();
        switch (mStep) {
        case 0:
            PlaySoundOnce(1, 0, 26);
            if (TurnToWalkTarget(0x280, 0)) {
                if (mBlendFrames <= 0) {
                    BlendToNamedAnimation("i_wait00");
                    mStep = 2;
                } else {
                    mStep++;
                }
            }
            MoveAlongAngle();
            break;
        case 1:
            if (mBlendFrames > 0) {
                return;
            }
            BlendToNamedAnimation("i_wait00");
            mStep++;
            break;
        case 2:
            if (mBlendFrames > 0) {
                return;
            }
            Speak(0, 0x148, 0);
            TurnHeadToCamera();
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            gSceneManager.RequestScene(0xAA);
            gSceneMemory.mCharacters[0] = mIndex;
            break;
        }
        break;
    case 0x80:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x14A, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x81:
        if (mEntered) {
            mEntered = FALSE;
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            if (!TurnToWalkTarget(0x280, 0)) {
                BlendToNamedAnimation("i_walk00");
                ResetSoundPhase(0);
                mStep = 0;
            } else {
                BlendToNamedAnimation("i_wait00");
                mStep = 2;
            }
            UpdateAnimSpeed();
            return;
        }
        PlaceHeldItem();
        switch (mStep) {
        case 0:
            PlaySoundOnce(1, 0, 26);
            if (TurnToWalkTarget(0x280, 0)) {
                if (mBlendFrames <= 0) {
                    BlendToNamedAnimation("i_wait00");
                    mStep = 2;
                } else {
                    mStep++;
                }
            }
            MoveAlongAngle();
            break;
        case 1:
            if (mBlendFrames > 0) {
                return;
            }
            BlendToNamedAnimation("i_wait00");
            mStep++;
            break;
        case 2:
            if (mBlendFrames > 0) {
                return;
            }
            Speak(0, 0x140, 0);
            TurnHeadToCamera();
            mStep++;
            mFrame = 0;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            mStep++;
            mFrame = 0;
            gSoundManager.MoveSeqVolume(0, 0, 40);
            break;
        case 4:
            brightness = mFrame * 2;
            if (brightness > 16) {
                brightness = 16;
            } else if (brightness < 0) {
                brightness = 0;
            }
            ScreenControl_SetBlendBrightness(&gScreenControl, 0x3F3F, brightness);
            if (mFrame < 8) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 5:
            if (mFrame < 32) {
                return;
            }
            for (u8 i = 0; i < 12; i++) {
                gSceneMemory.mCharacters[i] = 0xFF;
            }
            gSceneManager.RequestScene(0x22);
            gSceneMemory.mCharacters[0] = mIndex;
            break;
        }
        break;
    case 0x82:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x142, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x83:
        if (mEntered) {
            mEntered = FALSE;
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            if (!TurnToWalkTarget(0x280, 0)) {
                BlendToNamedAnimation("i_walk00");
                ResetSoundPhase(0);
                mStep = 0;
            } else {
                BlendToNamedAnimation("i_wait00");
                mStep = 2;
            }
            UpdateAnimSpeed();
            return;
        }
        PlaceHeldItem();
        switch (mStep) {
        case 0:
            PlaySoundOnce(1, 0, 26);
            if (TurnToWalkTarget(0x280, 0)) {
                if (mBlendFrames <= 0) {
                    BlendToNamedAnimation("i_wait00");
                    mStep = 2;
                } else {
                    mStep++;
                }
            }
            MoveAlongAngle();
            break;
        case 1:
            if (mBlendFrames > 0) {
                return;
            }
            BlendToNamedAnimation("i_wait00");
            mStep++;
            break;
        case 2:
            if (mBlendFrames > 0) {
                return;
            }
            Speak(0, 0x154, 0);
            TurnHeadToCamera();
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            Speak(0, 0x156, 0);
            mStep++;
            break;
        case 4: {
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            ClearMoodIfTroubleOfCategory4OrKind8();
            int partner = func_ov075_0217520c(this);
            if (partner != -1) {
                gSceneManager.RequestScene(0x79);
                gSceneMemory.mCharacters[0] = mIndex;
                gSceneMemory.mCharacters[1] = partner;
            } else {
                gSceneManager.RequestScene(0x78);
                gSceneMemory.mCharacters[0] = mIndex;
            }
            break;
        }
        }
        break;
    case 0x84:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            Speak(0, 0x158, 0);
            mStep++;
            break;
        case 1:
            if (mTtsPlayer->IsEngineIdle()) {
                Speak(0, 0x15A, 0);
                mStep++;
            }
            // fallthrough
        case 2:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x8B);
            break;
        }
        break;
    case 0x85:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            if (gSceneManager.mPreviousSceneId != 0x77) {
                Speak(0, 0x15C, 0);
            } else {
                Speak(0, 0x15E, 0);
            }
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    }
}

// The character's state when the player has touched it, and its talk about its troubles (the trouble messages
// it speaks): 0x86 it stands facing the camera (or walks to face it, 0x88) and, after a while, goes on with what
// it has to say; 0x89..0x8C it hands the player a present; 0x8D, 0x8E it levels up; 0x8F it walks back into place;
// 0x90..0x92 asking how to talk to someone it wants as a friend ("How should I talk?", scene 0xAF), 0x93, 0x94
// whether it became friends; 0x95..0x9C a confession ("How should I confess?", "Should I change first?" with the
// curtain, scene 0x5A), 0x9C "Why do you object?" and its answers 0x9D..0xA0; 0xA1..0xA4 a fight ("Can I have a
// moment?", scene 0xAB, "Then calm this anger", scene 0xB2), 0xA5..0xAB making up, separations and their outcomes
// with a sound and a line in the speech window; 0xAD..0xB0 the word the player teaches it (0xAF repeats it, 0xB0
// stores it as its motto); 0xB1 recovered from heartbreak.
void Scene40Character::UpdateStates86ToB2() {
    Camera *camera = &gCamera;
    FloorPos target;
    int brightness;

    switch (mState) {
    case 0x86:
        if (mEntered) {
            mEntered = FALSE;
            UpdateCameraAngle();
            if (IsFacingCamera()) {
                BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
                UpdateAnimSpeed();
                TurnHeadToCamera();
            } else {
                SetState(0x88);
            }
            if (mUnknown2ED4) {
                gSceneManager.SetSoundGroup(0x37);
                gSoundManager.StartSeqIfNotPlaying(0, 0x62);
            }
        } else if (mFrame == 30) {
            SpeakAboutTrouble();
            StartTroubleProgress();
            if (mUnknown2ED4) {
                SetState(0x87);
            }
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        if (mFrame < 60) {
            return;
        }
        if (mUnknown2ED0) {
            mUnknown2ED0 = FALSE;
            SetState(0);
        } else if (IsInTrouble() && data_02114d78[mTroubleCategory][mTroubleId].kind == 18) {
            SetState(0xAC);
        }
        if (mUnknown2ED8) {
            GoToTogetherScene();
            return;
        }
        if (mUnknown2EDC) {
            GoToScene3C();
            return;
        }
        if (!IsInTrouble()) {
            return;
        }
        if (data_02114d78[mTroubleCategory][mTroubleId].kind != 6) {
            return;
        }
        GoToWedding();
        break;
    case 0x87:
        if (mEntered) {
            mEntered = FALSE;
            return;
        }
        if (mStep != 0) {
            return;
        }
        if (gSoundManager.GetSeqTick(0) < 548) {
            return;
        }
        GoToScene51();
        break;
    case 0x88:
        if (mEntered) {
            mEntered = FALSE;
            ResetHeadTurn();
            BlendToNamedAnimation(sWalkAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            ResetSoundPhase(0);
            UpdateSpeedScales();
            target.x = camera->mEye.x;
            target.z = camera->mEye.z;
            SetTargetFloorPos(target);
            return;
        }
        if (TurnToWalkTarget(0x280, 0)) {
            SetState(0x86);
        }
        MoveAlongAngle();
        PlaySoundOnce(1, 0, 26);
        break;
    case 0x89:
        if (mEntered) {
            mEntered = FALSE;
            mMotion.mHeld = 0;
            SetFace(8);
            PlayAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood], FALSE);
            UpdateAnimSpeed();
            mShadowVisible = TRUE;
            TurnHeadToCamera();
            Speak(0, 0x1C9, 0);
            return;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        SetState(0x8B);
        break;
    case 0x8A:
        if (mEntered) {
            mEntered = FALSE;
            mMotion.mHeld = 0;
            SetFace(8);
            PlayAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood], FALSE);
            UpdateAnimSpeed();
            mShadowVisible = TRUE;
            TurnHeadToCamera();
            Speak(0, 0x1CB, 0);
            return;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        SetState(0x8B);
        break;
    case 0x8B:
        if (mEntered) {
            mEntered = FALSE;
            TurnHeadToCamera();
            UpdateAnimSpeed();
            PlayAnimation("i_give01", TRUE);
            mContext->mPhysics->TakeFreeItem(&mHeldItem);
            mHeldItem->ChangeItem(mGift.mMessage);
            mHeldItem->mVisible = FALSE;
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            if (!GetAnimation()->mFinished) {
                ModelAnimation *animation = GetAnimation();
                int frames = (((NNSG3dResJntAnm *)animation->mAnmObj->resAnm)->numFrame << FX32_SHIFT) / (fx32)animation->mUnknown10 + 1;
                int quarter = frames / 4;
                int half = frames / 2;
                if (mFrame == quarter) {
                    gSoundManager.StartSeqArc(1, 26, 8);
                } else if (mFrame == half) {
                    fx32 scale = ((mFrame - half + 1) << FX32_SHIFT) / (frames - half);
                    mHeldItem->SetScale(scale);
                    mHeldItem->mVisible = TRUE;
                    PlaceHeldItem(scale);
                } else if (mFrame > half) {
                    fx32 scale = ((mFrame - half + 1) << FX32_SHIFT) / (frames - half);
                    mHeldItem->SetScale(scale);
                    PlaceHeldItem(scale);
                }
            } else {
                PlayAnimation("i_give00", TRUE);
                SetAnimSpeed(0xA66);
                mHeldItem->SetScale(FX32_ONE);
                PlaceHeldItem();
                mFrame = 0;
                mStep++;
            }
            break;
        case 1:
            PlaceHeldItem();
            if (!GetAnimation()->mFinished) {
                return;
            }
            SetState(0x8C);
            break;
        }
        break;
    case 0x8C:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x8D:
        if (mEntered) {
            mEntered = FALSE;
            PlayAnimation("levelup00", TRUE);
            SetAnimSpeed(FX32_ONE);
            SetFace(2);
            ResetHeadTurn();
            mHeadIcons[0].Show();
            mHeadIcons[0].mIconId = 6;
            mHeadIcons[0].Reload();
            mHeadIcons[0].SetMode(3);
            mHeadIconPlaces[0] = ICON_AT_HEAD;
            mHeadIcons[1].Show();
            mHeadIcons[1].mIconId = 7;
            mHeadIcons[1].Reload();
            mHeadIcons[1].SetMode(3);
            mHeadIconPlaces[1] = ICON_AT_BODY;
            return;
        }
        if (mFrame == 1) {
            gSoundManager.StartSeqArc(1, 27, 9);
            gSoundManager.StopSeq(0, 1);
        } else if (GetAnimation()->mFinished) {
            SetFace(9);
            SetState(0x8E);
        }
        break;
    case 0x8E:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0x8F:
        if (mEntered) {
            mEntered = FALSE;
            mMotion.mHeld = 0;
            SetFace(8);
            mShadowVisible = TRUE;
            UpdateCameraAngle();
            if (IsFacingCameraClosely()) {
                PlayAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood], FALSE);
                UpdateAnimSpeed();
                mStep = 1;
                return;
            } else {
                PlayAnimation(sWalkAnimations[gPersonalityKinds[mPersonality]][mMood], FALSE);
                UpdateAnimSpeed();
                ResetSoundPhase(0);
                mUnknown2EA8 = mContext->mCamera->GetEyeFloorPos();
                SetTargetFloorPos(mUnknown2EA8);
                mStep = 0;
            }
            return;
        }
        switch (mStep) {
        case 0:
            if (TurnToWalkTarget(0x280, 0)) {
                BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
                mStep++;
            }
            MoveAlongAngle();
            PlaySoundOnce(1, 0, 26);
            break;
        case 1:
            if (mBlendFrames > 0) {
                return;
            }
            TurnHeadToCamera();
            mFrame = 0;
            mStep++;
            break;
        case 2:
            if (mFrame >= 35) {
                mStep++;
            }
            break;
        case 3:
            Speak(0, 0x1CD, 0);
            mStep++;
            break;
        case 4:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x8B);
            break;
        }
        break;
    case 0x90:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x160, 2);
        }
        break;
    case 0x91:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x162, 2);
        }
        break;
    case 0x92:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x164, 2);
            return;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        gSceneManager.RequestScene(0xAF);
        gSceneMemory.mCharacters[0] = mTroubleTarget;
        gSceneMemory.mCharacters[1] = mIndex;
        break;
    case 0x93:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            gSoundManager.MoveSeqVolume(0, 0, 10);
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 30) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x6C);
            gSoundManager.StartSeq(0, 0x7C);
            gSoundManager.LoadGroup(0x4D);
            PlayAnimation("happy00", TRUE);
            SetAnimSpeed(FX32_ONE);
            SetFace(2);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->Show(0x1A7, SpeechWindow::MODE_FRAMES, 120);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (mFrame < 120) {
                return;
            }
            Speak(0, 0x168, 0);
            mStep++;
            mFrame = 0;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x94:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 40) {
                return;
            }
            Speak(0, 0x16A, 0);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0x95:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x16C, 2);
        }
        break;
    case 0x96:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x16E, 2);
        }
        break;
    case 0x97:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x170, 2);
        }
        break;
    case 0x98:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x172, 0);
            mStep = 0;
            mFrame = 0;
            return;
        }
        switch (mStep) {
        case 0:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            mStep++;
            mFrame = 0;
            gSoundManager.MoveSeqVolume(0, 0, 60);
            break;
        case 1:
            brightness = mFrame * 2;
            if (brightness > 16) {
                brightness = 16;
            } else if (brightness < 0) {
                brightness = 0;
            }
            ScreenControl_SetBlendBrightness(&gScreenControl, 0x3F3F, brightness);
            if (mFrame < 8) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (mFrame < 52) {
                return;
            }
            gSceneManager.RequestScene(0x5A);
            gSceneMemory.mCharacters[0] = mIndex;
            gSceneMemory.mCharacters[1] = mTroubleTarget;
            break;
        }
        break;
    case 0x99:
        if (mEntered) {
            mEntered = FALSE;
            mStep = 0;
            mFrame = 0;
            TurnHeadToCamera();
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 45) {
                return;
            }
            Speak(0, 0x174, 0);
            mStep++;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            mStep++;
            mFrame = 0;
            gSoundManager.MoveSeqVolume(0, 0, 60);
            break;
        case 2:
            brightness = mFrame * 2;
            if (brightness > 16) {
                brightness = 16;
            } else if (brightness < 0) {
                brightness = 0;
            }
            ScreenControl_SetBlendBrightness(&gScreenControl, 0x3F3F, brightness);
            if (mFrame < 8) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 3:
            if (mFrame < 52) {
                return;
            }
            gSceneManager.RequestScene(0x5A);
            gSceneMemory.mCharacters[0] = mIndex;
            gSceneMemory.mCharacters[1] = mTroubleTarget;
            break;
        }
        break;
    case 0x9A:
        if (mEntered) {
            mEntered = FALSE;
            ShowBalloon(3);
            ResetHeadTurn();
            mUnknown2EA0.x = 0;
            mUnknown2EA0.z = 0;
            SetTargetFloorPos(mUnknown2EA0);
            if (!WalkToTarget(ScaleBySpeed(0x7A), 0x22, 4)) {
                ResetSoundPhase(0);
                BlendToNamedAnimation(sWalkAnimations[gPersonalityKinds[mPersonality]][mMood]);
                mStep = 0;
                MoveAlongAngle();
            } else {
                target.x = camera->mEye.x;
                target.z = camera->mEye.z;
                SetTargetFloorPos(target);
                MoveAlongAngle();
                if (!TurnToWalkTarget(0x280, 0)) {
                    ResetSoundPhase(0);
                    BlendToNamedAnimation(sWalkAnimations[gPersonalityKinds[mPersonality]][mMood]);
                    mStep = 1;
                } else {
                    BlendToNamedAnimation("idle00");
                    mStep = 3;
                }
                MoveAlongAngle();
            }
            return;
        }
        switch (mStep) {
        case 0:
            PlaySoundOnce(1, 0, 26);
            if (WalkToTarget(ScaleBySpeed(0x7A), 0x22, 4)) {
                target.x = camera->mEye.x;
                target.z = camera->mEye.z;
                SetTargetFloorPos(target);
                mStep++;
            }
            MoveAlongAngle();
            break;
        case 1:
            PlaySoundOnce(1, 0, 26);
            if (TurnToWalkTarget(0x280, 0)) {
                mStep++;
            }
            MoveAlongAngle();
            break;
        case 2:
            if (mBlendFrames > 0) {
                return;
            }
            BlendToNamedAnimation("idle00");
            mStep++;
            break;
        case 3:
            if (mBlendFrames > 0) {
                return;
            }
            SetState(0x9B);
            break;
        }
        break;
    case 0x9B:
        if (mEntered) {
            mEntered = FALSE;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 20;
            if (mFrame < mWait) {
                return;
            }
            mCover.SetKind(Scene40Cover::KIND_CURTAIN);
            mCover.SetPosition(mPos, mAnim.mRotY);
            mCover.Appear();
            gSoundManager.StartSeqArc(1, 10, 1);
            mStep++;
            break;
        case 1:
            if (!mCover.HasAppeared()) {
                return;
            }
            mCover.PlayWait();
            mFrame = 0;
            mStep++;
            break;
        case 2:
            mWait = 52;
            if (mFrame == 10 || mFrame == 36) {
                gSoundManager.StartSeqArc(1, 10, 2);
            }
            if (mFrame < mWait) {
                return;
            }
            ModelResources_ReleaseModel(gModelResources, 0);
            ArchiveManager_Unmount(&gArchiveManager, mArchiveName);
            SetArchive(mArchiveName);
            ArchiveManager_MountCompressed(&gArchiveManager, mArchiveName, mArchivePath, 1);
            ModelResources_SetModel(gModelResources, 0, mModelPath);
            MakeModel(0, 0);
            UpdateBodyColors();
            mFrame = 0;
            mStep++;
            break;
        case 3:
            mWait = 52;
            if (mFrame == 10 || mFrame == 36) {
                gSoundManager.StartSeqArc(1, 10, 2);
            }
            if (mFrame < mWait) {
                return;
            }
            mCover.Disappear();
            gSoundManager.StartSeqArc(1, 10, 0);
            mFrame = 0;
            mStep++;
            break;
        case 4:
            if (!mCover.HasDisappeared()) {
                return;
            }
            mCover.mModel.mVisible = FALSE;
            SetState(0x99);
            break;
        }
        break;
    case 0x9C:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(13);
            BlendToNamedAnimation("idle00");
            mTtsPlayer->StopEngine();
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 45) {
                return;
            }
            SetFace(8);
            Speak(0, 0x176, 2);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            mStep++;
            break;
        case 1:
            break;
        }
        break;
    case 0x9D:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x178, 0);
            return;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        SetState(0x33);
        break;
    case 0x9E:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x17A, 0);
            return;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        SetState(0x33);
        break;
    case 0x9F:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x17C, 0);
            return;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        SetState(0x33);
        break;
    case 0xA0:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x17E, 0);
            return;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        SetState(0x33);
        break;
    case 0xA1:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(10);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][1]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            mStep++;
            Speak(0, 0x182, 0);
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            gSceneManager.RequestScene(0xAB);
            gSceneMemory.mCharacters[0] = mIndex;
            gSceneMemory.mCharacters[1] = mTroubleTarget;
            break;
        }
        break;
    case 0xA2:
        if (mEntered) {
            mEntered = FALSE;
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 10;
            if (mFrame < mWait) {
                return;
            }
            mStep++;
            Speak(0, 0x184, 0);
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0);
            break;
        }
        break;
    case 0xA3:
        if (mEntered) {
            mEntered = FALSE;
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][0]);
            TurnHeadToCamera();
            mHeadIcons[0].SetMode(1);
            mHeadIcons[1].SetMode(1);
            mFrame = 0;
            mStep = 0;
            return;
        }
        if (mFrame < 72) {
            return;
        }
        SetState(0xA4);
        break;
    case 0xA4:
        if (mEntered) {
            mEntered = FALSE;
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][0]);
            TurnHeadToCamera();
            mHeadIcons[0].SetMode(1);
            mHeadIcons[1].SetMode(1);
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 40;
            if (mFrame < mWait) {
                return;
            }
            mStep++;
            Speak(0, 0x186, 0);
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            gSceneManager.RequestScene(0xB2);
            gSceneMemory.mCharacters[0] = mTroubleTarget;
            gSceneMemory.mCharacters[1] = mIndex;
            break;
        }
        break;
    case 0xA5:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 30) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x6F);
            gSoundManager.StartSeq(0, 0x7F);
            gSoundManager.LoadGroup(0x4D);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->Show(0x1A9, SpeechWindow::MODE_MANUAL, 120);
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (gSoundManager.CountPlayingSeq(0x7F) != 0) {
                return;
            }
            mContext->mSpeechWindow->Hide();
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            mStep++;
            break;
        case 3:
            if (mBlendFrames > 0) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 4:
            if (mFrame < 20) {
                return;
            }
            Speak(0, 0x188, 0);
            mStep++;
            break;
        case 5:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0xA6:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 30) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x70);
            gSoundManager.StartSeq(0, 0x80);
            gSoundManager.LoadGroup(0x4D);
            PlayAnimation("sign00", TRUE);
            SetAnimSpeed(FX32_ONE);
            SetFace(4);
            mStep++;
            mFrame = 0;
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->Show(0x1AA, SpeechWindow::MODE_MANUAL, 120);
            break;
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (gSoundManager.CountPlayingSeq(0x80) != 0) {
                return;
            }
            mContext->mSpeechWindow->Hide();
            mStep++;
            mFrame = 0;
            break;
        case 3:
            if (mBlendFrames > 0) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 4:
            if (mFrame < 20) {
                return;
            }
            Speak(0, 0x18A, 0);
            mStep++;
            break;
        case 5:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0xA7:
        if (mEntered) {
            mEntered = FALSE;
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 10;
            if (mFrame < mWait) {
                return;
            }
            mStep++;
            Speak(0, 0x18C, 1, mTroubleTarget, 0);
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            mStep++;
            mFrame = 0;
            gSoundManager.MoveSeqVolume(0, 0, 120);
            break;
        case 2:
            brightness = mFrame * 2;
            if (brightness > 16) {
                brightness = 16;
            } else if (brightness < 0) {
                brightness = 0;
            }
            ScreenControl_SetBlendBrightness(&gScreenControl, 0x3F3F, brightness);
            if (mFrame < 8) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 3:
            if (mFrame < 112) {
                return;
            }
            gSceneManager.RequestScene(0xB3);
            gSceneMemory.mCharacters[0] = mIndex;
            gSceneMemory.mCharacters[1] = mTroubleTarget;
            break;
        }
        break;
    case 0xA8:
        if (mEntered) {
            mEntered = FALSE;
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 45;
            if (mFrame < mWait) {
                return;
            }
            mStep++;
            Speak(0, 0x18E, 0);
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            gSceneManager.RequestScene(0xB5);
            gSceneMemory.mCharacters[0] = mIndex;
            gSceneMemory.mCharacters[1] = mTroubleTarget;
            break;
        }
        break;
    case 0xA9:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0: {
            if (mFrame < 30) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x71);
            gSoundManager.StartSeq(0, 0x81);
            gSoundManager.LoadGroup(0x4D);
            PlayAnimation("sign00", TRUE);
            SetAnimSpeed(FX32_ONE);
            SetFace(4);
            mStep++;
            mFrame = 0;
            SavedCharacter other;
            wchar_t otherName[11];
            wchar_t name[11];
            GameData_ReadCharacter(&gGameData, mTroubleTarget, &other);
            MiiData_GetName(this, name);
            MiiData_GetName(&other, otherName);
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->SetStringArg(name, 0);
            mContext->mSpeechWindow->SetStringArg(otherName, 1);
            mContext->mSpeechWindow->Show(0x1AD, SpeechWindow::MODE_MANUAL, 120);
            break;
        }
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (gSoundManager.CountPlayingSeq(0x81) != 0) {
                return;
            }
            mContext->mSpeechWindow->Hide();
            mStep++;
            mFrame = 0;
            break;
        case 3:
            if (mBlendFrames > 0) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 4:
            if (mFrame < 20) {
                return;
            }
            Speak(0, 0x190, 0);
            mStep++;
            break;
        case 5:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0xAA:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 30) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x72);
            gSoundManager.StartSeq(0, 0x82);
            gSoundManager.LoadGroup(0x4D);
            SetFace(8);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->Show(0x1AB, SpeechWindow::MODE_MANUAL, 120);
            SetFace(8);
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (gSoundManager.CountPlayingSeq(0x82) != 0) {
                return;
            }
            mContext->mSpeechWindow->Hide();
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            mStep++;
            break;
        case 3:
            if (mBlendFrames > 0) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 4:
            if (mFrame < 20) {
                return;
            }
            Speak(0, 0x192, 0);
            mStep++;
            break;
        case 5:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0xAB:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 30) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x73);
            gSoundManager.StartSeq(0, 0x83);
            gSoundManager.LoadGroup(0x4D);
            PlayAnimation("sign00", TRUE);
            SetAnimSpeed(FX32_ONE);
            SetFace(4);
            mStep++;
            mFrame = 0;
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->Show(0x1AA, SpeechWindow::MODE_MANUAL, 120);
            break;
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (gSoundManager.CountPlayingSeq(0x83) != 0) {
                return;
            }
            mContext->mSpeechWindow->Hide();
            mStep++;
            mFrame = 0;
            break;
        case 3:
            if (mBlendFrames > 0) {
                return;
            }
            mStep++;
            mFrame = 0;
            break;
        case 4:
            if (mFrame < 20) {
                return;
            }
            Speak(0, 0x194, 0);
            mStep++;
            break;
        case 5:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0xAC:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    case 0xAD:
        if (mEntered) {
            mEntered = FALSE;
            SpeakAboutTarget2();
        }
        break;
    case 0xAE:
        if (mEntered) {
            mEntered = FALSE;
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 30) {
                return;
            }
            SpeakAboutTarget3();
            mStep++;
            mFrame = 0;
            break;
        case 1:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0xAF:
        if (mEntered) {
            mEntered = FALSE;
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            mWait = 23;
            if (mFrame < mWait) {
                return;
            }
            if (wcslen(mTaughtWord) <= 13) {
                Speak(0, 0x118, 6, 0, 2);
            } else {
                Speak(0, 0x11A, 6, 0, 2);
            }
            mStep++;
            break;
        case 1:
            break;
        }
        break;
    case 0xB0:
        if (mEntered) {
            mEntered = FALSE;
            Speak(0, 0x11C, 0);
            switch (mTroubleId) {
            case 0:
                wcscpy(mMotto, mTaughtWord);
                break;
            case 1:
                wcscpy(mMottos[0], mTaughtWord);
                break;
            case 2:
                wcscpy(mMottos[1], mTaughtWord);
                break;
            case 3:
                wcscpy(mMottos[2], mTaughtWord);
                break;
            case 4:
                wcscpy(mMottos[3], mTaughtWord);
                break;
            }
            return;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            return;
        }
        SetState(0x33);
        break;
    case 0xB1:
        if (mEntered) {
            mEntered = FALSE;
            SetFace(8);
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            UpdateAnimSpeed();
            GetAnimation()->mPlayOnce = FALSE;
            TurnHeadToCamera();
            mFrame = 0;
            mStep = 0;
            return;
        }
        switch (mStep) {
        case 0:
            if (mFrame < 40) {
                return;
            }
            gSoundManager.SetSeqTrackMuteOnly(0, 0xFFFF, FALSE);
            gSceneManager.SetSoundGroup(0x74);
            gSoundManager.StartSeq(0, 0x84);
            gSoundManager.LoadGroup(0x4D);
            PlayAnimation("levelup00", TRUE);
            SetAnimSpeed(FX32_ONE);
            SetFace(1);
            mStep++;
            mFrame = 0;
            break;
        case 1:
            if (!GetAnimation()->mFinished) {
                return;
            }
            mContext->mSpeechWindow->mMessageFile = mContext->mTroubleMessages;
            mContext->mSpeechWindow->mText[0].mHAlign = 1;
            mContext->mSpeechWindow->Show(0x1B4, SpeechWindow::MODE_FRAMES, 120);
            SetFace(8);
            UpdateAnimSpeed();
            mStep++;
            mFrame = 0;
            break;
        case 2:
            if (mFrame < 120) {
                return;
            }
            BlendToNamedAnimation(sWaitAnimations[gPersonalityKinds[mPersonality]][mMood]);
            Speak(0, 0x180, 0);
            mStep++;
            break;
        case 3:
            if (!mTtsPlayer->IsEngineIdle()) {
                return;
            }
            SetState(0x33);
            break;
        }
        break;
    case 0xB2:
        if (mEntered) {
            mEntered = FALSE;
        }
        break;
    }
}

void Scene40Balloon::Draw() {
    mModel.Draw();
}

void Scene40Cover::Draw() {
    mModel.Draw();
}

extern "C" {
void ParticleSystem_Draw(ParticleSystem *particles, int unknown); // Draws the particles
void GameData_WriteCharacter(GameData *game, int index, SavedCharacter *character); // Writes a saved character
void DestroyMainFrameHeap(void);
BOOL ArchiveManager_IsIdle(ArchiveLoader *loader);
extern int gGameClock[];
s64 Clock_GetDateTime(int offset, RTCDate *date, RTCTime *time); // The date and time now, and the same in seconds

// Not decompiled yet
}

// --- ov075-e: the character's helpers, its balloon and cover, the scene's clock, input, camera, lights, effects ---

extern "C" {
// The id of an animation of a resource set by its name
int ResourceSet_FindIndexByName(void *resourceSet, u32 resType, const char *name);
void AnimModel_SetSpeed(AnimModel *model, fx32 speed);
void func_0202a5a4(AnimModel *model, fx32 frame);
BOOL LineMeetsSphere(const VecFx32 *origin, const VecFx32 *dir, fx32 radius);
extern VecFx32 gTouchLineDirection; // The direction of the line through the touched point
void GameData_ReadCharacter(GameData *game, int index, SavedCharacter *character);
void Camera_Reset(Camera *camera);
void Camera_UpdateEye(Camera *camera);
void Light_Reset(Light *light);
void Light_SetDirection(Light *light, int unk, u16 angle, int unk2, VecFx32 direction);
void ScaleVec(fx32 scale, const VecFx32 *in, VecFx32 *out);
void Model_SetAlpha(Model *model, int alpha);
}

#define RESOURCE_BTA0 0x30415442 // Texture SRT animation

// The character's state machine: a new state takes effect at the next update, which sees mEntered set
void Scene40Character::SetState(int state) {
    mPrevState = mState;
    mNextState = state;
    mStep = 0;
    mEntered = TRUE;
    mFrame = 0;
}

void Scene40Character::ForceState(int state) {
    mNextState = state;
    mState = state;
    mStep = 0;
    mEntered = TRUE;
    mFrame = 0;
}

void Scene40Character::ShowBalloon(int state) {
    mBalloon.ChooseContent();
    mBalloon.SetState(state);
}

BOOL Scene40Character::IsBalloonTouched() {
    return mBalloon.IsTouched();
}

void Scene40Character::UpdateCameraAngle() {
    u16 angle = mAngle;
    mCameraAngle = angle - GetAngleTo(&gCamera.mEye);
}

BOOL Scene40Character::IsFacingCameraLoosely() {
    BOOL facing = TRUE;
    if (mCameraAngle >= 0x4000 && mCameraAngle <= 0xC000) {
        facing = FALSE;
    }
    return facing;
}

BOOL Scene40Character::IsFacingCamera() {
    BOOL facing = TRUE;
    if (mCameraAngle >= 0x2800 && mCameraAngle <= 0xD800) {
        facing = FALSE;
    }
    return facing;
}

BOOL Scene40Character::IsFacingCameraClosely() {
    BOOL facing = TRUE;
    if (mCameraAngle >= 0x1000 && mCameraAngle <= 0xF000) {
        facing = FALSE;
    }
    return facing;
}

// Where a thing thrown from pos with a velocity lands on the floor
FloorPos GetLandingPoint(VecFx32 pos, VecFx32 velocity) {
    int frames = Scene40Physics::GetFallFrames(pos.y, velocity.y);
    FloorPos landing;
    landing.x = pos.x + velocity.x * frames;
    landing.z = pos.z + velocity.z * frames;
    return landing;
}

void Scene40Character::SetGift(Scene40Gift *gift) {
    mGift.Set(gift->mKind, gift->mItem.InventoryItem::mId);
}

// What the record's liking of an item (SavedCharacter::GetLiking and the like, 0 to 17) gives: a reaction, 0 (the
// least) to 4, and how much the gift satisfies, by the main program's tables. 18 and 19 are the goods it likes
// most (mFavoriteItem) and least (mDislikedItem).
static const int sGoodsReactions[18] = {19, 4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 0, 0, 0, 18};

static const int sClothesReactions[16] = {9, 9, 9, 8, 8, 8, 7, 7, 7, 7, 6, 6, 6, 5, 5, 5};

static const int sInteriorReactions[16] = {4, 4, 4, 3, 3, 3, 2, 2, 2, 2, 1, 1, 1, 0, 0, 0};

extern "C" {
extern const s8 gGoodsSatisfactions[];    // By liking (0x020F8E78)
extern const s8 gClothesSatisfactions[];  // By liking (0x020F8E58)
extern const s8 gInteriorSatisfactions[]; // By liking (0x020F8E68)
}

// How much a gift of goods satisfies: the main program's value by liking, by the size of the food (half, 80% or
// all of it), and a hundredth of the price for goods it likes; at least 40 for its favourite, at most 70
int Scene40Character::GetGoodsSatisfaction(int item) {
    int liking = GetLiking(item);
    const ItemInfo *info = &gGoodsTable[(u16)item];
    int percent;
    int value = gGoodsSatisfactions[liking];
    switch (info->mFoodSize) {
    case 0:
        percent = 50;
        break;
    case 1:
        percent = 80;
        break;
    case 2:
        percent = 100;
        break;
    }
    int bonus = (int)info->mPrice / 100;
    if (liking >= 17) {
        value = bonus + value * percent / 100;
        if (value < 40) {
            value = 40;
        }
    } else if (liking > 6) {
        value = bonus + value * percent / 100;
    } else if (liking > 0) {
        value = value * percent / 100;
    }
    if (value > 70) {
        value = 70;
    } else if (value < -100) {
        value = -100;
    }
    return value;
}

int Scene40Character::GetGoodsReaction(int item) {
    return sGoodsReactions[GetLiking(item)];
}

int Scene40Character::GetClothesSatisfaction(int clothes) {
    return gClothesSatisfactions[GetClothesLiking(clothes)];
}

int Scene40Character::GetClothesReaction(int clothes) {
    return sClothesReactions[GetClothesLiking(clothes)];
}

int Scene40Character::GetInteriorReaction(int interior) {
    return sInteriorReactions[GetInteriorLiking(interior)];
}

int Scene40Character::GetInteriorSatisfaction(int interior) {
    return gInteriorSatisfactions[GetInteriorLiking(interior)];
}

// The record keeps the three goods it liked best of those given (mUnknown292, 0xFF for none), best first. An item
// it likes at least 11 (and that is not its favourite) takes its place among them; TRUE if it did.
BOOL Scene40Character::RankLikedItem(int item) {
    int liking = GetLiking(item);
    if (item == mFavoriteItem) {
        liking = 0;
    }
    BOOL ranked = FALSE;
    int slot = 0;
    if (liking >= 11) {
        for (int i = 0; i < 3; i++) {
            if (item == GetUnknown292(i)) {
                ranked = FALSE;
                break;
            }
            if (GetUnknown292(i) == 0xFF) {
                slot = i;
                ranked = TRUE;
                break;
            }
            if (GetLiking(GetUnknown292(i)) < liking) {
                slot = i;
                ranked = TRUE;
                break;
            }
        }
    }
    if (ranked) {
        for (int i = 2; i > slot; i--) {
            SetUnknown292(i, GetUnknown292(i - 1));
        }
        SetUnknown292(slot, item);
    }
    return ranked;
}

// The main program's table of troubles (by category, then id), as far as this file reads it


// 0xC

extern "C" {

}

// In a trouble of kind 1 while still full, mUnknown23C is put back by an hour for every tenth of fullness, and one
void Scene40Character::SetUnknown23CFromFullness(s64 now) {
    if (!IsInTrouble()) {
        return;
    }
    if (data_02114d78[mTroubleCategory][mTroubleId].kind != 1) {
        return;
    }
    if (GetFullness(now) <= 0) {
        return;
    }
    mUnknown23C = now - (mEatFullness / 10 + 1) * 3600;
}

// Three things of the visit add to the record's mUnknown1C4 only once each
void Scene40Character::AddUnknown1C4Once0() {
    if (mUnknown1C4Added[0]) {
        return;
    }
    AddUnknown1C4(2);
    mUnknown1C4Added[0] = TRUE;
}

void Scene40Character::AddUnknown1C4Once1() {
    if (mUnknown1C4Added[1]) {
        return;
    }
    AddUnknown1C4(5);
    mUnknown1C4Added[1] = TRUE;
}

void Scene40Character::AddUnknown1C4Once2() {
    if (mUnknown1C4Added[2]) {
        return;
    }
    AddUnknown1C4(2);
    mUnknown1C4Added[2] = TRUE;
}

void Scene40Character::AddUnknown1C4ByTrouble() {
    if (IsInTrouble()) {
        AddUnknown1C4(1);
    } else {
        AddUnknown1C4(2);
    }
}

void Scene40Character::ClearMoodIfTroubleOfCategory4OrKind8() {
    if (mMood == 0) {
        return;
    }
    if ((IsInTrouble() && mTroubleCategory == 4) ||
        (IsInTrouble() && data_02114d78[mTroubleCategory][mTroubleId].kind == 8)) {
        mMood = 0;
    }
}

void Scene40Character::ClearMoodIfTroubleOfKind8() {
    if (mMood == 0) {
        return;
    }
    if (!IsInTrouble()) {
        return;
    }
    if (data_02114d78[mTroubleCategory][mTroubleId].kind == 8) {
        mMood = 0;
    }
}

// --- The character's trouble ---------------------------------------------------------------------------------

// The trouble talks' messages: by trouble id (for the progressing trouble), by relation and meter, ...
static const int sTroubleProgressMessages[7] = {242, 244, 246, 248, 250, 252, 254};

// The message about the trouble: by category, from the category's first message, two for each trouble
int Scene40Character::GetTroubleMessage() {
    int id = mTroubleId;
    switch (mTroubleCategory) {
    case 0:
        return id * 2;
    case 1:
        return id * 2 + 0xD0;
    case 2:
        return id * 2 + 0x78;
    case 3:
        return id * 2 + 0xB8;
    case 4:
        return id * 2 + 0xE0;
    case 5:
        return id * 2 + 0xC6;
    case 6:
        return id * 2 + 0xD8;
    case 8:
        return id * 2 + 0xB6;
    case 9:
        return id * 2 + 0xEE;
    }
}

void Scene40Character::SpeakTroubleProgress() {
    Speak(0, sTroubleProgressMessages[mTroubleId], 0);
}

// --- The character's talk ---------------------------------------------------------------------------------------

extern "C" {
// The personalities' kinds (0 to 3), by personality
}

// The tools an entry can require to have been had (Scene40TalkEntry::mItem: goods rows, by the record's bits at
// 0x1C8)
static const int sTalkItems[9] = {0, 236, 241, 243, 242, 239, 240, 237, 238};

static const int sPersonalityBits[16] = {1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768};

static const int sMonthBits[13] = {156, 1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048};

static const int sHourBits[24] = {8, 8, 8, 8, 8, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 4, 4, 4, 4, 4, 4};

static const int sWeekdayBits[7] = {1, 2, 4, 8, 16, 32, 64};

// The days since the last meeting at which each step of Scene40TalkConditions::mElapsed begins
static const u16 sElapsedDays[9] = {0, 0, 7, 14, 21, 28, 51, 81, 111};

// Whether an entry's conditions all hold
BOOL Scene40Character::MatchesTalk(const Scene40TalkEntry *entry, RTCDate *date, Scene40TalkConditions *conditions) {
    BOOL matches = TRUE;
    if (!(entry->mSexes & conditions->mSex)) {
        matches = FALSE;
    }
    if (!(entry->mPersonalities & conditions->mPersonality)) {
        matches = FALSE;
    }
    if (!(entry->mMoods & conditions->mMood)) {
        matches = FALSE;
    }
    if (!(entry->mMonths & conditions->mMonth)) {
        matches = FALSE;
    }
    if (!(entry->mHours & conditions->mHour)) {
        matches = FALSE;
    }
    if (!(entry->mWeekdays & conditions->mWeekday)) {
        matches = FALSE;
    }
    if (entry->mDay != 0 && entry->mDay != date->day) {
        matches = FALSE;
    }
    if (entry->mNeedsCharacters == 1 && conditions->mHasCharacters == FALSE) {
        matches = FALSE;
    }
    if (entry->mNeedsFriends == 1 && conditions->mHasFriends == FALSE) {
        matches = FALSE;
    }
    if (entry->mNeedsLover == 1 && conditions->mHasLover == FALSE) {
        matches = FALSE;
    }
    if (entry->mNeedsSpouse == 1 && conditions->mHasSpouse == FALSE) {
        matches = FALSE;
    }
    if (entry->mItem != 0) {
        int item = sTalkItems[entry->mItem];
        if (!((mUnknown1C8.m[item >> 5] >> (item & 31)) & 1)) {
            matches = FALSE;
        }
    }
    if (entry->mElapsed != 0 && entry->mElapsed != conditions->mElapsed) {
        matches = FALSE;
    }
    return matches;
}

static inline RTCTime Midnight() {
    RTCTime time = {0, 0, 0};
    return time;
}

// What the entries are tested against: the character's sex, mood and personality, the month, hour and day of the
// week, how long since the last meeting, and its relations
void Scene40Character::GetTalkConditions(RTCDate *date, RTCTime *time, Scene40TalkConditions *conditions) {
    mPersonality = GetPersonality();
    if (MiiData_GetSex(this) == 0) {
        conditions->mSex = 1;
    }
    if (MiiData_GetSex(this) == 1) {
        conditions->mSex = 2;
    }
    conditions->mPersonality = sPersonalityBits[mPersonality];
    switch (mMood) {
    case 0:
        conditions->mMood = 1;
        break;
    case 1:
        conditions->mMood = 2;
        break;
    case 2:
        conditions->mMood = 4;
        break;
    case 3:
        conditions->mMood = 8;
        break;
    case 4:
        conditions->mMood = 16;
        break;
    }
    conditions->mMonth = sMonthBits[date->month];
    conditions->mHour = sHourBits[time->hour];
    conditions->mWeekday = sWeekdayBits[date->week];
    RTCDate meet;
    RTCTime midnight = Midnight();
    GetMeetDate(&meet);
    s64 met = RTC_ConvertDateTimeToSecond(&meet, &midnight);
    int days = (RTC_ConvertDateTimeToSecond(date, time) - met) / 86400;
    if (days < 0) {
        days = 0;
    }
    for (int i = 8; i >= 0; i--) {
        if (sElapsedDays[i] <= days) {
            conditions->mElapsed = i;
            break;
        }
    }
    conditions->mHasCharacters = mCharacterCount >= 3;
    conditions->mHasFriends = mFriendCount >= 2;
    conditions->mHasLover = mLover != -1;
    conditions->mHasSpouse = mSpouse != -1;
}

extern "C" {
// INTERIM: the talk tables by personality kind, with their entry counts (the file's data, laid out at the end)
extern const Scene40TalkEntry *const sFirstTalks[4];   // The first time the player touches the character
extern const int sFirstTalkCounts[4];
extern const Scene40TalkEntry *const sTouchTalks[4];   // Being touched
extern const int sTouchTalkCounts[4];
extern const Scene40TalkEntry *const sRecentTalks[4];  // One time in ten, when they met within the week
extern const int sRecentTalkCounts[4];
extern const Scene40TalkEntry *const sNewsTalks1[4];   // Something to tell (record byte 0x24A) of kind 1
extern const int sNewsTalk1Counts[4];
extern const Scene40TalkEntry *const sNewsTalks2[4];   // of kind 2
extern const int sNewsTalk2Counts[4];
extern const Scene40TalkEntry *const sNewsTalksFE[4];  // of kind 0xFE (and 2 when its birthday is set)
extern const int sNewsTalkFECounts[4];
}

// A random number below max, compared as an int
static inline int Rand(int max) {
    return MATH_Rand16(&gRandom, max);
}

// The two arguments of a message: up to two, while the entry has a type for them. Two friends are picked at
// random for types 16 and 17.
void Scene40Character::GetTalkArguments(const Scene40TalkEntry *entry, int *count, int *types, int *values) {
    int friends[2] = {0, 0};
    u16 foods[256];
    int n;
    int wanted = 2;
    for (int i = 0; i < mFriendCount; i++) {
        if (Rand(mFriendCount - i) < wanted) {
            friends[2 - wanted] = mFriends[i];
            wanted--;
        }
    }
    if (Rand(100) < 50) {
        int other = friends[0];
        friends[0] = friends[1];
        friends[1] = other;
    }
    for (*count = 0; (*count == 0 ? entry->mArgType0 : entry->mArgType1) != 0 && *count < 2; (*count)++) {
        types[*count] = *count == 0 ? entry->mArgType0 : entry->mArgType1;
        switch (types[*count]) {
        case 1:
        case 4:
        case 7:
        case 10:
        case 15:
            values[*count] = mIndex;
            break;
        case 14:
            values[*count] = 0;
            break;
        case 13:
            n = 0;
            for (int item = 0; item < 255; item++) {
                int kind = gGoodsTable[(u16)item].mKind;
                if (kind == 1 || kind == 4) {
                    foods[n++] = item;
                }
            }
            values[*count] = foods[Rand(n)];
            break;
        case 2:
        case 3:
        case 5:
        case 6:
        case 8:
        case 9:
        case 11:
        case 12:
            values[*count] = 0;
            break;
        case 16:
            values[*count] = friends[0];
            break;
        case 17:
            values[*count] = friends[1];
            break;
        case 18:
            values[*count] = mLover;
            break;
        case 19:
            values[*count] = mSpouse;
            break;
        default:
            values[*count] = 0;
            break;
        }
    }
}

// What the character says when touched: the first time, its first-meeting talk (and the record's bit 0x54.20 is
// set); one time in ten, when they met within the week, a talk of those; otherwise a touch talk. An entry whose
// conditions hold is chosen at random, or the first first-meeting talk if none does.
void Scene40Character::ChooseTouchTalk(u32 *message, int *count, int *types, int *values) {
    RTCDate date;
    RTCTime time;
    Scene40TalkConditions conditions;
    const Scene40TalkEntry *entry;
    Scene40Clock *clock = mContext->mClock;
    date.year = clock->mDate.year;
    date.month = clock->mDate.month;
    date.day = clock->mDate.day;
    date.week = clock->mDate.week;
    clock = mContext->mClock;
    time.hour = clock->mTime.hour;
    time.minute = clock->mTime.minute;
    time.second = clock->mTime.second;
    GetTalkConditions(&date, &time, &conditions);
    if (!mUnknown054Bit20) {
        int found[33];
        int n = 0;
        int total = sFirstTalkCounts[gPersonalityKinds[mPersonality]];
        for (int i = 0; i < total; i++) {
            if (MatchesTalk(&sFirstTalks[gPersonalityKinds[mPersonality]][i], &date, &conditions)) {
                found[n++] = i;
            }
        }
        if (n > 0) {
            entry = &sFirstTalks[gPersonalityKinds[mPersonality]][found[MATH_Rand16(&gRandom, n)]];
        } else {
            entry = sFirstTalks[gPersonalityKinds[mPersonality]];
        }
        mUnknown054Bit20++;
    } else if (MATH_Rand16(&gRandom, 10) != 0 || conditions.mElapsed != 1) {
        int found[65];
        int n = 0;
        int total = sTouchTalkCounts[gPersonalityKinds[mPersonality]];
        for (int i = 0; i < total; i++) {
            if (MatchesTalk(&sTouchTalks[gPersonalityKinds[mPersonality]][i], &date, &conditions)) {
                found[n++] = i;
            }
        }
        if (n > 0) {
            entry = &sTouchTalks[gPersonalityKinds[mPersonality]][found[MATH_Rand16(&gRandom, n)]];
        } else {
            entry = sFirstTalks[gPersonalityKinds[mPersonality]];
        }
    } else {
        int found[48];
        int n = 0;
        int total = sRecentTalkCounts[gPersonalityKinds[mPersonality]];
        for (int i = 0; i < total; i++) {
            if (MatchesTalk(&sRecentTalks[gPersonalityKinds[mPersonality]][i], &date, &conditions)) {
                found[n++] = i;
            }
        }
        if (n > 0) {
            entry = &sRecentTalks[gPersonalityKinds[mPersonality]][found[MATH_Rand16(&gRandom, n)]];
        } else {
            entry = sFirstTalks[gPersonalityKinds[mPersonality]];
        }
    }
    *message = entry->mMessage;
    GetTalkArguments(entry, count, types, values);
}

// Something to tell, of a kind (the record's byte 0x24A): kind 2 becomes 0xFE when the Mii has a birthday set
void Scene40Character::ChooseNewsTalk(int kind, u32 *message, int *count, int *types, int *values) {
    RTCDate date;
    RTCTime time;
    Scene40TalkConditions conditions;
    const Scene40TalkEntry *entry;
    int found[393];
    Scene40Clock *clock = mContext->mClock;
    date.year = clock->mDate.year;
    date.month = clock->mDate.month;
    date.day = clock->mDate.day;
    date.week = clock->mDate.week;
    clock = mContext->mClock;
    time.hour = clock->mTime.hour;
    time.minute = clock->mTime.minute;
    time.second = clock->mTime.second;
    GetTalkConditions(&date, &time, &conditions);
    int n = 0;
    if (kind == 2) {
        int month = GetParameter(PARAM_BIRTH_MONTH);
        int day = GetParameter(PARAM_BIRTH_DAY);
        if (month != 0 && day != 0) {
            kind = 0xFE;
        }
    }
    switch (kind) {
    case 1: {
        int total = sNewsTalk1Counts[gPersonalityKinds[mPersonality]];
        for (int i = 0; i < total; i++) {
            if (MatchesTalk(&sNewsTalks1[gPersonalityKinds[mPersonality]][i], &date, &conditions)) {
                found[n++] = i;
            }
        }
        if (n > 0) {
            entry = &sNewsTalks1[gPersonalityKinds[mPersonality]][found[Rand(n)]];
        } else {
            entry = sFirstTalks[gPersonalityKinds[mPersonality]];
        }
        break;
    }
    case 2: {
        int total = sNewsTalk2Counts[gPersonalityKinds[mPersonality]];
        for (int i = 0; i < total; i++) {
            if (MatchesTalk(&sNewsTalks2[gPersonalityKinds[mPersonality]][i], &date, &conditions)) {
                found[n++] = i;
            }
        }
        if (n > 0) {
            entry = &sNewsTalks2[gPersonalityKinds[mPersonality]][found[Rand(n)]];
        } else {
            entry = sFirstTalks[gPersonalityKinds[mPersonality]];
        }
        break;
    }
    case 0xFE: {
        int total = sNewsTalkFECounts[gPersonalityKinds[mPersonality]];
        for (int i = 0; i < total; i++) {
            if (MatchesTalk(&sNewsTalksFE[gPersonalityKinds[mPersonality]][i], &date, &conditions)) {
                found[n++] = i;
            }
        }
        if (n > 0) {
            entry = &sNewsTalksFE[gPersonalityKinds[mPersonality]][found[Rand(n)]];
        } else {
            entry = sFirstTalks[gPersonalityKinds[mPersonality]];
        }
        break;
    }
    }
    *message = entry->mMessage;
    GetTalkArguments(entry, count, types, values);
}

// A long act with a partner ends for both: the partner's record is read, cleared and written back
void Scene40Character::ClearLongActOfPair() {
    if (IsLongActPaired()) {
        SavedCharacter partner;
        u8 index = mLongActPartner;
        GameData_ReadCharacter(&gGameData, index, &partner);
        partner.mIndex = index;
        partner.ClearLongAct();
        GameData_WriteCharacter(&gGameData, index, &partner);
    }
    ClearLongAct();
}

void Scene40Character::ClearLongActOfPairFrom(s64 now) {
    if (IsLongActPaired()) {
        SavedCharacter partner;
        u8 index = mLongActPartner;
        GameData_ReadCharacter(&gGameData, index, &partner);
        partner.mIndex = index;
        partner.ClearLongActFrom(now);
        GameData_WriteCharacter(&gGameData, index, &partner);
    }
    ClearLongActFrom(now);
}

// Something to tell the player (the record's byte 0x24A: 1, 2 or 0xFE, the kind of talk; 0 or 0xFF for none)
BOOL Scene40Character::HasSomethingToTell() {
    BOOL has = FALSE;
    if (mUnknown24A != 0 && mUnknown24A != 0xFF) {
        has = TRUE;
    }
    return has;
}

// The record's byte 0x246, which the main program sets by chance as the character falls asleep (long acts 0x12
// and 0xF, SavedCharacter::SetUnknown246ByChance): the balloon then shows a dream
BOOL Scene40Character::IsDreaming() {
    return mUnknown246 != 0;
}

// Says what it has to tell (the speech's file 1), and has nothing to tell any more. Kind 1 keeps the year in
// the record's byte 0x2A3.
void Scene40Character::TellNews() {
    if (!HasSomethingToTell()) {
        return;
    }
    u32 message;
    int count;
    int types[2];
    int values[2];
    ChooseNewsTalk(mUnknown24A, &message, &count, types, values);
    Speak(1, message, count, types, values, 0);
    switch (mUnknown24A) {
    case 1:
        mUnknown2A3 = mContext->mClock->mDate.year;
        break;
    case 2:
    case 0xFE:
        break;
    }
    mUnknown24A = 0xFF;
}

// The date it was last patted ("PatYear", "PatMonth", "PatDay") becomes today: TRUE if it was another day
BOOL Scene40Character::UpdatePatDate() {
    RTCDate *today = &mContext->mClock->mDate;
    BOOL changed = FALSE;
    u32 year = today->year;
    u32 month = today->month;
    u32 day = today->day;
    if (year != mPatYear || month != mPatMonth || day != mPatDay) {
        mPatYear = year;
        mPatMonth = month;
        mPatDay = day;
        changed = TRUE;
    }
    return changed;
}

// A trouble in state 1 starts to progress (state 2) from now
void Scene40Character::StartTroubleProgress() {
    if (mTroubleState != 1) {
        return;
    }
    mTroubleState = 2;
    mTroubleStart = mContext->mClock->mSeconds;
}

// The things the character can propose to do (each a scene of its own, which overlay 75 requests), and how many
// saved characters each needs
struct Scene40Activity {
    /* 0x0 */ int mScene;
    /* 0x4 */ u8 mMinCharacters;
    /* 0x5 */ u8 mUnknown5;
    /* 0x6 */ u8 mUnknown6;
    /* 0x7 */ u8 mUnknown7;
    /* 0x8 */ u8 mUnknown8;
    /* 0x9 */ u8 mEnabled;
};

// 0xC

static const Scene40Activity sActivities[20] = {
    {24, 2, 0, 9, 0, 0, 1},    {28, 2, 0, 166, 0, 0, 1},  {43, 2, 1, 14, 1, 0, 1},   {50, 4, 0, 218, 0, 0, 1},
    {54, 1, 1, 24, 1, 0, 1},   {55, 3, 0, 169, 0, 0, 1},  {58, 1, 0, 35, 0, 0, 1},   {61, 2, 1, 104, 0, 0, 1},
    {76, 2, 0, 167, 0, 0, 1},  {77, 2, 0, 151, 0, 0, 1},  {80, 1, 0, 0, 0, 1, 1},    {82, 1, 0, 164, 0, 0, 1},
    {118, 1, 0, 125, 0, 0, 1}, {123, 3, 0, 220, 0, 0, 1}, {136, 1, 0, 219, 0, 0, 1}, {139, 1, 0, 189, 0, 0, 1},
    {141, 1, 0, 153, 0, 0, 1}, {143, 5, 1, 0, 0, 1, 1},   {144, 5, 1, 0, 0, 1, 1},   {145, 2, 0, 0, 0, 1, 1},
};

// One of the activities that are enabled and that there are enough characters for, at random
int Scene40Character::ChooseActivity() {
    int candidates[20];
    int count = 0;
    int i;
    for (i = 0; i < 20; i++) {
        if (sActivities[i].mEnabled == 1 && sActivities[i].mMinCharacters <= mCharacterCount) {
            candidates[count++] = i;
        }
    }
    return candidates[MATH_Rand16(&gRandom, count)];
}

#define gSceneMemoryEx (*(Scene40SceneMemory *)&gSceneMemory)

// The two characters of the next scene, a cleared scratch buffer and the scene's mode
static inline void PrepareTogetherScene(int first, int second, int mode) {
    gSceneMemoryEx.mCharacters[0] = first;
    gSceneMemoryEx.mCharacters[1] = second;
    MI_CpuFill8(&gSceneScratch, 0, 100);
    gSceneMemoryEx.mNextMode = mode;
}

// To scene 0x51 with the character and its trouble's target, mode 3
void Scene40Character::GoToScene51() {
    PrepareTogetherScene(mIndex, mTroubleTarget, 3);
    gSceneManager.RequestScene(0x51);
}

// To one of three scenes of the character with its trouble's target (mode 4): 0x31 one time in three
// when there are at least four characters, 0x51 one time in three, else 0x34
void Scene40Character::GoToTogetherScene() {
    PrepareTogetherScene(mIndex, mTroubleTarget, 4);
    gSceneManager.SetSoundGroup(0x37);
    gSoundManager.StartSeqIfNotPlaying(0, 100);
    int choice = Rand(300);
    if (choice < 100 && mCharacterCount >= 4) {
        gSceneManager.RequestScene(0x31);
    } else if (choice < 200) {
        gSceneManager.RequestScene(0x51);
    } else {
        gSceneManager.RequestScene(0x34);
    }
}

void Scene40Character::GoToScene3C() {
    gSceneMemory.mCharacters[0] = mIndex;
    gSceneMemory.mCharacters[1] = mTroubleTarget;
    gSceneManager.RequestScene(0x3C);
}

// To the wedding (scene 0xAB, overlay 96) of the character and its trouble's target
void Scene40Character::GoToWedding() {
    gSceneMemory.mCharacters[0] = mIndex;
    gSceneMemory.mCharacters[1] = mTroubleTarget;
    gSceneManager.RequestScene(0xAB);
}

// The game keeps a bit for each activity that was proposed (gGameData+0x110)
void MarkActivityProposed(int activity) {
    gGameData.mActivitiesProposed |= 1 << activity;
}

// --- The character's head ---------------------------------------------------------------------------------------

u16 Scene40Character::GetHeadYawTo(VecFx32 *target) {
    return mAngle - FX_Atan2Idx(target->z - mPos.z, target->x - mPos.x);
}

u16 Scene40Character::GetAngleTo(VecFx32 *target) {
    return FX_Atan2Idx(target->z - mPos.z, target->x - mPos.x);
}

// --- The balloon ------------------------------------------------------------------------------------------------

void Scene40Balloon::Init(int resourceId, int unk, CharacterBase *face, Scene40Character *character) {
    mModel.Load(resourceId, unk);
    PlayAnimation(RESOURCE_BCA0, "appear");
    PlayAnimation(RESOURCE_BTP0, "appear");
    mState = STATE_HIDDEN;
    mContent = CONTENT_NONE;
    mUnknown08 = FALSE;
    mCloseQueued = FALSE;
    mCharacter = character;
    mFace = face;
    SetState(STATE_HIDDEN);
}

void Scene40Balloon::Update() {
    switch (mState) {
    case STATE_HIDDEN:
        break;
    case STATE_SHOWN:
        if (mCloseQueued) {
            SetState(STATE_DISAPPEAR);
        }
        break;
    case STATE_APPEAR:
        if (FindAnimation(RESOURCE_BTP0, "appear")->mFinished) {
            SetState(STATE_SHOWN);
        }
        break;
    case STATE_DISAPPEAR:
        mCloseQueued = FALSE;
        if (FindAnimation(RESOURCE_BTP0, "disappear")->mFinished) {
            SetState(STATE_HIDDEN);
        }
        break;
    }
    UpdatePosition();
}

// The length of (x, z), by the hardware square root
static inline fx32 SqrtXZ(fx32 x, fx32 z) {
    fx64 t = (fx64)x * x + (fx64)z * z;
    CP_SetSqrt64((u64)(t << 2));
    return ((s32)CP_GetSqrtResult32() + 1) >> 1;
}

static const VecFx32 sBalloonUp = {0, FX32_ONE, 0};

static inline VecFx32 GetPos(Character *character) {
    return character->mPos;
}

// Over the character's head, beside it as seen from the camera (on the other side when it dreams, lying down),
// turned to the camera; the face in it looks at the camera
void Scene40Balloon::UpdatePosition() {
    if (mState == STATE_HIDDEN) {
        return;
    }
    VecFx32 offset;
    VecFx32 side;
    VecFx32 up = sBalloonUp;
    VecFx32 toCamera;
    VecFx32 head;
    Camera *camera = &gCamera;
    if (mContent == CONTENT_DREAM) {
        head = mCharacter->GetJointPosition(CharacterModel::JOINT_NECK);
        VEC_Subtract(&head, &camera->mEye, &toCamera);
        VEC_CrossProduct(&up, &toCamera, &side);
        VEC_Normalize(&side, &offset);
        ScaleVec(-0xCCC, &offset, &offset);
        offset.y += 0x1666;
        VEC_Add(&offset, &head, &mModel.mPosition);
        mModel.mRotY = FX_Atan2Idx(-toCamera.z, toCamera.x) + 0x4000;
    } else {
        head = GetPos(mCharacter);
        head.y = mCharacter->GetJointPosition(CharacterModel::JOINT_NECK).y;
        VEC_Subtract(&head, &camera->mEye, &toCamera);
        VEC_CrossProduct(&up, &toCamera, &side);
        VEC_Normalize(&side, &offset);
        ScaleVec(0x1666, &offset, &offset);
        offset.y += 0x14CC;
        VEC_Add(&offset, &head, &mModel.mPosition);
        mModel.mRotY = FX_Atan2Idx(-toCamera.z, toCamera.x) - 0x4000;
    }
    VecFx32 eye = camera->mEye;
    fx32 y = mModel.mPosition.y;
    u16 pitch = FX_Atan2Idx(eye.y - y, SqrtXZ(eye.x - mFace->mFaceOffset.x, eye.z - mFace->mFaceOffset.z));
    u16 yaw = -FX_Atan2Idx(eye.z - mFace->mFaceOffset.z, eye.x - mFace->mFaceOffset.x);
    mFace->mHeadPitchNow = -pitch;
    mFace->mHeadYawNow = yaw + 0x4000;
    MtxFx43 *matrix = &mFace->mHeadMatrix;
    matrix->_30 = mModel.mPosition.x;
    matrix->_31 = mModel.mPosition.y - 0x666;
    matrix->_32 = mModel.mPosition.z;
}

void Scene40Balloon::SetState(int state) {
    switch (state) {
    case STATE_HIDDEN:
        mState = STATE_HIDDEN;
        mModel.mVisible = FALSE;
        break;
    case STATE_SHOWN:
        mState = STATE_SHOWN;
        mModel.mVisible = TRUE;
        switch (mContent) {
        case CONTENT_NONE:
            break;
        case CONTENT_TROUBLE:
            PlayAnimation(RESOURCE_BCA0, "wait_etc");
            PlayAnimation(RESOURCE_BTP0, "wait_etc");
            FindAnimation(RESOURCE_BTP0, "wait_etc")->mPlayOnce = FALSE;
            break;
        case CONTENT_FACE:
            PlayAnimation(RESOURCE_BCA0, "wait_plain");
            PlayAnimation(RESOURCE_BTP0, "wait_plain");
            FindAnimation(RESOURCE_BTP0, "wait_plain")->mPlayOnce = FALSE;
            mFace->mFaceVisible = TRUE;
            break;
        case CONTENT_DOTS:
            PlayAnimation(RESOURCE_BCA0, "wait_dot");
            PlayAnimation(RESOURCE_BTP0, "wait_dot");
            FindAnimation(RESOURCE_BTP0, "wait_dot")->mPlayOnce = FALSE;
            break;
        case CONTENT_DREAM:
            PlayAnimation(RESOURCE_BCA0, "wait_dream");
            PlayAnimation(RESOURCE_BTP0, "wait_dream");
            FindAnimation(RESOURCE_BTP0, "wait_dream")->mPlayOnce = FALSE;
            break;
        }
        break;
    case STATE_APPEAR:
        if (mContent == CONTENT_NONE) {
            return;
        }
        if (mUnknown08) {
            return;
        }
        mState = STATE_APPEAR;
        mModel.mVisible = TRUE;
        PlayAnimation(RESOURCE_BCA0, "appear");
        PlayAnimation(RESOURCE_BTP0, "appear");
        FindAnimation(RESOURCE_BTP0, "appear")->mPlayOnce = TRUE;
        mModel.mScale.x = FX32_ONE;
        mModel.mScale.y = FX32_ONE;
        mModel.mScale.z = FX32_ONE;
        break;
    case STATE_DISAPPEAR:
        if (mState == STATE_SHOWN) {
            mState = STATE_DISAPPEAR;
            mModel.mVisible = TRUE;
            PlayAnimation(RESOURCE_BCA0, "disappear");
            PlayAnimation(RESOURCE_BTP0, "disappear");
            FindAnimation(RESOURCE_BTP0, "disappear")->mPlayOnce = TRUE;
            mFace->mFaceVisible = FALSE;
            mCloseQueued = FALSE;
        } else if (mState != STATE_HIDDEN) {
            mCloseQueued = TRUE;
        }
        break;
    }
}

// What the balloon shows, by what the character is doing and its trouble
void Scene40Balloon::ChooseContent() {
    BOOL asleep = mCharacter->mLongActState == 18;
    BOOL act15 = mCharacter->mLongActState == 15;
    BOOL inTrouble = mCharacter->IsInTrouble();
    BOOL hasNews = mCharacter->HasSomethingToTell();
    BOOL dreaming = mCharacter->IsDreaming();
    BOOL thinksOfSomeone = FALSE;
    if (inTrouble && mCharacter->mTroubleCategory == 4) {
        thinksOfSomeone = TRUE;
    }
    mContent = CONTENT_NONE;
    if (asleep || act15) {
        if (dreaming) {
            mContent = CONTENT_DREAM;
        }
        return;
    }
    if (thinksOfSomeone) {
        mContent = CONTENT_FACE;
        u16 target = mCharacter->mTroubleTarget;
        GameData_ReadCharacter(&gGameData, target, mFace);
        mFace->mIndex = target;
        mFace->MakeFace();
        return;
    }
    if (inTrouble) {
        mContent = CONTENT_TROUBLE;
        return;
    }
    if (hasNews) {
        mContent = CONTENT_DOTS;
    }
}

// Whether the line through the touched point passes within 0.5 of the balloon, while it is out
BOOL Scene40Balloon::IsTouched() {
    VecFx32 offset;
    VecFx32 pos = mModel.mPosition;
    VecFx32 dir = gTouchLineDirection;
    VEC_Subtract(&gCamera.mEye, &pos, &offset);
    return LineMeetsSphere(&offset, &dir, FX32_HALF) && mState != STATE_HIDDEN;
}

// --- The cover --------------------------------------------------------------------------------------------------

void Scene40Cover::Init(int kind) {
    mKind = kind;
    switch (kind) {
    case KIND_BED:
        mModel.Load(4, 0);
        mModel.mVisible = FALSE;
        break;
    case KIND_CURTAIN:
        mModel.Load(4, 0);
        PlayAnimation(RESOURCE_BCA0, "appear00");
        PlayAnimation(RESOURCE_BTA0, "appear00");
        mModel.mVisible = FALSE;
        mDisappearing = FALSE;
        break;
    }
}

// Swaps the model: the bed or the curtain, with their archives. The curtain's path has no directory here, unlike
// the one StartLoading mounts ("Mdl/Misc/Tch/mii_tch_curtain_3D_LZ.bin"): so the ROM has it.
void Scene40Cover::SetKind(int kind) {
    if (mKind == kind) {
        return;
    }
    mKind = kind;
    switch (kind) {
    case KIND_BED:
        ModelResources_ReleaseModel(gModelResources, 4);
        ArchiveManager_Unmount(&gArchiveManager, "3HT");
        ArchiveManager_MountCompressed(&gArchiveManager, "3HT", "Mdl/Goods/gd10_3D_LZ.bin", 1);
        ModelResources_SetModel(gModelResources, 4, "3HT:/goods.imd");
        mModel.Load(4, 0);
        mModel.mVisible = FALSE;
        break;
    case KIND_CURTAIN:
        ModelResources_ReleaseModel(gModelResources, 4);
        ArchiveManager_Unmount(&gArchiveManager, "3HT");
        ArchiveManager_MountCompressed(&gArchiveManager, "3HT", "mii_tch_curtain_3D_LZ.bin", 1);
        ModelResources_SetModel(gModelResources, 4, "3HT:/curtain.imd");
        ModelResources_SetModel(gModelResources, 4, "3HT:/appear00.ica");
        ModelResources_SetModel(gModelResources, 4, "3HT:/appear00.ita");
        mModel.Load(4, 0);
        PlayAnimation(RESOURCE_BCA0, "appear00");
        PlayAnimation(RESOURCE_BTA0, "appear00");
        mModel.mVisible = FALSE;
        mDisappearing = FALSE;
        break;
    }
}

void Scene40Cover::SetPosition(VecFx32 pos, u16 angle) {
    switch (mKind) {
    case KIND_BED:
        mModel.SetPosition(pos);
        mModel.mRotY = angle;
        break;
    case KIND_CURTAIN:
        mModel.SetPosition(pos);
        mModel.mRotY = angle;
        break;
    }
}

void Scene40Cover::SetDiffuse(GXRgb color) {
    NNS_G3dMdlSetMdlDiff(mModel.mResMdl, 0, color);
}

// The curtain comes down around the character
void Scene40Cover::Appear() {
    if (mKind != KIND_CURTAIN) {
        return;
    }
    PlayAnimation(RESOURCE_BCA0, "appear00");
    PlayAnimation(RESOURCE_BTA0, "appear00");
    AnimModel_SetSpeed(&mModel, FX32_ONE);
    FindAnimation(RESOURCE_BCA0, "appear00")->mPlayOnce = TRUE;
    FindAnimation(RESOURCE_BTA0, "appear00")->mPlayOnce = TRUE;
    mModel.RestartAnims();
    mModel.mVisible = TRUE;
    mDisappearing = FALSE;
}

void Scene40Cover::PlayWait() {
    if (mKind != KIND_CURTAIN) {
        return;
    }
    PlayAnimation(RESOURCE_BCA0, "wait00");
    AnimModel_SetSpeed(&mModel, FX32_ONE);
    FindAnimation(RESOURCE_BCA0, "wait00")->mPlayOnce = FALSE;
}

// The curtain goes: "appear00" from its end, backwards
void Scene40Cover::Disappear() {
    if (mKind != KIND_CURTAIN) {
        return;
    }
    PlayAnimation(RESOURCE_BCA0, "appear00");
    PlayAnimation(RESOURCE_BTA0, "appear00");
    func_0202a5a4(&mModel, ((NNSG3dResAnmCommon *)FindAnimation(RESOURCE_BTA0, "appear00")->mAnmObj->resAnm)->numFrame * FX32_ONE);
    AnimModel_SetSpeed(&mModel, -FX32_ONE);
    FindAnimation(RESOURCE_BCA0, "appear00")->mPlayOnce = TRUE;
    FindAnimation(RESOURCE_BTA0, "appear00")->mPlayOnce = TRUE;
    mModel.RestartAnims();
    mModel.mVisible = TRUE;
    mDisappearing = TRUE;
}

BOOL Scene40Cover::HasAppeared() {
    if (mKind != KIND_CURTAIN) {
        return FALSE;
    }
    BOOL done = FALSE;
    if (FindAnimation(RESOURCE_BTA0, "appear00")->mFinished && !mDisappearing) {
        done = TRUE;
    }
    return done;
}

BOOL Scene40Cover::HasDisappeared() {
    if (mKind != KIND_CURTAIN) {
        return FALSE;
    }
    BOOL done = FALSE;
    if (FindAnimation(RESOURCE_BTA0, "appear00")->mFinished && mDisappearing) {
        done = TRUE;
    }
    return done;
}

// --- The stylus -------------------------------------------------------------------------------------------------

static inline ScreenPoint ZeroPoint() {
    ScreenPoint point = {0, 0};
    return point;
}

void Scene40Touch::Init() {
    mPressed = FALSE;
    mHeld = FALSE;
    mReleased = FALSE;
    mSmooth = FALSE;
    Reset();
    mUnknownB4 = 0;
    mTaps = 0;
    mTapFrames = 0;
}

void Scene40Touch::Reset() {
    mPressPoint = ZeroPoint();
    mPoint = ZeroPoint();
    mReleasePoint = ZeroPoint();
    mDirection = ZeroPoint();
    mDistance = 0;
    mHeldFrames = 0;
    mVelocity = ZeroPoint();
    mMoveIndex = 0;
    for (int i = 0; i < 5; i++) {
        mMoves[i] = ZeroPoint();
    }
    if (!mSmooth) {
        return;
    }
    mPointCount = 0;
    mPointIndex = 0;
    for (int i = 0; i < 8; i++) {
        mPoints[i].x = 0;
        mPoints[i].y = 0;
    }
}

// The mean of the last five moves
ScreenPoint Scene40Touch::GetVelocity() {
    ScreenPoint sum = ZeroPoint();
    for (int i = 0; i < 5; i++) {
        sum.x += mMoves[i].x;
        sum.y += mMoves[i].y;
    }
    sum.x /= 5;
    sum.y /= 5;
    return sum;
}

// A tap counts when it comes 3 to 30 frames after the one before
void Scene40Touch::CountTap() {
    if (mTaps == 0) {
        mTaps++;
        mTapFrames = 0;
        return;
    }
    if (mTapFrames >= 3 && mTapFrames <= 30) {
        mTaps++;
    }
}

// --- The camera -------------------------------------------------------------------------------------------------

static const VecFx32 sCameraStartPosition = {0, 0x41D0, 0xC000};

static const VecFx32 sCameraStartTarget = {0, 0x1999, 0};

static const VecFx32 sCameraTarget = {0, 0x1999, 0};

static inline void CopyVec(VecFx32 *dst, const VecFx32 *src) {
    dst->x = src->x;
    dst->y = src->y;
    dst->z = src->z;
}

static inline void SetVec(VecFx32 *dst, VecFx32 src) {
    dst->x = src.x;
    dst->y = src.y;
    dst->z = src.z;
}

static inline void SetCameraPosition(Camera *camera, VecFx32 position) {
    camera->mPosition.x = position.x;
    camera->mPosition.y = position.y;
    camera->mPosition.z = position.z;
}

static inline void SetCameraTarget(Camera *camera, VecFx32 target) {
    camera->mTarget.x = target.x;
    camera->mTarget.y = target.y;
    camera->mTarget.z = target.z;
}

// The shift is unsigned, as overlay 96's
static inline VecFx32 ShiftVec(const VecFx32 &v, u32 shift) {
    VecFx32 result;
    result.x = v.x >> shift;
    result.y = v.y >> shift;
    result.z = v.z >> shift;
    return result;
}

// (x, z) made one long, as VEC_Normalize does it (overlay 96's NormalizeXZ)
static inline FloorPos NormalizeXZ(fx32 x, fx32 z) {
    FloorPos result;
    fx64 t = (fx64)x * x + (fx64)z * z;
    CP_SetSqrt64((u64)(t << 2));
    CP_SetDiv64_64(1LL << (FX32_SHIFT + FX32_SHIFT + FX64C_SHIFT), (u64)t);
    s32 sqrt = (s32)CP_GetSqrtResult32();
    t = CP_GetDivResult64();
    t = t * sqrt;
    result.x = (fx32)((t * x + (1LL << (32 + FX32_SHIFT))) >> (32 + FX32_SHIFT + 1));
    result.z = (fx32)((t * z + (1LL << (32 + FX32_SHIFT))) >> (32 + FX32_SHIFT + 1));
    return result;
}

static inline fx32 ClampFx(fx32 value, fx32 min, fx32 max) {
    if (value > max) {
        value = max;
    } else if (value < min) {
        value = min;
    }
    return value;
}

void Scene40Camera::Init(Scene40Character *character) {
    mCamera = &gCamera;
    Camera_Reset(mCamera);
    SetCameraPosition(mCamera, sCameraStartPosition);
    SetCameraTarget(mCamera, sCameraStartTarget);
    mCamera->mAngleX = 0;
    mCamera->mAngleY = 0;
    mCamera->mAngleZ = 0;
    Camera_UpdateEye(mCamera);
    mCharacter = character;
    CopyVec(&mPosition, &mCamera->mPosition);
    CopyVec(&mTarget, &mCamera->mTarget);
    mState = 1;
}

// Coming back from another scene: the camera as that scene left it, put 14 in front of the target
void Scene40Camera::Reset(BOOL restore) {
    if (!restore) {
        return;
    }
    SetVec(&mPosition, gSceneMemory.mCameraPosition);
    SetVec(&mTarget, gSceneMemory.mCameraTarget);
    VEC_Subtract(&mPosition, &mTarget, &mPosition);
    VecFx32 target = sCameraTarget;
    mTarget.x = target.x;
    mTarget.y = target.y;
    mTarget.z = target.z;
    PlaceAround(&mPosition, &mTarget, &mPosition);
    CopyVec(&mCamera->mPosition, &mPosition);
    CopyVec(&mCamera->mTarget, &mTarget);
    Camera_UpdateEye(mCamera);
}

// Follows the character within the room, keeping the distance and the height
void Scene40Camera::Update() {
    switch (mState) {
    case 0:
        return;
    case 1:
        break;
    default:
        return;
    }
    VecFx32 middle;
    VecFx32 position;
    VecFx32 pos = mCharacter->mPos;
    middle = pos;
    middle.x = ClampFx(middle.x, -0x3000, 0x3000);
    middle.z = ClampFx(middle.z, -0x3000, 0x3000);
    middle.y = mTarget.y;
    mTarget = Approach(&middle, &mTarget, 5);
    PlaceAround(&mPosition, &middle, &position);
    mPosition = Approach(&position, &mPosition, 5);
    CopyVec(&mCamera->mTarget, &mTarget);
    CopyVec(&mCamera->mPosition, &mPosition);
}

void Scene40Camera::SetState(int state) {
    mState = state;
}

// result: 14 from the target, in the direction of position from it, at the height 0x41D0
void Scene40Camera::PlaceAround(VecFx32 *position, VecFx32 *target, VecFx32 *result) {
    FloorPos direction = NormalizeXZ(position->x - target->x, position->z - target->z);
    result->x = target->x + direction.x * 14;
    result->z = target->z + direction.z * 14;
    result->y = 0x41D0;
}

// The camera in front of the character, looking at it
void Scene40Camera::ViewFromFront() {
    VecFx32 pos = mCharacter->mPos;
    mTarget.x = pos.x;
    mTarget.y = pos.y;
    mTarget.z = pos.z;
    mTarget.x = ClampFx(mTarget.x, -0x3000, 0x3000);
    mTarget.z = ClampFx(mTarget.z, -0x3000, 0x3000);
    mTarget.y = 0x1999;
    u16 angle = mCharacter->mAngle;
    mPosition.x = FX_Mul(FX_CosIdx(angle), FX32_CONST(200));
    mPosition.z = FX_Mul(FX_SinIdx(angle), FX32_CONST(200));
    PlaceAround(&mPosition, &mTarget, &mPosition);
    CopyVec(&mCamera->mTarget, &mTarget);
    CopyVec(&mCamera->mPosition, &mPosition);
    Camera_UpdateEye(mCamera);
}

void Scene40Camera::SetPosition(VecFx32 position) {
    mPosition.x = position.x;
    mPosition.y = position.y;
    mPosition.z = position.z;
    CopyVec(&mCamera->mPosition, &mPosition);
}

void Scene40Camera::SetTarget(VecFx32 target) {
    mTarget.x = target.x;
    mTarget.y = target.y;
    mTarget.z = target.z;
    CopyVec(&mCamera->mTarget, &mTarget);
}

// A step from current towards goal: 1 / 2^shift of the way
VecFx32 Scene40Camera::Approach(VecFx32 *goal, VecFx32 *current, int shift) {
    VecFx32 offset;
    VEC_Subtract(goal, current, &offset);
    offset = ShiftVec(offset, shift);
    VEC_Add(&offset, current, &offset);
    return offset;
}

static inline FloorPos ToFloorPos(VecFx32 v) {
    FloorPos pos;
    pos.x = v.x;
    pos.z = v.z;
    return pos;
}

// Where the camera's eye is, on the floor
FloorPos Scene40Camera::GetEyeFloorPos() {
    VecFx32 eye = mCamera->mEye;
    return ToFloorPos(eye);
}

// --- The lights -------------------------------------------------------------------------------------------------

static const VecFx32 sLightDirections[2] = {{0x640, -0x258, -0xFA0}, {0, -0xFA0, 0x5DC}};

void Scene40Lights::Init(Camera *camera, Room *room) {
    VecFx32 view;
    mState = 0;
    mCamera = camera;
    mRoom = room;
    VEC_Subtract(&mCamera->mPosition, &mCamera->mTarget, &view);
    u16 angle = FX_Atan2Idx(view.x, view.z);
    for (int i = 0; i < 2; i++) {
        mLights[i] = &gLights[i];
        Light_Reset(&gLights[i]);
        mLights[i]->mColor = gInteriorTable[mRoom->mId].mLightColor;
        Light_SetDirection(mLights[i], 6000, angle, 0, sLightDirections[i]);
    }
}

void Scene40Lights::Update() {
    VecFx32 view;
    switch (mState) {
    case 0: {
        VEC_Subtract(&mCamera->mEye, &mCamera->mTarget, &view);
        u16 angle = FX_Atan2Idx(view.x, view.z);
        for (int i = 0; i < 2; i++) {
            Light_SetDirection(mLights[i], 6000, angle, 0, sLightDirections[i]);
        }
        break;
    }
    case 1:
        SetColor(mRoom->GetLightColor());
        mState = 0;
        break;
    }
}

void Scene40Lights::SetState(int state) {
    mState = state;
}

void Scene40Lights::SetColor(u16 color) {
    for (int i = 0; i < 2; i++) {
        mLights[i]->mColor = color;
    }
}

// --- Falling and bouncing ---------------------------------------------------------------------------------------

void Scene40Physics::Init() {
    mCharacter = NULL;
    for (int i = 0; i < 10; i++) {
        mItems[i] = NULL;
    }
    mItemCount = 0;
}

void Scene40Physics::AddItem(Scene40ItemModel *item) {
    mItems[mItemCount++] = item;
}

void Scene40Physics::Update() {
    UpdateCharacter();
    UpdateItems();
}

void Scene40Motion::Reset() {
    VecFx32 zero = {0, 0, 0};
    mVelocity = zero;
    mAtRest = FALSE;
    mHeld = FALSE;
    mOffered = FALSE;
}

// At rest: on the floor and not moving
void Scene40Motion::UpdateAtRest(VecFx32 *pos, VecFx32 *velocity) {
    BOOL atRest = TRUE;
    if (pos->y != 0 || velocity->x != 0 || velocity->y != 0 || velocity->z != 0) {
        atRest = FALSE;
    }
    mAtRest = atRest;
}

// The item models that are (or are not) visible
int Scene40Physics::GetItems(Scene40ItemModel **items, int *count, BOOL visible) {
    *count = 0;
    for (int i = 0; i < mItemCount; i++) {
        if (visible == mItems[i]->mVisible) {
            items[(*count)++] = mItems[i];
        }
    }
    return *count;
}

// One item model no one uses yet, marked used: 1 if there was one
int Scene40Physics::TakeFreeItem(Scene40ItemModel **item) {
    int taken = 0;
    for (int i = 0; i < mItemCount; i++) {
        if (!mItems[i]->mInUse) {
            mItems[i]->mInUse = TRUE;
            taken++;
            *item = mItems[i];
            break;
        }
    }
    return taken;
}

// Frames until a thing at a height with an upward velocity reaches the floor, falling by 100 a frame
int Scene40Physics::GetFallFrames(fx32 height, fx32 velocity) {
    return FX_Div(-velocity - FX_Sqrt(FX_Mul(velocity, velocity) - FX_Mul(height * 2, -100)), -100) >> FX32_SHIFT;
}

// --- The item models --------------------------------------------------------------------------------------------

void Scene40ItemModel::Load(int resourceId) {
    LoadModel(resourceId);
    mMotion.Reset();
    mInUse = 0;
}

// Out of sight: transparent, at its normal size, and far away
void Scene40ItemModel::Hide() {
    mVisible = FALSE;
    mMotion.mHeld = FALSE;
    mMotion.mOffered = 0;
    mInUse = 0;
    Model_SetAlpha(this, 31);
    mPosition.x = 0x7FFFFFFF;
    mPosition.y = 0x7FFFFFFF;
    mPosition.z = 0x7FFFFFFF;
    mScale.x = FX32_ONE;
    mScale.y = FX32_ONE;
    mScale.z = FX32_ONE;
}

// Shows another item: lets go of the old one's files, and loads the new one's (as overlay 96's WeddingGiftModel)
void Scene40ItemModel::ChangeItem(int item) {
    ModelResources_ReleaseModel(gModelResources, mResourceId);
    ArchiveManager_Unmount(&gArchiveManager, mArchiveName);
    SetItem(mArchiveName, item);
    ArchiveManager_MountCompressed(&gArchiveManager, mArchiveName, mArchivePath, 1);
    ModelResources_SetModel(gModelResources, mResourceId, mModelPath);
    LoadAnimation(mResourceId);
    ResetScale();
}

// Where a character looks at it: half a unit above it
VecFx32 Scene40ItemModel::GetLookPoint() {
    VecFx32 point = mPosition;
    point.y += FX32_HALF;
    return point;
}

// --- The character: the item in its hands -----------------------------------------------------------------------

static inline fx32 LengthSq(const VecFx32 *v) {
    return FX_Mul(v->x, v->x) + FX_Mul(v->y, v->y) + FX_Mul(v->z, v->z);
}

// Puts the held item in the right hand: the hand, or a little way from the elbow towards the hand if the hand is
// nearer than that (PlaceHeldItem with the left hand left out)
void Scene40Character::PlaceHeldItemInHand() {
    VecFx32 middle;
    VecFx32 rightHand;
    VecFx32 rightArm;
    VecFx32 rightOffset;
    VecFx32 rightReach;
    VecFx32 rightPoint;

    if (mHeldItem == NULL) {
        return;
    }
    mTrackLimbs = TRUE;
    rightHand = GetJointPosition(JOINT_R_HAND);
    rightArm = GetJointPosition(JOINT_R_ELBOW);

    VEC_Subtract(&rightHand, &rightArm, &rightOffset);
    VEC_Normalize(&rightOffset, &rightReach);
    ScaleVec(0x999, &rightReach, &rightReach);
    if (LengthSq(&rightReach) > LengthSq(&rightOffset)) {
        VEC_Add(&rightArm, &rightReach, &rightPoint);
        rightPoint.y = rightHand.y;
    } else {
        rightPoint = rightHand;
    }
    middle = rightPoint;
    mHeldItem->SetPosition(middle);
    mHeldItem->mMotion.mHeld = TRUE;
}

void Scene40Character::PlaceHeldItem() {
    PlaceHeldItem(FX32_ONE);
}

// Puts the held item between the hands, each hand's point found as above. Goods 247 hang lower, by 0xD99 scaled.
void Scene40Character::PlaceHeldItem(fx32 scale) {
    VecFx32 middle;
    VecFx32 rightHand;
    VecFx32 leftHand;
    VecFx32 rightArm;
    VecFx32 leftArm;
    VecFx32 rightOffset;
    VecFx32 leftOffset;
    VecFx32 rightReach;
    VecFx32 leftReach;
    VecFx32 rightPoint;
    VecFx32 leftPoint;

    if (mHeldItem == NULL) {
        return;
    }
    mTrackLimbs = TRUE;
    rightHand = GetJointPosition(JOINT_R_HAND);
    rightArm = GetJointPosition(JOINT_R_ELBOW);
    leftHand = GetJointPosition(JOINT_L_HAND);
    leftArm = GetJointPosition(JOINT_L_ELBOW);

    VEC_Subtract(&rightHand, &rightArm, &rightOffset);
    VEC_Normalize(&rightOffset, &rightReach);
    ScaleVec(0x999, &rightReach, &rightReach);
    if (LengthSq(&rightReach) > LengthSq(&rightOffset)) {
        VEC_Add(&rightArm, &rightReach, &rightPoint);
        rightPoint.y = rightHand.y;
    } else {
        rightPoint = rightHand;
    }

    VEC_Subtract(&leftHand, &leftArm, &leftOffset);
    VEC_Normalize(&leftOffset, &leftReach);
    ScaleVec(0x999, &leftReach, &leftReach);
    if (LengthSq(&leftReach) > LengthSq(&leftOffset)) {
        VEC_Add(&leftArm, &leftReach, &leftPoint);
        leftPoint.y = leftHand.y;
    } else {
        leftPoint = leftHand;
    }

    VEC_Add(&rightPoint, &leftPoint, &middle);
    middle.x /= 2;
    middle.y /= 2;
    middle.z /= 2;
    if (mHeldItem->mId == 247) {
        if (scale == FX32_ONE) {
            middle.y += -0xD99;
        } else {
            middle.y -= FX_Mul(scale, 0xD99);
        }
        middle.y = middle.y < 0 ? 0 : middle.y;
    }
    mHeldItem->SetPosition(middle);
    mHeldItem->mMotion.mHeld = TRUE;
}

// The cover between the hands, on the floor, turned as the body
void Scene40Character::PlaceCoverAtHands() {
    MtxFx43 matrix;
    MtxFx33 rotation;
    GetJointMatrices(JOINT_R_HAND, &matrix, &rotation);
    fx32 rightX = matrix._30;
    fx32 rightZ = matrix._32;
    GetJointMatrices(JOINT_L_HAND, &matrix, &rotation);
    VecFx32 pos;
    pos.x = (rightX + matrix._30) / 2;
    pos.y = 0;
    pos.z = (rightZ + matrix._32) / 2;
    mCover.SetPosition(pos, mAnim.mRotY);
}

// The mouth open (face 7) from an eighth to half of the body's animation, closed (face 1) otherwise
void Scene40Character::UpdateMouthByAnimation() {
    ModelAnimation *anim = AnimModel_FindAnimation(&mAnim, RESOURCE_BCA0, mAnimId);
    fx32 frames = ((NNSG3dResAnmCommon *)anim->mAnmObj->resAnm)->numFrame * FX32_ONE;
    fx32 frame = anim->mAnmObj->frame;
    if (frame >= frames >> 3 && frame <= frames >> 1) {
        SetFace(7);
    } else {
        SetFace(1);
    }
}

// --- The character: what it was taught, its head and its speech --------------------------------------------------

extern "C" {
wchar_t *wcsncpy(wchar_t *dst, const wchar_t *src, size_t n);
}

void Scene40Character::SetTaughtWord(const wchar_t *word) {
    MI_CpuClear16(mTaughtWord, sizeof(mTaughtWord));
    wcsncpy(mTaughtWord, word, 16);
}

void Scene40Character::SetTaughtText(const wchar_t *text) {
    MI_CpuClear16(mTaughtText, sizeof(mTaughtText));
    wcsncpy(mTaughtText, text, 64);
}

// Outside the middle of the room (+-1 each way) it walks back to the middle, turned to the camera
BOOL Scene40Character::WalkToMiddleIfAway() {
    BOOL away = FALSE;
    fx32 eyeX = gCamera.mEye.x;
    fx32 eyeZ = gCamera.mEye.z;
    if (mPos.x < -FX32_ONE || mPos.x > FX32_ONE || mPos.z < -FX32_ONE || mPos.z > FX32_ONE) {
        mUnknown2EA0.x = 0;
        mUnknown2EA0.z = 0;
        mUnknown2EA8.x = eyeX;
        mUnknown2EA8.z = eyeZ;
        SetTargetFloorPos(mUnknown2EA0);
        away = TRUE;
    }
    return away;
}

// Not turned to the camera: it walks towards it
BOOL Scene40Character::WalkToCameraIfNotFacing() {
    BOOL turning = FALSE;
    fx32 eyeX = gCamera.mEye.x;
    fx32 eyeZ = gCamera.mEye.z;
    if (!IsFacingCamera()) {
        mUnknown2EA8.x = eyeX;
        mUnknown2EA8.z = eyeZ;
        SetTargetFloorPos(mUnknown2EA8);
        turning = TRUE;
    }
    return turning;
}

// Takes the visible item the player holds out (the last of them), if any
BOOL Scene40Character::TakeOfferedItem() {
    Scene40ItemModel *items[10];
    int count;
    int index;
    BOOL found = FALSE;
    mContext->mPhysics->GetItems(items, &count, TRUE);
    for (int i = 0; i < count; i++) {
        if (items[i]->mMotion.mOffered) {
            index = i;
            found = TRUE;
        }
    }
    if (found) {
        mHeldItem = items[index];
    }
    return found;
}

// The head stops turning and looks ahead
void Scene40Character::StopHeadTurn() {
    mHeadTurn = 0;
    mHeadPitchNow = 0;
    mHeadYawNow = 0;
    mHeadRollNow = 0;
}

void Scene40Character::ResetHeadTurn() {
    mHeadTurn = 1;
    mHeadPitch = 0;
    mHeadYaw = 0;
    mHeadRoll = 0;
}

static inline u16 AngleTo(Character *character, FloorPos target) {
    FloorPos delta = target - character->GetFloorPos();
    return FX_Atan2Idx(delta.z, delta.x);
}

void Scene40Character::FaceCamera() {
    FloorPos camera;
    camera.x = gCamera.mEye.x;
    camera.z = gCamera.mEye.z;
    Character::SetAngle(AngleTo(this, camera));
}

// Turns the head towards a point, as far as the limits allow
void Scene40Character::TurnHeadTo(VecFx32 *target, int pitchLimit, int yawLimit) {
    u16 pitch = GetHeadPitchTo(target);
    u16 yaw = GetHeadYawTo(target);
    mHeadTurn = TRUE;
    CharacterBase::ClampHeadAngle(&pitch, pitchLimit);
    CharacterBase::ClampHeadAngle(&yaw, yawLimit);
    mHeadPitch = pitch;
    mHeadYaw = yaw;
    mHeadRoll = 0;
}

void Scene40Character::TurnHeadToCamera() {
    TurnHeadTo(&gCamera.mEye, 0x2000, 0x3F00);
}

void Scene40Character::TurnHeadToHeldItem(int pitchLimit, int yawLimit) {
    if (mHeldItem == NULL) {
        return;
    }
    VecFx32 point = mHeldItem->GetLookPoint();
    TurnHeadTo(&point, pitchLimit, yawLimit);
}

// The same, at once
void Scene40Character::SnapHeadTo(VecFx32 *target, int pitchLimit, int yawLimit) {
    u16 pitch = GetHeadPitchTo(target);
    u16 yaw = GetHeadYawTo(target);
    mHeadTurn = TRUE;
    CharacterBase::ClampHeadAngle(&pitch, pitchLimit);
    CharacterBase::ClampHeadAngle(&yaw, yawLimit);
    mHeadPitch = pitch;
    mHeadYaw = yaw;
    mHeadRoll = 0;
    mHeadPitchNow = pitch;
    mHeadYawNow = yaw;
    mHeadRollNow = 0;
}

void Scene40Character::SnapHeadToCamera() {
    SnapHeadTo(&gCamera.mEye, 0x2000, 0x3F00);
}

u16 Scene40Character::GetHeadPitchTo(VecFx32 *target) {
    MtxFx43 matrix;
    MtxFx33 rotation;
    GetJointMatrices(0, &matrix, &rotation);
    fx32 dz = target->z - mPos.z;
    fx32 dx = target->x - mPos.x;
    fx32 eyes = matrix._31 + 0xCCC;
    return -FX_Atan2Idx(target->y - eyes, SqrtXZ(dx, dz));
}

void Scene40Character::Speak(int file, u32 message, int type, int value, int mode) {
    Speak(file, message, 1, &type, &value, mode);
}

void Scene40Character::Speak(int file, u32 message, int mode) {
    Speak(file, message, 0, 0, mode);
}

// How old a character is on a date: -1 without a birth year
int Scene40Character::GetAge(SavedCharacter *character, RTCDate *date) {
    int year = date->year + 2000;
    int month = date->month;
    int day = date->day;
    int birthYear = character->mBirthYear;
    int birthMonth = character->GetParameter(PARAM_BIRTH_MONTH);
    int birthDay = character->GetParameter(PARAM_BIRTH_DAY);
    if (birthYear == 0xFFF) {
        return -1;
    }
    if (day < birthDay) {
        month--;
    }
    if (month < birthMonth) {
        year--;
    }
    int age = year - birthYear;
    if (age < 0) {
        age = -1;
    }
    return age;
}

// --- The file's static initialiser ------------------------------------------------------------------------------

// A text position of the profile page (read by 0x0215f180), a copy of a pool point: initialised at run time by the
// file's static initialiser, so it is in .data, zero in the ROM
// (not written yet)

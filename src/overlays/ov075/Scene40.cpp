#include "Scene40.h"
#include "MainProgram.h"
#include "../../scene/MainProgram3d.h"
#include "GameData.h"
#include "SceneManager.h"
#include "SceneMemory.h"
#include "ScreenControl.h"
#include "sound/SoundManager.h"

#include <wchar.h>

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
void func_ov075_02161988(Scene40Character *character);
void func_ov075_02161d94(Scene40Character *character);
void func_ov075_02178364(Scene40CharacterPartA *part);
}

// What this scene keeps in the main program's gSceneScratch (an array of bytes there) for the scenes it goes to
struct Scene40Scratch {
    /* 0x00 */ u8 mUnknown00;
    /* 0x01 */ u8 mUnknown01;
    /* 0x02 */ u8 mUnknown02[2];
    /* 0x04 */ BOOL mMenuAvailable[5]; // The menu's first five buttons, as Scene40MenuBar::Refresh takes them back
    /* 0x18 */ u8 mUnknown18[0x58];
}; // 0x70

extern Scene40Scratch gSceneScratch;

// Overlay 75: scenes 0x40 and 0x6C, the scene of touching a Mii ("Scn/Scene/SceneMiiTouch_LZ.bin"). One source
// file by its one static initialiser. Most of it is not decompiled yet.

// The saved game's flags and counters that the scene changes (defined at their place, 0x02150540)
static void SetGameFlags(u32 flags, BOOL set);
static u32 GetGameFlags(u32 flags);
static void AddTroublesSolved(int count);


// The scenes after which the visit starts with the pages' entry 23 (CameFromSceneOfTables)
static const int sReturnScenesA[3] = {89, 59, 124};
static const int sReturnScenesB[9] = {130, 131, 132, 133, 134, 135, 183, 185, 184};
static const Scene40SceneEntry sReturnScenesC[7] = {{83, 0xA01}, {117, 0xA01}, {122, 0x501}, {137, 0x501}, {138, 0xA01}, {146, 0xA01}, {147, 0xA03}};
// The planes each change of layout fades, by the new layout and the old
static const int sFadePlanes[4][4] = {{0, 16191, 16191, 16191}, {16191, 0, 16128, 16128}, {16191, 16128, 0, 16128}, {16191, 16128, 16128, 0}};
// The scenes after which the visit starts with the pages' entry 24, and what the touch page does for each
static const Scene40PresentEntry sPresentScenes[20] = {
    {24, 2, 0, 9, 0, 0, 1, {0, 0}},
    {28, 2, 0, 166, 0, 0, 1, {0, 0}},
    {43, 2, 1, 14, 1, 0, 1, {0, 0}},
    {50, 4, 0, 218, 0, 0, 1, {0, 0}},
    {54, 1, 1, 24, 1, 0, 1, {0, 0}},
    {55, 3, 0, 169, 0, 0, 1, {0, 0}},
    {58, 1, 0, 35, 0, 0, 1, {0, 0}},
    {61, 2, 1, 104, 0, 0, 1, {0, 0}},
    {76, 2, 0, 167, 0, 0, 1, {0, 0}},
    {77, 2, 0, 151, 0, 0, 1, {0, 0}},
    {80, 1, 0, 0, 0, 1, 1, {0, 0}},
    {82, 1, 0, 164, 0, 0, 1, {0, 0}},
    {118, 1, 0, 125, 0, 0, 1, {0, 0}},
    {123, 3, 0, 220, 0, 0, 1, {0, 0}},
    {136, 1, 0, 219, 0, 0, 1, {0, 0}},
    {139, 1, 0, 189, 0, 0, 1, {0, 0}},
    {141, 1, 0, 153, 0, 0, 1, {0, 0}},
    {143, 5, 1, 0, 0, 1, 1, {0, 0}},
    {144, 5, 1, 0, 0, 1, 1, {0, 0}},
    {145, 2, 0, 0, 0, 1, 1, {0, 0}},
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
    if (gGameData.mUnknownB2[0] < mCharacter.mSatisfyLevel) {
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
    char archiveName[] = "IM?";
    for (int i = 0; i < 3; i++) {
        char name[4];
        name[0] = archiveName[0];
        name[1] = archiveName[1];
        name[2] = '0' + i;
        name[3] = archiveName[3];
        mItemModels[i].SetItem(name, 0);
        ArchiveManager_MountCompressed(&gArchiveManager, mItemModels[i].mArchiveName, mItemModels[i].mArchivePath, 0);
    }
    mMannequin.CharacterModel::SetArchive("3MM", 0, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3MM", mMannequin.mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacter.mArchiveName, mCharacter.mArchivePath, 0);

    BOOL keepMusic = FALSE;
    if (gSceneManager.mPreviousSceneId == 0x38) {
        if (GetSceneScratchByte0() == 0) {
            keepMusic = TRUE;
        }
    } else if (CameFromSceneOfTable(gSceneManager.mPreviousSceneId)) {
        if (gSceneScratch.mUnknown00 == 2 && gSceneScratch.mUnknown01 == 0) {
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
    func_ov075_02174fe0(&mCharacter, mClock.mSeconds);
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

    func_ov075_02161cc0(&mCharacter, &mContext);
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
        switch (GetSceneScratchByte0()) {
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

    func_ov075_021582fc(OV075_PAGE(0x1F44), context, subCharOffset);
    if (end < *subCharOffset) {
        end = *subCharOffset;
    }
    *subCharOffset = start;
    func_ov075_0215af4c(OV075_PAGE(0x214B0), context, subCharOffset);
    if (end < *subCharOffset) {
        end = *subCharOffset;
    }
    *subCharOffset = start;
    func_ov075_02159c1c(OV075_PAGE(0x1AAB8), context, subCharOffset);
    if (end < *subCharOffset) {
        end = *subCharOffset;
    }
    *subCharOffset = start;
    func_ov075_0215f180(OV075_PAGE(0x2A49C), context, subCharOffset);
    func_ov075_0215ff8c(OV075_PAGE(0x2A49C));
    if (end < *subCharOffset) {
        end = *subCharOffset;
    }
    *subCharOffset = start;

    mMenuBar.Init(context);
    mTouchPage.Init(context, &mTouch, &mMenuBar);
    func_ov075_0215350c(OV075_PAGE(0x180), context, mainCharOffset, &mMenuBar, OV075_PAGE(0x2A49C));
    func_ov075_02154aa4(OV075_PAGE(0x1D94), context, &mTouch, OV075_PAGE(0x2A49C), OV075_PAGE(0x386B0));
    func_ov075_02157590(OV075_PAGE(0x1E1C), context, &mTouch);
    func_ov075_0216147c(OV075_PAGE(0x386B0), context, mainCharOffset);
    *(int *)OV075_PAGE(0x20CD0) = end;
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
        case PAGE_1:
            UpdatePage1();
            break;
        case PAGE_2:
            UpdatePage2();
            break;
        case PAGE_3:
            UpdatePage3();
            break;
        case PAGE_LIST_4:
            UpdatePage4();
            break;
        case PAGE_LIST_5:
            UpdatePage5();
            break;
        case PAGE_LIST_6:
            UpdatePage6();
            break;
        case PAGE_LIST_7:
            UpdatePage7();
            break;
        case PAGE_8:
            UpdatePage8();
            break;
        case PAGE_9:
            UpdatePage9();
            break;
        }
    }
    mMenuBar.Update();
}

// The pages take touches only while the scene is not changing the upper screen
void Scene40Pages::SetActive(BOOL active) {
    if (active == mActive) {
        return;
    }
    mActive = active;
    mTouchPage.mActive = active;
    *(BOOL *)OV075_PAGE(0x198) = active;
    *(BOOL *)OV075_PAGE(0x1DAC) = active;
    *(BOOL *)OV075_PAGE(0x1E30) = active;
    *(BOOL *)OV075_PAGE(0x1F54) = active;
    *(BOOL *)OV075_PAGE(0x1AAC4) = active;
    *(BOOL *)OV075_PAGE(0x21504) = active;
}

void Scene40Pages::HideMenu(BOOL hide) {
    if (!hide) {
        return;
    }
    mTouchPage.HideMenu(TRUE);
}

// The page shown: the first four together with the touch page's, and the menu
void Scene40Pages::Draw() {
    func_ov075_021617a0(OV075_PAGE(0x386B0));
    switch (mPage) {
    case PAGE_TOUCH:
    case PAGE_1:
    case PAGE_2:
    case PAGE_3:
        mTouchPage.Draw();
        func_ov075_02154a94(OV075_PAGE(0x180));
        func_ov075_02157004(OV075_PAGE(0x1D94));
        func_ov075_021582c4(OV075_PAGE(0x1E1C));
        func_ov075_0215fd8c(OV075_PAGE(0x2A49C));
        mMenuBar.Draw();
        break;
    case PAGE_LIST_4:
    case PAGE_LIST_5:
    case PAGE_LIST_6:
    case PAGE_LIST_7:
        func_ov075_02159418(&mListPage);
        break;
    case PAGE_8:
        func_ov075_0215aef8(OV075_PAGE(0x1AAB8));
        break;
    case PAGE_9:
        func_ov075_0215bac8(OV075_PAGE(0x214B0));
        break;
    }
}

// --- What the stylus does to the character (the pages' +0x2C) --------------------------------------------------------

void Scene40CharacterTouch::Init(Scene40Touch *tracker, Scene40Character *character) {
    mTracker = tracker;
    mCharacter = character;
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
    if (func_ov075_02173f04(mCharacter)) {
        parts |= PART_BODY;
    }
    if (parts == 0) {
        parts = PART_NONE;
    }
    return parts;
}

// --- The saved game's flags and counters that the scene changes --------------------------------------------------

// Sets or clears bits of gGameData's flags at 0xCC
static void SetGameFlags(u32 flags, BOOL set) {
    if (set) {
        gGameData.mUnknownCC |= flags;
    } else {
        gGameData.mUnknownCC &= ~flags;
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
    mUnknown00 = 0;
    mUnknown08 = 0;
    mUnknown18 = 0;
    mUnknown20 = 0;
    mMenuHidden = FALSE;
    mActive = TRUE;
    mUnknown60 = 0;
    mUnknown64 = 0;
    mUnknown68 = 0;
    mMenu = menu;
    RefreshMenu();
}

// --- The scene's character ------------------------------------------------------------------------------------------

// With speaking set, the lips follow the scene's speech
void Scene40Character::Update(BOOL speaking) {
    Character::Update();
    func_ov075_02161988(this);
    mHeadIcons[0].Update();
    mHeadIcons[1].Update();
    func_ov075_02178364(&mPartA);
    if (!speaking) {
        return;
    }
    func_ov075_02161d94(this);
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
    mPartA.Draw();
    mPartB.Draw();
}

void Scene40CharacterPartA::Draw() {
    mModel.Draw();
}

void Scene40CharacterPartB::Draw() {
    mModel.Draw();
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

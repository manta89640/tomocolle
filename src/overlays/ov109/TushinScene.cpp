#include "TushinScene.h"

#include <string.h>

// Overlay 109: wireless play (TushinScene). One source file.

// --- The file's constants ---

static const u8 sYesNoSequences[2] = { 0x74, 0x74 };
static const u8 sYesNoSequences2[2] = { 0x75, 0x75 };

static const WindowPoint sListScrollDownSize = { 14, 12 };
static const u32 sYesNoMessages[2] = { 2, 3 };
static const WindowPoint sSignalPosition = { 244, 182 }; // Where the signal strength sprite is
static const WindowPoint sListTitlePos = { 119, 47 };
static const WindowPoint sBackButtonSize = { 94, 30 };
static const WindowPoint sYesNoSizes[2] = { { 94, 30 }, { 94, 30 } };
static const ScreenRect sYesNoWindowRect = { 20, 20, 216, 152 };
static const ScreenRect sListBarRect = { -8, -8, 16, 16 };
static const ScreenRect sWindowRect = { 20, 20, 216, 152 };
static const ButtonSounds sButtonStyle = { 10, -1, 0, -1 };
static const WindowPoint sFirstPageIconPositions[2] = { { 204, 74 }, { 204, 106 } };
static const WindowPoint sButtonTextPositions3[3] = { { 38, 66 }, { 38, 90 }, { 38, 114 } };
static const WindowPoint sListButtonPositions[3] = { { 213, 19 }, { 242, 13 }, { 128, 173 } };
static const WindowPoint sListButtonTextPositions[3] = { { 26, 34 }, { 24, 22 }, { 116, 30 } };
static const WindowPoint sButtonPositions2[4] = { { 128, 74 }, { 128, 106 }, { 128, 124 }, { 128, 149 } };
static const WindowPoint sButtonPositions3[4] = { { 128, 72 }, { 128, 96 }, { 128, 120 }, { 128, 149 } };
static const ButtonSounds sYesNoStyles[2] = { { 10, -1, 0, -1 }, { 12, -1, 0, -1 } };

static const WindowPoint sButtonSize2 = { 190, 26 };
static const WindowPoint sListCursorPos = { 12, 47 };
static const WindowPoint sListTextPosition = { 8, 2 };
static const WindowPoint sTextPosition = { 24, 12 };
static const WindowPoint sListTitleSize = { 228, 18 };
static const WindowPoint sListScrollUpPos = { 244, 150 };
static const WindowPoint sListScrollDownPos = { 244, 44 };
static const WindowPoint sButtonSize3 = { 190, 20 };
static const WindowPoint sListBarPos = { 244, 97 };
static const WindowPoint sListTitleTextPos = { 19, 41 };
static const WindowPoint sListScrollBarSize = { 14, 12 };
static const VecFx32 sCameraTarget = { 0, 0x1CCC, 0 };
static const VecFx32 sEmitterPosition0 = { 0x1000, 0, 0 };
static const VecFx32 sEmitterPosition1 = { -0x1000, 0, 0 };
static const u32 sListButtonMessages[3] = { (u32)-1, (u32)-1, 0x105 };
static const int sListButtonFonts[3] = { 0, -1, 2 };
static const int sListButtonFiles[3] = { 1, 1, 0 };
static const VecFx32 sCameraPosition = { 0, 0x24CC, 0xC000 };
static const ScreenRect sListWindowRect = { 20, 12, 216, 168 };
static const WindowPoint sYesNoPositions[2] = { { 75, 149 }, { 181, 149 } };
static const WindowPoint sButtonTextPositions2[3] = { { 38, 68 }, { 38, 100 }, { 38, 118 } };
static const WindowPoint sTextMargins[3] = { { 32, 33 }, { 32, 50 }, { 32, 33 } };
static const int sListOrders[7] = { 0, 1, 2, 3, 4, 5, 6 }; // What the list is sorted by, by the button chosen
static const u32 sListMessages[7] = { 127, 128, 129, 130, 131, 132, 133 };
static const ButtonSounds sListButtonStyles[3] = { { 10, -1, 0, -1 }, { 12, -1, 0, -1 }, { 10, -1, 0, -1 } };
static const WindowPoint sListItemPositions[7] = {
    { 128, 30 }, { 128, 52 }, { 128, 74 }, { 128, 96 }, { 128, 118 }, { 128, 140 }, { 128, 162 },
};
static const WindowPoint sListItemSizes[7] = {
    { 128, 20 }, { 128, 20 }, { 128, 20 }, { 128, 20 }, { 128, 20 }, { 128, 20 }, { 128, 20 },
};

static const u8 sListButtonSequences2[3] = { 0x7C, 0x01, 0x77 };
static const u8 sListButtonSequences[3] = { 0x7B, 0x00, 0x76 };
// How strong the wireless signal is: the sequence of the signal sprite by link level
static const u8 sLinkLevelSequences[4] = { 0, 1, 2, 3 };
static const char sListWindowScreenName[] = "MLT:/Window_down.nsc";
static const char sWindowScreenName[] = "MLT:/Window_common.nsc";

static const WindowLayout sLayouts[9] = {
    { { TRUE, TRUE, FALSE }, TRUE },
    { { TRUE, TRUE, FALSE }, TRUE },
    { { TRUE, TRUE, FALSE }, TRUE },
    { { FALSE, FALSE, FALSE }, TRUE },
    { { TRUE, TRUE, TRUE }, TRUE },
    { { FALSE, FALSE, FALSE }, TRUE },
    { { FALSE, FALSE, FALSE }, FALSE },
    { { FALSE, FALSE, FALSE }, TRUE },
    { { FALSE, FALSE, FALSE }, TRUE },
};
static const ButtonSounds sBackButtonStyles[9] = {
    { 12, -1, 0, -1 }, { 12, -1, 0, -1 }, { 12, -1, 0, -1 }, { 12, -1, 0, -1 }, { 12, -1, 0, -1 },
    { 10, -1, 0, -1 }, { 10, -1, 0, -1 }, { 10, -1, 0, -1 }, { 11, -1, 0, -1 },
};
static const u32 sLayoutMessages[9][4] = {
    { 0xF8, 0xF9, 0xF9, 9 }, { 0xFB, 0xFC, 0xF9, 9 }, { 0xFE, 0xFF, 0xF9, 9 }, { 0xFE, 0xFF, 0xF9, 5 },
    { 0x102, 0x102, 0x102, 5 }, { 0xFE, 0xFF, 0xF9, 4 }, { 0xFE, 0xFF, 0xF9, 4 }, { 0xFE, 0xFF, 0xF9, 4 },
    { 0xFE, 0xFF, 0xF9, 4 },
};
static const u32 sPageMessages[68] = {
    0xF7,  0xFA,  0xFD,  0x100, 0x101, 0x103, 0x113, 0x104, 0x104, 0x108, 0x108, 0x106, 0x114, 0x108,
    0x10B, 0x10C, 0x10D, 0x10A, 0x118, 0x11C, 0x11B, 0x120, 0x123, 0x10E, 0x116, 0x115, 0x119, 0x11F,
    0x11A, 0x11A, 0x122, 0x117, 0x124, 0x112, 0x111, 0x11E, 0x11D, 0x112, 0x121, 0x103, 0x113, 0x104,
    0x108, 0x108, 0x106, 0x114, 0x108, 0x109, 0x10D, 0x10A, 0x118, 0x11C, 0x11B, 0x10F, 0x116, 0x115,
    0x119, 0x11F, 0x11A, 0x11A, 0x122, 0x125, 0x112, 0x112, 0x111, 0x11E, 0x11D, 0x112,
};
// The archive names of the three characters
static const char *sCharacterArchives[3] = { "3HM", "BI1", "SRM" };
// The words that a received character's texts must not be (IsNgWord compares each text with them, without its
// trailing spaces and exclamation marks)
static const wchar_t *sNgWords[41] = {
    L"シネ", L"アイエキ", L"セイエキ", L"adhd", L"fuck", L"イヌゴロシ", L"インバイ", L"インモウ", L"ウンコ", L"ウンチ", L"オメコ", L"カタワ",
    L"キチガイ", L"クロンボ", L"コロス", L"ゴウカン", L"サンゴクジン", L"シナジン", L"セックス", L"チャンコロ", L"チンコ", L"チンチン", L"チンポ", L"チンバ",
    L"ツンボ", L"ドカタ", L"ドモリ", L"ニグロ", L"ニンピニン", L"ビッコ", L"フェラ", L"ブラク", L"ペニス", L"マンコ", L"オマンコ", L"メクラ",
    L"ヤラセロ", L"ヤリマン", L"リョウジョク", L"レイプ", L"ロリコン",
};

// Where the list window's texts are: on its items, 32 pixels left of and 6 above each (set by the static
// initialiser)
static const WindowPoint sListTextOrigins[7] = {
    { sListItemPositions[0].x - 32, sListItemPositions[0].y - 6 },
    { sListItemPositions[1].x - 32, sListItemPositions[1].y - 6 },
    { sListItemPositions[2].x - 32, sListItemPositions[2].y - 6 },
    { sListItemPositions[3].x - 32, sListItemPositions[3].y - 6 },
    { sListItemPositions[4].x - 32, sListItemPositions[4].y - 6 },
    { sListItemPositions[5].x - 32, sListItemPositions[5].y - 6 },
    { sListItemPositions[6].x - 32, sListItemPositions[6].y - 6 },
};

static const int sPageLayouts[68] = {
    0, 1, 2, 3, 3, 4, 6, 7, 7, -1, -1, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    7, 5, 6, 7, 3, 4, 6, 7, -1, -1, 5, 5, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 5, 6, 7,
};

// The Mii data of the residents, as they are sent (all 100 saved characters, empty ones cleared)
static MiiData sMiiData[100];
// The character sent or received
static SavedCharacter sTransfer;

// The state of the wireless exchange: separate statics, which the compiler pools by size (a u8, the u16s, the
// ints, then sParentTimers and sReceivedName). Those still named by address are used by nothing in the ROM but
// TushinScene_UnusedClear (the 13 u16s) or only cleared (data_ov109_0215320c, data_ov109_02153230).
static u8 sDisconnected;
static u16 data_ov109_021531ee;
static u16 data_ov109_021531fe;
static u16 data_ov109_021531ea;
static u16 sSendSongChunk;
static u16 sCommand; // The 2-byte answer sent to the partner (Send)
static u16 data_ov109_021531ec;
static u16 sReceivedSongChunks;
static u16 sSendChunk;
static u16 data_ov109_021531fc;
static u16 sReceiveCount;
static u16 data_ov109_021531f2;
static u16 sNextReceivedCommand;
static u16 data_ov109_021531f8;
static u16 data_ov109_021531fa;
static u16 data_ov109_021531e6;
static u16 data_ov109_021531f6;
static u16 data_ov109_021531e8;
static u16 data_ov109_021531f0;
static u16 sReceivedCommand;
static u16 data_ov109_021531f4;
static int sWiiParentCount;
static int data_ov109_02153230;
static int sReceivedResident;
static int data_ov109_0215320c;
static int sSent;
static int sWiiSendDone;
static int sWiiReceiveResult;
static int sWiiReceivedIndex;
static int sWiiSendResult;
static int sSongsReceived;
static int sParentTimers[3]; // Frames for which each found partner is still shown (600 when it is new)
static wchar_t sReceivedName[21];
static u8 sSongs[6][0x40]; // The songs sent with a character
static WMBssDesc sParents[3]; // The partners found

static inline void ClearText(MessageText *text) {
    NNS_G2dCharCanvasClear(&text->mCanvas, 0);
}

static inline void PlayResultSound(int index) {
    gSoundManager.StartSeqArc(1, 0x19, index);
}

static inline BOOL IsWindowOpen(MessageWindow *window) {
    return (window->mUnknownAD50 & 1) ? TRUE : FALSE;
}

static inline void PlaySound(int index) {
    gSoundManager.StartSeqArc(1, 0x19, index);
}

// @0x02149a40
void TushinScene::StartLoading() {
    gSoundManager.StartSeqIfNotPlaying(0, 0x61);
    ArchiveManager_MountCompressed(&gArchiveManager, "STS", "Scn/Scene/SceneTushin_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "MLT", "Scn/List/List_TushinList_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TBM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TBC", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "BIC", "Mdl/Ica/ica_mii_contents_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "STE", "Mdl/Spl/SplTushinEffect_LZ.bin", 0);
}

// @0x02149b08
void TushinScene::SetUpYesNoWindow(int *charOffset, MessageFile *file, u32 message) {
    ScreenRect rect = sYesNoWindowRect;
    func_02032fb8(&mWindow, rect);
    func_02033174(&mWindow, sWindowScreenName, 0x400);
    func_020331bc(&mWindow, 0x3F00, rect);
    for (int i = 0; i <= 1; i++) {
        func_02033844(&mWindow, i, 0, sYesNoSequences[i], sYesNoSequences2[i], sYesNoPositions[i], sYesNoSizes[i],
                      &mMessageFiles[1], sYesNoMessages[i], &gFonts[2].mFont, 0x1000, charOffset, &sYesNoStyles[i],
                      1);
    }
    func_020338d0(&mWindow, file, message, &gFonts[0].mFont, 0x1000, sTextPosition, sTextMargins[2],
                  charOffset);
    mWindow.mTexts[2].mHAlign = mWindow.mTexts[2].mMessageFile->GetAlignment(mWindow.mTexts[2].mMessage);
    mWindow.mTexts[2].mVAlign = 0;
}

// @0x02149cb4
void TushinScene::SetUpWindow(int page) {
    int charOffset = mCharOffset;
    int layout = sPageLayouts[page];
    if (layout == 5) {
        SetUpYesNoWindow(&charOffset, &mMessageFiles[0], sPageMessages[page]);
        return;
    }

    ScreenRect rect = sWindowRect;
    func_02032fb8(&mWindow, rect);
    func_02033174(&mWindow, sWindowScreenName, 0x400);
    func_020331bc(&mWindow, 0x3F00, rect);

    WindowPoint margin;
    if (sLayouts[layout].mHasButton[0]) {
        margin.x = 32;
        margin.y = 33;
    } else if (sLayouts[layout].mHasBackButton) {
        margin.x = 32;
        margin.y = 33;
    } else {
        margin.x = 32;
        margin.y = 50;
    }
    func_020338d0(&mWindow, &mMessageFiles[0], sPageMessages[page], &gFonts[0].mFont, 0x1000, sTextPosition, margin,
                  &charOffset);
    mWindow.mTexts[0].mHAlign = mWindow.mTexts[0].mMessageFile->GetAlignment(mWindow.mTexts[0].mMessage);
    if (sLayouts[layout].mHasButton[0] == FALSE) {
        mWindow.mTexts[0].mVAlign = 1;
    } else {
        mWindow.mTexts[0].mVAlign = 0;
    }

    if (sLayouts[layout].mHasButton[2]) {
        for (int i = 0; i < 3; i++) {
            if (sLayouts[layout].mHasButton[i]) {
                func_02033448(&mWindow, i, 0, 0x6E, 0x6F, sButtonPositions3[i], sButtonSize3, &sButtonStyle, 1);
                func_02033a58(&mWindow, &mMessageFiles[0], sLayoutMessages[layout][i], &gFonts[0].mFont, 0x1000,
                              sButtonTextPositions3[i], &charOffset);
            }
        }
    } else {
        for (int i = 0; i < 3; i++) {
            if (sLayouts[layout].mHasButton[i]) {
                func_02033448(&mWindow, i, 0, 0x6C, 0x6D, sButtonPositions2[i], sButtonSize2, &sButtonStyle, 1);
                func_02033a58(&mWindow, &mMessageFiles[0], sLayoutMessages[layout][i], &gFonts[0].mFont, 0x1000,
                              sButtonTextPositions2[i], &charOffset);
            }
        }
    }

    if (sLayouts[layout].mHasBackButton) {
        func_02033844(&mWindow, 3, 0, 0x74, 0x75, sButtonPositions2[3], sBackButtonSize, &mMessageFiles[1],
                      sLayoutMessages[layout][3], &gFonts[2].mFont, 0x1000, &charOffset, &sBackButtonStyles[layout],
                      1);
    }

    if (page != 0) {
        return;
    }
    for (int i = 0; i < 2; i++) {
        func_02033bb4(&mWindow, 0, 0x7E, sFirstPageIconPositions[i]);
    }
    for (int i = 0; i < 3; i++) {
        mWindow.mSprites[i].mUnknown5E = 8;
    }
    mWindow.mButtons[0].mUnknown5E = 8;
}

// @0x0214a140
void TushinScene::SetUpList(int *charOffset) {
    func_020a1fe0(&mList, 6);
    func_020a2104(&mList, 0x100, 0x26);
    func_020278f8(&data_021213b8, 0x5FDF, 0x100);
    func_020a2118(&mList, 0, 0x68, 0x69, sListTitlePos, sListTitleSize);
    func_020a23cc(&mList, 0, 0x7A, sListCursorPos);
    func_020a2280(&mList, 0x1000, sListTitleTextPos, charOffset);
    func_020a258c(&mList, 0, 2, 3, sListScrollDownPos, sListScrollBarSize);
    func_020a2644(&mList, 0, 4, 5, sListScrollUpPos, sListScrollDownSize);
    func_020a26fc(&mList, 0, 6, 7, 0x61, 0x4A, 1, sListBarPos, sListBarRect);
    func_020a27dc(&mList, 3, 6, 7);
    func_020a380c(&mPages, 0, 8);
    func_020a03e8(&mResidents, &mList, &mPages);
    func_020a05dc(&mResidents);
    mUnknown35924 = mList.mUnknown9464;
    mSortKey = gGameData.mMiiListSortKey;
    func_020a123c(&mResidents, mSortKey, 1);
    mResidents.mUnknown1139C = 1;

    for (int i = 0; i < 3; i++) {
        Sprite_Init(&mButtons[i], 0, sListButtonSequences[i], 0x80, 0x60, TRUE);
        func_02031260(&mButtons[i], 4);
        func_02031278(&mButtons[i], sListButtonSequences[i], sListButtonSequences2[i]);
        mButtons[i].SetPosition(
            FxVec2(sListButtonPositions[i].x << FX32_SHIFT, sListButtonPositions[i].y << FX32_SHIFT));
        func_02031eb0(&mButtons[i], sListButtonTextPositions[i].x, sListButtonTextPositions[i].y);
        mButtons[i].mUnknownDC = 1;
        func_02032158(&mButtons[i], &sListButtonStyles[i]);
        func_0203119c(&mButtons[i]);
        mButtons[i].mUnknown5E = 1;
        if (sListButtonMessages[i] == (u32)-1) {
            mButtons[i].mText.mSprite.mVisible = FALSE;
        } else {
            *charOffset += func_02031f58(&mButtons[i], &mMessageFiles[sListButtonFiles[i]], sListButtonMessages[i],
                                         &gFonts[sListButtonFonts[i]].mFont, 0x1000, *charOffset);
        }
    }
    func_02031e88(&mButtons[2], 0);

    ScreenRect rect = sListWindowRect;
    func_02032fb8(&mListWindow, rect);
    func_02033174(&mListWindow, sListWindowScreenName, 0x400);
    func_020331bc(&mListWindow, 0x3F00, rect);
    for (int i = 0; i < 7; i++) {
        func_02033448(&mListWindow, i, 0, 0x6A, 0x6B, sListItemPositions[i], sListItemSizes[i], &sButtonStyle, 1);
    }
    for (int i = 0; i < 7; i++) {
        func_020338d0(&mListWindow, &mMessageFiles[0], sListMessages[i], &gFonts[0].mFont, 0x1000, sListTextPosition,
                      sListTextOrigins[i], charOffset);
    }
    func_02034f4c(&mListWindow, 1);
}

// @0x0214a68c
void TushinScene::InitCharacters() {
    int i;
    func_0202b0e8(&data_02122824);
    for (i = 2; i >= 0; i--) {
        SavedCharacter *resident = &mResidents.mResidents[mResidents.mSelected];
        CharacterRecord *record = &mCharacters[i];
        *record = *resident;
        func_02091048(&mCharacters[i], sCharacterArchives[i]);
        CharacterModel *model = &mCharacters[i].mModel;
        ArchiveManager_MountCompressed(&gArchiveManager, model->mArchiveName, model->mArchivePath, 1);
        func_0202ad04(data_021223e4, i, model->mArchiveName, 0);
    }
    void *animations = func_0202ad04(data_021223e4, 0x1F, "BIC", 0);
    G3X_SetClearColor(GX_RGB(21, 21, 21), 0, 0x7FFF, 63, FALSE);
    for (i = 0; i < 3; i++) {
        func_02090fdc(&mCharacters[i], i, 0, 0);
        func_02029660(mCharacters[i].mModel.mResourceSet, RESOURCE_BCA0, animations);
        Character_SetAnimation(&mCharacters[i], RESOURCE_BCA0, GetStandAnimation(i));
        mCharacters[i].mModel.mPos.y = 0;
        mCharacters[i].mModel.mPos.x = -0x1000;
        mCharacters[i].mUnknown02C8 = 10;
        NNS_G3dMdlSetMdlPolygonIDAll(mCharacters[i].mModel.mResMdl, 3);
    }
    func_02091a14(&mCharacters[0], TRUE);
    mCurrent = 1;
    func_02091a14(&mCharacters[1], FALSE);
    func_02091a14(&mCharacters[2], FALSE);
    mCharactersVisible = 0;
    mCamera = &data_02122764;
    func_0202a640(mCamera);
    VEC_Set(&mCamera->mPosition, sCameraPosition.x, sCameraPosition.y, sCameraPosition.z);
    VEC_Set(&mCamera->mTarget, sCameraTarget.x, sCameraTarget.y, sCameraTarget.z);
    func_0202a96c(&data_021227cc[0]);
    func_0202a96c(data_021227d8);
    func_0202ab2c(&data_021227cc[0], mCamera->mAngleX, mCamera->mAngleY, mCamera->mAngleZ, 0x640, 0x3E8 - 0x640,
                  -0xFA0);
    func_0202ab2c(data_021227d8, mCamera->mAngleX, mCamera->mAngleY, mCamera->mAngleZ, 0, -0xFA0, 0x5DC);
    LoadCharacters();
}

// NOT MATCHING: the two `|= 2` of the emitters' flags use r2/r3 where the original uses r0/r2 and sets
// mParticleFrames's 0 after them (register choice, 9 instructions).
// @0x0214ad80
void TushinScene::InitParticles() {
    func_0202680c(0x4000);
    mParticles = func_0207ab20(func_02026824, 2, 100, 0, 1, 30);
    mParticleFile = ArchiveManager_GetFileByName(&gArchiveManager, "STE:/SplTushinEffect.spa");
    func_0207ac78(mParticles, mParticleFile);
    func_0207b110(mParticles);
    func_0207b124(mParticles);
    mParticleResource = 0;
    mEmitterStarted = 1;
    VecFx32 position0 = sEmitterPosition0;
    mEmitterPositions[0].x = position0.x;
    mEmitterPositions[0].y = position0.y;
    mEmitterPositions[0].z = position0.z;
    VecFx32 position1 = sEmitterPosition1;
    mEmitterPositions[1].x = position1.x;
    mEmitterPositions[1].y = position1.y;
    mEmitterPositions[1].z = position1.z;
    mEmitters[0] = func_0207b2fc(mParticles, mParticleResource, &mEmitterPositions[0]);
    mEmitters[1] = func_0207b2fc(mParticles, mParticleResource, &mEmitterPositions[1]);
    mEmitters[0]->mFlags |= 2;
    mEmitters[1]->mFlags |= 2;
    mParticleFrames = 0;
}

// @0x0214aed8
void TushinScene::UpdateMiiData() {
    for (int i = 0; i < 100; i++) {
        if (func_0202c354(&gGameData, i)) {
            CharacterRecord record;
            func_0202c264(&gGameData, i, &record);
            MI_CpuCopy8(&record, &sMiiData[i], sizeof(sMiiData[i]));
        } else {
            MI_CpuFill8(&sMiiData[i], 0, sizeof(sMiiData[i]));
            NFL_ClearMiiCreateId(&sMiiData[i]);
        }
    }
}

// @0x0214af6c
void TushinScene::Init() {
    mMessageFiles[0].Open("TBM:tmd_etc_msg_JP.bmg");
    mMessageFiles[1].Open("TBC:tmd_common_msg_JP.bmg");
    func_02028f24(&gVramTransferQueue, "MLT:/BG_list.ncg", 0x700, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "MLT:/BG_list.ncl", 0x700, -1, -1, -1);
    func_02028f24(&gVramTransferQueue, "STS:/BG_up.ncg", 2, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "STS:/BG_up.ncl", 2, -1, -1, -1);
    func_02027d7c(&data_02121834, "MLT:/BG_list.nsc", 0x100, 0, 0, 0, 0, -1);
    func_02028280(&data_02121834, 0x100, 3);
    func_02027d7c(&data_02121834, "MLT:/BG_Tushin_frame.nsc", 0x200, 0, 0, 0, 0, -1);
    func_02028280(&data_02121834, 0x200, 2);
    func_02027d7c(&data_02121834, "STS:/BG_up.nsc", 2, 0, 0, 0, 0, -1);
    func_02028280(&data_02121834, 2, 3);
    ObjResourceManager_Load(&gObjResourceManager, "MLT:/Obj_Mii_list.nce", "MLT:/Obj_list.ncg", 0x1000, 0, -1);
    mPalette = func_02027534(&data_021213b8, "MLT:/Obj_list.ncl", 0x1000, -1, -1, -1);
    ObjResourceManager_Load(&gObjResourceManager, "STS:/Obj_up.nce", "STS:/Obj_up.ncg", 0x10, 1, -1);
    func_02027534(&data_021213b8, "STS:/Obj_up.ncl", 0x10, -1, -1, -1);
    data_0211d604.mPlanes = 0x1713;
    data_0211d604.mPlanesChanged = TRUE;
    func_02023ee0(&data_0211d604, 1, 0x12, 8, 8);

    mPage = 0;
    mNextPage = 0;
    mPageAfterSave = 0;
    mBgScroll = 0;
    mWait = 0;
    mCloseListToReceive = 0;
    mLostInList = 0;
    int charOffset = 0x200;
    SetUpList(&charOffset);
    mCharOffset = charOffset;
    SetUpWindow(mPage);
    func_02034f38(&mWindow, 0, sPageMessages[mPage]);
    func_02034720(&mWindow, 0x12, 1);
    InitCharacters();
    InitParticles();

    Sprite_Init(&mSprite, 1, 0, 0x80, 0x60, TRUE);
    mSprite.SetPosition(FxVec2(sSignalPosition.x << FX32_SHIFT, sSignalPosition.y << FX32_SHIFT));
    mSprite.mVisible = FALSE;

    sSendChunk = 0;
    sSendSongChunk = 0;
    sReceiveCount = 0;
    sReceivedSongChunks = 0;
    sReceivedResident = 0;
    sDisconnected = 0;
    sReceivedCommand = 0;
    sNextReceivedCommand = 0;
    sWiiSendDone = 0;
    sWiiSendResult = 0;
    sWiiReceiveResult = 0;
    MI_CpuFill8(sReceivedName, 0, sizeof(sReceivedName));
    gWirelessSession->SetParentBuffer(sParents, 3);
    gWirelessSession->SetTimers(sParentTimers);
    gWirelessSession->Reset(0);
    MI_CpuFill8(sParents, 0, sizeof(sParents));
    MI_CpuFill8(sParentTimers, 0, sizeof(sParentTimers));
    mMusicPlaying = 0;
    mSongStoreFull = 0;
    mShirtDue = 0;
    mWiiShirtDue = 0;
    mReceiveResult = 0;
    mSendQueued = 0;
    mPartnerReady = 0;
    mPartnerSending = 0;
    mUnknown3F090 = 0;
    mIsReceiveAnimation = 0;
    mNeedsSave = 0;
    mWindowReady = 0;
    mConnectRequested = 0;
    data_ov109_02153230 = 0;
    MiiTransfer_Init(&data_0212349c, sMiiData, 100);
    MiiTransfer_SetSendResultCallback(TushinScene_OnWiiSendResult);
    MiiTransfer_SetReceiveCallback(TushinScene_OnWiiMiiReceived);
    MiiTransfer_SetAcceptSpecial(1);
    UpdateMiiData();
    gSoundManager.LoadGroup(0x5D);
    ResetExchange();
}

// @0x0214b3d8
void TushinScene::ResetExchange() {
    mPartnerSending = 0;
    sSendChunk = 0;
    sSendSongChunk = 0;
    sReceiveCount = 0;
    sReceivedSongChunks = 0;
    sReceivedCommand = 0;
    sNextReceivedCommand = 0;
    UpdateMiiData();
    MI_CpuFill8(sReceivedName, 0, sizeof(sReceivedName));
    ClearParentNames();
    mConnectRequested = 0;
    for (int i = 0; i < 12; i++) {
        gWHChildSsid[i] = 0;
    }
    mSendQueued = 0;
}

// @0x0214b464
void TushinScene::SetUpCharacter(int i) {
    func_0208fefc(&mCharacters[i]);
    func_02091a88(&mCharacters[i]);
    func_02091a24(&mCharacters[i]);
    mCharacters[i].mPersonality = func_02098f2c(&mCharacters[i]);
    func_0209decc(&mCharacters[i]);
    Character_SetAnimation(&mCharacters[i], RESOURCE_BCA0, GetStandAnimation(i));
    func_020919a0(&mCharacters[i], 0x1000);
}

// @0x0214b4e8
void TushinScene::ReloadCharacter(int i) {
    func_0202aecc(data_021223e4, i);
    CharacterModel *model = &mCharacters[i].mModel;
    ArchiveManager_Unmount(&gArchiveManager, model->mArchiveName);
    func_02091048(&mCharacters[i], sCharacterArchives[i]);
    ArchiveManager_MountCompressed(&gArchiveManager, model->mArchiveName, model->mArchivePath, 1);
    func_0202ad04(data_021223e4, i, model->mArchiveName, 0);
    func_0208ef5c(&mCharacters[i].mModel, i, 0);
}

// @0x0214b590
void TushinScene::LoadCharacters() {
    for (int i = 0; i < 3; i++) {
        LoadCharacter(i);
    }
}

// @0x0214b5b8
void TushinScene::ShowNextCharacter() {
    (CharacterRecord &)mCharacters[mCurrent] = mResidents.mResidents[mResidents.mSelected];
    ReloadCharacter(mCurrent);
    SetUpCharacter(mCurrent);
    func_02091a14(&mCharacters[mCurrent], TRUE);
    func_02091a14(&mCharacters[mCurrent ^ 1], FALSE);
    mCurrent ^= 1;
    ReloadCharacterFromResident(2);
}

// @0x0214ba98
void TushinScene::LoadSavedCharacter(int i, int index) {
    if (index < 0 || index >= 100) {
        return;
    }
    func_0202c264(&gGameData, index, &mCharacters[i]);
    ReloadCharacter(i);
    SetUpCharacter(i);
}

// @0x0214baf4
void TushinScene::LoadCharacter(int i) {
    SavedCharacter *resident = &mResidents.mResidents[mResidents.mSelected];
    CharacterRecord *record = &mCharacters[i];
    *record = *resident;
    SetUpCharacter(i);
}

// @0x0214bf5c
void TushinScene::ReloadCharacterFromResident(int i) {
    SavedCharacter *resident = &mResidents.mResidents[mResidents.mSelected];
    CharacterRecord *record = &mCharacters[i];
    *record = *resident;
    ReloadCharacter(i);
    SetUpCharacter(i);
}

// @0x0214c3d0
void TushinScene::ReloadCharacterFromReceived(int i) {
    CharacterRecord *record = &mCharacters[i];
    *record = sTransfer;
    ReloadCharacter(i);
    SetUpCharacter(i);
}

#pragma optimization_level 2
// @0x0214c84c
void TushinScene::SendSongs() {
    if (sSendSongChunk != 0) {
        return;
    }
    sSendSongChunk = 1;
    int offset = mResidents.mSelected * sizeof(SavedCharacter);
    u8 *residents = (u8 *)mResidents.mResidents;
    int n = 0;
    MI_CpuClear16(sSongs, sizeof(sSongs));
    for (int i = 0; i < 6; i++) {
        int song = func_0209c718((SavedCharacter *)(residents + offset), i);
        if (song != 0xFF) {
            func_0202c920(&gGameData, song, sSongs[n++]);
        }
    }
    WH_SendData(sSongs, 0x80, TushinScene_SendCallback);
    sSent = 0;
}
#pragma optimization_level reset

// @0x0214c908
void TushinScene::Send(int kind) {
    if (GetMode() == 2) {
        SendFoundIndex();
        return;
    }
    int state = WH_GetSystemState();
    if (!(state != 0 && state != 9 && state != 10)) {
        sSendChunk = 0;
        sSendSongChunk = 0;
        sSent = 0;
        return;
    }
    sCommand = kind;
    switch (kind) {
    case 0:
        if (sSendChunk != 0) {
            return;
        }
        sSendChunk = 1;
        func_020979e0(&sTransfer);
        (CharacterRecord &)sTransfer = mResidents.mResidents[mResidents.mSelected];
        WH_SendData(&sTransfer, 0x80, TushinScene_SendCallback);
        sSent = 0;
        return;
    case 1:
        SendSongs();
        return;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
    default:
        WH_SendData(&sCommand, 2, TushinScene_SendCallback);
        return;
    }
}

#pragma optimization_level 2
// @0x0214ce60
void TushinScene::UpdateSending() {
    if (sSent != 1) {
        return;
    }
    int state = WH_GetSystemState();
    if (!(state != 0 && state != 9 && state != 10)) {
        sSendChunk = 0;
        sSendSongChunk = 0;
        sSent = 0;
        mNextPage = 0x22;
        return;
    }
    if (sSendSongChunk != 0) {
        if (sSendSongChunk >= 3) {
            int result = NFL_IsMiiCreateIdBit7Clear(GetSelectedResident());
            switch (sReceivedCommand) {
            case 5:
                sSendSongChunk = 0;
                if (NFL_IsMiiCreateIdBit7Clear(GetSelectedResident())) {
                    func_020a0664(&mResidents);
                }
                break;
            case 6:
            case 9:
            case 10:
                sSendSongChunk = 0;
                break;
            case 7:
            case 8: {
                sSendSongChunk = 0;
                func_0202c328(&gGameData, GetSelectedResident()->GetIndex(), 1);
                SetOtherStandAnimation();
                break;
            }
            default:
                return;
            }
            if (result == 1) {
                SetNeedsSave();
            }
        } else {
            WH_SendData((u8 *)sSongs + sSendSongChunk * 0x80, 0x80, TushinScene_SendCallback);
            sSendSongChunk++;
            sSent = 0;
        }
    } else if (sSendChunk != 0) {
        if (sSendChunk >= 6) {
            sSendChunk = 0;
            Send(1);
        } else {
            WH_SendData((u8 *)&sTransfer + sSendChunk * 0x80, 0x80, TushinScene_SendCallback);
            sSendChunk++;
            sSent = 0;
        }
    }
}

#pragma optimization_level reset
// @0x0214d064
void TushinScene::SendFoundIndex() {
    sWiiSendResult = 0;
    u16 index = FindResident(GetSelectedResident());
    if (index == -1) {
        return;
    }
    MiiTransfer_SendMii(index);
}

// @0x0214d0b0
void TushinScene::UpdateSaving() {
    if (mSendQueued == 0) {
        return;
    }
    if (GetMode() == 1) {
        if (mPartnerReady == 0) {
            return;
        }
    } else if (func_0202cd38(&gGameData)) {
        return;
    }
    mSendQueued = 0;
    mPartnerReady = 0;
    Send(0);
}

// @0x0214d118
int TushinScene::FindResident(CharacterRecord *record) {
    for (int i = 0; i < 100; i++) {
        if (func_0202c354(&gGameData, i)) {
            CharacterRecord resident;
            func_0202c264(&gGameData, i, &resident);
            if (NFL_AreMiiCreateIdsEqual(&resident, record) == 1) {
                return i;
            }
        }
    }
    return -1;
}

// @0x0214d18c
void TushinScene::RefreshRecord(CharacterRecord *record) {
    SavedCharacter character;
    (CharacterRecord &)character = *record;
    func_02097cc0(&character);
    func_0209e01c(&character);
    *record = character;
}

#pragma optimization_level 2
// NOT MATCHING (optimization level 2, as the original's loops are): one instruction, the `orr` of 0x299, whose
// operands are the other way round (`orr r0, r0, r1`: the record's byte first, the received one loaded first);
// `record | received`, `|=`, a local, casts and a pointer all load the record's byte first.
// @0x0214d978
int TushinScene::ReceiveCharacter() {
    if (IsInvalid(&sTransfer)) {
        return 7;
    }
    if (HasNgWord(&sTransfer) == TRUE) {
        return 7;
    }
    if (HasNgSongReceived() == TRUE) {
        return 7;
    }
    sReceivedResident = FindResident(&sTransfer);
    if (sReceivedResident != -1) {
        if (NFL_IsMiiCreateIdBit7Clear(&sTransfer)) {
            return 5;
        }
        if (func_02098584(&sTransfer)) {
            return 3;
        }
        if (MiiData_GetSex(&sTransfer) != func_0202c604(&gGameData, sReceivedResident)) {
            return 6;
        }
        CharacterRecord record;
        func_0202c264(&gGameData, sReceivedResident, &record);
        record.mMii = sTransfer.mMii;
        if (sTransfer.mUnknown050Bit0 == 0) {
            if (record.mUnknown050Bit0 == 1) {
                RefreshRecord(&record);
            }
            record.mUnknown050Bit0 = 0;
            for (int i = 0; i < 5; i++) {
                record.mUnknown04A[i] = sTransfer.mUnknown04A[i];
            }
            u32 bits = sTransfer.mUnknown050Bits29;
            if (bits != 4) {
                record.mUnknown050Bits29 = bits;
            }
            bits = sTransfer.mUnknown054Bits0;
            record.mUnknown054Bits0 = bits;
            bits = sTransfer.mUnknown054Bits21;
            if (bits != 0) {
                record.mUnknown054Bits21 = bits;
            }
            bits = sTransfer.mUnknown054Bits25;
            if (bits != 0) {
                record.mUnknown054Bits25 = bits;
            }
            if (sTransfer.mName != NULL) {
                MI_CpuCopy8(sTransfer.mName, record.mName, sizeof(record.mName));
            }
            if (sTransfer.mUnknown06E != NULL) {
                MI_CpuCopy8(sTransfer.mUnknown06E, record.mUnknown06E, sizeof(record.mUnknown06E));
            }
            MI_CpuCopy8(sTransfer.mSpeechDefault, record.mSpeechDefault, sizeof(record.mSpeechDefault));
            for (int i = 0; i < 4; i++) {
                MI_CpuCopy8(sTransfer.mSpeechByMood[i], record.mSpeechByMood[i], sizeof(record.mSpeechByMood[i]));
            }
            for (int i = 0; i < 6; i++) {
                record.mVoice[i] = sTransfer.mVoice[i];
            }
            record.mUnknown299 = sTransfer.mUnknown299 | record.mUnknown299;
            int n = 0;
            for (int i = 0; i < 6; i++) {
                if (func_0209c718(&sTransfer, i) == 0xFF) {
                    continue;
                }
                u8 song = record.mSongSlots[i];
                if (song == 0xFF) {
                    song = func_0202ca70(&gGameData);
                    if (song == 0xFF) {
                        mSongStoreFull = 1;
                    }
                }
                func_0209c73c(&sTransfer, i, song);
                if (song != 0xFF) {
                    func_0202c934(&gGameData, func_0209c718(&sTransfer, i), sSongs[n]);
                    SongEntry entry;
                    entry.mSong = func_0209c718(&sTransfer, i);
                    entry.Load(entry.mSong);
                    entry.SetNotesFromSlot(-1, i);
                    entry.Save(entry.mSong);
                }
                n++;
                record.mSongSlots[i] = func_0209c718(&sTransfer, i);
                func_0202c28c(&gGameData, sReceivedResident, &record);
            }
        }
        func_0202c28c(&gGameData, sReceivedResident, &record);
        return 4;
    }

    sReceiveCount++;
    sReceivedResident = -1;
    int i = 0;
    do {
        if (!func_0202c354(&gGameData, i)) {
            sReceivedResident = i;
            func_02097cc0(&sTransfer);
            int n = 0;
            for (int j = 0; j < 6; j++) {
                if (func_0209c718(&sTransfer, j) == 0xFF) {
                    continue;
                }
                u8 song = func_0202ca70(&gGameData);
                if (song == 0xFF) {
                    mSongStoreFull = 1;
                }
                func_0209c73c(&sTransfer, j, song);
                if (song != 0xFF) {
                    func_0202c934(&gGameData, func_0209c718(&sTransfer, j), sSongs[n]);
                    SongEntry entry;
                    entry.mSong = func_0209c718(&sTransfer, j);
                    entry.Load(entry.mSong);
                    entry.SetNotesFromSlot(-1, j);
                    entry.Save(entry.mSong);
                }
                n++;
                func_0202c28c(&gGameData, i, &sTransfer);
            }
            func_0202c28c(&gGameData, i, &sTransfer);
            MiiData_GetName(&sTransfer, sReceivedName);
            goto received;
        }
        i++;
    } while (i < 100);
    return NFL_IsMiiCreateIdBit7Clear(&sTransfer) ? 2 : 1;
received:
    return 0;
}
#pragma optimization_level reset

// @0x0214df0c
void TushinScene::UpdateListButtons() {
    func_020a3658(&mList, TRUE);
    int pressed = -1;
    for (int i = 0; i < 3; i++) {
        if (func_0203133c(&mButtons[i])) {
            pressed = i;
        }
    }
    switch (pressed) {
    case 0:
        if (GetMode() == 1) {
            mPage = 10;
        } else {
            mPage = 0x2B;
        }
        func_02034720(&mListWindow, 0x12, 1);
        break;
    case 1:
        if (GetMode() == 1) {
            mPage = 0x23;
        } else {
            mPage = 0x41;
        }
        SetUpWindow(mPage);
        func_02034720(&mWindow, 0x12, 1);
        break;
    case 2:
        mPage = GetMode() == 1 ? 0xB : 0x2C;
        MiiData_GetName(GetSelectedResident(), sReceivedName);
        SetUpWindow(mPage);
        mWindow.mTexts[0].SetStringArg(sReceivedName, 0);
        func_02034720(&mWindow, 0x12, 1);
        break;
    case -1:
        if (sReceiveCount == 0) {
            break;
        }
        if (GetMode() == 1) {
            mPage = 0xE;
        } else {
            mPage = NextPageAfterReceive();
            ResetList();
            func_020a3634(&mList);
            func_02031228(&mButtons[0]);
        }
        sSongsReceived = 0;
        SetUpWindow(mPage);
        mWindow.mTexts[0].SetStringArg(sReceivedName, 0);
        func_02034f38(&mWindow, 0, sPageMessages[mPage]);
        func_02034720(&mWindow, 0x12, 1);
        break;
    }
    func_020a0454(&mResidents);
    if (mResidents.mUnknown11398 == 0) {
        return;
    }
    ShowNextCharacter();
    func_02031e88(&mButtons[2], 0);
    mUnknown35924 = mList.mUnknown9464;
}

// NOT MATCHING: the original keeps the list window's result in r1 for the test against -1 and copies it to r4
// for the table; here it is loaded into r4 directly (one instruction fewer).
// @0x0214e144
void TushinScene::UpdateListWindow() {
    func_020341d4(&mListWindow);
    if (mListWindow.mTransitionDone) {
        if (sReceiveCount != 0 || mCloseListToReceive == 1) {
            if (GetMode() == 1) {
                mPage = 9;
            } else {
                mPage = NextPageAfterReceive();
            }
            mCloseListToReceive = 0;
            sSongsReceived = 0;
            if (mPage == 9) {
                return;
            }
            SetUpWindow(mPage);
            mWindow.mTexts[0].SetStringArg(sReceivedName, 0);
            func_02034f38(&mWindow, 0, sPageMessages[mPage]);
            func_02034720(&mWindow, 0x12, 1);
            return;
        }
        if (mLostInList == 1) {
            if (GetMode() == 1) {
                mPage = 9;
                mNextPage = 0x22;
            } else {
                mPage = 0x2A;
                mNextPage = 0x40;
            }
        } else {
            if (GetMode() == 1) {
                mPage = 9;
            } else {
                mPage = 0x2A;
            }
            return;
        }
    }
    if (((mListWindow.mUnknownAD50 & 1) && sReceiveCount != 0) || mCloseListToReceive == 1 || mLostInList == 1) {
        mListWindow.mUnknownSetBeforeClose = 15;
        func_020349cc(&mListWindow, 0x24);
        for (int i = 0; i < 7; i++) {
            func_02031228(&mListWindow.mSprites[i]);
        }
        return;
    }
    int result = mListWindow.mResult;
    if (result == -1) {
        return;
    }
    mListWindow.mUnknownSetBeforeClose = 15;
    func_020349cc(&mListWindow, 0x24);
    int order = sListOrders[result];
    if (order != mResidents.mUnknown11306) {
        func_020a123c(&mResidents, order, 0);
    }
}

// @0x0214e348
void TushinScene::UpdateConnection() {
    if (mWait > 0) {
        return;
    }
    if (GetMode() == 2) {
        UpdateWiiConnection();
        return;
    }
    switch (WH_GetSystemState()) {
    case 1:
        gWirelessSession->SetUserGameInfo();
        gWirelessSession->UpdateScan();
        gWirelessSession->UpdateMeasureChannel();
        if (gWirelessSession->UpdateConnect()) {
            return;
        }
        mNextPage = 0x25;
        WH_Reset();
        return;
    case 7: {
        u16 tgid = WM_GetNextTgid();
        WH_ParentConnect(0, tgid, WH_GetMeasureChannel());
        return;
    }
    case 2:
        if (gWirelessSession->IsParentFound()) {
            gWirelessSession->ClearParentFound();
            WirelessSession *manager = gWirelessSession;
            manager->GetFoundParentName(mParentNames[manager->GetFoundParent()]);
        }
        if (mPage != 5) {
            return;
        }
        for (int i = 0; i < 3; i++) {
            if (sParentTimers[i] <= 0) {
                continue;
            }
            if (sParentTimers[i] == 600) {
                mWindow.mTexts[i + 1].SetStringArg(mParentNames[i], 0);
                ClearText(&mWindow.mTexts[i + 1]);
                mWindow.mTexts[i + 1].DrawAll();
                func_02031e88(&mWindow.mSprites[i], 0);
            }
            if (--sParentTimers[i] == 0) {
                mWindow.mTexts[i + 1].SetStringArg(L"", 0);
                ClearText(&mWindow.mTexts[i + 1]);
                mWindow.mTexts[i + 1].DrawAll();
                func_02031e88(&mWindow.mSprites[i], 1);
            }
        }
        return;
    case 4: {
        int next;
        BOOL go;
        go = FALSE;
        next = 0x44;
        switch (mPage) {
        case 3:
            next = 7;
            if (WH_GetBitmap() != 1) {
                go = TRUE;
            }
            break;
        case 6:
            next = 8;
            if (mWindow.mUnknownAD50 & 1) {
                go = TRUE;
            }
            break;
        }
        if (!go || mNextPage == next) {
            return;
        }
        mNextPage = next;
        if (next == 7) {
            SetEntry(FALSE);
        }
        func_020349cc(&mWindow, 0x1C);
        return;
    }
    case 8:
        if (mPage == 0x25) {
            return;
        }
        mNextPage = 0x25;
        if (mWindow.mUnknownAD50 & 1) {
            func_020349cc(&mWindow, 0x1C);
        }
        WH_Reset();
        return;
    }
}

// @0x0214e620
void TushinScene::UpdateWiiConnection() {
    int state = MiiTransfer_GetState();
    if (state == 0) {
        return;
    }
    switch (state) {
    case 1:
        sWiiParentCount = MiiTransfer_GetParentCount();
        for (int i = 0; i < sWiiParentCount; i = (u16)(i + 1)) {
            if (!MiiTransfer_HasParent(i)) {
                continue;
            }
            if (MiiTransfer_GetParentAge(i) > 0) {
                continue;
            }
            BOOL known = FALSE;
            WMBssDesc *parent = MiiTransfer_GetParent(i);
            for (int j = 0; j < 3; j++) {
                if (sParentTimers[j] != 0 && memcmp(parent->bssid, sParents[j].bssid, 12) == 0) {
                    sParentTimers[j] = 599;
                    known = TRUE;
                    break;
                }
            }
            if (known) {
                continue;
            }
            for (u16 k = 0; k < 3; k++) {
                if (sParentTimers[k] == 0) {
                    sParentTimers[k] = 600;
                    sParents[k] = *parent;
                    u16 size = MiiTransfer_GetParentUserGameInfoLength(i);
                    const u16 *name = MiiTransfer_GetParentUserGameInfo(i);
                    for (int j = 0; j < size / 2; j++) {
                        mParentNames[k][j] = name[j];
                    }
                    mParentNames[k][size / 2] = 0;
                    mParentIds[k] = i;
                    break;
                }
            }
        }
        if (mPage != 0x27) {
            return;
        }
        for (int i = 0; i < 3; i++) {
            if (sParentTimers[i] <= 0) {
                continue;
            }
            if (sParentTimers[i] == 600) {
                mWindow.mTexts[i + 1].SetStringArg(mParentNames[i], 0);
                ClearText(&mWindow.mTexts[i + 1]);
                mWindow.mTexts[i + 1].DrawAll();
                func_02031e88(&mWindow.mSprites[i], 0);
            }
            if (--sParentTimers[i] == 0) {
                mWindow.mTexts[i + 1].SetStringArg(L"", 0);
                ClearText(&mWindow.mTexts[i + 1]);
                mWindow.mTexts[i + 1].DrawAll();
                func_02031e88(&mWindow.mSprites[i], 1);
            }
        }
        break;
    case 6:
        if (mPage != 0x28) {
            return;
        }
        if (!(mWindow.mUnknownAD50 & 1)) {
            return;
        }
        mNextPage = 0x29;
        CloseWindow();
        break;
    }
}

#pragma optimization_level 2
#pragma opt_common_subs off
// NOT MATCHING (optimization level 2, without common subexpressions): one block, the call that finds the current
// animation, where the original computes the animation id (r2, through r0) before the model's address (r0); here
// the address comes first and the id goes through r1. Without opt_common_subs the original's hoisting (only
// &mCharacters[0] and its model) and its registers come out.
static inline CharacterModel *GetModelOf(CharacterModel *model) { return model; }
// @0x0214e9e8
void TushinScene::UpdateCharacters() {
    BOOL sent = FALSE;
    int i;
    for (i = 0; i < 3; i++) {
        if (i == 2 && GetModelOf(&mCharacters[i].mModel)->IsVisible() == TRUE) {
            int animation = mCharacters[i].mModel.mAnimId;
            if (animation == 0x28 || animation == 0x23 || animation == GetStandAnimation(i)) {
                switch (mPage) {
                case 14: case 15: case 16:
                case 23: case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32: case 33:
                case 53: case 54: case 55: case 56: case 57: case 58: case 59: case 60: case 61:
                case 63:
                    break;
                default:
                    func_02091a14(&mCharacters[i], FALSE);
                    StartEmitter(0);
                    sent = TRUE;
                    ReloadCharacterFromResident(i);
                    break;
                }
            }
        }
        if (AnimModel_FindAnimation(&mCharacters[i].mModel, RESOURCE_BCA0, mCharacters[i].mModel.mAnimId)->mFinished) {
            mCharacters[i].mModel.mPos.x = -0x1000;
            int next = GetStandAnimation(i);
            switch (mCharacters[i].mModel.mAnimId) {
            case 0x26:
                next = 0x24;
                break;
            case 0x27:
                Character_SetAnimation(&mCharacters[i], RESOURCE_BCA0, 0x25);
                func_0208ef00(&mCharacters[i].mModel, 1, 0);
                mCharacters[i].mModel.mPos.x = 0x1000;
                return;
            case 0x25:
                next = 0x29;
                Character_SetAnimation(&mCharacters[i], RESOURCE_BCA0, next);
                func_0208ef00(&mCharacters[i].mModel, 1, 0);
                mCharacters[i].mModel.mPos.x = 0x1000;
                break;
            case 0x29:
                mSendQueued = 1;
                func_02091a14(&mCharacters[i], FALSE);
                break;
            case 0x23:
                mCharacters[i].mModel.mPos.x = 0x1000;
                break;
            case 0x28:
                mCharacters[i].mModel.mPos.x = 0x1000;
                next = 0x23;
                break;
            }
            func_020919a0(&mCharacters[i], 0x1000);
            func_020919e8(&mCharacters[i], RESOURCE_BCA0, next);
            if (next != GetStandAnimation(i)) {
                func_0208ef00(&mCharacters[i].mModel, 1, 0);
            }
            Character_ResetSoundPhase(&mCharacters[i], 0);
            Character_ResetSoundPhase(&mCharacters[i], 1);
        } else {
            switch (mCharacters[i].mModel.mAnimId) {
            case 0x26:
                Character_PlaySoundAtAnimPhase(&mCharacters[i], 0, 1, 0x19, 0, 1);
                break;
            case 0x27:
                Character_PlaySoundAtAnimPhase(&mCharacters[i], 0, 1, 0x19, 1, 1);
                break;
            case 0x24:
                Character_PlaySoundAtAnimPhase(&mCharacters[i], 0, 1, 0x19, 2, 1);
                break;
            case 0x25:
                Character_PlaySoundAtAnimPhase(&mCharacters[i], 0, 1, 0x19, 3, 1);
                break;
            case 0x29:
                Character_PlaySoundAtAnimPhase(&mCharacters[i], 0, 1, 0x19, 4, 1);
                break;
            case 0x23:
                Character_PlaySoundAtAnimPhase(&mCharacters[i], 0, 1, 0x19, 6, 1);
                break;
            }
        }
    }
    if (sent != TRUE) {
        return;
    }
    if (FindResident(&sTransfer) != FindResident(GetSelectedResident())) {
        return;
    }
    BOOL show = FALSE;
    if (GetMode() == 1 && mReceiveResult == 4) {
        show = TRUE;
    }
    if (GetMode() == 2 && mReceiveResult == 0x37) {
        show = TRUE;
    }
    if (show != TRUE) {
        return;
    }
    ShowNextCharacter();
    StartEmitter(1);
}
#pragma opt_common_subs reset
#pragma optimization_level reset

// @0x0214ef08
void TushinScene::DrawCharacters() {
    switch (mPage) {
    case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:
    case 38: case 39: case 40: case 41:
        mCharactersVisible = FALSE;
        return;
    case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20:
    case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32:
    case 33: case 35: case 36: case 42: case 43: case 44: case 45: case 46: case 47: case 48: case 49: case 50:
    case 51: case 52: case 53: case 54: case 55: case 56: case 57: case 58: case 59: case 60: case 61: case 62:
    case 63: case 65: case 66:
        mCharactersVisible = TRUE;
        break;
    case 34: case 37: case 64: case 67:
        if (mCharactersVisible == FALSE) {
            return;
        }
        break;
    }
    for (int i = 0; i < 3; i++) {
        func_02091968(&mCharacters[i]);
    }
}

// @0x0214f088
int TushinScene::GetStandAnimation(int i) {
    return data_020fb870[3][0];
}

// @0x0214f098
void TushinScene::SetStandAnimation(u8 i) {
    func_020919e8(&mCharacters[i], RESOURCE_BCA0, GetStandAnimation(i));
    func_020919a0(&mCharacters[i], 0x1000);
    func_02091a14(&mCharacters[i], TRUE);
}

// @0x0214f0e4
void TushinScene::SetOtherStandAnimation() {
    SetStandAnimation(mCurrent ^ 1);
}

// @0x0214f100
void TushinScene::StartSendAnimation() {
    func_020919e8(&mCharacters[mCurrent ^ 1], RESOURCE_BCA0, 0x26);
    func_0208ef00(&mCharacters[mCurrent ^ 1].mModel, 1, 0);
    func_020919a0(&mCharacters[mCurrent ^ 1], 0x1000);
    func_020919e8(&mCharacters[2], RESOURCE_BCA0, 0x27);
    func_0208ef00(&mCharacters[2].mModel, 1, 0);
    mCharacters[2].mModel.mPos.x = -0x1000;
    func_020919a0(&mCharacters[2], 0x1000);
    func_02091a14(&mCharacters[2], TRUE);
    mIsReceiveAnimation = 0;
}

// @0x0214f1ec
void TushinScene::StartReceiveAnimation() {
    func_02091a14(&mCharacters[0], FALSE);
    func_02091a14(&mCharacters[1], FALSE);
    func_020919e8(&mCharacters[2], RESOURCE_BCA0, 0x29);
    func_0208ef00(&mCharacters[2].mModel, 1, 0);
    mCharacters[2].mModel.mPos.x = -0x1000;
    func_020919a0(&mCharacters[2], 0x1000);
    func_02091a14(&mCharacters[2], TRUE);
    mIsReceiveAnimation = 1;
}

// @0x0214f280
void TushinScene::UpdateMusic() {
    switch (mPage) {
    case 3:
    case 4:
    case 6:
    case 13:
    case 14:
    case 15:
    case 16:
    case 0x26:
    case 0x28:
    case 0x2E:
    case 0x2F:
    case 0x30:
        if (mMusicPlaying != 0) {
            return;
        }
        mMusicPlaying = 1;
        gSoundManager.StartSeqArc(2, 0x19, 9);
        return;
    default:
        if (mMusicPlaying != 1) {
            return;
        }
        mMusicPlaying = 0;
        gSoundManager.StopSeqArc(2, 0);
        return;
    }
}

// @0x0214f374
void TushinScene::ResetList() {
    func_020a3658(&mList, FALSE);
    func_020a0454(&mResidents);
}

// @0x0214f39c
void TushinScene::ShowReceived() {
    int mode = GetMode();
    if (mode != 1) {
        if (mode != 2) {
            return;
        }
        if (sReceivedResident == -1) {
            return;
        }
        LoadSavedCharacter(2, sReceivedResident);
    } else {
        ReloadCharacterFromReceived(2);
    }
    Character_SetAnimation(&mCharacters[2], RESOURCE_BCA0, 0x28);
    func_0208ef00(&mCharacters[2].mModel, 1, 0);
    mCharacters[2].mModel.mPos.x = 0x1000;
    func_020919a0(&mCharacters[2], 0x1000);
    func_02091a14(&mCharacters[2], TRUE);
    gSoundManager.StartSeqArc(1, 0x19, 5);
}

// @0x0214f45c
void TushinScene::CloseWindow() {
    mWindow.mUnknownSetBeforeClose = 15;
    func_020349cc(&mWindow, 0x1C);
}

// @0x0214f47c
void TushinScene::StartEmitter(int i) {
    if (mEmitters[i] != NULL) {
        func_0207b364(mParticles, mEmitters[i]);
    }
    mParticleResource++;
    if (i > 1) {
        i = 1;
    } else if (i < 0) {
        i = 0;
    }
    mEmitters[i] = func_0207b2fc(mParticles, 0, &mEmitterPositions[i]);
    mEmitterStarted = 1;
    gSoundManager.StartSeqArc(1, 0x19, 7);
    mParticleFrames = 0;
}

// @0x0214f528
void TushinScene::CountFrame() {
    mParticleFrames++;
}

// @0x0214f53c
void TushinScene::DrawParticles() {
    if (mParticleFrames > 600) {
        return;
    }
    func_0202ad84(data_021223e4);
    NNS_G3dGlbFlushP();
    func_0207b258(mParticles, &NNS_G3dGlb.cameraMtx);
    func_0207b138(mParticles);
}

// @0x0214f588
void TushinScene::UpdateSignal() {
    switch (mPage) {
    case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18:
    case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29: case 30:
    case 31: case 32: case 33: case 35: case 36: case 41: case 42: case 43: case 44: case 45: case 46: case 47:
    case 48: case 49: case 50: case 51: case 52: case 53: case 54: case 55: case 56: case 57: case 58: case 59:
    case 60: case 61: case 62: case 63: case 65: case 66:
        mSprite.mVisible = TRUE;
        int level = WH_GetLinkLevel();
        if (level > 3) {
            level = 3;
        } else if (level < 0) {
            level = 0;
        }
        Sprite_SetSequence(&mSprite, sLinkLevelSequences[level], TRUE);
        return;
    default:
        mSprite.mVisible = FALSE;
        return;
    }
}

// @0x0214f700
void TushinScene::Disconnect(int kind) {
    switch (kind) {
    case 0:
        switch (mPage) {
        case 0x22:
        case 0x24:
        case 0x25:
        case 0x40:
        case 0x42:
        case 0x43:
            return;
        }
        WH_Finalize();
        gWirelessSession->RequestEnd(1);
        return;
    case 1:
    case 2:
        WH_Initialize();
        return;
    }
}

// @0x0214f788
void TushinScene::Leave() {
    IslandProgress progress;
    progress.Init(NULL, NULL);
    progress.Count();
    if (progress.GetNewFacility(FALSE) != -1) {
        gSceneManager.RequestScene(0x5F);
    } else {
        gSceneManager.RequestScene(0x5E);
    }
}

#pragma optimization_level 2
// NOT MATCHING (optimization level 2): register order only. In the four calls of func_020a123c with the sort key the
// original loads the key's offset into r0 and forms the list's address through r2 (ours: r1, r0), and in the two
// SetStringArg calls with a parent's name the parent index and the names' base swap r2/r3 in the mla. The same
// call alone in a test file comes out as in the original: the surrounding code decides it, not the call's form.
static inline CharacterModel *GetModel(SceneCharacter *c) { return &c->mModel; }
// @0x0214f7ec
void TushinScene::Update() {
    UpdateSignal();
    CountFrame();
    gWirelessSession->UpdateEnd();
    UpdateMusic();
    UpdateCharacters();
    switch (mPage) {
    case 0x29:
    case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37: case 0x38: case 0x39:
    case 0x3A: case 0x3B: case 0x3C: case 0x3D: case 0x3E: case 0x3F:
        if (MiiTransfer_GetState() == 6) {
            break;
        }
        MiiTransfer_Update();
        break;
    default:
        MiiTransfer_Update();
        break;
    }
    func_020341d4(&mWindow);
    if (mWindow.mTransitionDone) {
        mWindowReady = 1;
    }
    if (mWait > 0) {
        mWait--;
        return;
    }

    int layout = sPageLayouts[mPage];
    if (sNextReceivedCommand != 0 && sReceivedCommand == 0) {
        sReceivedCommand = sNextReceivedCommand;
        sNextReceivedCommand = 0;
    }
    if (sReceivedCommand != 0) {
        switch (sReceivedCommand) {
        case 2:
        case 3:
            mPartnerSending = 2;
            sReceivedCommand = 0;
            if (mPage == 10) {
                mCloseListToReceive = 1;
            } else {
                return;
            }
            break;
        case 4:
            sReceivedCommand = 0;
            mPartnerReady = 1;
            break;
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
        case 10:
            break;
        }
    }
    if (mPartnerSending != 0 && mPage == 9) {
        if (mPartnerSending == 2) {
            mPage = 0xE;
        } else {
            mPage = 0xF;
        }
        ResetList();
        func_020a3634(&mList);
        func_02031228(&mButtons[0]);
        mPartnerSending = 0;
        Send(4);
        sSongsReceived = 0;
        SetUpWindow(mPage);
        mWindow.mTexts[0].SetStringArg(sReceivedName, 0);
        func_02034f38(&mWindow, 0, sPageMessages[mPage]);
        func_02034720(&mWindow, 0x12, 1);
    }

    BOOL lost = FALSE;
    int next = 0x22;
    switch (GetMode()) {
    case 1:
        if (sDisconnected == 0) {
            if (WH_GetSystemState() != 9 && WH_GetSystemState() != 10) {
                break;
            }
            if (mNextPage == 0x22) {
                break;
            }
        }
        if (mPage == 10) {
            mLostInList = 1;
            break;
        }
        if (WH_GetSystemState() != 0) {
            WH_Finalize();
            gWirelessSession->RequestEnd(1);
        }
        sDisconnected = 0;
        lost = TRUE;
        next = 0x22;
        break;
    case 2:
        if (MiiTransfer_GetState() == 0 && mPage != 0x26 && mPage != 0x27 && mPage != 0x42 && mPage != 0x30 &&
            mNextPage != 0x40 && mNextPage != 0x43 && mNextPage != 0x30) {
            if (mNeedsSave == 1) {
                mPageAfterSave = 0x40;
                mNextPage = 0x30;
                CloseWindow();
                if (!func_0202c37c(&gGameData)) {
                    return;
                }
                if (!NFL_IsMiiCreateIdBit7Clear(GetSelectedResident())) {
                    return;
                }
                func_020a0664(&mResidents);
                return;
            }
            if (mPage == 0x2B) {
                mLostInList = 1;
                break;
            }
            lost = TRUE;
            next = mPage != 0x28 ? 0x40 : 0x43;
            break;
        }
        if (mLostInList == 1) {
            mLostInList = 0;
            lost = TRUE;
            next = 0x40;
        }
        break;
    }
    if (lost) {
        mNextPage = next;
        if (layout != -1) {
            CloseWindow();
            PlaySound(0xD);
        } else {
            mPage = next;
            SetUpWindow(mPage);
            func_02034f38(&mWindow, 0, sPageMessages[mPage]);
            func_02034720(&mWindow, 0x12, 1);
            PlaySound(0xC);
        }
        ResetList();
        func_020a3634(&mList);
        func_02031228(&mButtons[0]);
        mLostInList = 0;
        return;
    }

    if (layout != -1) {
        int result = mWindow.mResult;
        if (result != -1) {
            mWindow.mUnknownSetBeforeClose = 15;
            int close = 0x1C;
            switch (mPage) {
            case 0:
                switch (result) {
                case 0:
                    mNextPage = 1;
                    break;
                case 1:
                    gSceneManager.RequestScene(0xB1);
                    mNextPage = 1;
                    close = 0x2C;
                    break;
                case 3:
                    Leave();
                    mNextPage = 1;
                    close = 0x2C;
                    break;
                }
                break;
            case 1:
                switch (result) {
                case 0:
                    mNextPage = 2;
                    break;
                case 1:
                    mNextPage = 0x26;
                    WH_Finalize();
                    gWirelessSession->RequestEnd(1);
                    sWiiParentCount = 0;
                    data_ov109_0215320c = 0;
                    break;
                case 3:
                    mNextPage = 0;
                    close = 0x2C;
                    break;
                }
                break;
            case 2:
                switch (result) {
                case 0:
                    mNextPage = 3;
                    gWirelessSession->Initialize();
                    WH_SetReceiver(TushinScene_Receive);
                    break;
                case 1:
                    mNextPage = 4;
                    gWirelessSession->Initialize();
                    gWirelessSession->SetSsid();
                    WH_SetReceiver(TushinScene_Receive);
                    break;
                case 3:
                    mNextPage = 1;
                    close = 0x2C;
                    break;
                }
                break;
            case 3:
                if (result == 3) {
                    mNextPage = 2;
                    close = 0x2C;
                    WH_Finalize();
                    gWirelessSession->RequestEnd(1);
                }
                break;
            case 4:
                if (result == 3) {
                    mNextPage = 2;
                    close = 0x2C;
                    WH_Finalize();
                    gWirelessSession->RequestEnd(1);
                }
                break;
            case 5:
                if (result == 3) {
                    mNextPage = 2;
                    close = 0x2C;
                    WH_Finalize();
                    gWirelessSession->RequestEnd(1);
                } else if (result >= 0 && result < 3 && sParentTimers[result] > 0) {
                    gWirelessSession->EndScan(result);
                    mNextPage = 6;
                }
                break;
            case 7:
            case 8:
                mNextPage = 9;
                close = 0x14;
                break;
            case 11:
                if (result == 0) {
                    if (NFL_IsMiiCreateIdBit7Clear(GetSelectedResident()) == TRUE) {
                        mNextPage = 0xC;
                    } else if (CannotSend(GetSelectedResident())) {
                        mNextPage = 0x21;
                    } else {
                        mNextPage = 0xD;
                        Send(2);
                    }
                } else {
                    mNextPage = 9;
                    close = 0x14;
                }
                break;
            case 12:
                if (result == 0) {
                    if (CannotSend(GetSelectedResident()) == TRUE) {
                        mNextPage = 0x21;
                    } else {
                        mNextPage = 0xD;
                        Send(3);
                    }
                } else {
                    mNextPage = 9;
                    close = 0x14;
                }
                break;
            case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27:
            case 28: case 29: case 30: case 31: case 32: case 33:
                if (mSongStoreFull == 1) {
                    mSongStoreFull = 0;
                    mNextPage = 0x1F;
                    break;
                }
                if (mShirtDue == 1) {
                    mShirtDue = 0;
                    if (GiveShirt() == TRUE) {
                        mNextPage = 0x20;
                        break;
                    }
                }
                mNextPage = 9;
                close = 0x14;
                if (mPage == 0x11 && mIsReceiveAnimation == 1) {
                    ShowNextCharacter();
                }
                break;
            case 34:
                mNextPage = 0;
                ResetExchange();
                break;
            case 35:
                if (result == 0) {
                    mNextPage = 0x24;
                } else {
                    mNextPage = 9;
                    close = 0x14;
                }
                break;
            case 37:
            case 67:
                mNextPage = 0;
                ResetExchange();
                break;
            case 38:
                mNextPage = 1;
                close = 0x2C;
                MiiTransfer_Cut();
                break;
            case 39:
                if (result == 3) {
                    mNextPage = 1;
                    close = 0x2C;
                    MiiTransfer_Cut();
                } else if (result >= 0 && result < 3 && sParentTimers[result] > 0) {
                    gWirelessSession->mParent = result;
                    mConnectRequested = 1;
                    mNextPage = 0x28;
                }
                break;
            case 41:
                mNextPage = 0x2A;
                close = 0x14;
                break;
            case 44:
                if (result == 0) {
                    if (sReceiveCount != 0) {
                        break;
                    }
                    if (NFL_IsMiiCreateIdBit7Clear(GetSelectedResident()) == TRUE) {
                        mNextPage = 0x2D;
                    } else if (CannotSend(GetSelectedResident()) == TRUE) {
                        mNextPage = 0x3F;
                    } else {
                        mNextPage = 0x2E;
                        StartSendAnimation();
                        UpdateMiiData();
                    }
                } else {
                    mNextPage = 0x2A;
                    close = 0x14;
                }
                break;
            case 45:
                if (result == 0) {
                    if (CannotSend(GetSelectedResident()) == TRUE) {
                        mNextPage = 0x3F;
                    } else {
                        mNextPage = 0x2E;
                        func_0202c328(&gGameData, GetSelectedResident()->GetIndex(), 2);
                        func_0202d110(&gGameData, GetSelectedResident()->GetIndex());
                        StartReceiveAnimation();
                        SetNeedsSave();
                    }
                } else {
                    mNextPage = 0x2A;
                    close = 0x14;
                }
                break;
            case 49: case 50: case 51: case 52: case 53: case 54: case 55: case 56: case 57: case 58: case 59:
            case 60: case 61: case 62: case 63:
                if (mWiiShirtDue == 1) {
                    mWiiShirtDue = 0;
                    if (GiveWiiShirt() == TRUE) {
                        mNextPage = 0x3D;
                        break;
                    }
                }
                mNextPage = 0x2A;
                close = 0x14;
                if (mPage == 0x31 && mIsReceiveAnimation == 1) {
                    ShowNextCharacter();
                }
                break;
            case 64:
                mNextPage = 0;
                SetOtherStandAnimation();
                SetStandAnimation(2);
                func_02091a14(&mCharacters[2], FALSE);
                ResetExchange();
                break;
            case 65:
                if (result == 0) {
                    mNextPage = 0x42;
                } else {
                    mNextPage = 0x2A;
                    close = 0x14;
                }
                break;
            }
            func_020349cc(&mWindow, close);
        } else {
            switch (mPage) {
            case 4:
                if (WH_GetSystemState() != 2) {
                    break;
                }
                if (!IsWindowOpen(&mWindow) || mNextPage == 5) {
                    break;
                }
                if (gWirelessSession->CountParents() <= 0) {
                    break;
                }
                mNextPage = 5;
                CloseWindow();
                break;
            case 11:
            case 12:
            case 35:
                if (mPartnerSending == 0 || mNextPage == 0xD) {
                    break;
                }
                if (mPartnerSending == 2) {
                    mNextPage = 0xE;
                } else {
                    mNextPage = 0xF;
                }
                CloseWindow();
                break;
            case 13:
                if (mSendQueued != 0) {
                    break;
                }
                if (!IsWindowOpen(&mWindow)) {
                    break;
                }
                if (sSendChunk != 0 || sSendSongChunk != 0) {
                    break;
                }
                if (GetModel(&mCharacters[2])->IsVisible()) {
                    break;
                }
                switch (sReceivedCommand) {
                case 5:
                    mNextPage = 0x11;
                    break;
                case 6:
                    mNextPage = 0x13;
                    break;
                case 7:
                    mNextPage = 0x14;
                    func_02091a14(&mCharacters[mCurrent ^ 1], TRUE);
                    break;
                case 8:
                    mNextPage = 0x12;
                    func_02091a14(&mCharacters[mCurrent ^ 1], TRUE);
                    break;
                case 9:
                    mNextPage = 0x15;
                    break;
                case 10:
                    mNextPage = 0x16;
                    break;
                }
                if (mNeedsSave == 1) {
                    mPageAfterSave = mNextPage;
                    mNextPage = 0x10;
                }
                sReceivedCommand = 0;
                CloseWindow();
                func_020a05dc(&mResidents);
                func_020a123c(&mResidents, mResidents.mUnknown11306, 0);
                break;
            case 14:
            case 15: {
                if (!IsWindowOpen(&mWindow)) {
                    break;
                }
                if (sReceiveCount <= 5 || sReceivedSongChunks <= 2) {
                    break;
                }
                BOOL done = FALSE;
                if (sSongsReceived == 1) {
                    sSongsReceived = 0;
                    mReceiveResult = ReceiveCharacter();
                    func_020a05dc(&mResidents);
                    func_020a123c(&mResidents, mResidents.mUnknown11306, 0);
                    mNextPage = 0x17;
                    if (mReceiveResult != 0) {
                        sReceiveCount = 0;
                        sReceivedSongChunks = 0;
                        switch (mReceiveResult) {
                        case 3:
                            mNextPage = 0x18;
                            Send(5);
                            mShirtDue = 1;
                            ShowReceived();
                            break;
                        case 4:
                            mNextPage = 0x19;
                            Send(5);
                            mShirtDue = 1;
                            ShowReceived();
                            done = TRUE;
                            break;
                        case 1:
                            mNextPage = 0x1C;
                            Send(6);
                            break;
                        case 2:
                            mNextPage = 0x1D;
                            Send(7);
                            break;
                        case 5:
                            mNextPage = 0x1A;
                            Send(8);
                            break;
                        case 6:
                            mNextPage = 0x1B;
                            Send(9);
                            break;
                        case 7:
                            mNextPage = 0x1E;
                            Send(10);
                            break;
                        }
                        CloseWindow();
                        if (done != TRUE) {
                            break;
                        }
                        if (NFL_IsMiiCreateIdBit7Clear(&sTransfer) != TRUE) {
                            break;
                        }
                    }
                    mShirtDue = 1;
                    ShowReceived();
                    if (NFL_IsMiiCreateIdBit7Clear(&sTransfer) == TRUE) {
                        mPageAfterSave = mNextPage;
                        mNextPage = 0x10;
                        CloseWindow();
                        SetNeedsSave();
                        done = TRUE;
                    }
                    Send(5);
                    if (done == TRUE) {
                        break;
                    }
                }
                sReceiveCount = 0;
                sReceivedSongChunks = 0;
                CloseWindow();
                break;
            }
            case 16:
                if (!IsWindowOpen(&mWindow)) {
                    break;
                }
                if (mNeedsSave == 1) {
                    mWait = 60;
                    Save();
                    mNeedsSave = 0;
                    break;
                }
                if (func_0202cd38(&gGameData)) {
                    break;
                }
                sReceiveCount = 0;
                sReceivedSongChunks = 0;
                mNextPage = mPageAfterSave;
                CloseWindow();
                break;
            case 36:
                if (WH_GetSystemState() == 0 || WH_GetSystemState() == 1) {
                    mNextPage = 0;
                    CloseWindow();
                    ResetExchange();
                }
                break;
            case 38:
                if (MiiTransfer_GetState() != 1) {
                    break;
                }
                if (!IsWindowOpen(&mWindow) || mNextPage == 0x27) {
                    break;
                }
                if (sParentTimers[0] + sParentTimers[1] + sParentTimers[2] <= 0) {
                    break;
                }
                mNextPage = 0x27;
                CloseWindow();
                break;
            case 40:
                if (mConnectRequested != 1) {
                    break;
                }
                MiiTransfer_Connect(mParentIds[gWirelessSession->mParent]);
                mConnectRequested = 0;
                break;
            case 66:
                if (MiiTransfer_GetState() == 0) {
                    mNextPage = 0;
                    CloseWindow();
                    ResetExchange();
                }
                // fall through
            case 44:
            case 45:
            case 65:
                if (sReceiveCount == 0) {
                    break;
                }
                if (!IsWindowOpen(&mWindow) || mNextPage == 0x2E || mNextPage == 0x2F) {
                    break;
                }
                mNextPage = NextPageAfterReceive();
                CloseWindow();
                mWindow.mTexts[0].SetStringArg(sReceivedName, 0);
                func_02034f38(&mWindow, 0, sPageMessages[mPage]);
                break;
            case 46:
            case 47: {
                if (!IsWindowOpen(&mWindow)) {
                    break;
                }
                if (sWiiSendDone != 1) {
                    break;
                }
                if (sWiiSendResult == 0) {
                    break;
                }
                int sent = NFL_IsMiiCreateIdBit7Clear(GetSelectedResident());
                mNextPage = 0x31;
                switch (sWiiSendResult) {
                case 1:
                    if (sent) {
                        func_020a0664(&mResidents);
                    }
                    break;
                case 2:
                    mNextPage = 0x32;
                    CloseWindow();
                    sWiiSendDone = 0;
                    func_0202c328(&gGameData, GetSelectedResident()->GetIndex(), 1);
                    SetOtherStandAnimation();
                    break;
                case -5:
                    mNextPage = 0x33;
                    CloseWindow();
                    sWiiSendDone = 0;
                    return;
                case -4:
                    mNextPage = 0x34;
                    CloseWindow();
                    sWiiSendDone = 0;
                    func_0202c328(&gGameData, GetSelectedResident()->GetIndex(), 1);
                    SetOtherStandAnimation();
                    break;
                }
                if (sent) {
                    SetNeedsSave();
                    mPageAfterSave = mNextPage;
                    mNextPage = 0x30;
                }
                CloseWindow();
                sWiiSendResult = 0;
                break;
            }
            case 48:
                if (!IsWindowOpen(&mWindow)) {
                    break;
                }
                if (mNeedsSave == 1) {
                    mWait = 60;
                    Save();
                    mNeedsSave = 0;
                    break;
                }
                if (func_0202cd38(&gGameData)) {
                    break;
                }
                mNextPage = mPageAfterSave;
                CloseWindow();
                sWiiSendDone = 0;
                func_020a05dc(&mResidents);
                func_020a123c(&mResidents, mResidents.mUnknown11306, 0);
                break;
            case 53: case 54: case 55: case 56: case 57: case 58: case 59: case 60: case 62: case 63:
                if (!IsWindowOpen(&mWindow) || sReceiveCount == 0) {
                    break;
                }
                sReceiveCount = 0;
                func_020a05dc(&mResidents);
                func_020a123c(&mResidents, mResidents.mUnknown11306, 0);
                if (mPage == 0x35 || mPage == 0x36 || mPage == 0x37) {
                    ShowReceived();
                    mWiiShirtDue = 1;
                }
                break;
            }
        }

        if (mWindowReady == 1) {
            int nextLayout = sPageLayouts[mNextPage];
            if (nextLayout >= 0) {
                SetUpWindow(mNextPage);
                if (nextLayout == 4) {
                    for (int i = 0; i < 3; i++) {
                        if (sParentTimers[i] > 0) {
                            func_02031e88(&mWindow.mSprites[i], 0);
                            mWindow.mTexts[i + 1].SetStringArg(mParentNames[i], 0);
                        } else {
                            func_02031e88(&mWindow.mSprites[i], 1);
                        }
                    }
                }
                switch (mNextPage) {
                case 3:
                    gWirelessSession->RequestMeasureChannel(1);
                    break;
                case 4:
                    gWirelessSession->RequestScan(1);
                    for (int i = 0; i < 3; i++) {
                        sParentTimers[i] = 0;
                        mParentIds[i] = 0;
                    }
                    ClearParentNames();
                    break;
                case 5:
                case 39:
                    PlaySound(0xA);
                    break;
                case 7:
                    mWindow.mTexts[0].SetStringArg((wchar_t *)gWHChildSsid, 0);
                    PlaySound(0xB);
                    mUnknown3F090 = 1;
                    break;
                case 13:
                    if (mPartnerSending != 0) {
                        if (mUnknown3F090 == 0) {
                            if (mPartnerSending == 2) { mNextPage = 0xE; } else { mNextPage = 0xF; }
                            mPartnerSending = 0;
                            Send(4);
                            sSongsReceived = 0;
                            break;
                        }
                        mPartnerSending = 0;
                    }
                    switch (mPage) {
                    case 0xB:
                        StartSendAnimation();
                        break;
                    case 0xC:
                        func_0202c328(&gGameData, GetSelectedResident()->GetIndex(), 2);
                        func_0202d110(&gGameData, GetSelectedResident()->GetIndex());
                        StartReceiveAnimation();
                        break;
                    }
                    break;
                case 14:
                case 15:
                    sSongsReceived = 0;
                    mCloseListToReceive = 0;
                    if (mPage != 0xB && mPage != 0xC && mPage != 0x23) {
                        break;
                    }
                    if (mPartnerSending == 0) {
                        break;
                    }
                    mPartnerSending = 0;
                    Send(4);
                    break;
                case 16:
                    mWait = 60;
                    PlaySound(0xC);
                    break;
                case 8:
                    mWindow.mTexts[0].SetStringArg(mParentNames[gWirelessSession->mParent], 0);
                    PlaySound(0xB);
                    mUnknown3F090 = 0;
                    break;
                case 17:
                case 49:
                    PlaySound(0xB);
                    break;
                case 23:
                    mWindow.mTexts[0].SetStringArg(sReceivedName, 0);
                    PlaySound(0xB);
                    break;
                case 22: case 24: case 28: case 30: case 33:
                    PlaySound(0xD);
                    break;
                case 18: case 19: case 20: case 21: case 25: case 26: case 27: case 29: case 31:
                    PlaySound(0xC);
                    break;
                case 32:
                case 61:
                    PlaySound(0xC);
                    break;
                case 6:
                    gWirelessSession->RequestConnect(1);
                    mWait = 60;
                    break;
                case 36:
                    WH_Finalize();
                    gWirelessSession->RequestEnd(1);
                    mWait = 60;
                    break;
                case 37:
                    gWirelessSession->RequestEnd(1);
                    PlaySound(0xD);
                    break;
                case 40:
                    mWait = 60;
                    break;
                case 41:
                    mWindow.mTexts[0].SetStringArg(mParentNames[gWirelessSession->mParent], 0);
                    PlaySound(0xB);
                    break;
                case 46:
                case 47:
                    sWiiSendDone = 0;
                    break;
                case 53:
                    PlaySound(0xB);
                    break;
                case 66: {
                    int state = MiiTransfer_GetState();
                    if (state != 7 && state != 10) {
                        MiiTransfer_Cut();
                    }
                    mWait = 60;
                    break;
                }
                case 34:
                    PlaySound(0xD);
                    // fall through
                case 64:
                    mLostInList = 0;
                    break;
                case 38:
                    for (int i = 0; i < 3; i++) {
                        sParentTimers[i] = 0;
                        mParentIds[i] = 0;
                    }
                    ClearParentNames();
                    MiiTransfer_Start();
                    break;
                case 67:
                    PlaySound(0xD);
                    break;
                }
                func_02034f38(&mWindow, 0, sPageMessages[mNextPage]);
                if (mPage > mNextPage) {
                    func_02034720(&mWindow, 0x2A, 1);
                } else {
                    func_02034720(&mWindow, 0x1A, 1);
                }
                if (nextLayout == 4) {
                    for (int i = 0; i < 3; i++) {
                        if (sParentTimers[i] > 0) {
                            func_02031e88(&mWindow.mSprites[i], 0);
                        } else {
                            func_02031e88(&mWindow.mSprites[i], 1);
                        }
                    }
                }
            }
            Disconnect(mNextPage);
            mPage = mNextPage;
        }
        mWindowReady = 0;
    } else {
        switch (mPage) {
        case 9:
        case 0x2A:
            UpdateListButtons();
            break;
        case 10:
        case 0x2B:
            UpdateListWindow();
            break;
        }
    }
    UpdateConnection();
    UpdateSending();
    UpdateSaving();
}
#pragma optimization_level reset

// @0x02151590
void TushinScene::Draw() {
    Sprite_Draw(&mSprite);
    func_020345d8(&mWindow);
    for (int i = 0; i < 3; i++) {
        func_0203206c(&mButtons[i]);
    }
    func_020345d8(&mListWindow);
    func_020a05b8(&mResidents);
    DrawParticles();
    DrawCharacters();
    mBgScroll += 0x4CC;
    func_02028108(&data_02121834, 2, mBgScroll >> FX32_SHIFT);
}

// @0x02151620
void TushinScene::Exit() {
    if (GetMode() == 1 && WH_GetSystemState() != 0 && gWirelessSession->mEndRequested == 0) {
        WH_Finalize();
        gWirelessSession->RequestEnd(1);
    }
    if (mSortKey != mResidents.mUnknown11306) {
        gGameData.mMiiListSortKey = mResidents.mUnknown11306;
    }
    for (int i = 0; i < 3; i++) {
        CharacterModel *model = &mCharacters[i].mModel;
        ArchiveManager_Unmount(&gArchiveManager, model->mArchiveName);
        func_0202aecc(data_021223e4, i);
    }
    func_02024234(&data_0211d604, 0x3F00, 0);
    func_02023f4c(&data_0211d604, 0x3F00, 0);
    func_02023eac(&data_0211d604, 0x3F00);
    func_0202683c();
    ArchiveManager_Unmount(&gArchiveManager, "STE");
    ArchiveManager_Unmount(&gArchiveManager, "TBC");
    ArchiveManager_Unmount(&gArchiveManager, "TBM");
    ArchiveManager_Unmount(&gArchiveManager, "MLT");
    ArchiveManager_Unmount(&gArchiveManager, "STS");
    ArchiveManager_Unmount(&gArchiveManager, "BIC");
    if (GetMode() == 1) {
        gWirelessSession->RequestEnd(1);
        while (gWirelessSession->UpdateEnd() == 1) {
        }
        return;
    }
    if (MiiTransfer_GetState() == 0) {
        return;
    }
    do {
        MiiTransfer_Update();
    } while (MiiTransfer_GetState() != 0);
}

// @0x021517c8
int TushinScene::GetMode() {
    switch (mPage) {
    case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15:
    case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27:
    case 28: case 29: case 30: case 31: case 32: case 33: case 35: case 36:
        return 1;
    case 38: case 39: case 40: case 41: case 42: case 43: case 44: case 45: case 46: case 47: case 49: case 50:
    case 51: case 52: case 53: case 54: case 55: case 56: case 57: case 58: case 59: case 60: case 61: case 62:
    case 63: case 65: case 66:
        return 2;
    default:
        return 0;
    }
}

// @0x021518fc
BOOL TushinScene::IsNgWord(const wchar_t *text) {
    wchar_t buffer[20];
    MI_CpuClear16(buffer, sizeof(buffer));
    wcscpy(buffer, text);
    func_0201f508(buffer);
    func_02036bb4(buffer);
    u32 length = wcslen(buffer);
    if (length >= 1) {
        wchar_t last = buffer[length - 1];
        switch (last) {
        case 0x20:
        case 0x21:
        case 0x3000:
        case 0xFF01:
            for (; length >= 1; length--) {
                if (last != buffer[length - 1]) {
                    break;
                }
                buffer[length - 1] = buffer[length];
            }
            break;
        }
    }
    for (int i = 0; i < 41; i++) {
        if (wcscmp(buffer, sNgWords[i]) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

// @0x02151a0c
BOOL TushinScene::IsBlank(const wchar_t *text) {
    wchar_t buffer[20];
    MI_CpuClear16(buffer, sizeof(buffer));
    wcscpy(buffer, text);
    u32 length = wcslen(buffer);
    if (length == 0) {
        return FALSE;
    }
    for (u32 i = 0; i < length; i++) {
        if (buffer[i] != 0x20 && buffer[i] != 0x3000) {
            return FALSE;
        }
    }
    return TRUE;
}

// @0x02151a90
BOOL TushinScene::HasNgWord(CharacterRecord *record) {
    if (IsNgWord(record->mName) == TRUE) {
        return TRUE;
    }
    if (IsNgWord(record->mUnknown06E) == TRUE) {
        return TRUE;
    }
    if (IsNgWord(record->mSpeechDefault) == TRUE) {
        return TRUE;
    }
    for (int i = 0; i < 4; i++) {
        if (IsNgWord(record->mSpeechByMood[i]) == TRUE) {
            return TRUE;
        }
    }
    wchar_t nickname[11];
    MI_CpuClear16(nickname, sizeof(nickname));
    MiiData_GetCreatorName(record, nickname);
    if (IsNgWord(nickname) == TRUE) {
        return TRUE;
    }
    wchar_t name[11];
    MI_CpuClear16(name, sizeof(name));
    MiiData_GetName(record, name);
    return IsNgWord(name) == TRUE ? TRUE : FALSE;
}

// @0x02151b98
#pragma optimization_level 2
BOOL TushinScene::HasNgSongReceived() {
    for (int i = 0; i < 6; i++) {
        if (IsNgWord((wchar_t *)sSongs[i]) == TRUE) {
            return TRUE;
        }
    }
    return FALSE;
}
#pragma optimization_level reset

// @0x02151bdc
BOOL TushinScene::HasNgSong(CharacterRecord *record) {
    if (HasNgWord(record) == TRUE) {
        return TRUE;
    }
    wchar_t lyrics[32];
    for (int i = 0; i < 6; i++) {
        int song = func_0209c718(record, i);
        if (song != 0xFF) {
            func_0202c920(&gGameData, song, lyrics);
            if (IsNgWord(lyrics) == TRUE) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

// @0x02151c68
BOOL TushinScene::IsMiiBroken(CharacterRecord *record) {
    u8 buffer[0x3C];
    return NFL_GetCharInfo(buffer) ? TRUE : FALSE;
}

// @0x02151c8c
BOOL TushinScene::IsNameBlank(CharacterRecord *record) {
    wchar_t name[11];
    MI_CpuClear16(name, sizeof(name));
    MiiData_GetName(record, name);
    return IsBlank(name) == TRUE ? TRUE : FALSE;
}

// @0x02151cd8
BOOL TushinScene::IsNicknameBlank(CharacterRecord *record) {
    wchar_t nickname[11];
    MI_CpuClear16(nickname, sizeof(nickname));
    MiiData_GetCreatorName(record, nickname);
    return IsBlank(nickname) == TRUE ? TRUE : FALSE;
}

// @0x02151d24
BOOL TushinScene::IsInvalid(CharacterRecord *record) {
    if (IsMiiBroken(record) == TRUE) {
        return TRUE;
    }
    if (IsNameBlank(record) == TRUE) {
        return TRUE;
    }
    return IsNicknameBlank(record) == TRUE ? TRUE : FALSE;
}

// Whether a resident may not be sent: a song with an NG word, or an invalid Mii, name or nickname
// @0x02151d74
BOOL TushinScene::CannotSend(CharacterRecord *record) {
    if (HasNgSong(record) == TRUE) {
        return TRUE;
    }
    return IsInvalid(record) ? TRUE : FALSE;
}

// @0x02151dac
BOOL TushinScene::GiveItem(int item) {
    ClothesInventory inventory;
    BOOL given = FALSE;
    func_0209e248(&inventory);
    ClothesItem *clothes = &inventory.mItems[item];
    if (clothes->mOwned == FALSE) {
        clothes->Add(1);
        given = TRUE;
    }
    func_0209e310(&inventory);
    return given;
}

// @0x02151e24
BOOL TushinScene::GiveShirt() {
    return GiveItem(0x113);
}

// @0x02151e38
BOOL TushinScene::GiveWiiShirt() {
    return GiveItem(0x114);
}

// @0x02151e48
void TushinScene::SetEntry(BOOL enabled) {
    WM_SetEntry(TushinScene_EntryCallback, enabled);
}

// @0x02151e5c
void TushinScene::ClearParentNames() {
    MI_CpuFill8(mParentNames, 0, sizeof(mParentNames));
}

// @0x02151e78
void TushinScene::Save() {
    if (!func_0202cd48(&gGameData)) {
        gSceneManager.RequestScene(0xC2);
    }
}

// @0x02151ea4
void TushinScene::SetNeedsSave() {
    mNeedsSave = 1;
}

// @0x02151eb4
extern "C" void TushinScene_SendCallback(BOOL result) {
    switch (WH_GetSystemState()) {
    case 0:
    case 9:
    case 10:
        return;
    }
    sSent = 1;
}

// @0x02151ee0
extern "C" void TushinScene_Receive(u16 aid, u16 *data, u16 size) {
    switch (size) {
    case 0:
        sDisconnected = 1;
        return;
    case 2:
        if (sReceivedCommand == 0) {
            sReceivedCommand = *data;
        } else {
            sNextReceivedCommand = *data;
        }
        return;
    }
    if (sReceiveCount == 0) {
        func_020979e0(&sTransfer);
    }
    if (sReceiveCount >= 6) {
        MI_CpuCopy8(data, (u8 *)sSongs + sReceivedSongChunks * 0x80, 0x80);
        if (sReceivedSongChunks < 2) {
            sReceivedSongChunks++;
        } else {
            sReceivedSongChunks++;
            sSongsReceived = 1;
        }
    } else {
        MI_CpuCopy8(data, (u8 *)&sTransfer + sReceiveCount * 0x80, 0x80);
        if (sReceiveCount < 5) {
            ++sReceiveCount;
        } else {
            sReceiveCount++;
        }
    }
}

// @0x02151fcc
extern "C" void TushinScene_OnWiiSendResult(int a, int b) {
    sWiiSendResult = a;
    sWiiSendDone = 1;
}

// @0x02151fe4
int TushinScene::NextPageAfterReceive() {
    int page = ReceiveWiiMii();
    mReceiveResult = page;
    if (page != 0x35) {
        return page;
    }
    if (mNeedsSave == 1) {
        mPageAfterSave = page;
        page = 0x30;
    }
    return page;
}

// NOT MATCHING: the original derives the -1 for sReceivedResident from a register holding 0 (mov r2, #0; sub r1, r2,
// #1), and sets that 0 earlier (five instructions). mwcc derives a constant so when a register already holds a nearby
// one (storing 0 first does it, but then the stores come out in the other order); a variable equal to 0 that is not
// propagated would do it too (ItemTradeList::Receive), but none fits here.
// @0x02152014
int TushinScene::ReceiveWiiMii() {
    SavedCharacter character;
    sReceivedResident = -1;
    int result = sWiiReceiveResult;
    sWiiReceiveResult = 0;
    switch (result) {
    case -5:
        PlayResultSound(0xD);
        return 0x3A;
    case -4:
        PlayResultSound(0xC);
        return 0x3B;
    case -3:
    case -2:
        PlayResultSound(0xD);
        return 0x3E;
    }
    MI_CpuCopy8(&sMiiData[sWiiReceivedIndex], &sTransfer, sizeof(MiiData));
    if (IsInvalid((CharacterRecord *)&sMiiData[sWiiReceivedIndex])) {
        UpdateMiiData();
        PlayResultSound(0xD);
        return 0x3C;
    }
    func_020979e0(&character);
    MI_CpuCopy8(&sMiiData[sWiiReceivedIndex], &character, sizeof(MiiData));
    if (HasNgWord(&character) == TRUE) {
        UpdateMiiData();
        PlayResultSound(0xD);
        return 0x3F;
    }
    sReceivedResident = FindResident((CharacterRecord *)&sMiiData[sWiiReceivedIndex]);
    if (sReceivedResident != -1) {
        func_020979e0(&character);
        func_0202c264(&gGameData, sReceivedResident, &character);
        if (NFL_IsMiiCreateIdBit7Clear(&character)) {
            PlayResultSound(0xD);
            return 0x38;
        }
        if (func_02098584(&character)) {
            PlayResultSound(0xD);
            return 0x36;
        }
        MI_CpuCopy8(&sMiiData[sWiiReceivedIndex], &character, sizeof(MiiData));
        if (MiiData_GetSex(&character) != func_0202c604(&gGameData, sReceivedResident)) {
            PlayResultSound(0xC);
            return 0x39;
        }
        func_0202c28c(&gGameData, sReceivedResident, &character);
        MiiData_GetName(&character, sReceivedName);
        UpdateMiiData();
        PlayResultSound(0xC);
        return 0x37;
    }
    for (int i = 0; i < 100; i++) {
        if (!func_0202c354(&gGameData, i)) {
            sReceivedResident = i;
            func_020979e0(&character);
            MI_CpuCopy8(&sMiiData[sWiiReceivedIndex], &character, sizeof(MiiData));
            func_02097c64(&character);
            func_0202c28c(&gGameData, i, &character);
            MiiData_GetName(&character, sReceivedName);
            if (NFL_IsMiiCreateIdBit7Clear(&character)) {
                SetNeedsSave();
            }
            UpdateMiiData();
            return 0x35;
        }
    }
    return 0x3A;
}

// @0x021523a4
extern "C" void TushinScene_OnWiiMiiReceived(int index, int result) {
    sWiiReceiveResult = result;
    switch (result) {
    case 1:
        sWiiReceivedIndex = (u8)index;
        sReceiveCount++;
        break;
    case 2:
        sWiiReceivedIndex = (u8)index;
        sReceiveCount++;
        break;
    case 3:
        sWiiReceivedIndex = (u8)index;
        sReceiveCount++;
        break;
    case -5:
    case -4:
        sReceiveCount++;
        break;
    case -2:
        sReceiveCount++;
        break;
    case -3:
        sReceiveCount++;
        break;
    }
}

// @0x0215245c
extern "C" void TushinScene_EntryCallback(void *arg) {
}

// Not in the ROM: the link left it out, as nothing calls it. Something of this file used the 13 u16s at
// 0x021531E6 to 0x021531FE, or the compiler would have dropped them and the statics after them would sit lower; what
// it did with them is not known, and this stands in for it.
extern "C" void TushinScene_UnusedClear() {
    data_ov109_021531e6 = 0;
    data_ov109_021531e8 = 0;
    data_ov109_021531ea = 0;
    data_ov109_021531ec = 0;
    data_ov109_021531ee = 0;
    data_ov109_021531f0 = 0;
    data_ov109_021531f2 = 0;
    data_ov109_021531f4 = 0;
    data_ov109_021531f6 = 0;
    data_ov109_021531f8 = 0;
    data_ov109_021531fa = 0;
    data_ov109_021531fc = 0;
    data_ov109_021531fe = 0;
}

// The static initialiser is compiled with the settings in effect at the end of the file: the original's reads the
// item positions from the .rodata pool instead of folding them, which constant folding turned off gives.
#pragma opt_foldconstants off

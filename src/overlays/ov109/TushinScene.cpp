#include "TushinScene.h"

#include <string.h>

// Overlay 109: wireless play (TushinScene). One source file.

// --- The file's constants ---

static const u8 sYesNoSequences2[2] = { 0x75, 0x75 };
static const u8 sYesNoSequences[2] = { 0x74, 0x74 };

static const WindowPoint sListCursorPos = { 12, 47 };
static const WindowPoint sListTextPosition = { 8, 2 };
static const WindowPoint sListTitleTextPos = { 19, 41 };
static const WindowPoint sListScrollDownSize = { 14, 12 };
static const WindowPoint sBackButtonSize = { 94, 30 };
static const ScreenRect sListWindowRect = { 20, 12, 216, 168 };
static const ScreenRect sWindowRect = { 20, 20, 216, 152 };
static const WindowPoint sYesNoPositions[2] = { { 75, 149 }, { 181, 149 } };
static const WindowPoint sYesNoSizes[2] = { { 94, 30 }, { 94, 30 } };
static const ScreenRect sYesNoWindowRect = { 20, 20, 216, 152 };
static const ScreenRect sListBarRect = { -8, -8, 16, 16 };
static const WindowPoint sListButtonPositions[3] = { { 213, 19 }, { 242, 13 }, { 128, 173 } };
static const WindowPoint sListButtonTextPositions[3] = { { 26, 34 }, { 24, 22 }, { 116, 30 } };
static const WindowPoint sButtonTextPositions2[3] = { { 38, 68 }, { 38, 100 }, { 38, 118 } };
static const WindowPoint sButtonPositions2[4] = { { 128, 74 }, { 128, 106 }, { 128, 124 }, { 128, 149 } };
static const WindowPoint sButtonPositions3[4] = { { 128, 72 }, { 128, 96 }, { 128, 120 }, { 128, 149 } };
static const WindowButtonStyle sYesNoStyles[2] = { { 10, -1, 0, -1 }, { 12, -1, 0, -1 } };

static const WindowPoint sSignalPosition = { 244, 182 };
static const WindowPoint sListTitlePos = { 119, 47 };
static const WindowPoint sButtonSize2 = { 190, 26 };
static const WindowPoint sListScrollUpPos = { 244, 150 };
static const WindowPoint sButtonSize3 = { 190, 20 };
static const WindowPoint sListTitleSize = { 228, 18 };
static const WindowPoint sListScrollBarSize = { 14, 12 };
static const WindowPoint sTextPosition = { 24, 12 };
static const WindowPoint sListScrollDownPos = { 244, 44 };
static const WindowPoint sListBarPos = { 244, 97 };
// Where the signal strength sprite is
static const u32 sYesNoMessages[2] = { 2, 3 };
static const u32 sListButtonMessages[3] = { (u32)-1, (u32)-1, 0x105 };
static const int sListButtonFonts[3] = { 0, -1, 2 };
static const VecFx32 sEmitterPosition0 = { 0x1000, 0, 0 };
static const int sListButtonFiles[3] = { 1, 1, 0 };
static const VecFx32 sCameraPosition = { 0, 0x24CC, 0xC000 };
static const VecFx32 sCameraTarget = { 0, 0x1CCC, 0 };
static const VecFx32 sEmitterPosition1 = { -0x1000, 0, 0 };
static const WindowButtonStyle sButtonStyle = { 10, -1, 0, -1 };
static const WindowPoint sFirstPageIconPositions[2] = { { 204, 74 }, { 204, 106 } };
static const WindowPoint sTextMargins[3] = { { 32, 33 }, { 32, 50 }, { 32, 33 } };
static const WindowPoint sButtonTextPositions3[3] = { { 38, 66 }, { 38, 90 }, { 38, 114 } };
static const int sListOrders[7] = { 0, 1, 2, 3, 4, 5, 6 }; // What the list is sorted by, by the button chosen
static const u32 sListMessages[7] = { 127, 128, 129, 130, 131, 132, 133 };
static const WindowButtonStyle sListButtonStyles[3] = { { 10, -1, 0, -1 }, { 12, -1, 0, -1 }, { 10, -1, 0, -1 } };
static const WindowPoint sListItemPositions[7] = {
    { 128, 30 }, { 128, 52 }, { 128, 74 }, { 128, 96 }, { 128, 118 }, { 128, 140 }, { 128, 162 },
};
static const WindowPoint sListItemSizes[7] = {
    { 128, 20 }, { 128, 20 }, { 128, 20 }, { 128, 20 }, { 128, 20 }, { 128, 20 }, { 128, 20 },
};

static const u8 sListButtonSequences[3] = { 0x7B, 0x00, 0x76 };
static const u8 sListButtonSequences2[3] = { 0x7C, 0x01, 0x77 };
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
    { { FALSE, FALSE, FALSE }, FALSE },
    { { FALSE, TRUE, FALSE }, FALSE },
};
static const WindowButtonStyle sBackButtonStyles[9] = {
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
// Where the list window's texts are: on its items, 32 pixels left of and 6 above each (set by the static
// initialiser)
// The words that a received character's texts must not be (IsNgWord compares each text with them, without its
// trailing spaces and exclamation marks)
static const wchar_t *sNgWords[41] = {
    L"シネ", L"アイエキ", L"セイエキ", L"adhd", L"fuck", L"イヌゴロシ", L"インバイ", L"インモウ", L"ウンコ", L"ウンチ", L"オメコ", L"カタワ",
    L"キチガイ", L"クロンボ", L"コロス", L"ゴウカン", L"サンゴクジン", L"シナジン", L"セックス", L"チャンコロ", L"チンコ", L"チンチン", L"チンポ", L"チンバ",
    L"ツンボ", L"ドカタ", L"ドモリ", L"ニグロ", L"ニンピニン", L"ビッコ", L"フェラ", L"ブラク", L"ペニス", L"マンコ", L"オマンコ", L"メクラ",
    L"ヤラセロ", L"ヤリマン", L"リョウジョク", L"レイプ", L"ロリコン",
};

// NOT MATCHING (the static initialiser, __sinit_TushinScene.cpp): the compiler folds sListItemPositions into
// immediates here, where the original loads them from the .rodata pool (pool+0x294) and subtracts; a const source
// is folded by every compiler version tried (2.0 SP1p2 to SP2p3), and only a non-const one is loaded.
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
    5, 6, 7, 3, 4, 6, 7, -1, -1, 5, 5, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 5, 6, 7,
};

// The Mii data of the residents, as they are sent (all 100 saved characters, empty ones cleared)
static MiiData sMiiData[100];
// The character sent or received
static SavedCharacter sTransfer;

// The state of the wireless exchange (WORK IN PROGRESS: these are separate statics of the file, pooled by size)
struct WirelessState {
    /* 0x00 */ u8 m00;
    /* 0x02 */ u16 m02;
    /* 0x04 */ u16 m04;
    /* 0x06 */ u16 m06[13];
    /* 0x20 */ u16 m20;
    /* 0x22 */ u16 m22;
    /* 0x24 */ u16 m24;
    /* 0x26 */ u16 m26;
    /* 0x28 */ u16 m28;
    /* 0x2C */ int m2C;
    /* 0x30 */ int m30;
    /* 0x34 */ int m34;
    /* 0x38 */ int m38;
    /* 0x3C */ int m3C;
    /* 0x40 */ int m40;
    /* 0x44 */ int m44;
    /* 0x48 */ int m48;
    /* 0x4C */ int m4C;
    /* 0x50 */ int m50;
};
static WirelessState sWireless;
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
        mWindow.mSprites[i].mSprite.mUnknown5E = 8;
    }
    mWindow.mButtons[0].mSprite.mUnknown5E = 8;
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
    mUnknown35920 = gGameData.mMiiListSortKey;
    func_020a123c(&mResidents, mUnknown35920, 1);
    mResidents.mUnknown1139C = 1;

    for (int i = 0; i < 3; i++) {
        Sprite_Init(&mButtons[i].mSprite, 0, sListButtonSequences[i], 0x80, 0x60, TRUE);
        func_02031260(&mButtons[i], 4);
        func_02031278(&mButtons[i], sListButtonSequences[i], sListButtonSequences2[i]);
        mButtons[i].mSprite.SetPosition(
            FxVec2(sListButtonPositions[i].x << FX32_SHIFT, sListButtonPositions[i].y << FX32_SHIFT));
        func_02031eb0(&mButtons[i], sListButtonTextPositions[i].x, sListButtonTextPositions[i].y);
        mButtons[i].mUnknown0DC = 1;
        func_02032158(&mButtons[i], &sListButtonStyles[i]);
        func_0203119c(&mButtons[i]);
        mButtons[i].mSprite.mUnknown5E = 1;
        if (sListButtonMessages[i] == (u32)-1) {
            mButtons[i].mUnknown158 = 0;
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
        func_02065abc(mCharacters[i].mModel.mResMdl, 3);
    }
    func_02091a14(&mCharacters[0], TRUE);
    mCurrent = 1;
    func_02091a14(&mCharacters[1], FALSE);
    func_02091a14(&mCharacters[2], FALSE);
    mUnknown3F000 = 0;
    mCamera = &data_02122764;
    func_0202a640(mCamera);
    VEC_Set(&mCamera->mPosition, sCameraPosition.x, sCameraPosition.y, sCameraPosition.z);
    VEC_Set(&mCamera->mTarget, sCameraTarget.x, sCameraTarget.y, sCameraTarget.z);
    func_0202a96c(&data_021227cc[0]);
    func_0202a96c(&data_021227cc[1]);
    func_0202ab2c(&data_021227cc[0], mCamera->mAngleX, mCamera->mAngleY, mCamera->mAngleZ, 0x640, 0x3E8 - 0x640,
                  -0xFA0);
    func_0202ab2c(&data_021227cc[1], mCamera->mAngleX, mCamera->mAngleY, mCamera->mAngleZ, 0, -0xFA0, 0x5DC);
    LoadCharacters();
}

// NOT MATCHING: the two `|= 2` of the emitters' flags use r2/r3 where the original uses r0/r2 and sets
// mUnknown3F034's 0 after them (register choice, 9 instructions).
// @0x0214ad80
void TushinScene::InitParticles() {
    func_0202680c(0x4000);
    mParticles = func_0207ab20(func_02026824, 2, 100, 0, 1, 30);
    mParticleFile = ArchiveManager_GetFileByName(&gArchiveManager, "STE:/SplTushinEffect.spa");
    func_0207ac78(mParticles, mParticleFile);
    func_0207b110(mParticles);
    func_0207b124(mParticles);
    mParticleResource = 0;
    mUnknown3F030 = 1;
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
    mUnknown3F034 = 0;
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
            func_02073574(&sMiiData[i]);
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
    mUnknown18 = 0;
    mUnknown1C = 0;
    mWait = 0;
    mUnknown24 = 0;
    mUnknown28 = 0;
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

    sWireless.m22 = 0;
    sWireless.m24 = 0;
    sWireless.m26 = 0;
    sWireless.m04 = 0;
    sWireless.m44 = 0;
    sWireless.m00 = 0;
    sWireless.m02 = 0;
    sWireless.m28 = 0;
    sWireless.m40 = 0;
    sWireless.m48 = 0;
    sWireless.m34 = 0;
    MI_CpuFill8(sReceivedName, 0, sizeof(sReceivedName));
    func_0203b7d4(data_0213b700, sParents, 3);
    func_0203b7e0(data_0213b700, sParentTimers);
    func_0203b76c(data_0213b700, 0);
    MI_CpuFill8(sParents, 0, sizeof(sParents));
    MI_CpuFill8(sParentTimers, 0, sizeof(sParentTimers));
    mUnknown3F080 = 0;
    mUnknown3F09C = 0;
    mUnknown3F0A0 = 0;
    mUnknown3F0A4 = 0;
    mUnknown3F0A8 = 0;
    mUnknown3F084 = 0;
    mUnknown3F088 = 0;
    mUnknown3F08C = 0;
    mUnknown3F090 = 0;
    mUnknown3F094 = 0;
    mUnknown3F098 = 0;
    mUnknown3F0AC = 0;
    mUnknown3F0B0 = 0;
    sWireless.m50 = 0;
    func_0207777c(&data_0212349c, sMiiData, 100);
    func_02077720(func_ov109_02151fcc);
    func_02077710(func_ov109_021523a4);
    func_02077804(1);
    UpdateMiiData();
    gSoundManager.LoadGroup(0x5D);
    ResetExchange();
}

// @0x0214b3d8
void TushinScene::ResetExchange() {
    mUnknown3F08C = 0;
    sWireless.m22 = 0;
    sWireless.m24 = 0;
    sWireless.m26 = 0;
    sWireless.m04 = 0;
    sWireless.m02 = 0;
    sWireless.m28 = 0;
    UpdateMiiData();
    MI_CpuFill8(sReceivedName, 0, sizeof(sReceivedName));
    ClearParentNames();
    mUnknown3F0B0 = 0;
    for (int i = 0; i < 12; i++) {
        gWHChildSsid[i] = 0;
    }
    mUnknown3F084 = 0;
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
    if (sWireless.m24 != 0) {
        return;
    }
    sWireless.m24 = 1;
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
    WH_SendData(sSongs, 0x80, func_ov109_02151eb4);
    sWireless.m3C = 0;
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
        sWireless.m22 = 0;
        sWireless.m24 = 0;
        sWireless.m3C = 0;
        return;
    }
    sWireless.m20 = kind;
    switch (kind) {
    case 0:
        if (sWireless.m22 != 0) {
            return;
        }
        sWireless.m22 = 1;
        func_020979e0(&sTransfer);
        (CharacterRecord &)sTransfer = mResidents.mResidents[mResidents.mSelected];
        WH_SendData(&sTransfer, 0x80, func_ov109_02151eb4);
        sWireless.m3C = 0;
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
        WH_SendData(&sWireless.m20, 2, func_ov109_02151eb4);
        return;
    }
}

// NOT MATCHING: for messages 7 and 8, the original reads the selected resident's mIndex off the element's address
// (base + selected * 0x2C0, then +0x2A4); here the offsets 0xB004 and 0x2A4 are folded into one (two instructions
// fewer).
// @0x0214ce60
void TushinScene::UpdateSending() {
    if (sWireless.m3C != 1) {
        return;
    }
    int state = WH_GetSystemState();
    if (!(state != 0 && state != 9 && state != 10)) {
        sWireless.m22 = 0;
        sWireless.m24 = 0;
        sWireless.m3C = 0;
        mNextPage = 0x22;
        return;
    }
    if (sWireless.m24 != 0) {
        if (sWireless.m24 >= 3) {
            int result = func_02073520(GetSelectedResident());
            switch (sWireless.m02) {
            case 5:
                sWireless.m24 = 0;
                if (func_02073520(GetSelectedResident())) {
                    func_020a0664(&mResidents);
                }
                break;
            case 6:
            case 9:
            case 10:
                sWireless.m24 = 0;
                break;
            case 7:
            case 8: {
                sWireless.m24 = 0;
                func_0202c328(&gGameData, mResidents.mResidents[mResidents.mSelected].GetIndex(), 1);
                SetOtherStandAnimation();
                break;
            }
            default:
                return;
            }
            if (result == 1) {
                SetUnknown3F098();
            }
        } else {
            WH_SendData((u8 *)sSongs + sWireless.m24 * 0x80, 0x80, func_ov109_02151eb4);
            sWireless.m24++;
            sWireless.m3C = 0;
        }
    } else if (sWireless.m22 != 0) {
        if (sWireless.m22 >= 6) {
            sWireless.m22 = 0;
            Send(1);
        } else {
            WH_SendData((u8 *)&sTransfer + sWireless.m22 * 0x80, 0x80, func_ov109_02151eb4);
            sWireless.m22++;
            sWireless.m3C = 0;
        }
    }
}

// @0x0214d064
void TushinScene::SendFoundIndex() {
    sWireless.m48 = 0;
    u16 index = FindResident(GetSelectedResident());
    if (index == -1) {
        return;
    }
    func_02078db8(index);
}

// @0x0214d0b0
void TushinScene::UpdateSaving() {
    if (mUnknown3F084 == 0) {
        return;
    }
    if (GetMode() == 1) {
        if (mUnknown3F088 == 0) {
            return;
        }
    } else if (func_0202cd38(&gGameData)) {
        return;
    }
    mUnknown3F084 = 0;
    mUnknown3F088 = 0;
    Send(0);
}

// @0x0214d118
int TushinScene::FindResident(CharacterRecord *record) {
    for (int i = 0; i < 100; i++) {
        if (func_0202c354(&gGameData, i)) {
            CharacterRecord resident;
            func_0202c264(&gGameData, i, &resident);
            if (func_02073560(&resident, record) == 1) {
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
// NOT MATCHING (optimization level 2, as the original's loops are): the copy of bits 0..3 of the record's 0x54
// and the `|=` of 0x299 load their operands into other registers (10 instructions).
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
    sWireless.m44 = FindResident(&sTransfer);
    if (sWireless.m44 != -1) {
        if (func_02073520(&sTransfer)) {
            return 5;
        }
        if (func_02098584(&sTransfer)) {
            return 3;
        }
        if (func_0207a710(&sTransfer) != func_0202c604(&gGameData, sWireless.m44)) {
            return 6;
        }
        CharacterRecord record;
        func_0202c264(&gGameData, sWireless.m44, &record);
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
            record.mUnknown054Bits0 = sTransfer.mUnknown054Bits0;
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
            u8 flags = sTransfer.mUnknown299;
            record.mUnknown299 |= flags;
            int n = 0;
            for (int i = 0; i < 6; i++) {
                if (func_0209c718(&sTransfer, i) == 0xFF) {
                    continue;
                }
                u8 song = record.mSongSlots[i];
                if (song == 0xFF) {
                    song = func_0202ca70(&gGameData);
                    if (song == 0xFF) {
                        mUnknown3F09C = 1;
                    }
                }
                func_0209c73c(&sTransfer, i, song);
                if (song != 0xFF) {
                    func_0202c934(&gGameData, func_0209c718(&sTransfer, i), sSongs[n]);
                    SongEntry entry;
                    func_020af058(&entry);
                    entry.mSong = func_0209c718(&sTransfer, i);
                    func_020af084(&entry, entry.mSong);
                    func_020af0b8(&entry, -1, i);
                    func_020af06c(&entry, entry.mSong);
                }
                n++;
                record.mSongSlots[i] = func_0209c718(&sTransfer, i);
                func_0202c28c(&gGameData, sWireless.m44, &record);
            }
        }
        func_0202c28c(&gGameData, sWireless.m44, &record);
        return 4;
    }

    sWireless.m26++;
    sWireless.m44 = -1;
    int i = 0;
    do {
        if (!func_0202c354(&gGameData, i)) {
            sWireless.m44 = i;
            func_02097cc0(&sTransfer);
            int n = 0;
            for (int j = 0; j < 6; j++) {
                if (func_0209c718(&sTransfer, j) == 0xFF) {
                    continue;
                }
                u8 song = func_0202ca70(&gGameData);
                if (song == 0xFF) {
                    mUnknown3F09C = 1;
                }
                func_0209c73c(&sTransfer, j, song);
                if (song != 0xFF) {
                    func_0202c934(&gGameData, func_0209c718(&sTransfer, j), sSongs[n]);
                    SongEntry entry;
                    func_020af058(&entry);
                    entry.mSong = func_0209c718(&sTransfer, j);
                    func_020af084(&entry, entry.mSong);
                    func_020af0b8(&entry, -1, j);
                    func_020af06c(&entry, entry.mSong);
                }
                n++;
                func_0202c28c(&gGameData, i, &sTransfer);
            }
            func_0202c28c(&gGameData, i, &sTransfer);
            func_0207a80c(&sTransfer, sReceivedName);
            goto received;
        }
        i++;
    } while (i < 100);
    return func_02073520(&sTransfer) ? 2 : 1;
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
        func_0207a80c(GetSelectedResident(), sReceivedName);
        SetUpWindow(mPage);
        mWindow.mTexts[0].SetStringArg(sReceivedName, 0);
        func_02034720(&mWindow, 0x12, 1);
        break;
    case -1:
        if (sWireless.m26 == 0) {
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
        sWireless.m30 = 0;
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
        if (sWireless.m26 != 0 || mUnknown24 == 1) {
            if (GetMode() == 1) {
                mPage = 9;
            } else {
                mPage = NextPageAfterReceive();
            }
            mUnknown24 = 0;
            sWireless.m30 = 0;
            if (mPage == 9) {
                return;
            }
            SetUpWindow(mPage);
            mWindow.mTexts[0].SetStringArg(sReceivedName, 0);
            func_02034f38(&mWindow, 0, sPageMessages[mPage]);
            func_02034720(&mWindow, 0x12, 1);
            return;
        }
        if (mUnknown28 == 1) {
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
    if (((mListWindow.mUnknownAD50 & 1) && sWireless.m26 != 0) || mUnknown24 == 1 || mUnknown28 == 1) {
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
        func_0203b898(data_0213b700);
        func_0203b908(data_0213b700);
        func_0203b948(data_0213b700);
        if (func_0203b970(data_0213b700)) {
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
        if (func_0203b9b4(data_0213b700)) {
            func_0203b9fc(data_0213b700);
            WirelessManager *manager = data_0213b700;
            func_0203ba20(manager, mParentNames[func_0203ba10(manager)]);
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
    int state = func_02078f04();
    if (state == 0) {
        return;
    }
    switch (state) {
    case 1:
        sWireless.m4C = func_0207808c();
        for (int i = 0; i < sWireless.m4C; i = (u16)(i + 1)) {
            if (!func_0207800c(i)) {
                continue;
            }
            if (func_02078040(i) > 0) {
                continue;
            }
            BOOL known = FALSE;
            WMBssDesc *parent = func_02078064(i);
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
                    u16 size = func_0207809c(i);
                    const u16 *name = func_020780b4(i);
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
// NOT MATCHING (optimization level 2): the original reads mVisible of character 2's model off the model's
// address and hoists only &mCharacters[0] and &mCharacters[0].mModel out of the loop; here more addresses are
// hoisted and the registers differ (four bytes fewer).
// @0x0214e9e8
void TushinScene::UpdateCharacters() {
    BOOL sent = FALSE;
    int i;
    for (i = 0; i < 3; i++) {
        if (i == 2 && mCharacters[i].mModel.IsVisible() == TRUE) {
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
                mUnknown3F084 = 1;
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
    if (FindResident(&sTransfer) != FindResident(&mResidents.mResidents[mResidents.mSelected])) {
        return;
    }
    BOOL show = FALSE;
    if (GetMode() == 1 && mUnknown3F0A8 == 4) {
        show = TRUE;
    }
    if (GetMode() == 2 && mUnknown3F0A8 == 0x37) {
        show = TRUE;
    }
    if (show != TRUE) {
        return;
    }
    ShowNextCharacter();
    StartEmitter(1);
}
#pragma optimization_level reset

// @0x0214ef08
void TushinScene::DrawCharacters() {
    switch (mPage) {
    case 0: case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8:
    case 38: case 39: case 40: case 41:
        mUnknown3F000 = FALSE;
        return;
    case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20:
    case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32:
    case 33: case 35: case 36: case 42: case 43: case 44: case 45: case 46: case 47: case 48: case 49: case 50:
    case 51: case 52: case 53: case 54: case 55: case 56: case 57: case 58: case 59: case 60: case 61: case 62:
    case 63: case 65: case 66:
        mUnknown3F000 = TRUE;
        break;
    case 34: case 37: case 64: case 67:
        if (mUnknown3F000 == FALSE) {
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
    mUnknown3F094 = 0;
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
    mUnknown3F094 = 1;
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
        if (mUnknown3F080 != 0) {
            return;
        }
        mUnknown3F080 = 1;
        gSoundManager.StartSeqArc(2, 0x19, 9);
        return;
    default:
        if (mUnknown3F080 != 1) {
            return;
        }
        mUnknown3F080 = 0;
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
        if (sWireless.m44 == -1) {
            return;
        }
        LoadSavedCharacter(2, sWireless.m44);
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
    mUnknown3F030 = 1;
    gSoundManager.StartSeqArc(1, 0x19, 7);
    mUnknown3F034 = 0;
}

// @0x0214f528
void TushinScene::CountFrame() {
    mUnknown3F034++;
}

// @0x0214f53c
void TushinScene::DrawParticles() {
    if (mUnknown3F034 > 600) {
        return;
    }
    func_0202ad84(data_021223e4);
    NNS_G3dGlbFlushP();
    func_0207b258(mParticles, data_02142438);
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
        func_0203b8b4(data_0213b700, 1);
        return;
    case 1:
    case 2:
        WH_Initialize();
        return;
    }
}

// @0x0214f788
void TushinScene::Leave() {
    SceneCheck check;
    func_020af224(&check, 0, 0);
    func_020af260(&check);
    if (func_020af4d8(&check, 0) != -1) {
        func_0203c02c(&gSceneManager, 0x5F);
    } else {
        func_0203c02c(&gSceneManager, 0x5E);
    }
}

#pragma optimization_level 2
// NOT MATCHING (optimization level 2): the selected resident's mIndex is read with folded offsets (see
// UpdateSending), mVisible of character 2's model is not read off the model's address, the parent timers are
// read through their own symbol instead of the .bss pool's base (the .bss is not in the ROM's order yet), and a
// few moves are scheduled otherwise (0x40 bytes fewer in all).
// @0x0214f7ec
void TushinScene::Update() {
    UpdateSignal();
    CountFrame();
    func_0203b800(data_0213b700);
    UpdateMusic();
    UpdateCharacters();
    switch (mPage) {
    case 0x29:
    case 0x30: case 0x31: case 0x32: case 0x33: case 0x34: case 0x35: case 0x36: case 0x37: case 0x38: case 0x39:
    case 0x3A: case 0x3B: case 0x3C: case 0x3D: case 0x3E: case 0x3F:
        if (func_02078f04() == 6) {
            break;
        }
        func_020777f4();
        break;
    default:
        func_020777f4();
        break;
    }
    func_020341d4(&mWindow);
    if (mWindow.mTransitionDone) {
        mUnknown3F0AC = 1;
    }
    if (mWait > 0) {
        mWait--;
        return;
    }

    int layout = sPageLayouts[mPage];
    if (sWireless.m28 != 0 && sWireless.m02 == 0) {
        sWireless.m02 = sWireless.m28;
        sWireless.m28 = 0;
    }
    if (sWireless.m02 != 0) {
        switch (sWireless.m02) {
        case 2:
        case 3:
            mUnknown3F08C = 2;
            sWireless.m02 = 0;
            if (mPage == 10) {
                mUnknown24 = 1;
            } else {
                return;
            }
            break;
        case 4:
            sWireless.m02 = 0;
            mUnknown3F088 = 1;
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
    if (mUnknown3F08C != 0 && mPage == 9) {
        if (mUnknown3F08C == 2) {
            mPage = 0xE;
        } else {
            mPage = 0xF;
        }
        ResetList();
        func_020a3634(&mList);
        func_02031228(&mButtons[0]);
        mUnknown3F08C = 0;
        Send(4);
        sWireless.m30 = 0;
        SetUpWindow(mPage);
        mWindow.mTexts[0].SetStringArg(sReceivedName, 0);
        func_02034f38(&mWindow, 0, sPageMessages[mPage]);
        func_02034720(&mWindow, 0x12, 1);
    }

    BOOL lost = FALSE;
    int next = 0x22;
    switch (GetMode()) {
    case 1:
        if (sWireless.m00 == 0) {
            if (WH_GetSystemState() != 9 && WH_GetSystemState() != 10) {
                break;
            }
            if (mNextPage == 0x22) {
                break;
            }
        }
        if (mPage == 10) {
            mUnknown28 = 1;
            break;
        }
        if (WH_GetSystemState() != 0) {
            WH_Finalize();
            func_0203b8b4(data_0213b700, 1);
        }
        sWireless.m00 = 0;
        lost = TRUE;
        next = 0x22;
        break;
    case 2:
        if (func_02078f04() == 0 && mPage != 0x26 && mPage != 0x27 && mPage != 0x42 && mPage != 0x30 &&
            mNextPage != 0x40 && mNextPage != 0x43 && mNextPage != 0x30) {
            if (mUnknown3F098 == 1) {
                mUnknown18 = 0x40;
                mNextPage = 0x30;
                CloseWindow();
                if (!func_0202c37c(&gGameData)) {
                    return;
                }
                if (!func_02073520(GetSelectedResident())) {
                    return;
                }
                func_020a0664(&mResidents);
                return;
            }
            if (mPage == 0x2B) {
                mUnknown28 = 1;
                break;
            }
            next = mPage != 0x28 ? 0x40 : 0x43;
            lost = TRUE;
            break;
        }
        if (mUnknown28 == 1) {
            mUnknown28 = 0;
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
        mUnknown28 = 0;
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
                    func_0203c02c(&gSceneManager, 0xB1);
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
                    func_0203b8b4(data_0213b700, 1);
                    sWireless.m4C = 0;
                    sWireless.m2C = 0;
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
                    func_0203b7e8(data_0213b700);
                    WH_SetReceiver(func_ov109_02151ee0);
                    break;
                case 1:
                    mNextPage = 4;
                    func_0203b7e8(data_0213b700);
                    func_0203b87c(data_0213b700);
                    WH_SetReceiver(func_ov109_02151ee0);
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
                    func_0203b8b4(data_0213b700, 1);
                }
                break;
            case 4:
                if (result == 3) {
                    mNextPage = 2;
                    close = 0x2C;
                    WH_Finalize();
                    func_0203b8b4(data_0213b700, 1);
                }
                break;
            case 5:
                if (result == 3) {
                    mNextPage = 2;
                    close = 0x2C;
                    WH_Finalize();
                    func_0203b8b4(data_0213b700, 1);
                } else if (result >= 0 && result < 3 && sParentTimers[result] > 0) {
                    func_0203bac4(data_0213b700, result);
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
                    if (func_02073520(GetSelectedResident()) == TRUE) {
                        mNextPage = 0xC;
                    } else if (func_ov109_02151d74(GetSelectedResident())) {
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
                    if (func_ov109_02151d74(GetSelectedResident()) == TRUE) {
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
                if (mUnknown3F09C == 1) {
                    mUnknown3F09C = 0;
                    mNextPage = 0x1F;
                    break;
                }
                if (mUnknown3F0A0 == 1) {
                    mUnknown3F0A0 = 0;
                    if (GiveShirt() == TRUE) {
                        mNextPage = 0x20;
                        break;
                    }
                }
                mNextPage = 9;
                close = 0x14;
                if (mPage == 0x11 && mUnknown3F094 == 1) {
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
                func_02078d48();
                break;
            case 39:
                if (result == 3) {
                    mNextPage = 1;
                    close = 0x2C;
                    func_02078d48();
                } else if (result >= 0 && result < 3 && sParentTimers[result] > 0) {
                    data_0213b700->mParent = result;
                    mUnknown3F0B0 = 1;
                    mNextPage = 0x28;
                }
                break;
            case 41:
                mNextPage = 0x2A;
                close = 0x14;
                break;
            case 44:
                if (result == 0) {
                    if (sWireless.m26 != 0) {
                        break;
                    }
                    if (func_02073520(GetSelectedResident()) == TRUE) {
                        mNextPage = 0x2D;
                    } else if (func_ov109_02151d74(GetSelectedResident()) == TRUE) {
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
                    if (func_ov109_02151d74(GetSelectedResident()) == TRUE) {
                        mNextPage = 0x3F;
                    } else {
                        mNextPage = 0x2E;
                        func_0202c328(&gGameData, mResidents.GetSelected()->mIndex, 2);
                        func_0202d110(&gGameData, mResidents.GetSelected()->mIndex);
                        StartReceiveAnimation();
                        SetUnknown3F098();
                    }
                } else {
                    mNextPage = 0x2A;
                    close = 0x14;
                }
                break;
            case 49: case 50: case 51: case 52: case 53: case 54: case 55: case 56: case 57: case 58: case 59:
            case 60: case 61: case 62: case 63:
                if (mUnknown3F0A4 == 1) {
                    mUnknown3F0A4 = 0;
                    if (GiveWiiShirt() == TRUE) {
                        mNextPage = 0x3D;
                        break;
                    }
                }
                mNextPage = 0x2A;
                close = 0x14;
                if (mPage == 0x31 && mUnknown3F094 == 1) {
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
                if (func_0203b9c4(data_0213b700) <= 0) {
                    break;
                }
                mNextPage = 5;
                CloseWindow();
                break;
            case 11:
            case 12:
            case 35:
                if (mUnknown3F08C == 0 || mNextPage == 0xD) {
                    break;
                }
                if (mUnknown3F08C == 2) {
                    mNextPage = 0xE;
                } else {
                    mNextPage = 0xF;
                }
                CloseWindow();
                break;
            case 13:
                if (mUnknown3F084 != 0) {
                    break;
                }
                if (!IsWindowOpen(&mWindow)) {
                    break;
                }
                if (sWireless.m22 != 0 || sWireless.m24 != 0) {
                    break;
                }
                if (mCharacters[2].mModel.IsVisible()) {
                    break;
                }
                switch (sWireless.m02) {
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
                if (mUnknown3F098 == 1) {
                    mUnknown18 = mNextPage;
                    mNextPage = 0x10;
                }
                sWireless.m02 = 0;
                CloseWindow();
                func_020a05dc(&mResidents);
                func_020a123c(&mResidents, mResidents.mUnknown11306, 0);
                break;
            case 14:
            case 15: {
                if (!IsWindowOpen(&mWindow)) {
                    break;
                }
                if (sWireless.m26 <= 5 || sWireless.m04 <= 2) {
                    break;
                }
                BOOL done = FALSE;
                if (sWireless.m30 == 1) {
                    sWireless.m30 = 0;
                    mUnknown3F0A8 = ReceiveCharacter();
                    func_020a05dc(&mResidents);
                    func_020a123c(&mResidents, mResidents.mUnknown11306, 0);
                    mNextPage = 0x17;
                    if (mUnknown3F0A8 != 0) {
                        sWireless.m26 = 0;
                        sWireless.m04 = 0;
                        switch (mUnknown3F0A8) {
                        case 3:
                            mNextPage = 0x18;
                            Send(5);
                            mUnknown3F0A0 = 1;
                            ShowReceived();
                            break;
                        case 4:
                            mNextPage = 0x19;
                            Send(5);
                            mUnknown3F0A0 = 1;
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
                        if (func_02073520(&sTransfer) != TRUE) {
                            break;
                        }
                    }
                    mUnknown3F0A0 = 1;
                    ShowReceived();
                    if (func_02073520(&sTransfer) == TRUE) {
                        mUnknown18 = mNextPage;
                        mNextPage = 0x10;
                        CloseWindow();
                        SetUnknown3F098();
                        done = TRUE;
                    }
                    Send(5);
                    if (done == TRUE) {
                        break;
                    }
                }
                sWireless.m26 = 0;
                sWireless.m04 = 0;
                CloseWindow();
                break;
            }
            case 16:
                if (!IsWindowOpen(&mWindow)) {
                    break;
                }
                if (mUnknown3F098 == 1) {
                    mWait = 60;
                    Save();
                    mUnknown3F098 = 0;
                    break;
                }
                if (func_0202cd38(&gGameData)) {
                    break;
                }
                sWireless.m26 = 0;
                sWireless.m04 = 0;
                mNextPage = mUnknown18;
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
                if (func_02078f04() != 1) {
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
                if (mUnknown3F0B0 != 1) {
                    break;
                }
                func_02078da4(mParentIds[data_0213b700->mParent]);
                mUnknown3F0B0 = 0;
                break;
            case 66:
                if (func_02078f04() == 0) {
                    mNextPage = 0;
                    CloseWindow();
                    ResetExchange();
                }
                // fall through
            case 44:
            case 45:
            case 65:
                if (sWireless.m26 == 0) {
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
                if (sWireless.m40 != 1) {
                    break;
                }
                if (sWireless.m48 == 0) {
                    break;
                }
                int sent = func_02073520(GetSelectedResident());
                mNextPage = 0x31;
                switch (sWireless.m48) {
                case 1:
                    if (sent) {
                        func_020a0664(&mResidents);
                    }
                    break;
                case 2:
                    mNextPage = 0x32;
                    CloseWindow();
                    sWireless.m40 = 0;
                    func_0202c328(&gGameData, mResidents.GetSelected()->mIndex, 1);
                    SetOtherStandAnimation();
                    break;
                case -5:
                    mNextPage = 0x33;
                    CloseWindow();
                    sWireless.m40 = 0;
                    return;
                case -4:
                    mNextPage = 0x34;
                    CloseWindow();
                    sWireless.m40 = 0;
                    func_0202c328(&gGameData, mResidents.GetSelected()->mIndex, 1);
                    SetOtherStandAnimation();
                    break;
                }
                if (sent) {
                    SetUnknown3F098();
                    mUnknown18 = mNextPage;
                    mNextPage = 0x30;
                }
                CloseWindow();
                sWireless.m48 = 0;
                break;
            }
            case 48:
                if (!IsWindowOpen(&mWindow)) {
                    break;
                }
                if (mUnknown3F098 == 1) {
                    mWait = 60;
                    Save();
                    mUnknown3F098 = 0;
                    break;
                }
                if (func_0202cd38(&gGameData)) {
                    break;
                }
                mNextPage = mUnknown18;
                CloseWindow();
                sWireless.m40 = 0;
                func_020a05dc(&mResidents);
                func_020a123c(&mResidents, mResidents.mUnknown11306, 0);
                break;
            case 53: case 54: case 55: case 56: case 57: case 58: case 59: case 60: case 62: case 63:
                if (!IsWindowOpen(&mWindow) || sWireless.m26 == 0) {
                    break;
                }
                sWireless.m26 = 0;
                func_020a05dc(&mResidents);
                func_020a123c(&mResidents, mResidents.mUnknown11306, 0);
                if (mPage == 0x35 || mPage == 0x36 || mPage == 0x37) {
                    ShowReceived();
                    mUnknown3F0A4 = 1;
                }
                break;
            }
        }

        if (mUnknown3F0AC == 1) {
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
                    func_0203b940(data_0213b700, 1);
                    break;
                case 4:
                    func_0203b900(data_0213b700, 1);
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
                    if (mUnknown3F08C != 0) {
                        if (mUnknown3F090 == 0) {
                            mNextPage = mUnknown3F08C == 2 ? 0xE : 0xF;
                            mUnknown3F08C = 0;
                            Send(4);
                            sWireless.m30 = 0;
                            break;
                        }
                        mUnknown3F08C = 0;
                    }
                    switch (mPage) {
                    case 0xB:
                        StartSendAnimation();
                        break;
                    case 0xC:
                        func_0202c328(&gGameData, mResidents.GetSelected()->mIndex, 2);
                        func_0202d110(&gGameData, mResidents.GetSelected()->mIndex);
                        StartReceiveAnimation();
                        break;
                    }
                    break;
                case 14:
                case 15:
                    sWireless.m30 = 0;
                    mUnknown24 = 0;
                    if (mPage != 0xB && mPage != 0xC && mPage != 0x23) {
                        break;
                    }
                    if (mUnknown3F08C == 0) {
                        break;
                    }
                    mUnknown3F08C = 0;
                    Send(4);
                    break;
                case 16:
                    mWait = 60;
                    PlaySound(0xC);
                    break;
                case 8:
                    mWindow.mTexts[0].SetStringArg(mParentNames[data_0213b700->mParent], 0);
                    PlaySound(0xB);
                    mUnknown3F090 = 0;
                    break;
                case 17:
                case 49:
                    PlaySound(0xB);
                    break;
                case 18: case 19: case 20: case 21: case 25: case 26: case 27: case 29: case 31:
                    PlaySound(0xC);
                    break;
                case 22: case 24: case 28: case 30: case 33:
                    PlaySound(0xD);
                    break;
                case 23:
                    mWindow.mTexts[0].SetStringArg(sReceivedName, 0);
                    PlaySound(0xB);
                    break;
                case 32:
                case 61:
                    PlaySound(0xC);
                    break;
                case 6:
                    func_0203b968(data_0213b700, 1);
                    mWait = 60;
                    break;
                case 36:
                    WH_Finalize();
                    func_0203b8b4(data_0213b700, 1);
                    mWait = 60;
                    break;
                case 37:
                    func_0203b8b4(data_0213b700, 1);
                    PlaySound(0xD);
                    break;
                case 40:
                    mWait = 60;
                    break;
                case 41:
                    mWindow.mTexts[0].SetStringArg(mParentNames[data_0213b700->mParent], 0);
                    PlaySound(0xB);
                    break;
                case 46:
                case 47:
                    sWireless.m40 = 0;
                    break;
                case 53:
                    PlaySound(0xB);
                    break;
                case 66: {
                    int state = func_02078f04();
                    if (state != 7 && state != 10) {
                        func_02078d48();
                    }
                    mWait = 60;
                    break;
                }
                case 34:
                    PlaySound(0xD);
                    // fall through
                case 64:
                    mUnknown28 = 0;
                    break;
                case 38:
                    for (int i = 0; i < 3; i++) {
                        sParentTimers[i] = 0;
                        mParentIds[i] = 0;
                    }
                    ClearParentNames();
                    func_02078d04();
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
        mUnknown3F0AC = 0;
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
    mUnknown1C += 0x4CC;
    func_02028108(&data_02121834, 2, mUnknown1C >> FX32_SHIFT);
}

// @0x02151620
void TushinScene::Exit() {
    if (GetMode() == 1 && WH_GetSystemState() != 0 && data_0213b700->mUnknown00 == 0) {
        WH_Finalize();
        func_0203b8b4(data_0213b700, 1);
    }
    if (mUnknown35920 != mResidents.mUnknown11306) {
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
        func_0203b8b4(data_0213b700, 1);
        while (func_0203b800(data_0213b700) == 1) {
        }
        return;
    }
    if (func_02078f04() == 0) {
        return;
    }
    do {
        func_020777f4();
    } while (func_02078f04() != 0);
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
    func_0207a8c4(record, nickname);
    if (IsNgWord(nickname) == TRUE) {
        return TRUE;
    }
    wchar_t name[11];
    MI_CpuClear16(name, sizeof(name));
    func_0207a80c(record, name);
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
    return func_020733a4(buffer) ? TRUE : FALSE;
}

// @0x02151c8c
BOOL TushinScene::IsNameBlank(CharacterRecord *record) {
    wchar_t name[11];
    MI_CpuClear16(name, sizeof(name));
    func_0207a80c(record, name);
    return IsBlank(name) == TRUE ? TRUE : FALSE;
}

// @0x02151cd8
BOOL TushinScene::IsNicknameBlank(CharacterRecord *record) {
    wchar_t nickname[11];
    MI_CpuClear16(nickname, sizeof(nickname));
    func_0207a8c4(record, nickname);
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

// @0x02151d74
BOOL TushinScene::func_ov109_02151d74(CharacterRecord *record) {
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
    WM_SetEntry(func_ov109_0215245c, enabled);
}

// @0x02151e5c
void TushinScene::ClearParentNames() {
    MI_CpuFill8(mParentNames, 0, sizeof(mParentNames));
}

// @0x02151e78
void TushinScene::Save() {
    if (!func_0202cd48(&gGameData)) {
        func_0203c02c(&gSceneManager, 0xC2);
    }
}

// @0x02151ea4
void TushinScene::SetUnknown3F098() {
    mUnknown3F098 = 1;
}

// @0x02151eb4
extern "C" void func_ov109_02151eb4(BOOL result) {
    switch (WH_GetSystemState()) {
    case 0:
    case 9:
    case 10:
        return;
    }
    sWireless.m3C = 1;
}

// @0x02151ee0
extern "C" void func_ov109_02151ee0(u16 aid, u16 *data, u16 size) {
    switch (size) {
    case 0:
        sWireless.m00 = 1;
        return;
    case 2:
        if (sWireless.m02 == 0) {
            sWireless.m02 = *data;
        } else {
            sWireless.m28 = *data;
        }
        return;
    }
    if (sWireless.m26 == 0) {
        func_020979e0(&sTransfer);
    }
    if (sWireless.m26 >= 6) {
        MI_CpuCopy8(data, (u8 *)sSongs + sWireless.m04 * 0x80, 0x80);
        if (sWireless.m04 < 2) {
            sWireless.m04++;
        } else {
            sWireless.m04++;
            sWireless.m30 = 1;
        }
    } else {
        MI_CpuCopy8(data, (u8 *)&sTransfer + sWireless.m26 * 0x80, 0x80);
        if (sWireless.m26 < 5) {
            ++sWireless.m26;
        } else {
            sWireless.m26++;
        }
    }
}

// @0x02151fcc
extern "C" void func_ov109_02151fcc(int a, int b) {
    sWireless.m48 = a;
    sWireless.m40 = 1;
}

// @0x02151fe4
int TushinScene::NextPageAfterReceive() {
    int page = ReceiveWiiMii();
    mUnknown3F0A8 = page;
    if (page != 0x35) {
        return page;
    }
    if (mUnknown3F098 == 1) {
        mUnknown18 = page;
        page = 0x30;
    }
    return page;
}

// NOT MATCHING: the original derives the -1 for sWireless.m44 from a register holding 0 (mov r2, #0; sub r1, r2,
// #1), and sets that 0 earlier (five instructions).
// @0x02152014
int TushinScene::ReceiveWiiMii() {
    SavedCharacter character;
    sWireless.m44 = -1;
    int result = sWireless.m34;
    sWireless.m34 = 0;
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
    MI_CpuCopy8(&sMiiData[sWireless.m38], &sTransfer, sizeof(MiiData));
    if (IsInvalid((CharacterRecord *)&sMiiData[sWireless.m38])) {
        UpdateMiiData();
        PlayResultSound(0xD);
        return 0x3C;
    }
    func_020979e0(&character);
    MI_CpuCopy8(&sMiiData[sWireless.m38], &character, sizeof(MiiData));
    if (HasNgWord(&character) == TRUE) {
        UpdateMiiData();
        PlayResultSound(0xD);
        return 0x3F;
    }
    sWireless.m44 = FindResident((CharacterRecord *)&sMiiData[sWireless.m38]);
    if (sWireless.m44 != -1) {
        func_020979e0(&character);
        func_0202c264(&gGameData, sWireless.m44, &character);
        if (func_02073520(&character)) {
            PlayResultSound(0xD);
            return 0x38;
        }
        if (func_02098584(&character)) {
            PlayResultSound(0xD);
            return 0x36;
        }
        MI_CpuCopy8(&sMiiData[sWireless.m38], &character, sizeof(MiiData));
        if (func_0207a710(&character) != func_0202c604(&gGameData, sWireless.m44)) {
            PlayResultSound(0xC);
            return 0x39;
        }
        func_0202c28c(&gGameData, sWireless.m44, &character);
        func_0207a80c(&character, sReceivedName);
        UpdateMiiData();
        PlayResultSound(0xC);
        return 0x37;
    }
    for (int i = 0; i < 100; i++) {
        if (!func_0202c354(&gGameData, i)) {
            sWireless.m44 = i;
            func_020979e0(&character);
            MI_CpuCopy8(&sMiiData[sWireless.m38], &character, sizeof(MiiData));
            func_02097c64(&character);
            func_0202c28c(&gGameData, i, &character);
            func_0207a80c(&character, sReceivedName);
            if (func_02073520(&character)) {
                SetUnknown3F098();
            }
            UpdateMiiData();
            return 0x35;
        }
    }
    return 0x3A;
}

// @0x021523a4
extern "C" void func_ov109_021523a4(int index, int result) {
    sWireless.m34 = result;
    switch (result) {
    case 1:
        sWireless.m38 = (u8)index;
        sWireless.m26++;
        break;
    case 2:
        sWireless.m38 = (u8)index;
        sWireless.m26++;
        break;
    case 3:
        sWireless.m38 = (u8)index;
        sWireless.m26++;
        break;
    case -5:
    case -4:
        sWireless.m26++;
        break;
    case -2:
        sWireless.m26++;
        break;
    case -3:
        sWireless.m26++;
        break;
    }
}

// @0x0215245c
extern "C" void func_ov109_0215245c(void *arg) {
}

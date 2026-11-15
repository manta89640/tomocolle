#include "TushinScene.h"

// Overlay 109: wireless play (TushinScene). One source file.

// --- The file's constants ---

static const u8 sYesNoSequences[2] = { 0x74, 0x74 };
static const u8 sYesNoSequences2[2] = { 0x75, 0x75 };

static const WindowPoint sTextPosition = { 24, 12 };
static const WindowPoint sButtonSize2 = { 190, 26 };
static const WindowPoint sBackButtonSize = { 94, 30 };
static const u32 sYesNoMessages[2] = { 2, 3 };
static const WindowPoint sButtonSize3 = { 190, 20 };
static const WindowPoint sYesNoPositions[2] = { { 75, 149 }, { 181, 149 } };
static const WindowPoint sYesNoSizes[2] = { { 94, 30 }, { 94, 30 } };
static const ScreenRect sYesNoWindowRect = { 20, 20, 216, 152 };
static const ScreenRect sWindowRect = { 20, 20, 216, 152 };
static const WindowButtonStyle sButtonStyle = { 10, -1, 0, -1 };
static const WindowPoint sFirstPageIconPositions[2] = { { 204, 74 }, { 204, 106 } };
static const WindowPoint sButtonTextPositions2[3] = { { 38, 68 }, { 38, 100 }, { 38, 118 } };
static const WindowPoint sTextMargins[3] = { { 32, 33 }, { 32, 50 }, { 32, 33 } };
static const WindowPoint sButtonTextPositions3[3] = { { 38, 66 }, { 38, 90 }, { 38, 114 } };
static const WindowPoint sButtonPositions2[4] = { { 128, 74 }, { 128, 106 }, { 128, 124 }, { 128, 149 } };
static const WindowPoint sButtonPositions3[4] = { { 128, 72 }, { 128, 96 }, { 128, 120 }, { 128, 149 } };
static const WindowButtonStyle sYesNoStyles[2] = { { 10, -1, 0, -1 }, { 12, -1, 0, -1 } };

static const WindowPoint sListTitleSize = { 228, 18 };
static const WindowPoint sListTitleTextPos = { 19, 41 };
static const WindowPoint sListScrollBarSize = { 14, 12 };
static const WindowPoint sListScrollUpPos = { 244, 150 };
static const WindowPoint sListScrollDownSize = { 14, 12 };
static const WindowPoint sListTextPosition = { 8, 2 };
static const WindowPoint sListScrollDownPos = { 244, 44 };
static const WindowPoint sListCursorPos = { 12, 47 };
static const WindowPoint sListTitlePos = { 119, 47 };
static const WindowPoint sListBarPos = { 244, 97 };
static const u32 sListButtonMessages[3] = { (u32)-1, (u32)-1, 0x105 };
static const int sListButtonFonts[3] = { 0, -1, 2 };
static const int sListButtonFiles[3] = { 1, 1, 0 };
static const VecFx32 sEmitterPosition0 = { 0x1000, 0, 0 };
static const VecFx32 sCameraPosition = { 0, 0x24CC, 0xC000 };
static const VecFx32 sCameraTarget = { 0, 0x1CCC, 0 };
static const VecFx32 sEmitterPosition1 = { -0x1000, 0, 0 };
static const ScreenRect sListWindowRect = { 20, 12, 216, 168 };
static const ScreenRect sListBarRect = { -8, -8, 16, 16 };
static const WindowPoint sListButtonPositions[3] = { { 213, 19 }, { 242, 13 }, { 128, 173 } };
static const WindowPoint sListButtonTextPositions[3] = { { 26, 34 }, { 24, 22 }, { 116, 30 } };
static const int sListUnknown[7] = { 0, 1, 2, 3, 4, 5, 6 };
static const u32 sListMessages[7] = { 127, 128, 129, 130, 131, 132, 133 };
static const WindowButtonStyle sListButtonStyles[3] = { { 10, -1, 0, -1 }, { 12, -1, 0, -1 }, { 10, -1, 0, -1 } };
static const WindowPoint sListItemPositions[7] = {
    { 128, 30 }, { 128, 52 }, { 128, 74 }, { 128, 96 }, { 128, 118 }, { 128, 140 }, { 128, 162 },
};
static const WindowPoint sListItemSizes[7] = {
    { 128, 20 }, { 128, 20 }, { 128, 20 }, { 128, 20 }, { 128, 20 }, { 128, 20 }, { 128, 20 },
};

static const u8 sListButtonSequences2[3] = { 0x7C, 0x01, 0x77 };
static const u8 sListButtonSequences[3] = { 0x7B, 0x00, 0x76 };
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
static WindowPoint sListTextMargins[7] = { { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } };

static const int sPageLayouts[68] = {
    0, 1, 2, 3, 3, 4, 6, 7, 7, -1, -1, 5, 5, 6, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
    5, 6, 7, 3, 4, 6, 7, -1, -1, 5, 5, 6, 6, 6, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 5, 6, 7,
};

// The Mii data of the residents, as they are sent (all 100 saved characters, empty ones cleared)
static u8 sMiiData[100][0x4A];
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
static u8 sUnknown02153234[0xC];
static wchar_t sReceivedName[21];
static u8 sSongs[6][0x40]; // The songs sent with a character
static u8 sUnknown02153400[0x240];


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
    mWindow.mTexts[0].mVAlign = sLayouts[layout].mHasButton[0] == FALSE ? 1 : 0;

    if (sLayouts[layout].mHasButton[2]) {
        WindowPoint size = sButtonSize3;
        for (int i = 0; i < 3; i++) {
            if (sLayouts[layout].mHasButton[i]) {
                func_02033448(&mWindow, i, 0, 0x6E, 0x6F, sButtonPositions3[i], size, &sButtonStyle, 1);
                func_02033a58(&mWindow, &mMessageFiles[0], sLayoutMessages[layout][i], &gFonts[0].mFont, 0x1000,
                              sButtonTextPositions3[i], &charOffset);
            }
        }
    } else {
        WindowPoint size = sButtonSize2;
        for (int i = 0; i < 3; i++) {
            if (sLayouts[layout].mHasButton[i]) {
                func_02033448(&mWindow, i, 0, 0x6C, 0x6D, sButtonPositions2[i], size, &sButtonStyle, 1);
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
    mUnknown35920 = gGameData.mUnknown4D;
    func_020a123c(&mResidents, mUnknown35920, 1);
    mResidents.mUnknown1139C = 1;

    for (int i = 0; i < 3; i++) {
        Sprite_Init(&mButtons[i].mSprite, 0, sListButtonSequences[i], 0x80, 0x60, TRUE);
        func_02031260(&mButtons[i], 4);
        func_02031278(&mButtons[i], sListButtonSequences[i], sListButtonSequences2[i]);
        mButtons[i].mSprite.SetPosition(sListButtonPositions[i].x << FX32_SHIFT,
                                        sListButtonPositions[i].y << FX32_SHIFT);
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
    WindowPoint position = sListTextPosition;
    for (int i = 0; i < 7; i++) {
        func_020338d0(&mListWindow, &mMessageFiles[0], sListMessages[i], &gFonts[0].mFont, 0x1000, position,
                      sListTextMargins[i], charOffset);
    }
    func_02034f4c(&mListWindow, 1);
}

// @0x0214a68c
void TushinScene::InitCharacters() {
    func_0202b0e8(&data_02122824);
    for (int i = 2; i >= 0; i--) {
        SavedCharacter *resident = &mResidents.mResidents[mResidents.mSelected];
        CharacterRecord *record = &mCharacters[i];
        *record = *resident;
        func_02091048(&mCharacters[i], sCharacterArchives[i]);
        ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[i].mModel.mArchiveName, mCharacters[i].mModel.mArchivePath,
                                       1);
        func_0202ad04(data_021223e4, i, mCharacters[i].mModel.mArchiveName, 0);
    }
    void *animations = func_0202ad04(data_021223e4, 0x1F, "BIC", 0);
    G3X_SetClearColor(GX_RGB(21, 21, 21), 0, 0x7FFF, 63, FALSE);
    for (int i = 0; i < 3; i++) {
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
    func_0202a640(&data_02122764);
    mCamera->mPosition = sCameraPosition;
    mCamera->mTarget = sCameraTarget;
    func_0202a96c(&data_021227cc[0]);
    func_0202a96c(&data_021227cc[1]);
    func_0202ab2c(&data_021227cc[0], mCamera->mAngleX, mCamera->mAngleY, mCamera->mAngleZ, 0x640, 0x3E8 - 0x640,
                  -0xFA0);
    func_0202ab2c(&data_021227cc[1], mCamera->mAngleX, mCamera->mAngleY, mCamera->mAngleZ, 0, -0xFA0, 0x5DC);
    LoadCharacters();
}

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
            MI_CpuCopy8(&record, sMiiData[i], sizeof(sMiiData[i]));
        } else {
            MI_CpuFill8(sMiiData[i], 0, sizeof(sMiiData[i]));
            func_02073574(sMiiData[i]);
        }
    }
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
    CharacterModel *model;
    func_0202aecc(data_021223e4, i);
    model = &mCharacters[i].mModel;
    ArchiveManager_Unmount(&gArchiveManager, model->mArchiveName);
    func_02091048(&mCharacters[i], sCharacterArchives[i]);
    ArchiveManager_MountCompressed(&gArchiveManager, model->mArchiveName, model->mArchivePath, 1);
    func_0202ad04(data_021223e4, i, model->mArchiveName, 0);
    func_0208ef5c(model, i, 0);
}

// @0x0214b590
void TushinScene::LoadCharacters() {
    for (int i = 0; i < 3; i++) {
        LoadCharacter(i);
    }
}

// @0x0214b5b8
void TushinScene::ShowNextCharacter() {
    CharacterRecord *record = &mCharacters[mCurrent];
    SavedCharacter *resident = &mResidents.mResidents[mResidents.mSelected];
    *record = *resident;
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
    mSprite.SetPosition(0xF4000, 0xB6000);
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
    func_0203b7d4(data_0213b700, sUnknown02153400, 3);
    func_0203b7e0(data_0213b700, sUnknown02153234);
    func_0203b76c(data_0213b700, 0);
    MI_CpuFill8(sUnknown02153400, 0, sizeof(sUnknown02153400));
    MI_CpuFill8(sUnknown02153234, 0, sizeof(sUnknown02153234));
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
    func_ov109_02151e5c();
    mUnknown3F0B0 = 0;
    for (int i = 0; i < 12; i++) {
        data_02138fb4[i] = 0;
    }
    mUnknown3F084 = 0;
}

// @0x0214f088
int TushinScene::GetStandAnimation(int i) {
    return data_020fb870[3][0];
}

// @0x0214c84c
void TushinScene::SendSongs() {
    int i;
    int n;
    if (sWireless.m24 != 0) {
        return;
    }
    sWireless.m24 = 1;
    int offset = mResidents.mSelected * sizeof(SavedCharacter);
    n = 0;
    MI_CpuClear16(sSongs, sizeof(sSongs));
    for (i = 0; i < 6; i++) {
        int song = func_0209c718((SavedCharacter *)((u8 *)mResidents.mResidents + offset), i);
        if (song != 0xFF) {
            func_0202c920(&gGameData, song, sSongs[n]);
            n++;
        }
    }
    WH_SendData(sSongs, 0x80, func_ov109_02151eb4);
    sWireless.m3C = 0;
}

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
        CharacterRecord *record = &sTransfer;
        *record = *GetSelectedResident();
        WH_SendData(&sTransfer, 0x80, func_ov109_02151eb4);
        sWireless.m3C = 0;
        return;
    case 1:
        SendSongs();
        return;
    default:
        WH_SendData(&sWireless.m20, 2, func_ov109_02151eb4);
        return;
    }
}

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
            case 8:
                sWireless.m24 = 0;
                func_0202c328(&gGameData, mResidents.GetSelected()->mIndex, 1);
                func_ov109_0214f0e4();
                break;
            default:
                return;
            }
            if (result == 1) {
                func_ov109_02151ea4();
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
    CharacterRecord *copy = &character;
    *copy = *record;
    func_02097cc0(&character);
    func_0209e01c(&character);
    *record = character;
}

// @0x0214d978
int TushinScene::ReceiveCharacter() {
    if (func_ov109_02151d24(&sTransfer)) {
        return 7;
    }
    if (func_ov109_02151a90(&sTransfer) == TRUE) {
        return 7;
    }
    if (func_ov109_02151b98() == TRUE) {
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
            record.mUnknown299 |= sTransfer.mUnknown299;
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
                    u8 saved = func_0209c718(&sTransfer, i);
                    func_020af084(&entry, saved);
                    func_020af0b8(&entry, -1, i);
                    func_020af06c(&entry, saved);
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
    for (int i = 0; i < 100; i++) {
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
                    u8 saved = func_0209c718(&sTransfer, j);
                    func_020af084(&entry, saved);
                    func_020af0b8(&entry, -1, j);
                    func_020af06c(&entry, saved);
                }
                n++;
                func_0202c28c(&gGameData, i, &sTransfer);
            }
            func_0202c28c(&gGameData, i, &sTransfer);
            func_0207a80c(&sTransfer, sReceivedName);
            return 0;
        }
    }
    return func_02073520(&sTransfer) ? 2 : 1;
}

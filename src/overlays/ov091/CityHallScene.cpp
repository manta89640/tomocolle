#include <nitro.h>
#include <nnsys.h>

#include "sound/SoundManager.h"

#include "CityHallScene.h"
#include "MainProgram.h"

// Overlay 91: the city hall (CityHallScene). One source file: its constants are one pool at the start of .rodata,
// its strings the .data after the scene's virtual table.

struct TextPosition {
    int x;
    int y;
};

// The file's constants. Their order decides where the compiler puts them (see docs/notes/ov098.md).
static const u16 sTextButtonSequences[1] = {17};
static const u32 sTextButtonMessages[1] = {0x147};
static const TextPosition sTextButtonSizes[1] = {{238, 30}};
static const TextPosition sTextButtonPositions[1] = {{128, 170}};
static const WindowPoint sWindowButtonSize = {94, 30};
static const WindowPoint sWindowTextPosition = {38, 33};
static const WindowPoint sWindowButtonPosition = {128, 149};
static const VecFx32 sCameraPosition = {0, 0x3666, 0xA000};
static const VecFx32 sLightDirection1 = {0, -0xFA0, 0x5DC};
static const VecFx32 sCameraTarget = {0, 0x1800, 0};
static const VecFx32 sLightDirection0 = {0x640, -0x258, -0xFA0};
static const u16 sButtonSequences[7] = {1, 5, 7, 9, 11, 13, 15};
static const ScreenRect sWindowRect = {20, 20, 216, 152};
static const ButtonSounds sWindowButtonSounds = {{10, 1, 0, -1}};
static const ButtonSounds sTextButtonSounds[1] = {{{10, -1, 0, -1}}};
static const int sTextFonts[8] = {2, 2, 2, 2, 2, 2, 2, 2};
// The scene each button leads to: -1 for the first, which goes to the island map (0x5F) by itself; then the Mii
// editor (0x43, while the island has room), the friend list (0x1A), scene 0x17, the album (0xB4), the save scene
// (0x2D) and the options (0xAC). The last is the text button's (0x2C), read with a constant index and so folded.
static const int sButtonScenes[8] = {-1, 0x43, 0x1A, 0x17, 0xB4, 0x2D, 0xAC, 0x2C};
static const u32 sTextMessages[8] = {0x13F, 0x140, 0x141, 0x142, 0x143, 0x144, 0x145, 0x146};
static const TextPosition sButtonPositions[7] = {
    {242, 13}, {46, 57}, {128, 57}, {210, 57}, {46, 120}, {128, 120}, {210, 120},
};
static const TextPosition sButtonSizes[7] = {
    {24, 22}, {74, 60}, {74, 60}, {74, 60}, {74, 60}, {74, 60}, {74, 60},
};
static const char sWindowScreenName[] = "N83:/Window_down.nsc";
static const TextPosition sTextPositions[8] = {
    {46, 63}, {46, 76}, {128, 63}, {128, 76}, {210, 76}, {46, 139}, {128, 139}, {210, 139},
};
static const ButtonSounds sButtonSounds[7] = {
    {{13, 1, 0, -1}},  {{10, -1, 0, -1}}, {{10, -1, 0, -1}}, {{10, -1, 0, -1}},
    {{10, -1, 0, -1}}, {{10, -1, 0, -1}}, {{10, -1, 0, -1}},
};

// Puts a text at a position in pixels
static inline void SetTextPosition(MessageText *text, TextPosition position) {
    text->SetPosition(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
}

// Moves a sprite by an offset, built as an FxVec2 and passed by value (which leaves its dead copies on the stack)
static inline void MoveSprite(Sprite *sprite, fx32 x, fx32 y) {
    NNSG2dFVec2 offset = Sprite::MakeVec(x, y);
    sprite->mPosition.x += offset.x;
    sprite->mPosition.y += offset.y;
}

void CityHallScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "N83", "Scn/Scene/SceneCityHall_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TET", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TEC", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "M00", "Mdl/Misc/cityhall_3D_LZ.bin", 0);
    // Back from the options (0xAC), the information (0xAE) or the save scene (0x2C) the music is the scene's own
    int previous = gSceneManager.mPreviousSceneId;
    if (previous == 0xAC || previous == 0xAE || previous == 0x2C) {
        gSoundManager.StartSeqIfNotPlaying(0, 0x60);
        gSoundManager.SetSeqTrackMuteOnly(0, 0xF020, TRUE);
    }
}

void CityHallScene::Init() {
    gSoundManager.StartSeqIfNotPlaying(0, 0x60);
    gSoundManager.SetSeqTrackMuteOnly(0, 0xF020, TRUE);
    int charOffset = 800;
    mMessageFiles[0].Open("TET:/tmd_etc_msg_JP.bmg");
    mMessageFiles[1].Open("TEC:/tmd_common_msg_JP.bmg");

    VramManagers_Reset(&gVramManagerWork);
    gScreenControl.mUnknown48 = 1;
    gScreenControl.mUnknown48Changed = TRUE;
    G3X_SetClearColor(GX_RGB(15, 15, 15), 0, 0x7FFF, 63, FALSE);
    Camera *const camera = &gCamera;
    Camera_Reset(camera);
    camera->mPosition = sCameraPosition;
    camera->mTarget = sCameraTarget;
    camera->mAngleX = 0;
    camera->mAngleY = 0;
    camera->mAngleZ = 0;
    Light *const light0 = &gLights[0];
    Light_Reset(light0);
    Light_SetDirectionByCamera(light0, camera, sLightDirection0);
    Light *const light1 = &gLight1;
    Light_Reset(light1);
    Light_SetDirectionByCamera(light1, camera, sLightDirection1);
    ModelResources_SetModel(gModelResources, 1, "M00:/cityhall.imd");
    mModel.Load(1, 0);
    mScrollX = 0;

    PaletteLoader_LoadFile(&gPaletteLoader, "N83:/BG_up.ncl", 2, -1, -1, -1);
    ScreenLoader_LoadFile(&gScreenLoader, "N83:/BG_up.nsc", 2, 0, 0, 0, 0, -1);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "N83:/BG_up.ncg", 2, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "N83:/BG_down.ncl", 0x100, -1, -1, -1);
    ScreenLoader_LoadFile(&gScreenLoader, "N83:/BG_down.nsc", 0x100, 0, 0, 0, 0, -1);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "N83:/BG_down.ncg", 0x100, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "N83:/Obj_down.ncl", 0x1000, -1, -1, -1);
    ObjResourceManager_Load(&gObjResourceManager, "N83:/Obj_down.nce", "N83:/Obj_down.ncg", 0x1000, 1, -1);

    for (int i = 0; i < 7; i++) {
        Sprite_Init(&mButtons[i], 1, sButtonSequences[i], 0x80, 0x60, 1);
        SpriteButton_SetTouchRect(&mButtons[i], sButtonSizes[i].x, sButtonSizes[i].y);
        SpriteButton_SetSequences(&mButtons[i], sButtonSequences[i], sButtonSequences[i] + 1);
        SpriteButton_SetKind(&mButtons[i], 4);
        mButtons[i].SetPositionFromVec(sButtonPositions[i].x << FX32_SHIFT, sButtonPositions[i].y << FX32_SHIFT);
        SpriteButton_SetSounds(&mButtons[i], &sButtonSounds[i]);
        SpriteButton_Reset(&mButtons[i]);
    }

    for (int i = 0; i < 1; i++) {
        u16 sequence = sTextButtonSequences[i];
        Sprite_Init(&mTextButtons[i], 1, sequence, 0x80, 0x60, 1);
        SpriteButton_SetTouchRect(&mTextButtons[i], sTextButtonSizes[i].x, sTextButtonSizes[i].y);
        SpriteButton_SetSequences(&mTextButtons[i], sequence, sequence + 1);
        SpriteButton_SetKind(&mTextButtons[i], 4);
        mTextButtons[i].SetPositionFromVec(sTextButtonPositions[i].x << FX32_SHIFT,
                                           sTextButtonPositions[i].y << FX32_SHIFT);
        SpriteButton_SetSounds(&mTextButtons[i], &sTextButtonSounds[i]);
        SpriteButton_Reset(&mTextButtons[i]);
    }
    for (u16 j = 0; j < 1; j++) {
        charOffset += TextButton_SetMessage(&mTextButtons[j], &mMessageFiles[0], sTextButtonMessages[j],
                                            &gFonts[2].mFont, 0x1000, charOffset);
    }

    for (int i = 0; i < 8; i++) {
        mTexts[i].InitObjForMessage(&mMessageFiles[0], sTextMessages[i], &gFonts[sTextFonts[i]].mFont, 0x1000,
                                    charOffset, 0, 1, 0, 0, 0xF, NULL);
        SetTextPosition(&mTexts[i], sTextPositions[i]);
        MoveSprite(&mTexts[i].mSprite, (-mTexts[i].mCanvas.areaWidth * 4) << FX32_SHIFT,
                   (-mTexts[i].mCanvas.areaHeight * 4) << FX32_SHIFT);
        mTexts[i].mSprite.mOverwritePalette = 1;
        mTexts[i].mHAlign = 1;
        mTexts[i].mVAlign = 1;
        mTexts[i].DrawAll();
        charOffset += mTexts[i].GetCharCount();
    }

    ScreenLoader_LoadFile(&gScreenLoader, sWindowScreenName, 0x200, 0, 0, 0, 0, -1);
    ScreenRect rect = sWindowRect;
    MessageWindow_Init(&mWindow, rect);
    MessageWindow_SetScreen(&mWindow, sWindowScreenName, 0x200);
    func_020331bc(&mWindow, 0x3F00, rect);
    charOffset = MessageWindow_AddText(&mWindow, &mMessageFiles[0], 0x148, &gFonts[0].mFont, 0x1000,
                                       sWindowTextPosition, &charOffset);
    MessageWindow_AddTextButtonWithSounds(&mWindow, 0, 1, 19, 20, sWindowButtonPosition, sWindowButtonSize,
                                          &mMessageFiles[1], 4, &gFonts[2].mFont, 0x1000, &charOffset,
                                          &sWindowButtonSounds);
    MessageWindow_SetTextsVAlign(&mWindow, 1);
    ScreenLoader_SetPriority(&gScreenLoader, 0x200, 0);
    ScreenLoader_SetPriority(&gScreenLoader, 0x1000, 1);
    ScreenLoader_SetPriority(&gScreenLoader, 0x100, 2);
    gScreenControl.Set(0x1102);

    // A button and its label are shown only once its feature of the island is open (SaveHeader::GetUnknown44 1 or
    // 2, as overlay 72 reads the city hall's features 5 and 12). Only button 5, the save scene's, with its label,
    // text 6, depends on one. The test and the switch inside it are the original's: the switch's compare is the
    // second `bne` on the same flags, and the two variables live across the loop.
    int state;
    int text;
    for (int i = 1; i <= 6; i++) {
        if (i == 5) {
            switch (i) {
            case 5:
                state = gIslandState.GetUnknown44(12);
                text = 6;
                break;
            }
            if (state == 1 || state == 2) {
                mButtons[i].mVisible = TRUE;
                mTexts[text].mSprite.mVisible = TRUE;
            } else {
                mButtons[i].mVisible = FALSE;
                mTexts[text].mSprite.mVisible = FALSE;
            }
        }
    }
    SetState(STATE_MENU);
}

void CityHallScene::Update() {
    mScrollX += 0x4CC;
    ScreenLoader_SetScrollX(&gScreenLoader, 0x102, mScrollX >> FX32_SHIFT);
    switch (mState) {
    case STATE_MENU:
        for (int i = 0; i < 7; i++) {
            if (SpriteButton_Update(&mButtons[i])) {
                if (i == 0) {
                    gSceneManager.RequestScene(0x5F);
                } else {
                    int scene = sButtonScenes[i];
                    if (scene != -1) {
                        if (scene == 0x43) {
                            // A new resident only while the island has fewer than 100: else the window says so
                            int residents = 0;
                            for (int j = 0; j < 100; j++) {
                                if (GameData_IsCharacterSlotUsed(&gGameData, j)) {
                                    residents++;
                                }
                            }
                            if (residents >= 100) {
                                SetState(STATE_WINDOW_OPEN);
                            } else {
                                gSceneManager.RequestScene(scene);
                            }
                        } else if (scene == 0x1A) {
                            MI_CpuFill8(gSceneScratch, 0, 100);
                            GetSceneMemory().mScratch[0] = 0;
                            gSceneManager.RequestScene(scene);
                        } else {
                            gSceneManager.RequestScene(scene);
                        }
                    }
                }
            }
        }
        if (SpriteButton_Update(&mTextButtons[0])) {
            gSceneManager.RequestScene(sButtonScenes[7]);
        }
        break;
    case STATE_WINDOW_OPEN:
        if (mWindow.mResult != -1) {
            SetState(STATE_WINDOW_CLOSE);
        }
        break;
    case STATE_WINDOW_CLOSE:
        if (mWindow.mTransitionDone) {
            SetState(STATE_MENU);
        }
        break;
    }
    MessageWindow_Update(&mWindow);
}

void CityHallScene::Draw() {
    MessageWindow_Draw(&mWindow);
    for (int i = 0; i < 8; i++) {
        mTexts[i].Draw();
    }
    for (int i = 0; i < 7; i++) {
        SpriteButton_Draw(&mButtons[i]);
    }
    TextButton_Draw(&mTextButtons[0]);
    mModel.Draw();
}

void CityHallScene::Exit() {
    ScreenLoader_SetScrollX(&gScreenLoader, 0x3F, 0);
    ScreenLoader_SetScrollY(&gScreenLoader, 0x3F, 0);
    MessageWindow_Release(&mWindow);
    ScreenLoader_SetScrollX(&gScreenLoader, 1, 0);
    ArchiveManager_Unmount(&gArchiveManager, "N83");
    ArchiveManager_Unmount(&gArchiveManager, "TET");
    ArchiveManager_Unmount(&gArchiveManager, "TEC");
    ModelResources_ReleaseModel(gModelResources, 1);
    ArchiveManager_Unmount(&gArchiveManager, "M00");
}

void CityHallScene::SetState(int state) {
    switch (state) {
    case STATE_MENU:
        break;
    case STATE_WINDOW_OPEN:
        MessageWindow_Open(&mWindow, 0x12, 1);
        break;
    case STATE_WINDOW_CLOSE:
        MessageWindow_Close(&mWindow, 0x14);
        break;
    }
    mState = state;
    mUnknown0EDEC = 0;
}

CityHallScene::~CityHallScene() {}

#include <nitro.h>
#include <nnsys.h>

#include "PersonalityScene.h"
#include "MainProgram.h"

// Overlay 94: a character's personality (PersonalityScene). One source file: its constants are one pool at the start
// of .rodata, its strings the .data after the scene's virtual table.

// --- Data ---

static const int sUnknown24000[1] = {0x24000};
static const u32 sButtonMessages[1] = {4};
static const int sButtonFiles[1] = {0};
static const int sButtonSequences[1] = {2};
static const int sButtonKinds[1] = {4};
static const u32 sNameMessages[1] = {0x23};
static const ScreenPoint sRightHidden = {0x100, 0};
static const ScreenPoint sLeftHidden = {-0x100, 0};
static const ScreenPoint sButtonSizes[1] = {{0x5E, 0x1E}};
static const ScreenPoint sPersonalityTextOffset = {0x24, 0x58};
static const ScreenPoint sKindTitleOffset = {0x10, 0xB};
static const ScreenPoint sButtonPositions[1] = {{0x80, 0xAD}};
static const ScreenPoint sPersonalityTitleOffset = {0xBC, 0x47};
static const ScreenPoint sNamePositions[1] = {{8, 7}};
static const ScreenPoint sKindTextOffset = {0x60, 0x1C};
static const ScreenPoint sLeftShown = {0, 0};
static const ScreenPoint sRightShown = {0, 0};
static const VecFx32 sCharacterPosition = {0, 0, 0};
static const VecFx32 sCameraTarget = {0, 0x1C50, 0x11BF0};
static const VecFx32 sLightDirection0 = {0x640, -0x258, -0xFA0};
static const VecFx32 sCameraPosition = {0, 0x1C50, 0x19BF0};
static const VecFx32 sLightDirection1 = {0, -0xFA0, 0x5DC};
static const u32 sKindTexts[4] = {0x63, 0x61, 0x64, 0x62};
static const u32 sKindTitles[4] = {2, 0, 3, 1};
static const ButtonSounds sButtonSounds[1] = {{{-1, -1, 0, -1}}};
static const int sKindSounds[4] = {0x1A, 0x19, 0x18, 0x17};
// The kind of each personality: the quarter of the personality chart it is in
static const s8 sPersonalityKinds[16] = {0, 0, 1, 1, 0, 0, 1, 1, 2, 2, 3, 3, 2, 2, 3, 3};
static const u32 sPersonalityTitles[16] = {
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18,
};
static const u32 sPersonalityTexts[16] = {
    0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74,
};

// --- Helpers ---

template <class A, class B> static inline A AddPoints(A a, B b) {
    A p;
    p.x = a.x + b.x;
    p.y = a.y + b.y;
    return p;
}

// Puts a text canvas at a point in pixels
template <class P> static inline void SetCanvasPosition(ObjTextCanvas *canvas, P position) {
    canvas->SetPosition(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
}

static inline void SetCameraPosition(Camera *camera, const VecFx32 &position) {
    camera->mPosition.x = position.x;
    camera->mPosition.y = position.y;
    camera->mPosition.z = position.z;
}

static inline void SetCameraTarget(Camera *camera, const VecFx32 &target) {
    camera->mTarget.x = target.x;
    camera->mTarget.y = target.y;
    camera->mTarget.z = target.z;
}

// Shows a new message on a text: clears its canvas and draws it
static inline void Redraw(MessageText *text, u32 message) {
    text->mMessage = message;
    text->mColor = 8;
    NNS_G2dCharCanvasClear(&text->mCanvas, 0);
    text->DrawAll();
}

// --- The scene ---

void PersonalityScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "SCD", "Scn/Scene/SceneCharDistribution_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TBM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    mCast.mChosen[0] = gSceneMemory.mCharacters[0];
    mCast.mCount = 1;
    mCast.SetCharacters(mCharacters);
    mCast.MountArchives("Mdl/Ica/ica_mii_contents_LZ.bin");
    ArchiveManager_MountCompressed(&gArchiveManager, "T02", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "T03", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    gSoundManager.StartSeqIfNotPlaying(0, 2);
    gSoundManager.LoadGroup(0x52);
    mIslandProgress.Init(NULL, NULL);
}

// Not matched yet: the same instructions in another schedule and other registers. The original picks the character's
// personality load before the call's "this" by height, so its pre-RA scheduler was not in pressure mode there
// (docs/decompiling.md, "How the compiler orders and colours"): its block must start later than ours. Tried: a
// one-iteration loop over the character (with a one-element position table), a local for the animation, light
// helpers and pointers (the cast pointers below are the closest).
void PersonalityScene::Init() {
    VramManagers_Reset(&gVramManagerWork);
    gScreenControl.mUnknown44 = 1;
    gScreenControl.mUnknown44Changed = TRUE;
    G3X_SetClearColor(GX_RGB(31, 30, 28), 0, 0x7FFF, 63, FALSE);
    mCast.InitCharacters();
    mCharacters[0].mPos.x = sCharacterPosition.x;
    mCharacters[0].mPos.z = sCharacterPosition.y;
    mCharacters[0].mPos.z = sCharacterPosition.z;
    mCharacters[0].SetAngle(0x4000);
    mCharacters[0].mPersonality = mCharacters[0].GetPersonality();
    mCharacters[0].UpdateBiorhythm();
    mCharacters[0].BlendToAnimation(RESOURCE_BCA0, gStandAnimations[gPersonalityKinds[mCharacters[0].mPersonality]][0]);
    mCharacters[0].Update();

    Camera_Reset(mCast.mCamera);
    SetCameraPosition(mCast.mCamera, sCameraPosition);
    SetCameraTarget(mCast.mCamera, sCameraTarget);
    mCast.mCamera->mUnknown00 = 0x11E;
    mCast.mCamera->mUnknown04 = 0xFF6;
    Light *light0 = (Light *)&gLights;
    Light_Reset(light0);
    Light_SetDirectionByCamera(light0, mCast.mCamera, sLightDirection0);
    Light *light1 = (Light *)&gLight1;
    Light_Reset(light1);
    Light_SetDirectionByCamera(light1, mCast.mCamera, sLightDirection1);

    ScreenLoader_LoadFile(&gScreenLoader, "SCD:/BG_up_textframe.nsc", 4, 0, 0, 0, 0, -1);
    ScreenLoader_LoadFile(&gScreenLoader, "SCD:/BG_up.nsc", 0x202, 0, 0, 0, 0, -1);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "SCD:/BG_up.ncg", 0x206, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "SCD:/BG_up.ncl", 0x206, -1, -1, -1);
    ObjResourceManager_LoadCell(&gObjResourceManager, "SCD:/Obj_up.nce", 0x1000, 0);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "SCD:/Obj_up.ncg", 0x1000, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "SCD:/Obj_up.ncl", 0x1000, -1, -1, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "SCD:/Fw_Obj_up.ncl", 0x1000, 0, 0, 0x20);
    ScreenLoader_SetPriority(&gScreenLoader, 0x200, 3);
    ScreenLoader_SetPriority(&gScreenLoader, 0x1000, 0);
    ScreenLoader_SetPriority(&gScreenLoader, 0x10, 0);
    ScreenLoader_SetPriority(&gScreenLoader, 1, 0);
    ScreenLoader_SetPriority(&gScreenLoader, 2, 2);
    ScreenLoader_SetPriority(&gScreenLoader, 4, 1);
    gScreenControl.Set(0x1217);
    ScreenControl_SetBlendAlpha(&gScreenControl, 1, 4, 8, 8);

    mMessageFiles[0].Open("T02:/tmd_common_msg_JP.bmg");
    mMessageFiles[1].Open("T03:/tmd_etc_msg_JP.bmg");

    for (int i = 0; i < 1; i++) {
        u16 sequence = sButtonSequences[i];
        Sprite_Init(&mTextButtons[i], 0, sequence, 128, 96, TRUE);
        SpriteButton_SetTouchRect(&mTextButtons[i], sButtonSizes[i].x, sButtonSizes[i].y);
        SpriteButton_SetSequences(&mTextButtons[i], sequence, sequence + 1);
        SpriteButton_SetKind(&mTextButtons[i], sButtonKinds[i]);
        SpriteButton_SetSounds(&mTextButtons[i], &sButtonSounds[i]);
        mTextButtons[i].SetPosition(FxVec2(sButtonPositions[i].x << FX32_SHIFT, sButtonPositions[i].y << FX32_SHIFT));
        SpriteButton_Reset(&mTextButtons[i]);
        TextButton_SetMessage(&mTextButtons[i], &mMessageFiles[sButtonFiles[i]], sButtonMessages[i], &gFonts[2].mFont,
                              0x1000, 900);
    }

    mPanels.Init();
    mPanels.SetPersonality(mCharacters[0].GetPersonality());

    for (int i = 0; i < 1; i++) {
        wchar_t name[12];
        mTexts[i].InitObjForMessage(&mMessageFiles[1], sNameMessages[i], &gFonts[0].mFont, 0x10, 0, 0, 1, 0, 0, 15, NULL);
        SetCanvasPosition(&mTexts[i], sNamePositions[i]);
        MiiData_GetName(&mCharacters[0], name);
        mTexts[i].SetStringArg(name, 0);
        mTexts[i].DrawAll();
    }
    mScroll = 0;
}

void PersonalityScene::Update() {
    if (SpriteButton_Update(&mTextButtons[0])) {
        switch (mSceneId) {
        case SCENE_PERSONALITY:
            if (GetApartmentStage(mIslandProgress.mResidents) > gGameData.mApartmentStage ||
                mIslandProgress.GetNewFacility(FALSE) != -1) {
                s64 now = Clock_GetSeconds(gGameClock);
                mCharacters[0].SetLongAct(0, 0, now + mCharacters[0].GetLongActDuration(0, 3));
                mCharacters[0].ClearTrouble();
                GameData_WriteCharacter(&gGameData, mCast.mChosen[0], &mCharacters[0]);
                gSceneManager.RequestScene(0x5F);
                gSoundManager.StartSeqArc(2, 1, 0xD);
            } else {
                gSceneManager.RequestScene(0x11);
                gSoundManager.StartSeqArc(2, 1, 0xA);
            }
            break;
        case SCENE_TUTORIAL:
            gSceneManager.RequestScene(0x69);
            gSoundManager.StartSeqArc(2, 1, 0xA);
            break;
        }
    }
    mPanels.Update();
    mCast.UpdateCharacters(1);
}

void PersonalityScene::Draw() {
    mTexts[0].Draw();
    TextButton_Draw(&mTextButtons[0]);
    mPanels.Draw();
    mScroll += 0x4CC;
    ScreenLoader_SetScrollX(&gScreenLoader, 2, mScroll >> FX32_SHIFT);
    ScreenLoader_SetScrollX(&gScreenLoader, 0x200, mScroll >> FX32_SHIFT);
    mCast.DrawCharacters();
}

void PersonalityScene::Exit() {
    ArchiveManager_Unmount(&gArchiveManager, "SCD");
    ArchiveManager_Unmount(&gArchiveManager, "TBM");
    mCast.Release();
    ArchiveManager_Unmount(&gArchiveManager, "T02");
    ArchiveManager_Unmount(&gArchiveManager, "T03");
}

// --- The panels ---

// Not matched yet: the texts' positions are scheduled and coloured differently from the original (the helpers give
// PersonalityPanels::Draw exactly). Tried: the helpers by reference, the operands of AddPoints swapped, the addition of the
// texts' character counts to mCharOffset written five ways.
void PersonalityPanels::Init() {
    mLeftState = STATE_HIDDEN;
    mRightState = STATE_HIDDEN;
    mPersonality = 0;
    mKind = 0;
    mLeftPosition.x = sLeftHidden.x;
    mLeftPosition.y = sLeftHidden.y;
    mRightPosition.x = sRightHidden.x;
    mRightPosition.y = sRightHidden.y;
    mCharOffset = 0x180;
    mKindChanged = TRUE;

    Sprite_Init(&mLeftSprite, 0, 0, 128, 96, TRUE);
    mLeftSprite.SetPositionFromVec((mLeftPosition.x + 128) << FX32_SHIFT, (mRightPosition.y + 96) << FX32_SHIFT);
    mLeftSprite.mUnknown5E = 2;
    Sprite_Init(&mRightSprite, 0, 1, 128, 96, TRUE);
    mRightSprite.SetPositionFromVec((mRightPosition.x + 128) << FX32_SHIFT, (mRightPosition.y + 96) << FX32_SHIFT);
    mRightSprite.mUnknown5E = 1;

    mMessageFile.Open("TBM:/tmd_etc_msg_JP.bmg");
    mKindTitle.InitObjForMessage(&mMessageFile, sKindTitles[0], &gFonts[2].mFont, 0x1000, mCharOffset, 0, 1, 0, 0, 15,
                                 NULL);
    SetCanvasPosition(&mKindTitle, AddPoints(mLeftPosition, sKindTitleOffset));
    mKindTitle.mShadowDirections = CharCanvas::SHADOW_RIGHT | CharCanvas::SHADOW_DOWN | CharCanvas::SHADOW_LEFT |
                                   CharCanvas::SHADOW_UP;
    mKindTitle.mSprite.mOverwritePalette = 0;
    mCharOffset += mKindTitle.GetCharCount();

    mPersonalityTitle.InitObjForMessage(&mMessageFile, sPersonalityTitles[0], &gFonts[2].mFont, 0x1000, mCharOffset, 0,
                                        1, 0, 0, 15, NULL);
    SetCanvasPosition(&mPersonalityTitle, AddPoints(mRightPosition, sPersonalityTitleOffset));
    mPersonalityTitle.mShadowDirections = CharCanvas::SHADOW_RIGHT | CharCanvas::SHADOW_DOWN |
                                          CharCanvas::SHADOW_LEFT | CharCanvas::SHADOW_UP;
    mPersonalityTitle.mSprite.mOverwritePalette = 0;
    mCharOffset += mPersonalityTitle.GetCharCount();

    mKindText.InitObjForMessage(&mMessageFile, sKindTexts[0], &gFonts[0].mFont, 0x1000, mCharOffset, 0, 1, -80, 0, 15,
                                NULL);
    SetCanvasPosition(&mKindText, AddPoints(mLeftPosition, sKindTextOffset));
    mKindText.mSprite.mOverwritePalette = 0;
    mCharOffset += mKindText.GetCharCount();

    mPersonalityText.InitObjForMessage(&mMessageFile, sPersonalityTexts[0], &gFonts[0].mFont, 0x1000, mCharOffset, 0,
                                       1, 0, 0, 15, NULL);
    SetCanvasPosition(&mPersonalityText, AddPoints(mRightPosition, sPersonalityTextOffset));
    mPersonalityText.mSprite.mOverwritePalette = 0;
    mCharOffset += mPersonalityText.GetCharCount();
}

void PersonalityPanels::Update() {
    switch (mLeftState) {
    case STATE_SHOWN:
        break;
    case STATE_SLIDE_IN:
        mLeftPosition.x += 0x18;
        if (sLeftShown.x <= mLeftPosition.x) {
            mLeftPosition.x = sLeftShown.x;
            mLeftPosition.y = sLeftShown.y;
            mLeftState = STATE_SHOWN;
        }
        break;
    case STATE_SLIDE_OUT:
        mLeftPosition.x -= 0x18;
        if (mLeftPosition.x <= sLeftHidden.x) {
            mLeftPosition.x = sLeftHidden.x;
            mLeftPosition.y = sLeftHidden.y;
            mLeftState = STATE_HIDDEN;
        }
        break;
    case STATE_HIDDEN:
        break;
    }

    switch (mRightState) {
    case STATE_SHOWN:
        break;
    case STATE_SLIDE_IN:
        mRightPosition.x -= 0x18;
        if (mRightPosition.x <= sRightShown.x) {
            mRightPosition.x = sRightShown.x;
            mRightPosition.y = sRightShown.y;
            mRightState = STATE_SHOWN;
            gSoundManager.StartSeqArc(1, 0xE, sKindSounds[mKind]);
        }
        break;
    case STATE_SLIDE_OUT:
        mRightPosition.x += 0x18;
        if (mRightPosition.x >= sRightHidden.x) {
            mRightPosition.x = sRightHidden.x;
            mRightPosition.y = sRightHidden.y;
            mRightState = STATE_HIDDEN;
        }
        break;
    case STATE_HIDDEN:
        break;
    }

    if (mRightState != STATE_HIDDEN) {
        return;
    }
    if (mKindChanged == TRUE && mLeftState == STATE_HIDDEN) {
        Redraw(&mKindTitle, sKindTitles[mKind]);
        Redraw(&mKindText, sKindTexts[mKind]);
        Redraw(&mPersonalityTitle, sPersonalityTitles[mPersonality]);
        Redraw(&mPersonalityText, sPersonalityTexts[mPersonality]);
        PaletteLoader_LoadFile(&gPaletteLoader, "SCD:/Fw_Obj_up.ncl", 0x1000, (u16)(mPersonality * 0x20), 0, 0x20);
        mLeftState = STATE_SLIDE_IN;
        mRightState = STATE_SLIDE_IN;
        mKindChanged = FALSE;
    } else if (mKindChanged == FALSE) {
        Redraw(&mPersonalityTitle, sPersonalityTitles[mPersonality]);
        Redraw(&mPersonalityText, sPersonalityTexts[mPersonality]);
        PaletteLoader_LoadFile(&gPaletteLoader, "SCD:/Fw_Obj_up.ncl", 0x1000, (u16)(mPersonality * 0x20), 0, 0x20);
        mRightState = STATE_SLIDE_IN;
    }
}

void PersonalityPanels::Draw() {
    mLeftSprite.SetPositionFromVec((mLeftPosition.x + 128) << FX32_SHIFT, (mRightPosition.y + 96) << FX32_SHIFT);
    mRightSprite.SetPositionFromVec((mRightPosition.x + 128) << FX32_SHIFT, (mRightPosition.y + 96) << FX32_SHIFT);
    SetCanvasPosition(&mKindTitle, AddPoints(mLeftPosition, sKindTitleOffset));
    SetCanvasPosition(&mPersonalityTitle, AddPoints(mRightPosition, sPersonalityTitleOffset));
    SetCanvasPosition(&mKindText, AddPoints(mLeftPosition, sKindTextOffset));
    SetCanvasPosition(&mPersonalityText, AddPoints(mRightPosition, sPersonalityTextOffset));
    mKindTitle.Draw();
    mPersonalityTitle.Draw();
    mKindText.Draw();
    mPersonalityText.Draw();
    Sprite_Draw(&mLeftSprite);
    Sprite_Draw(&mRightSprite);
}

// Shows a personality: the right panel slides out, and the left one too if the kind changes
void PersonalityPanels::SetPersonality(int personality) {
    mPersonality = personality;
    mRightState = STATE_SLIDE_OUT;
    s8 kind = sPersonalityKinds[personality];
    if (mKind != kind) {
        mKind = kind;
        mLeftState = STATE_SLIDE_OUT;
        mKindChanged = TRUE;
    }
}

// Keeps the pool's first constant, which no function the link keeps reads
void PersonalityScene_Unused(int *out) {
    *out = sUnknown24000[0];
}

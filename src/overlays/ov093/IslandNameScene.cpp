#include <nitro.h>
#include <nnsys.h>

#include "IslandNameScene.h"
#include "MainProgram.h"

#include <wchar.h>

// Overlay 93: the player names the island (IslandNameScene). One source file. The island is overlay 72's map with
// no building built: the same ground models by month, a board at each of the twelve buildings' places, the same
// lights and the same music.

// Resource types of the ground's animations (the four letters in memory order)
#define RESOURCE_BTA0 0x30415442 // Texture SRT animation
#define RESOURCE_BMA0 0x30414D42 // Material colour animation

struct TextPosition {
    int x;
    int y;
};

// The file's constants, as overlay 72 has them
static const int sTextFonts[1] = {0};
static const u32 sTextMessages[1] = {0x17B}; // "Enter the island's name"
static const TextPosition sTextPositions[1] = {{128, 7}};
static const VecFx32 sCameraPosition = {-0x199, 0x1266, 0x6333};
static const VecFx32 sCameraTarget = {0, -0x147, 0};
static const VecFx32 sLightDirection1 = {0, -4000, 1500};
static const VecFx32 sLightDirection0 = {800, -600, -4000};
// The steps of the music's ramps: volume, pitch, tempo ratio, pan
static const int sMusicRampSteps[4] = {1, 1, 2, 1};
// The resource each model is drawn with: the ground, then a board for each building
static const int sModelResources[13] = {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
// The ground's model by month
static const u8 sMonthGrounds[13] = {0, 3, 3, 0, 1, 0, 0, 0, 0, 0, 2, 2, 3};
static const VecFx32 sModelPositions[13] = {
    {0, 0, 0},           {-696, -1146, 6553}, {-2129, -1146, 4915}, {122, -1146, 4505}, {-3235, -491, 2580},
    {-983, -368, 2416},  {2048, -327, 2048},  {3481, -327, 1966},   {0, 0, 0},          {-2990, 368, 655},
    {3194, 204, 204},    {-2498, 1146, -1433}, {2048, 1146, -1761},
};
// The music's volume, pitch, tempo ratio and pan by building model (0: none), as overlay 72's
static const int sModelMusic[13][4] = {
    {70, -96, 192, 0},   {127, 64, 256, 0},  {120, 48, 240, -8},   {120, 48, 240, 8},   {120, 16, 224, -16},
    {85, -32, 216, -10}, {115, 16, 224, 4},  {115, 32, 224, 32},   {80, -32, 216, 0},   {110, 0, 224, -24},
    {110, 0, 224, 24},   {100, -24, 216, -16}, {100, -24, 216, 16},
};

static GXRgb sLightColors[2] = {GX_RGB(31, 31, 31), GX_RGB(31, 31, 31)};
// The archive of each model resource: its path (the ground's is chosen by month), its name and the model in it
static const char *sResourcePaths[2] = {"Mdl/Map/map2_ground0_3D_LZ.bin", "Mdl/Map/map2_board_3D_LZ.bin"};
static const char *sResourceArchives[2] = {"M00", "M01"};
static const char *sResourceModels[2] = {"M00:/ground.imd", "M01:/board.imd"};
// The ground's model by season
static const char *sGroundModels[4] = {
    "Mdl/Map/map2_ground0_3D_LZ.bin",
    "Mdl/Map/map2_ground1_3D_LZ.bin",
    "Mdl/Map/map2_ground2_3D_LZ.bin",
    "Mdl/Map/map2_ground3_3D_LZ.bin",
};

// Puts a text at a position in pixels
static inline void SetTextPosition(MessageText *text, TextPosition position) {
    text->SetPosition(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
}

// Moves a sprite by an offset, built as an FxVec2 and passed by value
static inline void MoveSprite(Sprite *sprite, fx32 x, fx32 y) {
    NNSG2dFVec2 offset = Sprite::MakeVec(x, y);
    sprite->mPosition.x += offset.x;
    sprite->mPosition.y += offset.y;
}

void IslandNameScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "KBD", "Cmn/Kbd/SoftKeyboard_LZ.bin", 0);
    for (int i = 0; i < 2; i++) {
        if (i == 0) {
            RTCDate date;
            Clock_GetDate(gGameClock, &date);
            ArchiveManager_MountCompressed(&gArchiveManager, sResourceArchives[i],
                                           sGroundModels[sMonthGrounds[date.month]], 0);
        } else {
            ArchiveManager_MountCompressed(&gArchiveManager, sResourceArchives[i], sResourcePaths[i], 0);
        }
    }
    ArchiveManager_MountCompressed(&gArchiveManager, "TBM", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TBE", "Msg/TmdEtcMsgJP_LZ.bin", 0);
}

void IslandNameScene::Init() {
    mMessageFiles[0].Open("TBM:/tmd_common_msg_JP.bmg");
    mMessageFiles[1].Open("TBE:/tmd_etc_msg_JP.bmg");
    MI_CpuClear16(mName, sizeof(mName));
    mLength = 0;

    int charOffset = 0x384;
    mTextBox.Init(0x1000, 24, 2, charOffset, 0, 0, 0, 0, mTextBuffer);
    mTextBox.mColor = 8;
    mTextBox.SetPositionFromVec(0x20000, 0x28000);
    charOffset += mTextBox.GetCharCount();
    mSuffixBox.Init(0x1000, 24, 2, charOffset, 0, 0, 0, 0, mSuffixBuffer);
    mSuffixBox.mColor = 8;
    mSuffixBox.SetPositionFromVec(0xA9000, 0x26000);
    charOffset += mSuffixBox.GetCharCount();
    mCharBox.Init(0x1000, 24, 2, charOffset, 0, 0, NULL);
    mCharBox.mColor = 8;
    mCharBox.SetPositionFromVec(0x5B000, 0x26000);
    charOffset += mCharBox.GetCharCount();

    ScreenLoader_LoadFile(&gScreenLoader, "KBD:/bg_keyboard_6.nsc", 0x400, 0, 0, 0, 0, -1);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "KBD:/bg_keyboard.ncg", 0x400, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "KBD:/keyboardColor.ncl", 0x3F00, -1, -1, -1);
    gScreenControl.Show(0x400);
    mKeyboard.SetArchiveName("KBD");
    G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x08000,
                      GX_BG_EXTPLTT_01);
    gScreenControl.Show(0x100);
    mKeyboard.InitBg(0x100, 0);
    mKeyboard.SetMode(0, FALSE);
    mKeyboard.InitObj(0, &mMessageFiles[0], charOffset, 0x14000, 0, 13);
    mKeyboard.EnableTabs();
    charOffset += mKeyboard.GetButtonCharCount();
    ((KeyboardButton *)((u8 *)&mKeyboard.mButtons + sizeof(KeyboardButton)))->SetVisible(FALSE);
    mKeyboard.mButtons[0].SetVisible(FALSE);
    mKeyboard.SetButtonSounds(0, 1, 1, 0, 10);
    mKeyboard.mLayoutSprite.mPosition.x += 0x1A000;

    VramManagers_Reset(&gVramManagerWork);
    G3X_SetClearColor(GX_RGB(31, 31, 31), 31, 0x7FFF, 63, FALSE);
    for (int i = 0; i < 2; i++) {
        ModelResources_SetModel(gModelResources, i, sResourceModels[i]);
    }
    ModelResources_SetModel(gModelResources, 0, "M00:/wait.ita");
    ModelResources_SetModel(gModelResources, 0, "M00:/wait.ima");
    for (int i = 0; i < 13; i++) {
        mModels[i].Load(sModelResources[i], 0);
        mModels[i].mPosition.x = sModelPositions[i].x;
        mModels[i].mPosition.y = sModelPositions[i].y;
        mModels[i].mPosition.z = sModelPositions[i].z;
    }
    mModels[0].SetAnimation(RESOURCE_BTA0, 0);
    mModels[0].SetAnimation(RESOURCE_BMA0, 0);
    // The sky's colour, as the map left it in the scenes' scratch after gSceneMemory
    G3X_SetClearColor(*(GXRgb *)((u8 *)&gSceneMemory + sizeof(SceneMemory)), 31, 0x7FFF, 63, FALSE);

    Camera *const camera = &gCamera;
    Camera_Reset(camera);
    camera->mPosition.x = sCameraPosition.x;
    camera->mPosition.y = sCameraPosition.y;
    camera->mPosition.z = sCameraPosition.z;
    camera->mTarget.x = sCameraTarget.x;
    camera->mTarget.y = sCameraTarget.y;
    camera->mTarget.z = sCameraTarget.z;
    camera->mAngleX = 0;
    camera->mAngleY = 0;
    camera->mAngleZ = 0;
    camera->mUnknown0C = 4;
    gScreenControl.mPlanes &= ~0x3F;
    gScreenControl.mPlanesChanged = TRUE;
    gScreenControl.Show(1);

    Light *const light0 = &gLights[0];
    Light_Reset(light0);
    light0->mColor = sLightColors[1];
    Light_SetDirectionByCamera(light0, camera, sLightDirection0);
    Light *const light1 = &gLight1;
    Light_Reset(light1);
    light1->mColor = sLightColors[0];
    Light_SetDirectionByCamera(light1, camera, sLightDirection1);

    mMusic[0].Start();
    mMusic[0].SetTarget(0);

    for (int i = 0; i < 1; i++) {
        mText.InitObjForMessage(&mMessageFiles[1], sTextMessages[i], &gFonts[sTextFonts[i]].mFont, 0x1000,
                                charOffset, 0, 1, 0, 0, 0xF, NULL);
        SetTextPosition(&mText, sTextPositions[i]);
    }
    {
        mText.mSprite.mOverwritePalette = 0;
        MoveSprite(&mText.mSprite, (-mText.mCanvas.areaWidth * 4) << FX32_SHIFT, 0);
        mText.mHAlign = 1;
        mText.mVAlign = 1;
        NNS_G2dCharCanvasClear(&mText.mCanvas, 0);
        mText.DrawAll();
    }
    SetState(STATE_NAMING);
}

void IslandNameScene::Update() {
    if (mState == STATE_NAMING) {
        UpdateNaming();
    }
    mMusic[0].Update();
}

void IslandNameScene::Draw() {
    mTextBox.Draw();
    mSuffixBox.Draw();
    mCharBox.Draw();
    mKeyboard.Draw();
    for (int i = 0; i < 13; i++) {
        mModels[i].Draw();
    }
    mText.Draw();
}

void IslandNameScene::Exit() {
    ArchiveManager_Unmount(&gArchiveManager, "KBD");
    for (int i = 0; i < 2; i++) {
        ModelResources_ReleaseModel(gModelResources, i);
        ArchiveManager_Unmount(&gArchiveManager, sResourceArchives[i]);
    }
    ArchiveManager_Unmount(&gArchiveManager, "TBM");
    ArchiveManager_Unmount(&gArchiveManager, "TBE");
}

void IslandNameScene::SetState(int state) {
    if (state == STATE_NAMING) {
        SetName(L"");
        mTextBox.SetText((const NNSG2dChar *)L"");
        mTextBox.Redraw();
        mTextBox.Transfer();
        mSuffixBox.SetText((const NNSG2dChar *)L"島");
        mSuffixBox.Redraw();
        mSuffixBox.Transfer();
    }
    mUnknown7160 = 0;
    mState = state;
}

void IslandNameScene::SetName(const wchar_t *name) {
    MI_CpuClear16(mName, sizeof(mName));
    mLength = 0;
    NNS_G2dCharCanvasClear(&mCharBox.mCanvas, 0);
    while (mLength < NAME_LENGTH) {
        u16 c = name[mLength];
        if (c == 0) {
            return;
        }
        mName[mLength] = c;
        int x = mLength * 13 + 5;
        const NNSG2dFont *font = mCharBox.mFont;
        mCharBox.mCharX = x - (NNS_G2dFontGetCharWidths(font, c)->charWidth >> 1);
        mCharBox.SetChar(c);
        mCharBox.Redraw();
        mCharBox.Transfer();
        mLength++;
    }
}

void IslandNameScene::UpdateNaming() {
    mKeyboard.Update(0, gTouchInput.mTp);
    if (mKeyboard.mButtons[0].Update()) {
        wcscpy(data_02123510, mName);
        gSceneManager.RequestScene(0x69);
    } else if (gTouchInput.mTouched) {
        int x;
        u16 c = KeyboardCore_GetChar();
        if (c != 0) {
            mUnknown6008 = 0;
            if (c >= 0x20) {
                if (mLength < NAME_LENGTH) {
                    mName[mLength] = c;
                    x = mLength * 13 + 5;
                    const NNSG2dFont *font = mCharBox.mFont;
                    mCharBox.mCharX = x - (NNS_G2dFontGetCharWidths(font, c)->charWidth >> 1);
                    mCharBox.SetChar(c);
                    mCharBox.Redraw();
                    mLength++;
                    if (mLength < NAME_LENGTH) {
                        mKeyboard.mLayoutSprite.mPosition.x += 0xD000;
                    }
                }
            } else if (c == 8) {
                if (mLength > 0) {
                    if (mLength < NAME_LENGTH) {
                        mKeyboard.mLayoutSprite.mPosition.x -= 0xD000;
                    }
                    mLength--;
                    mName[mLength] = 0;
                    wchar_t text[NAME_LENGTH];
                    wcscpy(text, mName);
                    SetName(text);
                }
            } else if (c == 7) {
                int last = mLength - 1;
                if (last < 0) {
                    return;
                }
                u16 ch = mName[last];
                BOOL changed = FALSE;
                // ka ki ku ke ko, KA to KO, sa to so, SA to SO, ta chi tsu te to, TA to TO, ha to ho, HA to HO
                if (ch == 0x304B || ch == 0x304D || ch == 0x304F || ch == 0x3051 || ch == 0x3053 || ch == 0x30AB ||
                    ch == 0x30AD || ch == 0x30AF || ch == 0x30B1 || ch == 0x30B3 || ch == 0x3055 || ch == 0x3057 ||
                    ch == 0x3059 || ch == 0x305B || ch == 0x305D || ch == 0x30B5 || ch == 0x30B7 || ch == 0x30B9 ||
                    ch == 0x30BB || ch == 0x30BD || ch == 0x305F || ch == 0x3061 || ch == 0x3064 || ch == 0x3066 ||
                    ch == 0x3068 || ch == 0x30BF || ch == 0x30C1 || ch == 0x30C4 || ch == 0x30C6 || ch == 0x30C8 ||
                    ch == 0x306F || ch == 0x3072 || ch == 0x3075 || ch == 0x3078 || ch == 0x307B || ch == 0x30CF ||
                    ch == 0x30D2 || ch == 0x30D5 || ch == 0x30D8 || ch == 0x30DB) {
                    mName[last] = ch + 1;
                    changed = TRUE;
                } else if (ch == 0x304C || ch == 0x304E || ch == 0x3050 || ch == 0x3052 || ch == 0x3054 ||
                           ch == 0x30AC || ch == 0x30AE || ch == 0x30B0 || ch == 0x30B2 || ch == 0x30B4 ||
                           ch == 0x3056 || ch == 0x3058 || ch == 0x305A || ch == 0x305C || ch == 0x305E ||
                           ch == 0x30B6 || ch == 0x30B8 || ch == 0x30BA || ch == 0x30BC || ch == 0x30BE ||
                           ch == 0x3060 || ch == 0x3062 || ch == 0x3065 || ch == 0x3067 || ch == 0x3069 ||
                           ch == 0x30C0 || ch == 0x30C2 || ch == 0x30C5 || ch == 0x30C7 || ch == 0x30C9 ||
                           ch == 0x3070 || ch == 0x3073 || ch == 0x3076 || ch == 0x3079 || ch == 0x307C ||
                           ch == 0x30D0 || ch == 0x30D3 || ch == 0x30D6 || ch == 0x30D9 || ch == 0x30DC) {
                    mName[last] = ch - 1;
                    changed = TRUE;
                } else if (ch == 0x30A6) { // U, VU
                    mName[last] = 0x30F4;
                    changed = TRUE;
                } else if (ch == 0x30F4) {
                    mName[last] = 0x30A6;
                    changed = TRUE;
                }
                if (changed) {
                    wchar_t text[NAME_LENGTH];
                    wcscpy(text, mName);
                    SetName(text);
                }
            } else if (c == 9) {
                int last = mLength - 1;
                if (last < 0) {
                    return;
                }
                u16 ch = mName[last];
                BOOL changed = FALSE;
                // ha hi fu he ho, HA to HO
                if (ch == 0x306F || ch == 0x3072 || ch == 0x3075 || ch == 0x3078 || ch == 0x307B || ch == 0x30CF ||
                    ch == 0x30D2 || ch == 0x30D5 || ch == 0x30D8 || ch == 0x30DB) {
                    mName[last] = ch + 2;
                    changed = TRUE;
                } else if (ch == 0x3071 || ch == 0x3074 || ch == 0x3077 || ch == 0x307A || ch == 0x307D ||
                           ch == 0x30D1 || ch == 0x30D4 || ch == 0x30D7 || ch == 0x30DA || ch == 0x30DD) {
                    mName[last] = ch - 2;
                    changed = TRUE;
                }
                if (changed) {
                    wchar_t text[NAME_LENGTH];
                    wcscpy(text, mName);
                    SetName(text);
                }
            } else if (c == 11) {
                int last = mLength - 1;
                if (last < 0) {
                    return;
                }
                u16 ch = mName[last];
                BOOL changed = FALSE;
                // a i u e o, A to O, tsu, TSU, ya, YA, yu, YU, yo, YO, wa, WA
                if (ch == 0x3042 || ch == 0x3044 || ch == 0x3046 || ch == 0x3048 || ch == 0x304A || ch == 0x30A2 ||
                    ch == 0x30A4 || ch == 0x30A6 || ch == 0x30A8 || ch == 0x30AA || ch == 0x3064 || ch == 0x30C4 ||
                    ch == 0x3084 || ch == 0x30E4 || ch == 0x3086 || ch == 0x30E6 || ch == 0x3088 || ch == 0x30E8 ||
                    ch == 0x308F || ch == 0x30EF) {
                    mName[last] = ch - 1;
                    changed = TRUE;
                } else if (ch == 0x3041 || ch == 0x3043 || ch == 0x3045 || ch == 0x3047 || ch == 0x3049 ||
                           ch == 0x30A1 || ch == 0x30A3 || ch == 0x30A5 || ch == 0x30A7 || ch == 0x30A9 ||
                           ch == 0x3063 || ch == 0x30C3 || ch == 0x3083 || ch == 0x30E3 || ch == 0x3085 ||
                           ch == 0x30E5 || ch == 0x3087 || ch == 0x30E7 || ch == 0x308E || ch == 0x30EE) {
                    mName[last] = ch + 1;
                    changed = TRUE;
                } else if (ch == 0x30F5) { // Small KA, KA
                    mName[last] = 0x30AB;
                    changed = TRUE;
                } else if (ch == 0x30AB) {
                    mName[last] = 0x30F5;
                    changed = TRUE;
                } else if (ch == 0x30F6) { // Small KE, KE
                    mName[last] = 0x30B1;
                    changed = TRUE;
                } else if (ch == 0x30B1) {
                    mName[last] = 0x30F6;
                    changed = TRUE;
                }
                if (changed) {
                    wchar_t text[NAME_LENGTH];
                    wcscpy(text, mName);
                    SetName(text);
                }
            }
        }
    } else if (gTouchInput.mTp.touch != 0 || gTouchInput.mUnknown4C != 0 || gPadInput.mActive == 0) {
        mKeyboard.TransferCharData();
    }
    if (gTouchInput.mTp.touch != 0) {
        // The OK button shows once the name has more than spaces
        wchar_t name[NAME_LENGTH + 5];
        wcsncpy(name, mName, NAME_LENGTH + 5);
        CharacterSort::RemoveSpaces(name);
        if (wcslen(name) == 0) {
            mKeyboard.mButtons[0].SetVisible(FALSE);
        } else {
            mKeyboard.mButtons[0].SetVisible(TRUE);
        }
    }
}

// --- IslandNameMusic ---

void IslandNameMusic::Start() {
    gSoundManager.LoadGroup(0x46);
    gSoundManager.StartSeqArc(0, 3, 4);
    for (int i = 0; i < RAMP_COUNT; i++) {
        mRamps[i].mStep = sMusicRampSteps[i] << FX32_SHIFT;
        int value = sModelMusic[0][i] << FX32_SHIFT;
        mRamps[i].mValue = value;
        mRamps[i].mTarget = value;
        mRamps[i].SetMode(ValueRamp::MODE_TO_TARGET);
    }
    gSoundManager.SetSeqArcVolume(0, mRamps[0].mValue >> FX32_SHIFT);
    gSoundManager.SetSeqArcTrackPitch(0, 0xFFFF, mRamps[1].mValue >> FX32_SHIFT);
    gSoundManager.SetSeqArcTempoRatio(0, mRamps[2].mValue >> FX32_SHIFT);
    gSoundManager.SetSeqArcTrackPan(0, 0xFFFF, mRamps[3].mValue >> FX32_SHIFT);
}

void IslandNameMusic::Update() {
    SoundParamRamps::Update();
    gSoundManager.SetSeqArcVolume(0, mRamps[0].mValue >> FX32_SHIFT);
    gSoundManager.SetSeqArcTrackPitch(0, 0xFFFF, mRamps[1].mValue >> FX32_SHIFT);
    gSoundManager.SetSeqArcTempoRatio(0, mRamps[2].mValue >> FX32_SHIFT);
    gSoundManager.SetSeqArcTrackPan(0, 0xFFFF, mRamps[3].mValue >> FX32_SHIFT);
}

void IslandNameMusic::SetTarget(int setting) {
    for (int i = 0; i < RAMP_COUNT; i++) {
        mRamps[i].mTarget = sModelMusic[setting][i] << FX32_SHIFT;
        mRamps[i].SetMode(ValueRamp::MODE_TO_TARGET);
    }
}

#include "TravelScene.h"
#include "MainProgram.h"

#include <string.h>

// --- Data ---

// The data is declared in an order that gives the ROM's: see docs/decompiling.md, "How a file's data is laid out".

static const u32 sSightMessages[TravelScene::DESTINATION_COUNT] = {0x4B6, 0x4C6, 0x4D4, 0x4DC, 0x4EA,
                                                      0x4F4, 0x502, 0x50A, 0x51A};

// --- The scene ---

// Moves a sprite by an offset, by way of a copy as Sprite::SetPositionFromVec makes one
static inline void MoveSprite(Sprite *sprite, fx32 x, fx32 y) {
    NNSG2dFVec2 offset = Sprite::MakeVec(x, y);
    sprite->mPosition.x += offset.x;
    sprite->mPosition.y += offset.y;
}

// Puts a sprite at a point in pixels
static inline void SetPixelPosition(Sprite *sprite, ScreenPoint position) {
    sprite->SetPositionFromVec(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
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

static inline u16 Rand(int max) {
    return MATH_Rand16(&gRandom, max);
}

void TravelScene::StartLoading() {
    // The archives of the destinations, mounted as "N01"
    static const char *sDestinationArchives[DESTINATION_COUNT] = {
        "Scn/Travel/France_LZ.bin",  "Scn/Travel/Hawaii_LZ.bin",   "Scn/Travel/Hongkong_LZ.bin",
        "Scn/Travel/Italia_LZ.bin",  "Scn/Travel/Kyoto_LZ.bin",    "Scn/Travel/Okinawa_LZ.bin",
        "Scn/Travel/Shanghai_LZ.bin", "Scn/Travel/Spain_LZ.bin",   "Scn/Travel/Thai_LZ.bin",
    };
    static const u8 sSightCounts[DESTINATION_COUNT] = {8, 7, 4, 7, 5, 7, 4, 8, 4};
    static const int sSceneMemoryValues[DESTINATION_COUNT] = {0xB6, 0xC3, 0xD9, 0xC2, 0xC4, 0xC5, 0xC6, 0xC7, 0xC8};
    // The clothes of the dressed pair: for a man, for a woman
    static const u16 sClothes[DESTINATION_COUNT][2] = {
        {0x3A, 0x3A}, {0x46, 0x47}, {0xF3, 0xF3}, {0x60, 0x61}, {0x0E, 0x0D},
        {0x85, 0x83}, {0x90, 0x8F}, {0xD2, 0x17}, {0x20, 0x20},
    };
    static const int sSoundGroups[DESTINATION_COUNT] = {0x3E, 0x27, 0x25, 0x3D, 0x24, 0x23, 0x26, 0x28, 0x3F};


    int sights[MAX_SIGHTS];

    mDestination = Rand(DESTINATION_COUNT);
    gSceneMemory.mUnknown2C[0] = (u8)sSceneMemoryValues[mDestination];

    for (int i = 0; i < sSightCounts[mDestination]; i++) {
        sights[i] = i;
    }
    for (int i = 0; i < sSightCounts[mDestination]; i++) {
        int t = sights[i];
        int j = Rand(sSightCounts[mDestination]);
        sights[i] = sights[j];
        sights[j] = t;
    }
    for (int i = 0; i < PHOTO_COUNT; i++) {
        mSights[i] = sights[i];
    }

    switch (mSceneId) {
    case SCENE_PAIR_DRESSED:
    case SCENE_PAIR:
        mTravellerCount = 2;
        break;
    case SCENE_SOLO:
        mTravellerCount = 1;
        break;
    }

    if (!mCast.Choose(mTravellerCount)) {
        return;
    }
    mCast.SetCharacters(mCharacters);

    if (mSceneId == SCENE_PAIR_DRESSED) {
        if (MiiData_GetSex(&mCharacters[0]) == 0) {
            mCharacters[0].mClothesId = sClothes[mDestination][0];
            mCharacters[1].mClothesId = sClothes[mDestination][1];
        } else {
            mCharacters[0].mClothesId = sClothes[mDestination][1];
            mCharacters[1].mClothesId = sClothes[mDestination][0];
        }
    }

    mCast.MountArchives("Mdl/Ica/ica_mii_mini_contents_travel_LZ.bin");
    ArchiveManager_MountCompressed(&gArchiveManager, "TBM", "Msg/TmdMiniContentsMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "N00", "Scn/Scene/SceneTestNg116_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "N01", sDestinationArchives[mDestination], 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "SFL", "Scn/Scene/NgCaption_LZ.bin", 0);
    gSoundManager.LoadGroup(0x67);
    gSoundManager.LoadGroup(sSoundGroups[mDestination]);
}

void TravelScene::Init() {
    static const VecFx32 sCameraTarget = {0, FX32_CONST(2.25), FX32_CONST(-1.25)};
    static const ButtonSounds sShutterSounds = {{-1, -1, 0, -1}};
    static const u16 sButtonSequences[4] = {3, 3, 3, 3};
    static const int sButtonKinds[4] = {4, 4, 4, 4};
    static const ButtonSounds sButtonSounds[4] = {
        {{10, -1, 0, -1}}, {{10, -1, 0, -1}}, {{10, -1, 0, -1}}, {{10, -1, 0, -1}},
    };
    static const u32 sButtonMessages[4] = {0x535, 0x532, 0x533, 0x534};
    static const u16 sPairAnimations[2] = {1, 10};
    static const ScreenPoint sPairScreenPoints[2] = {{36, 120}, {92, 120}};
    static const int sCaptionFonts[1] = {0};
    static const ScreenPoint sCaptionPositions[1] = {{128, 14}};
    static const VecFx32 sCameraPosition = {0, FX32_CONST(2.25), FX32_CONST(10.75)};
    static const ScreenPoint sShutterSizes[1] = {{64, 64}};
    static const ScreenPoint sShutterPositions[1] = {{195, 89}};
    // The sprites of the upper screen: the destination's title
    static const u16 sDestinationSequences[DESTINATION_COUNT] = {2, 3, 4, 5, 6, 7, 8, 9, 10};
    static const u16 sSoloAnimations[1] = {0};
    static const int sSequences[DESTINATION_COUNT] = {0x6F, 0x34, 0x32, 0x6E, 0x31, 0x30, 0x33, 0x35, 0x70};
    static const ScreenPoint sButtonPositions[4] = {{128, 96}, {128, 96}, {128, 96}, {128, 96}};
    static const u16 sShutterSequences[1] = {0};
    // Where the travellers stand at first: points of the lower screen, put on the plane z = -7.0
    static const ScreenPoint sSoloScreenPoints[1] = {{64, 120}};
    static const ScreenPoint sButtonSizes[4] = {{96, 32}, {96, 32}, {96, 32}, {96, 32}};


    gSoundManager.StartSeq(0, sSequences[mDestination]);
    func_0202b0e8(&data_02122824);
    gScreenControl.mUnknown44 = 1;
    gScreenControl.mUnknown44Changed = TRUE;
    G3X_SetClearColor(GX_RGB(31, 31, 31), 0, 0x7FFF, 63, FALSE);
    mCast.InitCharacters();
    func_0202a96c(data_021227cc);
    func_0202a96c(data_021227d8);
    func_0202a640(mCast.mCamera);
    SetCameraPosition(mCast.mCamera, sCameraPosition);
    SetCameraTarget(mCast.mCamera, sCameraTarget);
    mCast.mCamera->mAngleX = 0x200;
    mCast.mCamera->mAngleY = 0;
    mCast.mCamera->mAngleZ = 0;

    for (int i = 0; i < mTravellerCount; i++) {
        MtxFx43 matrix;
        MtxFx33 rotation;
        VecFx32 near, far;
        mCharacters[i].Draw();
        mCharacters[i].GetJointMatrices(CharacterModel::JOINT_NECK, &matrix, &rotation);
        fx32 height = matrix.m[3][1] - mCharacters[i].mPos.y;
        if (mTravellerCount == 1) {
            NNS_G3dScrPosToWorldLine(sSoloScreenPoints[i].x, sSoloScreenPoints[i].y, &near, &far);
            mCharacters[i].SetAngle(0x4000);
            mCharacters[i].BlendToAnimation(RESOURCE_BCA0, sSoloAnimations[i]);
        } else {
            NNS_G3dScrPosToWorldLine(sPairScreenPoints[i].x, sPairScreenPoints[i].y, &near, &far);
            mCharacters[i].SetAngle(0x4000);
            mCharacters[i].BlendToAnimation(RESOURCE_BCA0, sPairAnimations[i]);
        }
        fx32 dx = far.x - near.x;
        fx32 dy = far.y - near.y;
        fx32 t = FX_Div(FX32_CONST(-7) - near.z, far.z - near.z);
        fx32 y = near.y + FX_Mul(dy, t);
        fx32 x = near.x + FX_Mul(dx, t);
        mCharacters[i].mPos.x = x;
        mCharacters[i].mPos.y = y - height;
        mCharacters[i].mPos.z = FX32_CONST(-7);
    }

    switch (mSceneId) {
    case SCENE_PAIR_DRESSED:
        ObjResourceManager_Load(&gObjResourceManager, "N00:/Obj_up_hm.nce", "N00:/Obj_up_hm.ncg", 0x10, 0, -1);
        PaletteLoader_LoadFile(&gPaletteLoader, "N00:/Obj_up_hm.ncl", 0x10, -1, -1, -1);
        ScreenLoader_LoadFile(&gScreenLoader, "N00:/BG_airplane_hm.nsc", 4, 0, 0, 0, 0, -1);
        ScreenLoader_LoadFile(&gScreenLoader, "N00:/BG_down_hm.nsc", 0x200, 0, 0, 0, 0, -1);
        break;
    case SCENE_SOLO:
    case SCENE_PAIR:
        ObjResourceManager_Load(&gObjResourceManager, "N00:/Obj_up.nce", "N00:/Obj_up.ncg", 0x10, 0, -1);
        PaletteLoader_LoadFile(&gPaletteLoader, "N00:/Obj_up.ncl", 0x10, -1, -1, -1);
        ScreenLoader_LoadFile(&gScreenLoader, "N00:/BG_airplane.nsc", 4, 0, 0, 0, 0, -1);
        ScreenLoader_LoadFile(&gScreenLoader, "N00:/BG_down.nsc", 0x200, 0, 0, 0, 0, -1);
        break;
    }

    Sprite_Init(&mSprites[0], 0, 0, 128, 96, TRUE);
    Sprite_Init(&mSprites[1], 0, 1, 128, 96, TRUE);
    Sprite_Init(&mSprites[2], 0, sDestinationSequences[mDestination], 128, 96, TRUE);
    Sprite_Init(&mSprites[3], 0, 11, 128, 96, TRUE);
    mSprites[0].mVisible = FALSE;
    ObjResourceManager_Load(&gObjResourceManager, "N00:/Obj_down.nce", "N00:/Obj_down.ncg", 0x1000, 1, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "N00:/Obj_down.ncl", 0x1000, -1, -1, -1);
    Sprite_Init(&mSprites[4], 1, 2, 128, 96, TRUE);
    mSprites[4].mVisible = FALSE;

    for (int i = 0; i < 1; i++) {
        u16 sequence = sShutterSequences[i];
        Sprite_Init(&mButtons[i], 1, sequence, 128, 96, TRUE);
        func_02031eb0(&mButtons[i], sShutterSizes[i].x, sShutterSizes[i].y);
        func_02031278(&mButtons[i], sequence, sequence + 1);
        func_02031260(&mButtons[i], 0);
        mButtons[i].SetPosition(FxVec2(sShutterPositions[i].x << FX32_SHIFT, sShutterPositions[i].y << FX32_SHIFT));
        func_02032158(&mButtons[i], &sShutterSounds);
        func_0203119c(&mButtons[i]);
        mButtons[i].mVisible = FALSE;
    }

    PaletteLoader_LoadFile(&gPaletteLoader, "N00:/BG_up.ncl", 0x2F, -1, -1, -1);
    func_02028f24(&gVramTransferQueue, "N00:/BG_up.ncg", 0xC, 0, 0, 0, -1);
    ScreenLoader_LoadFile(&gScreenLoader, "N00:/BG_pictureframe.nsc", 8, 0, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "N00:/BG_down.ncl", 0x200, -1, -1, -1);
    func_02028f24(&gVramTransferQueue, "N00:/BG_down.ncg", 0x200, 0, 0, 0, -1);
    mMessageFiles[0].Open("TBM:/tmd_minicontents_msg_JP.bmg");
    mText.mMessageFile = &mMessageFiles[0];
    ScreenControl_SetBlendAlpha(&gScreenControl, 2, 0x1D, 8, 8);
    func_02028f24(&gVramTransferQueue, "SFL:/caption.ncg", 2, 0, 0, 0, -1);
    ScreenLoader_LoadFile(&gScreenLoader, "SFL:/caption.nsc", 2, 0, 0, 0, 0, -1);

    const u32 pairCaption = 0x4AD; // "<name> and <name>'s"
    const u32 soloCaption = 0x4B2; // "<name>'s"
    int charOffset = 0x100;
    int buttonCharOffset = 200;
    charOffset += mSpeechWindow.Init(charOffset, &mMessageFiles[0], 2, 0x10, &mTtsPlayer, FALSE);
    for (int i = 0; i < 1; i++) {
        if (mTravellerCount == 1) {
            mCaptions[i].InitObjForMessage(&mMessageFiles[0], soloCaption, &gFonts[sCaptionFonts[i]].mFont, 0x10, charOffset, 0,
                                       1, 0, 0, 15, NULL);
        } else {
            mCaptions[i].InitObjForMessage(&mMessageFiles[0], pairCaption, &gFonts[sCaptionFonts[i]].mFont, 0x10, charOffset, 0,
                                       1, 0, 0, 15, NULL);
        }
        SetPixelPosition(&mCaptions[i].mSprite, sCaptionPositions[i]);
        MoveSprite(&mCaptions[i].mSprite, -mCaptions[i].mCanvas.areaWidth * 4 << FX32_SHIFT,
                   -mCaptions[i].mCanvas.areaHeight * 4 << FX32_SHIFT);
        mCaptions[i].mHAlign = 1;
        mCaptions[i].mVAlign = 1;
        if (mTravellerCount == 1) {
            mCaptions[i].SetStringArg(mCast.mSpeechTexts[0].mName, 0);
        } else {
            mCaptions[i].SetStringArg(mCast.mSpeechTexts[0].mName, 0);
            mCaptions[i].SetStringArg(mCast.GetSpeechTexts(1)->mName, 1);
        }
        mCaptions[i].mShadowDirections = CharCanvas::SHADOW_RIGHT | CharCanvas::SHADOW_DOWN | CharCanvas::SHADOW_LEFT |
                                     CharCanvas::SHADOW_UP;
        mCaptions[i].mShadowColor = 1;
        mCaptions[i].DrawAll();
    }

    for (int i = 0; i < 4; i++) {
        u16 sequence = sButtonSequences[i];
        Sprite_Init(&mTextButtons[i], 1, sequence, 128, 96, TRUE);
        func_02031eb0(&mTextButtons[i], sButtonSizes[i].x, sButtonSizes[i].y);
        func_02031278(&mTextButtons[i], sequence, sequence + 1);
        func_02031260(&mTextButtons[i], sButtonKinds[i]);
        mTextButtons[i].SetPositionFromVec(sButtonPositions[i].x << FX32_SHIFT, sButtonPositions[i].y << FX32_SHIFT);
        func_02032158(&mTextButtons[i], &sButtonSounds[i]);
        func_0203119c(&mTextButtons[i]);
        mTextButtons[i].mVisible = FALSE;
    }
    for (u16 i = 0; i < 4; i++) {
        buttonCharOffset += func_02031f58(&mTextButtons[i], &mMessageFiles[0], sButtonMessages[i], &gFonts[2].mFont,
                                          0x1000, buttonCharOffset);
    }

    ScreenLoader_SetPriority(&gScreenLoader, 0x10, 0);
    ScreenLoader_SetPriority(&gScreenLoader, 1, 1);
    ScreenLoader_SetPriority(&gScreenLoader, 8, 0);
    ScreenLoader_SetPriority(&gScreenLoader, 2, 0);
    ScreenLoader_SetPriority(&gScreenLoader, 4, 2);
    ScreenLoader_SetPriority(&gScreenLoader, 0x1000, 0);
    ScreenLoader_SetPriority(&gScreenLoader, 0x200, 1);
    gScreenControl.Set(0x1217);
    PaletteLoader_SetBackdrop(&gPaletteLoader, 0, 0x101);
    mTtsPlayer.InitEngine();
    mLastSpot = -1;
    SetState(STATE_START);
}

void TravelScene::Update() {
    mTtsPlayer.Update();
    switch (mState) {
    case STATE_START:
        if (func_0203133c(&mTextButtons[0])) {
            mTextButtons[0].mVisible = FALSE;
            SetState(mState + 1);
        }
        break;
    case STATE_DEPARTURE:
        ScreenControl_BlendFadeOut(&gScreenControl, 0x3F, -1);
        if (mFrame++ > 120) {
            mSprites[1].mVisible = FALSE;
            mSprites[2].mVisible = FALSE;
            mSprites[3].mVisible = FALSE;
            mSprites[4].mVisible = TRUE;
            SetState(mState + 1);
        }
        break;
    case STATE_PHOTO_FADE_IN:
    case STATE_PHOTO2_FADE_IN:
    case STATE_PHOTO3_FADE_IN:
        if (ScreenControl_BlendFadeIn(&gScreenControl, 0x3F, 1)) {
            SetState(mState + 1);
        }
        break;
    case STATE_PHOTO_INTRO:
    case STATE_PHOTO2_INTRO:
    case STATE_PHOTO3_INTRO:
        if (mTtsPlayer.IsEngineIdle() == TRUE) {
            mTextButtons[2].mVisible = FALSE;
            SetState(mState + 1);
        }
        break;
    case STATE_PHOTO_POSE:
    case STATE_PHOTO2_POSE:
    case STATE_PHOTO3_POSE:
        if (mFrame++ == mPoseFrame) {
            if (mTravellerCount == 1) {
                mCharacters[0].BlendToAnimation(RESOURCE_BCA0, Rand(5) + 5);
            } else {
                int first, second;
                switch (Rand(2)) {
                case 0: {
                    int pose = Rand(4);
                    first = pose + 1;
                    second = pose + 10;
                    break;
                }
                case 1:
                    first = Rand(5) + 5;
                    second = Rand(5) + 5;
                    break;
                }
                mCharacters[0].BlendToAnimation(RESOURCE_BCA0, first);
                mCharacters[1].BlendToAnimation(RESOURCE_BCA0, second);
            }
        }
        if (func_0203133c(&mButtons[0])) {
            gSoundManager.StartSeqArc(1, 0x30, 15);
            SetState(mState + 1);
        }
        break;
    case STATE_PHOTO_FLASH:
    case STATE_PHOTO2_FLASH:
    case STATE_PHOTO3_FLASH: {
        int level = 16 - mFrame * 2;
        if (level > 16) {
            level = 16;
        } else if (level < -16) {
            level = -16;
        }
        ScreenControl_SetBlendBrightness(&gScreenControl, 0x3F, level);
        if (mFrame++ > 60) {
            SetState(mState + 1);
        }
        break;
    }
    case STATE_PHOTO_NEXT:
    case STATE_PHOTO2_NEXT:
        if (func_0203133c(&mTextButtons[1])) {
            mTextButtons[1].mVisible = FALSE;
            SetState(mState + 1);
        }
        break;
    case STATE_PHOTO3_WAIT:
        if (mFrame++ > 60) {
            SetState(mState + 1);
        }
        break;
    case STATE_PHOTO_FADE_OUT:
    case STATE_PHOTO2_FADE_OUT:
        if (ScreenControl_BlendFadeOut(&gScreenControl, 0x3F, -1)) {
            SetState(mState + 1);
        }
        break;
    case STATE_RETURN:
        if (func_0203133c(&mTextButtons[3])) {
            SetState(STATE_LEAVE);
        }
        break;
    case STATE_LEAVE:
        ScreenControl_BlendFadeOut(&gScreenControl, 0x3F3F, 1);
        if (mFrame++ > 180) {
            gSceneManager.RequestScene(0x40);
        }
        break;
    }
    mCast.LipSync(mTtsPlayer.GetPhonemeName());
    mCast.UpdateCharacters(1);
    mSpeechWindow.Update();
}

void TravelScene::Draw() {
    func_02031d7c(&mButtons[0]);
    mCaptions[0].Draw();
    for (int i = 0; i < 4; i++) {
        func_0203206c(&mTextButtons[i]);
    }
    for (int i = 0; i < 5; i++) {
        Sprite_Draw(&mSprites[i]);
    }
    mCast.DrawCharacters();
    mSpeechWindow.Draw();
}

void TravelScene::Exit() {
    mCast.Release();
    ArchiveManager_Unmount(&gArchiveManager, "N00");
    ArchiveManager_Unmount(&gArchiveManager, "N01");
    ArchiveManager_Unmount(&gArchiveManager, "TBM");
    ArchiveManager_Unmount(&gArchiveManager, "SFL");
}

void TravelScene::SetState(int state) {
    static const VecFx32 sPairSpots[2] = {
        {-0x1333, 0x333, -0x1000},
        {0x1333, 0x333, -0x1000},
    };
    static const u16 sSoloAngles[4] = {0x38E3, 0x471C, 0x4000, 0x3555};
    // Where the travellers stand for a photograph: the lone one at one of four spots, never twice at one
    static const VecFx32 sSoloSpots[4] = {
        {-0x1333, 0, 0},
        {0x1000, 0x333, 0x2000},
        {0, 0, -0x2CCC},
        {-0xCCC, 0x333, 0x3333},
    };
    static const u16 sPairAngles[2] = {0x38E3, 0x471C};
    // The files of a sight's picture in the destination's archive; the second "0" becomes the sight's number
    static const char *sSightPictureFiles[3] = {
        "N01:/BG_00.ncg",
        "N01:/BG_00.ncl",
        "N01:/BG_00.nsc",
    };


    char palette[15];
    char screen[15];
    char chars[15];

    switch (state) {
    case STATE_START:
        for (int i = 0; i < mTravellerCount; i++) {
            ModelAnimation *animation =
                AnimModel_FindAnimation(&mCharacters[i].mAnim, RESOURCE_BCA0, mCharacters[i].mAnimId);
            animation->mFinished = FALSE;
            mCharacters[i].SetFace(CharacterBase::FACE_BLINK);
        }
        mSpeechWindow.Hide();
        mTextButtons[0].mVisible = TRUE;
        func_0203119c(&mTextButtons[0]);
        break;
    case STATE_DEPARTURE:
        if (mSceneId == SCENE_PAIR_DRESSED) {
            gSoundManager.StartSeqArc(1, 0x30, 14);
        } else {
            gSoundManager.StartSeqArc(1, 0x30, 13);
        }
        break;
    case STATE_PHOTO_FADE_IN:
    case STATE_PHOTO2_FADE_IN:
    case STATE_PHOTO3_FADE_IN:
        int sight;
        mCaptions[0].mSprite.mVisible = FALSE;
        for (int i = 0; i < mTravellerCount; i++) {
            mCharacters[i].SetAnimation(RESOURCE_BCA0, 0);
            if (mTravellerCount == 1) {
                u16 spot;
                do {
                    spot = Rand(4);
                } while (spot == mLastSpot);
                mLastSpot = spot;
                mCharacters[i].mPos = sSoloSpots[spot];
                mCharacters[i].SetAngle(sSoloAngles[spot]);
            } else {
                mCharacters[i].mPos = sPairSpots[i];
                mCharacters[i].SetAngle(sPairAngles[i]);
            }
            mCharacters[i].SetVisible(TRUE);
            AnimModel_FindAnimation(&mCharacters[i].mAnim, RESOURCE_BCA0, mCharacters[i].mAnimId)->mFinished = FALSE;
            mCharacters[i].SetFace(CharacterBase::FACE_BLINK);
            mCharacters[i].Update();
        }
        switch (state) {
        case STATE_PHOTO_FADE_IN:
            sight = mSights[0];
            break;
        case STATE_PHOTO2_FADE_IN:
            sight = mSights[1];
            break;
        case STATE_PHOTO3_FADE_IN:
            sight = mSights[2];
            break;
        }
        strcpy(palette, sSightPictureFiles[1]);
        strcpy(screen, sSightPictureFiles[2]);
        strcpy(chars, sSightPictureFiles[0]);
        char digit = '0' + sight;
        palette[9] = digit;
        screen[9] = digit;
        chars[9] = digit;
        PaletteLoader_LoadFile(&gPaletteLoader, palette, 0x2F, -1, -1, -1);
        ScreenLoader_LoadFile(&gScreenLoader, screen, 4, 0, 0, 0, 0, -1);
        func_02028f24(&gVramTransferQueue, chars, 0xC, 0, 0, 0, -1);
        gScreenControl.Set(0x1217);
        mSpeechWindow.Hide();
        break;
    case STATE_PHOTO_INTRO:
    case STATE_PHOTO2_INTRO:
    case STATE_PHOTO3_INTRO: {
        int sight;
        switch (state) {
        case STATE_PHOTO_INTRO:
            sight = mSights[0];
            break;
        case STATE_PHOTO2_INTRO:
            sight = mSights[1];
            break;
        case STATE_PHOTO3_INTRO:
            sight = mSights[2];
            break;
        }
        mSpeechWindow.SetMessageArg(mMessageFiles[0].GetText(GetSightName(mDestination, sight)), 0);
        mText.SetMessageArg(mMessageFiles[0].GetText(GetSightReading(mDestination, sight)), 0);
        Speak(0, 0x4AE, 0);
        ScreenControl_SetBlendAlpha(&gScreenControl, 2, 0x1D, 8, 8);
        break;
    }
    case STATE_PHOTO_POSE:
    case STATE_PHOTO2_POSE:
    case STATE_PHOTO3_POSE: {
        mButtons[0].mVisible = TRUE;
        func_0203119c(&mButtons[0]);
        mTtsPlayer.StopEngine();
        // The player, who takes the photograph and says "cheese", speaks with the voice of a resident drawn at random
        SavedCharacter photographer;
        func_0202c264(&gGameData, mCast.mResidents[Rand(mCast.mResidentCount)], &photographer);
        photographer.SetVoiceOf(&mTtsPlayer);
        Speak(-1, 0x4B0, 0);
        mPoseFrame = (photographer.mVoice[4] - 50) * 220 / 250 + 40;
        break;
    }
    case STATE_PHOTO_FLASH:
    case STATE_PHOTO2_FLASH:
    case STATE_PHOTO3_FLASH:
        PaletteLoader_SetBackdrop(&gPaletteLoader, 0, 0x101);
        gScreenControl.Set(0x1200);
        for (int i = 0; i < mTravellerCount; i++) {
            ModelAnimation *animation =
                AnimModel_FindAnimation(&mCharacters[i].mAnim, RESOURCE_BCA0, mCharacters[i].mAnimId);
            animation->mFinished = TRUE;
            if (Rand(3) == 0) {
                mCharacters[i].SetFace(6);
            } else {
                mCharacters[i].SetFace(1);
            }
        }
        break;
    case STATE_PHOTO_NEXT:
    case STATE_PHOTO2_NEXT:
    case STATE_PHOTO3_WAIT:
        mButtons[0].mVisible = FALSE;
        if (state == STATE_PHOTO_NEXT || state == STATE_PHOTO2_NEXT) {
            mTextButtons[1].mVisible = TRUE;
            func_0203119c(&mTextButtons[1]);
        }
        ScreenControl_SetBlendBrightness(&gScreenControl, 0x3F, 0);
        gScreenControl.Set(0x120D);
        break;
    case STATE_RETURN:
        mSpeechWindow.Hide();
        mTextButtons[3].mVisible = TRUE;
        func_0203119c(&mTextButtons[3]);
        mSprites[0].mVisible = TRUE;
        gScreenControl.Set(0x121D);
        break;
    case STATE_LEAVE:
        gSoundManager.StartSeqArc(1, 0x30, 13);
        gSoundManager.MoveSeqVolume(0, 0x7F, 0);
        gSoundManager.MoveSeqVolume(0, 0, 200);
        break;
    }
    mFrame = 0;
    mState = state;
}

// Shows a message in the speech window and speaks it, with the voice and the lips of the traveller "speaker", or
// with the voice the speech engine has if it is -1
void TravelScene::Speak(int speaker, u32 message, int mode) {
    mSpeechWindow.Show(message, mode, 120);
    mCast.mSpeaker = speaker;
    mCast.mSpeakerFace = 1;
    if (speaker != -1) {
        mCast.mCharacters[mCast.mSpeaker]->SetVoiceOf(&mTtsPlayer);
    }
    MI_CpuFill8(mSpeech, 0, sizeof(mSpeech));
    mText.mMessage = message + 1;
    mText.ExpandMessage();
    int length = sizeof(mSpeech);
    STD_ConvertStringUnicodeToSjis(mSpeech, &length, (const u16 *)mText.mText, NULL, NULL);
    mTtsPlayer.Speak(mSpeech, 0);
}

// The messages of a destination's sights: each has its name, then its reading
u32 TravelScene::GetSightName(int destination, int sight) {
    return sSightMessages[destination] + sight * 2;
}

u32 TravelScene::GetSightReading(int destination, int sight) {
    return GetSightName(destination, sight) + 1;
}

#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "SceneManager.h"
#include "SceneMemory.h"
#include "ScreenControl.h"
#include "sound/SoundManager.h"

#include "Marriage2Scene.h"
#include "../../scene/SceneHelpers.h"

#include <string.h>

// Overlay 97: the staff credits after a wedding (Marriage2Scene.h).

extern "C" {
void ModelResources_Reset(void *resources);
BOOL ArchiveManager_IsIdle(ArchiveLoader *loader); // Whether no archive is being loaded
BOOL ScreenControl_BlendFadeIn(ScreenControl *screens, int planes, int step);
BOOL ScreenControl_BlendFadeOut(ScreenControl *screens, int planes, int step);
BOOL GameData_CharacterExists(GameData *game, int index); // Whether the saved character with that index exists
BOOL GameData_GetCharacterSex(GameData *game, int index); // The gender bit of its Mii (set for a woman)
}

// A position in pixels
struct TextPosition {
    int x;
    int y;
};

// When each picture is taken down: the picture, and the frame of the scene by which the next one is to be up
struct PictureTime {
    /* 0x0 */ int mPicture;
    /* 0x4 */ u16 mEndFrame;
}; // 0x8

static const u16 sButtonSequences[1] = {0};
static const TextPosition sButtonSizes[1] = {{24, 22}};
static const TextPosition sButtonPositions[1] = {{242, 13}};
static const ButtonSounds sButtonSounds[1] = {{{13, -1, 0, -1}}};
static const TextPosition sTextPosition = {0, 96};

static const PictureTime sPictureTimes[Marriage2Scene::PICTURE_COUNT] = {
    {0, 0x20C}, {1, 0x418}, {7, 0x603}, {14, 0x7AD}, {3, 0xA1B}, {15, 0xC48}, {11, 0xE54},
    {9, 0xF39}, {13, 0x1060}, {4, 0x126C}, {10, 0x1351}, {19, 0x1478}, {6, 0x14FB}, {2, 0x157E},
    {17, 0x1684}, {8, 0x1707}, {5, 0x178A}, {16, 0x184E}, {18, 0x1A5A}, {12, 0x1CA8}, {20, 0x1EB4},
};

// The two faces' poses on each picture
static const Marriage2Scene::FacePose sFacePoses[Marriage2Scene::PICTURE_COUNT][2] = {
    {{-0x21, 0x16, 0x45, 0xF7, 0x09, 0xFF}, {0x22, 0x0C, 0x40, 0xFD, 0xF1, 0xFF}},
    {{0x3C, -0x04, 0x7E, 0x06, 0xC3, 0x09}, {-0x3F, -0x0F, 0x63, 0x00, 0x0E, 0x0A}},
    {{0x39, -0x24, 0x9E, 0xFF, 0xC5, 0x03}, {-0x2A, -0x20, 0x6E, 0x0C, 0x03, 0x06}},
    {{-0x2A, -0x16, 0x63, 0xFD, 0x11, 0x0F}, {0x21, 0x16, 0x5A, 0x05, 0xF0, 0x0A}},
    {{0x44, 0x0E, 0x69, 0x03, 0xFD, 0xEF}, {-0x46, 0x0D, 0x78, 0x12, 0x27, 0x0A}},
    {{0x3B, -0x37, 0x89, 0xF5, 0xE7, 0x0A}, {-0x35, -0x05, 0x72, 0x14, 0x12, 0x0A}},
    {{-0x24, -0x14, 0x94, 0x00, 0x0A, 0x10}, {0x33, -0x25, 0x96, 0x02, 0xC7, 0xF2}},
    {{0x46, -0x06, 0x57, 0xFB, 0xF6, 0x07}, {-0x43, -0x05, 0x50, 0xFB, 0xF6, 0x03}},
    {{-0x44, 0x08, 0x52, 0xF7, 0x09, 0x00}, {0x4E, 0x03, 0x4B, 0xF6, 0x00, 0x08}},
    {{-0x49, 0x15, 0x68, 0x02, 0x67, 0xF3}, {-0x0A, -0x1C, 0x6E, 0xF9, 0x77, 0xFF}},
    {{-0x4B, 0x06, 0x57, 0xF7, 0x09, 0xF7}, {0x3A, -0x25, 0x7D, 0x06, 0xEB, 0xF4}},
    {{0x32, -0x15, 0x62, 0xF6, 0xCB, 0x0F}, {-0x29, -0x12, 0x5A, 0x09, 0x2B, 0x0F}},
    {{0x3E, -0x1D, 0x73, 0xFE, 0xEB, 0xFC}, {-0x0C, -0x12, 0x67, 0x02, 0xFE, 0x0C}},
    {{-0x17, -0x01, 0x51, 0x05, 0x22, 0xFC}, {0x30, -0x17, 0x4F, 0xF6, 0xD6, 0xFC}},
    {{-0x2A, -0x1A, 0x59, 0xF2, 0x20, 0x03}, {0x30, -0x03, 0x59, 0x05, 0xDE, 0x08}},
    {{0x36, -0x02, 0x4D, 0xF1, 0xEF, 0x02}, {-0x0D, -0x0E, 0x4C, 0xEF, 0xEB, 0x09}},
    {{0x30, 0x02, 0x3E, 0xF9, 0xF0, 0xFE}, {-0x32, -0x0C, 0x3E, 0xFC, 0x1A, 0x0D}},
    {{0x27, -0x0F, 0x5D, 0xFD, 0xF2, 0xF8}, {-0x32, -0x0D, 0x59, 0x02, 0x04, 0x09}},
    {{0x27, -0x0B, 0x4C, 0xF7, 0xCB, 0x09}, {-0x36, -0x1A, 0x46, 0xF7, 0xCB, 0x08}},
    {{-0x19, -0x24, 0x77, 0x04, 0x17, 0x09}, {0x40, 0x00, 0x68, 0x04, 0xF4, 0x02}},
    {{0x24, -0x05, 0x50, 0x01, 0xE6, 0x09}, {-0x20, -0x1B, 0x4E, 0xFB, 0x0A, 0x09}},
};

// The archive of a picture: "Scn/Mrg/mrgNN_LZ.bin", NN put in by MountPicture
static const char *sPictureArchive = "Scn/Mrg/mrg00_LZ.bin";

// Puts a text at a position in pixels
static inline void SetTextPosition(MessageText *text, TextPosition position) {
    text->SetPosition(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
}

void Marriage2Scene::StartLoading() {
    GetCast()->StartLoading();
    ArchiveManager_MountCompressed(&gArchiveManager, "M00", "Scn/Scene/SceneMarriage2_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TBM", "Msg/TmdStaffMsgJP_LZ.bin", 0);
    gSoundManager.StartSeqIfNotPlaying(0, 0x3F);
}

void Marriage2Scene::Exit() {
    gSoundManager.StopStrm(0, 0);
    ModelResources_ReleaseModel(gModelResources, 0);
    ModelResources_Reset(gModelResources);
    GetCast()->ResetCamera();
    ArchiveManager_Unmount(&gArchiveManager, "M00");
    ArchiveManager_Unmount(&gArchiveManager, "TBM");
    ArchiveManager_Unmount(&gArchiveManager, "3BG");
}

void Marriage2Scene::Init() {
    VramManagers_Reset(&gVramManagerWork);
    ModelResources_Reset(gModelResources);
    G3X_SetClearColor(GX_RGB(31, 31, 31), 0, 0x7FFF, 63, FALSE);
    GetCast()->Init(11);

    ScreenLoader_LoadFile(&gScreenLoader, "M00:/BG_down.nsc", 0x200, 0, 0, 0, 0, -1);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "M00:/BG_down.ncg", 0x200, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "M00:/BG_down.ncl", 0x200, -1, -1, -1);
    PaletteLoader_SetBackdrop(&gPaletteLoader, GX_RGB(31, 31, 31), 0x101);
    ObjResourceManager_LoadCell(&gObjResourceManager, "M00:/Obj_down.nce", 0x1000, 0);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "M00:/Obj_down.ncg", 0x1000, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "M00:/Obj_down.ncl", 0x1000, -1, -1, -1);
    mMessageFiles[0].Open("TBM:/tmd_staff_msg_JP.bmg");

    for (int i = 0; i < 1; i++) {
        Sprite_Init(&mButtons[i], 0, 0, 0x80, 0x60, 1);
        SpriteButton_SetTouchRect(&mButtons[i], sButtonSizes[i].x, sButtonSizes[i].y);
        mButtons[i].SetPositionFromVec(sButtonPositions[i].x << FX32_SHIFT, sButtonPositions[i].y << FX32_SHIFT);
        SpriteButton_SetKind(&mButtons[i], 4);
        SpriteButton_SetSequences(&mButtons[i], sButtonSequences[i], sButtonSequences[i] + 1);
        SpriteButton_SetSounds(&mButtons[i], &sButtonSounds[i]);
        SpriteButton_Reset(&mButtons[i]);
    }

    int charOffset = 800;
    mNextMessage = 0;
    for (int i = 0; i < TEXT_COUNT; i++) {
        mTexts[i].InitObjForMessage(&mMessageFiles[0], mNextMessage++, &gFonts[2].mFont, 0x1000, charOffset, 0, 1, 0,
                                    0, 15, NULL);
        SetTextPosition(&mTexts[i], sTextPosition);
        mTexts[i].mHAlign = 0;
        mTexts[i].DrawAll();
        charOffset += mTexts[i].GetCharCount();
    }

    mTtsPlayer.InitEngine();
    GetCast()->mCount = 2;
    if (gSceneManager.mPreviousSceneId == 0xAB) {
        // The couple of the wedding, the man first
        u8 first = gSceneMemory.mCharacters[0];
        u8 second = gSceneMemory.mCharacters[1];
        GameData_CharacterExists(&gGameData, first) && GameData_CharacterExists(&gGameData, second);
        if (!GameData_GetCharacterSex(&gGameData, first)) {
            GetCast()->mChosen[0] = first;
            GetCast()->mChosen[1] = second;
        } else {
            GetCast()->mChosen[0] = second;
            GetCast()->mChosen[1] = first;
        }
    } else if (!GetCast()->Choose()) {
        return;
    }
    GetCast()->ReadCharacters();

    mFrame = 0;
    mPictureIndex = 0;
    MountPicture(sPictureTimes[mPictureIndex].mPicture, 1);
    ShowPicture();
    GetCast()->SetupCamera();
    gScreenControl.Set(0x1219);
    ScreenLoader_SetPriority(&gScreenLoader, 0x10, 0);
    ScreenLoader_SetPriority(&gScreenLoader, 1, 1);
    ScreenLoader_SetPriority(&gScreenLoader, 8, 2);
    ScreenLoader_SetPriority(&gScreenLoader, 0x1000, 1);
    ScreenLoader_SetPriority(&gScreenLoader, 0x200, 2);

    mScroll = 208 << FX32_SHIFT;
    for (int i = 0; i < TEXT_COUNT; i++) {
        mTextRows[i] = -1;
    }
    ScrollCredits();

    // The button only shows from the second time the credits are seen
    if (gGameData.mUnknownB2[1] == 0) {
        gGameData.mUnknownB2[1] = 1;
        mButtons[0].mVisible = FALSE;
    } else {
        mButtons[0].mVisible = TRUE;
    }

    if (mSceneId == 0xC1) {
        SetState(STATE_NONE);
    } else {
        SetState(STATE_CREDITS);
    }
    gSoundManager.StartStrm(0, 2, 0);
}

void Marriage2Scene::Draw() {
    for (int i = 0; i < 2; i++) {
        mModels[i].Draw();
    }
    GetCast()->DrawFaces();
    SpriteButton_Draw(&mButtons[0]);
    for (int i = 0; i < TEXT_COUNT; i++) {
        mTexts[i].Draw();
    }
}

void Marriage2Scene::Update() {
    mTtsPlayer.Update();
    mFrame++;
    switch (mState) {
    case STATE_CREDITS:
        ScrollCredits();
        if (gSoundManager.GetStrmPlayingPos(0) == 0) {
            gSceneManager.RequestScene(0xAB);
        }
        if (mPictureIndex + 1 < PICTURE_COUNT && mFrame + 16 > sPictureTimes[mPictureIndex].mEndFrame) {
            SetState(STATE_FADE_OUT);
        }
        if (SpriteButton_Update(&mButtons[0])) {
            SetState(STATE_SKIP);
        }
        break;
    case STATE_FADE_OUT:
        ScrollCredits();
        if (ScreenControl_BlendFadeOut(&gScreenControl, 0x3F, 1)) {
            SetState(STATE_LOAD);
        }
        if (SpriteButton_Update(&mButtons[0])) {
            SetState(STATE_SKIP);
        }
        break;
    case STATE_LOAD:
        ScrollCredits();
        if (ArchiveManager_IsIdle(&gArchiveManager)) {
            SetState(STATE_FADE_IN);
        }
        break;
    case STATE_FADE_IN:
        ScrollCredits();
        if (ScreenControl_BlendFadeIn(&gScreenControl, 0x3F, -1)) {
            SetState(STATE_CREDITS);
        }
        if (SpriteButton_Update(&mButtons[0])) {
            SetState(STATE_SKIP);
        }
        break;
    case STATE_SKIP:
        ScrollCredits();
        if (mStateFrame++ > 30) {
            gSceneManager.RequestScene(0xAB);
        }
        break;
    case STATE_END:
        if (gSoundManager.GetStrmPlayingPos(0) == 0) {
            gSceneManager.RequestScene(0xAB);
        }
        if (SpriteButton_Update(&mButtons[0])) {
            SetState(STATE_SKIP_END);
        }
        break;
    case STATE_SKIP_END:
        if (mStateFrame++ > 30) {
            gSceneManager.RequestScene(0xAB);
        }
        break;
    }
}

void Marriage2Scene::SetState(int state) {
    switch (state) {
    case STATE_CREDITS:
    case STATE_FADE_OUT:
        break;
    case STATE_LOAD:
        mPictureIndex++;
        MountPicture(sPictureTimes[mPictureIndex].mPicture, 0);
        break;
    case STATE_FADE_IN:
        ShowPicture();
        break;
    case STATE_END:
        break;
    case STATE_SKIP:
    case STATE_SKIP_END:
        gSoundManager.StopStrm(0, 30);
        break;
    }
    mState = state;
    mStateFrame = 0;
}

// Sets the two faces for a picture and mounts its archive as "3BG"
void Marriage2Scene::MountPicture(int picture, int unknown) {
    for (int i = 0; i < 2; i++) {
        SetFacePose(i, &sFacePoses[picture][i]);
    }
    ArchiveManager_Unmount(&gArchiveManager, "3BG");
    ModelResources_ReleaseModel(gModelResources, 0);
    char path[24];
    strcpy(path, sPictureArchive);
    u16 number = picture;
    path[11] = '0' + number % 100 / 10;
    path[12] = '0' + number % 10;
    ArchiveManager_MountCompressed(&gArchiveManager, "3BG", path, unknown);
    mPicture = picture;
}

// Shows the mounted picture: its background, and its body model twice
void Marriage2Scene::ShowPicture() {
    ScreenLoader_LoadFile(&gScreenLoader, "3BG:/bg.nsc", 8, 0, 0, 0, 0, -1);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "3BG:/bg.ncg", 8, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "3BG:/bg.ncl", 8, -1, -1, -1);
    G3X_SetClearColor(GX_RGB(31, 31, 31), 0, 0x7FFF, 63, FALSE);
    ModelResources_SetModel(gModelResources, 0, "3BG:/body.imd");
    for (int i = 0; i < 2; i++) {
        mModels[i].Load(0, i);
        mModels[i].mPosition.y = -1;
    }
    mModels[1].mPosition.z = 0x2000;
}

void Marriage2Scene::SetFacePose(int face, const FacePose *pose) {
    GetCast()->SetTransform(face, 0, pose->x, pose->y, pose->scale, pose->rotX, pose->rotY, pose->rotZ);
}

// Moves the credits up. Each line goes round the screen, 208 pixels; when one comes round to the bottom it takes
// the next message, and when there is none left the credits end.
void Marriage2Scene::ScrollCredits() {
    fx32 scroll = mScroll + 0x555;
    mScroll = scroll;
    for (int i = 0; i < TEXT_COUNT; i++) {
        int row = ((scroll >> FX32_SHIFT) - i * 16) % 208;
        if (row == 0 && mTextRows[i] != 0) {
            if (mNextMessage > 172) {
                SetState(STATE_END);
            } else {
                mTexts[i].mMessage = mNextMessage++;
                NNS_G2dCharCanvasClear(&mTexts[i].mCanvas, 0);
                mTexts[i].DrawAll();
            }
        }
        mTextRows[i] = row;
        mTexts[i].mSprite.mPosition.y = (192 - row) << FX32_SHIFT;
    }
}

Marriage2Scene::~Marriage2Scene() {}

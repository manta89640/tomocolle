#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "sound/SoundManager.h"

#include "TutorialRoomScene.h"

// Overlay 73: the apartment scene as the tutorial shows it (TutorialRoomScene), with its own versions of the
// classes of overlay 74's apartment. It is one source file: its small constants share one pool at the start of
// .rodata that functions throughout the overlay read by offset, it has one static initialiser, and the scene's
// implicit destructor, which the compiler emits with the scene's virtual table after the file's own functions, is
// at the very end of .text.

#define BCA0 RESOURCE_BCA0

// --- The file's constants and its one variable ---
//
// All in one place, and in this order, because their order decides where the compiler puts them. It keeps a list
// of the file's data objects (newest first) and heap-sorts it by size; constants of four-byte alignment are pooled
// into one object at the start of .rodata, which the code reads by offset from its start. So the declaration order
// fixes the offsets in the pool: in particular those of the eight 8-byte points, which the scene's Init and the
// character's entering states read relative to the pool. Swapping two objects of the same size swaps their places.
// This order gives the pool's 8-byte points in the ROM's order (see docs/notes/ov073.md).

struct ButtonPosition {
    int x;
    int y;
};

struct ButtonSequences {
    u16 mNormal;
    u16 mPressed;
};

// The rectangle of the floor the characters walk on. The static initialiser computes its size from its origin.
static FloorRect gWalkArea = { -FX32_CONST(2.5), -FX32_CONST(2.5), -gWalkArea.x * 2, -gWalkArea.z * 2 };
// The message window's button: where it is and how large
static const WindowPoint sWindowButtonPosition = { 128, 149 };
// Where a character coming in by the door walks to (the door is at z -9.0): state 10 inside the door and then to
// the front of the room, state 12 to the doorstep and then past the door
static const FloorPos sInsideDoor = { 0, -0x4800 };
static const FloorPos sRoomFront = { 0, -0x1000 };
// Where each kind of touch button is (the tutorial has kind 0 only)
static const ButtonPosition sButtonPositions[1] = { { 128, 177 } };
static const WindowPoint sWindowButtonSize = { 94, 30 };
static const FloorPos sDoorstep = { 0, -0x6000 };
// The screen (tile map) of the message window's background: the two uses share one address
static const char sWindowScreenName[] = "MLT:/Window_down2.nsc";
static const FloorPos sPastDoor = { 0, -0x3666 };
// The rectangle of the message window, in pixels
static const ScreenRect sWindowRect = { 20, 20, 216, 152 };
// How fast the camera's orbit slows down, and its fastest, in elevation (x) and direction (y). The values are
// folded into TutorialRoomCamera::UpdateOrbit; the objects stay in .rodata, as in the ROM
static const VecFx32 sOrbitFriction = { 0x4000, 0x7000, 0 };
// The rectangle around the cursor on the building front that takes a touch
static const ScreenRect sCursorTouchRect = { -16, -12, 34, 26 };
// How far the building front scrolls per floor, by building size
static const fx32 sScrollPerFloor[4] = { 0x16000, 0x16000, 0x19000, 0x1C000 };
// The id of the head icon of each mood (mood 0 shows none)
static const s16 sMoodHeadIcons[5] = { 4, 5, 0, 1, 7 };
// Which of the seven pictures of the sky each time of day shows
static const s8 sSkyPictures[32] = {
    0, 0, 0, 0, 1, 1, 1, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 1, 0,
};
// The animation sequences of each kind of touch button: normal and pressed
static const ButtonSequences sButtonSequences[1] = { { 2, 3 } };
// Where the message window's text is
static const WindowPoint sWindowTextPosition = { 38, 33 };
static const VecFx32 sOrbitMaxSpeed = { 0x40000, 0x100000, 0 };
// The rectangles of the touch button that take a touch: of kind 0, and of the others
static const ScreenRect sButtonRect0 = { -34, -12, 70, 26 };
static const ScreenRect sButtonRectOther = { -32, -24, 66, 56 };
// The animation sequence and the position of each sprite of the clock
static const u16 sClockSequences[TutorialClockDisplay::SPRITE_COUNT] = { 0, 0, 2, 0, 0, 3, 0, 0, 1, 0, 0 };
static const ButtonPosition sClockPositions[TutorialClockDisplay::SPRITE_COUNT] = {
    { 160, 8 }, { 167, 8 }, { 174, 8 }, { 181, 8 }, { 188, 8 }, { 205, 8 },
    { 222, 8 }, { 229, 8 }, { 236, 8 }, { 243, 8 }, { 250, 8 },
};
// How many rooms a building of each size has
static const u8 sRoomCounts[4] = { 24, 48, 72, 100 };

static inline int Random(int max) {
    return MATH_Rand16(&gRandom, max);
}

// How many characters (tiles) of VRAM a sprite text canvas takes: ObjTextCanvas::GetCharCount, with the canvas
// reached through a pointer. Both uses in Init match in this form; the header's form (mCanvas read directly)
// matches the second use only, so this is probably how the class's own function reads.
static inline int GetCharCount(ObjTextCanvas &text) {
    const NNSG2dCharCanvas *canvas = &text.mCanvas;
    return ((canvas->areaWidth * canvas->areaHeight - 1) >> text.mVramMode) + 1;
}

// Halves a vector in place
static inline void Halve(VecFx32 *v) {
    v->x >>= 1;
    v->y >>= 1;
    v->z >>= 1;
}

// --- TutorialRoomScene ---

// Lists the saved characters, chooses the two to show, and asks for the scene's archives.
// @0x02149a40
void TutorialRoomScene::StartLoading() {
    mClock.Update();
    gSoundManager.StartSeqIfNotPlaying(0, 11);
    gSoundManager.SetSeqTrackMuteOnly(0, 0x1C1, TRUE);
    gSoundManager.SetSeqTrackModDepth(0, 0xFFFF, 0);
    mTutorialStep = data_0213ba74.mTutorialStep;

    for (int i = 0; i < 100; i++) {
        if (func_0202c3dc(&gGameData, i)) {
            mResidents[mResidentCount] = i;
            mResidentCount++;
            mFront.mResidents[func_0202c414(&gGameData, i)] = i;
        }
    }

    if (!ChooseCharacters()) {
        func_0203c02c(&gSceneManager, 0x69);
        return;
    }
    LoadCharacters();
    if (mTutorialStep == 13) {
        // The visitor's event is to visit the resident
        mCharacters[1].mEvent = 0x17;
        mCharacters[1].mEventPartner = mCharacters[0].mIndex;
        mCharacters[0].mEvent = 0x18;
        mCharacters[0].mEventPartner = mCharacters[1].mIndex;
        SaveCharacters();
    } else if (mTutorialStep == 16) {
        mCharacters[0].mEvent = 0;
        mCharacters[1].mEvent = 0;
        SaveCharacters();
    }
    func_02091048(&mCharacters[0], "MT0");
    func_02091048(&mCharacters[1], "MT1");
    mRoom.SetStyle(mCharacters[0].mInterior);
    mRoom.mRoomIndex = mCharacters[0].mRoom;
    mFront.SetBuildingSize(0);
    mSky.Init(&mClock, 0);

    ArchiveManager_MountCompressed(&gArchiveManager, "BIC", "Mdl/Ica/ica_mii_room_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "MLT", "Scn/Scene/ScenePlayRoomTut_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "EFT", "Mdl/Spl/SplRoomEffect_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TCM", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TEM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TMM", "Msg/TmdMainRoomMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "ASP", "Scn/AFwBG/FwBgSkyPlt_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "APX", mFront.mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "ASX", mSky.mArchivePath, 0);
    func_020a54c4(&mRoom, "CO0", 0);
    func_020a56bc(&mRoom, "RM?", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[0].mArchiveName, mCharacters[0].mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[1].mArchiveName, mCharacters[1].mArchivePath, 0);
}

// Sets up the screens, the room, the two characters, the message window and the touch screen, when the archives
// are loaded, and starts the part of the tutorial that is due.
//
// The window's button position, size and text position are read from the pool of constants at the start of
// .rodata (see the constants at the top). Two things matter besides: func_02027290 is declared returning a value,
// and the character counts read the canvas through a pointer (GetCharCount above).
// @0x02149d8c
void TutorialRoomScene::Init() {
    int i;
    void *file;
    void *animations;

    func_0202b0e8(&data_02122824);
    data_0211d604.mUnknown48 = 1;
    data_0211d604.mUnknownEC = 1;
    G3X_SetClearColor(GX_RGB(1, 1, 1), 0, 0x7FFF, 63, FALSE);
    func_0202680c(0x4000);

    func_02028f24(&gVramTransferQueue, "MLT:/BG_down.ncg", 0x100, 0, 0, 0x4000, -1);
    func_02027534(&data_021213b8, "MLT:/BG_down.ncl", 0x100, -1, -1, -1);
    func_02027d7c(&data_02121834, sWindowScreenName, 0x100, 0, 0, 0, 0, -1);
    mFront.mScreen = func_02027d7c(&data_02121834, "APX:/BG_window.nsc", 0x200, 0, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, "APX:/BG_wall.nsc", 0x400, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "APX:/BG_down.ncg", 0x600, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "APX:/BG_down.ncl", 0x600, -1, -1, 0x140);
    func_02028f24(&gVramTransferQueue, "MLT:/BG_sub.ncg", 2, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "MLT:/BG_sub.ncl", 2, -1, -1, -1);
    func_02027d7c(&data_02121834, "MLT:/BG_dialogue.nsc", 2, 0, 0, 0, 0, -1);
    func_02027290(&gObjResourceManager, "MLT:/Obj_down.nce", 0x1000, 0);
    func_02028f24(&gVramTransferQueue, "MLT:/Obj_down.ncg", 0x1000, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "MLT:/Obj_down.ncl", 0x1000, -1, -1, -1);
    ObjResourceManager_Load(&gObjResourceManager, "MLT:/Obj_up.nce", "MLT:/Obj_up.ncg", 0x10, 1, -1);
    func_02027534(&data_021213b8, "MLT:/Obj_up.ncl", 0x10, -1, -1, -1);
    mSky.mColors = (u16 *)func_02027534(&data_021213b8, "ASP:/Fw_BG_sky.ncl", 0x2F00, 0, 0, 2);
    func_02028f24(&gVramTransferQueue, mSky.mCharPath, 0x400, 0, 0x1000, 0x3000, 0);
    mSky.mPalette = func_02027534(&data_021213b8, mSky.mPalettePath, 0x400, 0x80, 0x80, 0xC0);
    func_020278f8(&data_021213b8, 0, 0x101);
    func_02028280(&data_02121834, 1, 2);
    func_02028280(&data_02121834, 2, 1);
    data_0211d604.Set(0x1600);
    data_0211d604.Show(0x11);
    func_02023ee0(&data_0211d604, 2, 0x11, 8, 8);

    mEtcMessages.Open("TEM:/tmd_etc_msg_JP.bmg");
    mCommonMessages.Open("TCM:/tmd_common_msg_JP.bmg");
    mRoomMessages.Open("TMM:/tmd_mainroom_msg_JP.bmg");
    mRoom.Refresh();
    func_020a5514(&mRoom, 3);
    func_020a5760(&mRoom, 4);
    func_020a4acc(&mRoom.mParts, 10);

    file = ArchiveManager_GetFileByName(&gArchiveManager, "EFT:/SplRoomEffect.spa");
    func_02080e98(&mParticles, file, 3, 0x50, 0x15, 0x16, 0x1E);
    func_02080ee4(&mParticles);
    mIcon[0].Load(&mParticles, 0);

    func_0202ad04(data_021223e4, 0, mCharacters[0].mArchiveName, 0);
    func_0202ad04(data_021223e4, 1, mCharacters[1].mArchiveName, 0);
    func_02090fdc(&mCharacters[0], 0, 0, 0);
    func_02090fdc(&mCharacters[1], 1, 0, 1);
    mCharacters[0].mPartner = &mCharacters[1];
    mCharacters[1].mPartner = &mCharacters[0];
    animations = func_0202ad04(data_021223e4, 2, "BIC", 0);
    for (i = 0; i < 2; i++) {
        LoadCharacter(&mCharacters[i]);
        func_0208fefc(&mCharacters[i]);
        func_02091a24(&mCharacters[i]);
        mCharacters[i].mPersonality = func_02098f2c(&mCharacters[i]);
        func_02091a6c(&mCharacters[i]);
        func_020910d4(&mCharacters[i]);
        func_02091a88(&mCharacters[i]);
        mCharacters[i].CopyName();
        func_02029660(mCharacters[i].mModel.mUnknown04, RESOURCE_BCA0, animations);
        Character_SetAngle(&mCharacters[i], 0x4000);
        mCharacters[i].LoadHeadIcon(&mParticles);
        mCharacters[i].mApartmentRoom = &mRoom;
        mCharacters[i].mCamera = &mCamera;
    }

    mCamera.Reset();
    mCamera.mCharacters[0] = &mCharacters[0];
    mCamera.mCharacters[1] = &mCharacters[1];
    mCamera.mRoom = &mRoom;
    mLight.Init(&mCamera, &mRoom);

    int charOffset = 0x200;
    ScreenRect rect = sWindowRect;
    func_02032fb8(&mWindow[0], rect);
    func_02033174(&mWindow[0], sWindowScreenName, 0x100);
    func_020331bc(&mWindow[0], 0x3F00, rect);
    func_020336b4(&mWindow[0], 0, 0, 6, 7, sWindowButtonPosition, sWindowButtonSize, &mCommonMessages, 4,
                  &gFonts[2].mFont, 0x1000, &charOffset, -1, -1, 0, 10);
    func_02033a58(&mWindow[0], &mEtcMessages, 0x159, &gFonts[0].mFont, 0x1000, sWindowTextPosition, &charOffset);
    func_02034f94(&mWindow[0], 1);
    func_02034f4c(&mWindow[0], 1);
    func_02028280(&data_02121834, 0x100, 0);
    mWindow[0].mTexts[0].SetStringArg(mCharacters[0].mNameText, 0);

    mButtonLabel[0].InitObjForMessage(&mEtcMessages, 0xB3, &gFonts[0].mFont, 0x1000, charOffset, 0, 1, 0, 0, 15,
                                      NULL);
    mButtonLabel[0].mVAlign = 1;
    mButtonLabel[0].mShadowDirections = 0xAA;
    mButtonLabel[0].DrawAll();
    charOffset += GetCharCount(mButtonLabel[0]);

    int speechOffset = 0xC0;
    mTimedMessage.InitObjForMessage(&mRoomMessages, 3, &gFonts[0].mFont, 0x10, speechOffset, 0, 1, 0, 0, 15, NULL);
    mTimedMessage.mSprite.SetPosition(0x10000, 0xA0000);
    mTimedMessage.mVAlign = 1;
    speechOffset += GetCharCount(mTimedMessage);
    mNamePanel.SetNames(mRoom.mNumberText, mCharacters[0].mNameText);
    mNamePanel.Init(speechOffset, &mRoomMessages, &mRoomMessages);

    mButtons[0].mLabel = &mButtonLabel[0];
    mButtons[0].Init(0);
    mPressedButton = 0;
    mFront.Init(mCharacters[0].mRoom);
    mSky.Refresh();
    mClockDisplay.Init(&mClock);

    StartTutorialStep();
    if (mCharacters[1].mPresent) {
        mCamera.SetMode(TutorialRoomCamera::MODE_BOTH);
    } else if (mCharacters[0].mPresent) {
        mCamera.SetMode(TutorialRoomCamera::MODE_CHARACTER);
    } else {
        mCamera.SetMode(TutorialRoomCamera::MODE_ROOM);
    }
    gSoundManager.LoadGroup(0x4C);
}

// @0x0214a784
void TutorialRoomScene::Update() {
    if (data_0211f9d4.mHeld & PAD_KEY_RIGHT) {
        mCamera.mInput.y = 0x10000;
    } else if (data_0211f9d4.mHeld & PAD_KEY_LEFT) {
        mCamera.mInput.y = -0x10000;
    }
    if (data_0211f9d4.mHeld & PAD_KEY_UP) {
        mCamera.mInput.x = 0xE000;
    } else if (data_0211f9d4.mHeld & PAD_KEY_DOWN) {
        mCamera.mInput.x = -0xE000;
    }
    UpdateState();
    UpdateObjects();
}

// @0x0214a814
void TutorialRoomScene::Draw() {
    UpdateFade();
    func_020a4a50(&mRoom.mParts);
    for (int i = 0; i < 2; i++) {
        mCharacters[i].Draw();
    }
    func_02080f00(&mParticles, 0);
    mTimedMessage.Draw();
    func_020345d8(&mWindow[0]);
    mButtonLabel[0].Draw();
    mNamePanel.Draw();
    mButtons[0].Draw();
    mFront.Draw();
    mClockDisplay.Draw();
}

// @0x0214a8b8
void TutorialRoomScene::Exit() {
    SaveCharacters();
    func_02023f4c(&data_0211d604, 0x3F00, 0);
    func_02023eac(&data_0211d604, 0x3F3F);
    func_0202683c();
    func_0202aecc(data_021223e4, 0);
    func_0202aecc(data_021223e4, 1);
    func_0202aecc(data_021223e4, 2);
    func_020a5948(&mRoom);
    ArchiveManager_Unmount(&gArchiveManager, "BIC");
    ArchiveManager_Unmount(&gArchiveManager, "TEM");
    ArchiveManager_Unmount(&gArchiveManager, "TCM");
    ArchiveManager_Unmount(&gArchiveManager, "TMM");
    ArchiveManager_Unmount(&gArchiveManager, "MLT");
    ArchiveManager_Unmount(&gArchiveManager, "EFT");
    ArchiveManager_Unmount(&gArchiveManager, "APX");
    ArchiveManager_Unmount(&gArchiveManager, "ASP");
    ArchiveManager_Unmount(&gArchiveManager, "ASX");
    ArchiveManager_Unmount(&gArchiveManager, mCharacters[0].mArchiveName);
    ArchiveManager_Unmount(&gArchiveManager, mCharacters[1].mArchiveName);
}

// Chooses the resident and the visitor from the saved characters that exist, for the part of the tutorial that is
// due. Returns FALSE if the scene has no part for the tutorial's step.
// @0x0214a9d8
BOOL TutorialRoomScene::ChooseCharacters() {
    switch (mTutorialStep) {
    case 8:
        if (mResidentCount == 0) {
            UseDefaultCharacters();
        } else if (mResidentCount == 1) {
            mCharacters[0].mIndex = mResidents[0];
            if (mResidents[0] != 1) {
                mCharacters[1].mIndex = 1;
            } else {
                mCharacters[1].mIndex = 2;
            }
        } else {
            mCharacters[0].mIndex = mResidents[0];
            mCharacters[1].mIndex = mResidents[1];
        }
        break;
    case 13:
        if (mResidentCount <= 1) {
            UseDefaultCharacters();
        } else {
            mCharacters[0].mIndex = mResidents[1];
            mCharacters[1].mIndex = mResidents[0];
        }
        break;
    case 16:
        if (mResidentCount <= 1) {
            UseDefaultCharacters();
        } else {
            mCharacters[0].mIndex = mResidents[1];
            mCharacters[1].mIndex = mResidents[0];
        }
        break;
    case 21:
        if (mResidentCount <= 1) {
            UseDefaultCharacters();
        } else {
            mCharacters[0].mIndex = mResidents[1];
            mCharacters[1].mIndex = mResidents[0];
        }
        break;
    default:
        return FALSE;
    }
    return TRUE;
}

// The first two saved characters, whether they exist or not.
// @0x0214ab2c
void TutorialRoomScene::UseDefaultCharacters() {
    mCharacters[0].mIndex = 0;
    mCharacters[1].mIndex = 1;
}

// Starts the part of the tutorial that is due: the state, who is in the room and where, the camera and the light.
// @0x0214ab48
void TutorialRoomScene::StartTutorialStep() {
    switch (mTutorialStep) {
    case 8:
        SetState(STATE_RESIDENT_ARRIVES);
        func_02023f4c(&data_0211d604, 0x1F00, -10);
        mCamera.mOrbit.y = 0x1400000;
        mLight.SetState(TutorialRoomLight::STATE_SET_ROOM_COLOR);
        break;
    case 13:
        SetState(STATE_VISITOR_ARRIVES);
        mCharacters[0].mPresent = TRUE;
        mCharacters[0].SetState(4);
        Character_SetAngle(&mCharacters[0], 0x1800);
        func_02023f4c(&data_0211d604, 0x1F00, -10);
        mCamera.mOrbit.y = 0x1500000;
        mLight.SetState(TutorialRoomLight::STATE_SET_STYLE_COLOR);
        mTimedMessage.SetState(TutorialTimedMessage::STATE_SHOW);
        mTimedMessage.SetStringArg(mCharacters[0].mNameText, 0);
        mTimedMessage.mMessage = 3;
        break;
    case 16: {
        SetState(STATE_EXPLAIN);
        mCharacters[0].mPresent = TRUE;
        mCharacters[0].SetState(3);
        mCharacters[0].mWait = 120;
        mCharacters[0].mModel.mPos.x = -0x1000;
        Character_SetAngle(&mCharacters[0], 0x2000);
        FloorPos target;
        mCharacters[0].GetRandomPosInArea(&target);
        mCharacters[0].SetWalkTarget(target.x, 0, target.z);
        func_02023f4c(&data_0211d604, 0x1F00, -10);
        mCamera.mOrbit.y = -0x1000000;
        mLight.SetState(TutorialRoomLight::STATE_SET_STYLE_COLOR);
        mTimedMessage.SetState(TutorialTimedMessage::STATE_SHOW);
        mTimedMessage.SetStringArg(mCharacters[0].mNameText, 0);
        mTimedMessage.mMessage = 3;
        break;
    }
    case 21: {
        SetState(STATE_WAIT_TOUCH);
        mCharacters[0].mPresent = TRUE;
        mCharacters[0].SetState(3);
        mCharacters[0].mWait = 120;
        mCharacters[0].mModel.mPos.x = 0x1000;
        Character_SetAngle(&mCharacters[0], 0x5800);
        FloorPos target;
        mCharacters[0].GetRandomPosInArea(&target);
        mCharacters[0].SetWalkTarget(target.x, 0, target.z);
        mCamera.mOrbit.y = 0x1200000;
        mLight.SetState(TutorialRoomLight::STATE_SET_STYLE_COLOR);
        mTimedMessage.SetState(TutorialTimedMessage::STATE_SHOW);
        mTimedMessage.SetStringArg(mCharacters[0].mNameText, 0);
        mTimedMessage.mMessage = 3;
        break;
    }
    }
}

// The script of each part of the tutorial.
// @0x0214ae08
void TutorialRoomScene::UpdateState() {
    switch (mState) {
    case STATE_NONE:
        break;

    case STATE_RESIDENT_ARRIVES:
        if (mStep == 0) {
            mTimer++;
            if (mTimer > 55) {
                mStep++;
                mTimer = 0;
                mLight.SetState(TutorialRoomLight::STATE_FADE_STYLE_COLOR);
                mCharacters[0].mEvent = 0;
                SaveCharacter(&mCharacters[0]);
                mFront.DrawWindow(mCharacters[0].mRoom);
            }
        } else if (mStep == 1) {
            mTimer++;
            if (mTimer > 60) {
                mStep++;
                mCharacters[0].mPresent = TRUE;
                mCharacters[0].SetState(10);
                mCamera.SetMode(TutorialRoomCamera::MODE_CHARACTER);
                ShowMessage(&mCharacters[0], NULL, 9, 2);
            }
        } else if (mStep == 2) {
            if (mCharacters[0].mState == 0) {
                mStep++;
                mTimer = 0;
                mCharacters[0].SetState(1);
                mCharacters[0].mWait = 80;
            }
        } else if (mStep == 3) {
            mTimer++;
            if (mTimer > 240) {
                mStep++;
                func_02023f4c(&data_0211d604, 0x1F00, 0);
                func_02034f38(&mWindow[0], 0, 0x159);
                func_02034a3c(&mWindow[0], 0, 0, mCharacters[0].mNameText);
                func_02034720(&mWindow[0], MessageWindow::FLAG_OPEN | MessageWindow::FLAG_FROM_BELOW, 0);
            }
            UpdateIdle();
        } else if (mStep == 4) {
            if (mWindow[0].mResult != -1) {
                mStep++;
                mWindow[0].mUnknownSetBeforeClose = 5;
                func_020349cc(&mWindow[0], MessageWindow::FLAG_CLOSE | MessageWindow::FLAG_FROM_BELOW);
            }
            UpdateIdle();
        } else if (mStep == 5) {
            if (mWindow[0].mTransitionDone) {
                mStep++;
            }
            UpdateIdle();
        } else {
            if (UpdateTouch()) {
                mStep++;
                SetState(STATE_ENDING);
            } else {
                UpdateIdle();
            }
        }
        break;

    case STATE_VISITOR_ARRIVES:
        if (mStep == 0) {
            mTimer++;
            if (mTimer > 170) {
                mStep++;
                mTimer = 0;
                gSoundManager.StartSeqArc(1, 0x1D, 16);
            }
        } else if (mStep == 1) {
            mTimer++;
            if (mTimer > 40) {
                mStep++;
                mTimer = 0;
                mCharacters[1].mPresent = TRUE;
                mCharacters[1].SetState(11);
                mCharacters[0].SetState(14);
                mCamera.SetMode(TutorialRoomCamera::MODE_BOTH);
            }
        } else if (mStep == 2) {
            mTimer++;
            if (mTimer > 60) {
                mStep++;
                mTimer = 0;
                mCharacters[1].SetState(12);
                mTimedMessage.SetState(TutorialTimedMessage::STATE_SHOW);
                mTimedMessage.SetStringArg(mCharacters[1].mNameText, 0);
                mTimedMessage.mMessage = 9;
            }
        } else if (mStep == 3) {
            if (mCharacters[1].mState == 0) {
                mStep++;
                mCharacters[1].SetState(5);
            }
        } else {
            if (AreCharactersNear(0, 1)) {
                SetState(STATE_TALK);
                mCharacters[0].SetState(6);
                mCharacters[1].SetState(6);
            }
        }
        break;

    case STATE_TALK:
        if (mStep == 0) {
            if (mCharacters[0].mState == 0 && mCharacters[1].mState == 0) {
                mStep++;
                mCharacters[1].SetState(7);
                mCharacters[1].mWait = 160;
                mCharacters[0].mWait = 150;
                func_02099088(&mCharacters[1], mCharacters[0].mIndex, data_021135c8[0][1] * 2);
            }
        } else {
            // The one whose wait has run out says something, and both like each other more for it
            TutorialRoomCharacter *speaker = NULL;
            TutorialRoomCharacter *listener;
            for (int i = 0; i < 2; i++) {
                if (mCharacters[i].mWait <= 0) {
                    speaker = &mCharacters[i];
                    listener = speaker->mPartner;
                    break;
                }
            }
            if (speaker == NULL) {
                break;
            }
            if (Random(100) < 70) {
                speaker->SetState(7);
                func_02099088(speaker, listener->mIndex, data_021135c8[0][1] * 2);
            } else {
                speaker->SetState(8);
                func_02099088(speaker, listener->mIndex, data_021135c8[0][10] * 2);
            }
            speaker->mWait = 180;
            listener->mWait = 170;
            if (speaker->mAffinity[listener->mIndex] >= 200 && listener->mAffinity[speaker->mIndex] >= 200) {
                SetState(STATE_BECOME_FRIENDS);
            }
        }
        break;

    case STATE_BECOME_FRIENDS:
        if (mStep == 0) {
            if (mCharacters[0].mWait <= 0 && mCharacters[1].mWait <= 0) {
                mStep++;
                mTimer = 0;
                func_020990b0(&mCharacters[0], mCharacters[1].mIndex, 1, 100);
                func_020990b0(&mCharacters[1], mCharacters[0].mIndex, 1, 100);
                mCharacters[0].SetState(9);
                mCharacters[1].SetState(9);
                // The mark goes halfway between the two
                VecFx32 position = CharacterModel_GetJointPosition(&mCharacters[0].mModel, 3);
                VecFx32 other = CharacterModel_GetJointPosition(&mCharacters[1].mModel, 3);
                VEC_Add(&position, &other, &position);
                Halve(&position);
                VEC_Set(&mIcon[0].mPos, position.x, position.y, position.z);
            }
        } else if (mStep == 1) {
            mTimer++;
            if (mTimer > 40) {
                mStep++;
                mIcon[0].Show();
                mIcon[0].mIconId = 4;
                mIcon[0].Reload();
                HeadIcon_SetMode(&mIcon[0], 2);
                gSoundManager.StartSeqArc(1, 0x26, 0);
                mTimedMessage.SetState(TutorialTimedMessage::STATE_SHOW);
                mTimedMessage.SetStringArg(mCharacters[0].mNameText, 0);
                mTimedMessage.SetStringArg(mCharacters[1].mNameText, 1);
                mTimedMessage.mMessage = 10;
            }
        } else {
            if (mCharacters[0].mState == 0 && mCharacters[1].mState == 0 && mTimedMessage.mDrawMode == 0) {
                SetState(STATE_EXPLAIN_FRIENDS);
                mCharacters[0].SetState(1);
                mCharacters[1].SetState(1);
                HeadIcon_SetMode(&mIcon[0], 1);
            }
        }
        break;

    case STATE_EXPLAIN_FRIENDS:
        if (mStep == 0) {
            mStep++;
            mWindow[0].mTexts[0].mMessage = 0x165;
            func_02034720(&mWindow[0], MessageWindow::FLAG_OPEN | MessageWindow::FLAG_FROM_BELOW, 0);
        } else if (mStep == 1) {
            if (mWindow[0].mResult != -1) {
                mStep++;
                mWindow[0].mUnknownSetBeforeClose = 15;
                func_020349cc(&mWindow[0], 0x1C);
            }
        } else if (mStep == 2) {
            if (mWindow[0].mTransitionDone) {
                mStep++;
                mWindow[0].mTexts[0].mMessage = 0x166;
                func_02034720(&mWindow[0], 0x1A, 0);
            }
        } else if (mStep == 3) {
            if (mWindow[0].mResult != -1) {
                mStep++;
                mWindow[0].mUnknownSetBeforeClose = 5;
                func_020349cc(&mWindow[0], 0x1C);
            }
        } else if (mStep == 4) {
            if (mWindow[0].mTransitionDone) {
                mStep++;
                mWindow[0].mTexts[0].mMessage = 0x167;
                func_02034720(&mWindow[0], 0x1A, 0);
            }
        } else if (mStep == 5) {
            if (mWindow[0].mResult != -1) {
                mStep++;
                mWindow[0].mUnknownSetBeforeClose = 5;
                func_020349cc(&mWindow[0], 0x1C);
            }
        } else if (mStep == 6) {
            if (mWindow[0].mTransitionDone) {
                mStep++;
                mTimer = 0;
            }
        } else {
            mTimer++;
            if (mTimer > 50) {
                SetState(STATE_LEAVE);
            }
        }
        break;

    case STATE_EXPLAIN:
        if (mStep == 0) {
            mTimer++;
            if (mTimer > 60) {
                mStep++;
                func_02023f4c(&data_0211d604, 0x1F00, 0);
                func_02034f38(&mWindow[0], 0, 0x16C);
                func_02034720(&mWindow[0], MessageWindow::FLAG_OPEN | MessageWindow::FLAG_FROM_BELOW, 0);
            }
            UpdateIdle();
        } else if (mStep == 1) {
            if (mWindow[0].mResult != -1) {
                mStep++;
                mWindow[0].mUnknownSetBeforeClose = 5;
                func_020349cc(&mWindow[0], MessageWindow::FLAG_CLOSE | MessageWindow::FLAG_FROM_BELOW);
            }
            UpdateIdle();
        } else if (mStep == 2) {
            if (mWindow[0].mTransitionDone) {
                mStep++;
            }
            UpdateIdle();
        } else {
            if (UpdateTouch()) {
                SetState(STATE_ENDING);
            } else {
                UpdateIdle();
            }
        }
        break;

    case STATE_WAIT_TOUCH:
        if (mStep == 0) {
            if (UpdateTouch()) {
                SetState(STATE_ENDING);
            } else {
                UpdateIdle();
            }
        }
        break;

    case STATE_ENDING:
        UpdateEnding();
        break;

    case STATE_LEAVE:
        SetState(STATE_LEAVING);
        func_0203c02c(&gSceneManager, 0x69);
        break;

    case STATE_LEAVING:
        break;
    }
}

// Reads the clock and updates everything in the room and on the touch screen.
// @0x0214ba98
void TutorialRoomScene::UpdateObjects() {
    mClock.UpdateIfArchiveIdle();
    mCamera.Update();
    func_020341d4(&mWindow[0]);
    func_0207b138(mParticles.mManager);
    for (int i = 0; i < 2; i++) {
        mCharacters[i].Update();
    }
    mIcon[0].Update();
    mRoom.Update();
    mLight.Update();
    mFront.Update();
    mSky.Update();
    mTimedMessage.Update();
    mNamePanel.Update();
    mClockDisplay.Update();
    mButtons[0].Update();
}

// While the scene is being shown, the objects are updated from Draw (Update does not run yet).
// @0x0214bb64
void TutorialRoomScene::UpdateFade() {
    switch (mFadeState) {
    case FADE_NONE:
        break;
    case FADE_SHOW:
        mFadeStep++;
        if (mFadeStep > 3 || mPhase > PHASE_FADE_IN) {
            SetFadeState(FADE_NONE);
        } else {
            UpdateObjects();
        }
        break;
    }
}

// @0x0214bbb4
void TutorialRoomScene::SetState(int state) {
    mState = state;
    mStep = 0;
    mTimer = 0;
}

// @0x0214bbcc
void TutorialRoomScene::SetFadeState(int state) {
    mFadeState = state;
    mFadeStep = 0;
}

// The touch screen: the button and the cursor on the building front. TRUE when the player chose either.
// @0x0214bbe0
BOOL TutorialRoomScene::UpdateTouch() {
    if (mTouchStep == 0) {
        if (gTouchInput.mTouched) {
            ScreenPoint point = gTouchInput.mPoint;
            BOOL handled = FALSE;

            if (mButtons[0].Contains(point)) {
                handled = TRUE;
                mTouchStep++;
                mPressedButton = 0;
                mButtons[0].mState = 1;
                gSoundManager.StartSeqArc(1, 1, 0);
            }
            if (!handled) {
                if (mFront.mCursor.Contains(point)) {
                    gSoundManager.StartSeqArc(2, 9, 0);
                    return TRUE;
                }
            }
        }
    } else {
        if (gTouchInput.mTouching != 0) {
            if (mButtons[mPressedButton].Contains(gTouchInput.mPoint)) {
                mButtons[mPressedButton].mState = 1;
            } else {
                mButtons[mPressedButton].mState = 2;
            }
        } else {
            mTouchStep = 0;
            if (mButtons[mPressedButton].Contains(gTouchInput.mLastPoint)) {
                mButtons[mPressedButton].mState = 3;
                gSoundManager.StartSeqArc(2, 9, 0);
                return TRUE;
            } else {
                mButtons[mPressedButton].mState = 2;
            }
        }
    }
    return FALSE;
}

// The resident looks at the camera, turns to it and comes up to it (states 0x10 to 0x12); then the scene goes on
// to its state 9.
// @0x0214bdc0
void TutorialRoomScene::UpdateEnding() {
    if (mStep == 0) {
        mStep++;
        mCharacters[0].SetState(0x10);
        mCamera.mIgnoreInput = TRUE;
    } else if (mStep == 1) {
        if (mCharacters[0].mState == 0) {
            mStep++;
            mCharacters[0].SetState(0x11);
        }
    } else if (mStep == 2) {
        if (mCharacters[0].mState == 0) {
            mStep++;
            mCharacters[0].SetState(0x12);
        }
    } else if (mStep == 3) {
        if (mCharacters[0].mState == 0) {
            mStep++;
            mTimer = 0;
        }
    } else {
        mTimer++;
        if (mTimer > 10) {
            SetState(9);
            gSoundManager.StartSeqArc(2, 1, 11);
        }
    }
}

// @0x0214bec4
void TutorialRoomScene::ShowMessage(TutorialRoomCharacter *first, TutorialRoomCharacter *second, int message, int drawMode) {
    if (first != NULL) {
        mTimedMessage.SetStringArg(first->mNameText, 0);
    }
    if (second != NULL) {
        mTimedMessage.SetStringArg(second->mNameText, 1);
    }
    mTimedMessage.mColor = 1;
    mTimedMessage.SetState(drawMode);
    mTimedMessage.mMessage = message;
}

// @0x0214bf48
BOOL TutorialRoomScene::AreCharactersNear(int first, int second) {
    if (mCharacters[first].IsWithinReach(&mCharacters[second], 0x2000, 0x2999) ||
        mCharacters[second].IsWithinReach(&mCharacters[first], 0x2000, 0x2999)) {
        return TRUE;
    }
    return FALSE;
}

// When the resident has nothing to do: stand for two seconds, or walk to a random place for three.
// @0x0214c004
void TutorialRoomScene::UpdateIdle() {
    if (mCharacters[0].mWait > 0 && mCharacters[0].mState != 0) {
        return;
    }
    if (Random(100) < 70) {
        mCharacters[0].SetState(2);
        mCharacters[0].mWait = 120;
    } else {
        mCharacters[0].SetState(3);
        mCharacters[0].mWait = 180;
        FloorPos pos;
        mCharacters[0].GetRandomPosInArea(&pos);
        mCharacters[0].SetWalkTarget(pos.x, 0, pos.z);
    }
}

// @0x0214c0c8
void TutorialRoomScene::SaveCharacter(SavedCharacter *character) {
    u16 index = character->mIndex;
    if (func_0202c3dc(&gGameData, index)) {
        func_0202c28c(&gGameData, index, character);
    }
}

// @0x0214c104
void TutorialRoomScene::SaveCharacters() {
    SaveCharacter(&mCharacters[0]);
    SaveCharacter(&mCharacters[1]);
}

// @0x0214c128
void TutorialRoomScene::LoadCharacter(SavedCharacter *character) {
    u16 index = character->mIndex;
    if (func_0202c3dc(&gGameData, index)) {
        func_0202c264(&gGameData, index, character);
    } else {
        func_020979e0(character);
        func_02098c40(character, 40, index & 7);
        character->mIndex = index;
    }
}

// @0x0214c188
void TutorialRoomScene::LoadCharacters() {
    LoadCharacter(&mCharacters[0]);
    LoadCharacter(&mCharacters[1]);
}

// @0x0214c1ac
TutorialRoomCharacter::TutorialRoomCharacter() {
    mState = mPrevState = mPresent = 0;
    mWait = 0;
    MI_CpuClear16(mNameText, sizeof(mNameText));
    mSoundArmed = 0;
}

// @0x0214c20c
void TutorialRoomCharacter::SetState(int state) {
    mPrevState = mState;
    mState = state;
    mStep = 0;
}

// @0x0214c22c
void TutorialRoomCharacter::Update() {
    if (!mPresent) {
        return;
    }
    int state = mState;
    if (state != 0) {
        if (state <= 4) {
            UpdateStates01To04();
        } else if (state <= 8) {
            UpdateStates05To08();
        } else if (state <= 9) {
            UpdateState09();
        } else if (state <= 0xf) {
            UpdateStates0ATo0F();
        } else if (state <= 0x12) {
            UpdateStates10To12();
        } else {
            SetState(0);
        }
    }
    if (mWait > 0) {
        mWait--;
    }
    func_020910a8(this);
    mHeadIcon.Update();
}

// States 1 to 4: standing (1), standing with the head turned somewhere (2), walking to the walk target (3) and
// standing in animation 0x19 (4).
// @0x0214c2d4
void TutorialRoomCharacter::UpdateStates01To04() {
    switch (mState) {
    case 1:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(BCA0, GetIdleAnim());
                Character_SetFace(this, 8);
                ShowHeadIcon();
            }
        }
        break;
    case 2:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(BCA0, GetIdleAnim());
                Character_SetFace(this, 8);
                ShowHeadIcon();
                SetHeadTurn(0, (0x28 - Random(0x50)) << 8, 0, 5);
            }
        }
        break;
    case 3:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                ResetSoundPhaseUnlessWalking();
                SetAnimation(BCA0, GetWalkAnim());
                Character_SetFace(this, 8);
                mHeadTurnFrames = 5;
                ShowHeadIcon();
            }
        } else {
            if (StepWalkLooking(0x3800, 0, 0x199, 0x22, 6)) {
                SetState(0);
            }
        }
        break;
    case 4:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound0UnlessGroupA();
                SetAnimation(BCA0, 0x19);
                Character_SetFace(this, 8);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 5);
            }
        }
        break;
    }
    PlaceHeadIcon();
}

// States 5 to 8: walking up to the partner (5), turning to the walk target (6), and two animations with a sound
// (7: animation 0x55 with head icon 13, 8: animation 0x4A).
// @0x0214c5a4
void TutorialRoomCharacter::UpdateStates05To08() {
    switch (mState) {
    case 5:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                ResetSoundPhaseUnlessWalking();
                SetAnimation(BCA0, GetWalkAnim());
                Character_SetFace(this, 8);
                ShowHeadIcon();
                mHeadTurnFrames = 4;
            }
        } else if (mStep == 1) {
            SetWalkTargetOnFloor(mPartner->GetFloorPos());
            if (StepWalkLooking(0x3800, 0, 0x23D, 0x22, 5)) {
                mStep++;
            }
        }
        break;
    case 6:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                ResetSoundPhaseUnlessWalking();
                if (Character_TurnToWalkTarget(this, 0x100, 0)) {
                    mStep += 2;
                } else {
                    mStep++;
                    StartSound3IfAnimOfGroup();
                    SetAnimation(BCA0, GetWalkAnim());
                }
                Character_SetFace(this, 8);
                mHeadTurnFrames = 4;
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            mHeadPitch = GetHeadPitchToPartner(0, 0x1800);
            if (StepTurnLooking(0x100, 0, 0x3800)) {
                mStep++;
            }
        } else {
            if (mModel.mBlendFrames <= 0) {
                SetState(0);
                StartSound3IfAnimOfGroup();
                SetAnimation(BCA0, GetIdleAnim());
                SetHeadTurn(GetHeadPitchToPartner(0, 0x1800), 0, 0, 3);
            } else {
                func_02091fbc(this, 1, 0, 0x1A);
            }
        }
        Character_MoveAlongAngle(this);
        break;
    case 7:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(BCA0, 0x55);
                Character_SetFace(this, 0x15);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
                SetHeadIcon(13);
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 3, 1);
        }
        break;
    case 8:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(BCA0, 0x4A);
                Character_SetFace(this, 2);
                HeadIcon_SetMode(&mHeadIcon, 1);
                if (mModel.mBlendFrames > 0) {
                    Character_ResetSoundPhase(this, 0);
                }
            }
        } else {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1A, 4, 2);
        }
        break;
    }
    PlaceHeadIcon();
}

// State 9: stand for half a second, play animation 0x3A once, then stand for 140 frames.
// @0x0214ca04
void TutorialRoomCharacter::UpdateState09() {
    switch (mState) {
    case 9:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = 30;
                StartSound3IfAnimOfGroup();
                SetAnimation(BCA0, 7);
                Character_SetFace(this, 8);
                HeadIcon_SetMode(&mHeadIcon, 1);
            }
        } else if (mStep == 1) {
            mTimer--;
            if (mTimer <= 0 && mModel.mBlendFrames <= 0) {
                mStep++;
                SetAnimation(BCA0, 0x3A);
                func_0208ef00(&mModel, 1, 0);
                Character_ResetSoundPhase(this, 0);
            }
        } else if (mStep == 2) {
            Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 2, 2);
            if (AnimModel_FindAnimation(&mModel, BCA0, mModel.mAnimId)->mFinished) {
                mStep++;
                mTimer = 140;
                SetAnimation(BCA0, GetIdleAnim());
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0);
            }
        }
        break;
    }
    PlaceHeadIcon();
}

// States 10 to 15: coming in by the door (overlay 74's states 0x10C to 0x111).
// @0x0214cbb8
void TutorialRoomCharacter::UpdateStates0ATo0F() {
    switch (mState) {
    case 10:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mSoundArmed = 1;
                SetAnimation(BCA0, GetWalkAnim());
                AnimModel_FindAnimation(&mModel, BCA0, mModel.mAnimId)->mAnmObj->frame = 0;
                Character_SetFace(this, 8);
                VecFx32 pos = { 0, 0, -0x9000 };
                SetPos(pos);
                Character_SetAngle(this, 0x4000);
                mHeadTurnActive = 0;
                mUnknown25D6 = 0;
                Character_ResetSoundPhase(this, 0);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            mStep++;
            func_020a4144(&mApartmentRoom->mParts, 1);
            FloorPos target = sInsideDoor;
            SetWalkTargetOnFloor(target);
        } else if (mStep == 2) {
            if (StepWalkLooking(0x8000, 0, 0x23D, 0x28, 6)) {
                mStep++;
            }
        } else if (mStep == 3) {
            mStep++;
            func_020a4144(&mApartmentRoom->mParts, 2);
            func_02091fbc(this, 1, 0, 0x1A);
            FloorPos target = sRoomFront;
            SetWalkTargetOnFloor(target);
        } else {
            if (StepWalkLooking(0x2000, 0, 0x23D, 0x25, 6)) {
                SetState(0);
            }
        }
        break;
    case 11:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mSoundArmed = 1;
                SetAnimation(BCA0, GetIdleAnim());
                Character_SetFace(this, 8);
                VecFx32 pos = { 0, 0, -0x9000 };
                SetPos(pos);
                Character_SetAngle(this, 0x4000);
                mUnknown25D6 = 0;
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 5);
                Character_ResetSoundPhase(this, 0);
            }
        }
        break;
    case 12:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mSoundArmed = 1;
                SetAnimation(BCA0, GetWalkAnim());
                Character_SetFace(this, 8);
                VecFx32 pos = { 0, 0, -0x9000 };
                SetPos(pos);
                Character_SetAngle(this, 0x4000);
                mUnknown25D6 = 0;
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 5);
                Character_ResetSoundPhase(this, 0);
            }
        } else if (mStep == 1) {
            mStep++;
            func_020a4144(&mApartmentRoom->mParts, 1);
            FloorPos target = sDoorstep;
            SetWalkTargetOnFloor(target);
        } else if (mStep == 2) {
            if (StepWalkLooking(0x8000, 0, 0x23D, 0x33, 6)) {
                mStep++;
                FloorPos target = sPastDoor;
                SetWalkTargetOnFloor(target);
                mHeadTurnFrames = 4;
            }
        } else if (mStep == 3) {
            if (StepWalkLooking(0x8000, 0, 0x23D, 0x33, 6)) {
                mStep++;
                SetWalkTargetOnFloor(mPartner->GetFloorPos());
                func_020a4144(&mApartmentRoom->mParts, 2);
                mHeadTurnFrames = 3;
            }
            SetHeadTurn(0, GetHeadYawTo(mPartner->GetFloorPos(), 0x3800), 0, mHeadTurnFrames);
        } else if (mStep == 4) {
            if (StepTurnLooking(0x120, 0, 0x3800)) {
                if (mModel.mBlendFrames <= 0) {
                    mStep++;
                    SetAnimation(BCA0, GetIdleAnim());
                    SetHeadTurn(0, 0, 0, mHeadTurnFrames);
                }
            }
        } else if (mStep == 5) {
            if (mModel.mBlendFrames <= 0 && mPartner->mModel.mBlendFrames <= 0) {
                mStep++;
                if (!GetRelationToPartner()) {
                    SetAnimation(BCA0, 0x3A);
                } else {
                    SetAnimation(BCA0, 0x39);
                }
                func_0208ef28(&mModel, 1, 0, 0);
                mPartner->SetState(13);
                Character_ResetSoundPhase(this, 0);
            }
        } else {
            if (mModel.mAnimId == 0x3A) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 2, 2);
            } else if (mModel.mAnimId == 0x39) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 4, 2);
            }
            if (AnimModel_FindAnimation(&mModel, BCA0, mModel.mAnimId)->mFinished) {
                if (mPartner->mModel.mBlendFrames <= 0) {
                    SetState(15);
                    mPartner->SetState(15);
                }
            }
        }
        break;
    case 13:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                if (!GetRelationToPartner()) {
                    SetAnimation(BCA0, 0x3A);
                } else {
                    SetAnimation(BCA0, 0x39);
                }
                func_0208ef28(&mModel, 1, 0, 0);
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 5);
                Character_ResetSoundPhase(this, 0);
            }
        } else {
            if (mModel.mAnimId == 0x3A) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 2, 2);
            } else if (mModel.mAnimId == 0x39) {
                Character_PlaySoundAtAnimPhase(this, 0, 1, 0x1F, 4, 2);
            }
            if (AnimModel_FindAnimation(&mModel, BCA0, mModel.mAnimId)->mFinished) {
                SetState(0);
            }
        }
        break;
    case 14:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(BCA0, GetWalkAnim());
                ShowHeadIcon();
                SetHeadTurn(0, 0, 0, 3);
            }
        } else if (mStep == 1) {
            SetWalkTargetOnFloor(mPartner->GetFloorPos());
            if (StepTurnLooking(0x140, 0, 0x3800)) {
                if (mModel.mBlendFrames <= 0) {
                    mStep++;
                    SetAnimation(BCA0, GetIdleAnim());
                }
                SetHeadTurn(0, 0, 0, mHeadTurnFrames);
            }
        } else {
            SetWalkTargetOnFloor(mPartner->GetFloorPos());
            Character_TurnToWalkTarget(this, 0x60, 0);
            Character_MoveAlongAngle(this);
        }
        break;
    case 15:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(BCA0, GetIdleAnim());
                Character_SetFace(this, 8);
                SetHeadTurn(0, 0, 0, 5);
                ShowHeadIcon();
            }
        } else {
            if (mModel.mBlendFrames <= 0) {
                SetState(0);
            }
        }
        break;
    }
    PlaceHeadIcon();
}

// States 0x10 to 0x12: looking at the camera for a second and a half (0x10), turning to it (0x11) and coming half
// the way up to it (0x12). Overlay 74's states 0x113 to 0x115.
// @0x0214d6b8
void TutorialRoomCharacter::UpdateStates10To12() {
    switch (mState) {
    case 0x10:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                mTimer = 90;
                StartSound3IfAnimOfGroup();
                SetAnimation(BCA0, 0);
                Character_SetFace(this, 8);
                ShowHeadIcon();
            }
        } else if (mStep == 1) {
            mTimer--;
            if (mTimer <= 0) {
                mStep++;
                mTimer = 1;
            }
        } else {
            mTimer--;
            if (mTimer <= 0) {
                SetState(0);
            }
        }
        TurnHeadToCamera(3);
        break;
    case 0x11:
        if (mStep == 0) {
            if (mModel.mBlendFrames <= 0) {
                mStep++;
                StartSound3IfAnimOfGroup();
                SetAnimation(BCA0, 10);
                mTargetAngle = mAngle - mUnknown25D6;
                Character_ResetSoundPhase(this, 0);
                ShowHeadIcon();
            }
            TurnHeadToCamera(2);
        } else {
            TurnHeadToCamera(2);
            func_02091fbc(this, 1, 0, 0x1A);
            if (Character_TurnToTargetAngle(this, 0xA0, 0x10)) {
                SetState(0);
            }
            Character_MoveAlongAngle(this);
        }
        break;
    case 0x12:
        if (mStep == 0) {
            mStep++;
            if (mUnknown2AF0 < 0x1000) {
                mTimer = 40;
            } else {
                mTimer = 30;
            }
            VecFx32 offset = mCamera->mPosition;
            VEC_Subtract(&offset, &mModel.mPos, &offset);
            offset.x >>= 1;
            offset.y >>= 1;
            offset.z >>= 1;
            SetWalkTarget(offset.x, 0, offset.z);
        } else {
            mTimer--;
            if (mTimer <= 0 && mModel.mBlendFrames <= 0) {
                SetState(0);
                StartSound3IfAnimOfGroup();
                SetAnimation(BCA0, 7);
            }
            SetHeadTurn(mHeadPitch, GetHeadYawToCamera(), 0, mHeadTurnFrames);
            StepWalkLooking(0x8000, 0, 0x28F, 0x1F, 6);
        }
        break;
    }
    PlaceHeadIcon();
}

// @0x0214dabc
void TutorialRoomCharacter::Draw() {
    if (mPresent) {
        func_02091968(this);
    }
}

// @0x0214dad8
void TutorialRoomCharacter::CopyName() {
    func_0207a80c(this, mNameText);
}

// --- TutorialRoomCharacter: head turns, walking steps, the head icon, animations and sounds ---

// @0x0214daec
u16 TutorialRoomCharacter::GetHeadYaw(u16 angle, int limit) {
    u16 yaw = angle + mAngle;
    func_02090c78(this, &yaw, limit);
    return yaw;
}

// @0x0214db10
void TutorialRoomCharacter::SetHeadTurn(u16 pitch, u16 yaw, u16 roll, u8 frames) {
    mHeadTurnFrames = frames;
    mHeadPitch = pitch;
    mHeadYaw = yaw;
    mHeadRoll = roll;
    mHeadTurnActive = TRUE;
}

// @0x0214db3c
u16 TutorialRoomCharacter::GetHeadYawTo(FloorPos pos, int limit) {
    FloorPos offset = pos - GetFloorPos();
    return GetHeadYaw(-FX_Atan2Idx(offset.z, offset.x), limit);
}

// @0x0214dbc0
u16 TutorialRoomCharacter::GetHeadPitch(VecFx32 from, VecFx32 to, u16 limit) {
    VecFx32 offset;
    VEC_Subtract(&to, &from, &offset);
    fx32 z = MATH_ABS(offset.z);
    fx32 x = MATH_ABS(offset.x);
    u16 pitch = -FX_Atan2Idx(offset.y, FX_Mul((x + z) >> 1, 0x2CCC));
    func_02090c78(this, &pitch, limit);
    return pitch;
}

// @0x0214dc58
u16 TutorialRoomCharacter::GetHeadPitchToPartner(int joint, u16 limit) {
    return GetHeadPitch(CharacterModel_GetJointPosition(&mModel, 0),
                        CharacterModel_GetJointPosition(&mPartner->mModel, joint), limit);
}

// @0x0214dcc8
BOOL TutorialRoomCharacter::StepWalkLooking(u32 lookLimit, int lookAngle, fx32 reach, int speed, int turnShift) {
    BOOL arrived = FALSE;
    func_02091fbc(this, 1, 0, 0x1A);
    if (func_02091194(this, func_02091174(this, reach), speed, turnShift)) {
        arrived = TRUE;
    }
    Character_MoveAlongAngle(this);
    if (lookLimit < 0x4000) {
        SetHeadTurn(mHeadPitch, GetHeadYaw(lookAngle - mWalkAngle, lookLimit), mHeadRoll, mHeadTurnFrames);
    }
    return arrived;
}

// @0x0214dd7c
BOOL TutorialRoomCharacter::StepTurnLooking(int speed, int accel, u32 lookLimit) {
    BOOL facing = FALSE;
    func_02091fbc(this, 1, 0, 0x1A);
    if (Character_TurnToWalkTarget(this, speed, accel)) {
        facing = TRUE;
    }
    if (accel != 0) {
        Character_MoveAlongAngle(this);
    }
    if (lookLimit < 0x4000) {
        SetHeadTurn(mHeadPitch, GetHeadYaw(-mWalkAngle, lookLimit), mHeadRoll, mHeadTurnFrames);
    }
    return facing;
}

// @0x0214de28
void TutorialRoomCharacter::SetAnimation(u32 resType, int id) {
    func_020919e8(this, resType, id);
    func_0208ef00(&mModel, 0, 0);
}

// @0x0214de4c
void TutorialRoomCharacter::GetRandomPosInArea(FloorPos *out) {
    fx32 size = gWalkArea.w;
    out->x = MATH_Rand16(&gRandom, size) - (size >> 1);
    out->z = MATH_Rand16(&gRandom, size) - (size >> 1);
}

// @0x0214ded0
void TutorialRoomCharacter::LoadHeadIcon(ParticleSystem *particles) {
    mHeadIcon.Load(particles, sMoodHeadIcons[mMood]);
}

// @0x0214def8
void TutorialRoomCharacter::PlaceHeadIcon() {
    mHeadIcon.mAngle = 0x4000 - mAngle + mUnknown25D6;
    MtxFx43 *head = mModel.GetJointMatrix(0);
    VEC_Set(&mHeadIcon.mPos, head->_30, head->_31, head->_32);
}

// @0x0214df3c
void TutorialRoomCharacter::ShowHeadIcon() {
    if (mMood == 0) {
        HeadIcon_SetMode(&mHeadIcon, 1);
        return;
    }
    mHeadIcon.mAngle = 0x4000 - mAngle + mUnknown25D6;
    s16 iconId = sMoodHeadIcons[mMood];
    if (mHeadIcon.mIconId != iconId) {
        mHeadIcon.Show();
        mHeadIcon.mIconId = iconId;
        mHeadIcon.Reload();
        HeadIcon_SetMode(&mHeadIcon, 3);
    } else {
        mHeadIcon.Restart();
        HeadIcon_SetMode(&mHeadIcon, 3);
    }
}

// @0x0214dfec
BOOL TutorialRoomCharacter::IsWalkAnim() {
    int anim = mModel.mAnimId;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 5; j++) {
            if (anim == data_020fb898[i][j]) {
                return TRUE;
            }
        }
    }
    return FALSE;
}

// @0x0214e044
BOOL TutorialRoomCharacter::IsAnimOfGroupB(int anim) {
    switch (anim) {
    case 0x1E:
    case 0x1F:
    case 0x27:
    case 0x3D:
    case 0x44:
    case 0x45:
    case 0x46:
    case 0x5B:
    case 0x5C:
        return TRUE;
    }
    return FALSE;
}

// @0x0214e0bc
BOOL TutorialRoomCharacter::IsAnimOfGroupA(int anim) {
    switch (anim) {
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x24:
    case 0x33:
    case 0x3F:
    case 0x40:
    case 0x53:
        return TRUE;
    }
    return FALSE;
}

// @0x0214e148
void TutorialRoomCharacter::StartSound3IfAnimOfGroup() {
    if (mSoundArmed) {
        if (IsAnimOfGroupA(mModel.mAnimId) || IsAnimOfGroupB(mModel.mAnimId)) {
            gSoundManager.StartSeqArc(1, 0x1E, 3);
        }
    } else {
        mSoundArmed = TRUE;
    }
}

// @0x0214e1b0
void TutorialRoomCharacter::StartSound0UnlessGroupA() {
    if (mSoundArmed) {
        if (!IsAnimOfGroupA(mModel.mAnimId)) {
            gSoundManager.StartSeqArc(1, 0x1E, 0);
        }
    } else {
        mSoundArmed = TRUE;
    }
}

// --- TutorialApartmentRoom ---

// @0x0214e1fc
TutorialApartmentRoom::TutorialApartmentRoom() {
    SetState(STATE_IDLE);
    func_020a4ab8(&mParts, 3);
    MI_CpuClear16(mNumberText, sizeof(mNumberText));
}

// @0x0214e248
void TutorialApartmentRoom::Refresh() {
    UpdateNumberText();
}

// @0x0214e254
void TutorialApartmentRoom::SetStyle(u16 style) {
    mNextStyle = style;
    mStyle = style;
}

// @0x0214e264
void TutorialApartmentRoom::UpdateNumberText() {
    int room = (mRoomIndex & 7) + 1;
    int floor = (mRoomIndex >> 3) + 1;
    func_0203673c(floor * 100 + room, mNumberText, sizeof(mNumberText), 4, FALSE);
}

// @0x0214e2a8
void TutorialApartmentRoom::Update() {
    UpdateState();
    if (mState == STATE_IDLE) {
        func_020a5960(this);
    }
}

// @0x0214e2cc
void TutorialApartmentRoom::SetState(int state) {
    mState = state;
    mStep = 0;
}

// @0x0214e2e0
void TutorialApartmentRoom::UpdateState() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_RESET:
        SetState(STATE_IDLE);
        func_020a4ab8(&mParts, 0);
        func_020a4144(&mParts, 0);
        func_020a4b74(&mParts, 0);
        func_020a5948(this);
        UpdateNumberText();
        break;
    case STATE_CHANGE_STYLE:
        if (mStep == 0) {
            mStep++;
            mStyle = mNextStyle;
            func_020a54e4(this, 0);
        } else if (mStep == 1) {
            if (ArchiveManager_IsIdle(&gArchiveManager)) {
                mStep++;
                func_020a56dc(this, 0);
            }
        } else if (mStep == 2) {
            if (ArchiveManager_IsIdle(&gArchiveManager)) {
                SetState(STATE_IDLE);
            }
        }
        break;
    case STATE_UNKNOWN3:
        if (mStep == 0) {
            mStep++;
            func_020a5530(this, 3);
        } else if (mStep == 1) {
            SetState(STATE_IDLE);
            func_020a5854(this, 4);
        }
        break;
    }
}

// --- TutorialRoomCamera ---

// @0x0214e41c
TutorialRoomCamera::TutorialRoomCamera() {
    mCamera = &data_02122764;
}

// @0x0214e42c
TutorialRoomCamera::~TutorialRoomCamera() {
    VEC_Set(&data_0213ba74.mPosition, mCamera->mPosition.x, mCamera->mPosition.y, mCamera->mPosition.z);
    VEC_Set(&data_0213ba74.mTarget, mCamera->mTarget.x, mCamera->mTarget.y, mCamera->mTarget.z);
    data_0213ba74.SetOrbit(mOrbit.x >> FX32_SHIFT, mOrbit.y >> FX32_SHIFT, mOrbit.z >> FX32_SHIFT);
}

// @0x0214e48c
void TutorialRoomCamera::Reset() {
    SetMode(MODE_NONE);
    mIgnoreInput = FALSE;
    mSnap = TRUE;
    func_0202a640(mCamera);
    mPosition.x = 0;
    mPosition.y = FX32_CONST(5);
    mPosition.z = FX32_CONST(16);
    mTarget.x = 0;
    mTarget.y = FX32_CONST(1.5);
    mTarget.z = 0;
    mOrbit.x = 0x1200000;
    mOrbit.y = 0;
    mOrbit.z = 0;
    VecFx32 zero = { 0, 0, 0 };
    mInput = zero;
    mOrbitSpeed = mInput;
    ApplyToCamera();
    mCamera->SetAngles(0, 0, 0);
}

// @0x0214e57c
void TutorialRoomCamera::SetMode(int mode) {
    mMode = mode;
    mUnknown58 = mFollowFrames = 0;
}

// Goes to the wanted position and target: at once after a snap, otherwise a part of the way each frame.
// Written out in each of the three places: as an inline function its temporaries come in another order on the stack.
#define MOVE_TO(position, target)                                  \
    if (mSnap) {                                                   \
        mSnap = FALSE;                                             \
        mFollowFrames = 10000;                                     \
    } else {                                                       \
        target = Approach(target, mTarget, 5);                     \
        if (mFollowFrames < 80) {                                  \
            mFollowFrames++;                                       \
            position = Approach(position, mPosition, 5);           \
        } else {                                                   \
            position = Approach(position, mPosition, 4);           \
        }                                                          \
    }                                                              \
    mPosition = position;                                          \
    mTarget = target;                                              \
    ApplyToCamera();

// In MODE_CHARACTER the target is computed from the radius just copied (radius.x >> 1), not from the character's
// position again: only that gives the original's registers.
// @0x0214e590
void TutorialRoomCamera::Update() {
    switch (mMode) {
    case MODE_NONE:
        break;
    case MODE_ROOM: {
        UpdateOrbit();
        VecFx32 target;
        target.x = 0;
        target.y = 0x1E66;
        target.z = 0;
        VecFx32 radius;
        radius.x = FX32_CONST(19);
        radius.y = FX32_CONST(9);
        radius.z = FX32_CONST(19);
        VecFx32 position = GetOrbitPosition(radius, Shift(target, 3));
        MOVE_TO(position, target);
        break;
    }
    case MODE_CHARACTER: {
        UpdateOrbit();
        VecFx32 center;
        center = GetPresentCharacter()->GetPos();
        VecFx32 target;
        VecFx32 radius;
        radius.x = center.x;
        radius.y = center.y;
        radius.z = center.z;
        target.x = radius.x >> 1;
        target.z = radius.z >> 1;
        target.y = 0x1999;
        Clamp(&target.x, 0x3800);
        Clamp(&target.z, 0x3800);
        func_02028e0c(&radius, &radius);
        radius.x = (radius.x * 9 >> 2) + 0xA800;
        radius.z = (radius.z * 9 >> 2) + 0xA800;
        radius.y = 0x8000;
        if (radius.x > 0x12000) {
            radius.x = 0x12000;
        }
        if (radius.z > 0x12000) {
            radius.z = 0x12000;
        }
        VecFx32 position = GetOrbitPosition(radius, Shift(target, 4));
        MOVE_TO(position, target);
        break;
    }
    case MODE_BOTH: {
        UpdateOrbit();
        VecFx32 target;
        VEC_Add(&mCharacters[0]->mModel.mPos, &mCharacters[1]->mModel.mPos, &target);
        target.x >>= 1;
        target.z >>= 1;
        target.y = 0x1C00;
        Clamp(&target.x, 0x3800);
        Clamp(&target.z, 0x3800);
        VecFx32 radius;
        VEC_Subtract(&mCharacters[0]->mModel.mPos, &mCharacters[1]->mModel.mPos, &radius);
        func_02028e0c(&radius, &radius);
        radius.x = (radius.x * 9 >> 2) + 0x9000;
        radius.z = (radius.z * 9 >> 2) + 0x9000;
        radius.y = 0x8000;
        if (radius.x > 0x12000) {
            radius.x = 0x12000;
        }
        if (radius.z > 0x12000) {
            radius.z = 0x12000;
        }
        VecFx32 position = GetOrbitPosition(radius, Shift(target, 4));
        MOVE_TO(position, target);
        break;
    }
    }
}

// @0x0214ed7c
void TutorialRoomCamera::UpdateOrbit() {
    if ((mInput.x > 0 && mOrbit.x > 0x3000000) || (mInput.x < 0 && mOrbit.x < 0xA00000)) {
        mInput.x = 0;
    }
    if (mIgnoreInput) {
        mInput.x = 0;
        mInput.y = 0;
        mInput.z = 0;
    }
    ApplyInput(&mOrbitSpeed.x, mInput.x, sOrbitFriction.x, sOrbitMaxSpeed.x);
    ApplyInput(&mOrbitSpeed.y, mInput.y, sOrbitFriction.y, sOrbitMaxSpeed.y);
    VEC_Add(&mOrbit, &mOrbitSpeed, &mOrbit);
    mInput.x = 0;
    mInput.y = 0;
    mInput.z = 0;
}

// @0x0214ee30
void TutorialRoomCamera::ApplyInput(fx32 *speed, fx32 input, fx32 friction, fx32 max) {
    *speed += input;
    if (*speed > 0) {
        *speed -= friction;
    } else if (*speed < 0) {
        *speed += friction;
    }
    if (*speed > max) {
        *speed = max;
    } else if (*speed < -max) {
        *speed = -max;
    } else if (MATH_ABS(*speed) <= (friction + 1) >> 1) {
        *speed = 0;
    }
}

// @0x0214ee94
VecFx32 TutorialRoomCamera::GetOrbitPosition(VecFx32 radius, VecFx32 center) {
    u16 elevation = mOrbit.x >> FX32_SHIFT;
    u16 direction = mOrbit.y >> FX32_SHIFT;
    VecFx32 position;
    position.x = center.x + FX_Mul(radius.z, FX_SinIdx(direction));
    position.z = center.z + FX_Mul(radius.x, FX_CosIdx(direction));
    position.y = FX_Mul(radius.y, FX_SinIdx(elevation));
    return position;
}

// @0x0214ef6c
VecFx32 TutorialRoomCamera::Approach(VecFx32 to, VecFx32 from, int shift) {
    VecFx32 step;
    VEC_Subtract(&to, &from, &step);
    step = Shift(step, shift);
    VEC_Add(&step, &from, &step);
    return step;
}

// @0x0214f008
void TutorialRoomCamera::Clamp(fx32 *value, fx32 limit) {
    if (*value > limit) {
        *value = limit;
    } else if (*value < -limit) {
        *value = -limit;
    }
}

// --- TutorialRoomLight ---

// @0x0214f028
TutorialRoomLight::TutorialRoomLight() {
    SetState(STATE_IDLE);
}

// @0x0214f040
void TutorialRoomLight::Init(TutorialRoomCamera *camera, TutorialApartmentRoom *room) {
    mCamera = camera;
    mRoom = room;
    for (int i = 0; i < 2; i++) {
        mLights[i] = &data_021227cc[i];
        func_0202a96c(mLights[i]);
        mLights[i]->mColor = data_020f72f8[mRoom->mStyle].mLightColor;
    }
}

// @0x0214f0a0
void TutorialRoomLight::Update() {
    switch (mState) {
    case STATE_SET_STYLE_COLOR:
        SetColor(data_020f72f8[mRoom->mStyle].mLightColor);
        SetState(STATE_IDLE);
        break;
    case STATE_FADE_STYLE_COLOR:
        if (mStep == 0) {
            mStep++;
            mFade = 0;
        } else {
            int targetR, targetG, targetB;
            int r, g, b;
            u16 color;
            mFade += 0x80;
            func_02028b98(data_020f72f8[mRoom->mStyle].mLightColor, &targetR, &targetG, &targetB);
            BlendColor(&targetR, &targetG, &targetB, &r, &g, &b);
            if (r >= targetR || g >= targetG || b >= targetB) {
                color = data_020f72f8[mRoom->mStyle].mLightColor;
                SetState(STATE_IDLE);
            } else {
                color = r | (g << 5) | (b << 10);
            }
            SetColor(color);
        }
        break;
    case STATE_SET_ROOM_COLOR:
        SetColor(func_020a4d90(mRoom));
        SetState(STATE_IDLE);
        break;
    case STATE_FADE_ROOM_COLOR:
        if (mStep == 0) {
            mStep++;
            mFade = 0;
        } else {
            int targetR, targetG, targetB;
            int r, g, b;
            u16 color;
            mFade += 0x40;
            func_020a4dcc(mRoom, &targetR, &targetG, &targetB);
            BlendColor(&targetR, &targetG, &targetB, &r, &g, &b);
            if (r <= targetR || g <= targetG || b <= targetB) {
                color = targetR | (targetG << 5) | (targetB << 10);
                SetState(STATE_IDLE);
            } else {
                color = r | (g << 5) | (b << 10);
            }
            SetColor(color);
        }
        break;
    }
    func_0202ab2c(mLights[0], mCamera->mOrbit.x >> 12, mCamera->mOrbit.y >> 12, mCamera->mOrbit.z >> 12, 1600,
                  -600, -4000);
    func_0202ab2c(mLights[1], mCamera->mOrbit.x >> 12, mCamera->mOrbit.y >> 12, mCamera->mOrbit.z >> 12, 0,
                  -4000, 1500);
}

// @0x0214f38c
void TutorialRoomLight::SetState(int state) {
    mState = state;
    mStep = 0;
}

// @0x0214f39c
void TutorialRoomLight::SetColor(u16 color) {
    for (int i = 0; i < 2; i++) {
        mLights[i]->mColor = color;
    }
}

// @0x0214f3b8
void TutorialRoomLight::BlendColor(int *targetR, int *targetG, int *targetB, int *r, int *g, int *b) {
    func_02028b98(mLights[0]->mColor, r, g, b);
    fx32 weight = FX_Mul(mFade, mFade);
    *r = *r + ((weight * (*targetR - *r)) >> FX32_SHIFT);
    *g = *g + ((weight * (*targetG - *g)) >> FX32_SHIFT);
    *b = *b + ((weight * (*targetB - *b)) >> FX32_SHIFT);
}

// --- TutorialRoomCursor ---

// @0x0214f454
TutorialRoomCursor::TutorialRoomCursor() {
    SetState(STATE_IDLE);
}

// @0x0214f46c
void TutorialRoomCursor::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_MOVE:
        SetState(STATE_IDLE);
        mPosition.x = mTarget.x;
        mPosition.y = mTarget.y;
        break;
    }
}

// @0x0214f4a4
void TutorialRoomCursor::SetState(int state) {
    mState = state;
}

// @0x0214f4ac
BOOL TutorialRoomCursor::Contains(ScreenPoint point) {
    ScreenRect rect = sCursorTouchRect;
    ScreenPoint position = GetPixelPosition();
    rect.x += position.x;
    rect.y += position.y;
    return func_02028d88(&rect, &point) != 0;
}

// --- TutorialBuildingButton ---

// @0x0214f530
TutorialBuildingButton::TutorialBuildingButton() {
    SetState(STATE_IDLE);
}

// @0x0214f548
void TutorialBuildingButton::Init() {
    Sprite_Init(this, 0, 9, 219, 0, TRUE);
}

// @0x0214f574
void TutorialBuildingButton::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_PRESS:
        SetState(STATE_IDLE);
        Sprite_SetSequence(this, 10, TRUE);
        mAnim.animCtrl.bActive = FALSE;
        break;
    case STATE_RELEASE:
        SetState(STATE_IDLE);
        Sprite_SetSequence(this, 9, TRUE);
        mAnim.animCtrl.bActive = FALSE;
        break;
    case STATE_PLAY:
        SetState(STATE_IDLE);
        Sprite_SetPlaying(this, TRUE);
        break;
    }
}

// @0x0214f5fc
void TutorialBuildingButton::SetState(int state) {
    mState = state;
}

// --- TutorialApartmentFront ---

// @0x0214f604
TutorialApartmentFront::TutorialApartmentFront() {
    SetState(STATE_IDLE);
    mScrollTarget = 0;
    mUnknownClearedByConstructor = 0;
    mScroll = 0;
    mSelectedRoom = 0;
    mBuildingSize = 0;
    STD_CopyString(mArchivePath, "Scn/AFwBG/Apart0_LZ.bin");
    MI_CpuFill8(mResidents, 0xFF, sizeof(mResidents));
}

// @0x0214f668
void TutorialApartmentFront::SetBuildingSize(int size) {
    mBuildingSize = size;
    mArchivePath[15] = size + '0';
}

// @0x0214f678
void TutorialApartmentFront::Init(int room) {
    for (u8 i = 0; i < sRoomCounts[mBuildingSize]; i++) {
        DrawWindow(i);
    }
    mScreen = (u16 *)(func_020253d4(gGraphicsManager, 0x200) + HW_DB_BG_VRAM);
    mSelectedRoom = room;
    int column;
    int floor;
    GetRoomCell(room, &column, &floor);
    mScrollTarget = floor * sScrollPerFloor[mBuildingSize];
    ClampScrollTarget();
    mScroll = mScrollTarget;
    UpdateCursorTarget();
    ScreenPoint target = mCursor.GetPixelTarget();
    Sprite_Init(&mCursor, 0, 8, target.x, target.y, TRUE);
    mButton.Init();
    UpdateButtonPosition();
}

// @0x0214f78c
void TutorialApartmentFront::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_SCROLL: {
        fx32 target = mScrollTarget;
        fx32 step = (target - mScroll) >> 4;
        if (MATH_ABS(step) >= 0xF5) {
            mScroll += step;
        } else {
            SetState(STATE_IDLE);
            mScroll = target;
        }
        UpdateCursorTarget();
        mCursor.SetState(TutorialRoomCursor::STATE_MOVE);
        break;
    }
    }
    int y = 320 - (mScroll >> FX32_SHIFT);
    func_02028154(&data_02121834, 0x200, y);
    func_02028154(&data_02121834, 0x400, y);
    mCursor.Update();
    mButton.Update();
    UpdateButtonPosition();
}

// @0x0214f848
void TutorialApartmentFront::Draw() {
    Sprite_Draw(&mCursor);
    Sprite_Draw(&mButton);
}

// @0x0214f860
void TutorialApartmentFront::SetState(int state) {
    mState = state;
}

// @0x0214f868
void TutorialApartmentFront::GetRoomCell(int room, int *column, int *floor) {
    *column = room & 7;
    *floor = room >> 3;
    if (*floor == 12) {
        if (*column <= 3) {
            *column += 2;
        } else if (*column <= 5) {
            *column -= 4;
        }
    }
}

// @0x0214f8a4
void TutorialApartmentFront::ClampScrollTarget() {
    fx32 max = sScrollPerFloor[mBuildingSize] * (mBuildingSize * 3 + 2);
    if (mScrollTarget < 0) {
        mScrollTarget = 0;
    } else if (mScrollTarget > max) {
        mScrollTarget = max;
    }
}

// Not matching yet: same instructions, another order (11 of 25 differ): the original loads mScroll before the
// column, and stores each of the two locals as soon as it is computed. Tried: plain statements, the scroll in a
// local, compound assignments, the arithmetic in an inline function taking pointers (CellToPosition, kept), by
// references, split into two inline functions, the cell as a ScreenPoint or an array; also (overlay 73) local
// pointers to x and y, a free inline helper with the scroll first or last and by value or as fx32, the scroll
// read into a local first. With local pointers and mScroll read inside the y expression the column is stored at
// once as in the original (12 of 25 differ), but mScroll is then loaded after it. Byte-identical to overlay 74's.
// @0x0214f8e0
void TutorialApartmentFront::UpdateCursorTarget() {
    int x;
    int y;
    GetRoomCell(mSelectedRoom, &x, &y);
    CellToPosition(&x, &y, mScroll >> FX32_SHIFT);
    mCursor.SetTarget(x << FX32_SHIFT, y << FX32_SHIFT);
}

// @0x0214f944
void TutorialApartmentFront::UpdateButtonPosition() {
    mButton.mPosition.y = ((mScroll >> FX32_SHIFT) + 151) << FX32_SHIFT;
}

// @0x0214f95c
int TutorialApartmentFront::GetWindowKind(int room) {
    u8 character = mResidents[room];
    if (character == 0xFF) {
        return WINDOW_EMPTY;
    }
    int unknown1 = func_0202c468(&gGameData, character);
    u8 unknown2 = func_0202c53c(&gGameData, character);
    int unknown3 = func_0202c5b4(&gGameData, character);
    int unknown4 = data_02114d78[unknown3][func_0202c58c(&gGameData, character)].mUnknown0;
    if (func_02099c94(unknown1)) {
        return WINDOW_2;
    }
    if (unknown1 == 18) {
        return WINDOW_1;
    }
    if (unknown1 == 17) {
        return WINDOW_7;
    }
    if (unknown2 == 1 || unknown2 == 2) {
        if (unknown3 == 3 && (unknown4 == 5 || unknown4 == 6)) {
            return WINDOW_5;
        }
        if (unknown3 == 4 && unknown4 == 11) {
            return WINDOW_6;
        }
        return WINDOW_3;
    }
    return WINDOW_0;
}

// @0x0214fa44
void TutorialApartmentFront::DrawWindow(int room) {
    DrawWindowOfKind(GetWindowKind(room), room);
}

// @0x0214fa68
void TutorialApartmentFront::DrawWindowOfKind(int kind, int room) {
    int column;
    int floor;
    GetRoomCell(room, &column, &floor);
    DrawWindowTiles(kind, column, floor);
}

// @0x0214faac
void TutorialApartmentFront::DrawWindowTiles(int kind, u8 column, u8 floor) {
    u16 *screen = mScreen + (column * 4 + 0x660 - floor * 0x80);
    u16 tile;
    int i;
    int palette;
    switch (kind) {
    case WINDOW_0:
        palette = 0x0000;
        tile = 1;
        break;
    case WINDOW_1:
    case WINDOW_2:
        palette = 0x1000;
        tile = 5;
        break;
    case WINDOW_3:
        palette = 0x2000;
        tile = 9;
        break;
    case WINDOW_EMPTY:
        palette = 0x3000;
        tile = 13;
        break;
    case WINDOW_5:
        palette = 0x2000;
        tile = 17;
        break;
    case WINDOW_6:
        palette = 0x2000;
        tile = 21;
        break;
    case WINDOW_7:
        palette = 0x0000;
        tile = 25;
        break;
    default:
        return;
    }
    for (i = 0; i < 4; i++) {
        screen[0] = palette + ((screen[0] & 0xC00) + tile);
        screen[1] = palette + ((screen[1] & 0xC00) + (tile + 1));
        screen[2] = palette + ((screen[2] & 0xC00) + (tile + 2));
        screen[3] = palette + ((screen[3] & 0xC00) + (tile + 3));
        tile += 0x20;
        screen += 0x20;
    }
}

// --- TutorialSkyBackground ---

// @0x0214fbc8
TutorialSkyBackground::TutorialSkyBackground() {
    STD_CopyString(mArchivePath, "Scn/AFwBG/A0FwFBG0_LZ.bin");
    STD_CopyString(mCharPath, "ASX:/Fw_BG0.ncg");
    STD_CopyString(mPalettePath, "ASX:/Fw_BG0.ncl");
    SetState(STATE_WATCH);
    mPalette = NULL;
    mColors = NULL;
}

// @0x0214fc20
void TutorialSkyBackground::Init(TutorialClock *clock, int buildingSize) {
    mClock = clock;
    func_020a612c(this, &clock->mDate, &clock->mTime);
    mBuildingSize = buildingSize;
    mArchivePath[11] = buildingSize + '0';
    UpdatePaths();
}

// @0x0214fc58
void TutorialSkyBackground::Refresh() {
    ApplyPalette();
}

// @0x0214fc64
void TutorialSkyBackground::Update() {
    func_020a61cc(this, &mClock->mDate, &mClock->mTime);
    UpdateState();
}

// @0x0214fc88
void TutorialSkyBackground::SetState(int state) {
    mState = state;
    mStep = 0;
}

// @0x0214fc98
void TutorialSkyBackground::UpdateState() {
    switch (mState) {
    case STATE_WATCH:
        if (mChanged != 0) {
            SetState(STATE_RELOAD);
        }
        break;
    case STATE_RELOAD:
        if (mStep == 0) {
            if (ArchiveManager_IsIdle(&gArchiveManager)) {
                mStep++;
                UpdatePaths();
                ArchiveManager_Unmount(&gArchiveManager, "ASX");
                ArchiveManager_MountCompressed(&gArchiveManager, "ASX", mArchivePath, 0);
            }
        } else if (mStep == 1) {
            if (ArchiveManager_IsIdle(&gArchiveManager)) {
                SetState(STATE_WATCH);
                func_02028f24(&gVramTransferQueue, mCharPath, 0x400, 0, 0x1000, 0x3000, 0);
                mPalette = func_02027534(&data_021213b8, mPalettePath, 0x400, 0x80, 0x80, 0xC0);
                ApplyPalette();
            }
        }
        break;
    }
}

// @0x0214fdd8
void TutorialSkyBackground::UpdatePaths() {
    s8 digit = sSkyPictures[func_020a6214(this) & 0x1F] + '0';
    mArchivePath[17] = digit;
    mCharPath[10] = digit;
    mPalettePath[10] = digit;
}

// @0x0214fe10
void TutorialSkyBackground::ApplyPalette() {
    func_020278f8(&data_021213b8, mColors[func_020a6214(this)], 0x100);
}

// --- TutorialNamePanel ---

// @0x0214fe3c
TutorialNamePanel::TutorialNamePanel() {
    SetState(STATE_IDLE);
}

// The two character counts are read through a pointer to the canvas (GetCharCount above, as in the scene's Init),
// and the two positions are set by way of MakeVec (SetPositionFromVec): that orders the dead copies of the
// positions as the original has them.
// @0x0214fe6c
int TutorialNamePanel::Init(int charOffset, MessageFile *file1, MessageFile *file2) {
    Sprite_Init(this, 1, 4, 128, 96, FALSE);
    mText1.InitObjForMessage(file1, 1, &gFonts[0].mFont, 0x10, charOffset, 0, 1, 0, 0, 15, NULL);
    charOffset += GetCharCount(mText1);
    mText2.InitObjForMessage(file2, 2, &gFonts[0].mFont, 0x10, charOffset, 0, 1, 0, 0, 15, NULL);
    int count = GetCharCount(mText2);
    mText1.SetPositionFromVec(FX32_CONST(38), FX32_CONST(8));
    mText1.DrawAll();
    mText2.SetPositionFromVec(FX32_CONST(113), FX32_CONST(8));
    mText2.DrawAll();
    SetState(STATE_SHOWN);
    return charOffset + count;
}

// @0x0214fff8
void TutorialNamePanel::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_SHOW_BOTH:
    case STATE_SHOW_FIRST:
        if (mStep == 0) {
            mStep++;
            SetTextVisible(FALSE);
        } else if (mStep == 1) {
            mStep++;
            NNS_G2dCharCanvasClear(&mText1.mCanvas, 0);
            mText1.DrawAll();
        } else {
            NNS_G2dCharCanvasClear(&mText2.mCanvas, 0);
            SetVisible(TRUE);
            if (mState == STATE_SHOW_FIRST) {
                mText2.mSprite.mVisible = FALSE;
            } else {
                mText2.DrawAll();
            }
            SetState(STATE_SHOWN);
        }
        break;
    case STATE_SHOWN:
        if (mStep == 0) {
            mStep++;
            mFrames = 180;
        } else {
            mFrames--;
            if (mFrames <= 0) {
                SetState(STATE_IDLE);
                SetVisible(FALSE);
            }
        }
        break;
    case STATE_HIDE:
        SetVisible(FALSE);
        break;
    }
}

// @0x02150144
void TutorialNamePanel::Draw() {
    mText1.Draw();
    mText2.Draw();
    Sprite_Draw(this);
}

// @0x0215016c
void TutorialNamePanel::SetState(int state) {
    mState = state;
    mStep = 0;
}

// @0x02150180
void TutorialNamePanel::SetNames(const wchar_t *name1, const wchar_t *name2) {
    mText1.SetStringArg(name1, 0);
    mText2.SetStringArg(name2, 0);
}

// @0x021501b0
void TutorialNamePanel::SetTextVisible(BOOL visible) {
    mText1.mSprite.mVisible = visible;
    mText2.mSprite.mVisible = visible;
}

// @0x021501bc
void TutorialNamePanel::SetVisible(BOOL visible) {
    SetTextVisible(visible);
    mVisible = visible;
}

// --- TutorialTouchSprite, TutorialTouchButton ---

// @0x021501d4
TutorialTouchSprite::TutorialTouchSprite() {
    mEnabled = TRUE;
}

// @0x021501e0
BOOL TutorialTouchSprite::Contains(ScreenPoint point) {
    if (!mEnabled) {
        return FALSE;
    }
    ScreenRect rect = *mTouchRect;
    rect.x += mX;
    rect.y += mY;
    return func_02028d88(&rect, &point) != 0;
}

// @0x02150264
TutorialTouchButton::TutorialTouchButton() {
    mLabel = NULL;
}

// @0x02150280
void TutorialTouchButton::Init(int kind) {
    mKind = kind;
    mX = sButtonPositions[kind].x;
    mY = sButtonPositions[kind].y;
    Sprite_Init(this, 0, sButtonSequences[kind].mNormal, mX, mY, FALSE);
    mState = STATE_IDLE;
    if (kind == 0) {
        mTouchRect = &sButtonRect0;
        MessageText *label = mLabel;
        label->SetPositionFromVec((mX - label->mCanvas.areaWidth * 4) << FX32_SHIFT,
                                   (mY - label->mCanvas.areaHeight * 4) << FX32_SHIFT);
    } else {
        mTouchRect = &sButtonRectOther;
    }
}

// @0x02150354
void TutorialTouchButton::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_PRESS:
        mState = STATE_IDLE;
        Sprite_SetSequence(this, sButtonSequences[mKind].mPressed, TRUE);
        mAnim.animCtrl.bActive = FALSE;
        if (mLabel != NULL) {
            mLabel->mSprite.mPosition.y = (mY - mLabel->mCanvas.areaHeight * 4 + 2) << FX32_SHIFT;
        }
        break;
    case STATE_RELEASE:
        mState = STATE_IDLE;
        Sprite_SetSequence(this, sButtonSequences[mKind].mNormal, TRUE);
        mAnim.animCtrl.bActive = FALSE;
        if (mLabel != NULL) {
            mLabel->mSprite.mPosition.y = (mY - mLabel->mCanvas.areaHeight * 4) << FX32_SHIFT;
        }
        break;
    case STATE_PLAY:
        mState = STATE_IDLE;
        Sprite_SetPlaying(this, TRUE);
        break;
    }
}

// @0x0215044c
void TutorialTouchButton::Draw() {
    Sprite_Draw(this);
}

// @0x02150458
BOOL TutorialTouchButton::Contains(ScreenPoint point) {
    if (!mVisible) {
        return FALSE;
    }
    return TutorialTouchSprite::Contains(point);
}

// --- TutorialClock ---

// @0x02150490
TutorialClock::TutorialClock() {
    MI_CpuClear32(&mDate, sizeof(mDate));
    MI_CpuClear32(&mTime, sizeof(mTime));
    mSeconds = 0;
}

// @0x021504cc
void TutorialClock::Update() {
    mSeconds = func_02035870(data_02123440[0], &mDate, &mTime);
}

// @0x021504f4
void TutorialClock::UpdateIfArchiveIdle() {
    if (ArchiveManager_IsIdle(&gArchiveManager)) {
        mSeconds = func_02035870(data_02123440[0], &mDate, &mTime);
    }
}

// --- TutorialClockDisplay ---

// @0x02150530
void TutorialClockDisplay::Init(TutorialClock *clock) {
    mClock = clock;
    SetState(STATE_HIDE);
    for (int i = 0; i < SPRITE_COUNT; i++) {
        Sprite *sprite = &mSprites[i];
        Sprite_Init(sprite, 1, sClockSequences[i], 128, 96, TRUE);
        mSprites[i].SetPositionFromVec(sClockPositions[i].x << FX32_SHIFT, sClockPositions[i].y << FX32_SHIFT);
        sprite->mAnim.animCtrl.bActive = FALSE;
    }
    Refresh(TRUE);
}

// @0x021505e8
void TutorialClockDisplay::Update() {
    switch (mState) {
    case STATE_SHOWN:
        break;
    case STATE_HIDE:
        SetState(STATE_HIDDEN);
        SetVisible(FALSE);
        break;
    case STATE_HIDDEN:
        mHiddenFrames++;
        if (mHiddenFrames > 190) {
            SetState(STATE_SHOWN);
            SetVisible(TRUE);
        }
        break;
    }
    Refresh(FALSE);
}

// Not matching yet: 2 instructions are swapped in the hour's part ("add r0, r0, #0x400" for &mSprites[7] comes
// after the subtraction in the original, before it here). Everything else is identical. Tried: hour % 10, the
// ones in a local, a pointer to the sprite, mSprites + 7, 10 * tens, an int hour, hour % 12, 12 * half, an inline
// helper for the two digits (that gives add #0x410 + #0xAC). Byte-identical to overlay 74's.
// @0x02150668
void TutorialClockDisplay::Refresh(BOOL all) {
    TutorialClock *clock = mClock;
    if (mMonth != clock->mDate.month || all) {
        u32 tens = clock->mDate.month / 10;
        Sprite_SetFrame(&mSprites[SPRITE_MONTH_TENS], tens);
        Sprite_SetFrame(&mSprites[SPRITE_MONTH_ONES], clock->mDate.month - tens * 10);
        mMonth = clock->mDate.month;
    }
    if (mDay != clock->mDate.day || all) {
        u32 tens = clock->mDate.day / 10;
        Sprite_SetFrame(&mSprites[SPRITE_DAY_TENS], tens);
        Sprite_SetFrame(&mSprites[SPRITE_DAY_ONES], clock->mDate.day - tens * 10);
        mDay = clock->mDate.day;
    }
    if (mHour != clock->mTime.hour || all) {
        u32 half = clock->mTime.hour / 12;
        Sprite_SetFrame(&mSprites[SPRITE_HALF_OF_DAY], half);
        u32 hour = clock->mTime.hour - half * 12;
        u32 tens = hour / 10;
        Sprite_SetFrame(&mSprites[SPRITE_HOUR_TENS], tens);
        Sprite_SetFrame(&mSprites[SPRITE_HOUR_ONES], hour - tens * 10);
        mHour = clock->mTime.hour;
    }
    if (mMinute != clock->mTime.minute || all) {
        u32 tens = clock->mTime.minute / 10;
        Sprite_SetFrame(&mSprites[SPRITE_MINUTE_TENS], tens);
        Sprite_SetFrame(&mSprites[SPRITE_MINUTE_ONES], clock->mTime.minute - tens * 10);
        mMinute = clock->mTime.minute;
    }
}

// @0x021507e0
void TutorialClockDisplay::Draw() {
    for (int i = 0; i < SPRITE_COUNT; i++) {
        Sprite_Draw(&mSprites[i]);
    }
}

// @0x02150808
void TutorialClockDisplay::SetState(int state) {
    mState = state;
    mHiddenFrames = 0;
}

// @0x0215081c
void TutorialClockDisplay::SetVisible(BOOL visible) {
    for (int i = 0; i < SPRITE_COUNT; i++) {
        mSprites[i].mVisible = visible;
    }
}

// --- TutorialTimedMessage ---

// @0x02150838
TutorialTimedMessage::TutorialTimedMessage() {
    SetState(STATE_IDLE);
    mFrames = 0;
}

// @0x02150890
void TutorialTimedMessage::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_SHOW:
        NNS_G2dCharCanvasClear(&mCanvas, 0);
        mDrawMode = 1;
        mStarted = FALSE;
        SetVisible(TRUE);
        mFrames = 180;
        SetState(STATE_WAIT);
        break;
    case STATE_SHOW_BY_LINE:
        if (mStep == 0) {
            mStep++;
            NNS_G2dCharCanvasClear(&mCanvas, 0);
            mDrawMode = 3;
            mStarted = FALSE;
        } else if (mDrawMode == 0) {
            SetVisible(TRUE);
            mFrames = 180;
            SetState(STATE_WAIT);
        }
        break;
    case STATE_WAIT:
        if (mFrames > 0) {
            mFrames--;
        } else {
            SetState(STATE_HIDE);
        }
        break;
    case STATE_HIDE:
        SetState(STATE_IDLE);
        SetVisible(FALSE);
        break;
    }
    UpdateDraw();
}

// @0x021509c8
void TutorialTimedMessage::SetState(int state) {
    mState = state;
    mStep = 0;
}

// @0x021509dc
void TutorialTimedMessage::SetVisible(BOOL visible) {
    mSprite.mVisible = visible;
    if (visible == TRUE) {
        data_0211d604.Show(2);
    } else {
        data_0211d604.Hide(2);
    }
}

// --- TutorialRoomScene's destructor ---

// It has nothing to do of its own: the members are destroyed (the three message files, the camera, the room, the
// two characters). It comes last, after the classes the scene is made of.
// @0x02150a1c (D1), @0x02150adc (D0)
TutorialRoomScene::~TutorialRoomScene() {
}

// @0x02150d4c: __sinit_TutorialRoomScene.cpp, the static initialiser: gWalkArea's size.

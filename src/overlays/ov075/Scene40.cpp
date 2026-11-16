#include "Scene40.h"
#include "../../scene/MainProgram3d.h"
#include "GameData.h"
#include "save/SaveBlocks.h"
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
void func_ov075_0214fc00(void *object);                       // Draws the object at 0x30B50 of the scene
void func_ov075_02161988(Scene40Character *character);
void func_ov075_02161d94(Scene40Character *character);
void func_ov075_02178364(Scene40CharacterPartA *part);
}

// What this scene keeps in the main program's gSceneScratch (an array of bytes there) for the scenes it goes to
struct Scene40Scratch {
    /* 0x00 */ int mUnknown00;
    /* 0x04 */ BOOL mMenuAvailable[5]; // The menu's first five buttons, as Scene40MenuBar::Refresh takes them back
    /* 0x18 */ u8 mUnknown18[0x58];
}; // 0x70

extern Scene40Scratch gSceneScratch;
extern SaveHeader gIslandState; // The header of the save data

// Overlay 75: scenes 0x40 and 0x6C, the scene of touching a Mii ("Scn/Scene/SceneMiiTouch_LZ.bin"). One source
// file by its one static initialiser. Most of it is not decompiled yet.

// --- The scene -----------------------------------------------------------------------------------------------------

// For the first nine frames the room and the character are updated from here as well
void Scene40::Draw() {
    MessageWindow_Draw(&mWindows[0]);
    MessageWindow_Draw(&mWindows[1]);
    func_ov075_0214fc00(&mPages);
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
    date.year = mUnknown30A8C.mDate.year;
    date.month = mUnknown30A8C.mDate.month;
    date.day = mUnknown30A8C.mDate.day;
    date.week = mUnknown30A8C.mDate.week;
    mCharacter.SetMeetDate(&date);
    GameData_WriteCharacter(&gGameData, mCharacter.mIndex, &mCharacter);
    if ((int)gGameData.mUnknownB2[0] < (int)mCharacter.mSatisfyLevel) {
        gGameData.mUnknownB2[0] = mCharacter.mSatisfyLevel;
    }
    mClothesList.Save();
    mInventory.Save();
    mInteriors.Save();
    gGameData.SetCappedCounter(mUnknown309B4);

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

// --- The list page's tabs ------------------------------------------------------------------------------------------

static const u16 sListTabSequences[Scene40ListTabs::TAB_COUNT] = {41, 47, 43, 45};
static const u16 sListTabPressedSequences[Scene40ListTabs::TAB_COUNT] = {42, 48, 44, 46};
static const ScreenPoint sListTabPositions[Scene40ListTabs::TAB_COUNT] = {{23, 13}, {158, 13}, {68, 13}, {113, 13}};

void Scene40ListTabs::Init(int resource) {
    mPressed = 0;
    mSelected = 0;
    for (int i = 0; i < TAB_COUNT; i++) {
        Sprite_Init(&mTabs[i], resource, sListTabSequences[i], 128, 96, TRUE);
        SpriteButton_SetSequences(&mTabs[i], sListTabSequences[i], sListTabPressedSequences[i]);
        mTabs[i].SetPositionFromVec(sListTabPositions[i].x << FX32_SHIFT, sListTabPositions[i].y << FX32_SHIFT);
        SpriteButton_SetTouchRect(&mTabs[i], 44, 22);
        SpriteButton_Reset(&mTabs[i]);
        SpriteButton_SetKind(&mTabs[i], 3);
        mTabs[i].mOverwritePalette = 1;
        mTabs[i].mUnknownDC = TRUE;
        mTabs[i].mSounds.mSounds[0] = 3;
    }
    SetVisible(FALSE);
}

// A list's tab is there when the player owns some of it or the island has the shop that sells it
void Scene40ListTabs::UpdateAvailable(Scene40Context *context) {
    u8 built = gIslandState.GetUnknown44(3);
    BOOL owned = context->mInventory->HasAnyOwnedKind1();
    mAvailable[0] = owned | (built != 0);
    u8 level = gGameData.mUnknownB2[0];
    owned = context->mInventory->HasAnyOwnedKind3();
    mAvailable[1] = owned | (level >= 5);
    built = gIslandState.GetUnknown44(10);
    owned = context->mClothesList->HasAnyOwned();
    mAvailable[2] = owned | (built != 0);
    built = gIslandState.GetUnknown44(11);
    owned = context->mInteriors->HasAnyOwned();
    mAvailable[3] = owned | (built != 0);
}

void Scene40ListTabs::Update() {
    mPressed = 0;
    if (SpriteButton_Update(&mTabs[0])) {
        mPressed = 1 << 0;
        SpriteButton_SetSelected(&mTabs[mSelected], FALSE);
        mSelected = 0;
    } else if (SpriteButton_Update(&mTabs[1])) {
        mPressed = 1 << 1;
        SpriteButton_SetSelected(&mTabs[mSelected], FALSE);
        mSelected = 1;
    } else if (SpriteButton_Update(&mTabs[2])) {
        mPressed = 1 << 2;
        SpriteButton_SetSelected(&mTabs[mSelected], FALSE);
        mSelected = 2;
    } else if (SpriteButton_Update(&mTabs[3])) {
        mPressed = 1 << 3;
        SpriteButton_SetSelected(&mTabs[mSelected], FALSE);
        mSelected = 3;
    }
}

void Scene40ListTabs::Draw() {
    for (int i = 0; i < TAB_COUNT; i++) {
        SpriteButton_Draw(&mTabs[i]);
    }
}

void Scene40ListTabs::SetVisible(BOOL visible) {
    for (int i = 0; i < TAB_COUNT; i++) {
        mTabs[i].mVisible = visible & mAvailable[i];
    }
}

void Scene40ListTabs::Select(int tab) {
    for (int i = 0; i < TAB_COUNT; i++) {
        SpriteButton_SetSelected(&mTabs[i], FALSE);
    }
    mSelected = tab;
    SpriteButton_SetSelected(&mTabs[tab], TRUE);
}

static const u16 sGoodsTabSequences[Scene40GoodsTabs::TAB_COUNT] = {37, 39, 20, 22};
static const u16 sGoodsTabPressedSequences[Scene40GoodsTabs::TAB_COUNT] = {38, 40, 21, 23};
static const ScreenPoint sGoodsTabPositions[Scene40GoodsTabs::TAB_COUNT] = {{32, 41}, {88, 41}, {144, 41}, {200, 41}};
static const u32 sGoodsTabFilters[Scene40GoodsTabs::TAB_COUNT] = {1, 2, 8, 16};

void Scene40GoodsTabs::Init(int resource) {
    for (int i = 0; i < TAB_COUNT; i++) {
        Sprite_Init(&mTabs[i], resource, sGoodsTabSequences[i], 128, 96, TRUE);
        SpriteButton_SetSequences(&mTabs[i], sGoodsTabSequences[i], sGoodsTabPressedSequences[i]);
        mTabs[i].SetPositionFromVec(sGoodsTabPositions[i].x << FX32_SHIFT, sGoodsTabPositions[i].y << FX32_SHIFT);
        SpriteButton_SetTouchRect(&mTabs[i], 52, 24);
        SpriteButton_Reset(&mTabs[i]);
        SpriteButton_SetKind(&mTabs[i], 3);
        mTabs[i].mOverwritePalette = 1;
        mTabs[i].mUnknownDC = TRUE;
        mTabs[i].mSounds.mSounds[0] = 3;
    }
    SetVisible(FALSE);
    Select(0);
}

void Scene40GoodsTabs::Update() {
    mPressed = -1;
    for (int i = 0; i < TAB_COUNT; i++) {
        if (SpriteButton_Update(&mTabs[i])) {
            SpriteButton_SetSelected(&mTabs[mSelected], FALSE);
            mSelected = i;
            mPressed = i;
        }
    }
}

void Scene40GoodsTabs::Draw() {
    for (int i = 0; i < TAB_COUNT; i++) {
        SpriteButton_Draw(&mTabs[i]);
    }
}

void Scene40GoodsTabs::SetVisible(BOOL visible) {
    for (int i = 0; i < TAB_COUNT; i++) {
        mTabs[i].mVisible = visible;
    }
}

void Scene40GoodsTabs::Select(int tab) {
    for (int i = 0; i < TAB_COUNT; i++) {
        SpriteButton_SetSelected(&mTabs[i], FALSE);
    }
    SpriteButton_SetSelected(&mTabs[tab], TRUE);
    mSelected = tab;
}

u32 Scene40GoodsTabs::GetFilter() {
    return sGoodsTabFilters[mSelected];
}

int Scene40GoodsTabs::FindTab(u32 filter) {
    int tab = 0;
    switch (filter & 0x1F) {
    case 1:
        tab = 0;
        break;
    case 2:
        tab = 1;
        break;
    case 8:
        tab = 2;
        break;
    case 16:
        tab = 3;
        break;
    }
    return tab;
}

static const u16 sClothesTabSequences[Scene40ClothesTabs::TAB_COUNT] = {24, 26, 28, 30, 32};
static const u16 sClothesTabPressedSequences[Scene40ClothesTabs::TAB_COUNT] = {25, 27, 29, 31, 33};
static const ScreenPoint sClothesTabPositions[Scene40ClothesTabs::TAB_COUNT] = {
    {26, 41}, {72, 41}, {118, 41}, {164, 41}, {210, 41},
};
static const int sClothesTabKinds[Scene40ClothesTabs::TAB_COUNT] = {0, 1, 2, 3, 4};

void Scene40ClothesTabs::Init(int resource) {
    for (int i = 0; i < TAB_COUNT; i++) {
        Sprite_Init(&mTabs[i], resource, sClothesTabSequences[i], 128, 96, TRUE);
        SpriteButton_SetSequences(&mTabs[i], sClothesTabSequences[i], sClothesTabPressedSequences[i]);
        mTabs[i].SetPositionFromVec(sClothesTabPositions[i].x << FX32_SHIFT, sClothesTabPositions[i].y << FX32_SHIFT);
        SpriteButton_SetTouchRect(&mTabs[i], 42, 24);
        SpriteButton_Reset(&mTabs[i]);
        SpriteButton_SetKind(&mTabs[i], 3);
        mTabs[i].mOverwritePalette = 1;
        mTabs[i].mUnknownDC = TRUE;
        mTabs[i].mSounds.mSounds[0] = 3;
    }
    SetVisible(FALSE);
    Select(0);
}

void Scene40ClothesTabs::Update() {
    mPressed = -1;
    for (int i = 0; i < TAB_COUNT; i++) {
        if (SpriteButton_Update(&mTabs[i])) {
            SpriteButton_SetSelected(&mTabs[mSelected], FALSE);
            mSelected = i;
            mPressed = i;
        }
    }
}

void Scene40ClothesTabs::Draw() {
    for (int i = 0; i < TAB_COUNT; i++) {
        SpriteButton_Draw(&mTabs[i]);
    }
}

void Scene40ClothesTabs::SetVisible(BOOL visible) {
    for (int i = 0; i < TAB_COUNT; i++) {
        mTabs[i].mVisible = visible;
    }
}

void Scene40ClothesTabs::Select(int tab) {
    for (int i = 0; i < TAB_COUNT; i++) {
        SpriteButton_SetSelected(&mTabs[i], FALSE);
    }
    SpriteButton_SetSelected(&mTabs[tab], TRUE);
    mSelected = tab;
}

int Scene40ClothesTabs::GetKind() {
    return sClothesTabKinds[mSelected];
}

int Scene40ClothesTabs::FindTab(int kind) {
    int tab = 0;
    switch (kind) {
    case 0:
        tab = 0;
        break;
    case 1:
        tab = 1;
        break;
    case 2:
        tab = 2;
        break;
    case 3:
        tab = 3;
        break;
    case 4:
        tab = 4;
        break;
    }
    return tab;
}

// --- The scene's speech window (scene+0x163B0) ---------------------------------------------------------------------

void Scene40SpeechWindow::Update() {
    SpeechWindow::Update();
    if (mEnabled && gScreenControl.mUnknown44 == 0) {
        return;
    }
    Hide();
}

void Scene40SpeechWindow::Show(u32 message, int mode, int frames) {
    if (!mEnabled) {
        return;
    }
    if (gScreenControl.mUnknown44 != 0) {
        return;
    }
    SpeechWindow::Show(message, mode, frames);
}

// --- The scene's menu (scene+0x6947C) ------------------------------------------------------------------------------

// Each button's sequence, pressed sequence, place and touch rectangle in pixels and sound; the first button's
// values are written out in Init as well
static const u16 sMenuSequences[Scene40MenuBar::BUTTON_COUNT] = {4, 7, 2, 11, 9, 28, 0};
static const u16 sMenuPressedSequences[Scene40MenuBar::BUTTON_COUNT] = {6, 8, 3, 12, 10, 29, 1};
static const ScreenPoint sMenuPositions[Scene40MenuBar::BUTTON_COUNT] = {
    {18, 13}, {18, 109}, {18, 37}, {18, 85}, {18, 61}, {207, 13}, {242, 13},
};
static const ScreenPoint sMenuTouchSizes[Scene40MenuBar::BUTTON_COUNT] = {
    {32, 22}, {32, 22}, {32, 22}, {32, 22}, {32, 22}, {32, 22}, {24, 22},
};
static const int sMenuSounds[Scene40MenuBar::BUTTON_COUNT] = {10, 10, 10, 10, 10, 10, 12};

void Scene40CallButton::Reset() {
    SpriteButton_Reset(this);
    mCallState = CALL_NONE;
    mCallFrames = 0;
}

void Scene40CallButton::SetSequences(u16 sequence, u16 pressedSequence, u16 callSequence) {
    SpriteButton_SetSequences(this, sequence, pressedSequence);
    mCallSequence = callSequence;
}

void Scene40CallButton::StartCall() {
    if (!mVisible) {
        return;
    }
    mCallState = CALL_WAITING;
    mCallFrames = 0;
}

// Whether it was pressed; a touch stops the call
BOOL Scene40CallButton::Update() {
    BOOL pressed = SpriteButton_Update(this);
    BOOL selected = func_02031d00(this);
    if (pressed | selected) {
        mCallState = CALL_NONE;
    }
    return pressed;
}

void Scene40CallButton::UpdateCall() {
    switch (mCallState) {
    case CALL_NONE:
        break;
    case CALL_WAITING:
        if (++mCallFrames < 120) {
            break;
        }
        if (!mTtsPlayer->IsEngineIdle()) {
            break;
        }
        Sprite_SetSequence(this, mCallSequence, TRUE);
        gSoundManager.StartSeqArc(1, 10, 14);
        mCallState = CALL_CALLING;
        mCallFrames = 0;
        break;
    case CALL_CALLING:
        if (++mCallFrames < 160) {
            if (mCallFrames % 20 == 0 && (mCallFrames / 20) % 2 == 0) {
                gSoundManager.StartSeqArc(1, 10, 15);
            }
        } else if (mCallFrames >= 160) {
            Sprite_SetSequence(this, mSequence, TRUE);
            mCallState = CALL_NONE;
            mCallFrames = 0;
        }
        break;
    }
}

void Scene40CallButton::SetDisabled(BOOL disabled) {
    SpriteButton_SetDisabled(this, disabled);
    if (disabled) {
        mCallState = CALL_NONE;
    }
}

void Scene40ButtonFlash::Update() {
    if (!mVisible) {
        return;
    }
    if (!mAnim.animCtrl.bActive) {
        mVisible = FALSE;
    }
}

void Scene40ButtonFlash::Play() {
    if (mVisible) {
        return;
    }
    mVisible = TRUE;
    Sprite_SetPlaying(this, TRUE);
}

void Scene40MenuBar::Init(Scene40Context *context) {
    mUnknown000 = 0;
    mUnknown004 = 0;
    mTouchedButton = -1;
    mPressedButton = -1;
    mContext = context;
    mCharacter = context->mCharacter;
    mEnabled = FALSE;

    Sprite_Init(&mCallButton, 0, 4, 128, 96, TRUE);
    mCallButton.SetSequences(4, 6, 5);
    mCallButton.mTtsPlayer = mContext->mTtsPlayer;
    mCallButton.SetPositionFromVec(18 << FX32_SHIFT, 13 << FX32_SHIFT);
    SpriteButton_SetTouchRect(&mCallButton, 32, 22);
    mCallButton.Reset();
    SpriteButton_SetKind(&mCallButton, 4);
    mCallButton.mOverwritePalette = 1;
    mCallButton.mUnknownDC = TRUE;
    mCallButton.mSounds.mSounds[0] = 10;
    mCallButton.mSounds.mSounds[2] = 0;
    mButtonList[0] = &mCallButton;

    for (int i = 1; i < BUTTON_COUNT; i++) {
        Sprite_Init(&mButtons[i], 0, sMenuSequences[i], 128, 96, TRUE);
        SpriteButton_SetSequences(&mButtons[i], sMenuSequences[i], sMenuPressedSequences[i]);
        mButtons[i].SetPositionFromVec(sMenuPositions[i].x << FX32_SHIFT, sMenuPositions[i].y << FX32_SHIFT);
        SpriteButton_SetTouchRect(&mButtons[i], sMenuTouchSizes[i].x, sMenuTouchSizes[i].y);
        SpriteButton_Reset(&mButtons[i]);
        SpriteButton_SetKind(&mButtons[i], 4);
        mButtons[i].mOverwritePalette = 1;
        mButtons[i].mUnknownDC = TRUE;
        mButtons[i].mSounds.mSounds[0] = sMenuSounds[i];
        mButtons[i].mSounds.mSounds[2] = 0;
        mButtonList[i] = &mButtons[i];
    }
    for (int i = 0; i < 5; i++) {
        mButtonList[i]->mVisible = FALSE;
    }

    Sprite_Init(&mFlash, 0, 31, 128, 96, TRUE);
    mFlash.mOverwritePalette = 1;
    mFlash.mVisible = FALSE;
}

void Scene40MenuBar::Update() {
    mTouchedButton = -1;
    mPressedButton = -1;
    mFlash.Update();
    mCallButton.UpdateCall();
    if (mEnabled && SpriteButton_IsTouched(&mCallButton)) {
        mTouchedButton = 0;
    }
    if (mEnabled && mCallButton.Update()) {
        mPressedButton = 0;
    }
    for (int i = 1; i < BUTTON_COUNT; i++) {
        if (mEnabled && SpriteButton_IsTouched(mButtonList[i])) {
            mTouchedButton = i;
        }
        if (mEnabled && SpriteButton_Update(mButtonList[i])) {
            mPressedButton = i;
        }
    }
}

void Scene40MenuBar::SetEnabled(BOOL enabled) {
    if (enabled != mEnabled) {
        mEnabled = enabled;
    }
}

// Shows the buttons whose offer is there; coming back from scene 0x38 or 0x40, the first five are as that scene
// left them
void Scene40MenuBar::Refresh() {
    UpdateAvailable();
    if (gSceneManager.mPreviousSceneId == 0x38) {
        for (int i = 0; i < 5; i++) {
            mAvailable[i] = gSceneScratch.mMenuAvailable[i];
        }
    } else if (gSceneManager.mPreviousSceneId == 0x40) {
        for (int i = 0; i < 5; i++) {
            mAvailable[i] = gSceneScratch.mMenuAvailable[i];
        }
    }
    for (int i = 0; i < BUTTON_COUNT; i++) {
        mButtonList[i]->mVisible = mAvailable[i] ? TRUE : FALSE;
    }
}

void Scene40MenuBar::UpdateAvailable() {
    mAvailable[0] = mContext->mSceneId != 0x6C;
    mAvailable[1] = MATH_CountPopulation(mCharacter->mUnknown299) != 0;
    mAvailable[2] = mCharacter->CountOwnedClothes() > 1;
    mAvailable[3] = wcslen(mCharacter->mMotto) != 0;
    mAvailable[4] = mCharacter->CountOwnedInteriors() > 1;
    mAvailable[5] = TRUE;
    mAvailable[6] = TRUE;
}

void Scene40MenuBar::Draw() {
    Sprite_Draw(&mFlash);
    for (int i = 0; i < BUTTON_COUNT; i++) {
        SpriteButton_Draw(mButtonList[i]);
    }
}

void Scene40MenuBar::StartCall() {
    mCallButton.StartCall();
}

// Shows the button chosen and plays the flash over it
void Scene40MenuBar::Choose(int button) {
    SetButtonVisible(button, TRUE);
    ScreenPoint position = mButtonList[button]->GetPixelPosition();
    mFlash.SetPositionFromVec(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
    mFlash.Play();
    gSoundManager.StartSeqArc(1, 10, 17);
}

void Scene40MenuBar::SetButtonVisible(int button, BOOL visible) {
    mButtonList[button]->mVisible = visible;
}

void Scene40MenuBar::SetVisible(BOOL visible) {
    for (int i = 0; i < BUTTON_COUNT; i++) {
        mButtonList[i]->mVisible = visible;
    }
}

void Scene40MenuBar::SetDisabled(BOOL disabled) {
    mCallButton.SetDisabled(disabled);
    for (int i = 1; i < BUTTON_COUNT; i++) {
        SpriteButton_SetDisabled(mButtonList[i], disabled);
    }
}

void Scene40MenuBar::SetLeftDisabled(BOOL disabled) {
    mCallButton.SetDisabled(disabled);
    for (int i = 1; i < 5; i++) {
        SpriteButton_SetDisabled(mButtonList[i], disabled);
    }
}

void Scene40MenuBar::SetRightDisabled(BOOL disabled) {
    for (int i = 5; i <= 6; i++) {
        SpriteButton_SetDisabled(mButtonList[i], disabled);
    }
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

// --- The scene's clock, at 0x30A8C --------------------------------------------------------------------------------

Scene40Unknown30A8C::Scene40Unknown30A8C() {
    MI_CpuClear32(&mDate, sizeof(mDate));
    MI_CpuClear32(&mTime, sizeof(mTime));
    mSeconds = 0;
}

void Scene40Unknown30A8C::Read() {
    mSeconds = Clock_GetDateTime(gGameClock[0], &mDate, &mTime);
}

void Scene40Unknown30A8C::ReadIfIdle() {
    if (!ArchiveManager_IsIdle(&gArchiveManager)) {
        return;
    }
    mSeconds = Clock_GetDateTime(gGameClock[0], &mDate, &mTime);
}

#include "MiiEditScene.h"
#include "sound/SoundManager.h"

#include <wchar.h>

// --- The main program (stand-ins, not decompiled) ---
struct ArchiveLoader;
struct ScreenControl;
struct ScreenLoader;
struct ParticleSystem;
extern "C" {
extern ArchiveLoader gArchiveManager;
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
extern u32 data_021223e4[];
void func_0202aecc(void *resources, int resourceId);
void func_0202ad84(void *resources);
void func_0202683c(void);
extern ScreenControl data_0211d604;
extern ScreenLoader data_02121834;
void func_02024234(ScreenControl *screens, int planes, int a);
void func_02023f4c(ScreenControl *screens, int planes, int a);
void func_02023eac(ScreenControl *screens, int planes);
void func_02028108(ScreenLoader *loader, int plane, int offset);
void func_0208ee18(CharacterModel *model, const char *archiveName, int a, int b);
void func_02091968(SceneCharacter *character); // Draws it
void func_0208fefc(CharacterBase *character);
void func_0208ff94(CharacterBase *character, void *a, void *b);
void func_02090214(CharacterBase *character);
void func_0207b258(ParticleSystem *system, void *a);
extern u8 data_02142438[];
}

void MiiEditScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "E00", "Scn/Scene/SceneMiiEdit2_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "KBD", "Cmn/Kbd/SoftKeyboard_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TBM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "T02", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "SPL", "Mdl/Spl/SplEditEffect_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "BIC", "Mdl/Ica/ica_mii_edit_LZ.bin", 0);
    func_0208ee18(&mCharacter.mModel, "B0I", 0, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "B0I", mCharacter.mModel.mArchivePath, 0);
}

// The camera's moves last this many frames
#define CAMERA_MOVE_FRAMES 40

void MiiEditScene::ResetCameraGoal() {
    mCameraGoal = mCamera->mPosition;
    mCameraFrame = 0;
}

void MiiEditScene::UpdateCamera() {
    if (mCameraFrame >= CAMERA_MOVE_FRAMES) {
        return;
    }
    fx32 t = (mCameraFrame << FX32_SHIFT) / CAMERA_MOVE_FRAMES;
    t = FX_Mul(t, t);
    t = FX_Mul(t, t);
    mCamera->mPosition.x += FX_Mul(mCameraGoal.x - mCamera->mPosition.x, t);
    mCamera->mPosition.y += FX_Mul(mCameraGoal.y - mCamera->mPosition.y, t);
    mCamera->mPosition.z += FX_Mul(mCameraGoal.z - mCamera->mPosition.z, t);
    mCamera->mTarget.x = mCamera->mPosition.x;
    mCamera->mTarget.y = mCamera->mPosition.y;
    mCamera->mTarget.z = mCamera->mPosition.z - FX32_CONST(8);
    mCameraFrame++;
}

void MiiEditScene::MoveCameraTo(VecFx32 goal) {
    mCameraFrame = 13;
    mCameraGoal.x = goal.x;
    mCameraGoal.y = goal.y;
    mCameraGoal.z = goal.z;
}

void MiiEditScene::Draw() {
    if (mUnknown58DC && mUnknown58E0 && !mUnknown58E4) {
        Sprite_Draw(&mSprite582C);
    }
    mText1B94C.ObjTextCanvas::Draw();
    func_02091968(&mCharacter);
    if (mCharacter.IsUnknown24F8Clear()) {
        if (mUnknown580C == TRUE) {
            *(MiiData *)mEditedCharacter.mMii = *(MiiData *)mCharacter.mMii;
            func_0208fefc(&mEditedCharacter);
            VecFx32 v = mCharacter.mUnknown2568;
            mEditedCharacter.mUnknown2568 = v;
            mEditedCharacter.mUnknown25D4 = mCharacter.mUnknown25D4;
            mEditedCharacter.mUnknown25D6 = mCharacter.mUnknown25D6;
            mEditedCharacter.mUnknown25D8 = mCharacter.mUnknown25D8;
            func_0208ff94(&mEditedCharacter, mCharacter.mUnknown2538, mCharacter.mUnknown2514);
            mUnknown580C = FALSE;
        }
    } else {
        func_0208ff94(&mEditedCharacter, mCharacter.mUnknown2538, mCharacter.mUnknown2514);
        func_02090214(&mEditedCharacter);
    }
    func_0202ad84(data_021223e4);
    NNS_G3dGlbFlushP();
    func_0207b258(mParticles, data_02142438);
    for (u16 i = 0; i < 6; i++) {
        func_02031d7c(&mButtons[i]);
    }
    switch (mPage) {
    case 0:
        mPage0.Draw();
        break;
    case 1:
        mFacePage.Draw();
        break;
    case 2:
        mProfilePage.Draw();
        break;
    case 3:
        mVoicePage.Draw();
        break;
    case 4:
        mCharacterPage.Draw();
        break;
    case 6:
        mPage6.Draw();
        break;
    case 5:
        mPage5.Draw();
        break;
    }
}

void MiiEditScene::Exit() {
    func_0202aecc(data_021223e4, 0);
    func_0202aecc(data_021223e4, 0x1F);
    ArchiveManager_Unmount(&gArchiveManager, "E00");
    ArchiveManager_Unmount(&gArchiveManager, "B0I");
    ArchiveManager_Unmount(&gArchiveManager, "BIC");
    ArchiveManager_Unmount(&gArchiveManager, "SPL");
    func_0202683c();
    ArchiveManager_Unmount(&gArchiveManager, "KBD");
    ArchiveManager_Unmount(&gArchiveManager, "TBM");
    ArchiveManager_Unmount(&gArchiveManager, "T02");
    func_02024234(&data_0211d604, 0x3F00, 0);
    func_02023f4c(&data_0211d604, 0x3F00, 0);
    func_02023eac(&data_0211d604, 0x3F00);
    func_02028108(&data_02121834, 4, 0);
    switch (mPage) {
    case 0:
        mPage0.Exit();
        break;
    case 1:
        mFacePage.Exit();
        break;
    case 2:
        mProfilePage.Exit();
        break;
    case 3:
        mVoicePage.Exit();
        break;
    case 4:
        mCharacterPage.Exit();
        break;
    case 6:
        mPage6.Exit();
        break;
    case 5:
        mPage5.Exit();
        break;
    }
}

// --- Page 0 ---

void MiiEditPage0::Draw() {
    for (int i = 0; i < 2; i++) {
        func_020345d8(&mWindows[i]);
    }
}

void MiiEditPage0::Exit() {
    for (int i = 0; i < 2; i++) {
        func_020346cc(&mWindows[i]);
    }
}

// Shows the scene's own buttons and texts again, and the lower screen's background (the pages call it when they
// close)
void MiiEditScene::func_ov076_0214bbe4() {
    for (u16 i = 0; i < 9; i++) {
        mTextBoxes[i].mSprite.mVisible = TRUE;
    }
    mUnknown58E4 = FALSE;
    for (int i = 0; i < 6; i++) {
        mButtons[i].mVisible = TRUE;
    }
    if (mSceneId == 0x6F) {
        mButtons[5].mVisible = FALSE;
    }
    func_02027d7c(&data_02121834, "E00:/BG_down.nsc", 0x200, 0, 0, 0, 0, -1);
    data_0211d604.mPlanes = 0x1217;
    data_0211d604.mPlanesChanged = TRUE;
}

// --- The face page ---

void MiiEditFacePage::Draw() {
    for (u16 i = 0; i < 12; i++) {
        func_02031d7c(&mButtons227C[i]);
    }
    for (int i = 0; i < 2; i++) {
        Sprite_Draw(&mSprites2DF4[i].mSprite);
        Sprite_Draw(&mSprites2FF4[i]);
    }
    Sprite_Draw(&mSprite315C);
    Sprite_Draw(&mSprite32B4);
    Sprite_Draw(&mSprite3208);
    Sprite_Draw(&mSprite3360);
    for (int i = 0; i < 36; i++) {
        func_02031d7c(&mButtons[i]);
    }
}

void MiiEditFacePage::Exit() {}

// --- The profile page ---

void MiiEditProfilePage::Draw() {
    for (u16 i = 0; i < 2; i++) {
        Sprite_Draw(&mSprites[i]);
    }
    for (u16 i = 0; i < 9; i++) {
        mScene->mTextBoxes[i].Draw();
    }
    mScene->mText1B35C.ObjTextCanvas::Draw();
    for (u16 i = 0; i < 10; i++) {
        func_02031d7c(&mButtons[i]);
    }
    for (int i = 0; i < 6; i++) {
        func_020345d8(&mWindows[i]);
    }
    switch (mState) {
    case 3:
    case 5:
    case 7:
    case 9:
        func_02083bcc(&mKeyboard);
        mScene->mText1B35C.ObjTextCanvas::Draw();
        mScene->mCharBox.Draw();
        func_02031d7c(&mButton);
        break;
    }
}

void MiiEditProfilePage::Exit() {
    for (int i = 0; i < 6; i++) {
        func_020346cc(&mWindows[i]);
    }
    switch (mState) {
    case 3:
    case 5:
    case 7:
    case 9:
        func_02027d7c(&data_02121834, "E00:/BG_down_editface.nsc", 0x200, 0, 0, 0, 0, -1);
        func_02027d7c(&data_02121834, "E00:/Window_down.nsc", 0x100, 0, 0, 0, 0, -1);
        func_02028f24(&gVramTransferQueue, "E00:/BG_down.ncg", 0x200, 0, 0, 0, 0);
        func_02027534(&data_021213b8, "E00:/BG_down.ncl", 0x200, -1, -1, -1);
        mScene->func_ov076_0214bbe4();
        break;
    }
}

// What the profile page uses of the main program (stand-ins)
extern "C" {
void func_02037478(wchar_t *text); // Applied to the texts of numbers before they are shown
void func_02031e88(SpriteButton *button, BOOL disabled); // Sets mUnknownB8, and draws the button darkened while it is set
int func_02098dac(SavedCharacter *character, int param);  // A parameter of the character (0x26, 0x27: birth month, day)
BOOL func_02098e50(SavedCharacter *character, int param);
void func_020986c4(SavedCharacter *character, int param, int *min, int *max); // The range of a parameter
BOOL func_02098584(SavedCharacter *character);
void MiiData_GetName(SavedCharacter *mii, wchar_t *name);
void MiiData_GetCreatorName(SavedCharacter *mii, wchar_t *name);
int MiiData_GetSex(SavedCharacter *mii);
}

// Where the profile page's texts go: the place of each of mTextBoxes[0..8] in sProfileTextPositions
static const int sProfileTextSlots[9] = { 0, 1, 4, 2, 3, 6, 7, 8, 9 };
static const WindowPoint sProfileTextPositions[10] = {
    { 70, 40 }, { 186, 40 }, { 128, 112 }, { 160, 76 }, { 68, 76 },
    { 202, 112 }, { 216, 76 }, { 68, 149 }, { 192, 149 }, { 54, 112 },
};
// By MiiData_GetSex
static const wchar_t *sSexMarks[2] = { L"♂", L"♀" };

// Puts a text in mText (up to 10 characters) and shows it in the scene's character box, 13 pixels a character
void MiiEditProfilePage::SetText(const wchar_t *text) {
    MI_CpuClear16(mText, sizeof(mText));
    mTextLength = 0;
    NNS_G2dCharCanvasClear(&mScene->mCharBox.mCanvas, 0);
    while (mTextLength < 10) {
        u16 c = text[mTextLength];
        if (c == 0) {
            return;
        }
        mText[mTextLength] = c;
        int x = mTextLength * 13 + 5;
        x -= NNS_G2dFontGetCharWidths(mScene->mCharBox.mFont, c)->charWidth >> 1;
        mScene->mCharBox.mCharX = x;
        mScene->mCharBox.SetChar(c);
        mScene->mCharBox.Redraw();
        mScene->mCharBox.Transfer();
        mTextLength++;
    }
}

// Shows a number of up to four digits with the sprites of the third window, without its leading zeros
void MiiEditProfilePage::SetNumberDigits(int value) {
    int ones = value % 10;
    int tens = value % 100 / 10;
    int hundreds = value % 1000 / 100;
    int thousands = value % 10000 / 1000;
    Sprite_SetFrame(&mWindows[2].mPlainSprites[0], ones);
    Sprite_SetFrame(&mWindows[2].mPlainSprites[1], tens);
    Sprite_SetFrame(&mWindows[2].mPlainSprites[2], hundreds);
    Sprite_SetFrame(&mWindows[2].mPlainSprites[3], thousands);
    BOOL visible[4];
    for (int i = 0; i < 4; i++) {
        visible[i] = TRUE;
    }
    if (value / 10000 <= 0 && thousands == 0) {
        visible[3] = FALSE;
        if (hundreds == 0) {
            visible[2] = FALSE;
            if (tens == 0) {
                visible[1] = FALSE;
            }
        }
    }
    for (int i = 0; i < 4; i++) {
        mWindows[2].mPlainSprites[i].mVisible = visible[i];
    }
}

// Shows the birth day in two digits with the first two sprites of the fourth window
void MiiEditProfilePage::SetDayDigits(int day) {
    int ones = day % 10;
    int tens = day % 100 / 10;
    Sprite_SetFrame(&mWindows[3].mPlainSprites[0], ones);
    Sprite_SetFrame(&mWindows[3].mPlainSprites[1], tens);
    BOOL visible[2];
    for (int i = 0; i < 2; i++) {
        visible[i] = TRUE;
    }
    if (day / 100 <= 0 && tens == 0) {
        visible[1] = FALSE;
        if (ones == 0) {
            visible[0] = FALSE;
        }
    }
    for (int i = 0; i < 2; i++) {
        mWindows[3].mPlainSprites[i].mVisible = visible[i];
    }
}

// Shows the birth month in two digits with the next two sprites of the fourth window
void MiiEditProfilePage::SetMonthDigits(int month) {
    int ones = month % 10;
    int tens = month % 100 / 10;
    Sprite_SetFrame(&mWindows[3].mPlainSprites[2], ones);
    Sprite_SetFrame(&mWindows[3].mPlainSprites[3], tens);
    BOOL visible[2];
    for (int i = 0; i < 2; i++) {
        visible[i] = TRUE;
    }
    if (month / 100 <= 0 && tens == 0) {
        visible[1] = FALSE;
        if (ones == 0) {
            visible[0] = FALSE;
        }
    }
    for (int i = 0; i < 2; i++) {
        mWindows[3].mPlainSprites[i + 2].mVisible = visible[i];
    }
}

// Puts a text box at a place in pixels
static inline void SetTextBoxPosition(ObjTextBox *box, WindowPoint point) {
    box->SetPosition(point.x << FX32_SHIFT, point.y << FX32_SHIFT);
}

// The scene's buffers for the texts that UpdateTexts writes
#define NUMBER_TEXT(scene) ((wchar_t *)&(scene)->mUnknown1B304[0x00])
#define BIRTHDAY_TEXT(scene) ((wchar_t *)&(scene)->mUnknown1B304[0x14])
#define NAME_TEXT(scene) ((wchar_t *)&(scene)->mUnknown1B330[0x00])
#define CREATOR_NAME_TEXT(scene) ((wchar_t *)&(scene)->mUnknown1B330[0x16])

// Writes the character's profile into the scene's nine text boxes, each centred on its place
void MiiEditProfilePage::UpdateTexts() {
    u32 number = mScene->mCharacter.mUnknown050Bits17;
    if (number == 0xFFF) {
        swprintf(NUMBER_TEXT(mScene), 5, L"", number);
    } else {
        swprintf(NUMBER_TEXT(mScene), 5, L"%d", number);
        func_02037478(NUMBER_TEXT(mScene));
    }
    int month = func_02098dac(&mScene->mCharacter, 0x26);
    int day = func_02098dac(&mScene->mCharacter, 0x27);
    if (month >= 1 && month <= 12 && day >= 1 && day <= 31) {
        swprintf(BIRTHDAY_TEXT(mScene), 6, L"%d/%d", month, day);
        func_02037478(BIRTHDAY_TEXT(mScene));
    } else {
        swprintf(BIRTHDAY_TEXT(mScene), 6, L"");
    }
    mScene->mTextBoxes[0].SetText((const NNSG2dChar *)&mScene->mCharacter.mUnknown058[0x16]);
    mScene->mTextBoxes[1].SetText((const NNSG2dChar *)mScene->mCharacter.mUnknown058);
    MiiData_GetName(&mScene->mCharacter, NAME_TEXT(mScene));
    mScene->mTextBoxes[2].SetText((const NNSG2dChar *)NAME_TEXT(mScene));
    mScene->mTextBoxes[8].SetText((const NNSG2dChar *)sSexMarks[MiiData_GetSex(&mScene->mCharacter)]);
    mScene->mTextBoxes[3].SetText(
        (const NNSG2dChar *)mScene->mMessageFiles[0].GetText(mScene->mCharacter.mUnknown050Bits29 + 0x5A));
    mScene->mTextBoxes[4].SetText((const NNSG2dChar *)NUMBER_TEXT(mScene));
    mScene->mTextBoxes[5].SetText((const NNSG2dChar *)BIRTHDAY_TEXT(mScene));
    MiiData_GetCreatorName(&mScene->mCharacter, CREATOR_NAME_TEXT(mScene));
    mScene->mTextBoxes[6].SetText((const NNSG2dChar *)CREATOR_NAME_TEXT(mScene));
    mScene->mTextBoxes[7].SetText(
        (const NNSG2dChar *)mScene->mMessageFiles[0].GetText(mScene->mCharacter.mUnknown298 + 0x54));
    if (!func_02098584(&mScene->mCharacter)) {
        mScene->mTextBoxes[2].mColor = 9;
        mScene->mTextBoxes[6].mColor = 9;
        BOOL monthSet = func_02098e50(&mScene->mCharacter, 0x26);
        BOOL daySet = func_02098e50(&mScene->mCharacter, 0x27);
        if (monthSet && daySet) {
            mScene->mTextBoxes[5].mColor = 9;
        }
    }
    for (u16 i = 0; i < 9; i++) {
        mScene->mUnknown1B32C = 1;
        int width = NNS_G2dTextCanvasGetTextWidth(&mScene->mTextBoxes[i].mTextCanvas, mScene->mTextBoxes[i].mText);
        const WindowPoint *position = &sProfileTextPositions[sProfileTextSlots[i]];
        WindowPoint point;
        point.x = position->x + -width / 2;
        point.y = position->y - 7;
        SetTextBoxPosition(&mScene->mTextBoxes[i], point);
    }
    Sprite_SetFrame(&mSprites[0], func_02098dac(&mScene->mCharacter, 0x28));
}

// Darkens the second button of the third window while the birth day is past the last day the month has
void MiiEditProfilePage::UpdateDayButton() {
    int min, max;
    func_020986c4(&mScene->mCharacter, 0x27, &min, &max);
    if (func_02098dac(&mScene->mCharacter, 0x27) > max) {
        func_02031e88((SpriteButton *)&mWindows[2].mButtons[1], TRUE);
    } else {
        func_02031e88((SpriteButton *)&mWindows[2].mButtons[1], FALSE);
    }
}

// Enables the keyboard's first key in the states that type a text, when func_ov076_0214bf20 accepts the text
// (state 9 also with no text at all)
void MiiEditProfilePage::UpdateKeyboardKey(int state) {
    BOOL enabled = FALSE;
    switch (state) {
    case 3:
    case 5:
    case 7:
        enabled = mScene->func_ov076_0214bf20(mText) ? TRUE : FALSE;
        break;
    case 9:
        if (wcslen(mText) == 0 || mScene->func_ov076_0214bf20(mText)) {
            enabled = TRUE;
        } else {
            enabled = FALSE;
        }
        break;
    }
    func_02083608(mKeyboard.mKeys[0], enabled);
}

// --- The voice page ---

void MiiEditVoicePage::Draw() {
    for (u16 i = 0; i < 22; i++) {
        func_02031d7c(&mButtons[i]);
    }
    for (u16 i = 0; i < 3; i++) {
        func_0203206c(&mTextButtons[i]);
    }
    for (int i = 0; i < 5; i++) {
        Sprite_Draw(&mSprites2B2C[i].mSprite);
    }
    for (int i = 0; i < 2; i++) {
        Sprite_Draw(&mSprites29D4[i]);
    }
}

void MiiEditVoicePage::Exit() {
    func_ov076_02157bc0();
    if (mState != 4) {
        return;
    }
    func_02027d7c(&data_02121834, "E00:/BG_down_editface.nsc", 0x200, 0, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, "E00:/Window_down.nsc", 0x100, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "E00:/BG_down.ncg", 0x200, 0, 0, 0, 0);
    func_02027534(&data_021213b8, "E00:/BG_down.ncl", 0x200, -1, -1, -1);
    mScene->func_ov076_0214bbe4();
}

void MiiEditVoicePage::func_ov076_02157bc0() {
    func_ov076_02157508(0);
}

// --- The personality page ---

// What this page uses of the main program (stand-ins)
struct TouchInput {
    /* 0x00 */ u8 mUnknown00[0x2C];
    /* 0x2C */ u16 mTouching;
    /* 0x2E */ u8 mUnknown2E[2];
    /* 0x30 */ ScreenPoint mLastPoint; // Where it was touched last
    /* 0x38 */ ScreenPoint mPoint;     // Where it is touched
    /* 0x40 */ u8 mUnknown40[8];
    /* 0x48 */ BOOL mTouched;          // The stylus went down this frame
    /* 0x4C */ BOOL mUnknown4C;        // Which of the two points to test
};

extern "C" {
extern TouchInput gTouchInput;
void ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
void func_02098d5c(SavedCharacter *character, int index, u8 value); // Sets mUnknown04A[index]
void func_02091a6c(SceneCharacter *character);
}

// The sliders: a knob for each of the five values, and its label
static const u16 sCharacterKnobSequences[5] = { 0, 0, 0, 0, 0 };
static const u16 sCharacterLabelSequences[5] = { 1, 2, 3, 4, 5 };
static const WindowPoint sCharacterKnobSizes[5] = { { 12, 12 }, { 12, 12 }, { 12, 12 }, { 12, 12 }, { 12, 12 } };
static const int sCharacterKnobKinds[5] = { 4, 4, 4, 4, 4 };
static const WindowPoint sCharacterKnobPositions[5] = { { 128, 26 }, { 128, 59 }, { 128, 92 }, { 128, 125 }, { 128, 158 } };
static const WindowPoint sCharacterLabelPositions[5] = { { 128, 26 }, { 128, 59 }, { 128, 92 }, { 128, 125 }, { 128, 158 } };
static const VecFx32 sCharacterCameraGoal = { 0, 0x1C50, 0x19BF0 };
// Where each of the eight values of a slider takes a touch, relative to the slider's row
static const ScreenRect sCharacterValueRects[8] = {
    { 73, -6, 12, 12 }, { 87, -6, 12, 12 }, { 101, -6, 12, 12 }, { 115, -6, 12, 12 },
    { 129, -6, 12, 12 }, { 143, -6, 12, 12 }, { 157, -6, 12, 12 }, { 171, -6, 12, 12 },
};

void MiiEditCharacterPage::func_ov076_02157c04(MiiEditScene *scene) {
    mScene = scene;
    scene->func_ov076_0214bbe4();
    ObjResourceManager_Load(&gObjResourceManager, "E00:/Character.nce", "E00:/Character.ncg", 0x1000, 1, 0x10000);
    for (u16 i = 0; i < 5; i++) {
        u16 sequence = sCharacterKnobSequences[i];
        Sprite_Init(&mButtons[i], 1, sequence, 0x80, 0x60, TRUE);
        func_02031eb0(&mButtons[i], sCharacterKnobSizes[i].x, sCharacterKnobSizes[i].y);
        func_02031278(&mButtons[i], sequence, sequence + 1);
        func_02031260(&mButtons[i], sCharacterKnobKinds[i]);
        mButtons[i].SetPositionFromVec(sCharacterKnobPositions[i].x << FX32_SHIFT,
                                       sCharacterKnobPositions[i].y << FX32_SHIFT);
        func_0203119c(&mButtons[i]);
        mButtons[i].mVisible = FALSE;
    }
    for (int i = 0; i < 5; i++) {
        u8 value = mScene->mCharacter.mUnknown04A[i];
        if (value == 0) {
            mButtons[i].mVisible = FALSE;
        } else {
            u8 index = value - 1;
            mButtons[i].mVisible = TRUE;
            mButtons[i].mPosition.x = (0x87 - (4 - index) * 14) << FX32_SHIFT;
        }
    }
    for (u16 i = 0; i < 5; i++) {
        Sprite_Init(&mSprites[i], 1, sCharacterLabelSequences[i], 0x80, 0x60, TRUE);
        mSprites[i].SetPositionFromVec(sCharacterLabelPositions[i].x << FX32_SHIFT,
                                       sCharacterLabelPositions[i].y << FX32_SHIFT);
    }
    mScene->MoveCameraTo(sCharacterCameraGoal);
}

// Whether all five values are set
static inline BOOL IsPersonalitySet(MiiEditScene *scene) {
    BOOL set = TRUE;
    for (int i = 0; i < 5; i++) {
        if (scene->mCharacter.mUnknown04A[i] == 0) {
            set = FALSE;
            break;
        }
    }
    return set;
}

void MiiEditCharacterPage::func_ov076_02157e3c() {
    if (gTouchInput.mTouching == 0) {
        return;
    }
    ScreenPoint point = *(gTouchInput.mUnknown4C ? &gTouchInput.mLastPoint : &gTouchInput.mPoint);
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 8; j++) {
            ScreenRect rect = sCharacterValueRects[j];
            rect.y += sCharacterKnobPositions[i].y;
            if (func_02028d88(&rect, &point) && j + 1 != mScene->mCharacter.mUnknown04A[i]) {
                func_02098d5c(&mScene->mCharacter, i, j + 1);
                func_02091a6c(&mScene->mCharacter);
                mButtons[i].mVisible = TRUE;
                mButtons[i].mPosition.x = (0x87 - (4 - j) * 14) << FX32_SHIFT;
                gSoundManager.StartSeqArc(1, 14, 1);
            }
        }
    }
    if (mScene->mButtons[4].mUnknownB8 == 0) {
        return;
    }
    BOOL set = TRUE;
    for (int i = 0; i < 5; i++) {
        if (mScene->mCharacter.mUnknown04A[i] == 0) {
            set = FALSE;
            break;
        }
    }
    if (!set) {
        return;
    }
    func_02031e88(&mScene->mButtons[4], FALSE);
    mScene->mUnknown58DC = TRUE;
    mScene->mSprite582C.func_ov076_0214bec8(4);
}


void MiiEditCharacterPage::Draw() {
    for (u16 i = 0; i < 5; i++) {
        func_02031d7c(&mButtons[i]);
    }
    for (int i = 0; i < 5; i++) {
        Sprite_Draw(&mSprites[i]);
    }
}

void MiiEditCharacterPage::Exit() {}

// --- Pages 5 and 6 ---

// What these pages use of the main program (stand-ins)
struct SceneManager {
    /* 0x0 */ Scene *mScene;
    /* 0x4 */ int mNextSceneId;
    /* 0x8 */ int mPreviousSceneId;
};

// What the scenes keep for one another (data_0213ba74)
struct SceneMemory {
    /* 0x00 */ u8 mUnknown00[0x20];
    /* 0x20 */ u8 mCharacter; // The resident a scene is about
};

extern "C" {
extern SceneManager gSceneManager;
extern SceneMemory data_0213ba74;
void func_0203c02c(SceneManager *manager, int sceneId); // Asks for the next scene
void func_02033844(MessageWindow *window, int number, int a, u8 sequence, u8 pressedSequence, WindowPoint position,
                   WindowPoint size, MessageFile *file, u32 message, const NNSG2dFont *font, int plane,
                   int *charOffset, const ButtonSounds *sounds, int b);
}

// The two pages are windows with two buttons, common messages 2 and 3 ("はい", "いいえ")
static const ScreenRect sQuestionWindowRect = { 20, 20, 216, 152 };
static const u8 sQuestionButtonPressedSequences[2] = { 106, 106 };
static const u8 sQuestionButtonSequences[2] = { 105, 105 };
static const WindowPoint sQuestionButtonPositions[2] = { { 75, 149 }, { 181, 149 } };
static const WindowPoint sQuestionButtonSizes[2] = { { 94, 30 }, { 94, 30 } };
static const u32 sQuestionButtonMessages[2] = { 2, 3 };
static const WindowPoint sQuestionTextPosition = { 38, 33 };
static const ButtonSounds sPage6ButtonSounds[2] = { { 15, -1, 0, -1 }, { 16, -1, 0, -1 } };
static const ButtonSounds sPage5ButtonSounds[2] = { { 14, -1, 0, -1 }, { 16, -1, 0, -1 } };

void MiiEditPage6::func_ov076_02158048(MiiEditScene *scene, int previousPage) {
    mScene = scene;
    mPreviousPage = previousPage;
    ScreenRect rect = sQuestionWindowRect;
    int charOffset = 0x384;
    func_02032fb8(&mWindow, rect);
    func_02033174(&mWindow, "E00:/Window_down.nsc", 0x100);
    func_020331bc(&mWindow, 0x3F00, rect);
    func_02034f4c(&mWindow, 1);
    int number = 0;
    for (int i = 0; i <= 1; i++) {
        func_02033844(&mWindow, number, 0, sQuestionButtonSequences[i], sQuestionButtonPressedSequences[i],
                      sQuestionButtonPositions[i], sQuestionButtonSizes[i], &mScene->mMessageFiles[1],
                      sQuestionButtonMessages[i], &gFonts[2].mFont, 0x1000, &charOffset, &sPage6ButtonSounds[i], 1);
        number++;
    }
    charOffset = func_02033a58(&mWindow, &mScene->mMessageFiles[0], 0x2D, &gFonts[0].mFont, 0x1000,
                               sQuestionTextPosition, &charOffset);
    func_02034f4c(&mWindow, 1);
    func_02034f94(&mWindow, 1);
    func_02034720(&mWindow, MessageWindow::FLAG_OPEN | MessageWindow::FLAG_FROM_BELOW, 1);
}

void MiiEditPage6::func_ov076_0215821c() {
    func_020341d4(&mWindow);
    if (mWindow.mResult != -1) {
        mWindow.mUnknownSetBeforeClose = 15;
        func_020349cc(&mWindow, MessageWindow::FLAG_CLOSE | 0x20);
        switch (mWindow.mResult) {
        case 0:
            mNextPage = -1;
            break;
        case 1:
            mNextPage = mPreviousPage;
            break;
        }
    }
    if (!mWindow.mTransitionDone) {
        return;
    }
    if (mNextPage == -1) {
        mScene->func_ov076_0214b890();
        switch (mScene->mSceneId) {
        case 0x42:
            func_0203c02c(&gSceneManager, 0x1A);
            break;
        case 0x43:
        case 0x44:
            data_0213ba74.mCharacter = mScene->mUnknown5824;
            func_0203c02c(&gSceneManager, 0x74);
            break;
        case 0x6F:
            data_0213ba74.mCharacter = mScene->mUnknown5824;
            func_0203c02c(&gSceneManager, 0x69);
            break;
        }
    } else {
        mScene->func_ov076_0214b1ec(mNextPage);
    }
}

void MiiEditPage6::Draw() {
    func_020345d8(&mWindow);
}

void MiiEditPage6::Exit() {
    func_020346cc(&mWindow);
}

void MiiEditPage5::func_ov076_0215835c(MiiEditScene *scene, int previousPage) {
    mScene = scene;
    mPreviousPage = previousPage;
    ScreenRect rect = sQuestionWindowRect;
    int charOffset = 0x384;
    func_02032fb8(&mWindow, rect);
    func_02033174(&mWindow, "E00:/Window_down.nsc", 0x100);
    func_020331bc(&mWindow, 0x3F00, rect);
    func_02034f4c(&mWindow, 1);
    int number = 0;
    for (int i = 0; i <= 1; i++) {
        func_02033844(&mWindow, number, 0, sQuestionButtonSequences[i], sQuestionButtonPressedSequences[i],
                      sQuestionButtonPositions[i], sQuestionButtonSizes[i], &mScene->mMessageFiles[1],
                      sQuestionButtonMessages[i], &gFonts[2].mFont, 0x1000, &charOffset, &sPage5ButtonSounds[i], 1);
        number++;
    }
    func_02033a58(&mWindow, &mScene->mMessageFiles[0], 0x2E, &gFonts[0].mFont, 0x1000, sQuestionTextPosition,
                  &charOffset);
    func_02034f4c(&mWindow, 1);
    func_02034f94(&mWindow, 1);
    func_02034720(&mWindow, MessageWindow::FLAG_OPEN | MessageWindow::FLAG_FROM_BELOW, 1);
}

void MiiEditPage5::func_ov076_0215852c() {
    func_020341d4(&mWindow);
    if (mWindow.mResult != -1) {
        mWindow.mUnknownSetBeforeClose = 15;
        func_020349cc(&mWindow, MessageWindow::FLAG_CLOSE | 0x20);
        switch (mWindow.mResult) {
        case 0:
            mNextPage = -1;
            break;
        case 1:
            mNextPage = mPreviousPage;
            break;
        }
    }
    if (!mWindow.mTransitionDone) {
        return;
    }
    if (mNextPage == -1) {
        switch (mScene->mSceneId) {
        case 0x42:
        case 0x44:
            func_0203c02c(&gSceneManager, 0x1A);
            break;
        case 0x43:
            func_0203c02c(&gSceneManager, 0x5E);
            break;
        }
    } else {
        mScene->func_ov076_0214b1ec(mNextPage);
    }
}

void MiiEditPage5::Draw() {
    func_020345d8(&mWindow);
}

void MiiEditPage5::Exit() {
    func_020346cc(&mWindow);
}

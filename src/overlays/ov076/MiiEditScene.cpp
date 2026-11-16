#include "MiiEditScene.h"

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

// --- The face page's parts ---

extern "C" {
BOOL func_02031d00(SpriteButton *button); // Whether it is selected (mUnknownB0)
void func_02031ca4(SpriteButton *button); // Shows its pressed sequence, playing
// A Mii parameter of the character (numbered as the Mii data's getters and setters are, data_020fbc2c)
int func_02098dac(CharacterBase *character, int parameter);
void func_020986c4(CharacterBase *character, int parameter, int *min, int *max); // Its range
void func_0208fed8(CharacterBase *character, int parameter, int value, BOOL a);  // Sets it
void func_02091a88(CharacterBase *character);
void ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
// Of the scene: plays particle effect "emitter" at the character's head, raised by y
void func_ov076_0214b5dc(MiiEditScene *scene, int emitter, fx32 y);
}

// The part type that each of the parts' buttons shows, in the order of the buttons: hair (Mii parameter 0x19),
// eyebrows (6), eyes (0), nose (0x1C) and mouth (0xC)
static const int sHairTypes[72] = {
    33, 40, 51, 44, 39, 70, 45, 49, 59, 56, 68, 31, 32, 47, 37, 48, 66, 52, 58, 50, 55, 64, 60, 62,
    43, 38, 42, 23, 67, 54, 36, 41, 65, 30, 57, 34, 12, 13, 69, 26, 4,  25, 1,  19, 5,  8,  27, 7,
    14, 3,  22, 10, 6,  20, 11, 63, 17, 35, 21, 0,  61, 16, 46, 9,  18, 2,  28, 53, 71, 24, 15, 29,
};
static const int sEyebrowTypes[24] = {
    6, 0, 12, 1, 9, 19, 7, 21, 8, 17, 5, 4, 11, 10, 2, 3, 14, 20, 15, 13, 22, 18, 16, 23,
};
static const int sEyeTypes[48] = {
    2, 4,  0,  8,  39, 17, 1,  26, 16, 15, 27, 20, 33, 11, 19, 32, 9,  12, 23, 34, 21, 25, 40, 35,
    5, 41, 13, 36, 37, 6,  24, 30, 31, 18, 28, 46, 7,  44, 38, 42, 45, 29, 3,  43, 22, 10, 14, 47,
};
static const int sNoseTypes[12] = { 1, 10, 2, 3, 6, 0, 5, 4, 8, 9, 7, 11 };
static const int sMouthTypes[24] = {
    23, 1, 19, 21, 22, 5, 0, 8, 10, 16, 6, 13, 7, 9, 2, 17, 3, 4, 15, 11, 20, 18, 14, 12,
};
// By eye type, and by eyebrow type: SetParameter turns the eyes or the eyebrows by the difference between the old
// type's entry and the new one's
static const u8 sEyeRotations[50] = {
    29, 28, 28, 28, 29, 28, 28, 28, 29, 28, 28, 28, 28, 29, 29, 28, 28, 28, 29, 29, 28, 29, 28, 29, 29,
    28, 29, 28, 28, 29, 28, 28, 28, 29, 29, 29, 28, 28, 29, 29, 29, 28, 28, 29, 29, 29, 29, 29, 28, 28,
};
static const u8 sEyebrowRotations[24] = {
    26, 26, 27, 25, 26, 25, 26, 25, 28, 25, 26, 24, 27, 27, 26, 26, 25, 25, 26, 26, 27, 26, 25, 27,
};

// Shows which of the eight colour buttons (28 to 35) is selected, and loads the parts' palette for the category
void MiiEditFacePage::func_ov076_0214ee7c(int selected, int category, int tab) {
    for (int i = 0; i <= 7; i++) {
        if (i == selected) {
            if (!func_02031d00(&mButtons[i + 28])) {
                func_02031e28(&mButtons[i + 28], TRUE);
            }
        } else {
            func_02031e28(&mButtons[i + 28], FALSE);
        }
    }
    int row;
    switch (category) {
    case 1:
        switch (tab) {
        case 0:
            row = 1;
            break;
        case 1:
            row = 0;
            break;
        }
        break;
    case 0:
    case 2:
    case 3:
    case 5:
    case 7:
        row = 1;
        break;
    case 4:
        row = selected + 2;
        break;
    case 6:
        row = selected + 8;
        break;
    }
    func_02027534(&data_021213b8, "E00:/Fw_parts.ncl", 0x1000, row << 5, 0, 0x20);
}

// Shows a page of the parts' buttons: "count" of them, from the page's first
void MiiEditFacePage::func_ov076_0214ef88(int page, u32 count, int category) {
    for (u16 i = 0; i < count; i++) {
        mButtons227C[i].mVisible = TRUE;
        int type = GetPartType(i + count * page, category);
        func_02031278(&mButtons227C[i], type * 2, type * 2 + 1);
        func_0203119c(&mButtons227C[i]);
    }
}

int MiiEditFacePage::GetPartType(int button, int category) {
    switch (category) {
    case 2:
        button = sHairTypes[button];
        break;
    case 3:
        button = sEyebrowTypes[button];
        break;
    case 4:
        button = sEyeTypes[button];
        break;
    case 5:
        button = sNoseTypes[button];
        break;
    case 6:
        button = sMouthTypes[button];
        break;
    case 7:
        break;
    }
    return button;
}

int MiiEditFacePage::GetPartButton(int type, int category) {
    int max, min;
    switch (category) {
    case 2:
        func_020986c4(&mScene->mCharacter, 0x19, &min, &max);
        for (int i = min; i <= max; i++) {
            if (type == sHairTypes[i]) {
                type = i;
                break;
            }
        }
        break;
    case 3:
        func_020986c4(&mScene->mCharacter, 6, &min, &max);
        for (int i = min; i <= max; i++) {
            if (type == sEyebrowTypes[i]) {
                type = i;
                break;
            }
        }
        break;
    case 4:
        func_020986c4(&mScene->mCharacter, 0, &min, &max);
        for (int i = min; i <= max; i++) {
            if (type == sEyeTypes[i]) {
                type = i;
                break;
            }
        }
        break;
    case 5:
        func_020986c4(&mScene->mCharacter, 0x1C, &min, &max);
        for (int i = min; i <= max; i++) {
            if (type == sNoseTypes[i]) {
                type = i;
                break;
            }
        }
        break;
    case 6:
        func_020986c4(&mScene->mCharacter, 0xC, &min, &max);
        for (int i = min; i <= max; i++) {
            if (type == sMouthTypes[i]) {
                type = i;
                break;
            }
        }
        break;
    case 7:
        break;
    }
    return type;
}

// One of the parts' buttons was chosen
void MiiEditFacePage::func_ov076_0214f264(int parameter, int selected, int page, u16 count) {
    for (int i = 0; i < count; i++) {
        if (i != (u16)selected) {
            func_02031e28(&mButtons227C[i], FALSE);
        }
    }
    SetParameter(parameter, GetPartType(page * count + selected, mCategory));
}

#define CLAMP(x, low, high) ((x) > (high) ? (high) : ((x) < (low) ? (low) : (x)))

// Compiled without common subexpressions: the range's ends are read from the stack after each call of the getter,
// where the default hoists one of them into a register before it
#pragma opt_common_subs off
// Sets a Mii parameter of the character, and plays the particle effect at a height that depends on the part
void MiiEditFacePage::SetParameter(int parameter, int value) {
    switch (parameter) {
    case 0x00:
    case 0x06:
    case 0x0C:
    case 0x10:
    case 0x12:
    case 0x15:
    case 0x19:
    case 0x1C:
    case 0x1F:
    case 0x23:
    case 0x25:
        mUnknown2DF0 = parameter;
        mUnknown2DEC = func_02098dac(&mScene->mCharacter, parameter);
        func_ov076_0214fa14(1);
        break;
    }
    switch (parameter) {
    case 0: {
        int delta = sEyeRotations[mUnknown2DEC] - sEyeRotations[value];
        int min, max;
        func_020986c4(&mScene->mCharacter, 3, &min, &max);
        func_0208fed8(&mScene->mCharacter, 3, CLAMP(func_02098dac(&mScene->mCharacter, 3) + delta, min, max),
                      FALSE);
        break;
    }
    case 6: {
        int delta = sEyebrowRotations[mUnknown2DEC] - sEyebrowRotations[value];
        int min, max;
        func_020986c4(&mScene->mCharacter, 9, &min, &max);
        func_0208fed8(&mScene->mCharacter, 9, CLAMP(func_02098dac(&mScene->mCharacter, 9) + delta, min, max),
                      FALSE);
        break;
    }
    }
    func_0208fed8(&mScene->mCharacter, parameter, value, TRUE);
    func_02091a88(&mScene->mCharacter);
    mScene->mUnknown580C = TRUE;
    switch (parameter) {
    case 0:
        func_ov076_0214b5dc(mScene, 0, (12 - func_02098dac(&mScene->mCharacter, 5)) * FX32_ONE / 30 + 0x333);
        break;
    case 1:
        func_ov076_0214b5dc(mScene, 0, 0x333);
        break;
    case 6:
        func_ov076_0214b5dc(mScene, 0, (10 - func_02098dac(&mScene->mCharacter, 0xB)) * FX32_ONE / 30 + 0x4CC);
        break;
    case 7:
        if (func_02098dac(&mScene->mCharacter, 6) == 0x17) {
            break;
        }
        func_ov076_0214b5dc(mScene, 0, 0x4CC);
        break;
    case 12:
        func_ov076_0214b5dc(mScene, 0, (13 - func_02098dac(&mScene->mCharacter, 0xF)) * FX32_ONE / 30 - 0x333);
        break;
    case 13:
        func_ov076_0214b5dc(mScene, 0, -0x333);
        break;
    case 16:
        func_ov076_0214b5dc(mScene, 0, -0x666);
        break;
    case 17:
        if (func_02098dac(&mScene->mCharacter, 0x10) == 0 && func_02098dac(&mScene->mCharacter, 0x12) == 0) {
            break;
        }
        func_ov076_0214b5dc(mScene, 0, -0x666);
        break;
    case 18:
        func_ov076_0214b5dc(mScene, 0, (10 - func_02098dac(&mScene->mCharacter, 0x14)) * FX32_ONE / 30 - 0x333);
        break;
    case 21:
        func_ov076_0214b5dc(mScene, 0, (20 - func_02098dac(&mScene->mCharacter, 0x18)) * FX32_ONE / 30);
        break;
    case 25:
        func_ov076_0214b5dc(mScene, 0, 0x800);
        break;
    case 26:
        if (func_02098dac(&mScene->mCharacter, 0x19) == 0x1E) {
            break;
        }
        func_ov076_0214b5dc(mScene, 0, 0x800);
        break;
    case 28:
        func_ov076_0214b5dc(mScene, 0, (9 - func_02098dac(&mScene->mCharacter, 0x1E)) * FX32_ONE / 30);
        break;
    case 31:
        func_ov076_0214b5dc(mScene, 0, (10 - func_02098dac(&mScene->mCharacter, 0x22)) * FX32_ONE / 30 + 0x333);
        break;
    case 32:
        if (func_02098dac(&mScene->mCharacter, 0x1F) == 0) {
            break;
        }
        func_ov076_0214b5dc(mScene, 0, 0x333);
        break;
    case 35:
    case 36:
    case 37:
        func_ov076_0214b5dc(mScene, 0, 0);
        break;
    }
}

#pragma opt_common_subs reset
// Moves a Mii parameter by a step, around its range or up to its ends; returns whether it stopped at an end
BOOL MiiEditFacePage::func_ov076_0214f930(int parameter, int step, BOOL wrap) {
    BOOL stopped = FALSE;
    int value = func_02098dac(&mScene->mCharacter, parameter);
    int min, max;
    value += step;
    func_020986c4(&mScene->mCharacter, parameter, &min, &max);
    if (wrap) {
        if (value < min) {
            value += max - min + 1;
        } else if (max < value) {
            value -= max - min + 1;
        }
    } else {
        if (value < min) {
            value = min;
            stopped = TRUE;
        } else if (max < value) {
            value = max;
            stopped = TRUE;
        }
    }
    func_0208fed8(&mScene->mCharacter, parameter, (u16)value, TRUE);
    mScene->mUnknown580C = TRUE;
    return stopped;
}

// Shows button 27 or hides it, and which way it faces
void MiiEditFacePage::func_ov076_0214fa14(int mode) {
    switch (mode) {
    case 0:
        mButtons[27].mVisible = FALSE;
        break;
    case 1:
        mButtons[27].mVisible = TRUE;
        mButtons[27].mFlipH = FALSE;
        mButtons[27].mSounds.mSounds[0] = 14;
        break;
    case 2:
        mButtons[27].mVisible = TRUE;
        mButtons[27].mFlipH = TRUE;
        mButtons[27].mSounds.mSounds[0] = 15;
        break;
    }
    mUnknown340C = mode;
}

void MiiEditFaceTab::Init(MiiEditFacePage *page) {
    mPage = page;
    mTab = 0;
    func_02031e28(&mPage->mButtons[8], TRUE);
}

void MiiEditFaceTab::func_ov076_0214faac() {}

// Where the face tab's buttons are: the face types', and the face textures'
static const ScreenPoint sFaceTypePositions[8] = {
    { 80, 40 }, { 123, 40 }, { 80, 77 }, { 123, 77 }, { 80, 114 }, { 123, 114 }, { 80, 151 }, { 123, 151 },
};
static const ScreenPoint sFaceTexturePositions[12] = {
    { 59, 40 },  { 102, 40 },  { 145, 40 },  { 59, 77 },  { 102, 77 },  { 145, 77 },
    { 59, 114 }, { 102, 114 }, { 145, 114 }, { 59, 151 }, { 102, 151 }, { 145, 151 },
};

void MiiEditFaceTab::SelectTab(int tab, BOOL pressed) {
    ObjResourceManager_Load(&gObjResourceManager, "E00:/Face.nce", "E00:/Face.ncg", 0x1000, 1, 0x10000);
    mPage->func_ov076_0214fa14(0);
    mPage->mButtons[8].mVisible = TRUE;
    mPage->mButtons[9].mVisible = TRUE;
    switch (tab) {
    case 0:
        for (u16 i = 0; i < 8; i++) {
            u16 sequence = i * 2;
            mPage->mButtons227C[i].mVisible = TRUE;
            func_02031278(&mPage->mButtons227C[i], sequence, sequence + 1);
            func_0203119c(&mPage->mButtons227C[i]);
            mPage->mButtons227C[i].SetPositionFromVec(sFaceTypePositions[i].x << FX32_SHIFT,
                                                      sFaceTypePositions[i].y << FX32_SHIFT);
        }
        for (u16 i = 8; i < 12; i++) {
            mPage->mButtons227C[i].mVisible = FALSE;
        }
        break;
    case 1:
        for (u16 i = 0; i < 12; i++) {
            mPage->mButtons227C[i].mVisible = TRUE;
            u16 sequence = i * 2 + 0x10;
            func_02031278(&mPage->mButtons227C[i], sequence, sequence + 1);
            func_0203119c(&mPage->mButtons227C[i]);
            mPage->mButtons227C[i].SetPositionFromVec(sFaceTexturePositions[i].x << FX32_SHIFT,
                                                      sFaceTexturePositions[i].y << FX32_SHIFT);
        }
        break;
    }
    int color = func_02098dac(&mPage->mScene->mCharacter, 0x24);
    switch (tab) {
    case 0: {
        u32 button = mPage->GetPartButton(func_02098dac(&mPage->mScene->mCharacter, 0x23), 1);
        button %= 8;
        if (!pressed) {
            func_02031e28(&mPage->mButtons227C[button], TRUE);
        } else {
            func_02031ca4(&mPage->mButtons227C[button]);
        }
        break;
    }
    case 1: {
        u32 button = mPage->GetPartButton(func_02098dac(&mPage->mScene->mCharacter, 0x25), 1);
        button %= 12;
        if (!pressed) {
            func_02031e28(&mPage->mButtons227C[button], TRUE);
        } else {
            func_02031ca4(&mPage->mButtons227C[button]);
        }
        break;
    }
    }
    func_02027534(&data_021213b8, "E00:/Fw_color.ncl", 0x1000, 0x60, 0x40, 0x60);
    for (u16 i = 28; i <= 35; i++) {
        if (i - 28 < 6) {
            mPage->mButtons[i].mVisible = TRUE;
        } else {
            mPage->mButtons[i].mVisible = FALSE;
        }
    }
    mPage->func_ov076_0214ee7c(color, 1, tab);
    mTab = tab;
    mUnknown4 = 0;
}

void MiiEditEtcTab::Init(MiiEditFacePage *page) {
    mPage = page;
    mTab = 0;
    func_02031e28(&mPage->mButtons[10], TRUE);
}

// Where the etc tab's buttons are: the glasses types', the mustache types' (and the beard types'), the mole types'
static const ScreenPoint sGlassesPositions[9] = {
    { 59, 77 },  { 102, 77 },  { 145, 77 },  { 59, 114 }, { 102, 114 },
    { 145, 114 }, { 59, 151 }, { 102, 151 }, { 145, 151 },
};
static const ScreenPoint sMustachePositions[4] = { { 80, 77 }, { 123, 77 }, { 80, 114 }, { 123, 114 } };
static const ScreenPoint sMolePositions[2] = { { 80, 77 }, { 123, 77 } };
// The face page's buttons that the etc tab places beside the types (inferred: the arrows that change a part's
// size and place, as the glasses and the mustache have two such parameters and the mole three), and where
static const int sAdjustButtons[4] = { 14, 15, 16, 17 };
static const ScreenPoint sAdjustButtonPositions[4] = { { 207, 48 }, { 239, 48 }, { 207, 74 }, { 239, 74 } };
static const int sMoleAdjustButtons[6] = { 14, 15, 16, 17, 22, 23 };
static const ScreenPoint sMoleAdjustButtonPositions[6] = {
    { 207, 48 }, { 239, 48 }, { 207, 74 }, { 239, 74 }, { 207, 100 }, { 239, 100 },
};

void MiiEditEtcTab::SelectTab(int tab, BOOL pressed) {
    mPage->mButtons[10].mVisible = TRUE;
    mPage->mButtons[11].mVisible = TRUE;
    mPage->mButtons[12].mVisible = TRUE;
    mPage->mButtons[13].mVisible = TRUE;
    mPage->func_ov076_0214fa14(0);
    ObjResourceManager_Load(&gObjResourceManager, "E00:/Etc.nce", "E00:/Etc.ncg", 0x1000, 1, 0x10000);
    switch (tab) {
    case 0:
        for (u16 i = 0; i < 9; i++) {
            u16 sequence = i * 2;
            mPage->mButtons227C[i].mVisible = TRUE;
            func_02031278(&mPage->mButtons227C[i], sequence, sequence + 1);
            func_0203119c(&mPage->mButtons227C[i]);
            mPage->mButtons227C[i].SetPositionFromVec(sGlassesPositions[i].x << FX32_SHIFT,
                                                      sGlassesPositions[i].y << FX32_SHIFT);
        }
        for (u16 i = 9; i < 12; i++) {
            mPage->mButtons227C[i].mVisible = FALSE;
        }
        break;
    case 1:
        for (u16 i = 0; i < 4; i++) {
            mPage->mButtons227C[i].mVisible = TRUE;
            u16 sequence = i * 2 + 0x12;
            func_02031278(&mPage->mButtons227C[i], sequence, sequence + 1);
            func_0203119c(&mPage->mButtons227C[i]);
            mPage->mButtons227C[i].SetPositionFromVec(sMustachePositions[i].x << FX32_SHIFT,
                                                      sMustachePositions[i].y << FX32_SHIFT);
        }
        for (u16 i = 4; i < 12; i++) {
            mPage->mButtons227C[i].mVisible = FALSE;
        }
        break;
    case 2:
        for (u16 i = 0; i < 2; i++) {
            mPage->mButtons227C[i].mVisible = TRUE;
            u16 sequence = i * 2 + 0x1A;
            func_02031278(&mPage->mButtons227C[i], sequence, sequence + 1);
            func_0203119c(&mPage->mButtons227C[i]);
            mPage->mButtons227C[i].SetPositionFromVec(sMolePositions[i].x << FX32_SHIFT,
                                                      sMolePositions[i].y << FX32_SHIFT);
        }
        for (u16 i = 2; i < 12; i++) {
            mPage->mButtons227C[i].mVisible = FALSE;
        }
        break;
    case 3:
        for (u16 i = 0; i < 4; i++) {
            mPage->mButtons227C[i].mVisible = TRUE;
            u16 sequence = i * 2 + 0x1E;
            func_02031278(&mPage->mButtons227C[i], sequence, sequence + 1);
            func_0203119c(&mPage->mButtons227C[i]);
            mPage->mButtons227C[i].SetPositionFromVec(sMustachePositions[i].x << FX32_SHIFT,
                                                      sMustachePositions[i].y << FX32_SHIFT);
        }
        for (u16 i = 4; i < 12; i++) {
            mPage->mButtons227C[i].mVisible = FALSE;
        }
        break;
    }
    for (u16 i = 0; i < 6; i++) {
        mPage->mButtons[sMoleAdjustButtons[i]].mVisible = FALSE;
    }
    switch (tab) {
    case 0: {
        u32 button = mPage->GetPartButton(func_02098dac(&mPage->mScene->mCharacter, 0x1F), 7);
        button %= 9;
        int color = func_02098dac(&mPage->mScene->mCharacter, 0x20);
        if (!pressed) {
            func_02031e28(&mPage->mButtons227C[button], TRUE);
        } else {
            func_02031ca4(&mPage->mButtons227C[button]);
        }
        func_02027534(&data_021213b8, "E00:/Fw_color.ncl", 0x1000, 0x140, 0x40, 0x60);
        for (u16 i = 28; i <= 35; i++) {
            if (i - 28 < 6) {
                mPage->mButtons[i].mVisible = TRUE;
            } else {
                mPage->mButtons[i].mVisible = FALSE;
            }
        }
        mPage->func_ov076_0214ee7c(color, 7, -1);
        for (u16 i = 0; i < 4; i++) {
            mPage->mButtons[sAdjustButtons[i]].mVisible = TRUE;
            mPage->mButtons[sAdjustButtons[i]].SetPositionFromVec(sAdjustButtonPositions[i].x << FX32_SHIFT,
                                                                  sAdjustButtonPositions[i].y << FX32_SHIFT);
        }
        break;
    }
    case 1: {
        u32 button = mPage->GetPartButton(func_02098dac(&mPage->mScene->mCharacter, 0x12), 7);
        button %= 4;
        int color = func_02098dac(&mPage->mScene->mCharacter, 0x11);
        if (!pressed) {
            func_02031e28(&mPage->mButtons227C[button], TRUE);
        } else {
            func_02031ca4(&mPage->mButtons227C[button]);
        }
        func_02027534(&data_021213b8, "E00:/Fw_color.ncl", 0x1000, 0xA0, 0x40, 0x60);
        for (u16 i = 28; i <= 35; i++) {
            if (i - 28 < 8) {
                mPage->mButtons[i].mVisible = TRUE;
            } else {
                mPage->mButtons[i].mVisible = FALSE;
            }
        }
        mPage->func_ov076_0214ee7c(color, 7, -1);
        for (u16 i = 0; i < 4; i++) {
            mPage->mButtons[sAdjustButtons[i]].mVisible = TRUE;
            mPage->mButtons[sAdjustButtons[i]].SetPositionFromVec(sAdjustButtonPositions[i].x << FX32_SHIFT,
                                                                  sAdjustButtonPositions[i].y << FX32_SHIFT);
        }
        break;
    }
    case 2: {
        u32 button = mPage->GetPartButton(func_02098dac(&mPage->mScene->mCharacter, 0x15), 7);
        button %= 2;
        if (!pressed) {
            func_02031e28(&mPage->mButtons227C[button], TRUE);
        } else {
            func_02031ca4(&mPage->mButtons227C[button]);
        }
        func_02027534(&data_021213b8, "E00:/Fw_color.ncl", 0x1000, 0xA0, 0x40, 0x60);
        for (u16 i = 28; i <= 35; i++) {
            if (i - 28 < 0) {
                mPage->mButtons[i].mVisible = TRUE;
            } else {
                mPage->mButtons[i].mVisible = FALSE;
            }
        }
        mPage->func_ov076_0214ee7c(0, 7, -1);
        for (u16 i = 0; i < 6; i++) {
            mPage->mButtons[sMoleAdjustButtons[i]].mVisible = TRUE;
            mPage->mButtons[sMoleAdjustButtons[i]].SetPositionFromVec(
                sMoleAdjustButtonPositions[i].x << FX32_SHIFT, sMoleAdjustButtonPositions[i].y << FX32_SHIFT);
        }
        break;
    }
    case 3: {
        u32 button = mPage->GetPartButton(func_02098dac(&mPage->mScene->mCharacter, 0x10), 7);
        button %= 4;
        int color = func_02098dac(&mPage->mScene->mCharacter, 0x11);
        if (!pressed) {
            func_02031e28(&mPage->mButtons227C[button], TRUE);
        } else {
            func_02031ca4(&mPage->mButtons227C[button]);
        }
        func_02027534(&data_021213b8, "E00:/Fw_color.ncl", 0x1000, 0xA0, 0x40, 0x60);
        for (u16 i = 28; i <= 35; i++) {
            if (i - 28 < 8) {
                mPage->mButtons[i].mVisible = TRUE;
            } else {
                mPage->mButtons[i].mVisible = FALSE;
            }
        }
        mPage->func_ov076_0214ee7c(color, 7, -1);
        break;
    }
    }
    mTab = tab;
    mUnknown4 = 0;
}

// --- The profile page ---

// The touch screen as the main program keeps it (gTouchInput)
struct TouchInput {
    /* 0x00 */ u8 mUnknown00[0x28];
    /* 0x28 */ TPData mTp;
    /* 0x30 */ ScreenPoint mLastPoint;
    /* 0x38 */ ScreenPoint mPoint;
    /* 0x40 */ u8 mUnknown40[8];
    /* 0x48 */ BOOL mTouched; // The stylus went down this frame
    /* 0x4C */ BOOL mUnknown4C;
};

// The buttons of the pad (data_0211f9d4)
struct PadInput {
    /* 0x00 */ u8 mUnknown00[0x20];
    /* 0x20 */ u32 mUnknown20;
};

// The word at 0x50 of the saved character: bits 17 to 28 are a year, 1900 to 2099 (0xFFF: none), which the
// profile page's year window changes (func_02098cf4 sets it). A STAND-IN until the record's members are known.
struct CharacterYear {
    u32 mUnknown : 17;
    u32 mYear : 12;
};

extern "C" {
extern TouchInput gTouchInput;
extern PadInput data_0211f9d4;
BOOL func_02024004(ScreenControl *screens, int planes, int level);
BOOL func_02024118(ScreenControl *screens, int planes, int level);
// The keyboard: its update with the touch screen, ...
void func_02083244(SoftKeyboard *keyboard, int a, TPData tp);
void func_020831a4(SoftKeyboard *keyboard);
BOOL func_02083614(void *key, ScreenPoint point);
BOOL func_02083308(void *key);
u16 func_02082560(void); // The character typed (8 erases, 7 and 9 add the voicing marks, 11 makes it small)
BOOL func_02031294(SpriteButton *button);
void func_02098c40(CharacterBase *character, int parameter, int value); // Sets a Mii parameter
void func_02098cf4(CharacterBase *character, u16 year);
void func_02098d44(CharacterBase *character, int a);
void func_02098cd0(CharacterBase *character, int a);
void MiiData_SetName(CharacterBase *character, const wchar_t *name);
void MiiData_SetCreatorName(CharacterBase *character, const wchar_t *name);
wchar_t *wcscpy(wchar_t *dst, const wchar_t *src);
// Of the scene and of the profile page, after this
void func_ov076_0214b694(MiiEditScene *scene);
void func_ov076_02153cf4(MiiEditProfilePage *page, const wchar_t *text);
void func_ov076_02153e54(MiiEditProfilePage *page, int year);
void func_ov076_02153fb8(MiiEditProfilePage *page, int day);
void func_ov076_021540a4(MiiEditProfilePage *page, int month);
void func_ov076_02154574(MiiEditProfilePage *page);
void func_ov076_021545dc(MiiEditProfilePage *page, int state);
}

static const VecFx32 data_ov076_0215885c = { 0, 0x1C50, 0x19BF0 };
// The sequences of the windows' choices (each choice shows the next sequence while it is chosen)
static const u16 data_ov076_02158df4[29] = {
    0x18, 0x1A, 0x1C, 0x1E, 0x24, 0x26, 0x28, 0x2A, 0x2C, 0x2E, 0x30, 0x32, 0x34, 0x36, 0x38,
    0x3A, 0x20, 0x22, 0x3C, 0x3E, 0x3C, 0x3E, 0x40, 0x42, 0x4F, 0x4F, 0x4F, 0x4F, 0x4F,
};

void MiiEditProfilePage::func_ov076_02150874(MiiEditScene *scene) {
    mState = -1;
    mScene = scene;
    scene->MoveCameraTo(data_ov076_0215885c);
    func_ov076_02151f0c(1);
}

// The original reads the windows' choice and result (MessageWindow::mUnknownAF7C, mResult) anew at each use here,
// as it would a volatile. Declaring them volatile in MessageWindow.h does the same, but costs overlay 106's
// PreStartScene::Ask its match, so the volatile is in this file's reads only (a STAND-IN for whatever the
// original's declaration was).
#define READ(x) (*(volatile int *)&(x))

void MiiEditProfilePage::func_ov076_021508ac() {
    switch (mState) {
    case 0:
        if (mUnknown00004++ <= 2) {
            return;
        }
        func_ov076_02151f0c(mState + 1);
        return;
    case 2:
    case 4:
    case 6:
    case 8:
        if (!func_02024118(&data_0211d604, 0x3F00, 2)) {
            return;
        }
        func_ov076_02151f0c(mState + 1);
        return;
    case 1:
        if (!func_02024004(&data_0211d604, 0x3F00, -2)) {
            return;
        }
        for (int i = 0; i < 10; i++) {
            if (func_0203133c(&mButtons[i])) {
                switch (i) {
                case 0:
                    func_ov076_02151f0c(4);
                    break;
                case 1:
                    func_ov076_02151f0c(2);
                    break;
                case 2:
                    func_ov076_02151f0c(13);
                    break;
                case 3:
                    func_ov076_02151f0c(11);
                    break;
                case 4:
                    func_ov076_02151f0c(6);
                    break;
                case 5:
                    func_ov076_02151f0c(14);
                    break;
                case 6:
                    func_ov076_02151f0c(12);
                    break;
                case 7:
                    func_ov076_02151f0c(8);
                    break;
                case 8:
                    func_ov076_02151f0c(10);
                    break;
                case 9:
                    func_ov076_02151f0c(16);
                    break;
                }
            }
        }
        return;
    case 3:
    case 5:
    case 7:
    case 9: {
        func_02083244(&mKeyboard, 0, gTouchInput.mTp);
        if (!func_02024004(&data_0211d604, 0x3F00, -2)) {
            return;
        }
        if (func_0203133c(&mButton)) {
            func_ov076_02151f0c(0);
        } else if (gTouchInput.mTp.touch != 0 || gTouchInput.mUnknown4C != 0 || data_0211f9d4.mUnknown20 == 0) {
            func_020831a4(&mKeyboard);
        }
        if (!func_02031294(&mButton) && gTouchInput.mUnknown4C) {
            func_ov076_021545dc(this, mState);
        }
        if (gTouchInput.mTouched) {
            ScreenPoint point = gTouchInput.mPoint;

            // The second is reached from the first by a cast, which the original's address computation shows
            if (!func_02083614(&mKeyboard.mUnknown5178, point) &&
                !func_02083614((u8 *)&mKeyboard.mUnknown5178 + 0x6B8, point)) {
                int x;
                u16 c = func_02082560();
                if (c >= 0x20) {
                    if (mKeyboard.mLength >= 10) {
                        return;
                    }
                    mKeyboard.mText[mKeyboard.mLength] = c;
                    x = mKeyboard.mLength * 13 + 5;
                    const NNSG2dFont *font = mScene->mCharBox.mFont;
                    const NNSG2dCharWidths *widths = NNS_G2dFontGetCharWidths(font, c);
                    x -= widths->charWidth >> 1;
                    mScene->mCharBox.mCharX = x;
                    mScene->mCharBox.SetChar(c);
                    mScene->mCharBox.Redraw();
                    mKeyboard.mLength++;
                    if (mKeyboard.mLength >= 10) {
                        return;
                    }
                    mKeyboard.mUnknown5F8C += 0xD000;
                } else if (c == 8) {
                    if (mKeyboard.mLength <= 0) {
                        return;
                    }
                    if (mKeyboard.mLength < 10) {
                        mKeyboard.mUnknown5F8C -= 0xD000;
                    }
                    mKeyboard.mLength--;
                    mKeyboard.mText[mKeyboard.mLength] = 0;
                    wchar_t text[10];
                    wcscpy(text, mKeyboard.mText);
                    func_ov076_02153cf4(this, text);
                } else if (c == 7) {
                    int last = mKeyboard.mLength - 1;
                    if (last < 0) {
                        return;
                    }
                    u16 ch = mKeyboard.mText[last];
                    BOOL changed = FALSE;
                    // ka ki ku ke ko, KA to KO, sa to so, SA to SO, ta chi tsu te to, TA to TO, ha to ho, HA to HO
                    if (ch == 0x304B || ch == 0x304D || ch == 0x304F || ch == 0x3051 || ch == 0x3053 || ch == 0x30AB ||
                        ch == 0x30AD || ch == 0x30AF || ch == 0x30B1 || ch == 0x30B3 || ch == 0x3055 || ch == 0x3057 ||
                        ch == 0x3059 || ch == 0x305B || ch == 0x305D || ch == 0x30B5 || ch == 0x30B7 || ch == 0x30B9 ||
                        ch == 0x30BB || ch == 0x30BD || ch == 0x305F || ch == 0x3061 || ch == 0x3064 || ch == 0x3066 ||
                        ch == 0x3068 || ch == 0x30BF || ch == 0x30C1 || ch == 0x30C4 || ch == 0x30C6 || ch == 0x30C8 ||
                        ch == 0x306F || ch == 0x3072 || ch == 0x3075 || ch == 0x3078 || ch == 0x307B || ch == 0x30CF ||
                        ch == 0x30D2 || ch == 0x30D5 || ch == 0x30D8 || ch == 0x30DB) {
                        mKeyboard.mText[last] = ch + 1;
                        changed = TRUE;
                    } else if (ch == 0x304C || ch == 0x304E || ch == 0x3050 || ch == 0x3052 || ch == 0x3054 ||
                               ch == 0x30AC || ch == 0x30AE || ch == 0x30B0 || ch == 0x30B2 || ch == 0x30B4 ||
                               ch == 0x3056 || ch == 0x3058 || ch == 0x305A || ch == 0x305C || ch == 0x305E ||
                               ch == 0x30B6 || ch == 0x30B8 || ch == 0x30BA || ch == 0x30BC || ch == 0x30BE ||
                               ch == 0x3060 || ch == 0x3062 || ch == 0x3065 || ch == 0x3067 || ch == 0x3069 ||
                               ch == 0x30C0 || ch == 0x30C2 || ch == 0x30C5 || ch == 0x30C7 || ch == 0x30C9 ||
                               ch == 0x3070 || ch == 0x3073 || ch == 0x3076 || ch == 0x3079 || ch == 0x307C ||
                               ch == 0x30D0 || ch == 0x30D3 || ch == 0x30D6 || ch == 0x30D9 || ch == 0x30DC) {
                        mKeyboard.mText[last] = ch - 1;
                        changed = TRUE;
                    } else if (ch == 0x30A6) { // U, VU
                        mKeyboard.mText[last] = 0x30F4;
                        changed = TRUE;
                    } else if (ch == 0x30F4) {
                        mKeyboard.mText[last] = 0x30A6;
                        changed = TRUE;
                    }
                    if (!changed) {
                        return;
                    }
                    wchar_t text[10];
                    wcscpy(text, mKeyboard.mText);
                    func_ov076_02153cf4(this, text);
                } else if (c == 9) {
                    int last = mKeyboard.mLength - 1;
                    if (last < 0) {
                        return;
                    }
                    u16 ch = mKeyboard.mText[last];
                    BOOL changed = FALSE;
                    // ha hi fu he ho, HA to HO
                    if (ch == 0x306F || ch == 0x3072 || ch == 0x3075 || ch == 0x3078 || ch == 0x307B || ch == 0x30CF ||
                        ch == 0x30D2 || ch == 0x30D5 || ch == 0x30D8 || ch == 0x30DB) {
                        mKeyboard.mText[last] = ch + 2;
                        changed = TRUE;
                    } else if (ch == 0x3071 || ch == 0x3074 || ch == 0x3077 || ch == 0x307A || ch == 0x307D ||
                               ch == 0x30D1 || ch == 0x30D4 || ch == 0x30D7 || ch == 0x30DA || ch == 0x30DD) {
                        mKeyboard.mText[last] = ch - 2;
                        changed = TRUE;
                    }
                    if (!changed) {
                        return;
                    }
                    wchar_t text[10];
                    wcscpy(text, mKeyboard.mText);
                    func_ov076_02153cf4(this, text);
                } else if (c == 11) {
                    int last = mKeyboard.mLength - 1;
                    if (last < 0) {
                        return;
                    }
                    u16 ch = mKeyboard.mText[last];
                    BOOL changed = FALSE;
                    // a i u e o, A to O, tsu, TSU, ya, YA, yu, YU, yo, YO, wa, WA
                    if (ch == 0x3042 || ch == 0x3044 || ch == 0x3046 || ch == 0x3048 || ch == 0x304A || ch == 0x30A2 ||
                        ch == 0x30A4 || ch == 0x30A6 || ch == 0x30A8 || ch == 0x30AA || ch == 0x3064 || ch == 0x30C4 ||
                        ch == 0x3084 || ch == 0x30E4 || ch == 0x3086 || ch == 0x30E6 || ch == 0x3088 || ch == 0x30E8 ||
                        ch == 0x308F || ch == 0x30EF) {
                        mKeyboard.mText[last] = ch - 1;
                        changed = TRUE;
                    } else if (ch == 0x3041 || ch == 0x3043 || ch == 0x3045 || ch == 0x3047 || ch == 0x3049 ||
                               ch == 0x30A1 || ch == 0x30A3 || ch == 0x30A5 || ch == 0x30A7 || ch == 0x30A9 ||
                               ch == 0x3063 || ch == 0x30C3 || ch == 0x3083 || ch == 0x30E3 || ch == 0x3085 ||
                               ch == 0x30E5 || ch == 0x3087 || ch == 0x30E7 || ch == 0x308E || ch == 0x30EE) {
                        mKeyboard.mText[last] = ch + 1;
                        changed = TRUE;
                    } else if (ch == 0x30F5) { // Small KA, KA
                        mKeyboard.mText[last] = 0x30AB;
                        changed = TRUE;
                    } else if (ch == 0x30AB) {
                        mKeyboard.mText[last] = 0x30F5;
                        changed = TRUE;
                    } else if (ch == 0x30F6) { // Small KE, KE
                        mKeyboard.mText[last] = 0x30B1;
                        changed = TRUE;
                    } else if (ch == 0x30B1) {
                        mKeyboard.mText[last] = 0x30F6;
                        changed = TRUE;
                    }
                    if (!changed) {
                        return;
                    }
                    wchar_t text[10];
                    wcscpy(text, mKeyboard.mText);
                    func_ov076_02153cf4(this, text);
                }
                return;
            }
        }
        if (!func_02083308(mKeyboard.mUnknown5178[0])) {
            return;
        }
        switch (mState) {
        case 7:
            MiiData_SetName(&mScene->mCharacter, mKeyboard.mText);
            break;
        case 3:
            wcscpy((wchar_t *)&mScene->mCharacter.mUnknown04A[0xE], mKeyboard.mText);
            break;
        case 5:
            wcscpy((wchar_t *)&mScene->mCharacter.mUnknown04A[0x24], mKeyboard.mText);
            break;
        case 9:
            MiiData_SetCreatorName(&mScene->mCharacter, mKeyboard.mText);
            break;
        }
        func_ov076_02151f0c(0);
        return;
    }
    case 11: {
        func_020341d4(&mWindows[2]);
        int result = READ(mWindows[2].mResult);
        if (result != -1) {
            switch (result) {
            case 0: {
                int year = ((CharacterYear *)&mScene->mCharacter.mUnknown04A[6])->mYear + 1;
                if (year > 2099) {
                    year = 1900;
                }
                func_ov076_02153e54(this, year);
                func_02098cf4(&mScene->mCharacter, year);
                func_ov076_02154574(this);
                break;
            }
            case 1: {
                int year = ((CharacterYear *)&mScene->mCharacter.mUnknown04A[6])->mYear - 1;
                if (year < 1900) {
                    year = 2099;
                }
                func_ov076_02153e54(this, year);
                func_02098cf4(&mScene->mCharacter, year);
                func_ov076_02154574(this);
                break;
            }
            }
        }
        if (READ(mWindows[2].mResult) != -1) {
            switch (READ(mWindows[2].mResult)) {
            case 2:
                func_02098cf4(&mScene->mCharacter, 0xFFF);
                mWindows[2].mUnknownSetBeforeClose = 15;
                func_020349cc(&mWindows[2], 0x24);
                break;
            case 3:
                mWindows[2].mUnknownSetBeforeClose = 15;
                func_020349cc(&mWindows[2], 0x24);
                break;
            }
        }
        if (mWindows[2].mTransitionDone) {
            func_ov076_02151f0c(1);
        }
        return;
    }
    case 12: {
        func_020341d4(&mWindows[3]);
        int result = READ(mWindows[3].mResult);
        if (result != -1) {
            switch (result) {
            case 0: {
                int month = func_02098dac(&mScene->mCharacter, 0x26) + 1;
                if (month > 12) {
                    month = 1;
                }
                func_ov076_021540a4(this, month);
                func_02098c40(&mScene->mCharacter, 0x26, month);
                int min, max;
                func_020986c4(&mScene->mCharacter, 0x27, &min, &max);
                int day = func_02098dac(&mScene->mCharacter, 0x27);
                if (day > max) {
                    day = max;
                }
                func_ov076_02153fb8(this, day);
                func_02098c40(&mScene->mCharacter, 0x27, day);
                break;
            }
            case 1: {
                int month = func_02098dac(&mScene->mCharacter, 0x26) - 1;
                if (month < 1) {
                    month = 12;
                }
                func_ov076_021540a4(this, month);
                func_02098c40(&mScene->mCharacter, 0x26, month);
                int min, max;
                func_020986c4(&mScene->mCharacter, 0x27, &min, &max);
                int day = func_02098dac(&mScene->mCharacter, 0x27);
                if (day > max) {
                    day = max;
                }
                func_ov076_02153fb8(this, day);
                func_02098c40(&mScene->mCharacter, 0x27, day);
                break;
            }
            case 2: {
                int min, max;
                func_020986c4(&mScene->mCharacter, 0x27, &min, &max);
                int day = func_02098dac(&mScene->mCharacter, 0x27) + 1;
                if (day > max) {
                    day = 1;
                }
                func_ov076_02153fb8(this, day);
                func_02098c40(&mScene->mCharacter, 0x27, day);
                break;
            }
            case 3: {
                int min, max;
                func_020986c4(&mScene->mCharacter, 0x27, &min, &max);
                int day = func_02098dac(&mScene->mCharacter, 0x27) - 1;
                if (day < 1) {
                    day = max;
                }
                func_ov076_02153fb8(this, day);
                func_02098c40(&mScene->mCharacter, 0x27, day);
                break;
            }
            }
        }
        if (READ(mWindows[3].mResult) != -1) {
            switch (READ(mWindows[3].mResult)) {
            case 4:
                func_02098c40(&mScene->mCharacter, 0x26, 0);
                func_02098c40(&mScene->mCharacter, 0x27, 0);
                mWindows[3].mUnknownSetBeforeClose = 15;
                func_020349cc(&mWindows[3], 0x24);
                break;
            case 5:
                mWindows[3].mUnknownSetBeforeClose = 15;
                func_020349cc(&mWindows[3], 0x24);
                break;
            }
        }
        if (mWindows[3].mTransitionDone) {
            func_ov076_02151f0c(1);
        }
        return;
    }
    case 13: {
        func_020341d4(&mWindows[0]);
        int i;
        int choice;
        if (READ(mWindows[0].mUnknownAF7C) != -1 && (choice = READ(mWindows[0].mUnknownAF7C)) != 4) {
            switch (choice) {
            case 0:
                func_02098d44(&mScene->mCharacter, 0);
                break;
            case 1:
                func_02098d44(&mScene->mCharacter, 1);
                break;
            case 2:
                func_02098d44(&mScene->mCharacter, 2);
                break;
            case 3:
                func_02098d44(&mScene->mCharacter, 3);
                break;
            case 4:
                func_02098d44(&mScene->mCharacter, 4);
                break;
            }
            for (i = 0; i < 4; i++) {
                if (i == choice) {
                    Sprite_SetSequence(&mWindows[0].mSprites[i].mSprite, (u16)(data_ov076_02158df4[i] + 1), TRUE);
                } else {
                    Sprite_SetSequence(&mWindows[0].mSprites[i].mSprite, data_ov076_02158df4[i], TRUE);
                }
            }
            mWindows[0].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindows[0], 0x24);
        }
        if (READ(mWindows[0].mResult) != -1 && READ(mWindows[0].mResult) == 4) {
            func_02098d44(&mScene->mCharacter, 4);
            mWindows[0].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindows[0], 0x24);
        }
        if (mWindows[0].mTransitionDone) {
            func_ov076_02151f0c(1);
        }
        return;
    }
    case 14:
        func_020341d4(&mWindows[1]);
        if (mWindows[1].mUnknownAF88) {
            func_ov076_02151f0c(15);
        }
        return;
    case 15: {
        func_020341d4(&mWindows[1]);
        int i;
        int choice;
        if (READ(mWindows[1].mUnknownAF7C) != -1 && (choice = READ(mWindows[1].mUnknownAF7C)) < 12) {
            if (choice >= 0 && choice < 12) {
                func_02098c40(&mScene->mCharacter, 0x28, choice);
                func_02091a88(&mScene->mCharacter);
                func_ov076_0214b694(mScene);
            }
            for (i = 0; i < 12; i++) {
                if (i == choice) {
                    Sprite_SetSequence(&mWindows[1].mSprites[i].mSprite, (u16)(data_ov076_02158df4[i + 4] + 1), TRUE);
                } else {
                    Sprite_SetSequence(&mWindows[1].mSprites[i].mSprite, data_ov076_02158df4[i + 4], TRUE);
                }
            }
            mWindows[1].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindows[1], 0x24);
            for (int i = 0; i < 9; i++) {
                mScene->mTextBoxes[i].mSprite.mVisible = TRUE;
            }
        }
        if (READ(mWindows[1].mResult) != -1 && READ(mWindows[1].mResult) == 12) {
            mWindows[1].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindows[1], 0x24);
            for (int i = 0; i < 9; i++) {
                mScene->mTextBoxes[i].mSprite.mVisible = TRUE;
            }
        }
        if (mWindows[1].mTransitionDone) {
            func_ov076_02151f0c(1);
        }
        return;
    }
    case 10: {
        func_020341d4(&mWindows[4]);
        if (READ(mWindows[4].mUnknownAF7C) != -1) {
            int i;
            int choice = READ(mWindows[4].mUnknownAF7C);
            switch (choice) {
            case 0:
                mScene->mCharacter.mUnknown04A[0x24E] = 2;
                break;
            case 1:
                mScene->mCharacter.mUnknown04A[0x24E] = 3;
                break;
            case 2:
                mScene->mCharacter.mUnknown04A[0x24E] = 4;
                break;
            case 3:
                mScene->mCharacter.mUnknown04A[0x24E] = 5;
                break;
            case 4:
                mScene->mCharacter.mUnknown04A[0x24E] = 0;
                break;
            }
            for (i = 0; i < 5; i++) {
                if (i == choice) {
                    Sprite_SetSequence(&mWindows[4].mButtons[i].mSprite, (u16)(data_ov076_02158df4[i + 24] + 1), TRUE);
                } else {
                    Sprite_SetSequence(&mWindows[4].mButtons[i].mSprite, data_ov076_02158df4[i + 24], TRUE);
                }
            }
            mWindows[4].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindows[4], 0x24);
        }
        if (mWindows[4].mTransitionDone) {
            func_ov076_02151f0c(1);
        }
        return;
    }
    case 16: {
        func_020341d4(&mWindows[5]);
        int i;
        int choice;
        if (READ(mWindows[5].mUnknownAF7C) != -1 && (choice = READ(mWindows[5].mUnknownAF7C)) < 2) {
            switch (choice) {
            case 0:
                func_02098cd0(&mScene->mCharacter, 0);
                func_ov076_0214b694(mScene);
                break;
            case 1:
                func_02098cd0(&mScene->mCharacter, 1);
                func_ov076_0214b694(mScene);
                break;
            }
            for (i = 0; i < 2; i++) {
                if (i == choice) {
                    Sprite_SetSequence(&mWindows[5].mSprites[i].mSprite, (u16)(data_ov076_02158df4[i + 16] + 1), TRUE);
                } else {
                    Sprite_SetSequence(&mWindows[5].mSprites[i].mSprite, data_ov076_02158df4[i + 16], TRUE);
                }
            }
            mWindows[5].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindows[5], 0x24);
        }
        if (READ(mWindows[5].mResult) != -1 && READ(mWindows[5].mResult) == 2) {
            mWindows[5].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindows[5], 0x24);
        }
        if (mWindows[5].mTransitionDone) {
            func_ov076_02151f0c(1);
        }
        return;
    }
    }
}

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

void MiiEditPage6::Draw() {
    func_020345d8(&mWindow);
}

void MiiEditPage6::Exit() {
    func_020346cc(&mWindow);
}

void MiiEditPage5::Draw() {
    func_020345d8(&mWindow);
}

void MiiEditPage5::Exit() {
    func_020346cc(&mWindow);
}

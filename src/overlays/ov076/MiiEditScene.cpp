#include "MiiEditScene.h"

#include "GameData.h"
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

// --- Of the main program, for the scene's Init and Update, and the functions after Exit (stand-ins) ---
struct PadInput {
    /* 0x0 */ u32 mTrigger;
};
struct SceneManager;
struct SceneMemory {
    /* 0x00 */ u8 mUnknown00[0x20];
    /* 0x20 */ u8 mCharacters[12]; // The first is the character the scene edits
};
struct Light;
extern "C" {
extern PadInput data_0211f9d4;
extern SceneManager gSceneManager;
extern SceneMemory data_0213ba74;
extern Light data_021227cc;
extern Light data_021227d8;
void func_0203c02c(SceneManager *manager, int sceneId); // Asks for the next scene
BOOL func_02023fa4(ScreenControl *screens, int planes);
void func_0202aa28(Light *light, Camera *camera, VecFx32 direction);
void func_02091578(SceneCharacter *character, u16 angle);
void func_02090a58(SceneCharacter *character, const char *phoneme, int face); // Lip sync
void func_020910a8(SceneCharacter *character);
void func_0207b138(ParticleSystem *system);
void func_0202c264(GameData *game, int index, CharacterRecord *record); // Reads a saved character
void func_0202c28c(GameData *game, int index, CharacterRecord *record); // Writes one
BOOL func_0209d900(CharacterRecord *record, SceneCharacter *character);
void func_02031e88(SpriteButton *button, BOOL a);
void func_0207b3dc(ParticleSystem *system);
ParticleEmitter *func_0207b2fc(ParticleSystem *system, int resource, VecFx32 *position);
void CharacterModel_GetJointMatrices(CharacterModel *model, int joint, MtxFx43 *matrix, MtxFx33 *rotation);
void *func_0202ad04(void *resources, int resourceId, const char *archiveName, int unk);
void func_0202aecc(void *resources, int resourceId);
void func_02029660(void *resourceSet, u32 resType, void *archive);
void ModelResources_SetModel(void *resources, int resourceId, const char *modelPath);
int MiiData_GetSex(CharacterRecord *character);
void func_02091048(SceneCharacter *character, const char *archiveName);
void func_0208ef5c(CharacterModel *model, int resourceId, int a);
void Character_SetAnimation(SceneCharacter *character, u32 resType, int id); // Starts an animation
void Character_SetAngle(SceneCharacter *character, u16 angle);
void func_02091a88(SceneCharacter *character);
void func_02091a24(SceneCharacter *character);
void func_02091a6c(SceneCharacter *character);
void func_0209e01c(CharacterRecord *character);
s8 func_02098f2c(CharacterRecord *character); // Its personality
extern const int data_020fba28[];
void func_0209930c(CharacterRecord *character, int a, int b, s64 c);
void func_0209b1d8(CharacterRecord *character);
void func_02029f10(CharacterModel *model, GXRgb color);
void func_02029f20(CharacterModel *model, GXRgb color);
void func_02029f30(CharacterModel *model, GXRgb color);
void func_02029f40(CharacterModel *model, GXRgb color);
void func_02092178(wchar_t *name);
}

// The lights' directions
static const VecFx32 sLightDirection1 = { 0, -4000, 1500 };
static const VecFx32 sLightDirection0 = { 1600, -600, -4000 };

void MiiEditScene::Update() {
    if (mUnknown1B32C) {
        mUnknown1B32C = FALSE;
        for (u16 i = 0; i < 9; i++) {
            mTextBoxes[i].Redraw();
            mTextBoxes[i].mSprite.mVisible = TRUE;
        }
    }
    mTtsPlayer.Update();
    switch (mPage) {
    case 0:
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        if (data_0211f9d4.mTrigger & (PAD_BUTTON_R | PAD_BUTTON_L)) {
            if (mUnknown5828) {
                mUnknown5828 = FALSE;
            } else {
                gSoundManager.StartSeqArc(1, 0xE, 0x10);
                mUnknown5828 = TRUE;
            }
        }
        break;
    }
    if (mUnknown5828) {
        func_02091578(&mCharacter, 0xFE00);
        if (mCharacter.mModel.mUnknown526 == 0x4000) {
            mUnknown5828 = FALSE;
        }
    }
    func_02090a58(&mCharacter, mTtsPlayer.GetPhonemeName(), 1);
    func_020910a8(&mCharacter);
    func_0207b138(mParticles);
    UpdateCamera();
    func_0202aa28(&data_021227cc, mCamera, sLightDirection0);
    func_0202aa28(&data_021227d8, mCamera, sLightDirection1);
    mUnknown5904 += 0x4CC;
    func_02028108(&data_02121834, 4, mUnknown5904 >> FX32_SHIFT);
    switch (mPage) {
    case 0:
        mPage0.func_ov076_0214c2e4();
        break;
    case 1:
        mFacePage.func_ov076_0214cc34();
        break;
    case 2:
        mProfilePage.func_ov076_021508ac();
        break;
    case 3:
        mVoicePage.func_ov076_02155094();
        break;
    case 4:
        mCharacterPage.func_ov076_02157e3c();
        break;
    case 6:
        mPage6.func_ov076_0215821c();
        break;
    case 5:
        mPage5.func_ov076_0215852c();
        break;
    }
    if (mPage == 0) return;
    if (mPage == 6) return;
    if (mPage == 5) return;
    if (mPage == 2) {
        if (mProfilePage.mState == 10) return;
        if (mProfilePage.mState == 11) return;
        if (mProfilePage.mState == 12) return;
        if (mProfilePage.mState == 13) return;
        if (mProfilePage.mState == 14) return;
        if (mProfilePage.mState == 15) return;
        if (mProfilePage.mState == 16) return;
    }
    if (func_02023fa4(&data_0211d604, 0x3F00)) {
        return;
    }
    for (u16 i = 0; i < 6; i++) {
        if (!func_0203133c(&mButtons[i])) {
            continue;
        }
        switch (i) {
        case 0:
            func_ov076_0214b1ec(1);
            break;
        case 1:
            func_ov076_0214b1ec(2);
            break;
        case 2:
            func_ov076_0214b1ec(3);
            break;
        case 3:
            func_ov076_0214b1ec(4);
            break;
        case 4:
            func_ov076_0214b1ec(6);
            break;
        case 5:
            if (mSceneId == 0x42) {
                CharacterRecord record;
                func_0202c264(&gGameData, (u16)mUnknown5824, &record);
                if (!func_0209d900(&record, &mCharacter)) {
                    func_0203c02c(&gSceneManager, 0x1A);
                } else {
                    func_ov076_0214b1ec(5);
                }
            } else {
                func_ov076_0214b1ec(5);
            }
            break;
        }
        if (i <= 3) {
            for (int j = 0; j <= 3; j++) {
                if (i != j) {
                    func_02031e28(&mButtons[j], FALSE);
                }
            }
        }
    }
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

void MiiEditScene::func_ov076_0214b1ec(int page) {
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
    mUnknown58E8[page] = TRUE;
    switch (page) {
    case 0:
        NNS_G2dCharCanvasClear(&mText1B94C.mCanvas, 0);
        mText1B94C.mMessage = 0x1A;
        mText1B94C.DrawAll();
        mText1B94C.mSprite.mVisible = FALSE;
        mPage0.func_ov076_0214bf4c(this);
        break;
    case 1:
        NNS_G2dCharCanvasClear(&mText1B94C.mCanvas, 0);
        mText1B94C.mMessage = 0x1C;
        mText1B94C.DrawAll();
        mText1B94C.mSprite.mVisible = TRUE;
        func_ov076_0214ba88(FALSE);
        mFacePage.func_ov076_0214c6d0(this);
        func_02031e28(&mButtons[page - 1], TRUE);
        Sprite_SetFrame(&mButtons[page - 1], 1);
        break;
    case 2:
        if (mSprite582C.mPlace == 1) {
            mUnknown58DC = FALSE;
        }
        NNS_G2dCharCanvasClear(&mText1B94C.mCanvas, 0);
        mText1B94C.mMessage = 0x1D;
        mText1B94C.DrawAll();
        mText1B94C.mSprite.mVisible = TRUE;
        mProfilePage.func_ov076_02150874(this);
        func_02031e28(&mButtons[page - 1], TRUE);
        Sprite_SetFrame(&mButtons[page - 1], 1);
        break;
    case 3:
        if (mSprite582C.mPlace == 2) {
            mUnknown58DC = FALSE;
        }
        NNS_G2dCharCanvasClear(&mText1B94C.mCanvas, 0);
        mText1B94C.mMessage = 0x21;
        mText1B94C.DrawAll();
        mText1B94C.mSprite.mVisible = TRUE;
        mVoicePage.func_ov076_02154690(this);
        func_02031e28(&mButtons[page - 1], TRUE);
        Sprite_SetFrame(&mButtons[page - 1], 1);
        break;
    case 4:
        if (mSprite582C.mPlace == 3) {
            mUnknown58DC = FALSE;
        }
        NNS_G2dCharCanvasClear(&mText1B94C.mCanvas, 0);
        mText1B94C.mMessage = 0x22;
        mText1B94C.DrawAll();
        mText1B94C.mSprite.mVisible = TRUE;
        mCharacterPage.func_ov076_02157c04(this);
        func_02031e28(&mButtons[page - 1], TRUE);
        Sprite_SetFrame(&mButtons[page - 1], 1);
        break;
    case 6:
        mPage6.func_ov076_02158048(this, mPage);
        break;
    case 5:
        mPage5.func_ov076_0215835c(this, mPage);
        break;
    }
    mPage = page;
    mUnknown00154 = 0;
}

void MiiEditScene::func_ov076_0214b5dc(int resource, fx32 offsetY) {
    func_0207b3dc(mParticles);
    MtxFx43 matrix;
    MtxFx33 rotation;
    CharacterModel_GetJointMatrices(&mCharacter.mModel, 0, &matrix, &rotation);
    mUnknown5914.x = matrix.m[3][0];
    mUnknown5914.y = matrix.m[3][1];
    mUnknown5914.z = matrix.m[3][2];
    VecFx32 position = mUnknown5914;
    position.y += offsetY;
    mUnknown590C = func_0207b2fc(mParticles, resource, &position);
}

// Reads the saved character into mCharacter, and sets it up
void MiiEditScene::func_ov076_0214b670(int index) {
    func_0202c264(&gGameData, index, &mCharacter);
    func_ov076_0214b694();
}

void MiiEditScene::func_ov076_0214b694() {
    if (mCharacter.mUnknown054Bits11 <= 1) {
        switch (MiiData_GetSex(&mCharacter)) {
        case 0:
            func_0202aecc(data_021223e4, 0);
            ArchiveManager_Unmount(&gArchiveManager, "B0I");
            func_02091048(&mCharacter, "B0I");
            ArchiveManager_MountCompressed(&gArchiveManager, "B0I", mCharacter.mModel.mArchivePath, 1);
            ModelResources_SetModel(data_021223e4, 0, mCharacter.mModel.mUnknown4E8);
            func_0208ef5c(&mCharacter.mModel, 0, 0);
            break;
        case 1:
            func_0202aecc(data_021223e4, 0);
            ArchiveManager_Unmount(&gArchiveManager, "B0I");
            func_02091048(&mCharacter, "B0I");
            ArchiveManager_MountCompressed(&gArchiveManager, "B0I", mCharacter.mModel.mArchivePath, 1);
            ModelResources_SetModel(data_021223e4, 0, mCharacter.mModel.mUnknown4E8);
            func_0208ef5c(&mCharacter.mModel, 0, 0);
            break;
        }
    } else {
        func_0202aecc(data_021223e4, 0);
        ArchiveManager_Unmount(&gArchiveManager, "B0I");
        func_02091048(&mCharacter, "B0I");
        ArchiveManager_MountCompressed(&gArchiveManager, "B0I", mCharacter.mModel.mArchivePath, 1);
        ModelResources_SetModel(data_021223e4, 0, mCharacter.mModel.mUnknown4E8);
        func_0208ef5c(&mCharacter.mModel, 0, 0);
    }
    void *animations = func_0202ad04(data_021223e4, 0x1F, "BIC", 0);
    func_02029660((void *)mCharacter.mModel.mUnknown04, RESOURCE_BCA0, animations);
    Character_SetAnimation(&mCharacter, RESOURCE_BCA0, 0);
    Character_SetAngle(&mCharacter, 0x4000);
    func_0208fefc(&mCharacter);
    func_02091a88(&mCharacter);
    func_02091a24(&mCharacter);
    func_02091a6c(&mCharacter);
}

static inline void SetFlag(u32 *flags, u8 flag) {
    flags[flag >> 5] |= 1 << (flag & 0x1F);
}

static inline void ClearFlag(u32 *flags, u8 flag) {
    flags[flag >> 5] &= ~(1 << (flag & 0x1F));
}

void MiiEditScene::func_ov076_0214b890() {
    if (mCharacter.mUnknown050Bit0) {
        mCharacter.mUnknown050Bit0 = 0;
        func_0209e01c(&mCharacter);
    }
    int kind = data_020fba28[func_02098f2c(&mCharacter)];
    int style = 8;
    switch (kind) {
    case 0:
        style = MiiData_GetSex(&mCharacter) == 0 ? 6 : 7;
        break;
    case 1:
        style = MiiData_GetSex(&mCharacter) == 0 ? 2 : 3;
        break;
    case 2:
        style = MiiData_GetSex(&mCharacter) == 0 ? 4 : 5;
        break;
    case 3:
        if (MiiData_GetSex(&mCharacter) == 0) {
            style = 0;
        } else {
            style = 1;
        }
        break;
    }
    for (int i = 0; i < 8; i++) {
        if (i == style) {
            SetFlag(mCharacter.mUnknown210, i);
        } else {
            ClearFlag(mCharacter.mUnknown210, i);
        }
    }
    if (mCharacter.mInterior <= 7) {
        mCharacter.mInterior = (u8)style;
    }
    switch (mSceneId) {
    case 0x6F:
        func_0209930c(&mCharacter, 0x13, 0, 0x7FFFFFFFFFFFFFFF);
        mCharacter.mUnknown23A = 1;
        mCharacter.mUnknown227 = 0;
        mCharacter.mUnknown226 = 0;
        mCharacter.mUnknown238 = 0;
        mCharacter.mUnknown230 = 0x7FFFFFFFFFFFFFFF;
        break;
    case 0x43:
    case 0x44:
        func_0209930c(&mCharacter, 0x13, 0, 0x7FFFFFFFFFFFFFFF);
        func_0209b1d8(&mCharacter);
        mCharacter.mUnknown230 = 0x7FFFFFFFFFFFFFFF;
        break;
    }
    func_0202c28c(&gGameData, mUnknown5824, &mCharacter);
}

void MiiEditScene::func_ov076_0214ba88(BOOL a) {
    if (a) {
        mCharacter.mUnknown02D4 = 0;
        mCharacter.mUnknown02D6 = 0;
        func_02029f10(&mCharacter.mModel, 0);
        func_02029f20(&mCharacter.mModel, 0);
        func_02029f30(&mCharacter.mModel, 0);
        func_02029f40(&mCharacter.mModel, 0);
    } else {
        mCharacter.mUnknown02D4 = GX_RGB(31, 31, 31);
        mCharacter.mUnknown02D6 = GX_RGB(8, 8, 8);
        func_02029f10(&mCharacter.mModel, GX_RGB(31, 31, 31));
        func_02029f20(&mCharacter.mModel, GX_RGB(8, 8, 8));
    }
}

void MiiEditScene::func_ov076_0214bb24() {
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
    data_0211d604.mPlanes = 0x1317;
    data_0211d604.mPlanesChanged = TRUE;
}

// --- Page 0 ---

extern "C" {
void func_020341d4(MessageWindow *window);
void func_020349cc(MessageWindow *window, int flags);
void func_02098cd0(CharacterRecord *character, int sex);
void func_0208ff44(SceneCharacter *character, int a, int b);
void func_02098d68(CharacterRecord *character, int index, s16 value);
extern const s16 data_020fbaac[];
void func_0209859c(CharacterRecord *character);
void func_0209cbac(CharacterRecord *character, u8 *a, u8 *b);
void func_02098c40(CharacterRecord *character, int kind, int value);
void func_02091a14(SceneCharacter *character, BOOL visible);
void MiiData_GetName(CharacterRecord *character, wchar_t *name);
void MiiData_SetName(CharacterRecord *character, const wchar_t *name);
void MiiData_GetCreatorName(CharacterRecord *character, wchar_t *name);
void MiiData_SetCreatorName(CharacterRecord *character, const wchar_t *name);
}

void MiiEditPage0::func_ov076_0214c2e4() {
    switch (mState) {
    case 0:
        func_020341d4(&mWindows[0]);
        // The original reads mResult again after this test, which only a volatile read gives here (making the
        // field volatile in MessageWindow.h breaks PreStartScene::Ask)
        if (*(volatile int *)&mWindows[0].mResult != -1) {
            int result = mWindows[0].mResult;
            if (result == 2) {
                func_0203c02c(&gSceneManager, 0x5E);
            } else {
                mWindows[0].mUnknownSetBeforeClose = 15;
                func_020349cc(&mWindows[0], 0x24);
                func_02098cd0(&mScene->mCharacter, result);
                switch (MiiData_GetSex(&mScene->mCharacter)) {
                case 0:
                    func_0208ff44(&mScene->mCharacter, 2, 1);
                    for (int i = 0; i < 6; i++) {
                        func_02098d68(&mScene->mCharacter, i, data_020fbaac[i + 12]);
                    }
                    break;
                case 1:
                    func_0208ff44(&mScene->mCharacter, 5, 1);
                    for (int i = 0; i < 6; i++) {
                        func_02098d68(&mScene->mCharacter, i, data_020fbaac[i + 18]);
                    }
                    mScene->mUnknown580C = TRUE;
                    break;
                }
                func_0209859c(&mScene->mCharacter);
                u8 a;
                u8 b;
                func_0209cbac(&mScene->mCharacter, &a, &b);
                mScene->mCharacter.mUnknown290 = a;
                mScene->mCharacter.mUnknown291 = b;
                func_02098c40(&mScene->mCharacter, 0x28, 0);
                func_02098c40(&mScene->mCharacter, 0x24, 0);
                MiiData_SetName(&mScene->mCharacter, L"");
                mScene->func_ov076_0214b694();
                func_02091a14(&mScene->mCharacter, FALSE);
                mScene->func_ov076_0214ba88(TRUE);
                if (mScene->mUnknown5824 != 0) {
                    wchar_t name[11];
                    SavedCharacter first;
                    MI_CpuClear16(name, sizeof(name));
                    func_0202c264(&gGameData, 0, &first);
                    MiiData_GetCreatorName(&first, name);
                    if (mScene->func_ov076_0214bf20(name) == 0) {
                        MI_CpuClear16(name, sizeof(name));
                        MiiData_GetName(&first, name);
                    }
                    MiiData_SetCreatorName(&mScene->mCharacter, name);
                } else {
                    mScene->mCharacter.mUnknown298 = 1;
                }
            }
        }
        if (!mWindows[0].mTransitionDone) {
            return;
        }
        func_02091a14(&mScene->mCharacter, TRUE);
        mScene->func_ov076_0214b1ec(1);
        break;
    case 1:
        func_020341d4(&mWindows[1]);
        if (mWindows[1].mResult != -1) {
            mWindows[1].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindows[1], 0x24);
            switch (mWindows[1].mResult) {
            case 0:
                mNextPage = 1;
                break;
            case 1:
                mNextPage = 2;
                break;
            }
        }
        if (!mWindows[1].mTransitionDone) {
            return;
        }
        func_02091a14(&mScene->mCharacter, TRUE);
        mScene->func_ov076_0214b1ec(mNextPage);
        break;
    }
}

void MiiEditPage0::func_ov076_0214c624(int state) {
    switch (state) {
    case 0:
        func_02034720(&mWindows[0], 0x12, 1);
        break;
    case 1:
        func_02034720(&mWindows[1], 0x12, 1);
        break;
    }
    mState = state;
    mUnknown00004 = 0;
}

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

void MiiEditScene::func_ov076_0214bca4() {
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
    func_02027d7c(&data_02121834, "E00:/BG_down_editface.nsc", 0x200, 0, 0, 0, 0, -1);
    data_0211d604.mPlanes = 0x1217;
    data_0211d604.mPlanesChanged = TRUE;
}

void MiiEditScene::func_ov076_0214bd64() {
    for (u16 i = 0; i < 9; i++) {
        mTextBoxes[i].mSprite.mVisible = FALSE;
    }
    mUnknown58E4 = TRUE;
    for (int i = 0; i < 6; i++) {
        mButtons[i].mVisible = FALSE;
    }
    data_0211d604.mPlanes = 0x1317;
    data_0211d604.mPlanesChanged = TRUE;
}

void MiiEditScene::func_ov076_0214bdd8() {
    for (u16 i = 0; i < 9; i++) {
        mTextBoxes[i].mSprite.mVisible = FALSE;
    }
    mUnknown58E4 = TRUE;
    for (int i = 0; i < 6; i++) {
        mButtons[i].mVisible = FALSE;
    }
    func_02027d7c(&data_02121834, "E00:/BG_down_nobar.nsc", 0x200, 0, 0, 0, 0, -1);
    data_0211d604.mPlanes = 0x1217;
    data_0211d604.mPlanesChanged = TRUE;
}

// The five places of mSprite582C
static const ScreenPoint sSpriteB0Places[5] = { { 55, 182 }, { 55, 182 }, { 103, 182 }, { 151, 182 }, { 199, 182 } };

void MiiEditSpriteB0::func_ov076_0214be80(int place) {
    SetPosition(FxVec2(sSpriteB0Places[place].x << FX32_SHIFT, sSpriteB0Places[place].y << FX32_SHIFT));
    mPlace = place;
}

void MiiEditSpriteB0::func_ov076_0214bec8(int place) {
    if (place <= mPlace) {
        return;
    }
    SetPosition(FxVec2(sSpriteB0Places[place].x << FX32_SHIFT, sSpriteB0Places[place].y << FX32_SHIFT));
    mPlace = place;
}

// Returns the length of the name once func_02092178 has been through it
int MiiEditScene::func_ov076_0214bf20(const wchar_t *name) {
    wchar_t buffer[11];
    wcsncpy(buffer, name, 11);
    func_02092178(buffer);
    return wcslen(buffer);
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

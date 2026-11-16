#include "G3dTestScene.h"

// The edge marking colours: red, green and blue for polygon ids 8 to 31
static const GXRgb sEdgeColors[8] = {0, GX_RGB(31, 0, 0), GX_RGB(0, 31, 0), GX_RGB(0, 0, 31)};

// The polygon id that the Mii library draws a character's face with (NFLDrawParam's polygonID)
static inline void SetFacePolygonId(CharacterBase *character, int id) {
    *(int *)&character->mFaceDrawParam[8] = id;
}

void G3dTestScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "BIC", "Mdl/Ica/ica_mii_room_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "THM", "Mdl/Body/body000_3D_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TG3", "Scn/Scene/SceneTestG3d_LZ.bin", 0);
}

void G3dTestScene::Init() {
    func_0202b0e8(&data_02122824);
    ModelResources_SetModel(data_021223e4, 0, "THM:/body.imd");
    func_0202ad04(data_021223e4, 0, "BIC", 0);
    ModelResources_SetModel(data_021223e4, 1, "TG3:/Glass6.imd");
    NNS_GfdDumpFrmTexVramManager();
    func_0205ff40();
    G3X_SetClearColor(GX_RGB(21, 21, 21), 31, 0x7FFF, 63, FALSE);
    data_0211d604.Show(1);
    G3X_EdgeMarking(TRUE);
    G3X_SetEdgeColorTable(sEdgeColors);
    mCamera = &data_02122764;
    Light *light = &data_021227cc[0];
    u16 color = light->mColor;
    light->mDirection.x = 0xFFF;
    light->mEnabled = TRUE;
    fx16 x = light->mDirection.x;
    light->mDirection.y = 0;
    light->mDirection.z = 0;
    light->mDirection.x = x;
    light->mColor = color;
    func_02090fdc(&mCharacter, 0, 0, 0);
    Character_SetAnimation(&mCharacter, RESOURCE_BCA0, 0);
    mCharacter.mModel.mScale.x = 0x1800;
    mCharacter.mModel.mInvScale.x = FX_Inv(0x1800);
    SetFacePolygonId(&mCharacter, 10);
    NNS_G3dMdlSetMdlPolygonIDAll(mCharacter.mModel.mAnim.mResMdl, 3);
    func_02090fdc(&mSecondCharacter, 0, 0, 4);
    Character_SetAnimation(&mSecondCharacter, RESOURCE_BCA0, 1);
    mSecondCharacter.mModel.mPos.y = -0x2E00;
    SetFacePolygonId(&mSecondCharacter, 11);
    mSecondCharacter.mModel.mShadowAlpha = 0;
    mGlasses.Load(1, 1);
    NNSG3dResMdl *glasses = mGlasses.mResMdl;
    NNS_G3dMdlSetMdlPolygonID(glasses, 0, 9);
    NNS_G3dMdlSetMdlAlpha(glasses, 0, 30);
    mBody.Load(0, 0);
    mBody.mPos.x = -0x2000;
    NNS_G3dMdlSetMdlPolygonIDAll(mBody.mAnim.mResMdl, 4);
}

void G3dTestScene::Update() {
    if (data_0211f9d4.mHeld & PAD_BUTTON_A) {
        return;
    }
    if (data_0211f9d4.mHeld & PAD_BUTTON_B) {
        if (data_0211f9d4.mHeld & PAD_KEY_LEFT) {
            mCharacter.mModel.mAnim.mRotZ += 0x200;
        } else if (data_0211f9d4.mHeld & PAD_KEY_RIGHT) {
            mCharacter.mModel.mAnim.mRotZ += 0xFE00; // Written so: "-= 0x200" compiles to a sub
        } else if (data_0211f9d4.mHeld & PAD_KEY_UP) {
            mCharacter.mModel.mAnim.mRotY += 0x200;
        } else if (data_0211f9d4.mHeld & PAD_KEY_DOWN) {
            mCharacter.mModel.mAnim.mRotY += 0xFE00; // Written so: "-= 0x200" compiles to a sub
        }
    } else if (data_0211f9d4.mTrigger & PAD_BUTTON_X) {
        u16 anim = MATH_Rand16(&gRandom, 3);
        if (data_0211f9d4.mHeld & PAD_KEY_LEFT) {
            anim = 0;
        } else if (data_0211f9d4.mHeld & PAD_KEY_RIGHT) {
            anim = 1;
        } else if (data_0211f9d4.mHeld & PAD_KEY_UP) {
            anim = 2;
        } else if (data_0211f9d4.mHeld & PAD_KEY_DOWN) {
            anim = 1;
        }
        func_020919e8(&mCharacter, RESOURCE_BCA0, anim);
    } else if (data_0211f9d4.mTrigger & PAD_BUTTON_Y) {
        func_0208ff44(&mCharacter, MATH_Rand16(&gRandom, 6), 1);
        NNS_GfdDumpFrmTexVramManager();
        func_0205ff40();
    } else if (data_0211f9d4.mHeld & PAD_BUTTON_R) {
        if (data_0211f9d4.mHeld & PAD_KEY_RIGHT) {
            mCamera->mAngleY += 0x200;
        } else if (data_0211f9d4.mHeld & PAD_KEY_LEFT) {
            mCamera->mAngleY -= 0x200;
        }
        if (data_0211f9d4.mHeld & PAD_KEY_DOWN) {
            mCamera->mAngleX += 0x200;
        } else if (data_0211f9d4.mHeld & PAD_KEY_UP) {
            mCamera->mAngleX -= 0x200;
        }
    }
}

void G3dTestScene::Draw() {
    func_02091968(&mCharacter);
    func_02091968(&mSecondCharacter);
    MtxFx43 matrix;
    MtxFx33 rotation;
    mCharacter.mModel.GetJointMatrices(CharacterModel::JOINT_L_HAND, &matrix, &rotation);
    MI_Copy36B(&rotation, &mGlasses.mRotation);
    mGlasses.mPosition.x = matrix._30;
    mGlasses.mPosition.y = matrix._31;
    mGlasses.mPosition.z = matrix._32;
    mGlasses.Draw();
    mBody.Draw();
}

void G3dTestScene::Exit() {
    func_0202aecc(data_021223e4, 0);
    func_0202aecc(data_021223e4, 1);
    ArchiveManager_Unmount(&gArchiveManager, "TG3");
    ArchiveManager_Unmount(&gArchiveManager, "THM");
    ArchiveManager_Unmount(&gArchiveManager, "BIC");
}

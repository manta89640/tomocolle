#include "DrawManTestScene.h"
#include "MainProgram3d.h"

#include <nnsys/g3d/util.h>

void DrawManTestScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "DMN", "Scn/Scene/SceneTestDrawMan_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "CA3", "Scn/Scene/SceneTestG3d_3_LZ.bin", 0);
}

void DrawManTestScene::Init() {
    func_0202b0e8(&data_02122824);
    ModelResources_SetModel(data_021223e4, 0, "DMN:/HUMAN00_.imd");
    ModelResources_SetModel(data_021223e4, 0, "DMN:/HUMAN00.itx");
    gScreenControl.Show(1);
    G3X_SetClearColor(GX_RGB(21, 21, 21), 31, 0x7FFF, 63, FALSE);
    mCamera = &data_02122764;
    mCamera->mPosition.z = 0x12000;
    Light *light = &data_021227cc[0];
    light->mDirection.x = 0xFFF;
    light->mEnabled = TRUE;
    u16 color = light->mColor;
    fx16 x = light->mDirection.x;
    light->mDirection.y = 0;
    light->mDirection.z = 0;
    light->mDirection.x = x;
    light->mColor = color;
    mModel.Load(0, 0);
    func_02029c20(&mModel, RESOURCE_BTX0, 0, 1);
    NNS_GfdDumpFrmTexVramManager();
    NNS_GfdDumpFrmPlttVramManager();
    NNS_G3dRenderObjSetCallBack(&mModel.mRenderObj, HeadCallback, NULL, NNS_G3D_SBC_NODEDESC,
                                NNS_G3D_SBC_CALLBACK_TIMING_C);
    NNS_G3dRenderObjSetUserPtr(&mModel.mRenderObj, this);
    mCharacter.InitFace(0);
}

void DrawManTestScene::Update() {
    if (data_0211f9d4.mHeld & PAD_BUTTON_A) {
        if (data_0211f9d4.mHeld & PAD_KEY_RIGHT) {
            mModel.mPosition.x += 0x100;
        } else if (data_0211f9d4.mHeld & PAD_KEY_LEFT) {
            mModel.mPosition.x -= 0x100;
        }
    } else if (data_0211f9d4.mTrigger & PAD_BUTTON_B) {
        NNS_GfdDumpFrmTexVramManager();
        NNS_GfdDumpFrmPlttVramManager();
        NNSG3dResMdl *mdl = mModel.mResMdl;
        NNS_G3D_DEFRESNAME(name, "human01_base_pl");
        NNS_G3dReleaseMdlPlttEx(mdl, &name.resName);
        NNS_G3dForceBindMdlPltt(mdl, mModel.mResTex, 1, 7);
    } else if (data_0211f9d4.mTrigger & PAD_BUTTON_Y) {
        NNS_GfdDumpFrmTexVramManager();
        NNS_GfdDumpFrmPlttVramManager();
        void *resourceSet = (void *)mModel.mUnknown04;
        NNS_G3dReleaseMdlSet(NNS_G3dGetMdlSet(func_020295a4(resourceSet, RESOURCE_BMD0)));
        func_02029758(resourceSet, RESOURCE_BTX0);
        NNS_GfdDumpFrmTexVramManager();
        NNS_GfdDumpFrmPlttVramManager();
        func_0202b0e8(&data_02122824);
        ModelResources_SetModel(data_021223e4, 0, "DMN:/HUMAN00.itx");
        NNS_GfdDumpFrmTexVramManager();
        NNS_GfdDumpFrmPlttVramManager();
    } else {
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

void DrawManTestScene::Draw() {
    mModel.Draw();
    mCharacter.SetHeadMatrices(&mHeadMatrix, &mHeadRotation);
    mCharacter.Draw();
}

void DrawManTestScene::Exit() {
    func_0202aecc(data_021223e4, 0);
    ArchiveManager_Unmount(&gArchiveManager, "DMN");
    ArchiveManager_Unmount(&gArchiveManager, "CA3");
}

// The model's render callback: as the head joint's node is drawn, keeps its matrix and stops being called
void DrawManTestScene::HeadCallback(NNSG3dRS *rs) {
    NNSG3dRenderObj *obj = NNS_G3dRSGetRenderObj(rs);
    DrawManTestScene *scene = (DrawManTestScene *)obj->ptrUser;
    int head;
    NNS_G3D_GET_JNTID(NNS_G3dRenderObjGetResMdl(obj), &head, "head_h2");
    if (head == NNS_G3dRSGetCurrentNodeDescID(rs)) {
        NNS_G3dGetCurrentMtx(&scene->mHeadMatrix, &scene->mHeadRotation);
        NNS_G3dRSResetCallBack(rs, NNS_G3D_SBC_NODEDESC);
    }
}

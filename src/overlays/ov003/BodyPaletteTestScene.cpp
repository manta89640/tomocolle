#include <nitro.h>
#include <nnsys.h>

#include "../../scene/MainProgram3d.h"

#include "BodyPaletteTestScene.h"

// Overlay 3: a test of the body model's palette (scene 3). Two copies of Mdl/Body/body000 stand side by side, one
// mounted as TG3 and one as TG4. A puts the palette file TG3:/body_c1.ntfp, its colour 1 made red, in the first
// one's palette; X makes colour 1 of its own palette cyan; B replaces its model with body001's; Y looks up each
// palette that its materials use, and does nothing with them. The pad turns the camera.

void BodyPaletteTestScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "TG3", "Mdl/Body/body000_3D_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TG4", "Mdl/Body/body000_3D_LZ.bin", 0);
}

// Not matched yet (79%): opt_propagation off gives light 0's block the original's registers (the light in ip, the
// colour in r3, 0 in r1 before the x reload), and the path through a local the first call's early `ldr r2`. What is
// left is scheduling: the order of the light's stores after the reload, and the argument set-up of the two Load calls
// and of the second SetModel, which change with unrelated code anywhere earlier in the function.
#pragma push
#pragma opt_propagation off
void BodyPaletteTestScene::Init() {
    func_0202b0e8(&data_02122824);
    data_0211d604.Show(1);
    data_0211d604.mUnknown48 = 1;
    data_0211d604.mUnknown48Changed = TRUE;
    G3X_SetClearColor(GX_RGB(15, 15, 15), 31, 0x7FFF, 63, FALSE);
    mCamera = &data_02122764;
    Light *light = &data_021227cc[0];
    u16 color = light->mColor;
    light->mDirection.x = 0xFFF;
    light->mEnabled = TRUE;
    fx16 x = light->mDirection.x;
    light->mDirection.x = x;
    light->mColor = color;
    light->mDirection.z = 0;
    light->mDirection.y = 0;
    const char *path = "TG3:/body.imd";
    ModelResources_SetModel(data_021223e4, 0, path);
    mBody.Load(0, 0);
    ModelResources_SetModel(data_021223e4, 1, "TG4:/body.imd");
    mSecondBody.Load(1, 0);
    mSecondBody.mPosition.x = -0x2000;
    NNS_GfdDumpLnkTexVramManager();
    func_02060c1c();
}
#pragma pop

void BodyPaletteTestScene::Update() {
    if (data_0211f9d4.mTrigger & PAD_BUTTON_A) {
        GXRgb *palette = (GXRgb *)ArchiveManager_GetFileByName(&gArchiveManager, "TG3:/body_c1.ntfp");
        palette[1] = GX_RGB(31, 0, 0);
        func_02029d40(&mBody, palette, -1, -1);
    } else if (data_0211f9d4.mTrigger & PAD_BUTTON_B) {
        func_02029758(data_021223e4, RESOURCE_BMD0);
        ArchiveManager_Unmount(&gArchiveManager, "TG3");
        ArchiveManager_MountCompressed(&gArchiveManager, "TG3", "Mdl/Body/body001_3D_LZ.bin", 1);
        ModelResources_SetModel(data_021223e4, 0, "TG3:/body.imd");
        func_02029c20(&mBody, RESOURCE_BMD0, 0, 0);
        NNS_GfdDumpLnkTexVramManager();
        func_02060c1c();
    } else if (data_0211f9d4.mTrigger & PAD_BUTTON_Y) {
        NNSG3dResDict *dict;
        int i;
        NNSG3dResTex *textures = mBody.mResTex;
        NNSG3dResMat *materials = NNS_G3dGetMat(mBody.mResMdl);
        dict = (NNSG3dResDict *)((u8 *)materials + materials->ofsDictPlttToMatList);
        for (i = 0; i < dict->numEntry; i++) {
            NNS_G3dGetPlttDataByName(textures, NNS_G3dGetResNameByIdx(dict, i));
        }
    } else if (data_0211f9d4.mTrigger & PAD_BUTTON_X) {
        NNSG3dResTex *textures = mBody.mResTex;
        GXRgb *palette = (GXRgb *)((u8 *)textures + textures->plttInfo.ofsPlttData);
        palette[1] = GX_RGB(0, 31, 31);
        func_02029d40(&mBody, palette, -1, 4);
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

void BodyPaletteTestScene::Draw() {
    mBody.Draw();
    mSecondBody.Draw();
}

void BodyPaletteTestScene::Exit() {
    func_0202aecc(data_021223e4, 0);
    func_0202aecc(data_021223e4, 1);
    ArchiveManager_Unmount(&gArchiveManager, "TG3");
    ArchiveManager_Unmount(&gArchiveManager, "TG4");
}

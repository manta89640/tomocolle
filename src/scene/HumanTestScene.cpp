#include "HumanTestScene.h"

extern "C" {
void *ArchiveManager_GetFileByName(ArchiveLoader *loader, const char *path);
}

void HumanTestScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "HP0", "Scn/Scene/TestHuman00Itp_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "HM0", "Scn/Scene/TestHuman00Imd_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "BM0", "Scn/Scene/TestBody00Imd_LZ.bin", 0);
}

void HumanTestScene::Init() {
    func_020278f8(&data_021213b8, 0, 0x101);
    func_0202b0e8(&data_02122824);
    ModelResources_SetModel(data_021223e4, 0, "HP0:/H00point00.itp");
    ModelResources_SetModel(data_021223e4, 0, "HM0:/HUMAN00_.imd");
    ModelResources_SetModel(data_021223e4, 1, "BM0:/body00_.imd");
    G3X_SetClearColor(GX_RGB(1, 1, 1), 31, 0x7FFF, 63, FALSE);
    data_0211d604.Show(1);
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
    mHead.Load(0, 0);
    mBody.Load(1, 0);
    mBody.mPosition.x = -0x2000;
    mBody.mScale.x = mBody.mScale.y = mBody.mScale.z = 0x200;
    NNS_GfdDumpFrmTexVramManager();
    func_0205ff40();
    mTexturesLoaded = FALSE;
}

void HumanTestScene::Update() {
    if (data_0211f9d4.mTrigger & PAD_BUTTON_A) {
        ArchiveManager_Unmount(&gArchiveManager, "HT0");
        ArchiveManager_MountCompressed(&gArchiveManager, "HT0", "Scn/Scene/TestHuman00Tex_LZ.bin", 1);
        ModelResources_SetModel(data_021223e4, 0, "HT0:/HUMAN00.itx");
        func_02029c20(&mHead, RESOURCE_BTX0, 0, 0);
        mHead.SetAnimation(RESOURCE_BTP0, 1);
        ArchiveManager_Unmount(&gArchiveManager, "BT0");
        ArchiveManager_MountCompressed(&gArchiveManager, "BT0", "Scn/Scene/TestBody00Tex_LZ.bin", 1);
        ModelResources_SetModel(data_021223e4, 1, "BT0:/body00.itx");
        func_02029c20(&mBody, RESOURCE_BTX0, 1, 0);
        mTexturesLoaded = TRUE;
    } else if (mTexturesLoaded && (data_0211f9d4.mTrigger & PAD_BUTTON_B)) {
        u32 address = NNS_GfdGetPlttKeyAddr(mBody.mResTex->plttInfo.vramKey);
        GXRgb *palette = (GXRgb *)ArchiveManager_GetFileByName(&gArchiveManager, "BT0:/style01.ntfp");
        palette[0] = GX_RGB(17, 8, 4);
        GX_BeginLoadTexPltt();
        GX_LoadTexPltt(palette, address, 0x20);
        GX_EndLoadTexPltt();
    } else if (!(data_0211f9d4.mTrigger & PAD_BUTTON_Y)) {
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

void HumanTestScene::Draw() {
    mHead.Draw();
    mBody.Draw();
}

void HumanTestScene::Exit() {
    func_0202aecc(data_021223e4, 0);
    func_0202aecc(data_021223e4, 1);
    ArchiveManager_Unmount(&gArchiveManager, "HP0");
    ArchiveManager_Unmount(&gArchiveManager, "HM0");
    ArchiveManager_Unmount(&gArchiveManager, "BM0");
    ArchiveManager_Unmount(&gArchiveManager, "BT0");
    ArchiveManager_Unmount(&gArchiveManager, "HT0");
}

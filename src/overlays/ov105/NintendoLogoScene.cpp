#include "NintendoLogoScene.h"
#include "MainProgram.h"

// Overlay 105: the Nintendo logo (NintendoLogoScene). One source file: the scene and its virtual table, with the
// scene's implicit destructor at the end of .text, and the file's strings after the table in .data.

void NintendoLogoScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "MNU", "Cmn/Nin/NintendoLogo_LZ.bin", 0);
    // Both screens fade in (and out) at 0x19C2 a frame: in ten frames
    func_02023c54(&data_0211d604, 0x3F3F, 0x19C2);
}

void NintendoLogoScene::Init() {
    func_020278f8(&data_021213b8, 0x7FFF, 0x101);
    func_02027d7c(&data_02121834, "MNU:/Gray_White.nsc", 2, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "MNU:/Gray_White.ncg", 2, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "MNU:/Gray_White.ncl", 2, -1, -1, -1);
    data_0211d604.Set(2);
    SetState(STATE_SHOW);
}

void NintendoLogoScene::Update() {
    switch (mState) {
    case STATE_SHOW:
        mFrame++;
        if (mFrame > 60) {
            gSceneManager.RequestScene(0x39);
        }
        break;
    }
}

void NintendoLogoScene::Draw() {
}

void NintendoLogoScene::Exit() {
    ArchiveManager_Unmount(&gArchiveManager, "MNU");
}

void NintendoLogoScene::SetState(int state) {
    mState = state;
    mUnknown14 = 0;
    mFrame = 0;
}

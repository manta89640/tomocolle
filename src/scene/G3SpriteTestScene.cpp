#include "G3SpriteTestScene.h"

// The rectangle of the texture that a sprite shows: one of four 32-pixel squares side by side
static const ScreenRect sSpriteRect = {0, 0, 0x20, 0x20};

void G3SpriteTestScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "M01", "Mdl/Body/body000_3D_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "M00", "Scn/Scene/SceneTestG3Sprite_LZ.bin", 0);
}

void G3SpriteTestScene::Init() {
    func_0202b0e8(&data_02122824);
    data_0211d604.mUnknown48 = 1;
    data_0211d604.mUnknown48Changed = TRUE;
    data_0211d604.Show(1);
    G3X_SetClearColor(GX_RGB(21, 21, 21), 0, 0x7FFF, 63, FALSE);
    ModelResources_SetModel(&data_021223e4, 0, "M01:/body.imd");
    mCamera = &data_02122764;
    func_0202a640(mCamera);
    func_0202a93c(mCamera, 1);
    data_021223e4.mUnknown424 = 0;
    mNextSprite = SPRITE_COUNT - 1;
    func_0202b4d8(&data_021232e4);
    func_0202b4dc(&data_021232e4, 0, "M00:/Face.tga", 3, 0x80, 0x80);

    for (int i = 0; i < SPRITE_COUNT; i++) {
        ScreenRect spriteRect = sSpriteRect;
        spriteRect.x = (MATH_Rand16(&gRandom, 0) & 3) * 0x20;
        func_0202b1f0(&mSprites[i], 0, &spriteRect);
        mSprites[i].mScaleY = FX32_ONE;
        mSprites[i].mScaleX = FX32_ONE;
        func_0202b4c4(&mSprites[i]);
        mSprites[i].mDepth = i;
        mSprites[i].mUnknown19 = 20;
        mSprites[i].mX = 0;
        mSprites[i].mY = 0;
        SetRandomPosition(i);
    }

    func_0208ec28(&mModel, 0, 0);
    mModel.mPos.x = FX32_ONE;
    mModel.mPos.z = FX32_ONE;
    func_0208fd90(&mCharacter, 1);
    func_0208ff68(&mCharacter, 2, 3, 3, 1);
    mCharacter.mUnknown2570 = FX32_ONE;
    mCharacter.mUnknown2568 = -FX32_ONE;
}

void G3SpriteTestScene::Update() {
    UpdateSprites();
}

void G3SpriteTestScene::Draw() {
    func_0208ed30(&mModel);
    func_02090214(&mCharacter);
    for (int i = 0; i < SPRITE_COUNT; i++) {
        func_0202b4a4(&mSprites[i]);
    }
}

void G3SpriteTestScene::Exit() {
    func_0202aecc(&data_021223e4, 0);
    func_0202ae38(&data_021223e4);
    ArchiveManager_Unmount(&gArchiveManager, "M00");
    ArchiveManager_Unmount(&gArchiveManager, "M01");
}

void G3SpriteTestScene::SetRandomPosition(int index) {
    mSprites[index].mX = (MATH_Rand16(&gRandom, 0x100) - 0x80) << 7;
    mSprites[index].mY = (MATH_Rand16(&gRandom, 0xC0) - 0x60) << 7;
}

// Each frame the next sprite, counting down, moves to a new place and gets a new size between 1 and 2
void G3SpriteTestScene::UpdateSprites() {
    for (int i = 0; i < SPRITE_COUNT; i++) {
        if (i != mNextSprite) {
            mSprites[i].mDepth++;
        } else {
            mSprites[i].mDepth = 0;
            SetRandomPosition(i);
            mSprites[i].mScaleX = mSprites[i].mScaleY = (MATH_Rand16(&gRandom, 0) & 0xF) * 0x100 + FX32_ONE;
        }
    }
    mNextSprite--;
    if (mNextSprite < 0) {
        mNextSprite = SPRITE_COUNT - 1;
    }
}

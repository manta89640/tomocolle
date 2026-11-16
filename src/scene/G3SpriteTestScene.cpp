#include "G3SpriteTestScene.h"

// The rectangle of the texture that a sprite shows: one of four 32-pixel squares side by side
static const ScreenRect sSpriteRect = {0, 0, 0x20, 0x20};

void G3SpriteTestScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "M01", "Mdl/Body/body000_3D_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "M00", "Scn/Scene/SceneTestG3Sprite_LZ.bin", 0);
}

void G3SpriteTestScene::Init() {
    VramManagers_Reset(&gVramManagerWork);
    gScreenControl.mUnknown48 = 1;
    gScreenControl.mUnknown48Changed = TRUE;
    gScreenControl.Show(1);
    G3X_SetClearColor(GX_RGB(21, 21, 21), 0, 0x7FFF, 63, FALSE);
    ModelResources_SetModel(&sModelResources, 0, "M01:/body.imd");
    mCamera = &gCamera;
    Camera_Reset(mCamera);
    Camera_SetProjection(mCamera, 1);
    sModelResources.mUnknown424 = 0;
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

    mModel.Load(0, 0);
    mModel.mPos.x = FX32_ONE;
    mModel.mPos.z = FX32_ONE;
    mCharacter.InitFace(1);
    mCharacter.SetRandomMii(2, 3, 3, 1);
    mCharacter.mFaceOffset.z = FX32_ONE;
    mCharacter.mFaceOffset.x = -FX32_ONE;
}

void G3SpriteTestScene::Update() {
    UpdateSprites();
}

void G3SpriteTestScene::Draw() {
    mModel.Draw();
    mCharacter.Draw();
    for (int i = 0; i < SPRITE_COUNT; i++) {
        func_0202b4a4(&mSprites[i]);
    }
}

void G3SpriteTestScene::Exit() {
    ModelResources_ReleaseModel(gModelResources, 0);
    ModelResources_Reset(&sModelResources);
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

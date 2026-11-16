#include "TestDrawMiiScene.h"

#include "../../scene/MainProgram3d.h"

// Overlay 5: one source file, TestDrawMiiScene with its models and its camera, the tables of the scene and of
// TestDrawMiiModel, and the implicit destructors at the end of .text. The scene is a test of a Mii drawn in a room:
// the room (SME:/room.imd) as an animated model, two more models of the same file that fade out while they stand
// between the camera and the Mii, and the Mii's face. The camera circles the room: the pad turns it (left and right)
// and raises it (up and down); with X held, right moves the point it circles to a random place, left picks random
// distances and up a random turn.

extern "C" {
void func_02029f50(AnimModel *model, int alpha); // Sets the model's alpha (0 to 31)
}

void TestDrawMiiScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "SME", "Scn/Scene/SceneTestDrawMii_LZ.bin", 0);
}

// The two models: their rotations about y, which models of SME:/room.imd they are, and where they stand
static const u16 sModelRotY[2] = {0, 0x4000};
static const int sModelIndices[2] = {1, 2};
static const VecFx32 sModelHomes[2] = {{0x5000, 0, 0}, {0, 0, -0x5000}};

void TestDrawMiiScene::Init() {
    func_0202b0e8(&data_02122824);
    data_0211d604.Show(1);
    data_0211d604.mUnknown48 = 1;
    data_0211d604.mUnknown48Changed = TRUE;
    func_020278f8(&data_021213b8, 0, 0x101);
    G3X_AntiAlias(TRUE);
    G3X_SetShading(GX_SHADING_TOON);
    G3_SwapBuffers(GX_SORTMODE_AUTO, GX_BUFFERMODE_W);
    G3X_SetClearColor(GX_RGB(21, 21, 21), 31, 0x7FFF, 63, FALSE);
    // Light 0 along x, white. NOT MATCHED (Init is 93%): the original stores x and the colour, reads both back
    // and stores them again with y and z (light in r4, x reloaded into r0). Here the compiler drops the reload of x
    // and its store; volatile reads or `#pragma opt_dead_assignments off` keep them but give other registers. The
    // same block is unmatched in three of the main program's test scenes and in overlay 3.
    Light *light = &data_021227cc[0];
    light->mEnabled = TRUE;
    light->mDirection.x = 0xFFF;
    light->mColor = 0x7FFF;
    fx16 x = light->mDirection.x;
    u16 color = light->mColor;
    light->mDirection.x = x;
    light->mDirection.y = 0;
    light->mDirection.z = 0;
    light->mColor = color;
    ModelResources_SetModel(data_021223e4, 0, "SME:/room.imd");
    mAnimModel.Load(0, 0);
    for (int i = 0; i < 2; i++) {
        TestDrawMiiModel *model = &mModels[i];
        model->Load(0, sModelIndices[i]);
        model->Init();
        VecFx32 home = sModelHomes[i];
        mModels[i].mHome.x = home.x;
        mModels[i].mHome.y = home.y;
        mModels[i].mHome.z = home.z;
        mModels[i].mHomeRotY = sModelRotY[i];
    }
    mCharacter.InitFace(1);
    mCharacter.SetRandomMii(0, 0, 2, TRUE);
    mCamera.Init();
    mCamera.Update();
}

void TestDrawMiiScene::Update() {
    if (data_0211f9d4.mHeld & PAD_BUTTON_X) {
        if (data_0211f9d4.mTrigger & PAD_KEY_RIGHT) {
            mCamera.SetRandomTarget();
        } else if (data_0211f9d4.mTrigger & PAD_KEY_LEFT) {
            mCamera.SetRandomRadius();
        } else if (data_0211f9d4.mTrigger & PAD_KEY_UP) {
            mCamera.mAngles.y += MATH_Rand16(&gRandom, 0) << FX32_SHIFT;
        }
    } else {
        if (data_0211f9d4.mHeld & PAD_KEY_RIGHT) {
            mCamera.mAngles.y += 0x100 << FX32_SHIFT;
        } else if (data_0211f9d4.mHeld & PAD_KEY_LEFT) {
            mCamera.mAngles.y -= 0x100 << FX32_SHIFT;
        }
        if (data_0211f9d4.mHeld & PAD_KEY_UP) {
            mCamera.mAngles.x += 0x30 << FX32_SHIFT;
        } else if (data_0211f9d4.mHeld & PAD_KEY_DOWN) {
            mCamera.mAngles.x -= 0x30 << FX32_SHIFT;
        }
    }
    mCamera.Update();
    UpdateModelFades();
    for (int i = 0; i < 2; i++) {
        mModels[i].Update();
    }
}

void TestDrawMiiScene::Draw() {
    for (int i = 0; i < 2; i++) {
        mModels[i].Draw();
    }
    mAnimModel.Draw();
    mCharacter.Draw();
}

void TestDrawMiiScene::Exit() {
    func_0202aecc(data_021223e4, 0);
    ArchiveManager_Unmount(&gArchiveManager, "SME");
}

// Fades out each model that is nearer the camera than the centre of the room is (on the floor: x and z), and fades
// the others back in.
void TestDrawMiiScene::UpdateModelFades() {
    VecFx32 eye = mCamera.mEye;
    fx32 eyeDistSq = FX_Mul(eye.x, eye.x) + FX_Mul(eye.z, eye.z);
    for (int i = 0; i < 2; i++) {
        VecFx32 home = mModels[i].mHome;
        fx32 dx = home.x - eye.x;
        fx32 dz = home.z - eye.z;
        if (FX_Mul(dx, dx) + FX_Mul(dz, dz) < eyeDistSq) {
            if (mModels[i].mFade != TestDrawMiiModel::FADE_OUT) {
                mModels[i].mFade = TestDrawMiiModel::FADE_OUT;
            }
        } else {
            if (mModels[i].mFade != TestDrawMiiModel::FADE_IN) {
                mModels[i].mFade = TestDrawMiiModel::FADE_IN;
            }
        }
    }
}

TestDrawMiiModel::TestDrawMiiModel() {
    mFade = FADE_IN;
    mAlpha = 31;
}

void TestDrawMiiModel::Init() {}

void TestDrawMiiModel::Update() {
    switch (mFade) {
    case FADE_IN:
        if (mAlpha < 31) {
            mAlpha += 5;
            if (mAlpha > 31) {
                mAlpha = 31;
            }
            func_02029f50(this, mAlpha);
        }
        break;
    case FADE_OUT:
        if (mAlpha > 0) {
            mAlpha -= 5;
            if (mAlpha < 0) {
                mAlpha = 0;
            }
            func_02029f50(this, mAlpha);
        }
        break;
    }
    mPosition.x = mHome.x;
    mPosition.y = mHome.y;
    mPosition.z = mHome.z;
    mRotY = mHomeRotY;
}

// The camera's angles, set together (overlay 74 has this as a member of its Camera stand-in)
static inline void SetCameraAngles(Camera *camera, u16 x, u16 y, u16 z) {
    camera->mAngleX = x;
    camera->mAngleY = y;
    camera->mAngleZ = z;
}

// The camera's first angles: pitch, yaw
static const VecFx32 sCameraAngles = {0x1200 << FX32_SHIFT, 0x1600 << FX32_SHIFT, 0};

void TestDrawMiiCamera::Init() {

    mCamera = &data_02122764;
    func_0202a640(mCamera);
    SetCameraAngles(mCamera, 0, 0, 0);
    VecFx32 angles = sCameraAngles;
    mAngles.x = angles.x;
    mAngles.y = angles.y;
    mAngles.z = angles.z;
    mTarget.x = 0;
    mTarget.y = 0x1CCC;
    mTarget.z = 0;
    mRadius.x = 0x12800;
    mRadius.y = 0x9000;
    mRadius.z = 0x12800;
}

// Each component shifted right
static inline VecFx32 Shift(const VecFx32 &v, u32 shift) {
    VecFx32 result = {v.x >> shift, v.y >> shift, v.z >> shift};
    return result;
}

void TestDrawMiiCamera::Update() {
    mEye = GetEye(mRadius, Shift(mTarget, 3));
    mCamera->mPosition = mEye;
    mCamera->mTarget = mTarget;
}

VecFx32 TestDrawMiiCamera::GetEye(VecFx32 radius, VecFx32 center) {
    u16 yaw = (u16)(mAngles.y >> FX32_SHIFT);
    u16 pitch = (u16)(mAngles.x >> FX32_SHIFT);
    VecFx32 eye;
    eye.x = center.x + FX_Mul(radius.z, FX_SinIdx(yaw));
    eye.z = center.z + FX_Mul(radius.x, FX_CosIdx(yaw));
    eye.y = FX_Mul(radius.y, FX_SinIdx(pitch));
    return eye;
}

void TestDrawMiiCamera::SetRandomRadius() {
    mRadius.x = (MATH_Rand16(&gRandom, 90) + 90) * (FX32_ONE / 10);
    mRadius.z = (MATH_Rand16(&gRandom, 90) + 90) * (FX32_ONE / 10);
}

void TestDrawMiiCamera::SetRandomTarget() {
    mTarget.x = MATH_Rand16(&gRandom, 40) * (FX32_ONE / 10);
    if (MATH_Rand16(&gRandom, 0) & 1) {
        mTarget.x = -mTarget.x;
    }
    mTarget.z = MATH_Rand16(&gRandom, 40) * (FX32_ONE / 10);
    if (MATH_Rand16(&gRandom, 0) & 1) {
        mTarget.z = -mTarget.z;
    }
}

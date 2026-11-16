#include "ParticleTestScene.h"
#include "MainProgram3d.h"

extern "C" {
void *ArchiveManager_GetFileByName(ArchiveLoader *loader, const char *path);
void func_0202680c(int size);    // Sets aside a heap for the particles
void *func_02026824(int size);   // The allocator the particle system is given
void func_0202683c(void);        // Frees it
// The particle library (main program 0x0207AB20 on)
ParticleSystem *func_0207ab20(void *(*alloc)(int), int a, int b, int c, int d, int e);
void func_0207ac78(ParticleSystem *system, void *file);
void func_0207b110(ParticleSystem *system);
void func_0207b124(ParticleSystem *system);
void func_0207b138(ParticleSystem *system); // Draws the particles
void func_0207b258(ParticleSystem *system, void *a);
ParticleEmitter *func_0207b2fc(ParticleSystem *system, int effect, VecFx32 *position); // Starts an emitter
void func_0207b364(ParticleSystem *system, ParticleEmitter *emitter);
void func_0207b40c(ParticleSystem *system, ParticleEmitter *emitter);
}

void ParticleTestScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "STE", "Mdl/Spl/SplRoomEffect_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "STI", "Mdl/Body/body000_3D_LZ.bin", 0);
}

void ParticleTestScene::Init() {
    func_0202b0e8(&data_02122824);
    data_0211d604.Show(1);
    data_0211d604.mUnknown48 = 1;
    data_0211d604.mUnknown48Changed = TRUE;
    G3X_SetClearColor(GX_RGB(15, 5, 5), 31, 0x7FFF, 63, FALSE);
    func_0202680c(0x4000);
    mParticles = func_0207ab20(func_02026824, 1, 100, 0, 1, 30);
    mEffectFile = ArchiveManager_GetFileByName(&gArchiveManager, "STE:/SplRoomEffect.spa");
    func_0207ac78(mParticles, mEffectFile);
    func_0207b110(mParticles);
    func_0207b124(mParticles);
    mEffectCount = 0;
    mRunning = TRUE;
    VecFx32 origin = {0, 0, 0};
    mEmitterPosition = origin;
    mEmitter = func_0207b2fc(mParticles, mEffectCount, &mEmitterPosition);
    ModelResources_SetModel(data_021223e4, 0, "STI:/body.imd");
    mBody.Reload(0, 0);
    mBody.mAnim.mRotY = 0x3000;
    mBody.mPos.x = FX32_ONE;
    mBody.mPos.y = -FX32_ONE;
    mCamera = &data_02122764;
    mCamera->mPosition.z = 0xF000;
}

void ParticleTestScene::Update() {
    if (data_0211f9d4.mTrigger & PAD_BUTTON_A) {
        if (mEmitter != NULL) {
            func_0207b364(mParticles, mEmitter);
        }
        mEffectCount++;
        mEmitter = func_0207b2fc(mParticles, 0, &mEmitterPosition);
        mRunning = TRUE;
    } else if (data_0211f9d4.mHeld & PAD_BUTTON_B) {
        if (data_0211f9d4.mTrigger & PAD_KEY_UP) {
            mRunning ^= TRUE;
            mEmitter->mPaused = mRunning ? FALSE : TRUE;
        } else if (data_0211f9d4.mTrigger & PAD_KEY_DOWN) {
            func_0207b40c(mParticles, mEmitter);
        } else if (data_0211f9d4.mTrigger & PAD_KEY_RIGHT) {
            mEmitter = func_0207b2fc(mParticles, 0, &mEmitterPosition);
        } else if ((data_0211f9d4.mTrigger & PAD_KEY_LEFT) && mEmitter != NULL) {
            func_0207b364(mParticles, mEmitter);
        }
    } else if (data_0211f9d4.mTrigger & PAD_BUTTON_Y) {
        mEmitter->mEmissionInterval = 10;
    } else if (data_0211f9d4.mHeld & PAD_BUTTON_X) {
        if (data_0211f9d4.mHeld & PAD_KEY_RIGHT) {
            mEmitter->SetZ(FX32_ONE);
        } else if (data_0211f9d4.mHeld & PAD_KEY_LEFT) {
            mEmitter->SetZ(0);
        } else if (data_0211f9d4.mHeld & PAD_KEY_DOWN) {
            mEmitter->mEmissionPaused = TRUE;
        }
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
    MoveParticlesToNeck();
}

void ParticleTestScene::Draw() {
    mBody.Draw();
    func_0202ad84(data_021223e4);
    NNS_G3dGlbFlushP();
    func_0207b258(mParticles, &NNS_G3dGlb.cameraMtx);
    func_0207b138(mParticles);
}

void ParticleTestScene::Exit() {
    func_0202683c();
    func_0202aecc(data_021223e4, 0);
    ArchiveManager_Unmount(&gArchiveManager, "STE");
    ArchiveManager_Unmount(&gArchiveManager, "STI");
}

// Puts every particle of the emitter at the body's neck, offset by the effect's base position turned to the camera
void ParticleTestScene::MoveParticlesToNeck() {
    Particle *particle = mEmitter->mParticles;
    if (particle == NULL) {
        return;
    }
    MtxFx43 matrix;
    MtxFx33 rotation;
    mBody.GetJointMatrices(CharacterModel::JOINT_NECK, &matrix, &rotation);
    VecFx32 offset = mEmitter->mEffect->mHeader->mBasePosition;
    VecFx32 position = mBody.mPos;
    position.y = matrix._31;
    MtxFx33 inverseView;
    MI_Copy36B(NNS_G3dGlbGetInvV(), &inverseView);
    MTX_MultVec33(&offset, &inverseView, &offset);
    VEC_Add(&position, &offset, &position);
    while (particle != NULL) {
        particle->mPosition.x = position.x;
        particle->mPosition.y = position.y;
        particle->mPosition.z = position.z;
        particle = particle->mNext;
    }
}

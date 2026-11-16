#ifndef SCENE_PARTICLETESTSCENE_H
#define SCENE_PARTICLETESTSCENE_H

#include "Scene.h"
#include "Model.h"
#include "character/Character.h"

struct Camera;

// Scene 8 (main program, no overlay): a test of the particle library (main program 0x0207AB20 on) with the room
// effects "Mdl/Spl/SplRoomEffect_LZ.bin" (STE) and a body model "Mdl/Body/body000_3D_LZ.bin" (STI). An emitter of
// the first effect plays, its particles kept at the body's neck. A starts a new emitter; with B held, up pauses or
// resumes the emitter, down and left stop it and right starts another; Y sets its emission interval to 10 frames;
// with X held, right and left move it up and back and down pauses its emission; else the pad turns the camera.
// The scene factory (main func_0203c050, case 8) allocates 0x53C bytes.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

// STAND-INS for the particle library's structures, with the fields this scene touches.
struct Particle {
    /* 0x00 */ Particle *mNext;
    /* 0x04 */ u8 mUnknown04[0x34];
    /* 0x38 */ VecFx32 mPosition;
};

struct ParticleEffectHeader {
    /* 0x00 */ u32 mUnknown00;
    /* 0x04 */ VecFx32 mBasePosition; // Added to an emitter's position
};

struct ParticleEffect {
    /* 0x00 */ ParticleEffectHeader *mHeader;
};

struct ParticleEmitter {
    void SetZ(fx32 z) { mPosition.z = z + mEffect->mHeader->mBasePosition.z; }

    /* 0x00 */ ParticleEmitter *mNext;
    /* 0x04 */ ParticleEmitter *mPrev;
    /* 0x08 */ Particle *mParticles;
    /* 0x0C */ u8 mUnknown0C[0x14];
    /* 0x20 */ ParticleEffect *mEffect;
    /* 0x24 */ u32 mTerminate : 1;
    /*      */ u32 mEmissionPaused : 1;
    /*      */ u32 mPaused : 1;
    /*      */ u32 mUnknown24Bits3 : 29;
    /* 0x28 */ VecFx32 mPosition;
    /* 0x34 */ u8 mUnknown34[0x50];
    /* 0x84 */ u32 mEmissionInterval : 8;
    /*      */ u32 mUnknown84Bits8 : 24;
};

struct ParticleSystem;

class ParticleTestScene : public Scene {
public:
    ParticleTestScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void MoveParticlesToNeck();

    /* 0x010 */ ParticleSystem *mParticles;
    /* 0x014 */ ParticleEmitter *mEmitter;
    /* 0x018 */ void *mEffectFile;
    /* 0x01C */ VecFx32 mEmitterPosition;
    /* 0x028 */ int mEffectCount; // Counts the emitters A started
    /* 0x02C */ BOOL mRunning;    // B and up pause the emitter while it is cleared
    /* 0x030 */ Camera *mCamera;
    /* 0x034 */ CharacterModel mBody;
}; // 0x53C

#endif // SCENE_PARTICLETESTSCENE_H

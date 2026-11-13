#ifndef MODEL_H
#define MODEL_H

#include <nitro.h>
#include <nnsys.h>

// STAND-INS for the main program's 3D model classes (0x02029b04 to 0x0202b008), which are not decompiled yet.
// Only what the apartment scene's code shows is here: the virtual functions in the order of their tables
// (data_0210f648 and data_0210f664), the constructors and destructors as they appear inlined in the scene's own
// constructors and destructors, and the fields it touches. The names are descriptive placeholders.

// Resource types, as the files have them (the four letters in memory order)
#define RESOURCE_BCA0 0x30414342 // Joint animation
#define RESOURCE_BTP0 0x30505442 // Texture pattern animation

extern "C" {
// Frees a block of the main heap
void func_020267f4(void *block);
}

// A model of the library with its position, rotation and scale.
class Model {
public:
    Model() {
        mUnknown04 = 0;
        mUnknown0C = 0;
        mUnknown10 = 0;
        mResource = NULL;
    }
    virtual ~Model() {
        if (mResource != NULL) {
            func_020267f4(mResource);
        }
    }
    virtual void Load(int resourceId, int unk); // func_02029b04
    virtual void UnknownVirtual3();             // func_02029bd4
    virtual void Draw();                        // func_02029df4

    void AddPosition(const VecFx32 *offset); // func_02029edc

    /* 0x04 */ int mUnknown04;
    /* 0x08 */ int mUnknown08;
    /* 0x0C */ int mUnknown0C;
    /* 0x10 */ int mUnknown10;
    /* 0x14 */ NNSG3dRenderObj mRenderObj;
    /* 0x68 */ MtxFx33 mRotation;
    /* 0x8C */ VecFx32 mScale;
    /* 0x98 */ VecFx32 mPosition;
    /* 0xA4 */ u16 mRotX;
    /* 0xA6 */ u16 mRotY;
    /* 0xA8 */ u16 mRotZ;
    /* 0xAC */ void *mResource; // A block of the main heap that the model owns
    /* 0xB0 */ BOOL mVisible;
}; // 0xB4

// A model with animations.
class AnimModel : public Model {
public:
    AnimModel() {
        mAnimCount = 0;
        ResetAnims();
    }
    virtual ~AnimModel() { RemoveAnims(); }
    virtual void Draw(); // func_0202a0e4

    void SetAnimation(u32 resourceType, int index); // func_0202a2e0: e.g. 'BTP0', a texture pattern animation
    void RemoveAnims();                             // func_0202a464
    void RestartAnims();                            // func_0202a530
    void ResetAnims();                              // func_0202a618

    /* 0xB4 */ void *mAnims[8];
    /* 0xD4 */ int mAnimCount;
    /* 0xD8 */ fx32 mUnknownD8;
    /* 0xDC */ int mUnknownDC;
}; // 0xE0

extern "C" {
// The models' resource table (data_021223e4, 0x1c bytes per resource id): func_0202ad04 binds the files of a
// mounted archive to an id, func_0202aecc releases its model ('BMD0') and texture ('BTX0')
extern u32 data_021223e4[];
void *func_0202ad04(void *resources, int resourceId, const char *archiveName, int unk);
void func_0202aecc(void *resources, int resourceId);
}

#endif // MODEL_H

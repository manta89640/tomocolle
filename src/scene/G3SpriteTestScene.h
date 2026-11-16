#ifndef SCENE_G3SPRITETESTSCENE_H
#define SCENE_G3SPRITETESTSCENE_H

#include "MainProgram.h"

// Scene 0x25 (main program, no overlay): a test of the main program's 3D sprites, with the archive
// "Scn/Scene/SceneTestG3Sprite_LZ.bin". A hundred sprites of the texture "Face.tga" (32 by 32 pixels, one of four
// picked at random) stand at random places in front of a character's body model ("Mdl/Body/body000_3D_LZ.bin");
// each frame one of them, in turn, moves to a new place and gets a new random size. The scene factory (main
// func_0203c050, case 37) allocates 0x48B4 bytes and calls the constructor, which is in the main program's file of
// scene constructors (func_0204036c).
//
// The names are descriptive, not the original: no name of these classes survives in the ROM. The classes of the
// rest of the main program are STAND-INS: their sizes and the fields this scene touches.

// --- Models (main program 0x02029b04 to 0x0202b008): overlay 96's stand-ins ---

extern "C" {
void func_020267f4(void *block); // Frees a block of the main heap
}

// A model of the library with its position, rotation and scale.
class Model {
public:
    virtual ~Model() {
        if (mResource != NULL) {
            func_020267f4(mResource);
        }
    }
    virtual void Load(int resourceId, int unk); // func_02029b04
    virtual void UnknownVirtual3();             // func_02029bd4
    virtual void Draw();                        // func_02029df4

    /* 0x04 */ void *mResourceSet;
    /* 0x08 */ int mUnknown08;
    /* 0x0C */ NNSG3dResMdl *mResMdl;
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
    virtual ~AnimModel() { RemoveAnims(); }
    virtual void Draw(); // func_0202a0e4

    void RemoveAnims(); // func_0202a464

    /* 0xB4 */ void *mAnims[8];
    /* 0xD4 */ int mAnimCount;
    /* 0xD8 */ fx32 mUnknownD8;
    /* 0xDC */ int mUnknownDC;
}; // 0xE0

// The models' resource table (data_021223e4)
struct ModelResources {
    /* 0x000 */ u8 mUnknown000[0x424];
    /* 0x424 */ int mUnknown424;
};

extern "C" {
extern ModelResources data_021223e4;
void ModelResources_SetModel(ModelResources *resources, int resourceId, const char *modelPath);
void func_0202aecc(ModelResources *resources, int resourceId); // Releases an id's model and texture
void func_0202ae38(ModelResources *resources);
}

// --- Characters (main program 0x0208eb18 to 0x0209d898): overlay 74's stand-ins ---

// The character as the main program draws it: the saved character and its model's resources.
// func_0208fd18 constructs it, func_0208fd38 (its complete-object destructor) destroys it.
struct CharacterBase {
    ~CharacterBase();

    void InitFace(u32 defaultMii);
    void SetRandomMii(int sex, int unk1, int unk2, BOOL makeFace);
    void Draw();

    /* 0x0000 */ u8 mUnknown0000[0x2568];
    /* 0x2568 */ fx32 mUnknown2568;
    /* 0x256C */ u8 mUnknown256C[4];
    /* 0x2570 */ fx32 mUnknown2570;
    /* 0x2574 */ u8 mUnknown2574[0x70];
}; // 0x25E4

// The model of a character with its animations and joints (func_0208eba0 constructs it here). Not a class derived
// from the model: its constructors store only the tables of Model and AnimModel, and its destructor, inlined into
// the scene's, takes the render callback away before AnimModel's destructor runs.
class CharacterModel {
public:
    ~CharacterModel() { NNS_G3dRenderObjResetCallBack(&mModel.mRenderObj); }

    void Load(int resourceId, int unk); // 0x0208ec28
    void Draw();                        // 0x0208ed30

    /* 0x000 */ AnimModel mModel;
    /* 0x0E0 */ VecFx32 mPos;
    /* 0x0EC */ u8 mUnknown0EC[0x41C];
}; // 0x508


// --- 3D sprites (main program 0x0202b0e8 to 0x0202b6d0) ---

// A textured square drawn by the 3D engine: a rectangle of a texture of the sprite texture list
// (data_021232e4), at a position, with a scale. func_0202b130 constructs it.
class G3Sprite {
public:
    virtual ~G3Sprite(); // func_0202b6cc, which does nothing

    /* 0x04 */ int mTexture;
    /* 0x08 */ BOOL mUnknown08;     // func_0202b4a4 draws nothing while it or mUnknown18 is 0
    /* 0x0C */ int mFrames;       // The scene counts the frames since the sprite last moved
    /* 0x10 */ int mUnknown10;
    /* 0x14 */ int mUnknown14;
    /* 0x18 */ u8 mUnknown18;      // 31 by default
    /* 0x19 */ u8 mUnknown19;
    /* 0x1A */ u16 mUnknown1A;
    /* 0x1C */ fx32 mScaleX;       // 1.0 by default
    /* 0x20 */ fx32 mScaleY;
    /* 0x24 */ int mX;            // The scene puts it at a random place
    /* 0x28 */ int mY;
    /* 0x2C */ ScreenRect mRect;   // Of the texture
    /* 0x3C */ u8 mUnknown3C[0x10];
}; // 0x4C

// The textures of the 3D sprites
struct G3SpriteTextures;

extern "C" {
extern G3SpriteTextures data_021232e4;
void func_0202b4d8(G3SpriteTextures *textures);
void func_0202b4dc(G3SpriteTextures *textures, int index, const char *path, int format, int width, int height);
void func_0202b1f0(G3Sprite *sprite, int texture, const ScreenRect *rect); // Sets its texture and rectangle
void func_0202b4a4(G3Sprite *sprite);                                         // Draws it
void func_0202b4c4(G3Sprite *sprite);
}

// The scene.
class G3SpriteTestScene : public Scene {
public:
    enum { SPRITE_COUNT = 100 };

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void SetRandomPosition(int index);
    void UpdateSprites();

    /* 0x0010 */ Camera *mCamera;
    /* 0x0014 */ G3Sprite mSprites[SPRITE_COUNT];
    /* 0x1DC4 */ int mNextSprite; // The sprite that moves next: counts down from 99
    /* 0x1DC8 */ CharacterBase mCharacter;
    /* 0x43AC */ CharacterModel mModel;
}; // 0x48B4

#endif // SCENE_G3SPRITETESTSCENE_H

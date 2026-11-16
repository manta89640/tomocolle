#ifndef OV006_SCENE0F_H
#define OV006_SCENE0F_H

#include <nitro.h>

#include "character/Character.h"
#include "text/TextCanvas.h"
#include "Scene.h"

// The scene of overlay 6 (scene id 0x0F), which loads the characters' animations "Mdl/Ica/ica_mii_*_LZ.bin". The
// overlay is not decompiled: this is the layout its constructor (in SceneManager.cpp) shows. The classes' names
// are the scene id's: no name of these classes survives in the ROM.

// A character with a saved character of its own (constructor 0x0214BFC0 of the overlay; the implicit destructor is
// the main program's, 0x0203E98C, after the scene's constructor)
class Scene0FCharacter : public CharacterBase {
public:
    Scene0FCharacter();

    /* 0x25E4 */ CharacterModel mModel;
    /* 0x2AEC */ u8 mUnknown2AEC[0x2C];
    /* 0x2B18 */ SavedCharacter mSaved;
    /* 0x2DD8 */ int mUnknown2DD8;
    /* 0x2DDC */ int mUnknown2DDC;
    /* 0x2DE0 */ s16 mUnknown2DE0[4];
}; // 0x2DE8

// Three values of 12 and what 0x0214C204 of the overlay sets (constructor 0x0214C1C0)
struct Scene0FUnknown3BC4 {
    Scene0FUnknown3BC4();

    /* 0x0 */ u16 mUnknown0[3];
    /* 0x6 */ u8 mUnknown6[6];
}; // 0xC

// A sprite text canvas of the overlay (its tables are the overlay's; constructor 0x0214C5CC, which clears 0x42
// bytes at 0x150)
class Scene0FText : public ObjTextCanvas {
public:
    Scene0FText();

    /* 0x150 */ u8 mUnknown150[0x44];
}; // 0x1FC

// Sprite text canvases of the main program, with members of their own: their constructors are implicit or inline,
// their tables are in SceneManager.cpp's data
class Scene0FTextCanvas : public ObjTextCanvas {}; // 0x1B8

class Scene0FTextCanvas2 : public ObjTextCanvas {
public:
    /* 0x150 */ u8 mUnknown150[0x24];
}; // 0x1DC

class Scene0FTextBox : public ObjTextCanvas {
public:
    Scene0FTextBox() { mUnknown150 = 0; }

    /* 0x150 */ int mUnknown150;
    /* 0x154 */ u8 mUnknown154[0x18];
}; // 0x1D4

class Scene0F : public Scene {
public:
    Scene0F(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x0010 */ Scene0FCharacter mCharacters[1];
    /* 0x2DF8 */ u8 mUnknown2DF8[0xC];
    /* 0x2E04 */ ObjTextCanvas mCanvases[8];
    /* 0x3BC4 */ Scene0FUnknown3BC4 mUnknown3BC4;
    /* 0x3BD0 */ Scene0FTextCanvas mCanvases2[2];
    /* 0x3F40 */ Scene0FText mText1;
    /* 0x413C */ Scene0FTextCanvas2 mCanvas3;
    /* 0x4318 */ Scene0FTextBox mTextBoxes[12];
    /* 0x5908 */ Scene0FText mText2;
    /* 0x5B04 */ Scene0FTextBox mTextBox1;
    /* 0x5CD8 */ Scene0FTextBox mTextBox2;
    /* 0x5EAC */ Scene0FTextBox mTextBox3;
    /* 0x6080 */ Scene0FTextBox mTextBox4;
    /* 0x6254 */ BgTextCanvas mBgCanvas;
    /* 0x62C0 */ u8 mUnknown62C0[4];
    /* 0x62C4 */ int mUnknown62C4;
    /* 0x62C8 */ s16 mUnknown62C8[4];
    /* 0x62D0 */ u8 mUnknown62D0[0x64];
}; // 0x6334

#endif // OV006_SCENE0F_H

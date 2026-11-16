#ifndef OV057_TESTNG137SCENE_H
#define OV057_TESTNG137SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "MessageWindow.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/SceneCharacter.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 57, scenes 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1, 0xA2 (not
// decompiled). The scene factory (CreateScene) loads overlay 57 and allocates 0x2E3D8 bytes; the constructor is the
// main program's (0x020480D8, src/scene/SceneManager.cpp), which stores the overlay's virtual table (0x0214CBA0). The
// members are as the constructor constructs them, by their classes; what lies between is not known.
//
// The name is after the scene's archive, "Scn/Scene/SceneTestNg137_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
// A model of the scene, of 0xF8 bytes: the main program's AnimModel at +0xC, with something before and after it.
// Its implicit constructor and destructor, which only construct and destroy the model, are the main program's
// copies after the scene's constructor (0x02048284, 0x020482CC), emitted for the scene's __cxa_vec_ctor. A member,
// not a base: they store only the model's tables.
//
// The name is descriptive, not the original.
class TestNg137Model {
public:
    /* 0x00 */ u8 mUnknown00[0xC];
    /* 0x0C */ AnimModel mModel;
    /* 0xEC */ u8 mUnknownEC[0xC];
}; // 0xF8

class TestNg137Scene : public Scene {
public:
    TestNg137Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ SceneCharacter mCharacters[4];
    /* 0x0AC70 */ u8 mUnknown0AC70[0x1BC8];
    /* 0x0C838 */ AnimModel mModels[2];
    /* 0x0C9F8 */ TestNg137Model mModelsC9F8[3];
    /* 0x0CCE0 */ u8 mUnknown0CCE0[0x90];
    /* 0x0CD70 */ TtsPlayer mTtsPlayer;
    /* 0x216B4 */ SpriteButton mButtons[1];
    /* 0x217A8 */ u8 mUnknown217A8[0xC];
    /* 0x217B4 */ MessageFile mMessageFiles[2];
    /* 0x217E4 */ u8 mUnknown217E4[0x8];
    /* 0x217EC */ MessageText mTexts[1];
    /* 0x21DDC */ u8 mUnknown21DDC[0x98];
    /* 0x21E74 */ MessageText mText;
    /* 0x22464 */ u8 mUnknown22464[0xFCC];
    /* 0x23430 */ MessageWindow mWindow;
    /* 0x2E3D4 */ u8 mUnknown2E3D4[0x4];
}; // 0x2E3D8

#endif // OV057_TESTNG137SCENE_H

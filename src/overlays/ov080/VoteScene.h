#ifndef OV080_VOTESCENE_H
#define OV080_VOTESCENE_H

#include <nitro.h>

#include "DrawingCanvas.h"
#include "G3Sprite.h"
#include "Scene.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 80, scene 0x23 (not decompiled). The scene factory (CreateScene) loads overlay 80 and allocates 0x3CD5C
// bytes; the constructor is the main program's (0x020400F0, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214CF50). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene mounts "Scn/Scene/SceneVote_LZ.bin", "Mdl/Spl/SplVoteEffect_LZ.bin" and the contents and
// common messages, and holds a drawing canvas.
//
// The names are after the scene's archive, "Scn/Scene/SceneVote_LZ.bin", which no other overlay mounts; they are not
// the original: no name of these classes survives in the ROM.

// Two 3D sprites and what goes with them. Its implicit constructor (0x02040268) and destructor (0x02040284, empty:
// G3Sprite's destructor is inline and does nothing) follow the scene's constructor, for __cxa_vec_ctor.
class VoteSpritePair {
public:
    /* 0x00 */ G3Sprite mSprite1;
    /* 0x4C */ G3Sprite mSprite2;
    /* 0x98 */ u8 mUnknown98[0x18];
}; // 0xB0

class VoteScene : public Scene {
public:
    VoteScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ DrawingCanvas mCanvas;
    /* 0x002C4 */ CharacterBase mCharacters[11];
    /* 0x1A390 */ u8 mUnknown1A390[0x1DC0];
    /* 0x1C150 */ VoteSpritePair mSpritePairs[150];
    /* 0x22870 */ u8 mUnknown22870[0x810];
    /* 0x23080 */ TextButton mTextButtons[3];
    /* 0x24550 */ SpriteButton mButtons[3];
    /* 0x2482C */ u8 mUnknown2482C[0x5D4];
    /* 0x24E00 */ TtsPlayer mTtsPlayer;
    /* 0x39744 */ MessageFile mMessageFiles[2];
    /* 0x39774 */ MessageText mTexts[6];
    /* 0x3BB14 */ u8 mUnknown3BB14[0x14];
    /* 0x3BB28 */ MessageText mTexts2[2];
    /* 0x3C708 */ u8 mUnknown3C708[0x10];
    /* 0x3C718 */ MessageText mTexts3[1];
    /* 0x3CD08 */ u8 mUnknown3CD08[0x54];
}; // 0x3CD5C

#endif // OV080_VOTESCENE_H

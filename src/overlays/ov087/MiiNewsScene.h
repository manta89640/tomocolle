#ifndef OV087_MIINEWSSCENE_H
#define OV087_MIINEWSSCENE_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "character/SceneCharacter.h"
#include "item/Item.h"
#include "sound/ValueRamp.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// Overlay 87, scenes 0x46, 0x47, 0x48, 0x49, 0x4A (not decompiled). The scene factory (CreateScene) loads overlay 87
// and allocates 0x3F258 bytes; the constructor is the main program's (0x02043800, src/scene/SceneManager.cpp), which
// stores the overlay's virtual table (0x0214DF68). The members are as the constructor constructs them, by their
// classes; what lies between is not known.
//
// The name is after the scene's archive, "Scn/Scene/SceneMiiNews_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class MiiNewsScene : public Scene {
public:
    MiiNewsScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ u8 mUnknown00010[0x1BC0];
    /* 0x01BD0 */ SceneCharacter mCharacters[3];
    /* 0x09D18 */ CharacterBase mCharacterBases[11];
    /* 0x23DE4 */ u8 mUnknown23DE4[0x1DBC];
    /* 0x25BA0 */ AnimModel mModels[2];
    /* 0x25D60 */ AnimModel mModel;
    /* 0x25E40 */ AnimModel mModel25E40;
    /* 0x25F20 */ u8 mUnknown25F20[0x228];
    /* 0x26148 */ TtsPlayer mTtsPlayer;
    /* 0x3AA8C */ MessageText mText;
    /* 0x3B07C */ MessageFile mMessageFiles[3];
    /* 0x3B0C4 */ MessageText mTexts[4];
    /* 0x3C884 */ u8 mUnknown3C884[0x5F4];
    /* 0x3CE78 */ SavedCharacter mSavedCharacters[5];
    /* 0x3DC38 */ u8 mUnknown3DC38[0x470];
    /* 0x3E0A8 */ ItemModel mItemModels[2];
    /* 0x3E300 */ SpriteButton mButtons[1];
    /* 0x3E3F4 */ TextButton mTextButtons[2];
    /* 0x3F1D4 */ u8 mUnknown3F1D4[0x4];
    /* 0x3F1D8 */ SoundParamRamps mSoundRamps[2];
}; // 0x3F258

#endif // OV087_MIINEWSSCENE_H

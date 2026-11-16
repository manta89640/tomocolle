#ifndef OV071_RIGHTJOB2SCENE_H
#define OV071_RIGHTJOB2SCENE_H

#include <nitro.h>

#include "character/Character.h"
#include "Model.h"
#include "SpriteButton.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"
#include "Scene.h"

// The scene of overlay 71 (scene ids 0x20 to 0x22), which mounts Scn/Scene/SceneRightJob2_LZ.bin: eleven Mii faces,
// two animated models, a saved character, buttons, message texts and a speech player. The overlay is not
// decompiled: this is the layout its constructor (in SceneManager.cpp) shows. The name is after its archive.
class RightJob2Scene : public Scene {
public:
    RightJob2Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ CharacterBase mFaces[11];
    /* 0x1A0DC */ u8 mUnknown1A0DC[0x1DBC];
    /* 0x1BE98 */ AnimModel mModels[2];
    /* 0x1C058 */ u8 mUnknown1C058[0xC];
    /* 0x1C064 */ SavedCharacter mCharacter;
    /* 0x1C324 */ SpriteButton mButtons[1];
    /* 0x1C418 */ TextButton mTextButtons[1];
    /* 0x1CB08 */ MessageFile mMessageFiles[1];
    /* 0x1CB20 */ MessageText mTexts[8];
    /* 0x1FAA0 */ u8 mUnknown1FAA0[0x104];
    /* 0x1FBA4 */ TtsPlayer mTtsPlayer;
    /* 0x344E8 */ u8 mUnknown344E8[0x38];
}; // 0x34520

#endif // OV071_RIGHTJOB2SCENE_H

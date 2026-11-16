#ifndef OV070_RIGHTJOBSCENE_H
#define OV070_RIGHTJOBSCENE_H

#include <nitro.h>

#include "character/SceneCharacter.h"
#include "list/MiiList.h"
#include "MessageWindow.h"
#include "Model.h"
#include "SpriteButton.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"
#include "voice/TtsPlayer.h"
#include "Scene.h"

// The scene of overlay 70 (scene id 0x1F), which mounts Scn/Scene/SceneRightJob_LZ.bin: the list of
// residents with its panel and tabs, buttons, message windows and texts, and a part with a character, a Mii face and a model. The overlay is not
// decompiled: this is the layout its constructor (in SceneManager.cpp) shows. The name is after its archive.
class RightJobScene : public Scene {
public:
    RightJobScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    // A character, a Mii face and a model, which the constructor builds through a pointer to their start
    struct CharacterPart {
        /* 0x0000 */ SceneCharacter mCharacter;
        /* 0x2B18 */ CharacterBase mFace;
        /* 0x50FC */ u8 mUnknown50FC[4];
        /* 0x5100 */ AnimModel mModel;
        /* 0x51E0 */ u8 mUnknown51E0[0x20];
    }; // 0x5200

    /* 0x00010 */ MessageFile mMessageFiles[3];
    /* 0x00058 */ u8 mUnknown00058[0x1E8];
    /* 0x00240 */ TtsPlayer mTtsPlayer;
    /* 0x14B84 */ u8 mUnknown14B84[4];
    /* 0x14B88 */ MessageText mTexts[2];
    /* 0x15768 */ u8 mUnknown15768[0x38];
    /* 0x157A0 */ TextButton mTextButtons[3];
    /* 0x16C70 */ MiiList mList;
    /* 0x28018 */ MiiListPanel mListPanel;
    /* 0x31490 */ MiiListTabs mListTabs;
    /* 0x365E8 */ MessageWindow mWindows[2];
    /* 0x4C530 */ CharacterPart mCharacterPart;
}; // 0x51730

#endif // OV070_RIGHTJOBSCENE_H

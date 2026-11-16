#ifndef OV078_FRIENDLISTSCENE_H
#define OV078_FRIENDLISTSCENE_H

#include <nitro.h>

#include "character/SceneCharacter.h"
#include "list/MiiList.h"
#include "MessageWindow.h"
#include "SpriteButton.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"
#include "voice/TtsPlayer.h"
#include "Scene.h"

// The scene of overlay 78: the list of residents (MiiList with its panel and tabs), five message windows, buttons,
// a speech player, and a part with two characters and texts. The overlay is not decompiled: this is the layout its
// constructor (in SceneManager.cpp) shows. The name is the one the scene factory's stand-ins had; it is descriptive,
// not the original.
class FriendListScene : public Scene {
public:
    FriendListScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    // Two characters and texts, which the constructor builds through a pointer to their start
    struct CharacterPart {
        /* 0x0000 */ u8 mUnknown0000[8];
        /* 0x0008 */ SceneCharacter mCharacters[2];
        /* 0x5638 */ u8 mUnknown5638[0xE8];
        /* 0x5720 */ ObjTextBox mTextBox;
        /* 0x58DC */ u8 mUnknown58DC[0x808];
        /* 0x60E4 */ ObjTextBox mTextBoxes[9];
        /* 0x7080 */ MessageText mText1;
        /* 0x7670 */ MessageText mText2;
        /* 0x7C60 */ u8 mUnknown7C60[0xBC];
        /* 0x7D1C */ MessageText mTexts[9];
        /* 0xB28C */ ObjTextBox mTextBoxes2[10];
        /* 0xC3E4 */ MessageText mText3;
        /* 0xC9D4 */ u8 mUnknownC9D4[0xBBC];
    }; // 0xD590

    /* 0x00010 */ u8 mUnknown00010[0x10];
    /* 0x00020 */ MessageFile mMessageFile1;
    /* 0x00038 */ MessageFile mMessageFile2;
    /* 0x00050 */ MessageFile mMessageFile3;
    /* 0x00068 */ u8 mUnknown00068[0x28];
    /* 0x00090 */ TextButton mTextButtons[3];
    /* 0x01560 */ SpriteButton mButton1;
    /* 0x01654 */ SpriteButton mButton2;
    /* 0x01748 */ MiiList mList;
    /* 0x12AF0 */ MiiListPanel mListPanel;
    /* 0x1BF68 */ MiiListTabs mListTabs;
    /* 0x210C0 */ MessageWindow mWindows[5];
    /* 0x57EF4 */ TtsPlayer mTtsPlayer;
    /* 0x6C838 */ MessageText mText;
    /* 0x6CE28 */ CharacterPart mCharacterPart;
}; // 0x7A3B8

#endif // OV078_FRIENDLISTSCENE_H

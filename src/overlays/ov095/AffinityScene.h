#ifndef OV095_AFFINITYSCENE_H
#define OV095_AFFINITYSCENE_H

#include <nitro.h>

#include "Scene.h"
#include "list/MiiList.h"
#include "MessageWindow.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

// A part of the scene with its own members, which the constructor constructs from the part's address (the end of the
// windows, +0x583C4 of the scene): models, eleven faces, a message file and texts.
struct AffinityPart {
    /* 0x00000 */ AnimModel mModels[2];
    /* 0x001C0 */ AnimModel mModel1C0;
    /* 0x002A0 */ AnimModel mModel2A0;
    /* 0x00380 */ AnimModel mModels380[3];
    /* 0x00620 */ AnimModel mModel620;
    /* 0x00700 */ u8 mUnknown00700[0xC];
    /* 0x0070C */ CharacterBase mCharacterBases[11];
    /* 0x1A7D8 */ u8 mUnknown1A7D8[0x1DD8];
    /* 0x1C5B0 */ MessageFile mMessageFiles[1];
    /* 0x1C5C8 */ MessageText mTexts[5];
    /* 0x1E378 */ u8 mUnknown1E378[0x35C];
}; // 0x1E6D4

// Overlay 95, scene 0x80 (not decompiled). The scene factory (CreateScene) loads overlay 95 and allocates 0x76A98
// bytes; the constructor is the main program's (0x02046750, src/scene/SceneManager.cpp), which stores the overlay's
// virtual table (0x0214F8F8). The members are as the constructor constructs them, by their classes; what lies between
// is not known. The scene's StartLoading mounts "Scn/Scene/SceneAffinity_LZ.bin", "Scn/List/List_RightJobList_LZ.bin",
// "Msg/TmdEtcMsgJP_LZ.bin", "Msg/TmdContentsMsgJP_LZ.bin", "Msg/TmdcommonMsgJP_LZ.bin", "Scn/Scene/NgCaption_LZ.bin".
//
// The name is after the scene's archive, "Scn/Scene/SceneAffinity_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class AffinityScene : public Scene {
public:
    AffinityScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    /* 0x00010 */ MessageFile mMessageFiles[3];
    /* 0x00058 */ u8 mUnknown00058[0x98];
    /* 0x000F0 */ TtsPlayer mTtsPlayer;
    /* 0x14A34 */ u8 mUnknown14A34[0x8];
    /* 0x14A3C */ MessageText mTexts[1];
    /* 0x1502C */ u8 mUnknown1502C[0x3C];
    /* 0x15068 */ TextButton mTextButtons[4];
    /* 0x16C28 */ TextButton mTextButtons16C28[2];
    /* 0x17A08 */ u8 mUnknown17A08[0x158];
    /* 0x17B60 */ MiiList mList;
    /* 0x28F08 */ MiiListPanel mPanel;
    /* 0x32380 */ MiiListTabs mTabs;
    /* 0x374D8 */ MessageWindow mWindows[3];
    /* 0x583C4 */ AffinityPart mPart;
}; // 0x76A98

#endif // OV095_AFFINITYSCENE_H

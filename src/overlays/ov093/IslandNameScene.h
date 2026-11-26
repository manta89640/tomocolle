#ifndef OV093_ISLANDNAMESCENE_H
#define OV093_ISLANDNAMESCENE_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "keyboard/SoftKeyboard.h"
#include "sound/ValueRamp.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"

// Overlay 93, scene 0x72: the player names the island. Over a model of the island (its ground, in the season of
// the game clock's month, and twelve boards), the upper screen asks "島の名前を入力してください" ("enter the
// island's name", message 0x17B of tmd_etc_msg_JP.bmg) and shows the name typed with "島" (island) after it; the
// lower screen has the on-screen keyboard. Up to six characters; the keyboard's OK button, shown once the name has
// more than spaces, copies the name into the main program's island name and goes on to the tutorial's next step
// (scene 0x69, overlay 108).
//
// The scene factory (CreateScene) loads overlay 93 and allocates 0x8328 bytes; the constructor is the main
// program's (0x02045B6C, src/scene/SceneManager.cpp), which stores the overlay's virtual table (0x0214B3D4).
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

// The scene's music: sequence 4 of sequence archive 3 on handle 0, with its volume, pitch, tempo and pan ramped
// towards one of thirteen settings (the scene only ever uses the first). The scene's constructor builds it as an
// array of one.
class IslandNameMusic : public SoundParamRamps {
public:
    // Loads the sound group, starts the music and sets the ramps to the first setting at once
    void Start();
    // Steps the ramps and passes their values to the sound manager
    void Update();
    // Ramps towards a setting
    void SetTarget(int setting);
};

class IslandNameScene : public Scene {
public:
    enum { NAME_LENGTH = 6 };

    // What mState counts through
    enum State {
        STATE_NAMING, // The keyboard is in use
    };

    IslandNameScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void SetState(int state);
    // Shows a name, character by character on mCharBox, and takes it as the name typed so far
    void SetName(const wchar_t *name);
    // The keyboard: typing, erasing, the voicing marks and small kana, and the OK button
    void UpdateNaming();

    /* 0x00010 */ SoftKeyboard mKeyboard;
    /* 0x06008 */ int mUnknown6008;         // Cleared when a key gives a character
    /* 0x0600C */ ObjTextBox mTextBox;      // Given an empty text
    /* 0x061C8 */ u8 mTextBuffer[0x600];
    /* 0x067C8 */ ObjTextBox mSuffixBox;    // "島", after the name
    /* 0x06984 */ u8 mSuffixBuffer[0x600];
    /* 0x06F84 */ ObjCharBox mCharBox;      // The name, drawn a character at a time
    /* 0x07140 */ int mLength;              // How many characters are typed
    /* 0x07144 */ wchar_t mName[NAME_LENGTH + 5];
    /* 0x0715C */ int mState;
    /* 0x07160 */ int mUnknown7160;         // Cleared as a state starts; nothing in the overlay reads it
    /* 0x07164 */ u8 mUnknown07164[0x4];
    /* 0x07168 */ AnimModel mModels[13];    // The island's ground, then twelve boards
    /* 0x07CC8 */ MessageFile mMessageFiles[2]; // tmd_common_msg_JP.bmg (the keyboard's buttons), tmd_etc_msg_JP.bmg
    /* 0x07CF8 */ IslandNameMusic mMusic[1];
    /* 0x07D38 */ MessageText mText;        // "Enter the island's name"
}; // 0x8328

#endif // OV093_ISLANDNAMESCENE_H

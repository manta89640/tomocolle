#ifndef SCENE_KEYBOARDTESTSCENE_H
#define SCENE_KEYBOARDTESTSCENE_H

#include "Scene.h"
#include "text/MessageFile.h"
#include "text/TextCanvas.h"
#include "keyboard/SoftKeyboard.h"

// Scene 11 (main program, no overlay): a test of the on-screen keyboard (SoftKeyboard) on the lower screen, with
// "Cmn/Kbd/SoftKeyboard_LZ.bin" (KBD), "Scn/Scene/ScenePlayRoom2_LZ.bin" (MNU) and the common messages (TBM). Up to
// ten characters typed are shown on a sprite text box on the upper screen, the last of them also alone, centred
// over its place, on a character box; touching the keyboard's two buttons presses one and releases the other. A
// puts the room's background "MNU:/BG_down" on the keyboard's plane, B the keyboard's again. The scene factory
// (main func_0203c050, case 11) allocates 0x6AF8 bytes.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.
class KeyboardTestScene : public Scene {
public:
    KeyboardTestScene(int sceneId);

    enum { NAME_LENGTH = 10 };

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    // The keyboard's second button, its address formed as the original forms it (see decompiling.md)
    KeyboardButton *SecondButton() { return (KeyboardButton *)((u8 *)&mKeyboard.mButtons + sizeof(KeyboardButton)); }

    /* 0x0010 */ SoftKeyboard mKeyboard;
    /* 0x6008 */ int mUnknown6008;          // Cleared when a key gives a character
    /* 0x600C */ ObjTextBox mTextBox;       // The characters typed
    /* 0x61C8 */ ObjCharBox mCharBox;       // The last of them
    /* 0x6384 */ u8 mCharBuffer[0x600];     // The text box's
    /* 0x6984 */ int mLength;               // How many characters are typed
    /* 0x6988 */ wchar_t mName[NAME_LENGTH + 1];
    /* 0x69A0 */ u32 mSavedGraphics[0x50];  // The graphics state of the lower screen, as GraphicsManager_SaveState saved it
    /* 0x6AE0 */ MessageFile mFiles[1];
}; // 0x6AF8

#endif // SCENE_KEYBOARDTESTSCENE_H

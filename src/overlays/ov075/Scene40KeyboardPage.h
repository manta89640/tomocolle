#ifndef OV075_SCENE40KEYBOARDPAGE_H
#define OV075_SCENE40KEYBOARDPAGE_H

#include <nitro.h>

#include "SpriteButton.h"
#include "keyboard/SoftKeyboard.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"
#include "Scene40Context.h"

// The keyboard page (pages+0x1AAB8, the controller's page 8): the soft keyboard, to type a word the character then
// says (the text is made speakable and spoken by the TtsPlayer, 0x0215A368). Not decompiled yet: the fields are those
// its functions and the controller show.
//
// The name is descriptive: no name of this class survives in the ROM.
class Scene40KeyboardPage {
public:
    void Init(Scene40Context *context, int *charOffset); // 0x02159C1C
    void Update(); // 0x02159E58
    void Open();   // 0x02159EC4
    void Type();   // 0x0215A368
    void ShowText(); // 0x0215ADD4
    void Draw();   // 0x0215AEF8

    /* 0x0000 */ int mState;
    /* 0x0004 */ int mNextState;
    /* 0x0008 */ int mUnknown0008;
    /* 0x000C */ BOOL mActive;
    /* 0x0010 */ int mUnknown0010;
    /* 0x0014 */ SoftKeyboard mKeyboard;
    /* 0x600C */ ObjCharBox mCharBox;
    /* 0x61C8 */ int mUnknown61C8;
    /* 0x61CC */ int mUnknown61CC;
    /* 0x61D0 */ int mUnknown61D0;
    /* 0x61D4 */ wchar_t mText[17];
    /* 0x61F6 */ wchar_t mText2[17];
    /* 0x6218 */ int mObjCharOffset; // Where the keyboard's sprite characters start (the controller's Init sets it)
    /* 0x621C */ Scene40Context *mContext;
    /* 0x6220 */ MessageText mTitle;
    /* 0x6810 */ SpriteButton mButton;
    /* 0x6904 */ SpriteButton mButton2;
}; // 0x69F8

#endif // OV075_SCENE40KEYBOARDPAGE_H

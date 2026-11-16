#ifndef KEYBOARD_SOFTKEYBOARD_H
#define KEYBOARD_SOFTKEYBOARD_H

#include <nitro.h>
#include <nnsys.h>

#include "Sprite.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"

// The on-screen keyboard of the main program (src/keyboard/Keyboard.cpp): the object that a scene holds, with the
// keyboard's graphics, its five tabs and its two buttons. The keys themselves are the keyboard's core
// (src/keyboard/KeyboardCore.cpp), which this object sets up and draws through.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM. The graphics come from
// the archive Cmn/Kbd/SoftKeyboard_LZ.bin, hence the name.

// One of the two buttons under the keys: a sprite with a message text on it, which plays a sound as it is touched
// and released.
class KeyboardButton : public Sprite {
public:
    KeyboardButton();
    // Clears the touch state and sets the button's index (its place and message)
    void Reset(s16 index);
    // Clears the touch state and shows the button released: frame 0, stopped
    void Release();
    BOOL Update(); // Whether it was pressed: touched and released on it
    void Draw();
    // How many characters (tiles) the text took, added to charOffset
    int InitText(MessageFile *file, int charOffset);
    void SetVisible(BOOL visible);
    // Whether a point is inside the button's touch rectangle
    BOOL Contains(ScreenPoint point);
    // The sound handle and sequence archive of the two sounds, and the sounds (-1 for none) as it is touched and
    // as it is released
    void SetSounds(int handle, int seqArc, int touchSound, int releaseSound);
    void SetMessage(u32 message);
    int GetCharCount() { return mText.GetCharCount(); }

    /* 0x0AC */ BOOL mHeld;    // Set as it is touched, cleared as the stylus leaves the screen
    /* 0x0B0 */ BOOL mPressed; // Set as it is touched, cleared by Reset and Release
    /* 0x0B4 */ int mSoundHandle;
    /* 0x0B8 */ int mSoundSeqArc;
    /* 0x0BC */ int mTouchSound;
    /* 0x0C0 */ int mReleaseSound;
    /* 0x0C4 */ s16 mIndex; // 0 or 1: its place, and which message (6 - mIndex) it shows
    /* 0x0C8 */ MessageText mText;
}; // 0x6B8

// What the keyboard's core calls with the sounds to play (KeyboardCore.cpp, sound numbers 0 to 10)
typedef void (*KeyboardSoundCallback)(void *userData, int sound);

class SoftKeyboard {
public:
    SoftKeyboard();
    // Puts the name of the archive the graphics are in over the "xxx" of each of their paths
    void SetArchiveName(const char *name);
    // Sets up the background: the plane and the core (InitBgGraphics, InitCore with PlayKeySound), and sends the
    // character data, the screen and the palette (from paletteOffset in fs_keyboardColor.ncl) to it
    void InitBg(int plane, int paletteOffset);
    // Gives the core its buffers and files, and takes the plane
    void InitBgGraphics(int plane);
    // Initialises the core at the keys' origin (45, 59) with a sound callback, makes the screen, registers the
    // first two tabs without a callback, and scrolls the plane to the origin
    void InitCore(KeyboardSoundCallback callback, void *userData);
    // The core's update, from the pad and the touch screen
    void Update(int pad, TPData tp);
    // Selects a mode of the core: the tabs' order, kana, English, euro, sign, picto
    void SetMode(int mode, BOOL playSound);
    // Puts mLayoutSprite back at the position mLayout selects
    void PlaceLayoutSprite();
    // Loads the sprite graphics (SetObjResource, LoadObjCells, LoadObjPalette) and sets the sprites up
    void LoadObj(s16 resource, int a, int palette);
    // LoadObj with a layout, then the buttons' texts from the message file
    void InitObj(s16 resource, MessageFile *file, int charOffset, int a, s16 layout, int palette);
    // Registers the five tabs as the core's touch areas, with OnTabArea
    void EnableTabs();
    // The core's callback for the tabs: when one is pressed, lights it and selects its mode
    static void OnTabArea(SoftKeyboard *keyboard, int action, int pressedCode, int touchedCode);
    // The core's sound callback: sequence 1 for sound 0 (a mode is selected), sequence 0 for 1 to 8
    static void PlayKeySound(SoftKeyboard *keyboard, int sound);
    void Draw();
    // Sets the sounds of one of the two buttons
    void SetButtonSounds(int button, int handle, int seqArc, int touchSound, int releaseSound);
    // Puts the buttons' messages on them, from charOffset on; returns where they end
    int InitButtonTexts(MessageFile *file, int charOffset);
    // How many characters (tiles) the buttons' texts take
    int GetButtonCharCount();
    // Queues the keys' character data (the core's buffer) to the plane's VRAM
    void TransferCharData();
    // Sends mScreen to the plane
    void TransferScreen();
    // Loads a palette of fs_keyboardColor.ncl, from a byte offset in the file, as palette mPalette of the plane
    void LoadBgPalette(int offset);
    void SetObjResource(s16 resource);
    // Loads obj_keyboard.nce and obj_chat_keyboard.ncg as sprite resource mObjResource
    void LoadObjCells(int a);
    // Loads keyboardColor.ncl as sprite palette 'palette'
    void LoadObjPalette(int palette);
    // Sets up the tabs, the buttons and mLayoutSprite
    void InitSprites();

    /* 0x0000 */ int mPlane;        // The background plane the keyboard is on
    /* 0x0004 */ int mPalette;      // Its palette number
    /* 0x0008 */ int mSoundHandle;  // Of the key sounds
    /* 0x000C */ u16 mScreen[0x400]; // The keys' screen data (the core fills it)
    /* 0x080C */ u8 mBaseCharData[0x2280]; // The layout's graphics with the labels drawn (the core's)
    /* 0x2A8C */ u8 mCharData[0x2280]; // The keys' character data, which the core draws
    /* 0x4D0C */ char mBgFiles[5][0x30]; // fs_chat_keyboard_jp.NCGR, fs_chat_keyboard.NCGR, _euro, _sign, _picto
    /* 0x4DFC */ char mBgPaletteFile[0x20]; // fs_keyboardColor.ncl
    /* 0x4E1C */ Sprite mTabs[5];
    /* 0x5178 */ KeyboardButton mButtons[2];
    /* 0x5EE8 */ Sprite mLayoutSprite; // Cell 7 of obj_keyboard.nce, placed by mLayout
    /* 0x5F94 */ s16 mObjResource; // The sprite resource number of obj_keyboard.nce
    /* 0x5F96 */ s16 mLayout;      // Where mLayoutSprite is: an index of a table of five positions
    /* 0x5F98 */ char mObjCellFile[0x20];    // obj_keyboard.nce
    /* 0x5FB8 */ char mObjCharFile[0x20];    // obj_chat_keyboard.ncg
    /* 0x5FD8 */ char mObjPaletteFile[0x20]; // keyboardColor.ncl
}; // 0x5FF8: the Mii maker's profile page has fields of its own from +0x5FF8 of its keyboard on

#endif // KEYBOARD_SOFTKEYBOARD_H

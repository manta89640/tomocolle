// The on-screen keyboard's object: see docs/binary.md, "The keyboard's object".

#include <nitro.h>

#include "keyboard/SoftKeyboard.h"
#include "sound/SoundManager.h"
#include "text/TextCanvas.h"

// --- What the keyboard uses of the main program (STAND-INS, not decompiled) --------------------------------------

struct PaletteLoader;
struct ScreenLoader;
struct CellLoader;

// The touch screen
struct TouchInput {
    /* 0x00 */ u8 mUnknown00[0x2C];
    /* 0x2C */ u16 mTouching;
    /* 0x2E */ u8 mUnknown2E[2];
    /* 0x30 */ ScreenPoint mLastPoint; // Where it was touched last
    /* 0x38 */ ScreenPoint mPoint;     // Where it is touched
    /* 0x40 */ u8 mUnknown40[8];
    /* 0x48 */ BOOL mTouched;          // The stylus went down this frame
    /* 0x4C */ BOOL mUnknown4C;
};

extern "C" {
extern TouchInput gTouchInput;
extern PaletteLoader data_021213b8;
extern ScreenLoader data_02121834;
extern CellLoader gObjResourceManager;
extern u8 gVramTransferQueue[];
void *func_02027534(PaletteLoader *loader, const char *name, int plane, int a, int b, int c); // Loads a palette
void func_02027f3c(ScreenLoader *loader, const void *screen, int plane, int a, int size);
void func_02028108(ScreenLoader *loader, int plane, int offset);
void func_02028154(ScreenLoader *loader, int plane, int offset);
void func_0202543c(u32 *graphicsManager, int plane, int a, int b, int c, int d, int e, int f);
void VramTransferQueue_AddCharData(void *queue, void *source, int plane, int offset, int size);
void ObjResourceManager_Load(CellLoader *loader, const char *cellPath, const char *charPath, int plane,
                             int resource, int unk);
}

// --- The keyboard's core (KeyboardCore.cpp) ----------------------------------------------------------------------

// A key or a touch area: a rectangle in pixels and the code it gives
struct KeyArea {
    /* 0x0 */ u16 x : 8;
    u16 y : 8;
    /* 0x2 */ u16 width : 8;
    u16 height : 8;
    /* 0x4 */ u16 mUnknown4;
    /* 0x6 */ u16 code;
};

typedef void (*KeyCallback)(void *userData, int event);
typedef void (*AreaCallback)(void *userData, int event, int a, int code);

extern "C" {
void func_02081cf8(u16 *screen, int palette, int charOffset);
void func_02081d68(void *a, void *charData, char (*files)[0x30]);
void func_02081df0(int x, int y, int a, KeyCallback callback, void *userData);
void func_02081ff0(int mode, BOOL b);
void func_02082068(int a, TPData tp);
void *func_0208265c();
int func_02082680();
void func_02082e70(const KeyArea *areas, int count, AreaCallback callback, void *userData);
}

// --- This file ----------------------------------------------------------------------------------------------------

extern "C" {
void func_02083078(SoftKeyboard *keyboard, int plane, int palette);
void func_020830bc(SoftKeyboard *keyboard, int plane);
void func_02083128(SoftKeyboard *keyboard, KeyCallback callback, void *userData);
void func_020831a4(SoftKeyboard *keyboard);
void func_020831d8(SoftKeyboard *keyboard);
void func_02083204(SoftKeyboard *keyboard, int palette);
void func_02083244(SoftKeyboard *keyboard, int a, TPData tp);
void func_02083288(SoftKeyboard *keyboard, int mode, BOOL b);
void func_020832d0(KeyboardButton *button, s16 index);
void func_020832e4(KeyboardButton *button);
void func_020837a8(SoftKeyboard *keyboard);
void func_020837f4(SoftKeyboard *keyboard, s16 resource, int a, int palette);
void func_0208382c(SoftKeyboard *keyboard, s16 resource, MessageFile *file, int charOffset, int a, s16 layout,
                   int palette);
void func_02083868(SoftKeyboard *keyboard, s16 resource);
void func_02083874(SoftKeyboard *keyboard, int a);
void func_020838b4(SoftKeyboard *keyboard, int palette);
void func_020838f4(SoftKeyboard *keyboard);
void func_02083c30(SoftKeyboard *keyboard);
void func_02083c54(SoftKeyboard *keyboard, int event, int a, int code);
void func_02083d58(SoftKeyboard *keyboard, int event);
}

// (The two 16-byte tables are declared in the reverse of their order in the ROM: the compiler's sort by size
// puts objects of one size in the other order.)
// Where a button takes a touch, around its position
static const ScreenRect sButtonRect = {-58, -18, 120, 34};
// Where the two buttons are, in pixels
static const ScreenPoint sButtonPositions[2] = {{0x80, 0xAD}, {0x00, 0xAD}};
// The five tabs
static const KeyArea sTabAreas[5] = {
    {0x0B, 0x43, 0x21, 0x10, 0, 0x100},
    {0x0B, 0x53, 0x21, 0x10, 0, 0x200},
    {0x0B, 0x63, 0x21, 0x10, 0, 0x300},
    {0x0B, 0x73, 0x21, 0x10, 0, 0x400},
    {0x0B, 0x83, 0x21, 0x10, 0, 0x500},
};
// Where mSprite5EE8 is, by mLayout
static const ScreenPoint sLayoutPositions[5] = {{0x46, 0x32}, {0x17, 0x32}, {0x60, 0x32}, {0x53, 0x32}, {0x1A, 0x36}};

SoftKeyboard::SoftKeyboard() {
    mPalette = 13;
    mSoundHandle = 0;
    STD_CopyString(mBgFiles[0], "xxx:/fs_chat_keyboard_jp.NCGR");
    STD_CopyString(mBgFiles[1], "xxx:/fs_chat_keyboard.NCGR");
    STD_CopyString(mBgFiles[2], "xxx:/fs_chat_keyboard_euro.NCGR");
    STD_CopyString(mBgFiles[3], "xxx:/fs_chat_keyboard_sign.NCGR");
    STD_CopyString(mBgFiles[4], "xxx:/fs_chat_keyboard_picto.NCGR");
    STD_CopyString(mBgPaletteFile, "xxx:/fs_keyboardColor.ncl");
    STD_CopyString(mObjCellFile, "xxx:/obj_keyboard.nce");
    STD_CopyString(mObjCharFile, "xxx:/obj_chat_keyboard.ncg");
    STD_CopyString(mObjPaletteFile, "xxx:/keyboardColor.ncl");
    mLayout = 0;
}

void SoftKeyboard::SetArchiveName(const char *name) {
    MI_CpuCopy8(name, mBgFiles[0], 3);
    MI_CpuCopy8(name, mBgFiles[1], 3);
    MI_CpuCopy8(name, mBgFiles[2], 3);
    MI_CpuCopy8(name, mBgFiles[3], 3);
    MI_CpuCopy8(name, mBgFiles[4], 3);
    MI_CpuCopy8(name, mBgPaletteFile, 3);
    MI_CpuCopy8(name, mObjCellFile, 3);
    MI_CpuCopy8(name, mObjCharFile, 3);
    MI_CpuCopy8(name, mObjPaletteFile, 3);
}

void func_02083078(SoftKeyboard *keyboard, int plane, int palette) {
    func_020830bc(keyboard, plane);
    func_02083128(keyboard, (KeyCallback)func_02083d58, keyboard);
    func_020831a4(keyboard);
    func_020831d8(keyboard);
    func_02083204(keyboard, palette);
}

void func_020830bc(SoftKeyboard *keyboard, int plane) {
    func_02081d68(keyboard->mUnknown080C, keyboard->mCharData, keyboard->mBgFiles);
    keyboard->mPlane = plane;
    gGraphicsManager[6] |= plane;
    func_0202543c(gGraphicsManager, plane, -1, 0, 0, 0, 0, 0);
}

void func_02083128(SoftKeyboard *keyboard, KeyCallback callback, void *userData) {
    func_02081df0(45, 59, 0, callback, userData);
    func_02081cf8(keyboard->mScreen, keyboard->mPalette, 0);
    func_02082e70(sTabAreas, 2, NULL, NULL);
    int plane = keyboard->mPlane;
    func_02028108(&data_02121834, plane, -45);
    func_02028154(&data_02121834, plane, -59);
}

void func_020831a4(SoftKeyboard *keyboard) {
    void *data = func_0208265c();
    VramTransferQueue_AddCharData(gVramTransferQueue, data, keyboard->mPlane, 0, func_02082680());
}

void func_020831d8(SoftKeyboard *keyboard) {
    func_02027f3c(&data_02121834, keyboard->mScreen, keyboard->mPlane, 0, 0x800);
}

void func_02083204(SoftKeyboard *keyboard, int palette) {
    func_02027534(&data_021213b8, keyboard->mBgPaletteFile, keyboard->mPlane, palette, keyboard->mPalette * 32, 32);
}

void func_02083244(SoftKeyboard *keyboard, int a, TPData tp) {
    func_02082068(a, tp);
}

void func_02083288(SoftKeyboard *keyboard, int mode, BOOL b) {
    func_02081ff0(mode, b);
}

KeyboardButton::KeyboardButton() {
    mSoundHandle = 0;
    mTouchSound = -1;
    mReleaseSound = -1;
    mText.mSprite.mVisible = FALSE;
    mIndex = 0;
}

void func_020832d0(KeyboardButton *button, s16 index) {
    button->mUnknownAC = FALSE;
    button->mUnknownB0 = FALSE;
    button->mIndex = index;
}

void func_020832e4(KeyboardButton *button) {
    button->mUnknownAC = FALSE;
    button->mUnknownB0 = FALSE;
    Sprite_SetFrame(button, 0);
    NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&button->mAnim));
}

BOOL KeyboardButton::Update() {
    if (!mVisible) {
        return FALSE;
    }
    if (gTouchInput.mTouched && !mUnknownB0) {
        if (Contains(gTouchInput.mPoint)) {
            mUnknownAC = TRUE;
            mUnknownB0 = TRUE;
            Sprite_SetPlaying(this, TRUE);
            if (mTouchSound != -1) {
                gSoundManager.StartSeqArc(mSoundHandle, mSoundSeqArc, mTouchSound);
            }
        }
    } else if (gTouchInput.mUnknown4C && mUnknownB0) {
        mUnknownAC = FALSE;
        if (Contains(gTouchInput.mLastPoint)) {
            NNS_G2dSetAnimCtrlPlayModeOverridden(NNS_G2dGetCellAnimationAnimCtrl(&mAnim),
                                                 NNS_G2D_ANIMATIONPLAYMODE_FORWARD);
            Sprite_SetPlaying(this, TRUE);
            if (mReleaseSound != -1) {
                gSoundManager.StartSeqArc(mSoundHandle, mSoundSeqArc, mReleaseSound);
            }
            return TRUE;
        }
        func_020832d0(this, mIndex);
    } else if (gTouchInput.mTouching && mUnknownB0 && mUnknownAC) {
        if (Contains(gTouchInput.mPoint)) {
            Sprite_SetPlaying(this, TRUE);
        } else {
            Sprite_SetFrame(this, 0);
            NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&mAnim));
        }
    }
    return FALSE;
}

void KeyboardButton::Draw() {
    if (NNS_G2dGetAnimCtrlCurrentFrame(NNS_G2dGetCellAnimationAnimCtrl(&mAnim)) == 0) {
        mText.mSprite.mPosition.y = (sButtonPositions[mIndex].y - mText.mCanvas.areaHeight * 4) << FX32_SHIFT;
    } else {
        mText.mSprite.mPosition.y = (sButtonPositions[mIndex].y - mText.mCanvas.areaHeight * 4 + 2) << FX32_SHIFT;
    }
    mText.ObjTextCanvas::Draw();
    Sprite_Draw(this);
}

int KeyboardButton::InitText(MessageFile *file, int charOffset) {
    mText.InitObjForMessage(file, 6 - mIndex, &gFonts[2].mFont, 0x1000, charOffset, 0, 1, 0, 0, 15, NULL);
    mText.mHAlign = 1;
    mText.mVAlign = 1;
    int width = mText.mCanvas.areaWidth;
    int height = mText.mCanvas.areaHeight;
    int count = mText.GetCharCount();
    mText.SetPositionFromVec((sButtonPositions[mIndex].x - (width * 8 >> 1)) << FX32_SHIFT,
                             (sButtonPositions[mIndex].y - (height * 8 >> 1)) << FX32_SHIFT);
    mText.mSprite.mVisible = TRUE;
    mText.mColor = 8;
    mText.mShadowDirections = 0xAA;
    mText.DrawAll();
    return charOffset + count;
}

void KeyboardButton::SetVisible(BOOL visible) {
    mText.mSprite.mVisible = visible;
    mVisible = visible;
}

BOOL KeyboardButton::Contains(ScreenPoint point) {
    ScreenRect rect = sButtonRect;
    rect.x += mPosition.x >> FX32_SHIFT;
    rect.y += mPosition.y >> FX32_SHIFT;
    return func_02028d88(&rect, &point) ? TRUE : FALSE;
}

void KeyboardButton::SetSounds(int handle, int seqArc, int touchSound, int releaseSound) {
    mSoundHandle = handle;
    mSoundSeqArc = seqArc;
    mTouchSound = touchSound;
    mReleaseSound = releaseSound;
}

void KeyboardButton::SetMessage(u32 message) {
    mText.mMessage = message;
    NNS_G2dCharCanvasClear(&mText.mCanvas, 0);
    mText.DrawAll();
}

void SoftKeyboard::SetButtonSounds(int button, int handle, int seqArc, int touchSound, int releaseSound) {
    mButtons[button].SetSounds(handle, seqArc, touchSound, releaseSound);
}

int SoftKeyboard::InitButtonTexts(MessageFile *file, int charOffset) {
    for (int i = 0; i < 2; i++) {
        charOffset = mButtons[i].InitText(file, charOffset);
    }
    return charOffset;
}

int SoftKeyboard::GetButtonCharCount() {
    int count = 0;
    for (int i = 0; i < 2; i++) {
        count += mButtons[i].GetCharCount();
    }
    return count;
}

void func_020837a8(SoftKeyboard *keyboard) {
    const ScreenPoint &position = sLayoutPositions[keyboard->mLayout];
    keyboard->mSprite5EE8.SetPositionFromVec(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
}

void func_020837f4(SoftKeyboard *keyboard, s16 resource, int a, int palette) {
    func_02083868(keyboard, resource);
    func_02083874(keyboard, a);
    func_020838b4(keyboard, palette);
    func_020838f4(keyboard);
}

void func_0208382c(SoftKeyboard *keyboard, s16 resource, MessageFile *file, int charOffset, int a, s16 layout,
                   int palette) {
    keyboard->mLayout = layout;
    func_020837f4(keyboard, resource, a, palette);
    keyboard->InitButtonTexts(file, charOffset);
}

void func_02083868(SoftKeyboard *keyboard, s16 resource) {
    keyboard->mObjResource = resource;
}

void func_02083874(SoftKeyboard *keyboard, int a) {
    ObjResourceManager_Load(&gObjResourceManager, keyboard->mObjCellFile, keyboard->mObjCharFile, 0x1000,
                            keyboard->mObjResource, a);
}

void func_020838b4(SoftKeyboard *keyboard, int palette) {
    func_02027534(&data_021213b8, keyboard->mObjPaletteFile, 0x1000, palette * 32, 0x1A0, 32);
}

void func_020838f4(SoftKeyboard *keyboard) {
    Sprite_Init(&keyboard->mTabs[0], keyboard->mObjResource, 1, sTabAreas[0].x + 16, sTabAreas[0].y + 8, TRUE);
    Sprite_Init(&keyboard->mTabs[1], keyboard->mObjResource, 0, sTabAreas[1].x + 16, sTabAreas[1].y + 8, TRUE);
    Sprite_SetFrame(&keyboard->mTabs[0], 1);
    NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&keyboard->mTabs[1].mAnim));
    Sprite_Init(&keyboard->mTabs[2], keyboard->mObjResource, 2, sTabAreas[2].x + 16, sTabAreas[2].y + 8, TRUE);
    NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&keyboard->mTabs[2].mAnim));
    Sprite_Init(&keyboard->mTabs[3], keyboard->mObjResource, 3, sTabAreas[3].x + 16, sTabAreas[3].y + 8, TRUE);
    NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&keyboard->mTabs[3].mAnim));
    Sprite_Init(&keyboard->mTabs[4], keyboard->mObjResource, 4, sTabAreas[4].x + 16, sTabAreas[4].y + 8, TRUE);
    NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&keyboard->mTabs[4].mAnim));

    Sprite_Init(&keyboard->mButtons[0], keyboard->mObjResource, 5, 0x80, 0x60, TRUE);
    NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&keyboard->mButtons[0].mAnim));
    keyboard->mButtons[0].SetPositionFromVec(sButtonPositions[0].x << FX32_SHIFT, sButtonPositions[0].y << FX32_SHIFT);
    func_020832d0(&keyboard->mButtons[0], 0);
    Sprite_Init(&keyboard->mButtons[1], keyboard->mObjResource, 5, 0x80, 0x60, TRUE);
    NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&keyboard->mButtons[1].mAnim));
    keyboard->mButtons[1].SetPositionFromVec(sButtonPositions[1].x << FX32_SHIFT, sButtonPositions[1].y << FX32_SHIFT);
    func_020832d0(&keyboard->mButtons[1], 1);

    Sprite_Init(&keyboard->mSprite5EE8, keyboard->mObjResource, 7, 0x80, 0x60, TRUE);
    const ScreenPoint &position = sLayoutPositions[keyboard->mLayout];
    keyboard->mSprite5EE8.SetPositionFromVec(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
}

void SoftKeyboard::Draw() {
    for (int i = 0; i < 5; i++) {
        Sprite_Draw(&mTabs[i]);
    }
    for (int i = 0; i < 2; i++) {
        mButtons[i].Draw();
    }
    Sprite_Draw(&mSprite5EE8);
}

void func_02083c30(SoftKeyboard *keyboard) {
    func_02082e70(sTabAreas, 5, (AreaCallback)func_02083c54, keyboard);
}

void func_02083c54(SoftKeyboard *keyboard, int event, int a, int code) {
    Sprite *tabs = keyboard->mTabs;
    const int modes[5] = {0, 1, 2, 3, 4};
    switch (event) {
    case 0:
    case 2:
    case 3:
    case 4:
        break;
    case 1: {
        for (int i = 0; i < 5; i++) {
            Sprite_SetFrame(&tabs[i], 0);
            NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&tabs[i].mAnim));
        }
        int tab = -1;
        if (code == 0x100) {
            tab = 0;
        } else if (code == 0x200) {
            tab = 1;
        } else if (code == 0x300) {
            tab = 2;
        } else if (code == 0x400) {
            tab = 3;
        } else if (code == 0x500) {
            tab = 4;
        }
        if (tab < 0) {
            return;
        }
        Sprite_SetFrame(&tabs[tab], 1);
        Sprite_SetPlaying(&tabs[tab], TRUE);
        func_02081ff0(modes[tab], TRUE);
        break;
    }
    }
}

void func_02083d58(SoftKeyboard *keyboard, int event) {
    switch (event) {
    case 0:
        gSoundManager.StartSeqArc(keyboard->mSoundHandle, 2, 1);
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
        gSoundManager.StartSeqArc(keyboard->mSoundHandle, 2, 0);
        break;
    case 9:
        break;
    }
}

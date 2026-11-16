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
    /* 0x4 */ u16 link; // The next key of the same group (none for the tabs)
    /* 0x6 */ u16 code;
};

typedef void (*AreaCallback)(void *userData, int action, int pressedCode, int touchedCode);

extern "C" {
void KeyboardCore_MakeScreen(u16 *screen, int palette, int charOffset);
void KeyboardCore_InitGraphics(void *baseCharData, void *charData, char (*files)[0x30]);
void KeyboardCore_Init(int x, int y, BOOL extraKeys, KeyboardSoundCallback callback, void *userData);
void KeyboardCore_SetMode(int mode, BOOL playSound);
void KeyboardCore_Update(int pad, TPData tp);
void *KeyboardCore_GetCharData();
int KeyboardCore_GetCharDataSize();
void KeyboardCore_SetAreas(const KeyArea *areas, int count, AreaCallback callback, void *userData);
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
// Where mLayoutSprite is, by mLayout
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

void SoftKeyboard::InitBg(int plane, int paletteOffset) {
    InitBgGraphics(plane);
    InitCore((KeyboardSoundCallback)PlayKeySound, this);
    TransferCharData();
    TransferScreen();
    LoadBgPalette(paletteOffset);
}

void SoftKeyboard::InitBgGraphics(int plane) {
    KeyboardCore_InitGraphics(mBaseCharData, mCharData, mBgFiles);
    mPlane = plane;
    gGraphicsManager[6] |= plane;
    func_0202543c(gGraphicsManager, plane, -1, 0, 0, 0, 0, 0);
}

void SoftKeyboard::InitCore(KeyboardSoundCallback callback, void *userData) {
    KeyboardCore_Init(45, 59, 0, callback, userData);
    KeyboardCore_MakeScreen(mScreen, mPalette, 0);
    KeyboardCore_SetAreas(sTabAreas, 2, NULL, NULL);
    int plane = mPlane;
    func_02028108(&data_02121834, plane, -45);
    func_02028154(&data_02121834, plane, -59);
}

void SoftKeyboard::TransferCharData() {
    void *data = KeyboardCore_GetCharData();
    VramTransferQueue_AddCharData(gVramTransferQueue, data, mPlane, 0, KeyboardCore_GetCharDataSize());
}

void SoftKeyboard::TransferScreen() {
    func_02027f3c(&data_02121834, mScreen, mPlane, 0, 0x800);
}

void SoftKeyboard::LoadBgPalette(int offset) {
    func_02027534(&data_021213b8, mBgPaletteFile, mPlane, offset, mPalette * 32, 32);
}

void SoftKeyboard::Update(int pad, TPData tp) {
    KeyboardCore_Update(pad, tp);
}

void SoftKeyboard::SetMode(int mode, BOOL playSound) {
    KeyboardCore_SetMode(mode, playSound);
}

KeyboardButton::KeyboardButton() {
    mSoundHandle = 0;
    mTouchSound = -1;
    mReleaseSound = -1;
    mText.mSprite.mVisible = FALSE;
    mIndex = 0;
}

void KeyboardButton::Reset(s16 index) {
    mHeld = FALSE;
    mPressed = FALSE;
    mIndex = index;
}

void KeyboardButton::Release() {
    mHeld = FALSE;
    mPressed = FALSE;
    Sprite_SetFrame(this, 0);
    NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&mAnim));
}

BOOL KeyboardButton::Update() {
    if (!mVisible) {
        return FALSE;
    }
    if (gTouchInput.mTouched && !mPressed) {
        if (Contains(gTouchInput.mPoint)) {
            mHeld = TRUE;
            mPressed = TRUE;
            Sprite_SetPlaying(this, TRUE);
            if (mTouchSound != -1) {
                gSoundManager.StartSeqArc(mSoundHandle, mSoundSeqArc, mTouchSound);
            }
        }
    } else if (gTouchInput.mUnknown4C && mPressed) {
        mHeld = FALSE;
        if (Contains(gTouchInput.mLastPoint)) {
            NNS_G2dSetAnimCtrlPlayModeOverridden(NNS_G2dGetCellAnimationAnimCtrl(&mAnim),
                                                 NNS_G2D_ANIMATIONPLAYMODE_FORWARD);
            Sprite_SetPlaying(this, TRUE);
            if (mReleaseSound != -1) {
                gSoundManager.StartSeqArc(mSoundHandle, mSoundSeqArc, mReleaseSound);
            }
            return TRUE;
        }
        Reset(mIndex);
    } else if (gTouchInput.mTouching && mPressed && mHeld) {
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

void SoftKeyboard::PlaceLayoutSprite() {
    const ScreenPoint &position = sLayoutPositions[mLayout];
    mLayoutSprite.SetPositionFromVec(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
}

void SoftKeyboard::LoadObj(s16 resource, int a, int palette) {
    SetObjResource(resource);
    LoadObjCells(a);
    LoadObjPalette(palette);
    InitSprites();
}

void SoftKeyboard::InitObj(s16 resource, MessageFile *file, int charOffset, int a, s16 layout, int palette) {
    mLayout = layout;
    LoadObj(resource, a, palette);
    InitButtonTexts(file, charOffset);
}

void SoftKeyboard::SetObjResource(s16 resource) {
    mObjResource = resource;
}

void SoftKeyboard::LoadObjCells(int a) {
    ObjResourceManager_Load(&gObjResourceManager, mObjCellFile, mObjCharFile, 0x1000, mObjResource, a);
}

void SoftKeyboard::LoadObjPalette(int palette) {
    func_02027534(&data_021213b8, mObjPaletteFile, 0x1000, palette * 32, 0x1A0, 32);
}

void SoftKeyboard::InitSprites() {
    Sprite_Init(&mTabs[0], mObjResource, 1, sTabAreas[0].x + 16, sTabAreas[0].y + 8, TRUE);
    Sprite_Init(&mTabs[1], mObjResource, 0, sTabAreas[1].x + 16, sTabAreas[1].y + 8, TRUE);
    Sprite_SetFrame(&mTabs[0], 1);
    NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&mTabs[1].mAnim));
    Sprite_Init(&mTabs[2], mObjResource, 2, sTabAreas[2].x + 16, sTabAreas[2].y + 8, TRUE);
    NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&mTabs[2].mAnim));
    Sprite_Init(&mTabs[3], mObjResource, 3, sTabAreas[3].x + 16, sTabAreas[3].y + 8, TRUE);
    NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&mTabs[3].mAnim));
    Sprite_Init(&mTabs[4], mObjResource, 4, sTabAreas[4].x + 16, sTabAreas[4].y + 8, TRUE);
    NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&mTabs[4].mAnim));

    Sprite_Init(&mButtons[0], mObjResource, 5, 0x80, 0x60, TRUE);
    NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&mButtons[0].mAnim));
    mButtons[0].SetPositionFromVec(sButtonPositions[0].x << FX32_SHIFT, sButtonPositions[0].y << FX32_SHIFT);
    mButtons[0].Reset(0);
    Sprite_Init(&mButtons[1], mObjResource, 5, 0x80, 0x60, TRUE);
    NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&mButtons[1].mAnim));
    mButtons[1].SetPositionFromVec(sButtonPositions[1].x << FX32_SHIFT, sButtonPositions[1].y << FX32_SHIFT);
    mButtons[1].Reset(1);

    Sprite_Init(&mLayoutSprite, mObjResource, 7, 0x80, 0x60, TRUE);
    const ScreenPoint &position = sLayoutPositions[mLayout];
    mLayoutSprite.SetPositionFromVec(position.x << FX32_SHIFT, position.y << FX32_SHIFT);
}

void SoftKeyboard::Draw() {
    for (int i = 0; i < 5; i++) {
        Sprite_Draw(&mTabs[i]);
    }
    for (int i = 0; i < 2; i++) {
        mButtons[i].Draw();
    }
    Sprite_Draw(&mLayoutSprite);
}

void SoftKeyboard::EnableTabs() {
    KeyboardCore_SetAreas(sTabAreas, 5, (AreaCallback)OnTabArea, this);
}

// action: 1 an area is pressed, 3 the pen is lifted, 4 the pen left the pressed area
void SoftKeyboard::OnTabArea(SoftKeyboard *keyboard, int action, int pressedCode, int touchedCode) {
    Sprite *tabs = keyboard->mTabs;
    const int modes[5] = {0, 1, 2, 3, 4};
    switch (action) {
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
        if (touchedCode == 0x100) {
            tab = 0;
        } else if (touchedCode == 0x200) {
            tab = 1;
        } else if (touchedCode == 0x300) {
            tab = 2;
        } else if (touchedCode == 0x400) {
            tab = 3;
        } else if (touchedCode == 0x500) {
            tab = 4;
        }
        if (tab < 0) {
            return;
        }
        Sprite_SetFrame(&tabs[tab], 1);
        Sprite_SetPlaying(&tabs[tab], TRUE);
        KeyboardCore_SetMode(modes[tab], TRUE);
        break;
    }
    }
}

void SoftKeyboard::PlayKeySound(SoftKeyboard *keyboard, int sound) {
    switch (sound) {
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

#ifndef OV076_MIIEDITSCENE_H
#define OV076_MIIEDITSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "Character.h"
#include "MessageWindow.h"
#include "Scene.h"
#include "Sprite.h"
#include "SpriteButton.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"
#include "voice/TtsPlayer.h"

// --- What the scene uses of the main program (STAND-INS, not decompiled) ----------------------------------------

// The main program's camera (data_02122764; func_0202a640 resets it). As overlay 109 has it.
struct Camera {
    /* 0x00 */ u8 mUnknown00[0x14];
    /* 0x14 */ VecFx32 mPosition;
    /* 0x20 */ VecFx32 mUnknown20;
    /* 0x2C */ VecFx32 mTarget;
};

// The screens: which planes are shown (data_0211d604).
struct ScreenControl {
    /* 0x00 */ u32 mPlanes;
    /* 0x04 */ u8 mUnknown04[0xD0];
    /* 0xD4 */ BOOL mPlanesChanged;
};

// Graphics resources, loaded by file name into VRAM.
struct ArchiveLoader;
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;

extern "C" {
extern ArchiveLoader gArchiveManager;
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void *ArchiveManager_GetFileByName(ArchiveLoader *loader, const char *path);
extern ScreenControl data_0211d604;
extern CellLoader gObjResourceManager;
extern PaletteLoader data_021213b8;
extern ScreenLoader data_02121834;
extern CharLoader gVramTransferQueue;
void *func_02027534(PaletteLoader *loader, const char *name, int plane, int a, int b, int c); // Loads a palette
u16 *func_02027d7c(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028280(ScreenLoader *loader, int plane, int priority);
void func_02028108(ScreenLoader *loader, int plane, int offset);
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
}

// The on-screen keyboard of the main program (func_02082eac constructs it, 0x6014 bytes)
struct SoftKeyboard {
    /* 0x0000 */ u8 mUnknown0000[0x5178];
    /* 0x5178 */ u8 mUnknown5178[2][0x6B8]; // Given to func_02083614 with the point touched
    /* 0x5EE8 */ u8 mUnknown5EE8[0xA4];
    /* 0x5F8C */ fx32 mUnknown5F8C;          // Moves 13 pixels (0xD000) with each character typed
    /* 0x5F90 */ u8 mUnknown5F90[0x68];
    /* 0x5FF8 */ wchar_t mText[12];          // What is typed (the profile page writes it into the character)
    /* 0x6010 */ int mLength;                // and how many characters, up to 10
};

extern "C" {
void func_02083bcc(SoftKeyboard *keyboard); // Draws it
}

// The particle system of the main program (0x0207ab20 to 0x0207b364)
struct ParticleSystem;

// A Mii's data: the first 0x4A bytes of a saved character, copied as a structure
struct MiiData {
    /* 0x00 */ u8 mBytes[0x4A];
};

// --- The scene's pages -------------------------------------------------------------------------------------------

// The scene shows one page at a time (MiiEditScene::mPage); Draw and Exit pass on to the page's own. Each is a part
// of the scene, constructed in place by the scene's constructor (the main program's func_0204332c). What each
// page is, is not established yet: they are numbered by the value of mPage. Their sizes are measured: they follow
// one another in the scene.

class MiiEditScene;

// A sprite with something after it, 0x100 bytes (the face page and the voice page draw arrays of them)
struct MiiEditSprite100 {
    /* 0x00 */ Sprite mSprite;
    /* 0xAC */ u8 mUnknownAC[0x54];
}; // 0x100

class MiiEditPage0 {
public:
    void Draw();
    void Exit();

    /* 0x00000 */ u8 mUnknown00000[0x10];
    /* 0x00010 */ MessageWindow mWindows[2];
}; // 0x15F58

class MiiEditFacePage;

// Two tabs of the face page (its buttons 8 and 9, and 10 to 13, open them): the face's shape and make-up, which
// shows "Face" graphics and Mii parameters 0x23 to 0x25 (the face's type, colour and texture), and the other parts,
// which shows "Etc" graphics: glasses (parameters 0x1F, 0x20), mustache (0x12, 0x11), mole (0x15) and beard (0x10,
// 0x11).
class MiiEditFaceTab {
public:
    void Init(MiiEditFacePage *page);
    void func_ov076_0214faac();
    void SelectTab(int tab, BOOL pressed);

    /* 0x0 */ int mTab;
    /* 0x4 */ int mUnknown4;
    /* 0x8 */ MiiEditFacePage *mPage;
}; // 0xC

class MiiEditEtcTab {
public:
    void Init(MiiEditFacePage *page);
    void SelectTab(int tab, BOOL pressed);

    /* 0x0 */ int mTab;
    /* 0x4 */ int mUnknown4;
    /* 0x8 */ MiiEditFacePage *mPage;
}; // 0xC

// The face page: its Init and the functions after it load the parts' graphics (Face, Hair, Eyebrow, Eye, Nose,
// Mouth, Etc, with the colour palettes Fw_color and Fw_parts)
class MiiEditFacePage {
public:
    void Draw();
    void Exit();
    void func_ov076_0214ee7c(int selected, int category, int tab);
    void func_ov076_0214ef88(int page, u32 count, int category);
    int GetPartType(int button, int category);
    int GetPartButton(int type, int category);
    void func_ov076_0214f264(int parameter, int selected, int page, u16 count);
    void SetParameter(int parameter, int value);
    BOOL func_ov076_0214f930(int parameter, int step, BOOL wrap);
    void func_ov076_0214fa14(int mode);

    /* 0x0000 */ u8 mUnknown0000[0x20];
    /* 0x0020 */ int mCategory; // Which part the parts' buttons show (GetPartType's category)
    /* 0x0024 */ u8 mUnknown0024[4];
    /* 0x0028 */ MiiEditScene *mScene;
    /* 0x002C */ SpriteButton mButtons[36];
    /* 0x227C */ SpriteButton mButtons227C[12]; // The parts to choose from
    /* 0x2DEC */ int mUnknown2DEC;              // SetParameter keeps the value a part's type had before it
    /* 0x2DF0 */ int mUnknown2DF0;              // and which parameter it was
    /* 0x2DF4 */ MiiEditSprite100 mSprites2DF4[2];
    /* 0x2FF4 */ Sprite mSprites2FF4[2];
    /* 0x314C */ u8 mUnknown314C[0x10];
    /* 0x315C */ Sprite mSprite315C;
    /* 0x3208 */ Sprite mSprite3208;
    /* 0x32B4 */ Sprite mSprite32B4;
    /* 0x3360 */ Sprite mSprite3360;
    /* 0x340C */ int mUnknown340C; // func_ov076_0214fa14's mode
    /* 0x3410 */ MiiEditFaceTab mFaceTab;
    /* 0x341C */ MiiEditEtcTab mEtcTab;
}; // 0x3428

// The profile page, with the on-screen keyboard ("Profile" graphics)
class MiiEditProfilePage {
public:
    void Draw();
    void Exit();
    void func_ov076_02150874(MiiEditScene *scene);
    void func_ov076_021508ac();
    void func_ov076_02151f0c(int state);

    /* 0x00000 */ int mState;
    /* 0x00004 */ int mUnknown00004;
    /* 0x00008 */ MiiEditScene *mScene;
    /* 0x0000C */ MessageWindow mWindows[6];
    /* 0x41DE4 */ SpriteButton mButtons[10];
    /* 0x4276C */ Sprite mSprites[2];
    /* 0x428C4 */ SoftKeyboard mKeyboard;
    /* 0x488D8 */ SpriteButton mButton;
}; // 0x489CC

// The voice page ("Voice" graphics)
class MiiEditVoicePage {
public:
    void Draw();
    void Exit();
    void func_ov076_02157508(int a);
    void func_ov076_02157bc0();

    /* 0x0000 */ int mState;
    /* 0x0004 */ int mUnknown0004;
    /* 0x0008 */ MiiEditScene *mScene;
    /* 0x000C */ SpriteButton mButtons[22];
    /* 0x1504 */ TextButton mTextButtons[3];
    /* 0x29D4 */ Sprite mSprites29D4[2];
    /* 0x2B2C */ MiiEditSprite100 mSprites2B2C[5];
    /* 0x302C */ u8 mUnknown302C[0x53C];
    /* 0x3568 */ MessageText mText;
    /* 0x3B58 */ u8 mUnknown3B58[0x2A8];
}; // 0x3E00

// The page of the character's personality ("Character" graphics)
class MiiEditCharacterPage {
public:
    void Draw();
    void Exit();

    /* 0x000 */ u8 mUnknown000[0x4];
    /* 0x004 */ SpriteButton mButtons[5];
    /* 0x4C8 */ Sprite mSprites[5];
    /* 0x824 */
}; // 0x824

class MiiEditPage5 {
public:
    void Draw();
    void Exit();

    /* 0x0000 */ u8 mUnknown0000[0xC];
    /* 0x000C */ MessageWindow mWindow;
}; // 0xAFB0

class MiiEditPage6 {
public:
    void Draw();
    void Exit();

    /* 0x0000 */ u8 mUnknown0000[0xC];
    /* 0x000C */ MessageWindow mWindow;
}; // 0xAFB0

// --- The scene ----------------------------------------------------------------------------------------------------

// Overlay 76, the Mii maker: scenes 0x42, 0x43, 0x44 and 0x6F. The scene factory (main CreateScene, cases 66 to 68
// and 111) loads overlay 76 (0x4C) and allocates 0x983F8 bytes; the constructor is the main program's
// (func_0204332c), which stores the overlay's virtual table (0x0215A120). The layout is measured from that
// constructor (what it constructs where, and how many) and from the scene's destructor.
//
// The name is descriptive, not the original: it is after the scene's archive, "Scn/Scene/SceneMiiEdit2_LZ.bin".
class MiiEditScene : public Scene {
public:
    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    // The camera moves to mCameraGoal in 40 frames, eased by t^4 (as overlay 72's MapCamera), looking 8 units
    // ahead along z
    void ResetCameraGoal();
    void UpdateCamera();
    void MoveCameraTo(VecFx32 goal);
    void func_ov076_0214bbe4();

    /* 0x00010 */ u8 mUnknown00010[0x140];      // Init gives +0x10 to func_02024b10
    /* 0x00150 */ int mPage;                    // Which page Draw and Exit pass on to (0 to 6); Init sets -1
    /* 0x00154 */ u8 mUnknown00154[4];
    /* 0x00158 */ SpriteButton mButtons[6];
    /* 0x00710 */ SceneCharacter mCharacter;    // The Mii shown
    /* 0x03228 */ CharacterBase mEditedCharacter;
    /* 0x0580C */ BOOL mUnknown580C;
    /* 0x05810 */ Camera *mCamera;              // data_02122764
    /* 0x05814 */ int mCameraFrame;             // UpdateCamera moves the camera while it is under 40
    /* 0x05818 */ VecFx32 mCameraGoal;
    /* 0x05824 */ u8 mUnknown5824[8];
    /* 0x0582C */ Sprite mSprite582C;
    /* 0x058D8 */ u8 mUnknown58D8[4];
    /* 0x058DC */ BOOL mUnknown58DC;            // Draw shows mSprite582C while these two are set and 58E4 is not
    /* 0x058E0 */ BOOL mUnknown58E0;
    /* 0x058E4 */ BOOL mUnknown58E4;
    /* 0x058E8 */ u8 mUnknown58E8[0x20];
    /* 0x05908 */ ParticleSystem *mParticles;   // func_0207ab20 makes it, from "SPL:/SplEditEffect.spa"
    /* 0x0590C */ u8 mUnknown590C[4];
    /* 0x05910 */ void *mParticleFile;
    /* 0x05914 */ u8 mUnknown5914[0xC];
    /* 0x05920 */ TtsPlayer mTtsPlayer;         // The voice is tried out with it
    /*         */ u8 mUnknownAfterTts[0x1A368 - 0x5920 - sizeof(TtsPlayer)];
    /* 0x1A368 */ ObjTextBox mTextBoxes[9];
    /* 0x1B304 */ u8 mUnknown1B304[0x58];
    /* 0x1B35C */ MessageText mText1B35C;
    /* 0x1B94C */ MessageText mText1B94C;
    /* 0x1BF3C */ ObjCharBox mCharBox;
    /* 0x1C0F8 */ MessageFile mMessageFiles[2]; // Etc messages ("TBM"), common messages ("T02")
    /* 0x1C128 */ MiiEditPage0 mPage0;
    /* 0x32080 */ MiiEditFacePage mFacePage;
    /* 0x354A8 */ MiiEditProfilePage mProfilePage;
    /* 0x7DE74 */ MiiEditVoicePage mVoicePage;
    /* 0x81C74 */ MiiEditCharacterPage mCharacterPage;
    /* 0x82498 */ MiiEditPage6 mPage6;
    /* 0x8D448 */ MiiEditPage5 mPage5;
}; // 0x983F8

#endif // OV076_MIIEDITSCENE_H

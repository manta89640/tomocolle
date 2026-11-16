#ifndef OV076_MIIEDITSCENE_H
#define OV076_MIIEDITSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "Model.h"
#include "character/SceneCharacter.h"
#include "keyboard/SoftKeyboard.h"
#include "MessageWindow.h"
#include "Scene.h"
#include "Sprite.h"
#include "SpriteButton.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/TextCanvas.h"
#include "voice/TtsPlayer.h"

// The main program's camera and its particle system (MainProgram.h)
struct Camera;
struct ParticleSystem;
struct ParticleEmitter;

// --- The scene's pages -------------------------------------------------------------------------------------------

// The scene shows one page at a time (MiiEditScene::mPage); Draw and Exit pass on to the page's own. Each is a part
// of the scene, constructed in place by the scene's constructor (the main program's func_0204332c). What each
// page is, is not established yet: they are numbered by the value of mPage. Their sizes are measured: they follow
// one another in the scene.

class MiiEditScene;

// A sprite that is put at one of five places in a row on the lower screen, under the gaps between the page buttons
// (the scene's mProgressSprite): it shows how far the making has come, as AdvanceTo only moves it on to a higher place
struct MiiEditProgressSprite : public Sprite {
    void SetPlace(int place);
    void AdvanceTo(int place);

    /* 0xAC */ int mPlace;
}; // 0xB0

// A sprite with something after it, 0x100 bytes (the face page and the voice page draw arrays of them)
struct MiiEditSprite100 {
    /* 0x00 */ Sprite mSprite;
    /* 0xAC */ u8 mUnknownAC[0x10];
    /* 0xBC */ BOOL mUnknownBC; // The voice page reads the slider's value while it is set (func_020324e0)
    /* 0xC0 */ u8 mUnknownC0[8];
    /* 0xC8 */ BOOL mUnknownC8; // The voice page backs the record up while it is set
    /* 0xCC */ u8 mUnknownCC[0xC];
    /* 0xD8 */ int mUnknownD8;  // The voice page sets it to 1
    /* 0xDC */ u8 mUnknownDC[0x18];
    /* 0xF4 */ int mUnknownF4;  // The voice page sets it to 14
    /* 0xF8 */ u8 mUnknownF8[8];
}; // 0x100

// The texts of a character that the voice page speaks: func_0204e4b4 fills them from a record (the main program's)
struct CharacterSpeechTexts {
    /* 0x000 */ char mUnknown000[0x52];      // mUnknown068 in Shift JIS
    /* 0x052 */ wchar_t mUnknown052[11];     // The Mii's name (MiiData_GetName)
    /* 0x068 */ wchar_t mUnknown068[41];     // The same, through MakeSpeechTextInPlace
    /* 0x0BA */ wchar_t mUnknown0BA[41];     // CharacterRecord::mFamilyName, through MakeSpeechText
    /* 0x10C */ wchar_t mUnknown10C[41];     // CharacterRecord::mName, through MakeSpeechText
    /* 0x15E */ char mUnknown15E[0x82];      // mUnknown202 in Shift JIS
    /* 0x1E0 */ wchar_t mUnknown1E0[17];     // CharacterRecord::mSpeechDefault, or gText_MiNyuuRyoku when empty
    /* 0x202 */ wchar_t mUnknown202[65];     // mUnknown1E0 through MakeSpeechText
    /* 0x284 */ u8 mUnknown284[0xC];
    /* 0x290 */ BOOL mUnknown290;            // Whether mSpeechDefault was set
}; // 0x294

// Page 0: the sex choice (etc message 0x1A, "性別をえらんでください"), then whether to make the face (0x1B)
class MiiEditSexPage {
public:
    void Init(MiiEditScene *scene);
    void Update();
    void SetState(int state);
    void Draw();
    void Exit();

    /* 0x00000 */ int mState;
    /* 0x00004 */ int mUnknown00004;
    /* 0x00008 */ MiiEditScene *mScene;
    /* 0x0000C */ int mNextPage;            // The page Update goes on to
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
    void Update();
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
    void Init(MiiEditScene *scene);
    void Update();
    void ShowPart(int part, int a);
    void Draw();
    void Exit();
    void SelectColorButton(int selected, int category, int tab);
    void ShowPartButtons(int page, u32 count, int category);
    int GetPartType(int button, int category);
    int GetPartButton(int type, int category);
    void ChoosePart(int parameter, int selected, int page, u16 count);
    void SetParameter(int parameter, int value);
    BOOL StepParameter(int parameter, int step, BOOL wrap);
    void SetUndoMode(int mode);

    /* 0x0000 */ BOOL mUnknown0000[8]; // Set for each part as ShowPart shows it
    /* 0x0020 */ int mCategory; // Which part the parts' buttons show (GetPartType's category)
    /* 0x0024 */ int mUnknown0024;
    /* 0x0028 */ MiiEditScene *mScene;
    /* 0x002C */ SpriteButton mButtons[36];
    /* 0x227C */ SpriteButton mButtons227C[12]; // The parts to choose from
    /* 0x2DEC */ int mUndoValue;              // SetParameter keeps the value a part's type had before it
    /* 0x2DF0 */ int mUndoParameter;              // and which parameter it was
    /* 0x2DF4 */ MiiEditSprite100 mSprites2DF4[2];
    /* 0x2FF4 */ Sprite mSprites2FF4[2];
    /* 0x314C */ int mUnknown314C[4]; // Which page of parts 2, 3, 4 and 6 is shown (mSprite315C, 3208, 32B4, 3360 show it)
    /* 0x315C */ Sprite mSprite315C;
    /* 0x3208 */ Sprite mSprite3208;
    /* 0x32B4 */ Sprite mSprite32B4;
    /* 0x3360 */ Sprite mSprite3360;
    /* 0x340C */ int mUndoMode; // SetUndoMode's mode
    /* 0x3410 */ MiiEditFaceTab mFaceTab;
    /* 0x341C */ MiiEditEtcTab mEtcTab;
}; // 0x3428

// The profile page, with the on-screen keyboard ("Profile" graphics)
class MiiEditProfilePage {
public:
    void Init(MiiEditScene *scene);
    void Update();
    void Draw();
    void Exit();
    void SetText(const wchar_t *text);
    void SetNumberDigits(int value);
    void SetDayDigits(int day);
    void SetMonthDigits(int month);
    void UpdateTexts();
    void UpdateDayButton();
    void UpdateKeyboardKey(int state);
    void SetState(int state);

    /* 0x00000 */ int mState;
    /* 0x00004 */ int mUnknown00004;
    /* 0x00008 */ MiiEditScene *mScene;
    /* 0x0000C */ MessageWindow mWindows[6];
    /* 0x41DE4 */ SpriteButton mButtons[10];
    /* 0x4276C */ Sprite mSprites[2];
    /* 0x428C4 */ SoftKeyboard mKeyboard;
    /* 0x488BC */ wchar_t mText[11];     // What is typed: SetText shows it a character at a time in mScene->mCharBox
    /* 0x488D2 */ u8 mUnknown488D2[2];
    /* 0x488D4 */ int mTextLength;
    /* 0x488D8 */ SpriteButton mButton;
}; // 0x489CC

// The voice page ("Voice" graphics)
class MiiEditVoicePage {
public:
    void Init(MiiEditScene *scene);
    void Update();
    void SetState(int state);
    void Draw();
    void Exit();
    void ShowVoiceSettings();
    void RememberForUndo();
    void SpeakProfile(int a);
    void StartSpeaking();
    void StopSpeaking();
    int GetStarSignMessage(int month, int day);

    /* 0x0000 */ int mState;
    /* 0x0004 */ int mUnknown0004;
    /* 0x0008 */ MiiEditScene *mScene;
    /* 0x000C */ SpriteButton mButtons[22];
    /* 0x1504 */ TextButton mTextButtons[3];
    /* 0x29D4 */ Sprite mSprites29D4[2];
    /* 0x2B2C */ MiiEditSprite100 mSprites2B2C[5]; // The sliders of the five values of CharacterRecord::mVoice
    /* 0x302C */ CharacterRecord mUndoRecord;      // RememberForUndo copies the character's record in
    /* 0x32D0 */ int mSpeakStep;                    // Which part of the profile SpeakProfile speaks
    /* 0x32D4 */ CharacterSpeechTexts mSpeechTexts;
    /* 0x3568 */ MessageText mText;
    /* 0x3B58 */ u8 mUnknown3B58[4];
    /* 0x3B5C */ CharacterRecord mUnknown3B5C;      // The character's record as Init found it
}; // 0x3E00

// The page of the character's personality ("Character" graphics)
class MiiEditCharacterPage {
public:
    void Init(MiiEditScene *scene);
    void Update();
    void Draw();
    void Exit();

    /* 0x000 */ MiiEditScene *mScene;
    /* 0x004 */ SpriteButton mButtons[5]; // The sliders' knobs, one per value of CharacterRecord::mAnswers[0..4]
    /* 0x4C8 */ Sprite mSprites[5];
    /* 0x824 */
}; // 0x824

// Page 5: "保存しないで終了します。よろしいですか？" (etc message 0x2E: finish without saving?)
class MiiEditQuitPage {
public:
    void Init(MiiEditScene *scene, int previousPage);
    void Update();
    void Draw();
    void Exit();

    /* 0x0000 */ MiiEditScene *mScene;
    /* 0x0004 */ int mNextPage;     // Where it goes when the window has closed: -1 leaves the scene
    /* 0x0008 */ int mPreviousPage; // The page shown before this one, where the second button goes back to
    /* 0x000C */ MessageWindow mWindow[1]; // An array of one: the constructor builds it in a loop
}; // 0xAFB0

// Page 6: "登録します。よろしいですか？" (etc message 0x2D: register the Mii?); yes saves it (SaveCharacter)
class MiiEditRegisterPage {
public:
    void Init(MiiEditScene *scene, int previousPage);
    void Update();
    void Draw();
    void Exit();

    /* 0x0000 */ MiiEditScene *mScene;
    /* 0x0004 */ int mNextPage;     // Where it goes when the window has closed: -1 leaves the scene
    /* 0x0008 */ int mPreviousPage; // The page shown before this one, where the second button goes back to
    /* 0x000C */ MessageWindow mWindow[1]; // An array of one: the constructor builds it in a loop
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
    MiiEditScene(int sceneId); // The main program's (src/scene/SceneManager.cpp)

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
    void SetPage(int page); // Leaves the page shown (its Exit) and shows another
    void PlayEffectAtHead(int resource, fx32 offsetY);
    void LoadCharacter(int index);
    void LoadModel();
    void SaveCharacter();
    void SetCharacterColors(BOOL a);
    void ShowControlsWithWindow();
    void ShowControls();
    void ShowControlsEditFace();
    void HideControls();
    void HideControlsNoBar();
    int GetNameLength(const wchar_t *name);

    /* 0x00010 */ u8 mUnknown00010[0x140];      // Init gives +0x10 to func_02024b10
    /* 0x00150 */ int mPage;                    // Which page Draw and Exit pass on to (0 to 6); Init sets -1
    /* 0x00154 */ int mUnknown00154;           // SetPage clears it
    /* 0x00158 */ SpriteButton mButtons[6];
    /* 0x00710 */ SceneCharacter mCharacter;    // The Mii shown
    /* 0x03228 */ CharacterBase mEditedCharacter;
    /* 0x0580C */ BOOL mUnknown580C;
    /* 0x05810 */ Camera *mCamera;              // data_02122764
    /* 0x05814 */ int mCameraFrame;             // UpdateCamera moves the camera while it is under 40
    /* 0x05818 */ VecFx32 mCameraGoal;
    /* 0x05824 */ int mUnknown5824;            // The saved character's index (func_0202c264, func_0202c28c)
    /* 0x05828 */ BOOL mUnknown5828;           // While set, Update turns the character until its angle is 0x4000
    /* 0x0582C */ MiiEditProgressSprite mProgressSprite;
    /* 0x058DC */ BOOL mUnknown58DC;            // Draw shows mProgressSprite while these two are set and 58E4 is not
    /* 0x058E0 */ BOOL mUnknown58E0;
    /* 0x058E4 */ BOOL mUnknown58E4;
    /* 0x058E8 */ BOOL mUnknown58E8[7];        // Set for each page as SetPage shows it
    /* 0x05904 */ fx32 mUnknown5904;           // Update scrolls the lower screen's plane 4 by it
    /* 0x05908 */ ParticleSystem *mParticles;   // func_0207ab20 makes it, from "SPL:/SplEditEffect.spa"
    /* 0x0590C */ ParticleEmitter *mUnknown590C;
    /* 0x05910 */ void *mParticleFile;
    /* 0x05914 */ VecFx32 mUnknown5914;
    /* 0x05920 */ TtsPlayer mTtsPlayer;         // The voice is tried out with it
    /*         */ u8 mUnknownAfterTts[0x1A368 - 0x5920 - sizeof(TtsPlayer)];
    /* 0x1A368 */ ObjTextBox mTextBoxes[9];
    /* 0x1B304 */ u8 mUnknown1B304[0x28];
    /* 0x1B32C */ BOOL mUnknown1B32C;          // When set, Update redraws mTextBoxes and shows them
    /* 0x1B330 */ u8 mUnknown1B330[0x2C];
    /* 0x1B35C */ MessageText mText1B35C[1]; // Arrays of one: the constructor builds each in a loop
    /* 0x1B94C */ MessageText mText1B94C[1];
    /* 0x1BF3C */ ObjCharBox mCharBox;
    /* 0x1C0F8 */ MessageFile mMessageFiles[2]; // Etc messages ("TBM"), common messages ("T02")
    /* 0x1C128 */ MiiEditSexPage mSexPage;
    /* 0x32080 */ MiiEditFacePage mFacePage;
    /* 0x354A8 */ MiiEditProfilePage mProfilePage;
    /* 0x7DE74 */ MiiEditVoicePage mVoicePage;
    /* 0x81C74 */ MiiEditCharacterPage mCharacterPage;
    /* 0x82498 */ MiiEditRegisterPage mRegisterPage;
    /* 0x8D448 */ MiiEditQuitPage mQuitPage;
}; // 0x983F8

#endif // OV076_MIIEDITSCENE_H

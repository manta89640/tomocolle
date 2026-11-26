#ifndef OV095_AFFINITYSCENE_H
#define OV095_AFFINITYSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "Model.h"
#include "Scene.h"
#include "MessageWindow.h"
#include "Sprite.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "list/MiiList.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/SpeechWindow.h"
#include "voice/TtsPlayer.h"

#include "../../scene/FaceCast.h"

struct Camera;

// Overlay 95, scene 0x80: the affinity test ("Scn/Scene/SceneAffinity_LZ.bin"). The player picks two Miis from the
// list of residents on the touch screen, one after the other; the top screen shows their faces in a frame, thinks for
// a while ("Scn/Affn/affinity_think_*"), and gives their affinity as a percentage, with a picture of the result for
// two men ("_m_"), two women ("_f_") or a man and a woman ("_l_"), good, normal or bad. The results are kept in the
// saved game's two tables of pairs (src/list/MiiList.cpp), one for the same-sex pairs and one for the couples.
//
// The scene factory (CreateScene) loads overlay 95 and allocates 0x76A98 bytes; the constructor is the main program's
// (0x02046750, src/scene/SceneManager.cpp), which stores the overlay's virtual table (0x0214F8F8) and builds the
// members in place.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM.

// The top screen: the models of the frame and of the result, the two faces and the texts of the result. The
// scene's member at +0x583C4 (the end of its windows), constructed from that address.
class AffinityPart {
public:
    // What the top screen shows (SetMode)
    enum Mode {
        MODE_START,       // The frame, before a Mii is chosen
        MODE_FIRST_MAN,   // The first Mii chosen is a man: the left place
        MODE_FIRST_WOMAN, // A woman: the right place
        MODE_MEN,         // Two men
        MODE_WOMEN,       // Two women
        MODE_COUPLE,      // A man and a woman
        MODE_THINK_MEN,   // Thinking, for each kind of pair
        MODE_THINK_WOMEN,
        MODE_THINK_COUPLE,
        MODE_RESULT_MEN_GOOD, // The result: 66 and more, 33 and more, less
        MODE_RESULT_MEN_NORMAL,
        MODE_RESULT_MEN_BAD,
        MODE_RESULT_WOMEN_GOOD,
        MODE_RESULT_WOMEN_NORMAL,
        MODE_RESULT_WOMEN_BAD,
        MODE_RESULT_COUPLE_GOOD,
        MODE_RESULT_COUPLE_NORMAL,
        MODE_RESULT_COUPLE_BAD,
        MODE_RESULT_SHOWN, // A second after the result
    };

    void MountArchives();
    void Init(int *upperCharOffset, int *lowerCharOffset);
    void Update();
    void Draw();
    void Exit();
    void SetMode(int mode);
    void QueueSkinPalette(Model *model, int skin, int offset);
    void SetFirst(SavedCharacter *character, BOOL invalid);
    void SetSecond(SavedCharacter *character, BOOL invalid);
    void StartThinking();
    void ShowResult();
    void RecordPair();
    void RecordCouple();

    /* 0x00000 */ AnimModel mModels[2];
    /* 0x001C0 */ AnimModel mModel1C0;
    /* 0x002A0 */ AnimModel mModel2A0;
    /* 0x00380 */ AnimModel mModels380[3];
    /* 0x00620 */ AnimModel mModel620;
    /* 0x00700 */ Camera *mCamera;
    /* 0x00704 */ int mMode;
    /* 0x00708 */ int mFrame; // Frames in the mode
    /* 0x0070C */ FaceCast mCast;
    /* 0x1C594 */ int mFirst;     // The saved characters' indices (mIndex), in the order they were chosen
    /* 0x1C598 */ int mSecond;
    /* 0x1C59C */ int mFirstSex;  // MiiData_GetSex: 0 for a man
    /* 0x1C5A0 */ int mSecondSex;
    /* 0x1C5A4 */ int mAffinity;  // 0 to 100
    /* 0x1C5A8 */ BOOL mUnknown1C5A8;
    /* 0x1C5AC */ int mUnknown1C5AC;
    /* 0x1C5B0 */ MessageFile mMessageFiles[1];
    /* 0x1C5C8 */ MessageText mTexts[5];
    /* 0x1E378 */ Sprite mSprites[5];
    /* 0x1E6B4 */ u8 mUnknown1E6B4[0x20];
}; // 0x1E6D4

class AffinityScene : public Scene {
public:
    // mState
    enum State {
        STATE_CHOOSE_FIRST,  // The list: the first Mii
        STATE_SORT_FIRST,    // The sort menu, from there
        STATE_INTRO,         // The window that opens the scene
        STATE_CHOOSE_SECOND, // The list: the second Mii
        STATE_SORT_SECOND,
        STATE_FIRST_CHOSEN,  // The window that names the first
        STATE_QUESTION,      // The window that asks whether to test the two
        STATE_FADE_OUT,      // The touch screen fades out, and comes back with the result's buttons
        STATE_FADE_IN,
        STATE_RESULT,        // The result, before and after its jingle
        STATE_RESULT_SHOWN,
        STATE_STEPS,         // A sequence of steps (UpdateSteps) that ends in scene 0x20; nothing sets it
    };

    AffinityScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void UpdateChoosing();
    void ShowSelection();
    void UpdateSortMenu();
    void UpdateMessage();
    void UpdateQuestion();
    void UpdateSteps();
    void SetStep(int step);
    void NextStep();

    /* 0x00010 */ MessageFile mMessageFiles[3]; // Etc, contents and common messages
    /* 0x00058 */ int mState;
    /* 0x0005C */ int mFrame;
    /* 0x00060 */ int mAnswer;   // The button of the question window that was pressed
    /* 0x00064 */ int mStep;     // UpdateSteps
    /* 0x00068 */ int mStepFrame;
    /* 0x0006C */ u8 mUnknown0006C[0x84];
    /* 0x000F0 */ TtsPlayer mTtsPlayer;
    /* 0x14A34 */ SpeechWindow mSpeechWindow;
    /* 0x15040 */ u8 mUnknown15040[0x18];
    /* 0x15058 */ int mCharOffset;      // The touch screen's sprite characters that are free
    /* 0x1505C */ int mUpperCharOffset; // The top screen's
    /* 0x15060 */ fx32 mScroll;         // Of the touch screen's background 2
    /* 0x15064 */ u8 mSortKey;          // The list's order at the start: Exit saves the list's if it differs
    /* 0x15068 */ TextButton mButtons[4];       // Sort, back, decide, choose again
    /* 0x16C28 */ TextButton mResultButtons[2]; // On the result's screen
    /* 0x17A08 */ Sprite mResultSprites[2];
    /* 0x17B60 */ MiiList mList;
    /* 0x28F08 */ MiiListPanel mPanel;
    /* 0x32380 */ MiiListTabs mTabs;
    /* 0x374D8 */ MessageWindow mWindows[3]; // The sort menu, the messages, the question
    /* 0x583C4 */ AffinityPart mPart;
}; // 0x76A98

#endif // OV095_AFFINITYSCENE_H

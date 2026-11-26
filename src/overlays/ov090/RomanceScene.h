#ifndef OV090_ROMANCESCENE_H
#define OV090_ROMANCESCENE_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "Sprite.h"
#include "character/SceneCharacter.h"
#include "island/Sky.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/SpeechWindow.h"
#include "voice/TtsPlayer.h"

// Overlay 90, scene 0x5A: a confession. One character tells another of its love, in one of four places (by the sea,
// on a roof, under a tree, in a classroom), and the other accepts or turns it down. The scene plays a script of
// steps from a table (camera, where the two stand, animations, walking, speech); which script it plays follows from
// the place, a variant the scene before it chose, and the answer, which the scene works out from the two's
// relations, personalities and troubles. An accepted confession makes the two lovers and ends what either had with
// someone else; a rejected one halves their relation meters. The scene goes back to scene 0x40 (overlay 75), which
// is where it is asked for; from any other scene it picks a random man and woman and changes nothing.
//
// The scene factory (CreateScene) loads overlay 90 and allocates 0x1AA58 bytes; the constructor is the main
// program's (0x02044BA0, src/scene/SceneManager.cpp), which stores the overlay's virtual table (0x02156964), and
// constructs the members of classes with constructors that the class has.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM. The scene's is after its
// archive, "Scn/Scene/SceneRomance_LZ.bin", which no other overlay mounts.

// A plan of the scene on the lower screen: four sprites at where the camera is, where it looks and where the two
// characters stand, seen from above. The scene sets it up and places the sprites once; nothing shows or updates it.
class RomanceMap {
public:
    void Clear();
    void ClearPoints();
    void Init(int resource, Character *first, Character *second);
    void PlaceSprites();
    void ToScreen(ScreenPoint *point, const VecFx32 *position);

    /* 0x000 */ int mUnknown000[3];
    /* 0x00C */ NNSG2dFVec2 mUnknown00C;
    /* 0x014 */ NNSG2dFVec2 mUnknown014;
    /* 0x01C */ NNSG2dFVec2 mUnknown01C;
    /* 0x024 */ NNSG2dFVec2 mUnknown024[10];
    /* 0x074 */ int mUnknown074;
    /* 0x078 */ int mUnknown078;
    /* 0x07C */ BOOL mUnknown07C;
    /* 0x080 */ Sprite mCameraSprite;
    /* 0x12C */ Sprite mTargetSprite;
    /* 0x1D8 */ Sprite mFirstSprite;
    /* 0x284 */ Sprite mSecondSprite;
    /* 0x330 */ int mUnknown330;
    /* 0x334 */ Character *mFirst;
    /* 0x338 */ Character *mSecond;
}; // 0x33C

class RomanceScene : public Scene {
public:
    // What the scene does in a frame (mState)
    enum State {
        STATE_IDLE,
        STATE_SCRIPT, // Runs the script
        STATE_2,
    };

    // The places (mPlace): the model and the script follow from it
    enum Place {
        PLACE_SEA,
        PLACE_ROOF,
        PLACE_TREE, // The tree's model is by month
        PLACE_CLASSROOM,
    };

    RomanceScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void UpdateScript();
    void StartStep(int step);
    void UpdateSpeech();
    BOOL UpdateStep();
    void NextStep();
    void FindPartners();
    void DecideAnswer(BOOL *rejected, BOOL *outright);
    void EndPartnerships();
    void ApplyAnswer(int rejected);
    void SetConfessionFlags();
    int GetScript(int variant, int place, int rejected, int outright);
    void Speak(u32 message);
    void ChooseSecondClothes(int clothes, int sex, int *second);

    /* 0x00010 */ int mState;
    /* 0x00014 */ int mCharacterIndices[2]; // The saved characters: [0] confesses to [1]
    /* 0x0001C */ SceneCharacter mCharacters[2];
    /* 0x0564C */ BOOL mHasPartner[2];      // Each one's lover or spouse, if it has one (FindPartners)
    /* 0x05654 */ BOOL mCommitted[2];       // A spouse, or a lover whose relation meter is over 50
    /* 0x0565C */ int mPartners[2];         // The partner's saved index, -1 for none
    /* 0x05664 */ BOOL mRejected;           // The answer
    /* 0x05668 */ int mVariant;             // 0 to 3, from the scene before
    /* 0x0566C */ int mPlace;
    /* 0x05670 */ BOOL mOutright;           // Rejected without a chance: the script of four steps
    /* 0x05674 */ int mScript;              // Of sScripts
    /* 0x05678 */ int mStep;
    /* 0x0567C */ int mNextStep;
    /* 0x05680 */ BOOL mStepPending;        // Start mStep in the next frame
    /* 0x05684 */ u32 mWaiting;             // The step's flags that it still waits on
    /* 0x05688 */ int mFrame;               // Frames in the step
    /* 0x0568C */ int mWaitFrames;
    /* 0x05690 */ int mSpeaker;
    /* 0x05694 */ BOOL mVoiceEffect;        // Speech is given the voice effect's tags first
    /* 0x05698 */ int mCharOffset;
    /* 0x0569C */ MessageFile mMessageFile;
    /* 0x056B4 */ TtsPlayer mTtsPlayer;
    /* 0x19FF8 */ SpeechWindow mWindow;
    /* 0x1A604 */ AnimModel mModel;         // The place
    /* 0x1A6E4 */ SkyPalette mSky;
    /* 0x1A718 */ BOOL mStandalone;         // Not asked for by scene 0x40: changes nothing, and goes to scene 0
    /* 0x1A71C */ RomanceMap mMap;
}; // 0x1AA58

#endif // OV090_ROMANCESCENE_H

#ifndef OV039_TRAVELSCENE_H
#define OV039_TRAVELSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "Model.h"
#include "Scene.h"
#include "SceneMemory.h"
#include "ScreenControl.h"
#include "Sprite.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "character/SceneCharacter.h"
#include "sound/SoundManager.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "text/SpeechWindow.h"
#include "voice/TtsPlayer.h"

#include "../../scene/SceneCast.h"

// Overlay 39: a trip of one or two Miis (scenes 0x77, 0x78, 0x79), the first of the "Ng" family of small scenes to be
// decompiled. Its message 0x4B5 of "Msg/TmdMiniContentsMsgJP_LZ.bin" introduces it as a Mii's trip on which the
// player takes the travellers' photographs at the sights. The destination is one of nine, drawn at random; three of
// its sights are drawn too, and at each one a traveller names the sight, the player presses the shutter, and the
// travellers strike poses in the flash.
//
// The scene factory (CreateScene) loads overlay 39 and allocates 0x1EE84 bytes; the constructor is the main
// program's (0x020460B0, src/scene/SceneManager.cpp), which stores the overlay's virtual table (0x0214BA70).
//
// The name is descriptive, not the original: no name of this class survives in the ROM.

// --- The scene ---

class TravelScene : public Scene {
public:
    // The destinations (mDestination), in the order of the names of their archives "Scn/Travel/<name>_LZ.bin"
    enum Destination {
        DESTINATION_FRANCE,
        DESTINATION_HAWAII,
        DESTINATION_HONGKONG,
        DESTINATION_ITALIA,
        DESTINATION_KYOTO,
        DESTINATION_OKINAWA,
        DESTINATION_SHANGHAI,
        DESTINATION_SPAIN,
        DESTINATION_THAI,
        DESTINATION_COUNT,
    };

    // The scene ids that construct it
    enum {
        SCENE_PAIR_DRESSED = 0x77, // Two travellers, dressed for the destination; the "_hm" graphics
        SCENE_SOLO = 0x78,         // One traveller
        SCENE_PAIR = 0x79,         // Two travellers
    };

    enum { MAX_TRAVELLERS = 2, PHOTO_COUNT = 3, MAX_SIGHTS = 8 };

    // What the scene is doing (mState). Each photograph takes six states; the last one's sixth waits instead of
    // showing the "next" button.
    enum State {
        STATE_START,          // The start button
        STATE_DEPARTURE,      // The upper screen's departure (BG_airplane) fades out
        STATE_PHOTO_FADE_IN,  // The sight's picture fades in
        STATE_PHOTO_INTRO,    // The first traveller names the sight
        STATE_PHOTO_POSE,     // The shutter button; the travellers strike poses
        STATE_PHOTO_FLASH,    // The flash fades
        STATE_PHOTO_NEXT,     // The "next" button
        STATE_PHOTO_FADE_OUT, // The screens fade out
        STATE_PHOTO2_FADE_IN, // The second photograph, as the first
        STATE_PHOTO2_INTRO,
        STATE_PHOTO2_POSE,
        STATE_PHOTO2_FLASH,
        STATE_PHOTO2_NEXT,
        STATE_PHOTO2_FADE_OUT,
        STATE_PHOTO3_FADE_IN, // The third
        STATE_PHOTO3_INTRO,
        STATE_PHOTO3_POSE,
        STATE_PHOTO3_FLASH,
        STATE_PHOTO3_WAIT,    // 60 frames
        STATE_RETURN,         // The return button
        STATE_LEAVE,          // The screens fade out, then the scene asks for scene 0x40
    };

    TravelScene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void SetState(int state);
    void Speak(int speaker, u32 message, int mode);
    u32 GetSightName(int destination, int sight);
    u32 GetSightReading(int destination, int sight);

    /* 0x00010 */ int mDestination;
    /* 0x00014 */ int mTravellerCount;
    /* 0x00018 */ SceneCharacter mCharacters[MAX_TRAVELLERS];
    /* 0x05648 */ SceneCast mCast;
    /* 0x07204 */ int mLastSpot;      // Where the lone traveller stood for the last photograph, -1 at first
    /* 0x07208 */ int mPoseFrame;     // When the travellers strike their poses, by the photographer's voice
    /* 0x0720C */ int mState;
    /* 0x07210 */ int mFrame;         // Frames in the state
    /* 0x07214 */ int mSights[PHOTO_COUNT]; // The sights of the destination that are photographed
    /* 0x07220 */ char mSpeech[0x82]; // What the speech engine speaks, in Shift JIS
    /* 0x072A4 */ TtsPlayer mTtsPlayer;
    /* 0x1BBE8 */ int mUnknown1BBE8;
    /* 0x1BBEC */ Sprite mSprites[5];
    /* 0x1BF48 */ SpriteButton mButtons[1]; // The shutter
    /* 0x1C03C */ TextButton mTextButtons[4]; // Start, next, a "say cheese" never shown, return
    /* 0x1DBFC */ MessageFile mMessageFiles[1];
    /* 0x1DC14 */ SpeechWindow mSpeechWindow;
    /* 0x1E220 */ u8 mUnknown1E220[0x84];
    /* 0x1E2A4 */ MessageText mText;        // Expands the message that is spoken
    /* 0x1E894 */ MessageText mCaptions[1]; // "<name>'s" or "<name> and <name>'s", on the upper screen
}; // 0x1EE84

#endif // OV039_TRAVELSCENE_H

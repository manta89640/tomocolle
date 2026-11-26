#ifndef OV097_MARRIAGE2SCENE_H
#define OV097_MARRIAGE2SCENE_H

#include <nitro.h>

#include "Scene.h"
#include "Model.h"
#include "SpriteButton.h"
#include "character/Character.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

class FaceCast;

// Overlay 97, scenes 0xC0 and 0xC1: the staff credits that follow a wedding. The credits ("TBM:/tmd_staff_msg_JP.bmg",
// one message a line) scroll up the lower screen over the music stream, while the upper screen shows a series of
// pictures ("Scn/Mrg/mrg00_LZ.bin" to "mrg20"), each a background and a body model with the two faces of the
// couple placed on it, one after another at set frames of the stream. A button skips them, from the second time
// on. At the end the scene goes back to the wedding (scene 0xAB, overlay 96), which then says goodbye. The two faces
// are the couple that the wedding left in gSceneMemory when the scene comes from there, otherwise two characters
// chosen at random.
//
// The scene factory (CreateScene) loads overlay 97 and allocates 0x35BF0 bytes; the constructor is the main
// program's (0x02049D28, src/scene/SceneManager.cpp), which stores the overlay's virtual table (0x0214B03C). The
// members are as the constructor constructs them, by their classes; the first 0x1BE88 bytes are the main program's
// FaceCast (src/scene/SceneHelpers.h), which has no constructor of its own and is constructed as its faces.
//
// The name is after the scene's archive, "Scn/Scene/SceneMarriage2_LZ.bin", which no other overlay mounts; it is not the
// original: no name of this class survives in the ROM.
class Marriage2Scene : public Scene {
public:
    enum State {
        STATE_CREDITS,   // The credits scroll; when the picture's time is near, the screen fades out
        STATE_FADE_OUT,  // The upper screen fades out
        STATE_LOAD,      // The next picture's archive is being mounted
        STATE_FADE_IN,   // The next picture is shown and fades in
        STATE_END,       // The last line has gone by: the scene ends with the stream
        STATE_SKIP,      // The button was pressed: the stream fades out and the scene ends
        STATE_SKIP_END,  // The button was pressed after the end
        STATE_NONE,      // Scene 0xC1: nothing happens
    };

    enum {
        TEXT_COUNT = 13,    // The lines of the credits on the screen at once
        PICTURE_COUNT = 21, // "Scn/Mrg/mrg00_LZ.bin" to "mrg20"
    };

    // How one of the two faces sits on a picture: as FaceCast::SetTransform takes it from a table
    struct FacePose {
        /* 0x0 */ s8 x;
        /* 0x1 */ s8 y;
        /* 0x2 */ u8 scale;
        /* 0x3 */ u8 rotX;
        /* 0x4 */ u8 rotY;
        /* 0x5 */ u8 rotZ;
    }; // 0x6

    Marriage2Scene(int sceneId);

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();
    virtual ~Marriage2Scene();

    void SetState(int state);
    void MountPicture(int picture, int unknown);
    void ShowPicture();
    void SetFacePose(int face, const FacePose *pose);
    void ScrollCredits();

    FaceCast *GetCast() { return (FaceCast *)mCharacterBases; }

    /* 0x00010 */ CharacterBase mCharacterBases[11]; // The FaceCast's faces
    /* 0x1A0DC */ u8 mUnknown1A0DC[0x1DBC];          // The rest of the FaceCast
    /* 0x1BE98 */ AnimModel mModels[2];               // The bodies of the picture
    /* 0x1C058 */ int mState;
    /* 0x1C05C */ int mStateFrame;
    /* 0x1C060 */ int mPicture;                       // The picture whose archive is mounted
    /* 0x1C064 */ SavedCharacter mSavedCharacter;
    /* 0x1C324 */ SpriteButton mButtons[1];           // Skips the credits
    /* 0x1C418 */ fx32 mScroll;                       // How far the credits have scrolled
    /* 0x1C41C */ int mNextMessage;                   // The message the next line to come up shows
    /* 0x1C420 */ int mFrame;                         // Frames since the scene began
    /* 0x1C424 */ int mPictureIndex;                  // Into the pictures' timetable
    /* 0x1C428 */ int mTextRows[TEXT_COUNT];          // Where each line is in its round of the screen, 0 at its bottom
    /* 0x1C45C */ MessageFile mMessageFiles[1];
    /* 0x1C474 */ MessageText mTexts[TEXT_COUNT];
    /* 0x211A4 */ u8 mUnknown211A4[0x104];
    /* 0x212A8 */ TtsPlayer mTtsPlayer;
    /* 0x35BEC */ u8 mUnknown35BEC[0x4];
}; // 0x35BF0

#endif // OV097_MARRIAGE2SCENE_H

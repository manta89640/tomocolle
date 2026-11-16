#ifndef OV099_INFORMATIONSCENE_H
#define OV099_INFORMATIONSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "Scene.h"
#include "Sprite.h"
#include "SpriteButton.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"

#include <wchar.h>

// Overlay 99: the mansion's information (scene id 0xAE). One screen of statistics over the saved characters: how
// many live in the mansion, their average age, the ratio of men to women, how comfortable they find it (zero to
// five stars), how many troubles the player has solved, and a type of mansion by the residents' personalities. A
// button takes the player back to the apartment (scene 0x11).
//
// The names are descriptive, not the original: no name of these classes survives in the ROM. The main program's
// classes below are STAND-INS (not decompiled): their sizes and the fields that this overlay's code touches.

// Moves a sprite by an offset, built as an FxVec2 and passed by value (which leaves its dead copies on the stack)
static inline void MoveSprite(Sprite *sprite, FxVec2 offset) {
    sprite->mPosition.x += offset.x;
    sprite->mPosition.y += offset.y;
}
static inline void MoveSprite(Sprite *sprite, fx32 x, fx32 y) {
    NNSG2dFVec2 offset = Sprite::MakeVec(x, y);
    sprite->mPosition.x += offset.x;
    sprite->mPosition.y += offset.y;
}

// --- The main program's classes that the scene holds -----------------------------------------------------------

// A date as the clock gives it: an RTCDate, whose fields the scene compares as signed numbers
struct Date {
    /* 0x0 */ int year;
    /* 0x4 */ int month;
    /* 0x8 */ int day;
    /* 0xC */ int week;
};

// --- The scene -------------------------------------------------------------------------------------------------

// Constructed in the main program (func_02048918) after CreateScene loads the overlay for scene 0xAE. It is after
// the scene's archive, "Scn/Scene/SceneInformation_LZ.bin".
class InformationScene : public Scene {
public:
    // The texts, in the order they are made
    enum Text {
        TEXT_TYPE_TITLE,
        TEXT_RESIDENTS_TITLE,
        TEXT_AGE_TITLE,
        TEXT_RATIO_TITLE,
        TEXT_COMFORT_TITLE,
        TEXT_TROUBLES_TITLE,
        TEXT_TITLE,     // "{mansion} information"
        TEXT_TYPE,      // The mansion's type
        TEXT_RESIDENTS, // "{n}"
        TEXT_AGE,       // "{n} years", with one decimal
        TEXT_RATIO,     // "{men}:{women}"
        TEXT_COMFORT,   // "x{n}" beside one star when there are more than five
        TEXT_TROUBLES,
        TEXT_COUNT,
    };

    enum { STAR_COUNT = 5 };

    InformationScene(int sceneId); // func_02048918

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();
    virtual ~InformationScene();

    int GetAge(int year, int month, int day);
    int GetCommonDivisor(int a, int b);
    int GetComfort(int sum, int count);

    /* 0x0010 */ MessageFile mMessageFiles[1];
    /* 0x0028 */ int mUnknown28;
    /* 0x002C */ SpriteButton mButtons[1];
    /* 0x0120 */ MessageText mTexts[TEXT_COUNT];
    /* 0x4E50 */ Sprite mStars[STAR_COUNT];
    /* 0x51AC */ Date mDate;    // Today, as the game's clock has it
}; // 0x51BC

#endif // OV099_INFORMATIONSCENE_H

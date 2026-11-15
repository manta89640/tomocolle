#ifndef OV099_INFORMATIONSCENE_H
#define OV099_INFORMATIONSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "Scene.h"
#include "Sprite.h"
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

// --- The main program's sprite button (functions 0x0203115c to 0x02032158) ---------------------------------------

// The sounds a button plays (sequence archive entries of gSoundManager; -1 for none), in the order of its fields.
// What each is for is not known yet: func_0203133c plays them as the button is touched and released.
struct ButtonSounds {
    /* 0x0 */ int mSounds[4];
}; // 0x10

// A sprite that is a button on the touch screen. A STAND-IN: the constructor func_0203117c sets no sounds and
// mUnknownDC; func_0203133c, its update, returns whether it was pressed; func_02031d7c draws it. The message
// window of the main program holds twelve of them (overlay 73's MessageWindow), and the size is measured by the
// arrays that the scenes' constructors build.
struct SpriteButton : public Sprite {
    SpriteButton(); // func_0203117c

    /* 0x0AC */ int mUnknownAC; // These four and mUnknownF0 are cleared by func_0203119c
    /* 0x0B0 */ int mUnknownB0;
    /* 0x0B4 */ int mUnknownB4;
    /* 0x0B8 */ int mUnknownB8;
    /* 0x0BC */ ScreenRect mTouchRect; // Where it takes a touch, around its position (func_02031eb0 sets it)
    /* 0x0CC */ ButtonSounds mSounds;  // func_02032158 copies them in
    /* 0x0DC */ BOOL mUnknownDC;
    /* 0x0E0 */ int mUnknownE0; // func_02031260 sets it to 1 and mUnknownE4 to 0
    /* 0x0E4 */ int mUnknownE4;
    /* 0x0E8 */ int mKind;      // What the button does when touched (func_02031260); func_0203133c switches on it
    /* 0x0EC */ u16 mSequence;  // The animation sequences of the button as it is and as it is pressed
    /* 0x0EE */ u16 mPressedSequence;
    /* 0x0F0 */ int mUnknownF0;
}; // 0xF4

extern "C" {
void func_0203119c(SpriteButton *button);                          // Resets it to its normal sequence
void func_02031260(SpriteButton *button, int kind);                // Sets mKind
void func_02031278(SpriteButton *button, u16 sequence, u16 pressed); // Sets the two sequences
BOOL func_0203133c(SpriteButton *button);                          // Updates it: whether it was pressed
void func_02031d7c(SpriteButton *button);                          // Draws it
void func_02031eb0(SpriteButton *button, int width, int height);   // Its touch rectangle, centred on it
void func_02032158(SpriteButton *button, const ButtonSounds *sounds); // Copies its sounds in
}

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

// --- Other main-program things --------------------------------------------------------------------------------

// One saved character, as far as this overlay reads one: a STAND-IN of the main program's (overlay 74's
// SavedCharacter has more).
struct SavedCharacter {
    SavedCharacter(); // func_020979c0

    /* 0x000 */ u8 mUnknown000[0x50];
    /* 0x050 */ u32 mUnknown050Bits0 : 17;
    /*       */ u32 mBirthYear : 12; // BIRTH_YEAR_UNKNOWN when not given; the month and day are func_02098dac's
    /*       */ u32 mUnknown050Bits29 : 3;
    /* 0x054 */ u8 mUnknown054[0xA7];
    /* 0x0FB */ u8 mUnknown0FB;      // Summed over the residents for the stars of how comfortable they are
    /* 0x0FC */ u8 mUnknown0FC[0x1C4];
}; // 0x2C0

#define BIRTH_YEAR_UNKNOWN 0xFFF

// What func_02098dac returns of a character
enum CharacterProperty {
    PROPERTY_BIRTH_MONTH = 0x26,
    PROPERTY_BIRTH_DAY = 0x27,
};

struct ArchiveLoader;
struct CellLoader;
struct PaletteLoader;
struct ScreenLoader;
struct CharLoader;

struct ScreenControl {
    void Set(u32 planes) {
        mPlanes = planes;
        mPlanesChanged = TRUE;
    }

    /* 0x00 */ u32 mPlanes;
    /* 0x04 */ u8 mUnknown04[0xD0];
    /* 0xD4 */ BOOL mPlanesChanged;
};

struct SceneManager {
    /* 0x0 */ Scene *mScene;
    /* 0x4 */ int mNextSceneId;
    /* 0x8 */ int mPreviousSceneId;
};

extern "C" {
extern ArchiveLoader gArchiveManager;
extern CellLoader gObjResourceManager;
extern PaletteLoader data_021213b8;
extern ScreenLoader data_02121834;
extern CharLoader gVramTransferQueue;
extern ScreenControl data_0211d604;
extern SceneManager gSceneManager;
extern void *data_02123440; // The game's clock
// The mansion's name (what the information's title shows)
extern wchar_t data_02123510[];

void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void func_0203c02c(SceneManager *manager, int sceneId); // Asks for the next scene
void *func_02027534(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
u16 *func_02027d7c(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
void func_02028108(ScreenLoader *loader, int plane, int x);
void func_02028280(ScreenLoader *loader, int plane, int priority);
// A date as the clock gives it: an RTCDate, whose fields the scene compares as signed numbers
struct Date {
    /* 0x0 */ int year;
    /* 0x4 */ int month;
    /* 0x8 */ int day;
    /* 0xC */ int week;
};
void Clock_GetDate(void *clock, Date *date); // The date with the game's offset applied

BOOL func_0202c3dc(GameData *game, int index); // Whether the saved character with that index exists
BOOL func_0202c604(GameData *game, int index); // The gender bit of its Mii (set for a woman)
void func_0202c264(GameData *game, int index, SavedCharacter *character); // Reads one
int func_02098dac(SavedCharacter *character, int property);
s8 func_02098f2c(SavedCharacter *character); // Its personality, 0 to 15

// A number as a string: (value, buffer, size of the buffer in bytes, ...)
wchar_t *func_0203673c(int value, wchar_t *buffer, int size, int digits, BOOL unk);
// A number in tenths as a string, with a decimal point
wchar_t *func_02036a2c(int value, wchar_t *buffer, int size, int a, int b, int c);
}

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

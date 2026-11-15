#ifndef OV098_OPTIONSCENE_H
#define OV098_OPTIONSCENE_H

#include <nitro.h>
#include <nnsys.h>

#include "GameData.h"
#include "MessageWindow.h"
#include "Scene.h"
#include "Sprite.h"
#include "SpriteButton.h"
#include "text/MessageFile.h"
#include "text/MessageText.h"
#include "voice/TtsPlayer.h"

#include <wchar.h>

// Overlay 98: the settings (scene ids 0xAC and 0xAD). A menu of three: the sound settings (stereo or mono, and the
// volumes of the music, the sound effects and the voice, 0 to 3, which go into gGameData), setting the clock (the
// date and time, which go into the game's clock and the save data), and erasing all the save data, after three
// questions. Scene 0xAD is the first start: the player sets the clock and goes on to scene 0x69.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM. The main program's
// classes below are STAND-INS (not decompiled): their sizes and the fields that this overlay's code touches.

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

// The touch screen.
struct TouchInput {
    /* 0x00 */ u8 mUnknown00[0x30];
    /* 0x30 */ ScreenPoint mLastPoint; // Where it was touched last
    /* 0x38 */ ScreenPoint mPoint;     // Where it is touched
    /* 0x40 */ u8 mUnknown40[8];
    /* 0x48 */ BOOL mTouched;          // The stylus went down this frame
    /* 0x4C */ BOOL mUnknown4C;        // Which of the two points the scene tests
};

extern "C" {
extern ArchiveLoader gArchiveManager;
extern CellLoader gObjResourceManager;
extern PaletteLoader data_021213b8;
extern ScreenLoader data_02121834;
extern CharLoader gVramTransferQueue;
extern ScreenControl data_0211d604;
extern SceneManager gSceneManager;
extern TouchInput gTouchInput;
extern void *data_02123440; // The game's clock

void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
void func_0203c02c(SceneManager *manager, int sceneId); // Asks for the next scene
void *func_02027534(PaletteLoader *loader, const char *name, int plane, int a, int b, int c);
u16 *func_02027d7c(ScreenLoader *loader, const char *name, int plane, int a, int b, int c, int d, int e);
void func_02028f24(CharLoader *loader, const char *name, int plane, int a, int b, int c, int d);
void ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
void func_02028108(ScreenLoader *loader, int plane, int x);
void func_02028280(ScreenLoader *loader, int plane, int priority);

// The date and the time as the clock gives them (RTCDate and RTCTime: the year is 0 to 99)
void Clock_GetDate(void *clock, RTCDate *date);
void Clock_GetTime(void *clock, RTCTime *time);
// Sets the clock's offset so that it shows that date and time
void func_02035460(void *clock, RTCDate *date, RTCTime *time, BOOL unknown);
void func_02035668(void *clock, RTCDate *date, RTCTime *time); // func_02035460 with TRUE

BOOL func_0202cd38(GameData *game); // Whether the save data is still being written
void func_0202c8f8(GameData *game, int unknown);
BOOL func_0202cd48(GameData *game);
BOOL func_0202cd94(GameData *game);

// A number as a string: (value, buffer, size of the buffer in bytes, ...)
wchar_t *func_0203673c(int value, wchar_t *buffer, int size, int digits, BOOL unk);
}

// Constructed in the main program (func_020487e4) after CreateScene loads the overlay for scene 0xAC or 0xAD. It
// is after the scene's archive, "Scn/Scene/SceneOption_LZ.bin".
class OptionScene : public Scene {
public:
    enum State {
        STATE_MENU,             // The three choices
        STATE_CLOCK,            // Setting the date and time
        STATE_ASK_SAVE_CLOCK,   // "Change the time and save?"
        STATE_SAVING_CLOCK,     // "Saving..."
        STATE_CLOCK_SAVED,      // "Done; turn the power off"
        STATE_SOUND,            // The sound settings
        STATE_ASK_ERASE,        // "Erase everything?"
        STATE_ASK_ERASE_AGAIN,  // "Really?"
        STATE_ASK_ERASE_LAST,   // "Really, really?"
        STATE_ERASING,          // "Erasing..."
        STATE_ERASED,           // "Erased; turn the power off"
        STATE_FIRST_CLOCK,      // The first start: "Set the time", then the clock
        STATE_FIRST_CLOCK_SET,  // Setting it
        STATE_ASK_FIRST_CLOCK,  // "{date and time}: is this right?"
    };

    // The message windows that OpenWindow shows, by what they say
    enum Window {
        WINDOW_ASK_SAVE_CLOCK,
        WINDOW_SAVING_CLOCK,
        WINDOW_CLOCK_SAVED,
        WINDOW_ASK_ERASE,
        WINDOW_ASK_ERASE_AGAIN,
        WINDOW_ASK_ERASE_LAST,
        WINDOW_ERASING,
        WINDOW_ERASED,
        WINDOW_SET_CLOCK,
        WINDOW_ASK_CLOCK,
    };

    enum {
        BUTTON_COUNT = 17,
        TEXT_BUTTON_COUNT = 7,
        TEXT_COUNT = 4,
        SPRITE_COUNT = 16,
        DATE_TEXT_COUNT = 6,
    };

    // mButtons
    enum Button {
        BUTTON_BACK,
        BUTTON_YEAR_UP,
        BUTTON_YEAR_DOWN,
        BUTTON_MONTH_UP,
        BUTTON_MONTH_DOWN,
        BUTTON_DAY_UP,
        BUTTON_DAY_DOWN,
        BUTTON_HOUR_UP,
        BUTTON_HOUR_DOWN,
        BUTTON_MINUTE_UP,
        BUTTON_MINUTE_DOWN,
        BUTTON_MUSIC_DOWN,
        BUTTON_MUSIC_UP,
        BUTTON_EFFECT_DOWN,
        BUTTON_EFFECT_UP,
        BUTTON_VOICE_DOWN,
        BUTTON_VOICE_UP,
    };

    // mTextButtons
    enum TextButtonId {
        TEXT_BUTTON_SOUND, // "Sound settings"
        TEXT_BUTTON_CLOCK, // "Correct the time"
        TEXT_BUTTON_ERASE, // "Erase all data"
        TEXT_BUTTON_SET,   // "Correct it"
        TEXT_BUTTON_STEREO,
        TEXT_BUTTON_MONO,
        TEXT_BUTTON_DONE,  // "Setting done"
    };

    // mSprites: the digits of the date and time, the morning or afternoon, and the three volumes
    enum SpriteId {
        SPRITE_YEAR = 0,    // Four digits
        SPRITE_MONTH = 4,   // Two each
        SPRITE_DAY = 6,
        SPRITE_HOUR = 8,
        SPRITE_MINUTE = 10,
        SPRITE_AM_PM = 12,
        SPRITE_MUSIC_VOLUME,
        SPRITE_EFFECT_VOLUME,
        SPRITE_VOICE_VOLUME,
    };

    OptionScene(int sceneId); // func_020487e4

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();
    virtual ~OptionScene();

    void SetState(int state);
    void OpenWindow(int window, int charOffset);
    void ShowYear(u32 year);
    void ShowMonth(u32 month);
    void ShowDay(u32 day);
    void ShowHour(u32 hour);
    void ShowMinute(u32 minute);
    u32 GetDaysInMonth(u32 year, u32 month);

    /* 0x00010 */ int mState;
    /* 0x00014 */ int mStep;
    /* 0x00018 */ MessageFile mMessageFiles[2]; // The etc messages and the common ones
    /* 0x00048 */ fx32 mScroll;                 // Of the lower screen's background
    /* 0x0004C */ SpriteButton mButtons[BUTTON_COUNT];
    /* 0x01080 */ TextButton mTextButtons[TEXT_BUTTON_COUNT];
    /* 0x04110 */ MessageText mTexts[TEXT_COUNT]; // The names of the sound settings
    /* 0x058D0 */ Sprite mSprites[SPRITE_COUNT];
    /* 0x06390 */ int mCharOffset;
    /* 0x06394 */ int mWindowResult;
    /* 0x06398 */ MessageWindow mWindow;
    /* 0x1133C */ wchar_t mDateTexts[DATE_TEXT_COUNT][11]; // What the window asking about the clock shows
    /* 0x113C0 */ RTCDate mDate;
    /* 0x113D0 */ RTCTime mTime;
    /* 0x113DC */ char mSpeechText[0x82]; // Shift JIS for the speech synthesizer
    /* 0x11460 */ TtsPlayer mTtsPlayer;
    /* 0x25DA4 */ MessageText mSpeechMessage; // The volume of the voice, as words
}; // 0x26394

#endif // OV098_OPTIONSCENE_H

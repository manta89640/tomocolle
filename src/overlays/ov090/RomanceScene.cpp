#include "RomanceScene.h"
#include "MainProgram.h"

#include <wchar.h>

// What a step of a script does, and what it then waits for (RomanceStep::mFlags; RomanceScene::mWaiting keeps the
// flags still waited on)
enum StepFlag {
    STEP_WAIT = 0x1,              // mWaitFrames frames
    STEP_ANIMATE = 0x2,           // mAnimCharacter plays mAnimName once, then idles
    STEP_SPEAK = 0x4,             // mSpeaker says mMessage, waited on until the speech engine is idle
    STEP_WALK = 0x8,              // mWalker walks to mWalkTarget, then idles
    STEP_TURN = 0x10,             // mTurner turns towards mTurnTarget, then idles
    STEP_TURN_HEAD = 0x20,        // mHeadCharacter turns its head (sHeadPitches, sHeadYaws by mHeadDirection)
    STEP_ANIMATE_HOLD = 0x40,     // mAnimCharacter plays mAnimName once and stays at its end
    STEP_ANIMATE_LOOP = 0x80,     // mAnimCharacter plays mAnimName in a loop
    STEP_SPEAK_ON = 0x100,        // mSpeaker says mMessage, not waited on
    STEP_WAIT_SPEECH = 0x200,     // Until the speech engine is idle
    STEP_VOICE_EFFECT_ON = 0x400, // Speech from here on with the voice effect (Speak)
    STEP_VOICE_EFFECT_OFF = 0x800,
    STEP_START_MUSIC = 0x1000,
    STEP_FADE_MUSIC = 0x2000,
};

// A step of a script. Every step also places the camera (sCameras) and the two characters (sPlacements).
struct RomanceStep {
    /* 0x00 */ u32 mFlags;
    /* 0x04 */ int mWaitFrames;
    /* 0x08 */ int mAnimCharacter;
    /* 0x0C */ char mAnimName[16];
    /* 0x1C */ int mSpeaker;
    /* 0x20 */ u32 mMessage; // Of the trouble messages
    /* 0x24 */ int mWalker;
    /* 0x28 */ VecFx32 mWalkTarget;
    /* 0x34 */ int mTurner;
    /* 0x38 */ VecFx32 mTurnTarget;
    /* 0x44 */ int mHeadCharacter;
    /* 0x48 */ int mHeadDirection;
    /* 0x4C */ int mCamera;
    /* 0x50 */ int mPlacement;
}; // 0x54

struct RomanceScript {
    /* 0x0 */ const RomanceStep *mSteps;
    /* 0x4 */ int mCount;
}; // 0x8

struct RomanceCamera {
    /* 0x00 */ VecFx32 mPosition;
    /* 0x0C */ VecFx32 mTarget;
}; // 0x18

// Where the two characters stand and the way they face
struct RomancePlacement {
    /* 0x00 */ VecFx32 mPositions[2];
    /* 0x18 */ u16 mAngles[2];
}; // 0x1C

static const u16 sHeadPitches[5] = {0, 0, 0, 0xE000, 0x2000};
static const u16 sHeadYaws[5] = {0, 0xC100, 0x3F00, 0, 0};


static const RomanceCamera sCameras[40] = {
    {{-0x8688, 0xE500, -0x15F18}, {0x4B0, -0x10CD, 0x9C90}},
    {{-0xDB60, 0x2700, -0xC030}, {0x2418, 0x2733, 0x6720}},
    {{0xB7C0, 0x1F00, -0x10860}, {-0xC7B0, 0x1D33, 0x13C68}},
    {{-0x1248, 0x1C00, -0x9150}, {0x2508, 0x2133, 0x8250}},
    {{0x2E68, 0xED0, -0xC918}, {0, 0x2433, 0xB0B8}},
    {{-0x6B58, 0x700, -0x2A30}, {0x2EE0, 0x1E33, 0x2580}},
    {{0x10E0, 0xA00, -0xC198}, {0x870, 0x2C33, 0x106F8}},
    {{0x23A0, 0x9C00, -0xF870}, {-0xA50, -0x4E20, 0x110D0}},
    {{-0x1AD60, 0x5A00, 0xF870}, {0xD890, 0x9133, -0xB6D0}},
    {{0xA8C0, 0x1600, 0x5DC0}, {-0xD2F0, 0x1B33, -0x4E48}},
    {{-0x5910, 0x2800, 0x10C98}, {0x6900, 0x1433, -0x7530}},
    {{-0x9510, 0x3800, -0xE2E0}, {0x4650, 0x233, 0xEA60}},
    {{0x12840, 0x5F00, 0x5820}, {-0x12B88, -0x1ECD, -0x4560}},
    {{-0x7440, 0x1C00, -0x38B8}, {0x9600, 0x2133, 0x6270}},
    {{-0xC918, 0x1500, 0}, {0x12390, 0x2833, 0x2EE0}},
    {{-0x2A30, 0xE000, -0x1BD50}, {0x73C8, -0x16CD, 0xEF10}},
    {{-0x79E0, 0x1200, -0x6270}, {0xECB8, 0x1833, 0xE358}},
    {{0x4740, 0x7700, -0x111C0}, {-0x3840, -0x46CD, 0x11940}},
    {{-0x197D0, 0x10C00, 0x16260}, {0x8250, -0x16CD, -0x4B00}},
    {{0xA8C0, 0x1600, 0x5DC0}, {-0xD2F0, 0x1B33, -0x4E48}},
    {{-0x99C0, 0x8A00, -0xDC50}, {0x1C20, 0x1633, 0x3ED0}},
    {{-0xDB60, 0x2700, -0xC030}, {0x2418, 0x2733, 0x6720}},
    {{0xDDB8, 0xB300, 0x107E8}, {-0x4B00, -0x42CD, -0x7530}},
    {{-0xA938, 0x1C00, -0x5910}, {0x9B78, 0x1433, 0x6720}},
    {{-0xF870, 0x2710, -0x45D8}, {0x14A78, 0x1388, 0x79E0}},
    {{0x18D8, 0x1D00, -0x6720}, {0, 0x1E33, 0xC558}},
    {{-0xA938, 0x1C00, -0x5910}, {0x9B78, 0x1433, 0x6720}},
    {{-0xF3C0, 0x9B00, -0x61F8}, {0x125E8, -0x88CD, 0x9600}},
    {{0xE10, 0x1E00, 0x112B0}, {0x1D10, 0x1133, -0xBFB8}},
    {{0xA8C0, 0x1600, 0x5DC0}, {-0xD2F0, 0x1B33, -0x4E48}},
    {{-0x5898, 0x4C00, -0xD110}, {0x3CF0, 0x1633, 0x6720}},
    {{-0x4650, 0x2700, -0xFF78}, {0x5C58, 0x2733, 0x61F8}},
    {{-0x15630, 0x2700, 0x9510}, {0x11DF0, 0x2733, -0x5910}},
    {{-0xA488, 0x3A00, -0x6BD0}, {0x9150, -0x5CD, 0x7080}},
    {{-0x168, 0x1500, -0xA398}, {0x19C8, 0x1833, 0x8778}},
    {{0x18D8, 0x1D00, -0x6720}, {0, 0x1E33, 0xC558}},
    {{0x8340, 0x2C00, -0xD2F0}, {-0x6BD0, 0xF33, 0x122A0}},
    {{-0x119B8, 0x6900, -0x5118}, {0x17340, -0x4ACD, 0x8610}},
    {{0xC4E0, 0x4600, -0x89D0}, {-0xD188, -0x1BCD, 0xF078}},
    {{0x8250, 0x1600, 0x5460}, {-0xDDB8, 0x1B33, -0x6090}},
};

static const RomancePlacement sPlacements[12] = {
    {{{0x1800, 0, 0x2800}, {-0x9150, 0, -0x47B8}}, {0x2AC7, 0x1AAB}},
    {{{0x1800, 0, 0x2800}, {-0x4B00, 0, -0x1770}}, {0x75C7, 0x4493}},
    {{{0x2580, 0, 0x3840}, {0, 0, 0}}, {0xAA2B, 0x209C}},
    {{{0x1800, 0, 0x2800}, {-0x6270, 0, -0x30C0}}, {0x2AC7, 0x1AAB}},
    {{{0x1800, 0, 0x2800}, {-0x29D6, 0, -0x184C}}, {0x75C7, 0x4493}},
    {{{0x2580, 0, 0x3840}, {0, 0, 0}}, {0xAA2B, 0x209C}},
    {{{0x1800, 0, 0x2800}, {-0x9150, 0, -0x47B8}}, {0x2AC7, 0x2209}},
    {{{0x1800, 0, 0x2800}, {-0x2A30, 0, -0x4650}}, {0x75C7, 0x4493}},
    {{{0x2580, 0, 0x3840}, {0, 0, 0}}, {0xAA2B, 0x209C}},
    {{{0x1800, 0, 0x2800}, {0x438, 0, -0x7FF8}}, {0x46C7, 0x1AAB}},
    {{{0x1800, 0, 0x2800}, {-0x16F8, 0, -0x37C8}}, {0x75C7, 0x4493}},
    {{{0x2580, 0, 0x3840}, {0, 0, 0}}, {0xAA2B, 0x209C}},
};

static const RomanceStep sSeaSteps0Accepted[12] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 20, 6},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x7F88, 0, -0x391B}, -1, {0, 0, 0}, 0, 1, 20, 6},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0, 0, 0}, 0, {0, 0, 0}, 0, 0, 22, 7},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 23, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 23, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 23, 8},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 23, 8},
    {STEP_SPEAK, 0, -1, "", 0, 560, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 23, 8},
    {STEP_WAIT, 250, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 23, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 4, 28, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 0, 28, 8},
    {STEP_SPEAK, 0, -1, "", 1, 554, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 28, 8},
};
static const RomanceStep sSeaSteps0Rejected[12] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 20, 6},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x7F88, 0, -0x391B}, -1, {0, 0, 0}, 0, 1, 20, 6},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0, 0, 0}, 0, {0, 0, 0}, 0, 0, 22, 7},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 23, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 23, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 23, 8},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 23, 8},
    {STEP_SPEAK, 0, -1, "", 0, 560, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 23, 8},
    {STEP_WAIT, 250, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 23, 8},
    {STEP_VOICE_EFFECT_ON, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 29, 8},
    {STEP_SPEAK, 0, 1, "", 1, 556, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 29, 8},
    {STEP_VOICE_EFFECT_OFF, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 29, 8},
};
static const RomanceStep sSeaSteps1Accepted[16] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 20, 6},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x7F88, 0, -0x391B}, -1, {0, 0, 0}, 0, 1, 20, 6},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0, 0, 0}, 0, {0, 0, 0}, 0, 0, 22, 7},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 23, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 23, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 23, 8},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 23, 8},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_romantic00", 0, 568, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 24, 8},
    {STEP_ANIMATE_LOOP, 0, 0, "r_romantic01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 24, 8},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 24, 8},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 24, 8},
    {STEP_ANIMATE, 0, 0, "r_romantic02", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 24, 8},
    {STEP_WAIT, 200, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 24, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 4, 24, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 0, 24, 8},
    {STEP_SPEAK, 0, -1, "", 1, 554, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 24, 8},
};
static const RomanceStep sSeaSteps1Rejected[16] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 20, 6},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x7F88, 0, -0x391B}, -1, {0, 0, 0}, 0, 1, 20, 6},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0, 0, 0}, 0, {0, 0, 0}, 0, 0, 22, 7},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 23, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 23, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 23, 8},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 23, 8},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_romantic00", 0, 568, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 24, 8},
    {STEP_ANIMATE_LOOP, 0, 0, "r_romantic01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 24, 8},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 24, 8},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 24, 8},
    {STEP_ANIMATE, 0, 0, "r_romantic02", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 24, 8},
    {STEP_WAIT, 200, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 24, 8},
    {STEP_VOICE_EFFECT_ON, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 29, 8},
    {STEP_SPEAK, 0, 1, "", 1, 556, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 29, 8},
    {STEP_VOICE_EFFECT_OFF, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 29, 8},
};
static const RomanceStep sSeaSteps2Accepted[16] = {
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 20, 6},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x7F88, 0, -0x391B}, -1, {0, 0, 0}, 0, 1, 20, 6},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0, 0, 0}, 0, {0, 0, 0}, 0, 0, 22, 7},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 23, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 2, 23, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 23, 8},
    {STEP_WAIT, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 23, 8},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_proudly00", 0, 576, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 26, 8},
    {STEP_ANIMATE_LOOP, 0, 0, "r_proudly01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 26, 8},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 26, 8},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 26, 8},
    {STEP_ANIMATE_LOOP, 0, 0, "idle00", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 26, 8},
    {STEP_WAIT, 200, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 26, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 4, 28, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 0, 28, 8},
    {STEP_SPEAK, 0, -1, "", 1, 554, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 28, 8},
};
static const RomanceStep sSeaSteps2Rejected[15] = {
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 20, 6},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x7F88, 0, -0x391B}, -1, {0, 0, 0}, 0, 1, 20, 6},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0, 0, 0}, 0, {0, 0, 0}, 0, 0, 22, 7},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_proudly00", 0, 576, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 26, 8},
    {STEP_ANIMATE_LOOP, 0, 0, "r_proudly01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 26, 8},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 26, 8},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 26, 8},
    {STEP_ANIMATE_LOOP, 0, 0, "idle00", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 26, 8},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 26, 8},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 26, 8},
    {STEP_ANIMATE_LOOP, 0, 0, "idle00", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 26, 8},
    {STEP_WAIT, 200, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 26, 8},
    {STEP_VOICE_EFFECT_ON, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 29, 8},
    {STEP_SPEAK, 0, 1, "", 1, 556, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 29, 8},
    {STEP_VOICE_EFFECT_OFF, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 29, 8},
};
static const RomanceStep sSeaSteps3Accepted[16] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 20, 6},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x7F88, 0, -0x391B}, -1, {0, 0, 0}, 0, 1, 20, 6},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0, 0, 0}, 0, {0, 0, 0}, 0, 0, 22, 7},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 23, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 23, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 23, 8},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 23, 8},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_desperate00", 0, 584, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 27, 8},
    {STEP_ANIMATE_LOOP, 0, 0, "r_desperate01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 27, 8},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 27, 8},
    {STEP_WAIT, 250, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 27, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 4, 27, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 0, 27, 8},
    {STEP_SPEAK, 0, -1, "", 1, 554, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 27, 8},
    {STEP_ANIMATE, 0, 0, "r_desperate02", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 27, 8},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 27, 8},
};
static const RomanceStep sSeaSteps3Rejected[14] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 20, 6},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x7F88, 0, -0x391B}, -1, {0, 0, 0}, 0, 1, 20, 6},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0, 0, 0}, 0, {0, 0, 0}, 0, 0, 22, 7},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 23, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 23, 8},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 23, 8},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 23, 8},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_desperate00", 0, 584, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 27, 8},
    {STEP_ANIMATE_LOOP, 0, 0, "r_desperate01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 27, 8},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 27, 8},
    {STEP_WAIT, 250, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 27, 8},
    {STEP_VOICE_EFFECT_ON, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 27, 8},
    {STEP_SPEAK, 0, 1, "", 1, 556, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 27, 8},
    {STEP_VOICE_EFFECT_OFF, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 27, 8},
};
static const RomanceStep sSeaStepsOutright[4] = {
    {STEP_WAIT, 180, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 20, 6},
    {STEP_WAIT | STEP_FADE_MUSIC, 120, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 20, 6},
    {STEP_WAIT | STEP_TURN_HEAD, 100, -1, "", 0, 558, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 20, 6},
    {STEP_SPEAK, 0, -1, "", 0, 558, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 20, 6},
};
static const RomanceStep sRoofSteps0Accepted[12] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 10, 3},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x50A0, 0, -0x2580}, -1, {0, 0, 0}, 0, 1, 10, 3},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x9D8, 0, 0x3C0}, 0, {0, 0, 0}, 0, 0, 12, 4},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 13, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 13, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 13, 5},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 13, 5},
    {STEP_SPEAK, 0, -1, "", 0, 564, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 13, 5},
    {STEP_WAIT, 250, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 13, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 4, 18, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 0, 18, 5},
    {STEP_SPEAK, 0, -1, "", 1, 554, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 18, 5},
};
static const RomanceStep sRoofSteps0Rejected[12] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 10, 3},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x50A0, 0, -0x2580}, -1, {0, 0, 0}, 0, 1, 10, 3},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x9D8, 0, 0x3C0}, 0, {0, 0, 0}, 0, 0, 12, 4},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 13, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 13, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 13, 5},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 13, 5},
    {STEP_SPEAK, 0, -1, "", 0, 564, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 13, 5},
    {STEP_WAIT, 250, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 13, 5},
    {STEP_VOICE_EFFECT_ON, 0, 1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 19, 5},
    {STEP_SPEAK, 0, 1, "", 1, 556, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 19, 5},
    {STEP_VOICE_EFFECT_OFF, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 19, 5},
};
static const RomanceStep sRoofSteps1Accepted[16] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 10, 3},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x50A0, 0, -0x2580}, -1, {0, 0, 0}, 0, 1, 10, 3},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x9D8, 0, 0x3C0}, 0, {0, 0, 0}, 0, 0, 12, 4},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 13, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 13, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 13, 5},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 13, 5},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_romantic00", 0, 572, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 14, 5},
    {STEP_ANIMATE_LOOP, 0, 0, "r_romantic01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 14, 5},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 14, 5},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 14, 5},
    {STEP_ANIMATE, 0, 0, "r_romantic02", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 14, 5},
    {STEP_WAIT, 200, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 14, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 4, 18, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 0, 18, 5},
    {STEP_SPEAK, 0, -1, "", 1, 554, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 18, 5},
};
static const RomanceStep sRoofSteps1Rejected[16] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 10, 3},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x50A0, 0, -0x2580}, -1, {0, 0, 0}, 0, 1, 10, 3},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x9D8, 0, 0x3C0}, 0, {0, 0, 0}, 0, 0, 12, 4},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 13, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 13, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 13, 5},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 13, 5},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_romantic00", 0, 572, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 14, 5},
    {STEP_ANIMATE_LOOP, 0, 0, "r_romantic01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 14, 5},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 14, 5},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 14, 5},
    {STEP_ANIMATE, 0, 0, "r_romantic02", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 14, 5},
    {STEP_WAIT, 200, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 14, 5},
    {STEP_VOICE_EFFECT_ON, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 19, 5},
    {STEP_SPEAK, 0, 1, "", 1, 556, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 19, 5},
    {STEP_VOICE_EFFECT_OFF, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 19, 5},
};
static const RomanceStep sRoofSteps2Accepted[16] = {
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 10, 3},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x50A0, 0, -0x2580}, -1, {0, 0, 0}, 0, 1, 10, 3},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x9D8, 0, 0x3C0}, 0, {0, 0, 0}, 0, 0, 12, 4},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 13, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 2, 13, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 13, 5},
    {STEP_WAIT, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 13, 5},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_proudly00", 0, 580, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 16, 5},
    {STEP_ANIMATE_LOOP, 0, 0, "r_proudly01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 16, 5},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 16, 5},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 16, 5},
    {STEP_ANIMATE_LOOP, 0, 0, "idle00", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 16, 5},
    {STEP_WAIT, 200, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 16, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 4, 18, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 0, 18, 5},
    {STEP_SPEAK, 0, -1, "", 1, 554, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 18, 5},
};
static const RomanceStep sRoofSteps2Rejected[16] = {
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 10, 3},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x50A0, 0, -0x2580}, -1, {0, 0, 0}, 0, 1, 10, 3},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x9D8, 0, 0x3C0}, 0, {0, 0, 0}, 0, 0, 12, 4},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 13, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 2, 13, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 13, 5},
    {STEP_WAIT, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 13, 5},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_proudly00", 0, 580, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 16, 5},
    {STEP_ANIMATE_LOOP, 0, 0, "r_proudly01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 16, 5},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 16, 5},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 16, 5},
    {STEP_ANIMATE_LOOP, 0, 0, "idle00", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 16, 5},
    {STEP_WAIT, 200, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 16, 5},
    {STEP_VOICE_EFFECT_ON, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 9, 5},
    {STEP_SPEAK, 0, 1, "", 1, 556, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 9, 5},
    {STEP_VOICE_EFFECT_OFF, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 9, 5},
};
static const RomanceStep sRoofSteps3Accepted[16] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 10, 3},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x50A0, 0, -0x2580}, -1, {0, 0, 0}, 0, 1, 10, 3},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x9D8, 0, 0x3C0}, 0, {0, 0, 0}, 0, 0, 12, 4},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 13, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 13, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 13, 5},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 13, 5},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_desperate00", 0, 588, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 17, 5},
    {STEP_ANIMATE_LOOP, 0, 0, "r_desperate01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 17, 5},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 17, 5},
    {STEP_WAIT, 250, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 17, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 4, 17, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 0, 17, 5},
    {STEP_SPEAK, 0, -1, "", 1, 554, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 17, 5},
    {STEP_ANIMATE, 0, 0, "r_desperate02", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 17, 5},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 17, 5},
};
static const RomanceStep sRoofSteps3Rejected[14] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 10, 3},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x50A0, 0, -0x2580}, -1, {0, 0, 0}, 0, 1, 10, 3},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x9D8, 0, 0x3C0}, 0, {0, 0, 0}, 0, 0, 12, 4},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 13, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 13, 5},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 13, 5},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 13, 5},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_desperate00", 0, 588, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 17, 5},
    {STEP_ANIMATE_LOOP, 0, 0, "r_desperate01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 17, 5},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 17, 5},
    {STEP_WAIT, 250, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 17, 5},
    {STEP_VOICE_EFFECT_ON, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 17, 5},
    {STEP_SPEAK, 0, 1, "", 1, 556, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 17, 5},
    {STEP_VOICE_EFFECT_OFF, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 17, 5},
};
static const RomanceStep sRoofStepsOutright[4] = {
    {STEP_WAIT, 180, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 10, 3},
    {STEP_WAIT | STEP_FADE_MUSIC, 120, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 10, 3},
    {STEP_WAIT | STEP_TURN_HEAD, 100, -1, "", 0, 558, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 15, 3},
    {STEP_SPEAK, 0, -1, "", 0, 558, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 15, 3},
};
static const RomanceStep sTreeSteps0Accepted[10] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 0, 0},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x7FDB, 0, -0x2AA2}, -1, {0, 0, 0}, 0, 1, 0, 0},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x1608, 0, 0x9D8}, 0, {0, 0, 0}, 0, 0, 2, 1},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 3, 2},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 3, 2},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 3, 2},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 3, 2},
    {STEP_SPEAK, 0, -1, "", 0, 562, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 3, 2},
    {STEP_WAIT, 250, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 3, 2},
    {STEP_SPEAK, 0, -1, "", 1, 554, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 8, 2},
};
static const RomanceStep sTreeSteps0Rejected[12] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 0, 0},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x7FDB, 0, -0x2AA2}, -1, {0, 0, 0}, 0, 1, 0, 0},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x1608, 0, 0x9D8}, 0, {0, 0, 0}, 0, 0, 2, 1},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 3, 2},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 3, 2},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 3, 2},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 3, 2},
    {STEP_SPEAK, 0, -1, "", 0, 562, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 3, 2},
    {STEP_WAIT, 250, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 3, 2},
    {STEP_VOICE_EFFECT_ON, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 9, 2},
    {STEP_SPEAK, 0, 1, "", 1, 556, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 9, 2},
    {STEP_VOICE_EFFECT_OFF, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 9, 2},
};
static const RomanceStep sTreeSteps1Accepted[14] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 0, 0},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x7FDB, 0, -0x2AA2}, -1, {0, 0, 0}, 0, 1, 0, 0},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x1608, 0, 0x9D8}, 0, {0, 0, 0}, 0, 0, 2, 1},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 3, 2},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 3, 2},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 3, 2},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 3, 2},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_romantic00", 0, 570, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 4, 2},
    {STEP_ANIMATE_LOOP, 0, 0, "r_romantic01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 4, 2},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 4, 2},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 4, 2},
    {STEP_ANIMATE, 0, 0, "r_romantic02", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 4, 2},
    {STEP_WAIT, 200, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 4, 2},
    {STEP_SPEAK, 0, -1, "", 1, 554, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 8, 2},
};
static const RomanceStep sTreeSteps1Rejected[16] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 0, 0},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x7FDB, 0, -0x2AA2}, -1, {0, 0, 0}, 0, 1, 0, 0},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x1608, 0, 0x9D8}, 0, {0, 0, 0}, 0, 0, 2, 1},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 3, 2},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 3, 2},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 3, 2},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 3, 2},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_romantic00", 0, 570, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 4, 2},
    {STEP_ANIMATE_LOOP, 0, 0, "r_romantic01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 4, 2},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 4, 2},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 4, 2},
    {STEP_ANIMATE, 0, 0, "r_romantic02", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 4, 2},
    {STEP_WAIT, 200, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 4, 2},
    {STEP_VOICE_EFFECT_ON, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 9, 2},
    {STEP_SPEAK, 0, 1, "", 1, 556, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 9, 2},
    {STEP_VOICE_EFFECT_OFF, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 9, 2},
};
static const RomanceStep sTreeSteps2Accepted[14] = {
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 0, 0},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x7FDB, 0, -0x2AA2}, -1, {0, 0, 0}, 0, 1, 0, 0},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x1608, 0, 0x9D8}, 0, {0, 0, 0}, 0, 0, 2, 1},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 3, 2},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 2, 3, 2},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 3, 2},
    {STEP_WAIT, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 3, 2},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_proudly00", 0, 578, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 6, 2},
    {STEP_ANIMATE_LOOP, 0, 0, "r_proudly01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 6, 2},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 6, 2},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 6, 2},
    {STEP_ANIMATE_LOOP, 0, 0, "idle00", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 6, 2},
    {STEP_WAIT, 200, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 6, 2},
    {STEP_SPEAK, 0, -1, "", 1, 554, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 8, 2},
};
static const RomanceStep sTreeSteps2Rejected[16] = {
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 0, 0},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x7FDB, 0, -0x2AA2}, -1, {0, 0, 0}, 0, 1, 0, 0},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x1608, 0, 0x9D8}, 0, {0, 0, 0}, 0, 0, 2, 1},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 3, 2},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 2, 3, 2},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 3, 2},
    {STEP_WAIT, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 3, 2},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_proudly00", 0, 578, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 6, 2},
    {STEP_ANIMATE_LOOP, 0, 0, "r_proudly01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 6, 2},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 6, 2},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 6, 2},
    {STEP_ANIMATE_LOOP, 0, 0, "idle00", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 6, 2},
    {STEP_WAIT, 200, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 6, 2},
    {STEP_VOICE_EFFECT_ON, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 9, 2},
    {STEP_SPEAK, 0, 1, "", 1, 556, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 9, 2},
    {STEP_VOICE_EFFECT_OFF, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 9, 2},
};
static const RomanceStep sTreeSteps3Accepted[16] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 0, 0},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x7FDB, 0, -0x2AA2}, -1, {0, 0, 0}, 0, 1, 0, 0},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x1608, 0, 0x9D8}, 0, {0, 0, 0}, 0, 0, 2, 1},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 3, 2},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 3, 2},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 3, 2},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 3, 2},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_desperate00", 0, 586, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 7, 2},
    {STEP_ANIMATE_LOOP, 0, 0, "r_desperate01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 7, 2},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 7, 2},
    {STEP_WAIT, 250, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 7, 2},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 4, 7, 2},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 0, 7, 2},
    {STEP_SPEAK, 0, -1, "", 1, 554, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 7, 2},
    {STEP_ANIMATE, 0, 0, "r_desperate02", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 7, 2},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 7, 2},
};
static const RomanceStep sTreeSteps3Rejected[14] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 0, 0},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x7FDB, 0, -0x2AA2}, -1, {0, 0, 0}, 0, 1, 0, 0},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {-0x1608, 0, 0x9D8}, 0, {0, 0, 0}, 0, 0, 2, 1},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 3, 2},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 3, 2},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 3, 2},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 3, 2},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_desperate00", 0, 586, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 7, 2},
    {STEP_ANIMATE_LOOP, 0, 0, "r_desperate01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 7, 2},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 7, 2},
    {STEP_WAIT, 250, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 7, 2},
    {STEP_VOICE_EFFECT_ON, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 7, 2},
    {STEP_SPEAK, 0, 1, "", 1, 556, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 7, 2},
    {STEP_VOICE_EFFECT_OFF, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 7, 2},
};
static const RomanceStep sTreeStepsOutright[4] = {
    {STEP_WAIT, 180, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 0, 0},
    {STEP_WAIT | STEP_FADE_MUSIC, 120, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 0, 0},
    {STEP_WAIT | STEP_TURN_HEAD, 100, -1, "", 0, 558, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 0, 0},
    {STEP_SPEAK, 0, -1, "", 0, 558, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 0, 0},
};
static const RomanceStep sClassroomSteps0Accepted[12] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 30, 9},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0x78B, 0, -0x67CA}, -1, {0, 0, 0}, 0, 1, 30, 9},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0, 0, 0}, 0, {0x3E0, 0, -0x60BF}, 0, 0, 32, 10},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 33, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 33, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 33, 11},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 33, 11},
    {STEP_SPEAK, 0, -1, "", 0, 566, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 33, 11},
    {STEP_WAIT, 250, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 33, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 4, 38, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 0, 38, 11},
    {STEP_SPEAK, 0, -1, "", 1, 554, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 38, 11},
};
static const RomanceStep sClassroomSteps0Rejected[13] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 30, 9},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0x78B, 0, -0x67CA}, -1, {0, 0, 0}, 0, 1, 30, 9},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0, 0, 0}, 0, {0x3E0, 0, -0x60BF}, 0, 0, 32, 10},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 33, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 33, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 33, 11},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 33, 11},
    {STEP_SPEAK, 0, -1, "", 0, 566, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 33, 11},
    {STEP_WAIT, 250, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 33, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 0, 39, 11},
    {STEP_VOICE_EFFECT_ON, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 39, 2},
    {STEP_SPEAK, 0, 1, "", 1, 556, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 39, 2},
    {STEP_VOICE_EFFECT_OFF, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 39, 2},
};
static const RomanceStep sClassroomSteps1Accepted[17] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 30, 9},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0x78B, 0, -0x67CA}, -1, {0, 0, 0}, 0, 1, 30, 9},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0, 0, 0}, 0, {0x3E0, 0, -0x60BF}, 0, 0, 32, 10},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 33, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 33, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 33, 11},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 33, 11},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_romantic00", 0, 574, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 34, 11},
    {STEP_ANIMATE_LOOP, 0, 0, "r_romantic01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 34, 11},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 34, 11},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 34, 11},
    {STEP_ANIMATE, 0, 0, "r_romantic02", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 34, 11},
    {STEP_WAIT, 200, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 34, 11},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 34, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 4, 38, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 0, 38, 11},
    {STEP_SPEAK, 0, -1, "", 1, 554, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 38, 11},
};
static const RomanceStep sClassroomSteps1Rejected[16] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 30, 9},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0x78B, 0, -0x67CA}, -1, {0, 0, 0}, 0, 1, 30, 9},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0, 0, 0}, 0, {0x3E0, 0, -0x60BF}, 0, 0, 32, 10},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 33, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 33, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 33, 11},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 33, 11},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_romantic00", 0, 574, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 34, 11},
    {STEP_ANIMATE_LOOP, 0, 0, "r_romantic01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 34, 11},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 34, 11},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 34, 11},
    {STEP_ANIMATE, 0, 0, "r_romantic02", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 34, 11},
    {STEP_WAIT, 200, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 34, 11},
    {STEP_VOICE_EFFECT_ON, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 39, 11},
    {STEP_SPEAK, 0, 1, "", 1, 556, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 39, 11},
    {STEP_VOICE_EFFECT_OFF, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 39, 11},
};
static const RomanceStep sClassroomSteps2Accepted[16] = {
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 30, 9},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0x78B, 0, -0x67CA}, -1, {0, 0, 0}, 0, 1, 30, 9},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0, 0, 0}, 0, {0x3E0, 0, -0x60BF}, 0, 0, 32, 10},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 33, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 33, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 33, 11},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 33, 11},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_proudly00", 0, 582, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 36, 11},
    {STEP_ANIMATE_LOOP, 0, 0, "r_proudly01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 36, 11},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 36, 11},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 36, 11},
    {STEP_ANIMATE_LOOP, 0, 0, "idle00", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 36, 11},
    {STEP_WAIT, 200, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 36, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 4, 38, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 0, 38, 11},
    {STEP_SPEAK, 0, -1, "", 1, 554, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 38, 11},
};
static const RomanceStep sClassroomSteps2Rejected[16] = {
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 30, 9},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0x78B, 0, -0x67CA}, -1, {0, 0, 0}, 0, 1, 30, 9},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0, 0, 0}, 0, {0x3E0, 0, -0x60BF}, 0, 0, 32, 10},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 33, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 33, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 33, 11},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 33, 11},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_proudly00", 0, 582, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 36, 11},
    {STEP_ANIMATE_LOOP, 0, 0, "r_proudly01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 36, 11},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 36, 11},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 36, 11},
    {STEP_ANIMATE_LOOP, 0, 0, "idle00", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 36, 11},
    {STEP_WAIT, 200, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 36, 11},
    {STEP_VOICE_EFFECT_ON, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 39, 11},
    {STEP_SPEAK, 0, 1, "", 1, 556, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 39, 11},
    {STEP_VOICE_EFFECT_OFF, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 39, 11},
};
static const RomanceStep sClassroomSteps3Accepted[16] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 30, 9},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0x78B, 0, -0x67CA}, -1, {0, 0, 0}, 0, 1, 30, 9},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0, 0, 0}, 0, {0x3E0, 0, -0x60BF}, 0, 0, 32, 10},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 33, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 33, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 33, 11},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 33, 11},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_desperate00", 0, 590, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 37, 11},
    {STEP_ANIMATE_LOOP, 0, 0, "r_desperate01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 37, 11},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 37, 11},
    {STEP_WAIT, 250, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 37, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 4, 37, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 1, 0, 37, 11},
    {STEP_SPEAK, 0, -1, "", 1, 554, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 37, 11},
    {STEP_ANIMATE, 0, 0, "r_desperate02", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 37, 11},
    {STEP_WAIT, 100, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 37, 11},
};
static const RomanceStep sClassroomSteps3Rejected[14] = {
    {STEP_WAIT, 150, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 30, 9},
    {STEP_WALK | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0x78B, 0, -0x67CA}, -1, {0, 0, 0}, 0, 1, 30, 9},
    {STEP_WALK | STEP_TURN | STEP_TURN_HEAD, 0, -1, "", -1, 0, 1, {0, 0, 0}, 0, {0x3E0, 0, -0x60BF}, 0, 0, 32, 10},
    {STEP_WAIT | STEP_FADE_MUSIC, 90, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 33, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 10, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 33, 11},
    {STEP_WAIT | STEP_TURN_HEAD, 20, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 0, 33, 11},
    {STEP_WAIT, 30, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 33, 11},
    {STEP_ANIMATE_HOLD | STEP_SPEAK_ON, 0, 0, "r_desperate00", 0, 590, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 37, 11},
    {STEP_ANIMATE_LOOP, 0, 0, "r_desperate01", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 37, 11},
    {STEP_WAIT_SPEECH, 0, 0, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 37, 11},
    {STEP_WAIT, 250, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 37, 11},
    {STEP_VOICE_EFFECT_ON, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 37, 11},
    {STEP_SPEAK, 0, 1, "", 1, 556, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 37, 11},
    {STEP_VOICE_EFFECT_OFF, 0, 1, "", 1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 37, 11},
};
static const RomanceStep sClassroomStepsOutright[4] = {
    {STEP_WAIT, 180, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 30, 9},
    {STEP_WAIT | STEP_FADE_MUSIC, 120, -1, "", -1, 0, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 30, 9},
    {STEP_WAIT | STEP_TURN_HEAD, 100, -1, "", 0, 558, -1, {0, 0, 0}, -1, {0, 0, 0}, 0, 4, 30, 9},
    {STEP_SPEAK, 0, -1, "", 0, 558, -1, {0, 0, 0}, -1, {0, 0, 0}, -1, 0, 30, 9},
};

static const RomanceScript sScripts[36] = {
    {sSeaSteps0Accepted, 12},
    {sSeaSteps0Rejected, 12},
    {sSeaSteps1Accepted, 16},
    {sSeaSteps1Rejected, 16},
    {sSeaSteps2Accepted, 16},
    {sSeaSteps2Rejected, 15},
    {sSeaSteps3Accepted, 16},
    {sSeaSteps3Rejected, 14},
    {sSeaStepsOutright, 4},
    {sRoofSteps0Accepted, 12},
    {sRoofSteps0Rejected, 12},
    {sRoofSteps1Accepted, 16},
    {sRoofSteps1Rejected, 16},
    {sRoofSteps2Accepted, 16},
    {sRoofSteps2Rejected, 16},
    {sRoofSteps3Accepted, 16},
    {sRoofSteps3Rejected, 14},
    {sRoofStepsOutright, 4},
    {sTreeSteps0Accepted, 10},
    {sTreeSteps0Rejected, 12},
    {sTreeSteps1Accepted, 14},
    {sTreeSteps1Rejected, 16},
    {sTreeSteps2Accepted, 14},
    {sTreeSteps2Rejected, 16},
    {sTreeSteps3Accepted, 16},
    {sTreeSteps3Rejected, 14},
    {sTreeStepsOutright, 4},
    {sClassroomSteps0Accepted, 12},
    {sClassroomSteps0Rejected, 13},
    {sClassroomSteps1Accepted, 17},
    {sClassroomSteps1Rejected, 16},
    {sClassroomSteps2Accepted, 16},
    {sClassroomSteps2Rejected, 16},
    {sClassroomSteps3Accepted, 16},
    {sClassroomSteps3Rejected, 14},
    {sClassroomStepsOutright, 4},
};

// The scale of the plan: 1200 units of the floor to a pixel's 4096
static const fx32 sMapScale = FX_Inv(0x4B0);

static inline const RomanceStep *GetStep(int script, int step) {
    return &sScripts[script].mSteps[step];
}

void RomanceScene::StartLoading() {
    if (gSceneManager.mPreviousSceneId == 0x40) {
        mStandalone = FALSE;
        mVariant = gSceneScratch[0];
        mPlace = gSceneScratch[1];
        mCharacterIndices[0] = gSceneMemory.mCharacters[0];
        mCharacterIndices[1] = gSceneMemory.mCharacters[1];
    } else {
        u16 men[100];
        u16 women[100];
        mStandalone = TRUE;
        mVariant = 0;
        mPlace = PLACE_SEA;
        int menCount = 0;
        int womenCount = 0;
        for (u16 i = 0; i < 100; i++) {
            if (GameData_CharacterExists(&gGameData, i)) {
                if (!GameData_GetCharacterSex(&gGameData, i)) {
                    men[menCount] = i;
                    menCount++;
                } else {
                    women[womenCount] = i;
                    womenCount++;
                }
            }
        }
        if (menCount != 0 && womenCount != 0) {
            int man, woman;
            if (MATH_Rand16(&gRandom, 2) == 0) {
                man = 0;
                woman = 1;
            } else {
                man = 1;
                woman = 0;
            }
            mCharacterIndices[man] = men[MATH_Rand16(&gRandom, menCount)];
            mCharacterIndices[woman] = women[MATH_Rand16(&gRandom, womenCount)];
        } else {
            mCharacterIndices[0] = 0;
            mCharacterIndices[1] = 0;
        }
    }
    mCharacters[0].CharacterModel::SetArchive("3M0", 0, 0);
    mCharacters[1].CharacterModel::SetArchive("3M1", 0, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3M0", mCharacters[0].mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3M1", mCharacters[1].mArchivePath, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "3A0", "Mdl/Ica/ica_mii_event_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "SCT", "Scn/Scene/SceneRomance_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "SKY", "Scn/Scene/Sky_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TTM", "Msg/TmdTroubleMsgJP_LZ.bin", 0);
    switch (mPlace) {
    case PLACE_SEA:
        ArchiveManager_MountCompressed(&gArchiveManager, "3SS", "Mdl/Misc/roma_sea_3D_LZ.bin", 0);
        break;
    case PLACE_ROOF:
        ArchiveManager_MountCompressed(&gArchiveManager, "3SS", "Mdl/Misc/roma_roof_3D_LZ.bin", 0);
        break;
    case PLACE_TREE: {
        RTCDate date;
        Clock_GetDate(gGameClock, &date);
        if (date.month == 3 || (date.month >= 5 && date.month <= 9)) {
            ArchiveManager_MountCompressed(&gArchiveManager, "3SS", "Mdl/Misc/roma_tree0_3D_LZ.bin", 0);
        } else if (date.month == 4) {
            ArchiveManager_MountCompressed(&gArchiveManager, "3SS", "Mdl/Misc/roma_tree1_3D_LZ.bin", 0);
        } else if (date.month >= 10 && date.month <= 11) {
            ArchiveManager_MountCompressed(&gArchiveManager, "3SS", "Mdl/Misc/roma_tree2_3D_LZ.bin", 0);
        } else if (date.month <= 2 || date.month == 12) {
            ArchiveManager_MountCompressed(&gArchiveManager, "3SS", "Mdl/Misc/roma_tree3_3D_LZ.bin", 0);
        }
        break;
    }
    case PLACE_CLASSROOM:
        ArchiveManager_MountCompressed(&gArchiveManager, "3SS", "Mdl/Misc/roma_class_3D_LZ.bin", 0);
        break;
    }
    gSceneManager.SetSoundGroup(0xD);
    gSoundManager.LoadGroup(0x4D);
    if (mVariant == 2) {
        return;
    }
    gSoundManager.StartSeqIfNotPlaying(0, 0x14);
}

void RomanceScene::Init() {
    RTCDate date;
    RTCTime time;
    int clothes[2];

    mState = STATE_SCRIPT;
    mNextStep = 0;
    mStep = 0;
    mStepPending = TRUE;
    mSpeaker = 0;
    mVoiceEffect = FALSE;
    mCharOffset = 0x12C;
    mMessageFile.Open("TTM:tmd_trouble_msg_JP.bmg");
    mTtsPlayer.InitEngine();
    mCharOffset += mWindow.Init(mCharOffset, &mMessageFile, 2, 0x10, &mTtsPlayer, FALSE);
    mWindow.mText[0].mVAlign = 1;
    mWindow.Hide();

    VramManagers_Reset(&gVramManagerWork);
    gScreenControl.mUnknown48 = 1;
    gScreenControl.mUnknown48Changed = TRUE;
    G3X_SetClearColor(GX_RGB(15, 15, 15), 0, 0x7FFF, 63, FALSE);
    ScreenLoader_SetPriority(&gScreenLoader, 1, 1);
    gScreenControl.Set(0x1011);
    Clock_GetDateTime(gGameClock, &date, &time);
    mSky.Init("SKY", 1);
    PaletteLoader_SetBackdrop(&gPaletteLoader, mSky.mPalette[1], 0x101);
    gScreenControl.mUnknown44 = 0;
    gScreenControl.mUnknown44Changed = TRUE;
    Camera_Reset(&gCamera);
    gCamera.mUnknown10 *= 10;
    Light_Reset(&gLights[0]);
    Light_Reset(&gLight1);
    if (mPlace != PLACE_CLASSROOM) {
        gLights[0].mColor = GetSkyLightColor(mSky.GetPhase());
        gLight1.mColor = GetSkyLightColor2(mSky.GetPhase());
    }

    for (int i = 0; i < 2; i++) {
        GameData_ReadCharacter(&gGameData, mCharacterIndices[i], &mCharacters[i]);
        mCharacters[i].mIndex = mCharacterIndices[i];
    }
    clothes[0] = mCharacters[0].mClothesId;
    clothes[1] = mCharacters[1].mClothesId;
    if (mPlace == PLACE_CLASSROOM) {
        ChooseSecondClothes(clothes[0], MiiData_GetSex(&mCharacters[0]), &clothes[1]);
    }
    void *animations = ModelResources_BindArchive(gModelResources, 2, "3A0", 0);
    for (int i = 0; i < 2; i++) {
        SceneCharacter *character = &mCharacters[i];
        ModelResources_SetModel(gModelResources, i, mCharacters[i].mModelPath);
        character->Character::Init(i, 0, 0);
        GameData_ReadCharacter(&gGameData, mCharacterIndices[i], character);
        mCharacters[i].mIndex = mCharacterIndices[i];
        ModelResources_ReleaseModel(gModelResources, i);
        ArchiveManager_Unmount(&gArchiveManager, mCharacters[i].mArchiveName);
        character->CharacterModel::SetArchive(mCharacters[i].mArchiveName, clothes[i], MiiData_GetSex(character));
        ArchiveManager_MountCompressed(&gArchiveManager, mCharacters[i].mArchiveName, mCharacters[i].mArchivePath, 1);
        ModelResources_SetModel(gModelResources, i, mCharacters[i].mModelPath);
        character->CharacterModel::MakeModel(i, 0);
        character->CharacterBase::MakeFace();
        character->Character::UpdateBodyColors();
        character->Character::UpdateBodyScale();
        character->Character::UpdateAnimSpeed();
        mCharacters[i].mPersonality = character->GetPersonality();
        character->UpdateBiorhythm();
        character->Character::UpdateSpeedScales();
        character->CharacterBase::SetFace(8);
        ResourceSet_SetArchive((void *)mCharacters[i].mAnim.mUnknown04, RESOURCE_BCA0, animations);
        character->Character::SetAnimation(
            RESOURCE_BCA0, ResourceSet_FindIndexByName((void *)mCharacters[i].mAnim.mUnknown04, RESOURCE_BCA0, "idle00"));
        mCharacters[i].mShadowVisible = TRUE;
    }

    switch (mPlace) {
    case PLACE_SEA:
        ModelResources_SetModel(gModelResources, 3, "3SS:/sea.imd");
        ModelResources_SetModel(gModelResources, 3, "3SS:/sea.ima");
        ModelResources_SetModel(gModelResources, 3, "3SS:/sea.ica");
        ModelResources_SetModel(gModelResources, 3, "3SS:/sea.ita");
        mModel.Load(3, 0);
        mModel.SetAnimation(RESOURCE_BMA0, ResourceSet_FindIndexByName((void *)mModel.mUnknown04, RESOURCE_BMA0, "sea"));
        mModel.SetAnimation(RESOURCE_BCA0, ResourceSet_FindIndexByName((void *)mModel.mUnknown04, RESOURCE_BCA0, "sea"));
        mModel.SetAnimation(RESOURCE_BTA0, ResourceSet_FindIndexByName((void *)mModel.mUnknown04, RESOURCE_BTA0, "sea"));
        AnimModel_FindAnimation(&mModel, RESOURCE_BTA0,
                                ResourceSet_FindIndexByName((void *)mModel.mUnknown04, RESOURCE_BTA0, "sea"))
            ->mPlayOnce = FALSE;
        break;
    case PLACE_ROOF:
        ModelResources_SetModel(gModelResources, 3, "3SS:/roof.imd");
        mModel.Load(3, 0);
        break;
    case PLACE_TREE:
        ModelResources_SetModel(gModelResources, 3, "3SS:/tree.imd");
        mModel.Load(3, 0);
        break;
    case PLACE_CLASSROOM:
        ModelResources_SetModel(gModelResources, 3, "3SS:/classroom.imd");
        mModel.Load(3, 0);
        break;
    }

    PaletteLoader_LoadFile(&gPaletteLoader, "SCT:/Obj_down.ncl", 0x1000, -1, -1, -1);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "SCT:/Obj_down.ncg", 0x1000, 0, 0, 0, -1);
    ObjResourceManager_LoadCell(&gObjResourceManager, "SCT:/Obj_down.nce", 0x1000, 0);
    PaletteLoader_LoadFile(&gPaletteLoader, "SCT:/BG_up.ncl", 0x100, -1, -1, -1);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "SCT:/BG_up.ncg", 0x100, 0, 0, 0, -1);
    ScreenLoader_LoadFile(&gScreenLoader, "SCT:/BG_up.nsc", 0x100, 0, 0, 0, 0, -1);
    PaletteLoader_LoadFile(&gPaletteLoader, "SCT:/BG_sub.ncl", 2, -1, -1, -1);
    VramTransferQueue_LoadFile(&gVramTransferQueue, "SCT:/BG_sub.ncg", 2, 0, 0, 0, -1);
    ScreenLoader_LoadFile(&gScreenLoader, "SCT:/BG_dialogue.nsc", 2, 0, 0, 0, 0, -1);
    ScreenLoader_SetPriority(&gScreenLoader, 1, 1);
    ScreenLoader_SetPriority(&gScreenLoader, 2, 0);
    gScreenControl.mPlanes &= ~2;
    gScreenControl.mPlanesChanged = TRUE;
    ScreenControl_SetBlendAlpha(&gScreenControl, 2, 0x13D, 8, 8);
    mMap.Init(0, &mCharacters[0], &mCharacters[1]);

    mRejected = FALSE;
    mOutright = FALSE;
    if (!mStandalone) {
        FindPartners();
        DecideAnswer(&mRejected, &mOutright);
    }
    mScript = GetScript(mVariant, mPlace, mRejected, mOutright);

    int camera = GetStep(mScript, 0)->mCamera;
    gCamera.mPosition.x = sCameras[camera].mPosition.x;
    gCamera.mPosition.y = sCameras[camera].mPosition.y;
    gCamera.mPosition.z = sCameras[camera].mPosition.z;
    gCamera.mTarget.x = sCameras[camera].mTarget.x;
    gCamera.mTarget.y = sCameras[camera].mTarget.y;
    gCamera.mTarget.z = sCameras[camera].mTarget.z;
    const RomancePlacement *placement = &sPlacements[GetStep(mScript, 0)->mPlacement];
    for (int i = 0; i < 2; i++) {
        mCharacters[i].SetPos(placement->mPositions[i]);
        mCharacters[i].SetAngle(placement->mAngles[i]);
        mCharacters[i].Character::Update();
    }
    static const VecFx32 sLight0Direction = {0x640, -0x258, -0xFA0};
    static const VecFx32 sLight1Direction = {0, -0xFA0, 0x5DC};
    Light_SetDirectionByCamera(&gLights[0], &gCamera, sLight0Direction);
    Light_SetDirectionByCamera(&gLight1, &gCamera, sLight1Direction);
}

void RomanceScene::Update() {
    switch (mState) {
    case STATE_IDLE:
        break;
    case STATE_SCRIPT:
        UpdateScript();
        break;
    case STATE_2:
        break;
    }
    mTtsPlayer.Update();
}

// One frame of the script: starts the next step, or runs the current one until it has done all it waits for
void RomanceScene::UpdateScript() {
    mCharacters[0].Character::Update();
    mCharacters[1].Character::Update();
    mStep = mNextStep;
    if (mStepPending) {
        StartStep(mStep);
    } else {
        UpdateSpeech();
        if (UpdateStep()) {
            NextStep();
        }
    }
    mFrame++;
}

void RomanceScene::Draw() {
    mWindow.Draw();
    for (int i = 0; i < 2; i++) {
        mCharacters[i].Character::Draw();
    }
    mModel.Draw();
}

void RomanceScene::Exit() {
    gSoundManager.StopSeq(0, 0);
    GameData_WriteCharacter(&gGameData, mCharacters[0].mIndex, &mCharacters[0]);
    GameData_WriteCharacter(&gGameData, mCharacters[1].mIndex, &mCharacters[1]);
    ModelResources_ReleaseModel(gModelResources, 0);
    ModelResources_ReleaseModel(gModelResources, 1);
    ModelResources_ReleaseModel(gModelResources, 2);
    ModelResources_ReleaseModel(gModelResources, 3);
    ArchiveManager_Unmount(&gArchiveManager, "3M0");
    ArchiveManager_Unmount(&gArchiveManager, "3M1");
    ArchiveManager_Unmount(&gArchiveManager, "3A0");
    ArchiveManager_Unmount(&gArchiveManager, "3SS");
    ArchiveManager_Unmount(&gArchiveManager, "SCT");
    ArchiveManager_Unmount(&gArchiveManager, "SKY");
    ArchiveManager_Unmount(&gArchiveManager, "TTM");
}

void RomanceScene::StartStep(int step) {
    mStepPending = FALSE;
    mFrame = 0;
    mWaiting = 0;
    if (GetStep(mScript, step)->mFlags & STEP_WAIT) {
        mWaitFrames = GetStep(mScript, step)->mWaitFrames;
        mWaiting += STEP_WAIT;
    }
    if (GetStep(mScript, step)->mFlags & STEP_ANIMATE) {
        const RomanceStep *s = GetStep(mScript, step);
        int c = s->mAnimCharacter;
        mCharacters[c].Character::BlendToAnimation(
            RESOURCE_BCA0, ResourceSet_FindIndexByName((void *)mCharacters[c].mAnim.mUnknown04, RESOURCE_BCA0, s->mAnimName));
        AnimModel_FindAnimation(&mCharacters[c].mAnim, RESOURCE_BCA0, mCharacters[c].mAnimId)->mPlayOnce = TRUE;
        mWaiting += STEP_ANIMATE;
    }
    if (GetStep(mScript, step)->mFlags & STEP_ANIMATE_HOLD) {
        const RomanceStep *s = GetStep(mScript, step);
        int c = s->mAnimCharacter;
        mCharacters[c].Character::BlendToAnimation(
            RESOURCE_BCA0, ResourceSet_FindIndexByName((void *)mCharacters[c].mAnim.mUnknown04, RESOURCE_BCA0, s->mAnimName));
        AnimModel_FindAnimation(&mCharacters[c].mAnim, RESOURCE_BCA0, mCharacters[c].mAnimId)->mPlayOnce = TRUE;
        mWaiting += STEP_ANIMATE_HOLD;
    }
    if (GetStep(mScript, step)->mFlags & STEP_ANIMATE_LOOP) {
        const RomanceStep *s = GetStep(mScript, step);
        int c = s->mAnimCharacter;
        mCharacters[c].Character::BlendToAnimation(
            RESOURCE_BCA0, ResourceSet_FindIndexByName((void *)mCharacters[c].mAnim.mUnknown04, RESOURCE_BCA0, s->mAnimName));
        AnimModel_FindAnimation(&mCharacters[c].mAnim, RESOURCE_BCA0, mCharacters[c].mAnimId)->mPlayOnce = FALSE;
        mWaiting += STEP_ANIMATE_LOOP;
    }
    if (GetStep(mScript, step)->mFlags & STEP_SPEAK) {
        mSpeaker = GetStep(mScript, step)->mSpeaker;
        mCharacters[mSpeaker].SetVoiceOf(&mTtsPlayer);
        Speak(GetStep(mScript, step)->mMessage);
        mWaiting += STEP_SPEAK;
    }
    if (GetStep(mScript, step)->mFlags & STEP_SPEAK_ON) {
        mSpeaker = GetStep(mScript, step)->mSpeaker;
        mCharacters[mSpeaker].SetVoiceOf(&mTtsPlayer);
        Speak(GetStep(mScript, step)->mMessage);
        mWaiting += STEP_SPEAK_ON;
    }
    if (GetStep(mScript, step)->mFlags & STEP_WAIT_SPEECH) {
        mWaiting += STEP_WAIT_SPEECH;
    }
    if (GetStep(mScript, step)->mFlags & STEP_VOICE_EFFECT_ON) {
        mVoiceEffect = TRUE;
        mWaiting += STEP_VOICE_EFFECT_ON;
    }
    if (GetStep(mScript, step)->mFlags & STEP_VOICE_EFFECT_OFF) {
        mVoiceEffect = FALSE;
        mWaiting += STEP_VOICE_EFFECT_OFF;
    }
    if (GetStep(mScript, step)->mFlags & STEP_WALK) {
        const RomanceStep *s = GetStep(mScript, step);
        int c = s->mWalker;
        mCharacters[c].SetWalkTarget(s->mWalkTarget);
        mCharacters[c].Character::BlendToAnimation(
            RESOURCE_BCA0, ResourceSet_FindIndexByName((void *)mCharacters[c].mAnim.mUnknown04, RESOURCE_BCA0, "walk00"));
        mWaiting += STEP_WALK;
    }
    if (GetStep(mScript, step)->mFlags & STEP_TURN) {
        const RomanceStep *s = GetStep(mScript, step);
        int c = s->mTurner;
        mCharacters[c].SetWalkTarget(s->mTurnTarget);
        mCharacters[c].Character::BlendToAnimation(
            RESOURCE_BCA0, ResourceSet_FindIndexByName((void *)mCharacters[c].mAnim.mUnknown04, RESOURCE_BCA0, "walk00"));
        mWaiting += STEP_TURN;
    }
    if (GetStep(mScript, step)->mFlags & STEP_TURN_HEAD) {
        const RomanceStep *s = GetStep(mScript, step);
        int c = s->mHeadCharacter;
        int direction = s->mHeadDirection;
        mCharacters[c].mHeadTurn = 1;
        mCharacters[c].mHeadPitch = sHeadPitches[direction];
        mCharacters[c].mHeadYaw = sHeadYaws[direction];
        mWaiting += STEP_TURN_HEAD;
    }
    if (GetStep(mScript, step)->mFlags & STEP_START_MUSIC) {
        gSoundManager.StartSeqIfNotPlaying(0, 0x14);
        mWaiting += STEP_START_MUSIC;
    }
    if (GetStep(mScript, step)->mFlags & STEP_FADE_MUSIC) {
        gSoundManager.MoveSeqVolume(0, 0, 0x78);
        mWaiting += STEP_FADE_MUSIC;
    }

    int camera = GetStep(mScript, step)->mCamera;
    gCamera.mPosition.x = sCameras[camera].mPosition.x;
    gCamera.mPosition.y = sCameras[camera].mPosition.y;
    gCamera.mPosition.z = sCameras[camera].mPosition.z;
    gCamera.mTarget.x = sCameras[camera].mTarget.x;
    gCamera.mTarget.y = sCameras[camera].mTarget.y;
    gCamera.mTarget.z = sCameras[camera].mTarget.z;
    static const VecFx32 sLight0Direction = {0x640, -0x258, -0xFA0};
    static const VecFx32 sLight1Direction = {0, -0xFA0, 0x5DC};
    Light_SetDirectionByCamera(&gLights[0], &gCamera, sLight0Direction);
    Light_SetDirectionByCamera(&gLight1, &gCamera, sLight1Direction);
    const RomancePlacement *placement = &sPlacements[GetStep(mScript, step)->mPlacement];
    for (int i = 0; i < 2; i++) {
        mCharacters[i].SetPos(placement->mPositions[i]);
        mCharacters[i].SetAngle(placement->mAngles[i]);
    }
}

// The speaker's lips follow the speech, and the window its time
void RomanceScene::UpdateSpeech() {
    if (!mTtsPlayer.IsEngineIdle()) {
        mCharacters[mSpeaker].LipSync(mTtsPlayer.GetPhonemeName(), 1);
    }
    mWindow.Update();
}

// Clears the flags of what has finished; TRUE when nothing is left to wait for
BOOL RomanceScene::UpdateStep() {
    if (mWaiting & STEP_WAIT) {
        if (mFrame > mWaitFrames) {
            mWaiting ^= STEP_WAIT;
        }
    }
    if (mWaiting & STEP_ANIMATE) {
        int c = GetStep(mScript, mStep)->mAnimCharacter;
        if (AnimModel_FindAnimation(&mCharacters[c].mAnim, RESOURCE_BCA0, mCharacters[c].mAnimId)->mFinished) {
            mCharacters[c].Character::BlendToAnimation(
                RESOURCE_BCA0, ResourceSet_FindIndexByName((void *)mCharacters[c].mAnim.mUnknown04, RESOURCE_BCA0, "idle00"));
            mWaiting ^= STEP_ANIMATE;
        }
    }
    if (mWaiting & STEP_ANIMATE_HOLD) {
        int c = GetStep(mScript, mStep)->mAnimCharacter;
        if (AnimModel_FindAnimation(&mCharacters[c].mAnim, RESOURCE_BCA0, mCharacters[c].mAnimId)->mFinished) {
            mWaiting ^= STEP_ANIMATE_HOLD;
        }
    }
    if (mWaiting & STEP_ANIMATE_LOOP) {
        mWaiting ^= STEP_ANIMATE_LOOP;
    }
    if (mWaiting & STEP_SPEAK) {
        if (mTtsPlayer.IsEngineIdle()) {
            mWaiting ^= STEP_SPEAK;
        }
    }
    if (mWaiting & STEP_SPEAK_ON) {
        mWaiting ^= STEP_SPEAK_ON;
    }
    if (mWaiting & STEP_WAIT_SPEECH) {
        if (mTtsPlayer.IsEngineIdle()) {
            mWaiting ^= STEP_WAIT_SPEECH;
        }
    }
    if (mWaiting & STEP_VOICE_EFFECT_ON) {
        mWaiting ^= STEP_VOICE_EFFECT_ON;
    }
    if (mWaiting & STEP_VOICE_EFFECT_OFF) {
        mWaiting ^= STEP_VOICE_EFFECT_OFF;
    }
    if (mWaiting & STEP_WALK) {
        int c = GetStep(mScript, mStep)->mWalker;
        if (mCharacters[c].WalkToTarget(mCharacters[c].ScaleBySpeed(0x7A), 0x22, 4)) {
            mCharacters[c].Character::BlendToAnimation(
                RESOURCE_BCA0, ResourceSet_FindIndexByName((void *)mCharacters[c].mAnim.mUnknown04, RESOURCE_BCA0, "idle00"));
            mWaiting ^= STEP_WALK;
        }
        mCharacters[c].MoveAlongAngle();
        mCharacters[c].PlaySoundOnce(1, 0, 0x1A);
    }
    if (mWaiting & STEP_TURN) {
        int c = GetStep(mScript, mStep)->mTurner;
        if (mCharacters[c].TurnToWalkTarget(0x180, 0)) {
            mCharacters[c].Character::BlendToAnimation(
                RESOURCE_BCA0, ResourceSet_FindIndexByName((void *)mCharacters[c].mAnim.mUnknown04, RESOURCE_BCA0, "idle00"));
            mWaiting ^= STEP_TURN;
        }
        mCharacters[c].MoveAlongAngle();
    }
    if (mWaiting & STEP_TURN_HEAD) {
        mWaiting ^= STEP_TURN_HEAD;
    }
    if (mWaiting & STEP_START_MUSIC) {
        mWaiting ^= STEP_START_MUSIC;
    }
    if (mWaiting & STEP_FADE_MUSIC) {
        mWaiting ^= STEP_FADE_MUSIC;
    }
    return mWaiting == 0;
}

// Goes on to the next step, or at the script's end applies the answer and leaves
void RomanceScene::NextStep() {
    if (mNextStep < sScripts[mScript].mCount - 1) {
        mNextStep++;
        mStepPending = TRUE;
        return;
    }
    if (!mStandalone) {
        ApplyAnswer(mRejected);
        SetConfessionFlags();
        gSceneMemory.mCharacters[0] = mCharacters[0].mIndex;
        gSceneMemory.mCharacters[1] = mCharacters[1].mIndex;
        GetSceneMemory().mRejected = mRejected;
        gSceneManager.RequestScene(0x40);
    } else {
        gSceneManager.RequestScene(0);
    }
}

// Each one's lover or spouse among the saved characters
void RomanceScene::FindPartners() {
    for (int i = 0; i < 2; i++) {
        mHasPartner[i] = FALSE;
        mCommitted[i] = FALSE;
        mPartners[i] = -1;
        for (int j = 0; j < 100; j++) {
            if (GameData_CharacterExists(&gGameData, j) && j != mCharacters[i].mIndex) {
                int rank = mCharacters[i].mRelationRank[j];
                if (rank == RELATION_LOVERS || rank == RELATION_SPOUSES || rank == RELATION_LOVERS_FIGHTING ||
                    rank == RELATION_SPOUSES_FIGHTING) {
                    mHasPartner[i] = TRUE;
                    mPartners[i] = j;
                    if (mCharacters[i].mRelationRank[j] == RELATION_SPOUSES ||
                        mCharacters[i].mRelationRank[j] == RELATION_SPOUSES_FIGHTING ||
                        (mCharacters[i].mRelationMeter[j] > 50 && mCharacters[i].mRelationRank[j] == RELATION_LOVERS)) {
                        mCommitted[i] = TRUE;
                    }
                }
            }
        }
    }
}

// The answer of the second to the first
void RomanceScene::DecideAnswer(BOOL *rejected, BOOL *outright) {
    *outright = FALSE;
    int rank = mCharacters[0].mRelationRank[mCharacters[1].mIndex];
    int meter = mCharacters[1].mRelationMeter[mCharacters[0].mIndex];
    if (rank == RELATION_FRIENDS || rank == RELATION_BEST_FRIENDS) {
        int half = meter / 2;
        int chance = mCharacters[1].GetPersonalityPairValue(mCharacters[0].mPersonality, mCharacters[1].mPersonality) / 5 - 10 +
                     half * half / 100;
        int kind = data_02114d78[mCharacters[1].mTroubleCategory][mCharacters[1].mTroubleId].kind;
        if (kind == 11) {
            chance += 20;
        }
        if (mCommitted[0] != FALSE || mCommitted[1] != FALSE) {
            chance = 0;
        }
        if (kind == 5 && mCharacters[1].mTroubleTarget == mCharacters[0].mIndex) {
            chance = 100;
        }
        if (chance > MATH_Rand16(&gRandom, 100)) {
            *rejected = FALSE;
            return;
        }
        *rejected = TRUE;
        if (meter <= 55) {
            *outright = TRUE;
        }
    } else if (rank == RELATION_LOVERS || rank == RELATION_SPOUSES) {
        *rejected = FALSE;
    } else if (rank == RELATION_NONE || rank == RELATION_EX_LOVERS || (rank >= RELATION_DIVORCED && rank <= RELATION_SPOUSES_FIGHTING)) {
        *rejected = TRUE;
        *outright = TRUE;
    }
}

// The lovers the two leave: each such partner is now an ex, heartbroken, and its long activity ends
void RomanceScene::EndPartnerships() {
    s64 now = Clock_GetSeconds(gGameClock);
    for (int i = 0; i < 2; i++) {
        if (mHasPartner[i]) {
            SavedCharacter partner;
            GameData_ReadCharacter(&gGameData, mPartners[i], &partner);
            partner.mIndex = mPartners[i];
            if (partner.mRelationRank[mCharacters[i].mIndex] == RELATION_LOVERS ||
                partner.mRelationRank[mCharacters[i].mIndex] == RELATION_LOVERS_FIGHTING) {
                partner.SetRelation(mCharacters[i].mIndex, RELATION_EX_LOVERS, 100);
                mCharacters[i].SetRelation(partner.mIndex, RELATION_EX_LOVERS, 100);
                BOOL heartbreak = TRUE;
                if (partner.IsInTrouble()) {
                    int kind = data_02114d78[partner.mTroubleCategory][partner.mTroubleId].kind;
                    if ((kind == 7 || kind == 8) && partner.mTroubleTarget != mCharacters[i].mIndex) {
                        heartbreak = FALSE;
                    }
                }
                if (heartbreak) {
                    partner.ClearTrouble();
                    partner.mTroubleCategory = 4;
                    partner.mTroubleId = 1;
                    partner.mTroubleState = 1;
                    partner.mTroubleTarget = mCharacters[i].mIndex;
                    partner.mTroubleEnd = now + data_02114d78[4][1].minutes * 60;
                    partner.mMood = 3;
                    partner.ResetTroubleProgress();
                    partner.mSatisfyMeter = 0;
                    if (partner.IsLongActPaired()) {
                        SavedCharacter other;
                        u8 index = partner.mLongActPartner;
                        if (mCharacters[0].mIndex == index) {
                            mCharacters[0].ClearLongAct();
                        } else if (index == mCharacters[1].mIndex) {
                            mCharacters[1].ClearLongAct();
                        } else {
                            GameData_ReadCharacter(&gGameData, index, &other);
                            other.mIndex = index;
                            other.ClearLongAct();
                            GameData_WriteCharacter(&gGameData, index, &other);
                        }
                    }
                    partner.ClearLongAct();
                }
            }
            GameData_WriteCharacter(&gGameData, partner.mIndex, &partner);
        }
    }
}

// What the answer does to the two
void RomanceScene::ApplyAnswer(int rejected) {
    switch (rejected) {
    case FALSE:
        switch (mCharacters[0].mRelationRank[mCharacters[1].mIndex]) {
        case RELATION_FRIENDS:
        case RELATION_BEST_FRIENDS: {
            EndPartnerships();
            mCharacters[0].SetRelation(mCharacters[1].mIndex, RELATION_LOVERS, 100);
            mCharacters[1].SetRelation(mCharacters[0].mIndex, RELATION_LOVERS, 100);
            mCharacters[1].AddUnknown296(1);
            BOOL happy = TRUE;
            if (mCharacters[1].IsInTrouble()) {
                const TroubleInfo *trouble = &data_02114d78[mCharacters[1].mTroubleCategory][mCharacters[1].mTroubleId];
                if (trouble->mood != 0) {
                    happy = FALSE;
                }
                if (trouble->kind == 11) {
                    mCharacters[1].EndTrouble(Clock_GetSeconds(gGameClock));
                    happy = TRUE;
                } else if (trouble->kind == 5) {
                    if (mCharacters[1].mTroubleTarget == mCharacters[0].mIndex) {
                        int minutes = trouble->cooldownMinutes;
                        mCharacters[1].mTroubleEnd = Clock_GetSeconds(gGameClock) + minutes * 60;
                        mCharacters[1].mTroubleState = 3;
                        happy = TRUE;
                    } else {
                        mCharacters[1].ClearTrouble();
                        happy = TRUE;
                    }
                } else if (trouble->kind == 9) {
                    mCharacters[1].ClearTrouble();
                    happy = TRUE;
                } else if (trouble->kind == 7 || trouble->kind == 8) {
                    if (mPartners[1] == mCharacters[1].mTroubleTarget) {
                        mCharacters[1].ClearTrouble();
                        happy = TRUE;
                    }
                }
            }
            if (happy) {
                mCharacters[1].mMood = 1;
            }
            mCharacters[0].mMood = 1;
            break;
        }
        }
        break;
    case TRUE:
        mCharacters[0].AddUnknown297(1);
        mCharacters[0].mMood = 3;
        mCharacters[0].mRelationMeter[mCharacters[1].mIndex] >>= 1;
        mCharacters[1].mRelationMeter[mCharacters[0].mIndex] >>= 1;
        break;
    }
}

// The flag of the island's progress for a first confession to a man (13) or to a woman (14), once flag 6 is set
void RomanceScene::SetConfessionFlags() {
    if (MiiData_GetSex(&mCharacters[1]) == 0 && gIslandState.GetUnknown44(13) == 0 &&
        gIslandState.GetUnknown44(6) != 0) {
        gIslandState.SetUnknown44(13, 3);
    } else if (MiiData_GetSex(&mCharacters[1]) == 1 && gIslandState.GetUnknown44(14) == 0 &&
               gIslandState.GetUnknown44(6) != 0) {
        gIslandState.SetUnknown44(14, 3);
    }
}

// Nine scripts for each place: two for each variant (accepted, rejected), then the outright rejection
int RomanceScene::GetScript(int variant, int place, int rejected, int outright) {
    switch (place) {
    case PLACE_SEA:
        place = 0;
        break;
    case PLACE_ROOF:
        place = 9;
        break;
    case PLACE_TREE:
        place = 18;
        break;
    case PLACE_CLASSROOM:
        place = 27;
        break;
    }
    if (outright) {
        return place + 8;
    }
    return rejected + (place + variant * 2);
}

// Says a message and shows it in the window, with the voice effect's tags when it is on
void RomanceScene::Speak(u32 message) {
    wchar_t text[0x100];
    if (!mVoiceEffect) {
        mTtsPlayer.Speak((const u16 *)mMessageFile.GetText(message), 0);
        mWindow.Show(message - 1, 0, 0x78);
        return;
    }
    MI_CpuClear16(text, sizeof(text));
    wcscpy(text, L"<\\RVol=-90><\\RSpd=+70>");
    wcscat(text, L"<\\Ecg=90><\\Ecl=3000>");
    wcscat(text, mMessageFile.GetText(message));
    mTtsPlayer.Speak((const u16 *)text, 0);
    mWindow.Show(message - 1, 0, 0x78);
}

// In the classroom: the second's clothes to go with the first's
void RomanceScene::ChooseSecondClothes(int clothes, int sex, int *second) {
    if (sex == 0) {
        if (clothes == 0x96) {
            *second = 3;
        } else if (clothes == 0xCD) {
            *second = 0xCE;
        }
    } else {
        switch (clothes) {
        case 3:
        case 0x86:
            *second = 0x96;
            break;
        case 0xCE:
        case 0xFA:
        case 0xFE:
            *second = 0xCD;
            break;
        }
    }
}

void RomanceMap::Clear() {
    mUnknown000[0] = 0;
    mUnknown000[1] = 0;
    mUnknown000[2] = 0;
    ClearPoints();
}

void RomanceMap::ClearPoints() {
    {
        NNSG2dFVec2 v = {0, 0};
        mUnknown00C = v;
    }
    {
        NNSG2dFVec2 v = {0, 0};
        mUnknown014 = v;
    }
    {
        NNSG2dFVec2 v = {0, 0};
        mUnknown01C = v;
    }
    mUnknown074 = mUnknown078 = 0;
    for (int i = 0; i < 10; i++) {
        mUnknown024[i].x = 0;
        mUnknown024[i].y = 0;
    }
}

void RomanceMap::Init(int resource, Character *first, Character *second) {
    Clear();
    mUnknown07C = TRUE;
    mFirst = first;
    mSecond = second;
    Sprite_Init(&mCameraSprite, resource, 0, 0x80, 0x60, TRUE);
    mCameraSprite.mOverwritePalette = 1;
    Sprite_Init(&mTargetSprite, resource, 1, 0x80, 0x60, TRUE);
    mTargetSprite.mOverwritePalette = 1;
    Sprite_Init(&mFirstSprite, resource, 2, 0x80, 0x60, TRUE);
    mFirstSprite.mOverwritePalette = 1;
    Sprite_Init(&mSecondSprite, resource, 3, 0x80, 0x60, TRUE);
    mSecondSprite.mOverwritePalette = 1;
    PlaceSprites();
    mUnknown330 = 4;
}

void RomanceMap::PlaceSprites() {
    ScreenPoint camera, target, first, second;
    VecFx32 cameraPosition = gCamera.mPosition;
    VecFx32 cameraTarget = gCamera.mTarget;
    ToScreen(&camera, &cameraPosition);
    ToScreen(&target, &cameraTarget);
    VecFx32 firstPosition = mFirst->mPos;
    VecFx32 secondPosition = mSecond->mPos;
    ToScreen(&first, &firstPosition);
    ToScreen(&second, &secondPosition);
    mCameraSprite.SetPosition(FxVec2(camera.x << FX32_SHIFT, camera.y << FX32_SHIFT));
    mTargetSprite.SetPosition(FxVec2(target.x << FX32_SHIFT, target.y << FX32_SHIFT));
    mFirstSprite.SetPosition(FxVec2(first.x << FX32_SHIFT, first.y << FX32_SHIFT));
    mSecondSprite.SetPosition(FxVec2(second.x << FX32_SHIFT, second.y << FX32_SHIFT));
}

// A point of the floor as a pixel of the lower screen, the floor's origin at its centre
void RomanceMap::ToScreen(ScreenPoint *point, const VecFx32 *position) {
    point->x = (FX_Mul(position->x, sMapScale) + (128 << FX32_SHIFT)) >> FX32_SHIFT;
    point->y = (FX_Mul(position->z, sMapScale) + (96 << FX32_SHIFT)) >> FX32_SHIFT;
}

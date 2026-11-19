#ifndef OV075_SCENE40METER_H
#define OV075_SCENE40METER_H

#include <nitro.h>

#include "Sprite.h"
#include "text/TextCanvas.h"

// The meters of the character's status page (the page at scene+0x5AFEC): a value of 0 to 100 that moves to a
// target a step a frame, with a sound while it moves and another when it arrives. Three kinds draw it:
//
//   Scene40SegmentMeter  10 sprites side by side, each showing up to 10 of the value (the trouble meter)
//   Scene40LevelMeter    the same with a level shown beside it, the value being the points within the level: the
//                        satisfaction meter (SatisfyLevel, SatisfyMeter), which rolls over to the next level
//   Scene40SpriteMeter   one sprite whose frame shows the value (the fullness meter)
//
// The base has no table in the ROM: its virtual functions are all overridden, and its constructor's store of its
// table is dead in every derived constructor. The page calls Draw through the table.
//
// The names are descriptive: no name of these classes survives in the ROM.
class Scene40Meter {
public:
    enum State {
        STATE_IDLE,
        STATE_START,  // Start moving to the target, with the sound of the direction
        STATE_MOVING,
        STATE_ARRIVED, // The arrival's sound was played; back to idle in the next frame
    };

    Scene40Meter() {}

    virtual void Draw() = 0;
    virtual void SetPosition(ScreenPoint position) = 0;
    virtual void Show(int value) = 0;
    virtual void SetVisible(BOOL visible) = 0;

    void Init();
    void Update();
    void SetMoveSounds(int seqArc, int riseSound, int fallSound);
    void SetArriveSounds(int seqArc, int riseSound, int fallSound);
    void SetSteps(int riseStep, int fallStep);
    void SetValue(int value);
    void SetTarget(int value);

    /* 0x04 */ int mState;
    /* 0x08 */ BOOL mFalling;     // The value moves down
    /* 0x0C */ int mTarget;
    /* 0x10 */ int mValue;
    /* 0x14 */ BOOL mFull;        // Set for one frame when it arrived at 100
    /* 0x18 */ BOOL mEmpty;       // and when it arrived at 0
    /* 0x1C */ int mRiseStep;     // What the value moves by in a frame
    /* 0x20 */ int mFallStep;
    /* 0x24 */ int mSeqArc;       // The sounds' sequence archive (27)
    /* 0x28 */ int mRiseSound;    // While it moves up
    /* 0x2C */ int mFallSound;    // While it moves down
    /* 0x30 */ int mRiseEndSound; // When it arrives, moving up
    /* 0x34 */ int mFallEndSound; // When it arrives, moving down
}; // 0x38

class Scene40SegmentMeter : public Scene40Meter {
public:
    Scene40SegmentMeter() {}

    virtual void Draw();
    virtual void SetPosition(ScreenPoint position);
    virtual void Show(int value);
    virtual void SetVisible(BOOL visible);

    void Init(int resource, int sequence);

    /* 0x038 */ Sprite mSegments[10];
    /* 0x6F0 */ int mSpacing; // The segments are 10 times this apart, centred on the position
}; // 0x6F4

class Scene40LevelMeter : public Scene40SegmentMeter {
public:
    Scene40LevelMeter() {}

    virtual void Draw();
    virtual void SetVisible(BOOL visible);

    void Init(int *charOffset);
    void Update();
    void SetLevel(int level, int points);
    void SetTargetLevel(int level, int points, BOOL beyondTop);
    void Start();
    void ShowLevel(int level);

    /* 0x6F4 */ BOOL mLevelChanged; // Update sets it in the frame the level goes up
    /* 0x6F8 */ int mLevel;
    /* 0x6FC */ int mPoints;
    /* 0x700 */ int mTargetLevel;
    /* 0x704 */ int mTargetPoints;
    /* 0x708 */ int mTotal;         // mLevel * 100 + mPoints
    /* 0x70C */ int mTargetTotal;
    /* 0x710 */ int mDistance;      // mTargetTotal - mTotal when the target was set
    /* 0x714 */ ObjTextBox mLevelText;
}; // 0x8D0

class Scene40SpriteMeter : public Scene40Meter {
public:
    Scene40SpriteMeter() {}

    virtual void Draw();
    virtual void SetPosition(ScreenPoint position);
    virtual void Show(int value);
    virtual void SetVisible(BOOL visible);

    void Init(int resource, int sequence);

    /* 0x38 */ Sprite mSprite;
}; // 0xE4

#endif // OV075_SCENE40METER_H

#ifndef OV073_TUTORIALAPARTMENTFRONT_H
#define OV073_TUTORIALAPARTMENTFRONT_H

#include <nitro.h>
#include <nnsys.h>

#include "Sprite.h"

// The front of the apartment building on the touch screen, where a room is picked: a background of windows that
// scrolls floor by floor, a cursor on the picked room and a button that scrolls with the building.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM. Overlay 74 has the
// same classes without the prefix, which tells the two apart: the program is linked as one, so they cannot have
// one name.

// The cursor on the picked window: a sprite that is put where TutorialApartmentFront wants it.
class TutorialRoomCursor : public Sprite {
public:
    enum State {
        STATE_IDLE,
        STATE_MOVE, // Go to mTarget
    };

    TutorialRoomCursor();
    void Update();
    void SetState(int state);
    BOOL Contains(ScreenPoint point);
    void SetTarget(fx32 x, fx32 y) {
        mTarget.x = x;
        mTarget.y = y;
    }
    ScreenPoint GetPixelTarget() {
        ScreenPoint point;
        NNSG2dFVec2 target = mTarget;
        point.x = target.x >> FX32_SHIFT;
        point.y = target.y >> FX32_SHIFT;
        ScreenPoint copy = point;
        return copy;
    }

    /* 0xAC */ int mState;
    /* 0xB0 */ NNSG2dFVec2 mTarget;
}; // 0xB8

// The button beside the building (sequence 9 of the scene's sprites, 10 while it is held).
class TutorialBuildingButton : public Sprite {
public:
    enum State {
        STATE_IDLE,
        STATE_PRESS,
        STATE_RELEASE,
        STATE_PLAY,
    };

    TutorialBuildingButton();
    void Init();
    void Update();
    void SetState(int state);
    BOOL Contains(ScreenPoint point);

    /* 0xAC */ int mState;
}; // 0xB0

class TutorialApartmentFront {
public:
    enum State {
        STATE_IDLE,
        STATE_SCROLL, // mScroll is going to mScrollTarget
    };

    // What a room's window shows; the values are the argument of DrawWindowTiles
    enum WindowKind {
        WINDOW_0,
        WINDOW_1,
        WINDOW_2,
        WINDOW_3,
        WINDOW_EMPTY, // Nobody lives in the room
        WINDOW_5,
        WINDOW_6,
        WINDOW_7,
    };

    TutorialApartmentFront();
    void SetBuildingSize(int size);
    void Init(int room);
    void Touch(ScreenPoint point);
    void Update();
    void Draw();
    void SetState(int state);
    void SelectRoom(int room, int touchY);
    int GetRoomAt(ScreenPoint point);
    void GetRoomCell(int room, int *column, int *floor);
    void ClampScrollTarget();
    // Where on the screen a room's window is, in pixels, from its column and floor
    void CellToPosition(int *x, int *y, int scroll) {
        *x = *x * 32 + 16;
        *y = 176 - ((72 - scroll) + *y * 32);
    }
    void UpdateCursorTarget();
    void UpdateButtonPosition();
    int GetWindowKind(int room);
    void DrawWindow(int room);
    void DrawWindowOfKind(int kind, int room);
    void DrawWindowTiles(int kind, u8 column, u8 floor);

    /* 0x000 */ TutorialRoomCursor mCursor;
    /* 0x0B8 */ TutorialBuildingButton mButton;
    /* 0x168 */ int mState;
    /* 0x16C */ fx32 mScroll;        // How far up the building the view is, in pixels
    /* 0x170 */ fx32 mScrollTarget;
    /* 0x174 */ int mUnknownClearedByConstructor;
    /* 0x178 */ u16 *mScreen;        // The screen (tile map) of the building's background
    /* 0x17C */ char mArchivePath[0x1C]; // "Scn/AFwBG/Apart0_LZ.bin", the digit being mBuildingSize
    /* 0x198 */ u16 mSelectedRoom;
    /* 0x19A */ u8 mBuildingSize;    // 0 to 3: the building has 24, 48, 72 or 100 rooms
    /* 0x19B */ u8 mUnknown19B;
    /* 0x19C */ u8 mResidents[100];  // By room: which character lives there, 0xFF if nobody
}; // 0x200

#endif // OV073_TUTORIALAPARTMENTFRONT_H

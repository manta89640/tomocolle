#ifndef OV075_SCENE40MESSAGESEQUENCE_H
#define OV075_SCENE40MESSAGESEQUENCE_H

#include <nitro.h>

#include "MessageWindow.h"

class SavedCharacter;

// A chain of messages in the scene's two message windows (scene+0x2C908, 0x021505A4 to 0x021505F0): each entry of
// a table (sMessageEntries, 0x0217D3DC) says in which window its message goes, how long to wait before opening it
// and after closing it, whether it names the character, and which entry follows. A window stays open until one of
// its buttons is pressed.
class Scene40MessageSequence {
public:
    enum State {
        STATE_IDLE = 0,
        STATE_START = 1,   // Puts the entry's message in its window
        STATE_DELAY = 2,   // Waits before opening it
        STATE_OPENING = 3, // Waits for the window to open
        STATE_OPEN = 4,    // Waits for a button
        STATE_CLOSING = 5, // Waits for the window to close
        STATE_WAIT = 6,    // Waits after closing it
        STATE_NEXT = 7,    // Goes on to the next entry, or ends
        STATE_DONE = 8,
    };
    enum {
        ENTRY_NONE = 21, // The entry after the last
    };

    // One message of a chain
    struct Entry {
        /* 0x00 */ int mId;       // Its index in the table
        /* 0x04 */ int mWindow;   // 0 or 1
        /* 0x08 */ u32 mMessage;  // In the scene's own messages (TEM)
        /* 0x0C */ int mDelay;    // Frames before it opens
        /* 0x10 */ int mWait;     // Frames after it closes
        /* 0x14 */ int mArgKind;  // 1: the character's name is its string argument
        /* 0x18 */ int mNext;     // The entry after it, or ENTRY_NONE
    }; // 0x1C

    void Init(MessageWindow *window0, MessageWindow *window1);
    void SetCharacter(SavedCharacter *character);
    void Update();

    /* 0x00 */ MessageWindow *mWindows[2];
    /* 0x08 */ u8 mUnknown08[0x4];
    /* 0x0C */ int mEntry;    // The entry shown
    /* 0x10 */ int mLastState; // mState when Update began
    /* 0x14 */ int mState;
    /* 0x18 */ int mWindow;   // The entry's window
    /* 0x1C */ int mFrames;
    /* 0x20 */ int mDelay;
    /* 0x24 */ int mWait;
    /* 0x28 */ int mPrevious; // The entry before, or ENTRY_NONE
    /* 0x2C */ int mNext;
    /* 0x30 */ int mReturnState; // What the page that started the chain goes on to when it is done (the pages set it)
    /* 0x34 */ wchar_t mName[11]; // The character's
    /* 0x4A */ u8 mUnknown4A[0x2];
}; // 0x4C

#endif // OV075_SCENE40MESSAGESEQUENCE_H

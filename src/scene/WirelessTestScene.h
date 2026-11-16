#ifndef SCENE_WIRELESSTESTSCENE_H
#define SCENE_WIRELESSTESTSCENE_H

#include "MainProgram3d.h"

#include "../../lib/WH/wh.h"

// Scene 27 (main program, no overlay): a test of wireless play through WH, on a text canvas of background 2. Touch
// switches between sending (the parent) and receiving (a child); START begins. The parent measures the channels,
// connects, and once a child is there sends a saved character chosen with left and right (the name or "データが
// ありません" shown) in four packets of 0x80 bytes. The child scans, lists up to three parents by the nickname each
// sends as its user game info, connects to the one chosen with up and down, and stores what it receives as a new
// saved character in the first free slot. After a transfer a touch asks for scene 0x11. Exit ends WH. The scene
// factory (main func_0203c050, case 27) allocates 0xCC bytes.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.
class WirelessTestScene : public Scene {
public:
    enum State {
        STATE_IDLE,       // "スタートで通信開始"
        STATE_CONNECTING, // Measuring the channel (parent), or choosing a parent (child)
        STATE_CONNECTED,  // Choosing the character (parent), waiting for it (child)
        STATE_SENDING,
        STATE_DONE,       // "...成功(タッチで終了)"
        STATE_ERROR,
    };

    enum { PARENT_MAX = 3 };

    virtual void StartLoading();
    virtual void Init();
    virtual void Update();
    virtual void Draw();
    virtual void Exit();

    void UpdateIdle();
    void ConnectToParent();
    void Redraw();
    static void OnSent(BOOL result);
    static void OnReceive(u16 aid, u16 *data, u16 size);
    static void OnParentFound(WMBssDesc *bssDesc);

    /* 0x10 */ BgTextCanvas mCanvas;
    /* 0x7C */ int mRole;           // 0 the parent (sends), 1 a child (receives)
    /* 0x80 */ int mState;
    /* 0x84 */ int mSelectedParent;
    /* 0x88 */ wchar_t mParentNames[PARENT_MAX][11];
    /* 0xCA */ u16 mCharacter;      // The saved character the parent sends
}; // 0xCC

#endif // SCENE_WIRELESSTESTSCENE_H

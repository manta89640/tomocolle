#ifndef WIRELESS_WIRELESSSESSION_H
#define WIRELESS_WIRELESSSESSION_H

#include <nitro.h>

// The game's layer over WH (lib/WH) for finding another DS and connecting to it as a child, which the wireless
// scenes (overlays 109 and 110) drive: they request a step (scan, measure the channels, connect, end) and the
// step's Update function carries it out once. While it scans, the parents found are kept in a buffer the scene
// gives (SetParentBuffer), each with a timer the scene counts down (SetTimers): 600 when a parent is found, 599
// when it is seen again. A parent is one of this game only if its user game information ends with the same
// 16-bit id (mGameInfoId) as this DS's: the user game information is the owner's nickname, 15 characters, and that
// id (MakeUserGameInfo).
//
// The one instance registers itself (gWirelessSession) for the scan callback. The names are descriptive, not the
// original: no name of this class survives in the ROM.
class WirelessSession {
public:
    WirelessSession();
    void Reset(u16 gameInfoId);
    void SetParentBuffer(WMBssDesc *parents, int count);
    void SetTimers(int *timers);
    void Initialize();
    BOOL UpdateEnd();
    void MakeUserGameInfo();
    void SetSsid();
    void SetUserGameInfo();
    void RequestEnd(BOOL request);
    BOOL DoUpdateEnd();
    void RequestScan(BOOL request);
    void UpdateScan();
    void RequestMeasureChannel(BOOL request);
    void UpdateMeasureChannel();
    void RequestConnect(BOOL request);
    BOOL UpdateConnect();
    BOOL IsParentFound();
    int CountParents();
    void ClearParentFound();
    int GetFoundParent();
    void GetFoundParentName(wchar_t *name);
    void ClearParents();
    WMBssDesc *GetParents();
    int GetParentCount();
    int *GetTimers();
    void EndScan(int parent);

    static void OnParentFound(WMBssDesc *bssDesc);

    /* 0x00 */ BOOL mEndRequested;     // UpdateEnd ends WH once it is idle
    /* 0x04 */ BOOL mScanRequested;
    /* 0x08 */ BOOL mMeasureRequested;
    /* 0x0C */ BOOL mConnectRequested;
    /* 0x10 */ WMBssDesc *mParents;    // The parents found, mParentCount of them at most
    /* 0x14 */ int mParentCount;
    /* 0x18 */ int *mTimers;           // By parent: 0 for an empty slot
    /* 0x1C */ int mParent;            // Which of the parents found to connect to
    /* 0x20 */ u16 mGameInfoId;
}; // 0x24

// The one instance, which its constructor registers and the scan callback works on. Defined in the scene
// manager's file: it is the first object of that file's .bss (0x0213B700), before gSceneManager's destructor record.
extern WirelessSession *gWirelessSession;

#endif // WIRELESS_WIRELESSSESSION_H

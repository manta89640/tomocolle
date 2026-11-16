#include "wireless/WirelessSession.h"

#include <string.h>

#include "../../lib/WH/wh.h"

// The game's id for WM (WH_SetGgid)
#define GGID 0x1171

// What the scan has found: whether a new parent was found and which, for the scene to read
struct ScanResult {
    /* 0x0 */ BOOL mFound;
    /* 0x4 */ int mParent;
    /* 0x8 */ int mNextParent;
};

static ScanResult sScanResult;
// The user game information: the owner's nickname, and mGameInfoId in the last character
static u16 sUserGameInfo[16] ATTRIBUTE_ALIGN(32);


// Scan for any parent
static const u8 sScanMacAddress[8] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

WirelessSession::WirelessSession() {
    gWirelessSession = this;
    Reset(0);
}

void WirelessSession::Reset(u16 gameInfoId) {
    mEndRequested = FALSE;
    mScanRequested = FALSE;
    mMeasureRequested = FALSE;
    mConnectRequested = FALSE;
    for (int i = 0; i < 16; i++) {
        sUserGameInfo[i] = 0;
    }
    sUserGameInfo[15] = gameInfoId;
    ClearParents();
    mParent = 0;
    mGameInfoId = gameInfoId;
}

void WirelessSession::SetParentBuffer(WMBssDesc *parents, int count) {
    mParents = parents;
    mParentCount = count;
}

void WirelessSession::SetTimers(int *timers) {
    mTimers = timers;
}

void WirelessSession::Initialize() {
    WH_Initialize();
    WH_SetGgid(GGID);
}

BOOL WirelessSession::UpdateEnd() {
    return DoUpdateEnd();
}

void WirelessSession::MakeUserGameInfo() {
    OSOwnerInfo info;
    OS_GetOwnerInfo(&info);
    for (int i = 0; i < 15; i++) {
        if (i < info.nickNameLength) {
            sUserGameInfo[i] = info.nickName[i];
        } else {
            sUserGameInfo[i] = 0;
        }
    }
    sUserGameInfo[15] = mGameInfoId;
}

void WirelessSession::SetSsid() {
    MakeUserGameInfo();
    WH_SetSsid(sUserGameInfo, sizeof(sUserGameInfo));
}

void WirelessSession::SetUserGameInfo() {
    MakeUserGameInfo();
    WH_SetUserGameInfo(sUserGameInfo, sizeof(sUserGameInfo));
}

void WirelessSession::RequestEnd(BOOL request) {
    mEndRequested = request;
}

BOOL WirelessSession::DoUpdateEnd() {
    if (mEndRequested) {
        if (WH_GetSystemState() == WH_SYSSTATE_STOP) {
            mEndRequested = FALSE;
        }
        if (WH_GetSystemState() == WH_SYSSTATE_IDLE) {
            WH_End();
            mEndRequested = FALSE;
        }
    }
    return mEndRequested;
}

void WirelessSession::RequestScan(BOOL request) {
    mScanRequested = request;
}

void WirelessSession::UpdateScan() {
    if (mScanRequested == TRUE) {
        mScanRequested = FALSE;
        ClearParents();
        WH_StartScan(OnParentFound, sScanMacAddress, 0);
    }
}

void WirelessSession::RequestMeasureChannel(BOOL request) {
    mMeasureRequested = request;
}

void WirelessSession::UpdateMeasureChannel() {
    if (mMeasureRequested == TRUE) {
        mMeasureRequested = FALSE;
        WH_StartMeasureChannel();
    }
}

void WirelessSession::RequestConnect(BOOL request) {
    mConnectRequested = request;
}

BOOL WirelessSession::UpdateConnect() {
    if (mConnectRequested != TRUE) {
        return TRUE;
    }
    mConnectRequested = FALSE;
    return WH_ChildConnect(WH_CONNECTMODE_MP_CHILD, &mParents[gWirelessSession->mParent]);
}

BOOL WirelessSession::IsParentFound() {
    return sScanResult.mFound;
}

int WirelessSession::CountParents() {
    int count = 0;
    for (int i = 0; i < mParentCount; i++) {
        if (mTimers[i] > 0) {
            count++;
        }
    }
    return count;
}

void WirelessSession::ClearParentFound() {
    sScanResult.mFound = FALSE;
}

int WirelessSession::GetFoundParent() {
    return sScanResult.mParent;
}

void WirelessSession::GetFoundParentName(wchar_t *name) {
    for (int i = 0; i < 11; i++) {
        name[i] = mParents[sScanResult.mParent].gameInfo.userGameInfo[i];
    }
    sScanResult.mParent = sScanResult.mNextParent;
}

void WirelessSession::ClearParents() {
    sScanResult.mNextParent = 0;
    sScanResult.mParent = 0;
    for (int i = 0; i < mParentCount; i++) {
        mTimers[i] = 0;
    }
}

WMBssDesc *WirelessSession::GetParents() {
    return mParents;
}

int WirelessSession::GetParentCount() {
    return mParentCount;
}

int *WirelessSession::GetTimers() {
    return mTimers;
}

void WirelessSession::EndScan(int parent) {
    WH_EndScan();
    mParent = parent;
}

void WirelessSession::OnParentFound(WMBssDesc *bssDesc) {
    WMBssDesc *parents = gWirelessSession->GetParents();
    int *timers = gWirelessSession->GetTimers();
    if (bssDesc->gameInfo.userGameInfo[15] != gWirelessSession->mGameInfoId) {
        return;
    }
    int i;
    for (i = 0; i < gWirelessSession->GetParentCount(); i++) {
        if (timers[i] != 0 && memcmp(parents[i].bssid, bssDesc->bssid, 12) == 0) {
            timers[i] = 599;
            return;
        }
    }
    sScanResult.mFound = TRUE;
    WH_PrintBssDesc(bssDesc);
    for (i = 0; i < gWirelessSession->GetParentCount(); i++) {
        if (timers[i] == 0) {
            timers[i] = 600;
            parents[i] = *bssDesc;
            sScanResult.mParent = i;
            return;
        }
    }
}

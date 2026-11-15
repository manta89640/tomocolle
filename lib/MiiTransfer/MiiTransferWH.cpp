// WH, the SDK's wireless sample code, a second time: the copy that the Mii transfer (MiiTransfer.cpp, the library
// this directory holds) is built on, beside the game's main WH (lib/WH). This is the project's own decompilation.
//
// It is a later revision than the main copy (it scans with WM_StartScanEx and connects with WM_StartConnectEx), built
// without any of WH's messages, and only the child's side of it is in the ROM: the parent's functions are not, which
// the linker's dead-stripping explains, since nothing calls them. Its statics are reached from the start of its .bss,
// as the compiler does for C++ compiled with -ipa file (and not for C): so it is a C++ file, and its functions' names
// are WH's, mangled, which is how they do not clash with the main copy's. Whether they were in a namespace or a class
// is not known.

#include "MiiTransferWH.h"
#include <nitro/cht.h>

// Measured from the code
#define WH_CHILD_RECV_BUFFER_SIZE   0x280
#define WH_CHILD_SEND_BUFFER_SIZE   0xA0
#define WH_SEND_BUFFER_SIZE         0x120
#define WH_SCAN_BUF_SIZE            0x3C0
#define WH_DS_PORT                  13
#define WH_DS_DATA_SIZE             0x80
#define WH_DATA_PORT                4
#define WH_DATA_PRIO                WM_PRIORITY_NORMAL
#define WH_DMA_NO                   2

// The file's statics. The compiler lays a file's statics out from the start of its .bss, by size (the u16s, the
// words, then the aggregates; arrays of u8 go in sections of their own after them), and where each of one size goes
// follows from the order of all of them here, not from what they are: this order puts each where the ROM has it
// (measured by compiling). The data_* statics are used by none of the functions in the ROM, only, presumably, by
// WH's functions that the link left out: their names and types are not known (data_02145de0 is 0x200 bytes, the size
// of WH's data set for data-sharing, WMDataSet). wh_cpp_StrippedStatics, at the end, keeps them in the file.
static u16 sConnectBitmap;
static u16 data_02145c00;
static u16 sAutoConnectFlag;
static u16 sChannelIndex;
static u16 data_02145c02;
static u16 sMyAid;
static WHStartScanCallbackFunc sScanCallback;
static BOOL sPictoCatchFlag;
static u32 data_02145c2c;
static WHChildWEPKeyGeneratorFunc sChildWEPKeyGenerator;
static WHReceiverFunc sReceiverFunc;
static int sErrCode;
static u32 data_02145c30;
static s32 sRecvBufferSize;
static void *data_02145c38; // Cleared by WH_Initialize
static int sConnectMode;
static s32 sSendBufferSize;
static u32 data_02145c0c;
static u32 data_02145c20;
static int sSysState;

static u16 sWEPKey[20 / sizeof(u16)] ATTRIBUTE_ALIGN(32);
static WMParentParam sParentParam ATTRIBUTE_ALIGN(32);
static WMScanExParam sScanExParam ATTRIBUTE_ALIGN(32);
static WMBssDesc sBssDesc ATTRIBUTE_ALIGN(32);
static WMDataSet data_02145de0 ATTRIBUTE_ALIGN(32);
static WMBssDesc sScanBuf[WH_SCAN_BUF_SIZE / sizeof(WMBssDesc)] ATTRIBUTE_ALIGN(32);
static WMKeySetBuf sWMKeySetBuf ATTRIBUTE_ALIGN(32);
static WMDataSharingInfo sDSInfo ATTRIBUTE_ALIGN(32);
static u8 sSendBuffer[WH_SEND_BUFFER_SIZE] ATTRIBUTE_ALIGN(32);
static u8 sRecvBuffer[WH_CHILD_RECV_BUFFER_SIZE] ATTRIBUTE_ALIGN(32);
static u8 sWmBuffer[WM_SYSTEM_BUF_SIZE] ATTRIBUTE_ALIGN(32);

static void WH_ChangeSysState(int state);
static void WH_SetError(int code);
static BOOL WH_StateInEndParentKeyShare(void);
static BOOL WH_StateInEndParentMP(void);
static void WH_StateOutEndParentMP(void *arg);
static BOOL WH_StateInEndParent(void);
static void WH_StateOutEndParent(void *arg);
static BOOL WH_StateInStartScan(void);
static void WH_StateOutStartScan(void *arg);
static BOOL WH_StateInEndScan(void);
static void WH_StateOutEndScan(void *arg);
static BOOL WH_StateInSetChildWEPKey(void);
static void WH_StateOutSetChildWEPKey(void *arg);
static BOOL WH_StateInStartChild(void);
static void WH_StateOutStartChild(void *arg);
static BOOL WH_StateInStartChildMP(void);
static void WH_StateOutStartChildMP(void *arg);
static BOOL WH_StateInStartChildKeyShare(void);
static BOOL WH_StateInEndChildKeyShare(void);
static BOOL WH_StateInEndChildMP(void);
static void WH_StateOutEndChildMP(void *arg);
static BOOL WH_StateInEndChild(void);
static void WH_StateOutEndChild(void *arg);
static BOOL WH_StateInReset(void);
static void WH_StateOutReset(void *arg);
static BOOL WH_StateInSetMPData(const void *data, u16 datasize, WHSendCallbackFunc callback);
static void WH_StateOutSetMPData(void *arg);
static void WH_PortReceiveCallback(void *arg);
static void WH_StateOutEnd(void *arg);
static void WH_IndicateHandler(void *arg);
static BOOL WH_StateInInitialize(void);
static void WH_StateOutInitialize(void *arg);

static void WH_ChangeSysState(int state) {
    sSysState = state;
}

static void WH_SetError(int code) {
    if (sSysState == WH_SYSSTATE_ERROR || sSysState == WH_SYSSTATE_FATAL) {
        return;
    }
    sErrCode = code;
}

static BOOL WH_StateInEndParentKeyShare(void) {
    WMErrCode result = WM_EndKeySharing(&sWMKeySetBuf);
    if (result != WM_ERRCODE_SUCCESS) {
        WH_SetError(result);
        return FALSE;
    }
    if (!WH_StateInEndParentMP()) {
        WH_Reset();
        return FALSE;
    }
    return TRUE;
}

static BOOL WH_StateInEndParentMP(void) {
    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    WMErrCode result = WM_EndMP(WH_StateOutEndParentMP);
    if (result != WM_ERRCODE_OPERATING) {
        WH_SetError(result);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutEndParentMP(void *arg) {
    WMCallback *cb = (WMCallback *)arg;
    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_SetError(cb->errcode);
        WH_Reset();
        return;
    }
    if (!WH_StateInEndParent()) {
        WH_Reset();
    }
}

static BOOL WH_StateInEndParent(void) {
    WMErrCode result = WM_EndParent(WH_StateOutEndParent);
    if (result != WM_ERRCODE_OPERATING) {
        WH_SetError(result);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutEndParent(void *arg) {
    WMCallback *cb = (WMCallback *)arg;
    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_SetError(cb->errcode);
        return;
    }
    WH_ChangeSysState(WH_SYSSTATE_IDLE);
}

void WH_TurnOffPictoCatch(void) {
    sPictoCatchFlag = FALSE;
}

BOOL WH_StartScan(WHStartScanCallbackFunc callback, const u8 *macAddr, u16 channel) {
    WH_ChangeSysState(WH_SYSSTATE_SCANNING);
    sScanCallback = callback;
    sChannelIndex = channel;
    sAutoConnectFlag = FALSE;
    *(u16 *)(&sScanExParam.bssid[4]) = *(u16 *)(&macAddr[4]);
    *(u16 *)(&sScanExParam.bssid[2]) = *(u16 *)(&macAddr[2]);
    *(u16 *)(&sScanExParam.bssid[0]) = *(u16 *)(&macAddr[0]);
    if (!WH_StateInStartScan()) {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return FALSE;
    }
    return TRUE;
}

static BOOL WH_StateInStartScan(void) {
    u16 allowedChannel = WM_GetAllowedChannel();
    if (allowedChannel == 0x8000) {
        WH_SetError(WM_ERRCODE_ILLEGAL_STATE);
        return FALSE;
    }
    if (allowedChannel == 0) {
        WH_SetError(WH_ERRCODE_NO_RADIO);
        return FALSE;
    }
    sScanExParam.scanBuf = sScanBuf;
    sScanExParam.scanBufSize = WH_SCAN_BUF_SIZE;
    if (sChannelIndex != 0) {
        sScanExParam.channelList = sChannelIndex;
    } else {
        sScanExParam.channelList = WM_GetAllowedChannel();
    }
    sScanExParam.maxChannelTime = WM_GetDispersionScanPeriod();
    sScanExParam.bssid[0] = 0xFF;
    sScanExParam.bssid[1] = 0xFF;
    sScanExParam.bssid[2] = 0xFF;
    sScanExParam.bssid[3] = 0xFF;
    sScanExParam.bssid[4] = 0xFF;
    sScanExParam.bssid[5] = 0xFF;
    sScanExParam.scanType = WM_SCANTYPE_PASSIVE;
    sScanExParam.ssidLength = 0;
    sScanExParam.ssidMatchLength = 0;
    WMErrCode result = WM_StartScanEx(WH_StateOutStartScan, &sScanExParam);
    if (result != WM_ERRCODE_OPERATING) {
        WH_SetError(result);
        return FALSE;
    }
    return TRUE;
}

// Looks through the parents found for one to connect to: a PictoChat parent if those are sought, or one of this
// game that takes entries. Not in WH as the SDK has it.
extern "C" BOOL func_020791c4(WMStartScanExCallback *cb) {
    DC_InvalidateRange(sScanExParam.scanBuf, WH_SCAN_BUF_SIZE);
    for (s16 i = 0; i < cb->bssDescCount; i++) {
        if (sPictoCatchFlag && CHT_IsPictochatParent(cb->bssDesc[i])) {
            MI_CpuCopy8(cb->bssDesc[i], &sBssDesc, sizeof(WMBssDesc));
            if (sScanCallback != NULL) {
                sScanCallback(cb->bssDesc[i]);
            }
            return TRUE;
        }
        if (WM_IsValidGameBeacon(cb->bssDesc[i]) && sParentParam.ggid == cb->bssDesc[i]->gameInfo.ggid) {
            WMBssDesc *bssDesc = cb->bssDesc[i];
            if ((bssDesc->gameInfo.attribute & (WM_ATTR_FLAG_ENTRY | WM_ATTR_FLAG_MB)) != WM_ATTR_FLAG_ENTRY) {
                return FALSE;
            }
            MI_CpuCopy8(bssDesc, &sBssDesc, sizeof(WMBssDesc));
            if (sScanCallback != NULL) {
                sScanCallback(cb->bssDesc[i]);
            }
            return TRUE;
        }
    }
    return FALSE;
}

static void WH_StateOutStartScan(void *arg) {
    WMStartScanExCallback *cb = (WMStartScanExCallback *)arg;
    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_SetError(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    }
    if (sSysState != WH_SYSSTATE_SCANNING) {
        if (!WH_StateInEndScan()) {
            WH_ChangeSysState(WH_SYSSTATE_ERROR);
        }
        return;
    }
    switch (cb->state) {
    case WM_STATECODE_SCAN_START:
        return;
    case WM_STATECODE_PARENT_NOT_FOUND:
        break;
    case WM_STATECODE_PARENT_FOUND:
        if (func_020791c4(cb) && sAutoConnectFlag) {
            if (!WH_StateInEndScan()) {
                WH_ChangeSysState(WH_SYSSTATE_ERROR);
            }
            return;
        }
        break;
    }
    if (!WH_StateInStartScan()) {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
    }
}

BOOL WH_EndScan(void) {
    if (sSysState != WH_SYSSTATE_SCANNING) {
        return FALSE;
    }
    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    return TRUE;
}

static BOOL WH_StateInEndScan(void) {
    WMErrCode result = WM_EndScan(WH_StateOutEndScan);
    if (result != WM_ERRCODE_OPERATING) {
        WH_SetError(result);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutEndScan(void *arg) {
    WMCallback *cb = (WMCallback *)arg;
    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_SetError(cb->errcode);
        return;
    }
    WH_ChangeSysState(WH_SYSSTATE_IDLE);
    if (!sAutoConnectFlag) {
        return;
    }
    if (sChildWEPKeyGenerator != NULL) {
        if (!WH_StateInSetChildWEPKey()) {
            WH_ChangeSysState(WH_SYSSTATE_ERROR);
        }
    } else {
        if (!WH_StateInStartChild()) {
            WH_ChangeSysState(WH_SYSSTATE_ERROR);
        }
    }
}

static BOOL WH_StateInSetChildWEPKey(void) {
    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    u16 wepmode = sChildWEPKeyGenerator(sWEPKey, &sBssDesc);
    WMErrCode result = WM_SetWEPKey(WH_StateOutSetChildWEPKey, wepmode, sWEPKey);
    if (result != WM_ERRCODE_OPERATING) {
        WH_SetError(result);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutSetChildWEPKey(void *arg) {
    WMCallback *cb = (WMCallback *)arg;
    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_SetError(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    }
    if (!WH_StateInStartChild()) {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
    }
}

static BOOL WH_StateInStartChild(void) {
    if (sSysState == WH_SYSSTATE_CONNECTED || sSysState == WH_SYSSTATE_KEYSHARING ||
        sSysState == WH_SYSSTATE_DATASHARING) {
        return TRUE;
    }
    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    WMErrCode result = WM_StartConnectEx(WH_StateOutStartChild, &sBssDesc, NULL, TRUE,
                                         (u16)((sChildWEPKeyGenerator != NULL) ? WM_AUTHMODE_SHARED_KEY
                                                                               : WM_AUTHMODE_OPEN_SYSTEM));
    if (result != WM_ERRCODE_OPERATING) {
        WH_SetError(result);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutStartChild(void *arg) {
    WMStartConnectCallback *cb = (WMStartConnectCallback *)arg;
    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_SetError(cb->errcode);
        if (cb->errcode == WM_ERRCODE_OVER_MAX_ENTRY) {
            WH_ChangeSysState(WH_SYSSTATE_ERROR);
            return;
        } else if (cb->errcode == WM_ERRCODE_NO_ENTRY) {
            WH_ChangeSysState(WH_SYSSTATE_ERROR);
            return;
        } else if (cb->errcode == WM_ERRCODE_FAILED) {
            WH_ChangeSysState(WH_SYSSTATE_CONNECT_FAIL);
            return;
        } else {
            WH_ChangeSysState(WH_SYSSTATE_ERROR);
        }
        return;
    }
    if (cb->state == WM_STATECODE_BEACON_LOST) {
        return;
    }
    if (cb->state == WM_STATECODE_CONNECTED) {
        WH_ChangeSysState(WH_SYSSTATE_CONNECTED);
        if (!WH_StateInStartChildMP()) {
            WH_ChangeSysState(WH_SYSSTATE_BUSY);
            return;
        }
        sMyAid = cb->aid;
        return;
    }
    if (cb->state == WM_STATECODE_CONNECT_START) {
        return;
    }
    if (cb->state == WM_STATECODE_DISCONNECTED) {
        WH_SetError(WH_ERRCODE_DISCONNECTED);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    }
    if (cb->state == WM_STATECODE_DISCONNECTED_FROM_MYSELF) {
        return;
    }
    WH_ChangeSysState(WH_SYSSTATE_ERROR);
}

static BOOL WH_StateInStartChildMP(void) {
    WMErrCode result = WM_StartMP(WH_StateOutStartChildMP, (u16 *)sRecvBuffer, (u16)sRecvBufferSize,
                                  (u16 *)sSendBuffer, (u16)sSendBufferSize, 1);
    if (result != WM_ERRCODE_OPERATING) {
        WH_SetError(result);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutStartChildMP(void *arg) {
    WMStartMPCallback *cb = (WMStartMPCallback *)arg;
    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        if (cb->errcode == WM_ERRCODE_SEND_FAILED || cb->errcode == WM_ERRCODE_TIMEOUT ||
            cb->errcode == WM_ERRCODE_INVALID_POLLBITMAP) {
            return;
        }
        WH_SetError(cb->errcode);
        WH_Reset();
        return;
    }
    switch (cb->state) {
    case WM_STATECODE_MP_START:
        if (sConnectMode == WH_CONNECTMODE_KS_CHILD) {
            if (sSysState == WH_SYSSTATE_KEYSHARING) {
                return;
            }
            if (sSysState == WH_SYSSTATE_CONNECTED) {
                if (!WH_StateInStartChildKeyShare()) {
                    WH_Finalize();
                }
                return;
            }
        } else if (sConnectMode == WH_CONNECTMODE_DS_CHILD) {
            u16 aidBitmap = 3; // The parent and one child
            WMErrCode result = WM_StartDataSharing(&sDSInfo, WH_DS_PORT, aidBitmap, WH_DS_DATA_SIZE, TRUE);
            if (result != WM_ERRCODE_SUCCESS) {
                WH_SetError(result);
                WH_Finalize();
                return;
            }
            WH_ChangeSysState(WH_SYSSTATE_DATASHARING);
            return;
        }
        WH_ChangeSysState(WH_SYSSTATE_CONNECTED);
        break;
    case WM_STATECODE_MPEND_IND:
    case WM_STATECODE_MP_IND:
    case WM_STATECODE_MPACK_IND:
        break;
    }
}

static BOOL WH_StateInStartChildKeyShare(void) {
    if (sSysState == WH_SYSSTATE_KEYSHARING) {
        return TRUE;
    }
    if (sSysState != WH_SYSSTATE_CONNECTED) {
        return FALSE;
    }
    WH_ChangeSysState(WH_SYSSTATE_KEYSHARING);
    WMErrCode result = WM_StartKeySharing(&sWMKeySetBuf, WH_DS_PORT);
    if (result != WM_ERRCODE_SUCCESS) {
        WH_SetError(result);
        return FALSE;
    }
    return TRUE;
}

static BOOL WH_StateInEndChildKeyShare(void) {
    if (sSysState != WH_SYSSTATE_KEYSHARING) {
        return FALSE;
    }
    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    WMErrCode result = WM_EndKeySharing(&sWMKeySetBuf);
    if (result != WM_ERRCODE_SUCCESS) {
        WH_SetError(result);
        return FALSE;
    }
    if (!WH_StateInEndChildMP()) {
        return FALSE;
    }
    return TRUE;
}

static BOOL WH_StateInEndChildMP(void) {
    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    WMErrCode result = WM_EndMP(WH_StateOutEndChildMP);
    if (result != WM_ERRCODE_OPERATING) {
        WH_SetError(result);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutEndChildMP(void *arg) {
    WMCallback *cb = (WMCallback *)arg;
    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_SetError(cb->errcode);
        WH_Finalize();
        return;
    }
    if (!WH_StateInEndChild()) {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
    }
}

static BOOL WH_StateInEndChild(void) {
    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    WMErrCode result = WM_Disconnect(WH_StateOutEndChild, 0);
    if (result != WM_ERRCODE_OPERATING) {
        WH_SetError(result);
        WH_Reset();
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutEndChild(void *arg) {
    WMCallback *cb = (WMCallback *)arg;
    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_SetError(cb->errcode);
        return;
    }
    WH_ChangeSysState(WH_SYSSTATE_IDLE);
}

static BOOL WH_StateInReset(void) {
    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    WMErrCode result = WM_Reset(WH_StateOutReset);
    if (result != WM_ERRCODE_OPERATING) {
        WH_SetError(result);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutReset(void *arg) {
    WMCallback *cb = (WMCallback *)arg;
    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        WH_SetError(cb->errcode);
        return;
    }
    WH_ChangeSysState(WH_SYSSTATE_IDLE);
}

static BOOL WH_StateInSetMPData(const void *data, u16 datasize, WHSendCallbackFunc callback) {
    DC_FlushRange(sSendBuffer, (u32)sSendBufferSize);
    WMErrCode result = WM_SetMPDataToPortEx(WH_StateOutSetMPData, (void *)callback, (const u16 *)data, datasize, 0xFFFF,
                                            WH_DATA_PORT, WH_DATA_PRIO);
    if (result != WM_ERRCODE_OPERATING) {
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutSetMPData(void *arg) {
    WMPortSendCallback *cb = (WMPortSendCallback *)arg;
    if (cb->errcode != WM_ERRCODE_SUCCESS && cb->errcode != WM_ERRCODE_SEND_FAILED) {
        WH_SetError(cb->errcode);
        return;
    }
    if (cb->arg != NULL) {
        WHSendCallbackFunc callback = (WHSendCallbackFunc)cb->arg;
        callback(cb->errcode == WM_ERRCODE_SUCCESS);
    }
}

static void WH_PortReceiveCallback(void *arg) {
    WMPortRecvCallback *cb = (WMPortRecvCallback *)arg;
    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_SetError(cb->errcode);
        return;
    }
    if (sReceiverFunc == NULL) {
        return;
    }
    if (cb->state == WM_STATECODE_PORT_RECV) {
        sReceiverFunc(cb->aid, cb->data, cb->length);
    } else if (cb->state == WM_STATECODE_DISCONNECTED) {
        sReceiverFunc(cb->aid, NULL, 0);
    }
}

static void WH_StateOutEnd(void *arg) {
    WMCallback *cb = (WMCallback *)arg;
    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
        return;
    }
    WH_ChangeSysState(WH_SYSSTATE_STOP);
}

void WH_SetGgid(u32 ggid) {
    sParentParam.ggid = ggid;
}

int WH_GetSystemState(void) {
    return sSysState;
}

BOOL WH_Initialize(void) {
    sRecvBufferSize = 0;
    sSendBufferSize = 0;
    sReceiverFunc = NULL;
    sMyAid = 0;
    sConnectBitmap = 1;
    sErrCode = WM_ERRCODE_SUCCESS;
    sParentParam.userGameInfo = NULL;
    sParentParam.userGameInfoLength = 0;
    data_02145c38 = NULL;
    if (!WH_StateInInitialize()) {
        return FALSE;
    }
    return TRUE;
}

static void WH_IndicateHandler(void *arg) {
    WMIndCallback *cb = (WMIndCallback *)arg;
    if (cb->errcode == WM_ERRCODE_FIFO_ERROR) {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        OS_Terminate();
    }
}

static BOOL WH_StateInInitialize(void) {
    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    WMErrCode result = WM_Initialize(&sWmBuffer, WH_StateOutInitialize, WH_DMA_NO);
    if (result != WM_ERRCODE_OPERATING) {
        WH_SetError(result);
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutInitialize(void *arg) {
    WMCallback *cb = (WMCallback *)arg;
    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_SetError(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
        return;
    }
    WMErrCode result = WM_SetIndCallback(WH_IndicateHandler);
    if (result != WM_ERRCODE_SUCCESS) {
        WH_SetError(result);
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
        return;
    }
    WH_ChangeSysState(WH_SYSSTATE_IDLE);
}

BOOL WH_ChildConnect(int mode, WMBssDesc *bssDesc) {
    sRecvBufferSize = WH_CHILD_RECV_BUFFER_SIZE;
    sSendBufferSize = WH_CHILD_SEND_BUFFER_SIZE;
    sConnectMode = mode;
    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    switch (mode) {
    case WH_CONNECTMODE_MP_CHILD:
    case WH_CONNECTMODE_KS_CHILD:
    case WH_CONNECTMODE_DS_CHILD:
        MI_CpuCopy8(bssDesc, &sBssDesc, sizeof(WMBssDesc));
        DC_FlushRange(&sBssDesc, sizeof(WMBssDesc));
        DC_WaitWriteBufferEmpty();
        if (sChildWEPKeyGenerator != NULL) {
            return WH_StateInSetChildWEPKey();
        } else {
            return WH_StateInStartChild();
        }
    }
    return FALSE;
}

void WH_SetReceiver(WHReceiverFunc proc) {
    sReceiverFunc = proc;
    if (WM_SetPortCallback(WH_DATA_PORT, WH_PortReceiveCallback, NULL) != WM_ERRCODE_SUCCESS) {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
    }
}

BOOL WH_SendData(const void *data, u16 datasize, WHSendCallbackFunc callback) {
    return WH_StateInSetMPData(data, datasize, callback);
}

void WH_Reset(void) {
    if (sSysState != WH_SYSSTATE_DATASHARING) {
        WMErrCode result = WM_EndDataSharing(&sDSInfo);
        if (result != WM_ERRCODE_SUCCESS) {
            WH_SetError(result);
        }
    }
    if (!WH_StateInReset()) {
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
    }
}

void WH_Finalize(void) {
    if (sSysState == WH_SYSSTATE_IDLE) {
        return;
    }
    if (sSysState != WH_SYSSTATE_KEYSHARING && sSysState != WH_SYSSTATE_DATASHARING &&
        sSysState != WH_SYSSTATE_CONNECTED) {
        WH_ChangeSysState(WH_SYSSTATE_BUSY);
        WH_Reset();
        return;
    }
    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    switch (sConnectMode) {
    case WH_CONNECTMODE_KS_CHILD:
        if (!WH_StateInEndChildKeyShare()) {
            WH_Reset();
        }
        break;
    case WH_CONNECTMODE_MP_CHILD:
    case WH_CONNECTMODE_DS_CHILD:
        if (!WH_StateInEndChildMP()) {
            WH_Reset();
        }
        break;
    case WH_CONNECTMODE_KS_PARENT:
        if (!WH_StateInEndParentKeyShare()) {
            WH_Reset();
        }
        break;
    case WH_CONNECTMODE_MP_PARENT:
    case WH_CONNECTMODE_DS_PARENT:
        if (!WH_StateInEndParentMP()) {
            WH_Reset();
        }
        break;
    }
}

BOOL WH_End(void) {
    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    if (WM_End(WH_StateOutEnd) != WM_ERRCODE_OPERATING) {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return FALSE;
    }
    return TRUE;
}

// Not in the ROM: a stand-in for the functions of WH that used these statics, which the link left out. It keeps them
// in the file, so that the others are where the ROM has them, and the link strips it as it stripped those.
extern "C" void wh_cpp_StrippedStatics(void) {
    data_02145c00 = 0;
    data_02145c02 = 0;
    data_02145c0c = 0;
    data_02145c20 = 0;
    data_02145c2c = 0;
    data_02145c30 = 0;
    MI_CpuClear8(&data_02145de0, sizeof(data_02145de0));
}

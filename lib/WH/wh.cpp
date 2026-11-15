// WH, the wireless wrapper of the SDK's sample code, as the game compiles it in. This is the project's own
// decompilation of the game's code: a chain of states, each a function that starts a call of the WM driver (StateIn)
// and the callback that receives its end (StateOut). See docs/binary.md, "WH".

#include "wh.h"
#include <nitro/cht.h>

// C++, but the SDK's C names: nothing outside refers to WH by name but the game's code, whose calls are by address
extern "C" {

// The game's build is a final one: no messages, and a failed check stops the program.
#define WH_TRACE            if (wh_trace) wh_trace
#define WH_REPORT_FAILURE(result)   WH_SetError(result)
#define WH_ASSERT(exp)      (void)((exp) || (OS_Terminate(), 0))

// The statics are in an order that the compiler's size sort turns into the ROM's: the pool of those under 64 bytes
// (sChannelBitmap at its start, sSysState at 0x2C, sScanParam at 0xA0), the others each on its own, and the order of
// the name tables' strings in .data, which the same sort decides. Only their sizes say anything of the original's
// order. The buffers are arrays of u32, as an array of char would be kept out of the pool; sConnectionSsid is one.

static void (*wh_trace)(const char *, ...);
static u32 sRand;
static u16 sChannelIndex;
static int sErrCode;
static WMBssDesc sBssDesc ATTRIBUTE_ALIGN(32);
static WHReceiverFunc sReceiverFunc;
static WMScanParam sScanParam ATTRIBUTE_ALIGN(32);
static BOOL sPictoCatchFlag;
static u16 sConnectBitmap;
static WHStartScanCallbackFunc sScanCallback;
static u16 sWEPKey[20 / sizeof(u16)] ATTRIBUTE_ALIGN(32);
static s32 sRecvBufferSize;
static u32 sRecvBuffer[WH_CHILD_RECV_BUFFER_SIZE / sizeof(u32)] ATTRIBUTE_ALIGN(32);
static WMParentParam sParentParam ATTRIBUTE_ALIGN(32);
static u16 sChannelBusyRatio;
static int sConnectMode;
static u16 sMyAid;
static u32 sWmBuffer[WM_SYSTEM_BUF_SIZE / sizeof(u32)] ATTRIBUTE_ALIGN(32);
static WHParentWEPKeyGeneratorFunc sParentWEPKeyGenerator;
static u8 sConnectionSsid[WM_SIZE_CHILD_SSID] ATTRIBUTE_ALIGN(32);
static u16 sAutoConnectFlag;
static u32 sSendBuffer[WH_PARENT_SEND_BUFFER_SIZE / sizeof(u32)] ATTRIBUTE_ALIGN(32);
static WMDataSharingInfo sDSInfo ATTRIBUTE_ALIGN(32);
static WMKeySetBuf sWMKeySetBuf ATTRIBUTE_ALIGN(32);
static WHChildWEPKeyGeneratorFunc sChildWEPKeyGenerator;
static u16 sChannel;
static WHJudgeAcceptFunc sJudgeAcceptFunc;
u16 gWHChildSsid[WM_SIZE_CHILD_SSID / sizeof(u16)];
static u16 sChannelBitmap;
static WMCallbackFunc sIndicationCallback;
static int sSysState;
static u16 whInitialized;
static s32 sSendBufferSize;

#define RAND()          (sRand = sRand * 69069UL + 12345)
#define RAND_INIT(x)    (sRand = (u32)(x))

static void WH_IndicateHandler(void *arg);
static BOOL WH_StateInInitialize(void);
static void WH_StateOutInitialize(void *arg);
static u16 WH_StateInMeasureChannel(u16 channel);
static void WH_StateOutMeasureChannel(void *arg);
static WMErrCode WHi_MeasureChannel(WMCallbackFunc func, u16 channel);
static s16 SelectChannel(u16 bitmap);
static BOOL WH_StateInSetParentParam(void);
static void WH_StateOutSetParentParam(void *arg);
static BOOL WH_StateInSetParentWEPKey(void);
static void WH_StateOutSetParentWEPKey(void *arg);
static BOOL WH_StateInStartParent(void);
static void WH_StateOutStartParent(void *arg);
static BOOL WH_StateInStartParentMP(void);
static void WH_StateOutStartParentMP(void *arg);
static BOOL WH_StateInStartParentKeyShare(void);
static BOOL WH_StateInEndParentKeyShare(void);
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
static BOOL WH_StateInSetMPData(const void *data, u16 datasize, WHSendCallbackFunc callback);
static void WH_StateOutSetMPData(void *arg);
static void WH_PortReceiveCallback(void *arg);
static BOOL WH_StateInEndParentMP(void);
static void WH_StateOutEndParentMP(void *arg);
static BOOL WH_StateInEndParent(void);
static void WH_StateOutEndParent(void *arg);
static BOOL WH_StateInEndChildMP(void);
static void WH_StateOutEndChildMP(void *arg);
static BOOL WH_StateInEndChild(void);
static void WH_StateOutEndChild(void *arg);
static void WH_StateOutEnd(void *arg);
static BOOL WH_StateInReset(void);
static void WH_StateOutReset(void *arg);

static const char *WH_GetWMErrCodeName(int result)
{
    static const char *errnames[] = {
        "WM_ERRCODE_SUCCESS",
        "WM_ERRCODE_FAILED",
        "WM_ERRCODE_OPERATING",
        "WM_ERRCODE_ILLEGAL_STATE",
        "WM_ERRCODE_WM_DISABLE",
        "WM_ERRCODE_NO_DATASET",
        "WM_ERRCODE_INVALID_PARAM",
        "WM_ERRCODE_NO_CHILD",
        "WM_ERRCODE_FIFO_ERROR",
        "WM_ERRCODE_TIMEOUT",
        "WM_ERRCODE_SEND_QUEUE_FULL",
        "WM_ERRCODE_NO_ENTRY",
        "WM_ERRCODE_OVER_MAX_ENTRY",
        "WM_ERRCODE_INVALID_POLLBITMAP",
        "WM_ERRCODE_NO_DATA",
        "WM_ERRCODE_SEND_FAILED",
        "WM_ERRCODE_DCF_TEST",
        "WM_ERRCODE_WL_INVALID_PARAM",
        "WM_ERRCODE_WL_LENGTH_ERR",
        "WM_ERRCODE_FLASH_ERROR",
        "WH_ERRCODE_DISCONNECTED",
        "WH_ERRCODE_PARENT_NOT_FOUND",
        "WH_ERRCODE_NO_RADIO",
    };

    if (0 <= result && result < sizeof(errnames) / sizeof(char *)) {
        return errnames[result];
    } else {
        return "N/A";
    }
}

static const char *WH_GetWMStateCodeName(u16 code)
{
    static const char *statenames[] = {
        "WM_STATECODE_PARENT_START",
        "N/A",
        "WM_STATECODE_BEACON_SENT",
        "WM_STATECODE_SCAN_START",
        "WM_STATECODE_PARENT_NOT_FOUND",
        "WM_STATECODE_PARENT_FOUND",
        "WM_STATECODE_CONNECT_START",
        "WM_STATECODE_CONNECTED",
        "WM_STATECODE_BEACON_LOST",
        "WM_STATECODE_DISCONNECTED",
        "WM_STATECODE_MP_START",
        "WM_STATECODE_MPEND_IND",
        "WM_STATECODE_MP_IND",
        "WM_STATECODE_MPACK_IND",
        "WM_STATECODE_DCF_START",
        "WM_STATECODE_DCF_IND",
        "WM_STATECODE_BEACON_RECV",
        "WM_STATECODE_DISASSOCIATE",
        "WM_STATECODE_REASSOCIATE",
        "WM_STATECODE_AUTHENTICATE",
        "WM_STATECODE_PORT_SEND",
        "WM_STATECODE_PORT_RECV",
        "WM_STATECODE_FIFO_ERROR",
        "WM_STATECODE_INFORMATION",
        "WM_STATECODE_UNKNOWN",
        "WM_STATECODE_PORT_INIT",
        "WM_STATECODE_DISCONNECTED_FROM_MYSELF",
    };

    if (code < sizeof(statenames) / sizeof(char *)) {
        return statenames[code];
    } else {
        return "N/A";
    }
}

static void WH_ChangeSysState(int state)
{
    static const char *sysstates[] = {
        "WH_SYSSTATE_STOP",
        "WH_SYSSTATE_IDLE",
        "WH_SYSSTATE_SCANNING",
        "WH_SYSSTATE_BUSY",
        "WH_SYSSTATE_CONNECTED",
        "WH_SYSSTATE_DATASHARING",
        "WH_SYSSTATE_KEYSHARING",
        "WH_SYSSTATE_MEASURECHANNEL",
        "WH_SYSSTATE_CONNECT_FAIL",
        "WH_SYSSTATE_ERROR",
    };

    WH_TRACE("%s -> ", sysstates[sSysState]);
    sSysState = state;
    WH_TRACE("%s\n", sysstates[sSysState]);
}

static void WH_SetError(int code)
{
    // An error already recorded is kept
    if (sSysState == WH_SYSSTATE_ERROR || sSysState == WH_SYSSTATE_FATAL) {
        return;
    }
    sErrCode = code;
}

static BOOL WH_StateInSetParentParam(void)
{
    WMErrCode result;

    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    result = WM_SetParentParameter(WH_StateOutSetParentParam, &sParentParam);
    if (result != WM_ERRCODE_OPERATING) {
        WH_REPORT_FAILURE(result);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutSetParentParam(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_REPORT_FAILURE(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    }

    if (sParentWEPKeyGenerator != NULL) {
        if (!WH_StateInSetParentWEPKey()) {
            WH_ChangeSysState(WH_SYSSTATE_ERROR);
        }
    } else {
        if (!WH_StateInStartParent()) {
            WH_ChangeSysState(WH_SYSSTATE_ERROR);
        }
    }
}

static BOOL WH_StateInSetParentWEPKey(void)
{
    u16 wepmode;
    WMErrCode result;

    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    wepmode = (*sParentWEPKeyGenerator)(sWEPKey, &sParentParam);
    result = WM_SetWEPKey(WH_StateOutSetParentWEPKey, wepmode, sWEPKey);
    if (result != WM_ERRCODE_OPERATING) {
        WH_REPORT_FAILURE(result);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutSetParentWEPKey(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_REPORT_FAILURE(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    }

    if (!WH_StateInStartParent()) {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
    }
}

static BOOL WH_StateInStartParent(void)
{
    WMErrCode result;

    if (sSysState == WH_SYSSTATE_CONNECTED || sSysState == WH_SYSSTATE_KEYSHARING
        || sSysState == WH_SYSSTATE_DATASHARING) {
        return TRUE;
    }

    result = WM_StartParent(WH_StateOutStartParent);
    if (result != WM_ERRCODE_OPERATING) {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }

    sMyAid = 0;
    sConnectBitmap = 1;
    return TRUE;
}

static void WH_StateOutStartParent(void *arg)
{
    WMStartParentCallback *cb = (WMStartParentCallback *)arg;
    const u16 target_bitmap = (u16)(1 << cb->aid);

    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_REPORT_FAILURE(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    }

    switch (cb->state) {
    case WM_STATECODE_BEACON_SENT:
        break;

    case WM_STATECODE_CHILD_CONNECTED:
        MI_CpuCopy8(cb->ssid, gWHChildSsid, WM_SIZE_CHILD_SSID);
        WH_TRACE("StartParent - new child (aid 0x%x) connected\n", cb->aid);
        if (sJudgeAcceptFunc != NULL) {
            if (!sJudgeAcceptFunc(cb)) {
                WMErrCode result = WM_Disconnect(NULL, cb->aid);
                if (result != WM_ERRCODE_OPERATING) {
                    WH_REPORT_FAILURE(result);
                    WH_ChangeSysState(WH_SYSSTATE_ERROR);
                }
                break;
            }
        }
        sConnectBitmap |= target_bitmap;
        break;

    case WM_STATECODE_DISCONNECTED:
        WH_TRACE("StartParent - child (aid 0x%x) disconnected\n", cb->aid);
        sConnectBitmap &= ~target_bitmap;
        break;

    case WM_STATECODE_DISCONNECTED_FROM_MYSELF:
        WH_TRACE("StartParent - child (aid 0x%x) disconnected from myself\n", cb->aid);
        break;

    case WM_STATECODE_PARENT_START:
        if (!WH_StateInStartParentMP()) {
            WH_ChangeSysState(WH_SYSSTATE_ERROR);
        }
        break;

    default:
        WH_TRACE("unknown indicate, state = %d\n", cb->state);
    }
}

static BOOL WH_StateInStartParentMP(void)
{
    WMErrCode result;

    if (sSysState == WH_SYSSTATE_CONNECTED || sSysState == WH_SYSSTATE_KEYSHARING
        || sSysState == WH_SYSSTATE_DATASHARING) {
        return TRUE;
    }

    WH_ChangeSysState(WH_SYSSTATE_CONNECTED);
    result = WM_StartMP(WH_StateOutStartParentMP, (u16 *)sRecvBuffer, (u16)sRecvBufferSize, (u16 *)sSendBuffer,
                        (u16)sSendBufferSize, 1);
    if (result != WM_ERRCODE_OPERATING) {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutStartParentMP(void *arg)
{
    WMStartMPCallback *cb = (WMStartMPCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_REPORT_FAILURE(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    }

    switch (cb->state) {
    case WM_STATECODE_MP_START:
        if (sConnectMode == WH_CONNECTMODE_KS_PARENT) {
            if (sSysState == WH_SYSSTATE_CONNECTED) {
                if (!WH_StateInStartParentKeyShare()) {
                    WH_TRACE("WH_StateInStartParentKeyShare failed\n");
                    WH_ChangeSysState(WH_SYSSTATE_ERROR);
                }
                return;
            } else if (sSysState == WH_SYSSTATE_KEYSHARING) {
                return;
            }
        } else if (sConnectMode == WH_CONNECTMODE_DS_PARENT) {
            WMErrCode result;
            u16 aidBitmap = (u16)((1 << (WH_CHILD_MAX + 1)) - 1);

            result = WM_StartDataSharing(&sDSInfo, WH_DS_PORT, aidBitmap, WH_DS_DATA_SIZE, TRUE);
            if (result != WM_ERRCODE_SUCCESS) {
                WH_REPORT_FAILURE(result);
                WH_ChangeSysState(WH_SYSSTATE_ERROR);
                return;
            }
            WH_ChangeSysState(WH_SYSSTATE_DATASHARING);
            return;
        }
        WH_ChangeSysState(WH_SYSSTATE_CONNECTED);
        break;

    case WM_STATECODE_MPEND_IND:
        break;

    case WM_STATECODE_MP_IND:
    case WM_STATECODE_MPACK_IND:
    default:
        WH_TRACE("unknown indicate, state = %d\n", cb->state);
        break;
    }
}

static BOOL WH_StateInStartParentKeyShare(void)
{
    WMErrCode result;

    WH_ChangeSysState(WH_SYSSTATE_KEYSHARING);
    result = WM_StartKeySharing(&sWMKeySetBuf, WH_DS_PORT);
    if (result != WM_ERRCODE_SUCCESS) {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }
    return TRUE;
}

static BOOL WH_StateInEndParentKeyShare(void)
{
    WMErrCode result;

    result = WM_EndKeySharing(&sWMKeySetBuf);
    if (result != WM_ERRCODE_SUCCESS) {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }

    if (!WH_StateInEndParentMP()) {
        WH_TRACE("WH_StateInEndParentMP failed\n");
        WH_Reset();
        return FALSE;
    }
    return TRUE;
}

static BOOL WH_StateInEndParentMP(void)
{
    WMErrCode result;

    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    result = WM_EndMP(WH_StateOutEndParentMP);
    if (result != WM_ERRCODE_OPERATING) {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutEndParentMP(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_REPORT_FAILURE(cb->errcode);
        WH_Reset();
        return;
    }

    if (!WH_StateInEndParent()) {
        WH_TRACE("WH_StateInEndParent failed\n");
        WH_Reset();
    }
}

static BOOL WH_StateInEndParent(void)
{
    WMErrCode result;

    result = WM_EndParent(WH_StateOutEndParent);
    if (result != WM_ERRCODE_OPERATING) {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutEndParent(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_REPORT_FAILURE(cb->errcode);
        return;
    }
    WH_ChangeSysState(WH_SYSSTATE_IDLE);
}

BOOL WH_StartScan(WHStartScanCallbackFunc callback, const u8 *macAddr, u16 channel)
{
    WH_ASSERT(sSysState == WH_SYSSTATE_IDLE);

    WH_ChangeSysState(WH_SYSSTATE_SCANNING);

    sScanCallback = callback;
    sChannelIndex = channel;
    sScanParam.channel = 0;
    sAutoConnectFlag = FALSE;

    *(u16 *)(&sScanParam.bssid[4]) = *(u16 *)(macAddr + 4);
    *(u16 *)(&sScanParam.bssid[2]) = *(u16 *)(macAddr + 2);
    *(u16 *)(&sScanParam.bssid[0]) = *(u16 *)(macAddr);

    if (!WH_StateInStartScan()) {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return FALSE;
    }
    return TRUE;
}

static BOOL WH_StateInStartScan(void)
{
    WMErrCode result;
    u16 chanpat;

    WH_ASSERT(sSysState == WH_SYSSTATE_SCANNING);

    chanpat = WM_GetAllowedChannel();
    if (chanpat == 0x8000) {
        WH_REPORT_FAILURE(WM_ERRCODE_ILLEGAL_STATE);
        return FALSE;
    }
    if (chanpat == 0) {
        WH_REPORT_FAILURE(WH_ERRCODE_NO_RADIO);
        return FALSE;
    }

    if (sChannelIndex == 0) {
        // The next channel allowed after the last one
        while (TRUE) {
            sScanParam.channel++;
            if (sScanParam.channel > 16) {
                sScanParam.channel = 1;
            }
            if (chanpat & (1 << (sScanParam.channel - 1))) {
                break;
            }
        }
    } else {
        sScanParam.channel = sChannelIndex;
    }

    sScanParam.maxChannelTime = WM_GetDispersionScanPeriod();
    sScanParam.scanBuf = &sBssDesc;
    result = WM_StartScan(WH_StateOutStartScan, &sScanParam);
    if (result != WM_ERRCODE_OPERATING) {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutStartScan(void *arg)
{
    WMStartScanCallback *cb = (WMStartScanCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_REPORT_FAILURE(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    }

    if (sSysState != WH_SYSSTATE_SCANNING) {
        // Scanning was stopped
        sAutoConnectFlag = FALSE;
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
        WH_TRACE("WH_StateOutStartScan : MAC=%02x%02x%02x%02x%02x%02x ", cb->macAddress[0], cb->macAddress[1],
                 cb->macAddress[2], cb->macAddress[3], cb->macAddress[4], cb->macAddress[5]);

        // The ARM7 wrote the parent's description
        DC_InvalidateRange(&sBssDesc, sizeof(WMBssDesc));

        if (sPictoCatchFlag) {
            if (CHT_IsPictochatParent(&sBssDesc)) {
                WH_TRACE("pictochat parent find\n");
                if (sScanCallback != NULL) {
                    sScanCallback(&sBssDesc);
                }
                break;
            }
        }

        if (!WM_IsValidGameInfo(&cb->gameInfo, cb->gameInfoLength) || cb->gameInfo.ggid != sParentParam.ggid) {
            WH_TRACE("not my parent ggid \n");
            break;
        }

        if ((cb->gameInfo.gameNameCount_attribute & (WM_ATTR_FLAG_ENTRY | WM_ATTR_FLAG_MB)) != WM_ATTR_FLAG_ENTRY) {
            WH_TRACE("not recieve entry\n");
            break;
        }

        WH_TRACE("parent find\n");
        if (sScanCallback != NULL) {
            sScanCallback(&sBssDesc);
        }

        if (sAutoConnectFlag) {
            if (!WH_StateInEndScan()) {
                WH_ChangeSysState(WH_SYSSTATE_ERROR);
            }
            return;
        }
        break;
    }

    // On to the next channel
    if (!WH_StateInStartScan()) {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
    }
}

BOOL WH_EndScan(void)
{
    if (sSysState != WH_SYSSTATE_SCANNING) {
        return FALSE;
    }

    sAutoConnectFlag = FALSE;
    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    return TRUE;
}

static BOOL WH_StateInEndScan(void)
{
    WMErrCode result;

    result = WM_EndScan(WH_StateOutEndScan);
    if (result != WM_ERRCODE_OPERATING) {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutEndScan(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_REPORT_FAILURE(cb->errcode);
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
            WH_TRACE("WH_StateOutEndScan : startchild failed\n");
            WH_ChangeSysState(WH_SYSSTATE_ERROR);
        }
    }
}

static BOOL WH_StateInSetChildWEPKey(void)
{
    u16 wepmode;
    WMErrCode result;

    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    wepmode = (*sChildWEPKeyGenerator)(sWEPKey, &sBssDesc);
    result = WM_SetWEPKey(WH_StateOutSetChildWEPKey, wepmode, sWEPKey);
    if (result != WM_ERRCODE_OPERATING) {
        WH_REPORT_FAILURE(result);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutSetChildWEPKey(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_REPORT_FAILURE(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    }

    if (!WH_StateInStartChild()) {
        WH_TRACE("WH_StateOutSetChildWEPKey : startchild failed\n");
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
    }
}

static BOOL WH_StateInStartChild(void)
{
    WMErrCode result;

    if (sSysState == WH_SYSSTATE_CONNECTED || sSysState == WH_SYSSTATE_KEYSHARING
        || sSysState == WH_SYSSTATE_DATASHARING) {
        WH_TRACE("WH_StateInStartChild : already connected?\n");
        return TRUE;
    }

    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    result = WM_StartConnectEx(WH_StateOutStartChild, &sBssDesc, sConnectionSsid, TRUE,
                               (u16)((sChildWEPKeyGenerator != NULL) ? WM_AUTHMODE_SHARED_KEY : WM_AUTHMODE_OPEN_SYSTEM));
    if (result != WM_ERRCODE_OPERATING) {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutStartChild(void *arg)
{
    WMStartConnectCallback *cb = (WMStartConnectCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_REPORT_FAILURE(cb->errcode);

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
        WH_TRACE("Connect to Parent\n");
        WH_ChangeSysState(WH_SYSSTATE_CONNECTED);
        if (!WH_StateInStartChildMP()) {
            WH_TRACE("WH_StateInStartChildMP failed\n");
            WH_ChangeSysState(WH_SYSSTATE_BUSY);
            return;
        }
        sMyAid = cb->aid;
        return;
    } else if (cb->state == WM_STATECODE_CONNECT_START) {
        return;
    } else if (cb->state == WM_STATECODE_DISCONNECTED) {
        WH_TRACE("Disconnected from Parent\n");
        WH_SetError(WH_ERRCODE_DISCONNECTED);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    } else if (cb->state == WM_STATECODE_DISCONNECTED_FROM_MYSELF) {
        return;
    }

    WH_TRACE("unknown state %d, %s\n", cb->state, WH_GetWMStateCodeName(cb->state));
    WH_ChangeSysState(WH_SYSSTATE_ERROR);
}

static BOOL WH_StateInStartChildMP(void)
{
    WMErrCode result;

    result = WM_StartMP(WH_StateOutStartChildMP, (u16 *)sRecvBuffer, (u16)sRecvBufferSize, (u16 *)sSendBuffer,
                        (u16)sSendBufferSize, 1);
    if (result != WM_ERRCODE_OPERATING) {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutStartChildMP(void *arg)
{
    WMStartMPCallback *cb = (WMStartMPCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        // Retried by WM: nothing to do
        if (cb->errcode == WM_ERRCODE_SEND_FAILED || cb->errcode == WM_ERRCODE_TIMEOUT
            || cb->errcode == WM_ERRCODE_INVALID_POLLBITMAP) {
            return;
        }
        WH_REPORT_FAILURE(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
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
                    WH_TRACE("WH_StateInStartChildKeyShare failed\n");
                    WH_Finalize();
                }
                return;
            }
        } else if (sConnectMode == WH_CONNECTMODE_DS_CHILD) {
            WMErrCode result;
            u16 aidBitmap = (u16)((1 << (WH_CHILD_MAX + 1)) - 1);

            result = WM_StartDataSharing(&sDSInfo, WH_DS_PORT, aidBitmap, WH_DS_DATA_SIZE, TRUE);
            if (result != WM_ERRCODE_SUCCESS) {
                WH_REPORT_FAILURE(result);
                WH_Finalize();
                return;
            }
            WH_TRACE("WH_StateOutStartChildMP : WM_StartDataSharing OK\n");
            WH_ChangeSysState(WH_SYSSTATE_DATASHARING);
            return;
        }
        WH_ChangeSysState(WH_SYSSTATE_CONNECTED);
        break;

    case WM_STATECODE_MP_IND:
        break;

    case WM_STATECODE_MPACK_IND:
        break;

    case WM_STATECODE_MPEND_IND:
    default:
        WH_TRACE("unknown indicate, state = %d\n", cb->state);
        break;
    }
}

static BOOL WH_StateInStartChildKeyShare(void)
{
    WMErrCode result;

    if (sSysState == WH_SYSSTATE_KEYSHARING) {
        return TRUE;
    }
    if (sSysState != WH_SYSSTATE_CONNECTED) {
        return FALSE;
    }

    WH_ChangeSysState(WH_SYSSTATE_KEYSHARING);
    result = WM_StartKeySharing(&sWMKeySetBuf, WH_DS_PORT);
    if (result != WM_ERRCODE_SUCCESS) {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }
    return TRUE;
}

static BOOL WH_StateInEndChildKeyShare(void)
{
    WMErrCode result;

    if (sSysState != WH_SYSSTATE_KEYSHARING) {
        return FALSE;
    }

    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    result = WM_EndKeySharing(&sWMKeySetBuf);
    if (result != WM_ERRCODE_SUCCESS) {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }

    if (!WH_StateInEndChildMP()) {
        return FALSE;
    }
    return TRUE;
}

static BOOL WH_StateInEndChildMP(void)
{
    WMErrCode result;

    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    result = WM_EndMP(WH_StateOutEndChildMP);
    if (result != WM_ERRCODE_OPERATING) {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutEndChildMP(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_REPORT_FAILURE(cb->errcode);
        WH_Finalize();
        return;
    }

    if (!WH_StateInEndChild()) {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
    }
}

static BOOL WH_StateInEndChild(void)
{
    WMErrCode result;

    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    result = WM_Disconnect(WH_StateOutEndChild, 0);
    if (result != WM_ERRCODE_OPERATING) {
        WH_REPORT_FAILURE(result);
        WH_Reset();
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutEndChild(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_REPORT_FAILURE(cb->errcode);
        return;
    }
    WH_ChangeSysState(WH_SYSSTATE_IDLE);
}

static BOOL WH_StateInReset(void)
{
    WMErrCode result;

    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    result = WM_Reset(WH_StateOutReset);
    if (result != WM_ERRCODE_OPERATING) {
        WH_REPORT_FAILURE(result);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutReset(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        WH_REPORT_FAILURE(cb->errcode);
        return;
    }
    WH_ChangeSysState(WH_SYSSTATE_IDLE);
}

static BOOL WH_StateInSetMPData(const void *data, u16 datasize, WHSendCallbackFunc callback)
{
    WMErrCode result;

    DC_FlushRange(sSendBuffer, (u32)sSendBufferSize);
    result = WM_SetMPDataToPortEx(WH_StateOutSetMPData, (void *)callback, (const u16 *)data, datasize, 0xffff, WH_DATA_PORT,
                                  WH_DATA_PRIO);
    if (result != WM_ERRCODE_OPERATING) {
        WH_TRACE("WH_StateInSetMPData failed - %s\n", WH_GetWMErrCodeName(result));
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutSetMPData(void *arg)
{
    WMPortSendCallback *cb = (WMPortSendCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS && cb->errcode != WM_ERRCODE_SEND_FAILED) {
        WH_REPORT_FAILURE(cb->errcode);
        return;
    }

    if (cb->arg != NULL) {
        WHSendCallbackFunc callback = (WHSendCallbackFunc)cb->arg;
        (*callback)((cb->errcode == WM_ERRCODE_SUCCESS));
    }
}

static void WH_PortReceiveCallback(void *arg)
{
    WMPortRecvCallback *cb = (WMPortRecvCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_REPORT_FAILURE(cb->errcode);
    } else if (sReceiverFunc != NULL) {
        if (cb->state == WM_STATECODE_PORT_RECV) {
            (*sReceiverFunc)(cb->aid, cb->data, cb->length);
        } else if (cb->state == WM_STATECODE_DISCONNECTED) {
            (*sReceiverFunc)(cb->aid, NULL, 0);
        }
    }
}

static void WH_StateOutEnd(void *arg)
{
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
        return;
    }
    WH_ChangeSysState(WH_SYSSTATE_STOP);
}

void WH_SetGgid(u32 ggid)
{
    sParentParam.ggid = ggid;
}

void WH_SetSsid(const void *ssid, u32 length)
{
    length = (u32)MATH_MIN(length, WM_SIZE_CHILD_SSID);
    MI_CpuCopy8(ssid, sConnectionSsid, length);
    MI_CpuClear8(sConnectionSsid + length, (u32)(WM_SIZE_CHILD_SSID - length));
}

void WH_SetUserGameInfo(u16 *userGameInfo, u16 length)
{
    sParentParam.userGameInfo = userGameInfo;
    sParentParam.userGameInfoLength = length;
}

int WH_GetLinkLevel(void)
{
    return (int)WM_GetLinkLevel();
}

u16 WH_GetBitmap(void)
{
    return sConnectBitmap;
}

int WH_GetSystemState(void)
{
    return sSysState;
}

void WH_PrintBssDesc(WMBssDesc *info)
{
#pragma unused(info)
}

BOOL WH_StartMeasureChannel(void)
{
    u16 result;
    u8 macAddr[6];

    OS_GetMacAddress(macAddr);
    RAND_INIT(OS_GetVBlankCount() + *(u16 *)&macAddr[0] + *(u16 *)&macAddr[2] + *(u16 *)&macAddr[4]);
    RAND();

    sChannel = 0;
    sChannelBusyRatio = 100 + 1;

    WH_ChangeSysState(WH_SYSSTATE_BUSY);

    result = WH_StateInMeasureChannel(1);
    if (result == WH_ERRCODE_NOMORE_CHANNEL) {
        WH_REPORT_FAILURE(WH_ERRCODE_NOMORE_CHANNEL);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return FALSE;
    }
    if (result != WM_ERRCODE_OPERATING) {
        WH_REPORT_FAILURE(result);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return FALSE;
    }
    return TRUE;
}

static u16 WH_StateInMeasureChannel(u16 channel)
{
    u16 allowedChannel;

    allowedChannel = WM_GetAllowedChannel();
    if (allowedChannel == 0x8000) {
        WH_REPORT_FAILURE(WM_ERRCODE_ILLEGAL_STATE);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return WM_ERRCODE_ILLEGAL_STATE;
    }
    if (allowedChannel == 0) {
        WH_REPORT_FAILURE(WH_ERRCODE_NO_RADIO);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return WH_ERRCODE_NOMORE_CHANNEL;
    }

    while (((1 << (channel - 1)) & allowedChannel) == 0) {
        channel++;
        if (channel > 16) {
            return WH_ERRCODE_NOMORE_CHANNEL;
        }
    }

    return (u16)WHi_MeasureChannel(WH_StateOutMeasureChannel, channel);
}

static void WH_StateOutMeasureChannel(void *arg)
{
    u16 result;
    u16 channel;
    WMMeasureChannelCallback *cb = (WMMeasureChannelCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_REPORT_FAILURE(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return;
    }

    WH_TRACE("channel %d bratio = 0x%x\n", cb->channel, cb->ccaBusyRatio);

    channel = cb->channel;

    if (sChannelBusyRatio > cb->ccaBusyRatio) {
        sChannelBusyRatio = cb->ccaBusyRatio;
        sChannelBitmap = (u16)(1 << (channel - 1));
    } else if (sChannelBusyRatio == cb->ccaBusyRatio) {
        sChannelBitmap |= 1 << (channel - 1);
    }

    result = WH_StateInMeasureChannel(++channel);

    if (result == WH_ERRCODE_NOMORE_CHANNEL) {
        WH_ChangeSysState(WH_SYSSTATE_MEASURECHANNEL);
        return;
    }
    if (result != WM_ERRCODE_OPERATING) {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
    }
}

static WMErrCode WHi_MeasureChannel(WMCallbackFunc func, u16 channel)
{
    // Carrier sense with energy detection, threshold 17, for 30 ms
    return WM_MeasureChannel(func, 3, 17, channel, 30);
}

u16 WH_GetMeasureChannel(void)
{
    if (sSysState != WH_SYSSTATE_MEASURECHANNEL) {
        OS_Terminate();
    }

    WH_ChangeSysState(WH_SYSSTATE_IDLE);
    sChannel = (u16)SelectChannel(sChannelBitmap);
    WH_TRACE("decided channel = %d\n", sChannel);
    return sChannel;
}

static s16 SelectChannel(u16 bitmap)
{
    s16 i;
    s16 channel = 0;
    u16 num = 0;
    u16 select;

    for (i = 0; i < 16; i++) {
        if (bitmap & (1 << i)) {
            channel = (s16)(i + 1);
            num++;
        }
    }

    if (num <= 1) {
        return channel;
    }

    // Several are as quiet: pick one at random
    select = (u16)(((RAND() & 0xFF) * num) / 0x100);

    for (i = 0; i < 16; i++) {
        if (bitmap & 1) {
            if (select == 0) {
                return (s16)(i + 1);
            }
            select--;
        }
        bitmap >>= 1;
    }

    return 0;
}

BOOL WH_Initialize(void)
{
    if (whInitialized) {
        return FALSE;
    }

    if (sIndicationCallback == NULL) {
        sIndicationCallback = WH_IndicateHandler;
    }

    sRecvBufferSize = 0;
    sSendBufferSize = 0;
    sReceiverFunc = NULL;
    sMyAid = 0;
    sConnectBitmap = 1;
    sErrCode = WM_ERRCODE_SUCCESS;

    sParentParam.userGameInfo = NULL;
    sParentParam.userGameInfoLength = 0;

    MI_CpuClear8(sConnectionSsid, sizeof(sConnectionSsid));

    sJudgeAcceptFunc = NULL;

    if (!WH_StateInInitialize()) {
        return FALSE;
    }

    whInitialized = 1;
    return TRUE;
}

static void WH_IndicateHandler(void *arg)
{
    WMIndCallback *cb = (WMIndCallback *)arg;

    if (cb->errcode == WM_ERRCODE_FIFO_ERROR) {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        OS_Terminate();
    }
}

static BOOL WH_StateInInitialize(void)
{
    WMErrCode result;

    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    result = WM_Initialize(sWmBuffer, WH_StateOutInitialize, WH_DMA_NO);
    if (result != WM_ERRCODE_OPERATING) {
        WH_REPORT_FAILURE(result);
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
        return FALSE;
    }
    return TRUE;
}

static void WH_StateOutInitialize(void *arg)
{
    WMErrCode result;
    WMCallback *cb = (WMCallback *)arg;

    if (cb->errcode != WM_ERRCODE_SUCCESS) {
        WH_REPORT_FAILURE(cb->errcode);
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
        return;
    }

    result = WM_SetIndCallback(sIndicationCallback);
    if (result != WM_ERRCODE_SUCCESS) {
        WH_REPORT_FAILURE(result);
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
        return;
    }

    WH_ChangeSysState(WH_SYSSTATE_IDLE);
}

BOOL WH_ParentConnect(int mode, u16 tgid, u16 channel)
{
    WH_ASSERT(sSysState == WH_SYSSTATE_IDLE);

    sRecvBufferSize = WH_PARENT_RECV_BUFFER_SIZE;
    sSendBufferSize = WH_PARENT_SEND_BUFFER_SIZE;
    WH_TRACE("recv buffer size = %d\n", sRecvBufferSize);
    WH_TRACE("send buffer size = %d\n", sSendBufferSize);

    sConnectMode = mode;
    WH_ChangeSysState(WH_SYSSTATE_BUSY);

    sParentParam.tgid = tgid;
    sParentParam.channel = channel;
    sParentParam.beaconPeriod = WM_GetDispersionBeaconPeriod();
    sParentParam.parentMaxSize = WH_PARENT_MAX_SIZE;
    sParentParam.childMaxSize = WH_CHILD_MAX_SIZE;
    sParentParam.maxEntry = WH_CHILD_MAX;
    sParentParam.CS_Flag = 0;
    sParentParam.multiBootFlag = 0;
    sParentParam.entryFlag = 1;
    sParentParam.KS_Flag = (u16)((mode == WH_CONNECTMODE_KS_PARENT) ? TRUE : FALSE);

    switch (mode) {
    case WH_CONNECTMODE_MP_PARENT:
    case WH_CONNECTMODE_KS_PARENT:
    case WH_CONNECTMODE_DS_PARENT:
        return WH_StateInSetParentParam();
    default:
        break;
    }

    WH_TRACE("unknown connect mode %d\n", mode);
    return FALSE;
}

BOOL WH_ChildConnect(int mode, WMBssDesc *bssDesc)
{
    WH_ASSERT(sSysState == WH_SYSSTATE_IDLE);

    sRecvBufferSize = WH_CHILD_RECV_BUFFER_SIZE;
    sSendBufferSize = WH_CHILD_SEND_BUFFER_SIZE;
    WH_TRACE("recv buffer size = %d\n", sRecvBufferSize);
    WH_TRACE("send buffer size = %d\n", sSendBufferSize);

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
    default:
        break;
    }

    WH_TRACE("unknown connect mode %d\n", mode);
    return FALSE;
}

void WH_SetReceiver(WHReceiverFunc proc)
{
    sReceiverFunc = proc;
    if (WM_SetPortCallback(WH_DATA_PORT, WH_PortReceiveCallback, NULL) != WM_ERRCODE_SUCCESS) {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        WH_TRACE("WM not Initialized\n");
    }
}

BOOL WH_SendData(const void *data, u16 datasize, WHSendCallbackFunc callback)
{
    return WH_StateInSetMPData(data, datasize, callback);
}

void WH_Reset(void)
{
    WMErrCode result;

    if (sSysState == WH_SYSSTATE_DATASHARING) {
        result = WM_EndDataSharing(&sDSInfo);
        if (result != WM_ERRCODE_SUCCESS) {
            WH_REPORT_FAILURE(result);
        }
    }

    if (!WH_StateInReset()) {
        WH_ChangeSysState(WH_SYSSTATE_FATAL);
    }
}

void WH_Finalize(void)
{
    if (sSysState == WH_SYSSTATE_IDLE) {
        WH_TRACE("already WH_SYSSTATE_IDLE\n");
        return;
    }

    WH_TRACE("WH_Finalize, state = %d\n", sSysState);

    if (sSysState == WH_SYSSTATE_SCANNING) {
        if (!WH_EndScan()) {
            WH_Reset();
        }
        return;
    }

    if (sSysState != WH_SYSSTATE_KEYSHARING && sSysState != WH_SYSSTATE_DATASHARING
        && sSysState != WH_SYSSTATE_CONNECTED) {
        // Not connected: reset
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

    case WH_CONNECTMODE_DS_CHILD:
        if (WM_EndDataSharing(&sDSInfo) != WM_ERRCODE_SUCCESS) {
            WH_Reset();
            break;
        }
        // Then as MP
    case WH_CONNECTMODE_MP_CHILD:
        if (!WH_StateInEndChildMP()) {
            WH_Reset();
        }
        break;

    case WH_CONNECTMODE_KS_PARENT:
        if (!WH_StateInEndParentKeyShare()) {
            WH_Reset();
        }
        break;

    case WH_CONNECTMODE_DS_PARENT:
        if (WM_EndDataSharing(&sDSInfo) != WM_ERRCODE_SUCCESS) {
            WH_Reset();
            break;
        }
        // Then as MP
    case WH_CONNECTMODE_MP_PARENT:
        if (!WH_StateInEndParentMP()) {
            WH_Reset();
        }
        break;
    }
}

BOOL WH_End(void)
{
    if (sSysState != WH_SYSSTATE_IDLE) {
        OS_Terminate();
    }

    WH_ChangeSysState(WH_SYSSTATE_BUSY);
    if (WM_End(WH_StateOutEnd) != WM_ERRCODE_OPERATING) {
        WH_ChangeSysState(WH_SYSSTATE_ERROR);
        return FALSE;
    }

    whInitialized = 0;
    return TRUE;
}

}

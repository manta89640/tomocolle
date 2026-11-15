#ifndef WH_H
#define WH_H

// WH, the wireless wrapper of the SDK's sample code, as the game compiles it in: a state machine over the WM
// driver's asynchronous calls. The declarations are this project's own, written from what the game's code does.

#include <nitro.h>
#include <nitro/wm.h>

// What the game sets up (measured from the code): one child, data-sharing on port 13, data on port 14.
#define WH_CHILD_MAX            1
#define WH_DS_DATA_SIZE         200
#define WH_DS_PORT              13
#define WH_DATA_PORT            14
#define WH_DATA_PRIO            WM_PRIORITY_NORMAL
#define WH_DMA_NO               2
#define WH_PARENT_MAX_SIZE      (WH_DS_DATA_SIZE * (1 + WH_CHILD_MAX) + WM_DS_HEADER_SIZE)
#define WH_CHILD_MAX_SIZE       WH_DS_DATA_SIZE

#define WH_PARENT_RECV_BUFFER_SIZE  WM_SIZE_MP_PARENT_RECEIVE_BUFFER(WH_CHILD_MAX_SIZE, WH_CHILD_MAX, FALSE)
#define WH_PARENT_SEND_BUFFER_SIZE  WM_SIZE_MP_PARENT_SEND_BUFFER(WH_PARENT_MAX_SIZE, FALSE)
#define WH_CHILD_RECV_BUFFER_SIZE   WM_SIZE_MP_CHILD_RECEIVE_BUFFER(WH_PARENT_MAX_SIZE, FALSE)
#define WH_CHILD_SEND_BUFFER_SIZE   WM_SIZE_MP_CHILD_SEND_BUFFER(WH_CHILD_MAX_SIZE, FALSE)

enum {
    WH_SYSSTATE_STOP,
    WH_SYSSTATE_IDLE,
    WH_SYSSTATE_SCANNING,
    WH_SYSSTATE_BUSY,
    WH_SYSSTATE_CONNECTED,
    WH_SYSSTATE_DATASHARING,
    WH_SYSSTATE_KEYSHARING,
    WH_SYSSTATE_MEASURECHANNEL,
    WH_SYSSTATE_CONNECT_FAIL,
    WH_SYSSTATE_ERROR,
    WH_SYSSTATE_FATAL,
    WH_SYSSTATE_NUM
};

enum {
    WH_CONNECTMODE_MP_PARENT,
    WH_CONNECTMODE_MP_CHILD,
    WH_CONNECTMODE_KS_PARENT,
    WH_CONNECTMODE_KS_CHILD,
    WH_CONNECTMODE_DS_PARENT,
    WH_CONNECTMODE_DS_CHILD,
    WH_CONNECTMODE_NUM
};

// Errors of WH's own, after those of WM
enum {
    WH_ERRCODE_DISCONNECTED = WM_ERRCODE_MAX,
    WH_ERRCODE_PARENT_NOT_FOUND,
    WH_ERRCODE_NO_RADIO,
    WH_ERRCODE_LOST_PARENT,
    WH_ERRCODE_NOMORE_CHANNEL,
    WH_ERRCODE_MAX
};

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*WHStartScanCallbackFunc)(WMBssDesc *bssDesc);
typedef void (*WHSendCallbackFunc)(BOOL result);
typedef BOOL (*WHJudgeAcceptFunc)(WMStartParentCallback *cb);
typedef void (*WHReceiverFunc)(u16 aid, u16 *data, u16 size);
typedef u16 (*WHParentWEPKeyGeneratorFunc)(u16 *wepkey, const WMParentParam *parentParam);
typedef u16 (*WHChildWEPKeyGeneratorFunc)(u16 *wepkey, const WMBssDesc *bssDesc);

// The SSID (the user data of the connection) of the child that connected last, copied by the parent's callback
extern u16 gWHChildSsid[WM_SIZE_CHILD_SSID / sizeof(u16)];

BOOL WH_Initialize(void);
BOOL WH_End(void);
void WH_Reset(void);
void WH_Finalize(void);

void WH_SetGgid(u32 ggid);
void WH_SetSsid(const void *ssid, u32 length);
void WH_SetUserGameInfo(u16 *userGameInfo, u16 length);

int WH_GetLinkLevel(void);
u16 WH_GetBitmap(void);
int WH_GetSystemState(void);
void WH_PrintBssDesc(WMBssDesc *info);

BOOL WH_StartMeasureChannel(void);
u16 WH_GetMeasureChannel(void);

BOOL WH_StartScan(WHStartScanCallbackFunc callback, const u8 *macAddr, u16 channel);
BOOL WH_EndScan(void);

BOOL WH_ParentConnect(int mode, u16 tgid, u16 channel);
BOOL WH_ChildConnect(int mode, WMBssDesc *bssDesc);

void WH_SetReceiver(WHReceiverFunc proc);
BOOL WH_SendData(const void *data, u16 datasize, WHSendCallbackFunc callback);

#ifdef __cplusplus
}
#endif

#endif

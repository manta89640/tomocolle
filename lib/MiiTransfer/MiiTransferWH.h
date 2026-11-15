#ifndef WIRELESS_WH_H
#define WIRELESS_WH_H

// The second copy of WH, the SDK's wireless sample code, which the Mii transfer of wireless play (MiiTransfer.cpp)
// is built on. See MiiTransferWH.cpp. The declarations are this project's own, written from what
// the code does, under WH's names.

#include <nitro.h>

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
};

enum {
    WH_CONNECTMODE_MP_PARENT,
    WH_CONNECTMODE_MP_CHILD,
    WH_CONNECTMODE_KS_PARENT,
    WH_CONNECTMODE_KS_CHILD,
    WH_CONNECTMODE_DS_PARENT,
    WH_CONNECTMODE_DS_CHILD,
};

// WH's own error codes follow WM's
enum {
    WH_ERRCODE_DISCONNECTED = WM_ERRCODE_MAX,
    WH_ERRCODE_PARENT_NOT_FOUND,
    WH_ERRCODE_NO_RADIO,
};

typedef void (*WHStartScanCallbackFunc)(WMBssDesc *bssDesc);
typedef void (*WHReceiverFunc)(u16 aid, u16 *data, u16 size);
typedef void (*WHSendCallbackFunc)(BOOL result);
typedef u16 (*WHChildWEPKeyGeneratorFunc)(u16 *wepkey, const WMBssDesc *bssDesc);

void WH_TurnOffPictoCatch(void);
BOOL WH_StartScan(WHStartScanCallbackFunc callback, const u8 *macAddr, u16 channel);
BOOL WH_EndScan(void);
void WH_SetGgid(u32 ggid);
int WH_GetSystemState(void);
BOOL WH_Initialize(void);
BOOL WH_ChildConnect(int mode, WMBssDesc *bssDesc);
void WH_SetReceiver(WHReceiverFunc proc);
BOOL WH_SendData(const void *data, u16 datasize, WHSendCallbackFunc callback);
void WH_Reset(void);
void WH_Finalize(void);
BOOL WH_End(void);

#endif // WIRELESS_WH_H

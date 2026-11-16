// The Mii transfer of wireless play (overlay 109): the protocol by which this DS, as a child, finds parents, connects
// to one and sends Miis to it or takes them from it, on the second copy of WH (MiiTransferWH.cpp) and the Mii records of
// MiiTransferList.cpp. Overlay 109 calls it.
//
// Two layers, both updated once a frame: a session (sSessionState: start WH, scan, connect, exchange packets, end),
// which sends one packet at a time, numbered by sSeq, and takes the parent's answers; and over it a protocol of
// messages (sMsgState: 0x10 a Mii, 0x20 the answer to one, 0x30 to 0x50, 0xD0 idle, 0xE0 cut, 0xF0 the user's
// name) and the state that overlay 109 sees (sState).
//
// The file name is descriptive, not the original, and so are the names of the functions and statics: no name of this
// code survives in the ROM. They are the project's own, after what each does (MiiTransfer_ for the file's functions).

#include "MiiTransfer.h"
#include "MiiTransferWH.h"

#define PARENT_MAX 8
#define PARENT_TIMER 20

// A message of the protocol
typedef struct Message {
    u8 type;
    u8 arg;
    union {
        MiiRecord record; // type 0x10
        u16 name[10];     // type 0xF0
    };
} Message;

// A packet of the session, as sent and received
typedef struct Packet {
    u8 seq;
    u8 unk1;
    Message msg;
    u8 size;
    BOOL valid;
} ATTRIBUTE_ALIGN(32) Packet;

// A parent found by the scan, as WH gives it
typedef struct Parent {
    WMBssDesc bssDesc;
} Parent;

typedef void (*SendResultCallback)(int result);
typedef void (*ReceiveCallback)(int index, int result);

extern "C" {

BOOL MiiTransfer_IsConnected(void);
int MiiTransfer_GetSessionState(void);
void MiiTransfer_SetReceiveCallback(ReceiveCallback callback);
void MiiTransfer_SetSendResultCallback(SendResultCallback callback);
void MiiTransfer_CallReceiveCallback(void);
void MiiTransfer_CallSendResultCallback(void);
void MiiTransfer_Init(void *idSource, MiiRecord *records, u32 count);
void MiiTransfer_UpdateReceive(void);
void MiiTransfer_UpdateSend(void);
void MiiTransfer_Update(void);
void MiiTransfer_SetAcceptSpecial(BOOL flag);
void MiiTransfer_InitSession(void);
void MiiTransfer_UpdateSession(void);
void MiiTransfer_UpdateSessionReceive(void);
void MiiTransfer_UpdateSessionSend(void);
void MiiTransfer_StartSession(void);
void MiiTransfer_EndSession(void);
BOOL MiiTransfer_QueuePacket(const void *data, int size);
BOOL MiiTransfer_ReadPacket(void *dest);
void MiiTransfer_ConnectToParent(int index);
void MiiTransfer_OnParentFound(WMBssDesc *bssDesc);
void MiiTransfer_OnPacketReceived(u16 aid, u16 *data, u16 size);
void MiiTransfer_OnPacketSent(BOOL result);
BOOL MiiTransfer_HasParent(int index);
int MiiTransfer_GetParentAge(int index);
WMBssDesc *MiiTransfer_GetParent(int index);
int MiiTransfer_GetParentCount(void);
u8 MiiTransfer_GetParentUserGameInfoLength(u16 index);
u16 *MiiTransfer_GetParentUserGameInfo(u16 index);
void MiiTransfer_InitMessages(void);
void MiiTransfer_ResetMessages(void);
void MiiTransfer_HandleMessage(void);
void MiiTransfer_SendMessages(void);
BOOL MiiTransfer_CanSendMessage(void);
void MiiTransfer_PostMessage(int type, int arg, const MiiRecord *record);
int MiiTransfer_GetMessageSize(int type);
BOOL MiiTransfer_IsMessageIdle(void);
BOOL MiiTransfer_PostMii(u16 index);
BOOL MiiTransfer_PostCut(void);
BOOL MiiTransfer_IsSendingMii(void);
BOOL MiiTransfer_IsReceivingMii(void);
void MiiTransfer_UpdateState(void);
void MiiTransfer_Start(void);
void MiiTransfer_Cut(void);
void MiiTransfer_Connect(int index);
void MiiTransfer_SendMii(u16 index);
void MiiTransfer_SetReceivedNew(u16 index);
void MiiTransfer_SetReceivedExisting(u16 index);
void MiiTransfer_SetReceiveFull(const MiiCreateId *createId);
void MiiTransfer_SetReceivedSpecial(u16 index);
void MiiTransfer_SetSent(void);
void MiiTransfer_OnCutByParent(void);
int MiiTransfer_GetState(void);
void MiiTransfer_SetState(int state);

} // extern "C"


static u8 sSeq;
static int sPendingReply;
static BOOL sSending;
static ReceiveCallback sReceiveCallback;
static int sParentCount;
static BOOL sCutRequested;
static int sReceiveResult;
static SendResultCallback sSendResultCallback;
static int sSessionState;
static BOOL sSessionLost;
static BOOL sCutByParent;
static int sParentIndex;
static int sReceivedIndex;
static u32 sStateFrames;
static int sSendIndex;
static int sSendResult;
static int sState;
static BOOL sReceiveReported;
static BOOL sSendReported;
static int sMiiIndex;
static void *sIdSource;
static BOOL sSendPending;
static BOOL sAcceptSpecial;
static int sMsgState;
static OSMutex sMutex;
static u32 sParentAges[PARENT_MAX];
static u32 sParentTimers[PARENT_MAX];
static Message sSendMsg;
static Message sRecvMsg;
static Packet sRecvPacket;
static Packet sSendPacket;
static Packet sRecvBuffer;
static Parent sParents[PARENT_MAX];

extern "C" BOOL MiiTransfer_IsConnected(void) {
    return sSessionState == 4;
}

extern "C" int MiiTransfer_GetSessionState(void) {
    return sSessionState;
}

extern "C" void MiiTransfer_SetReceiveCallback(ReceiveCallback callback) {
    sReceiveCallback = callback;
}

extern "C" void MiiTransfer_SetSendResultCallback(SendResultCallback callback) {
    sSendResultCallback = callback;
}

extern "C" void MiiTransfer_CallReceiveCallback(void) {
    if (sReceiveCallback != NULL) {
        sReceiveCallback(sReceivedIndex, sReceiveResult);
    }
}

extern "C" void MiiTransfer_CallSendResultCallback(void) {
    if (sSendResultCallback != NULL) {
        sSendResultCallback(sSendResult);
    }
}

extern "C" void MiiTransfer_Init(void *idSource, MiiRecord *records, u32 count) {
    sIdSource = idSource;
    MiiTransferList_Init(records, count);
    MiiTransfer_InitSession();
    MiiTransfer_InitMessages();
}

extern "C" void MiiTransfer_UpdateReceive(void) {
    MiiTransfer_UpdateSessionReceive();
    MiiTransfer_HandleMessage();
    MiiTransfer_UpdateState();
}

extern "C" void MiiTransfer_UpdateSend(void) {
    MiiTransfer_SendMessages();
    MiiTransfer_UpdateSessionSend();
    if (sState != 0) {
        int state = MiiTransfer_GetSessionState();
        if (state == 0 || state == 6) {
            sSessionLost = TRUE;
        }
    }
}

extern "C" void MiiTransfer_Update(void) {
    MiiTransfer_UpdateSend();
    MiiTransfer_UpdateReceive();
}

extern "C" void MiiTransfer_SetAcceptSpecial(BOOL flag) {
    sAcceptSpecial = flag;
}

extern "C" void MiiTransfer_InitSession(void) {
    sSessionState = 0;
    OS_InitMutex(&sMutex);
    WH_SetGgid(0x412);
    sRecvBuffer.unk1 = 0;
    sSendPacket.unk1 = 0;
    sRecvPacket.unk1 = 0;
}

// The session's update. DOES NOT MATCH, by five branches only: every instruction and register is the ROM's, but in the
// jump table of case 3 the unlisted states branch to the function's end where the original branches to the inner
// switch's break (0x02077A2C); other case orders and fall-throughs tried give the same. Compiled at optimization level
// 2 without propagation, as the ROM has it: loops neither rotated nor strength reduced, the 0 for WH_StartScan kept in
// r4 (a variable, i, shared with the loop and the length), and the copy of the address from a local initialiser.
#pragma optimization_level 2
#pragma opt_propagation off
extern "C" void MiiTransfer_UpdateSession(void) {
    volatile u8 userGameInfo[0x16];
    int i;

    switch (sSessionState) {
    case 0:
        break;
    case 1:
        switch (WH_GetSystemState()) {
        case WH_SYSSTATE_IDLE: {
            u8 macAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            i = 0;
            WH_TurnOffPictoCatch();
            WH_StartScan(MiiTransfer_OnParentFound, macAddr, i);
            sSessionState = 2;
            break;
        }
        case WH_SYSSTATE_BUSY:
            break;
        }
        break;
    case 2:
        for (i = 0; i < PARENT_MAX; i++) {
            if (sParentTimers[i] != 0 && --sParentTimers[i] == 0) {
                sParentTimers[i] = PARENT_TIMER;
                sParentAges[i]++;
            }
        }
        break;
    case 3:
        switch (WH_GetSystemState()) {
        case WH_SYSSTATE_IDLE:
            WH_ChildConnect(WH_CONNECTMODE_MP_CHILD, &sParents[sParentIndex].bssDesc);
            WH_SetReceiver(MiiTransfer_OnPacketReceived);
            sRecvBuffer.valid = FALSE;
            sSendPacket.valid = FALSE;
            sRecvPacket.valid = FALSE;
            break;
        case WH_SYSSTATE_CONNECTED: {
            i = MiiTransfer_GetParentUserGameInfoLength(sParentIndex);
            u16 *info = MiiTransfer_GetParentUserGameInfo(sParentIndex);
            for (int j = 0; j < i; j++) {
                userGameInfo[j] = *info++;
            }
            sSessionState = 4;
            break;
        }
        case WH_SYSSTATE_CONNECT_FAIL:
            sSessionState = 5;
            break;
        case WH_SYSSTATE_SCANNING:
            WH_EndScan();
            break;
        case WH_SYSSTATE_BUSY:
            break;
        }
        break;
    case 4: {
        int state = WH_GetSystemState();
        if (state != WH_SYSSTATE_BUSY && state != WH_SYSSTATE_CONNECTED && state != WH_SYSSTATE_CONNECT_FAIL) {
            WH_Reset();
            sSessionState = 5;
        }
        break;
    }
    case 5:
        switch (WH_GetSystemState()) {
        case WH_SYSSTATE_IDLE:
            WH_End();
            break;
        case WH_SYSSTATE_SCANNING:
            WH_EndScan();
            break;
        case WH_SYSSTATE_BUSY:
            break;
        case WH_SYSSTATE_CONNECTED:
            WH_Finalize();
            break;
        case WH_SYSSTATE_STOP:
            sSessionState = 0;
            break;
        default:
            WH_Reset();
            break;
        }
        break;
    case 6:
        if (WH_GetSystemState() == WH_SYSSTATE_IDLE) {
            WH_End();
            sSessionState = 5;
        }
        break;
    }
    if (WH_GetSystemState() == WH_SYSSTATE_ERROR) {
        WH_Reset();
        sSessionState = 6;
    }
}
#pragma optimization_level reset

extern "C" void MiiTransfer_UpdateSessionReceive(void) {
    OS_LockMutex(&sMutex);
    MiiTransfer_UpdateSession();
    if (sSessionState == 4) {
        if (sRecvBuffer.valid) {
            MI_CpuCopy8(&sRecvBuffer, &sRecvPacket, sizeof(Packet));
            sRecvBuffer.valid = FALSE;
            sSeq = sRecvPacket.seq;
        }
        if (sSendPacket.valid == TRUE && sSending == FALSE && sSendPacket.seq == (u8)(sSeq - 1)) {
            sSendPacket.valid = FALSE;
        }
    }
    OS_UnlockMutex(&sMutex);
}

extern "C" void MiiTransfer_UpdateSessionSend(void) {
    if (sSessionState != 4) {
        return;
    }
    OS_LockMutex(&sMutex);
    if (sRecvPacket.valid) {
        sRecvPacket.valid = FALSE;
    }
    if (sSendPacket.valid == TRUE && sSending == FALSE && sSendPacket.seq == (u8)(sSeq - 1)) {
        sSendPacket.valid = FALSE;
    }
    if (sSendPacket.valid == TRUE && sSending == FALSE && sSeq == sSendPacket.seq) {
        if (WH_SendData(&sSendPacket, sSendPacket.size, MiiTransfer_OnPacketSent)) {
            sSending = TRUE;
        }
    }
    OS_UnlockMutex(&sMutex);
}

extern "C" void MiiTransfer_StartSession(void) {
    if (sSessionState != 0) {
        return;
    }
    MI_CpuClearFast(sParents, sizeof(sParents));
    for (int i = 0; i < PARENT_MAX; i++) {
        sParentTimers[i] = 0;
        sParentAges[i] = 0;
    }
    sParentCount = 0;
    if (WH_Initialize()) {
        sSessionState = 1;
        sSeq = 0;
    }
}

extern "C" void MiiTransfer_EndSession(void) {
    sSessionState = 5;
}

extern "C" BOOL MiiTransfer_QueuePacket(const void *data, int size) {
    if (sSessionState != 4) {
        return FALSE;
    }
    if (sSendPacket.valid == FALSE) {
        if ((sSeq & 1) == 1) {
            return FALSE;
        }
        sSendPacket.seq = ++sSeq;
        MI_CpuCopy8(data, &sSendPacket.msg, size);
        sSendPacket.size = size + 2;
        sSendPacket.valid = TRUE;
        sSending = FALSE;
        return TRUE;
    }
    return FALSE;
}

extern "C" BOOL MiiTransfer_ReadPacket(void *dest) {
    if (sSessionState != 4) {
        return FALSE;
    }
    if (sRecvPacket.valid) {
        MI_CpuCopy8(&sRecvPacket.msg, dest, sRecvPacket.size - 2);
    }
    return sRecvPacket.valid;
}

extern "C" void MiiTransfer_ConnectToParent(int index) {
    if (index < 0 || index >= sParentCount) {
        return;
    }
    if (sSessionState == 2) {
        sParentIndex = index;
        sSessionState = 3;
    }
}

extern "C" void MiiTransfer_OnParentFound(WMBssDesc *bssDesc) {
    int count = sParentCount;
    if (count >= PARENT_MAX) {
        return;
    }
    for (int i = 0; i < count; i++) {
        WMBssDesc *parent = &sParents[i].bssDesc;
        if (parent->bssid[0] == bssDesc->bssid[0] && parent->bssid[1] == bssDesc->bssid[1] &&
            parent->bssid[2] == bssDesc->bssid[2] && parent->bssid[3] == bssDesc->bssid[3] &&
            parent->bssid[4] == bssDesc->bssid[4] && parent->bssid[5] == bssDesc->bssid[5]) {
            sParentTimers[i] = PARENT_TIMER;
            sParentAges[i] = 0;
            if (bssDesc->gameInfo.tgid != sParents[i].bssDesc.gameInfo.tgid) {
                sParents[i] = *(Parent *)bssDesc;
            }
            return;
        }
    }
    sParents[count] = *(Parent *)bssDesc;
    sParentTimers[count] = PARENT_TIMER;
    sParentAges[count] = 0;
    sParentCount++;
}

// WH's receiver: takes a packet from the parent. Compiled without common subexpression elimination, as the ROM has
// it: with it, the value of sSeq and its low bit swap registers (r0 and r1), and no form of the test changes that.
#pragma opt_common_subs off
extern "C" void MiiTransfer_OnPacketReceived(u16 aid, u16 *data, u16 size) {
    if (data == NULL) {
        sSessionState = 5;
        return;
    }
    OS_LockMutex(&sMutex);
    u8 seq = *(u8 *)data;
    if ((sSeq & 1) == 1 && seq == (u8)(sSeq + 1)) {
        sRecvBuffer.valid = TRUE;
        sRecvBuffer.size = size;
        MI_CpuCopy8(data, &sRecvBuffer, sRecvBuffer.size);
    } else if (seq != sRecvBuffer.seq) {
        sSeq = seq - 1;
    }
    OS_UnlockMutex(&sMutex);
}
#pragma opt_common_subs reset

extern "C" void MiiTransfer_OnPacketSent(BOOL result) {
    OS_LockMutex(&sMutex);
    sSending = FALSE;
    OS_UnlockMutex(&sMutex);
}

extern "C" BOOL MiiTransfer_HasParent(int index) {
    if (index >= sParentCount) {
        return FALSE;
    }
    return sParentTimers[index] != 0;
}

extern "C" int MiiTransfer_GetParentAge(int index) {
    if (index >= sParentCount) {
        return -1;
    }
    return sParentAges[index];
}

extern "C" WMBssDesc *MiiTransfer_GetParent(int index) {
    if (index >= sParentCount) {
        return NULL;
    }
    return &sParents[index].bssDesc;
}

extern "C" int MiiTransfer_GetParentCount(void) {
    return sParentCount;
}

extern "C" u8 MiiTransfer_GetParentUserGameInfoLength(u16 index) {
    WMBssDesc *bssDesc = MiiTransfer_GetParent(index);
    if (bssDesc != NULL) {
        return bssDesc->gameInfo.userGameInfoLength;
    }
    return 0;
}

extern "C" u16 *MiiTransfer_GetParentUserGameInfo(u16 index) {
    WMBssDesc *bssDesc = MiiTransfer_GetParent(index);
    if (bssDesc != NULL) {
        return bssDesc->gameInfo.userGameInfo;
    }
    return NULL;
}

extern "C" void MiiTransfer_InitMessages(void) {
    MiiTransfer_ResetMessages();
}

extern "C" void MiiTransfer_ResetMessages(void) {
    sMsgState = 0xF0;
    sMiiIndex = -1;
    sPendingReply = 0;
    sSendPending = FALSE;
    sSendMsg.type = 0xD0;
    sSendMsg.arg = 0;
    sSendReported = FALSE;
    sReceiveReported = FALSE;
}

extern "C" void MiiTransfer_HandleMessage(void) {
    if (!MiiTransfer_IsConnected()) {
        return;
    }
    if (!MiiTransfer_ReadPacket(&sRecvMsg)) {
        return;
    }
    if (MiiTransfer_CanSendMessage()) {
        return;
    }
    switch (sMsgState) {
    case 0x11: {
        if (sRecvMsg.type != 0x20) {
            return;
        }
        MiiRecord *record = MiiTransferList_Get(sMiiIndex);
        switch (sRecvMsg.arg) {
        case 0:
            if (func_02073264(&record->createId)) {
                func_02073394(&record->createId);
                sPendingReply = 3;
            } else {
                MiiTransfer_PostMessage(0x30, 0, NULL);
                MiiTransfer_SetSent();
            }
            break;
        case 1:
            MiiTransfer_PostMessage(0x30, 0, NULL);
            if (func_02073264(&record->createId)) {
                sSendResult = -4;
            } else {
                sSendResult = -5;
            }
            break;
        case 2:
            MiiTransfer_PostMessage(0x30, 0, NULL);
            MiiTransfer_SetSent();
            break;
        case 5:
            MiiTransfer_PostMessage(0x30, 0, NULL);
            sSendResult = 2;
            break;
        default:
            MiiTransfer_PostMessage(0x30, 0, NULL);
            if (func_02073264(&record->createId)) {
                sSendResult = -2;
            } else {
                sSendResult = -3;
            }
            break;
        }
        sMsgState = 0x20;
        return;
    }
    case 0x31:
        if (sRecvMsg.type != 0x40) {
            return;
        }
        if (sSendReported) {
            MiiTransfer_PostMessage(0x50, 0, NULL);
            sSendReported = FALSE;
        }
        sMsgState = 0x40;
        return;
    case 0x51:
    case 0xD1:
    case 0xF0:
        if (sRecvMsg.type == 0xD0) {
            sMsgState = 0xD0;
            return;
        }
        if (sRecvMsg.type == 0x10) {
            if (!func_02073284(&sRecvMsg.record.createId)) {
                if (sAcceptSpecial == FALSE && func_02073264(&sRecvMsg.record.createId)) {
                    sReceiveResult = -2;
                    MiiTransfer_PostMessage(0x20, 4, NULL);
                } else {
                    // The Wii's localOnly set and favorite cleared (MiiData_SetLocalOnly, MiiData_SetFavorite)
                    sRecvMsg.record.unk20[0] |= 4;
                    sRecvMsg.record.unk0[0] &= ~1;
                    sMiiIndex = MiiTransferList_Find(&sRecvMsg.record.createId);
                    if (sMiiIndex >= 0) {
                        if (func_02073264(&sRecvMsg.record.createId)) {
                            MiiTransfer_PostMessage(0x20, 5, NULL);
                            MiiTransfer_SetReceivedSpecial(sMiiIndex);
                        } else if (func_020732a0(sIdSource, &sRecvMsg.record.createId)) {
                            MiiTransfer_PostMessage(0x20, 2, NULL);
                            MiiTransfer_SetReceivedExisting(sMiiIndex);
                        } else {
                            MiiTransferList_Set(sMiiIndex, &sRecvMsg.record);
                            sPendingReply = 2;
                            MiiTransfer_SetReceivedExisting(sMiiIndex);
                        }
                    } else {
                        sMiiIndex = MiiTransferList_FindFree();
                        if (sMiiIndex >= 0) {
                            MiiTransferList_Set(sMiiIndex, &sRecvMsg.record);
                            sPendingReply = 1;
                            MiiTransfer_SetReceivedNew(sMiiIndex);
                        } else {
                            MiiTransfer_SetReceiveFull(&sRecvMsg.record.createId);
                            MiiTransfer_PostMessage(0x20, 1, NULL);
                        }
                    }
                }
            } else {
                sReceiveResult = -3;
                MiiTransfer_PostMessage(0x20, 4, NULL);
            }
            sMsgState = 0x10;
            sReceiveReported = FALSE;
            return;
        }
        if (sRecvMsg.type == 0xE0) {
            MiiTransfer_OnCutByParent();
            sMsgState = 0xE0;
        }
        return;
    case 0x21:
        if (sRecvMsg.type != 0x30 || sRecvMsg.arg != 0) {
            return;
        }
        if (sReceiveReported) {
            MiiTransfer_PostMessage(0x40, 0, NULL);
            sReceiveReported = FALSE;
        }
        sMsgState = 0x30;
        return;
    case 0x41:
        if (sRecvMsg.type == 0x50) {
            sMsgState = 0x50;
        }
        return;
    }
}

extern "C" void MiiTransfer_SendMessages(void) {
    if (!MiiTransfer_IsConnected()) {
        return;
    }
    if (!MiiTransfer_CanSendMessage() && sMsgState != 0xF0) {
        return;
    }
    switch (sMsgState) {
    case 0x20:
        if (sPendingReply) {
            MiiTransfer_SetSent();
            MiiTransfer_PostMessage(0x30, 0, NULL);
            sPendingReply = 0;
        }
        break;
    case 0x40:
        if (sSendReported) {
            MiiTransfer_PostMessage(0x50, 0, NULL);
            sSendReported = FALSE;
        }
        break;
    case 0x10:
        if (sPendingReply == 1 || sPendingReply == 2) {
            if (sPendingReply == 1) {
                MiiTransfer_PostMessage(0x20, 0, NULL);
            } else {
                MiiTransfer_PostMessage(0x20, 2, NULL);
            }
            sPendingReply = 0;
        }
        break;
    case 0x30:
        if (sReceiveReported) {
            MiiTransfer_PostMessage(0x40, 0, NULL);
            sReceiveReported = FALSE;
        }
        break;
    case 0xF0:
        if (!sSendPending) {
            MiiTransfer_PostMessage(0xF0, 0, NULL);
        }
        break;
    }
    if (!sSendPending && (sMsgState == 0x50 || sMsgState == 0xD0)) {
        MiiTransfer_PostMessage(0xD0, 0, NULL);
    }
    if (!sSendPending) {
        return;
    }
    if (!MiiTransfer_QueuePacket(&sSendMsg, MiiTransfer_GetMessageSize(sSendMsg.type))) {
        return;
    }
    switch (sSendMsg.type) {
    case 0x10:
        sMsgState = 0x11;
        break;
    case 0x20:
        sMsgState = 0x21;
        break;
    case 0x30:
        sMsgState = 0x31;
        break;
    case 0x40:
        sMsgState = 0x41;
        break;
    case 0x50:
        sMsgState = 0x51;
        break;
    case 0xD0:
        sMsgState = 0xD1;
        break;
    case 0xE0:
        sMsgState = 0xE1;
        break;
    case 0xF0:
        sMsgState = 0xD1;
        break;
    }
    sSendPending = FALSE;
}

// Whether a message may be sent: not while one sent is unanswered (odd states), nor before the name is sent
extern "C" BOOL MiiTransfer_CanSendMessage(void) {
    return !(sMsgState & 1) && sMsgState != 0xF0;
}

// Puts a message in the send buffer
extern "C" void MiiTransfer_PostMessage(int type, int arg, const MiiRecord *record) {
    sSendMsg.type = type;
    sSendMsg.arg = arg;
    if (type == 0x10) {
        MI_CpuCopy8(record, &sSendMsg.record, sizeof(MiiRecord));
    } else if (type == 0xF0) {
        OSOwnerInfo info;
        OS_GetOwnerInfo(&info);
        for (int i = 0; i < 10; i++) {
            sSendMsg.name[i] = (i < info.nickNameLength) ? info.nickName[i] : 0;
        }
    }
    sSendPending = TRUE;
}

// The size of a message of a type
extern "C" int MiiTransfer_GetMessageSize(int type) {
    switch (type) {
    case 0x10:
        return 0x4C;
    case 0xF0:
        return 0x16;
    default:
        return 2;
    }
}

extern "C" BOOL MiiTransfer_IsMessageIdle(void) {
    return sMsgState == 0xD0 || sMsgState == 0x50;
}

// Sends the Mii of slot index
extern "C" BOOL MiiTransfer_PostMii(u16 index) {
    if (!MiiTransfer_IsMessageIdle()) {
        return FALSE;
    }
    MiiRecord *record = MiiTransferList_Get(index);
    if (record == NULL) {
        return FALSE;
    }
    MiiTransfer_PostMessage(0x10, 0, record);
    sMiiIndex = index;
    sSendReported = FALSE;
    return TRUE;
}

extern "C" BOOL MiiTransfer_PostCut(void) {
    if (!MiiTransfer_IsMessageIdle()) {
        return FALSE;
    }
    MiiTransfer_PostMessage(0xE0, 0, NULL);
    return TRUE;
}

extern "C" BOOL MiiTransfer_IsSendingMii(void) {
    switch (sMsgState) {
    case 0x11:
    case 0x20:
        return TRUE;
    }
    return FALSE;
}

extern "C" BOOL MiiTransfer_IsReceivingMii(void) {
    switch (sMsgState) {
    case 0x10:
    case 0x21:
    case 0x30:
        return TRUE;
    }
    return FALSE;
}

extern "C" void MiiTransfer_UpdateState(void) {
    sStateFrames++;
    switch (sState) {
    case 0:
        break;
    case 1:
        if (sSessionLost) {
            MiiTransfer_SetState(5);
            MiiTransfer_EndSession();
        }
        break;
    case 2:
        if (sSessionLost) {
            MiiTransfer_SetState(5);
            MiiTransfer_EndSession();
        } else {
            switch (MiiTransfer_GetSessionState()) {
            case 4:
                MiiTransfer_SetState(6);
                break;
            case 6:
                MiiTransfer_SetState(5);
                MiiTransfer_EndSession();
                break;
            }
        }
        break;
    case 3:
        if (MiiTransfer_GetSessionState() == 0) {
            MiiTransfer_SetState(0);
        }
        break;
    case 4:
        if (sSessionLost || sStateFrames > 300) {
            MiiTransfer_SetState(3);
            MiiTransfer_EndSession();
        } else {
            MiiTransfer_PostCut();
        }
        break;
    case 5:
        if (MiiTransfer_GetSessionState() == 0) {
            MiiTransfer_SetState(0);
        }
        break;
    case 6:
        if (sCutByParent) {
            MiiTransfer_SetState(3);
        } else if (sReceiveResult) {
            MiiTransfer_CallReceiveCallback();
            MiiTransfer_SetState(10);
        } else if (sSessionLost) {
            MiiTransfer_SetState(5);
            MiiTransfer_EndSession();
        } else if (sCutRequested && MiiTransfer_PostCut()) {
            MiiTransfer_SetState(4);
            sCutRequested = FALSE;
        }
        break;
    case 7:
        if (sSessionLost) {
            MiiTransfer_SetState(5);
        }
        break;
    case 10:
        sReceivedIndex = -1;
        sReceiveResult = 0;
        if (sSessionLost) {
            MiiTransfer_SetState(5);
            MiiTransfer_EndSession();
        } else if (!MiiTransfer_IsReceivingMii()) {
            if (sSendIndex >= 0) {
                if (MiiTransfer_PostMii(sSendIndex)) {
                    MiiTransfer_SetState(8);
                } else {
                    MiiTransfer_SetState(9);
                }
            } else {
                MiiTransfer_SetState(6);
            }
        }
        break;
    case 9:
        if (sCutByParent) {
            MiiTransfer_SetState(3);
        } else if (sSessionLost) {
            MiiTransfer_SetState(5);
            MiiTransfer_EndSession();
        } else if (sReceiveResult) {
            MiiTransfer_CallReceiveCallback();
            MiiTransfer_SetState(10);
        } else if (MiiTransfer_PostMii(sSendIndex)) {
            MiiTransfer_SetState(8);
        }
        break;
    case 8:
        if (sSessionLost) {
            MiiTransfer_SetState(5);
            MiiTransfer_EndSession();
        } else if (sSendResult) {
            MiiTransfer_CallSendResultCallback();
            MiiTransfer_SetState(11);
        }
        break;
    case 11:
        sSendIndex = -1;
        sSendResult = 0;
        if (sSessionLost) {
            MiiTransfer_SetState(5);
            MiiTransfer_EndSession();
        } else if (!MiiTransfer_IsSendingMii()) {
            MiiTransfer_SetState(6);
        }
        break;
    }
}

extern "C" void MiiTransfer_Start(void) {
    MiiTransfer_SetState(1);
    MiiTransfer_StartSession();
    MiiTransfer_ResetMessages();
    sReceivedIndex = -1;
    sSendIndex = -1;
    sReceiveResult = 0;
    sSendResult = 0;
    sSessionLost = FALSE;
    sCutByParent = FALSE;
    sCutRequested = FALSE;
}

extern "C" void MiiTransfer_Cut(void) {
    switch (sState) {
    case 1:
    case 2:
        MiiTransfer_SetState(3);
        MiiTransfer_EndSession();
        break;
    case 6:
        if (MiiTransfer_PostCut()) {
            MiiTransfer_SetState(4);
        }
        sCutRequested = TRUE;
        break;
    }
}

extern "C" void MiiTransfer_Connect(int index) {
    MiiTransfer_ConnectToParent(index);
    MiiTransfer_SetState(2);
}

extern "C" void MiiTransfer_SendMii(u16 index) {
    MiiRecord *record = MiiTransferList_Get(index);
    if (record == NULL || func_02073284(&record->createId)) {
        sSendResult = -3;
        MiiTransfer_CallSendResultCallback();
        MiiTransfer_SetState(11);
        return;
    }
    if (func_0207331c(&record->createId)) {
        sSendResult = -1;
        MiiTransfer_CallSendResultCallback();
        MiiTransfer_SetState(11);
        return;
    }
    sSendIndex = index;
    if (MiiTransfer_PostMii(index)) {
        MiiTransfer_SetState(8);
    } else {
        MiiTransfer_SetState(9);
    }
}

extern "C" void MiiTransfer_SetReceivedNew(u16 index) {
    sReceiveResult = 1;
    sReceivedIndex = index;
}

extern "C" void MiiTransfer_SetReceivedExisting(u16 index) {
    sReceiveResult = 3;
    sReceivedIndex = index;
}

extern "C" void MiiTransfer_SetReceiveFull(const MiiCreateId *createId) {
    if (func_02073264(createId)) {
        sReceiveResult = -4;
    } else {
        sReceiveResult = -5;
    }
    sReceivedIndex = -1;
}

extern "C" void MiiTransfer_SetReceivedSpecial(u16 index) {
    sReceiveResult = 2;
    sReceivedIndex = index;
}

extern "C" void MiiTransfer_SetSent(void) {
    sSendResult = 1;
}

extern "C" void MiiTransfer_OnCutByParent(void) {
    MiiTransfer_EndSession();
    sCutByParent = TRUE;
}

extern "C" int MiiTransfer_GetState(void) {
    return sState;
}

extern "C" void MiiTransfer_SetState(int state) {
    // The names of the 28 states, unused: the ROM keeps their initialiser right after the address of MiiTransfer_UpdateSession.
    const char stateNames[28][20] = {
        "Off",
        "Scanning",
        "Connecting",
        "Cutting",
        "WaitCut",
        "DispConnect",
        "DispCut",
        "CuttingForCommErr",
        "DispCommErr",
        "Idle",
        "Recv",
        "DispRecv",
        "DispRecvComb",
        "DispRecvDBFull",
        "DispRecvErr",
        "WaitAftRecv",
        "StbSend",
        "DispSend",
        "Send",
        "DispSendRtn",
        "DispSendDBFull",
        "DispSendDBFullSp",
        "DispSendErr",
        "DispSendErrSp",
        "DispSendErrTmp",
        "WaitAftSend",
        "CuttingForSaveErr",
        "DispSaveErr",
    };
    if (state == sState) {
        return;
    }
    sState = state;
    sStateFrames = 0;
    switch (state) {
    case 10:
        sReceiveReported = TRUE;
        break;
    case 11:
        sSendReported = TRUE;
        break;
    }
}

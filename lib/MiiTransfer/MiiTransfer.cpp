// The Mii transfer of wireless play (overlay 109): the protocol by which this DS, as a child, finds parents, connects
// to one and sends Miis to it or takes them from it, on the second copy of WH (wh.cpp) and the Mii records of
// MiiTransferList.cpp. Overlay 109 calls it.
//
// Two layers, both updated once a frame: a session (sSessionState: start WH, scan, connect, exchange packets, end),
// which sends one packet at a time, numbered by sSeq, and takes the parent's answers; and over it a protocol of
// messages (sMsgState: 0x10 a Mii, 0x20 the answer to one, 0x30 to 0x50, 0xD0 idle, 0xE0 cut, 0xF0 the user's
// name) and the state that overlay 109 sees (sState).
//
// The file name is descriptive, not the original. The functions keep their address names: no name of this code
// survives in the ROM.

#include "MiiTransfer.h"
#include "wh.h"

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

typedef void (*ResultCallback)(int result);
typedef void (*SendCallback)(int index, int result);

extern "C" {

BOOL func_020776e4(void);
int func_02077700(void);
void func_02077710(SendCallback callback);
void func_02077720(ResultCallback callback);
void func_02077730(void);
void func_02077758(void);
void func_0207777c(void *arg, MiiRecord *records, u32 count);
void func_020777a4(void);
void func_020777b8(void);
void func_020777f4(void);
void func_02077804(BOOL flag);
void func_02077814(void);
void func_02077858(void);
void func_02077b0c(void);
void func_02077bac(void);
void func_02077c70(void);
void func_02077cf4(void);
BOOL func_02077d08(const void *data, int size);
BOOL func_02077d90(void *dest);
void func_02077ddc(int index);
void func_02077e10(WMBssDesc *bssDesc);
void func_02077f48(u16 aid, u16 *data, u16 size);
void func_02077fe0(BOOL result);
BOOL func_0207800c(int index);
int func_02078040(int index);
WMBssDesc *func_02078064(int index);
int func_0207808c(void);
u8 func_0207809c(u16 index);
u16 *func_020780b4(u16 index);
void func_020780cc(void);
void func_020780d8(void);
void func_02078114(void);
void func_02078588(void);
BOOL func_02078800(void);
void func_02078824(int type, int arg, const MiiRecord *record);
int func_020788bc(int type);
BOOL func_020788e8(void);
BOOL func_02078908(u16 index);
BOOL func_0207895c(void);
BOOL func_02078988(void);
BOOL func_020789b0(void);
void func_020789dc(void);
void func_02078d04(void);
void func_02078d48(void);
void func_02078da4(int index);
void func_02078db8(u16 index);
void func_02078e58(u16 index);
void func_02078e70(u16 index);
void func_02078e88(const MiiCreateId *createId);
void func_02078ebc(u16 index);
void func_02078ed4(void);
void func_02078ee8(void);
int func_02078f04(void);
void func_02078f14(int state);

} // extern "C"

// 28 names of states (of a screen that shows the transfer, by the names), which nothing in the ROM refers to. In the
// ROM they follow the six bytes of WH_StartScan's address in func_02077858 with no padding, at 0x020B7BEA: as two
// objects of their own here, the link aligns them to four bytes, so this file's .rodata is not placed yet.
extern const char data_020b7bea[28][20] = {
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

static u8 sSeq;
static BOOL sSending;
static int sMsgState;
static SendCallback sSendCallback;
static BOOL data_02145384;
static int sParentCount;
static ResultCallback sResultCallback;
static int sSessionState;
static int data_021453c8;
static BOOL data_021453dc;
static BOOL data_021453d8;
static int data_021453d4;
static u32 sStateFrames;
static int data_021453cc;
static int data_021453a0;
static int sState;
static BOOL data_021453c0;
static BOOL data_021453bc;
static int sMiiIndex;
static void *data_021453b4;
static BOOL sSendPending;
static BOOL data_021453ac;
static int sParentIndex;
static int data_021453a4;
static OSMutex sMutex;
static u32 sParentAges[PARENT_MAX];
static u32 sParentTimers[PARENT_MAX];
static Message sRecvMsg;
static Message sSendMsg;
static Packet sSendPacket;
static Packet sRecvBuffer;
static Packet sRecvPacket;
static Parent sParents[PARENT_MAX];

extern "C" BOOL func_020776e4(void) {
    return sSessionState == 4;
}

extern "C" int func_02077700(void) {
    return sSessionState;
}

extern "C" void func_02077710(SendCallback callback) {
    sSendCallback = callback;
}

extern "C" void func_02077720(ResultCallback callback) {
    sResultCallback = callback;
}

extern "C" void func_02077730(void) {
    if (sSendCallback != NULL) {
        sSendCallback(data_021453d4, data_021453a4);
    }
}

extern "C" void func_02077758(void) {
    if (sResultCallback != NULL) {
        sResultCallback(data_021453c8);
    }
}

extern "C" void func_0207777c(void *arg, MiiRecord *records, u32 count) {
    data_021453b4 = arg;
    func_02079dfc(records, count);
    func_02077814();
    func_020780cc();
}

extern "C" void func_020777a4(void) {
    func_02077b0c();
    func_02078114();
    func_020789dc();
}

extern "C" void func_020777b8(void) {
    func_02078588();
    func_02077bac();
    if (sState != 0) {
        int state = func_02077700();
        if (state == 0 || state == 6) {
            data_021453d8 = TRUE;
        }
    }
}

extern "C" void func_020777f4(void) {
    func_020777b8();
    func_020777a4();
}

extern "C" void func_02077804(BOOL flag) {
    data_021453ac = flag;
}

extern "C" void func_02077814(void) {
    sSessionState = 0;
    OS_InitMutex(&sMutex);
    WH_SetGgid(0x412);
    sRecvBuffer.unk1 = 0;
    sSendPacket.unk1 = 0;
    sRecvPacket.unk1 = 0;
}

// The session's update. DOES NOT MATCH (0x2B0 bytes against 0x2B4): in the ROM its two loops are not rotated (a
// branch to the test at the bottom, as -O2 compiles them, where -O4 tests first and loops at the bottom, or drops the
// test for the constant bound), the loop counter, the 0 passed to WH_StartScan and the length of the parent's user
// game info share r4, and the copy of the user game info into the stack buffer is kept although nothing reads the
// buffer, where this compiler removes it. Not reproduced with volatile, register, other loop forms, inline helpers, a
// local structure holding both arrays, or #pragma optimization_level (2 gives the loops but not the rest). The buffer
// is volatile here only so that the copy is kept, which brings it closest (without it, 0x288 bytes).
extern "C" void func_02077858(void) {
    volatile u8 userGameInfo[0x16];

    switch (sSessionState) {
    case 0:
        break;
    case 1:
        switch (WH_GetSystemState()) {
        case WH_SYSSTATE_IDLE: {
            u8 macAddr[6] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
            WH_TurnOffPictoCatch();
            WH_StartScan(func_02077e10, macAddr, 0);
            sSessionState = 2;
            break;
        }
        case WH_SYSSTATE_BUSY:
            break;
        }
        break;
    case 2:
        for (int i = 0; i < PARENT_MAX; i++) {
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
            WH_SetReceiver(func_02077f48);
            sRecvBuffer.valid = FALSE;
            sSendPacket.valid = FALSE;
            sRecvPacket.valid = FALSE;
            break;
        case WH_SYSSTATE_SCANNING:
            WH_EndScan();
            break;
        case WH_SYSSTATE_BUSY:
            break;
        case WH_SYSSTATE_CONNECTED: {
            int length = func_0207809c(sParentIndex);
            u16 *info = func_020780b4(sParentIndex);
            for (int i = 0; i < length; i++) {
                userGameInfo[i] = *info++;
            }
            sSessionState = 4;
            break;
        }
        case WH_SYSSTATE_CONNECT_FAIL:
            sSessionState = 5;
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
        case WH_SYSSTATE_STOP:
            sSessionState = 0;
            break;
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

extern "C" void func_02077b0c(void) {
    OS_LockMutex(&sMutex);
    func_02077858();
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

extern "C" void func_02077bac(void) {
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
        if (WH_SendData(&sSendPacket, sSendPacket.size, func_02077fe0)) {
            sSending = TRUE;
        }
    }
    OS_UnlockMutex(&sMutex);
}

extern "C" void func_02077c70(void) {
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

extern "C" void func_02077cf4(void) {
    sSessionState = 5;
}

extern "C" BOOL func_02077d08(const void *data, int size) {
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

extern "C" BOOL func_02077d90(void *dest) {
    if (sSessionState != 4) {
        return FALSE;
    }
    if (sRecvPacket.valid) {
        MI_CpuCopy8(&sRecvPacket.msg, dest, sRecvPacket.size - 2);
    }
    return sRecvPacket.valid;
}

extern "C" void func_02077ddc(int index) {
    if (index < 0 || index >= sParentCount) {
        return;
    }
    if (sSessionState == 2) {
        sParentIndex = index;
        sSessionState = 3;
    }
}

extern "C" void func_02077e10(WMBssDesc *bssDesc) {
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

// WH's receiver: takes a packet from the parent. DOES NOT MATCH: the original loads sSeq into r0 and its low bit into
// r1, this gives the other way round (two instructions); the rest is the same. Not reproduced with other forms of the
// test (!= 0, % 2, a local copy, an inline helper, the packet's own type).
extern "C" void func_02077f48(u16 aid, u16 *data, u16 size) {
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

extern "C" void func_02077fe0(BOOL result) {
    OS_LockMutex(&sMutex);
    sSending = FALSE;
    OS_UnlockMutex(&sMutex);
}

extern "C" BOOL func_0207800c(int index) {
    if (index >= sParentCount) {
        return FALSE;
    }
    return sParentTimers[index] != 0;
}

extern "C" int func_02078040(int index) {
    if (index >= sParentCount) {
        return -1;
    }
    return sParentAges[index];
}

extern "C" WMBssDesc *func_02078064(int index) {
    if (index >= sParentCount) {
        return NULL;
    }
    return &sParents[index].bssDesc;
}

extern "C" int func_0207808c(void) {
    return sParentCount;
}

extern "C" u8 func_0207809c(u16 index) {
    WMBssDesc *bssDesc = func_02078064(index);
    if (bssDesc != NULL) {
        return bssDesc->gameInfo.userGameInfoLength;
    }
    return 0;
}

extern "C" u16 *func_020780b4(u16 index) {
    WMBssDesc *bssDesc = func_02078064(index);
    if (bssDesc != NULL) {
        return bssDesc->gameInfo.userGameInfo;
    }
    return NULL;
}

extern "C" void func_020780cc(void) {
    func_020780d8();
}

extern "C" void func_020780d8(void) {
    sMsgState = 0xF0;
    sMiiIndex = -1;
    data_021453a0 = 0;
    sSendPending = FALSE;
    sSendMsg.type = 0xD0;
    sSendMsg.arg = 0;
    data_021453bc = FALSE;
    data_021453c0 = FALSE;
}

extern "C" void func_02078114(void) {
    if (!func_020776e4()) {
        return;
    }
    if (!func_02077d90(&sRecvMsg)) {
        return;
    }
    if (func_02078800()) {
        return;
    }
    switch (sMsgState) {
    case 0x11: {
        if (sRecvMsg.type != 0x20) {
            return;
        }
        MiiRecord *record = func_02079ea4(sMiiIndex);
        switch (sRecvMsg.arg) {
        case 0:
            if (func_02073264(&record->createId)) {
                func_02073394(&record->createId);
                data_021453a0 = 3;
            } else {
                func_02078824(0x30, 0, NULL);
                func_02078ed4();
            }
            break;
        case 1:
            func_02078824(0x30, 0, NULL);
            if (func_02073264(&record->createId)) {
                data_021453c8 = -4;
            } else {
                data_021453c8 = -5;
            }
            break;
        case 2:
            func_02078824(0x30, 0, NULL);
            func_02078ed4();
            break;
        case 5:
            func_02078824(0x30, 0, NULL);
            data_021453c8 = 2;
            break;
        default:
            func_02078824(0x30, 0, NULL);
            if (func_02073264(&record->createId)) {
                data_021453c8 = -2;
            } else {
                data_021453c8 = -3;
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
        if (data_021453bc) {
            func_02078824(0x50, 0, NULL);
            data_021453bc = FALSE;
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
                if (data_021453ac == FALSE && func_02073264(&sRecvMsg.record.createId)) {
                    data_021453a4 = -2;
                    func_02078824(0x20, 4, NULL);
                } else {
                    sRecvMsg.record.unk20[0] |= 4;
                    sRecvMsg.record.unk0[0] &= ~1;
                    sMiiIndex = func_02079f28(&sRecvMsg.record.createId);
                    if (sMiiIndex >= 0) {
                        if (func_02073264(&sRecvMsg.record.createId)) {
                            func_02078824(0x20, 5, NULL);
                            func_02078ebc(sMiiIndex);
                        } else if (func_020732a0(data_021453b4, &sRecvMsg.record.createId)) {
                            func_02078824(0x20, 2, NULL);
                            func_02078e70(sMiiIndex);
                        } else {
                            func_02079ed4(sMiiIndex, &sRecvMsg.record);
                            data_021453a0 = 2;
                            func_02078e70(sMiiIndex);
                        }
                    } else {
                        sMiiIndex = func_02079e5c();
                        if (sMiiIndex >= 0) {
                            func_02079ed4(sMiiIndex, &sRecvMsg.record);
                            data_021453a0 = 1;
                            func_02078e58(sMiiIndex);
                        } else {
                            func_02078e88(&sRecvMsg.record.createId);
                            func_02078824(0x20, 1, NULL);
                        }
                    }
                }
            } else {
                data_021453a4 = -3;
                func_02078824(0x20, 4, NULL);
            }
            sMsgState = 0x10;
            data_021453c0 = FALSE;
            return;
        }
        if (sRecvMsg.type == 0xE0) {
            func_02078ee8();
            sMsgState = 0xE0;
        }
        return;
    case 0x21:
        if (sRecvMsg.type != 0x30 || sRecvMsg.arg != 0) {
            return;
        }
        if (data_021453c0) {
            func_02078824(0x40, 0, NULL);
            data_021453c0 = FALSE;
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

extern "C" void func_02078588(void) {
    if (!func_020776e4()) {
        return;
    }
    if (!func_02078800() && sMsgState != 0xF0) {
        return;
    }
    switch (sMsgState) {
    case 0x20:
        if (data_021453a0) {
            func_02078ed4();
            func_02078824(0x30, 0, NULL);
            data_021453a0 = 0;
        }
        break;
    case 0x40:
        if (data_021453bc) {
            func_02078824(0x50, 0, NULL);
            data_021453bc = FALSE;
        }
        break;
    case 0x10:
        if (data_021453a0 == 1 || data_021453a0 == 2) {
            if (data_021453a0 == 1) {
                func_02078824(0x20, 0, NULL);
            } else {
                func_02078824(0x20, 2, NULL);
            }
            data_021453a0 = 0;
        }
        break;
    case 0x30:
        if (data_021453c0) {
            func_02078824(0x40, 0, NULL);
            data_021453c0 = FALSE;
        }
        break;
    case 0xF0:
        if (!sSendPending) {
            func_02078824(0xF0, 0, NULL);
        }
        break;
    }
    if (!sSendPending && (sMsgState == 0x50 || sMsgState == 0xD0)) {
        func_02078824(0xD0, 0, NULL);
    }
    if (!sSendPending) {
        return;
    }
    if (!func_02077d08(&sSendMsg, func_020788bc(sSendMsg.type))) {
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
extern "C" BOOL func_02078800(void) {
    return !(sMsgState & 1) && sMsgState != 0xF0;
}

// Puts a message in the send buffer
extern "C" void func_02078824(int type, int arg, const MiiRecord *record) {
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
extern "C" int func_020788bc(int type) {
    switch (type) {
    case 0x10:
        return 0x4C;
    case 0xF0:
        return 0x16;
    default:
        return 2;
    }
}

extern "C" BOOL func_020788e8(void) {
    return sMsgState == 0xD0 || sMsgState == 0x50;
}

// Sends the Mii of slot index
extern "C" BOOL func_02078908(u16 index) {
    if (!func_020788e8()) {
        return FALSE;
    }
    MiiRecord *record = func_02079ea4(index);
    if (record == NULL) {
        return FALSE;
    }
    func_02078824(0x10, 0, record);
    sMiiIndex = index;
    data_021453bc = FALSE;
    return TRUE;
}

extern "C" BOOL func_0207895c(void) {
    if (!func_020788e8()) {
        return FALSE;
    }
    func_02078824(0xE0, 0, NULL);
    return TRUE;
}

extern "C" BOOL func_02078988(void) {
    switch (sMsgState) {
    case 0x11:
    case 0x20:
        return TRUE;
    }
    return FALSE;
}

extern "C" BOOL func_020789b0(void) {
    switch (sMsgState) {
    case 0x10:
    case 0x21:
    case 0x30:
        return TRUE;
    }
    return FALSE;
}

extern "C" void func_020789dc(void) {
    sStateFrames++;
    switch (sState) {
    case 0:
        break;
    case 1:
        if (data_021453d8) {
            func_02078f14(5);
            func_02077cf4();
        }
        break;
    case 2:
        if (data_021453d8) {
            func_02078f14(5);
            func_02077cf4();
        } else {
            switch (func_02077700()) {
            case 4:
                func_02078f14(6);
                break;
            case 6:
                func_02078f14(5);
                func_02077cf4();
                break;
            }
        }
        break;
    case 3:
        if (func_02077700() == 0) {
            func_02078f14(0);
        }
        break;
    case 4:
        if (data_021453d8 || sStateFrames > 300) {
            func_02078f14(3);
            func_02077cf4();
        } else {
            func_0207895c();
        }
        break;
    case 5:
        if (func_02077700() == 0) {
            func_02078f14(0);
        }
        break;
    case 6:
        if (data_021453dc) {
            func_02078f14(3);
        } else if (data_021453a4) {
            func_02077730();
            func_02078f14(10);
        } else if (data_021453d8) {
            func_02078f14(5);
            func_02077cf4();
        } else if (data_02145384 && func_0207895c()) {
            func_02078f14(4);
            data_02145384 = FALSE;
        }
        break;
    case 7:
        if (data_021453d8) {
            func_02078f14(5);
        }
        break;
    case 10:
        data_021453d4 = -1;
        data_021453a4 = 0;
        if (data_021453d8) {
            func_02078f14(5);
            func_02077cf4();
        } else if (!func_020789b0()) {
            if (data_021453cc >= 0) {
                if (func_02078908(data_021453cc)) {
                    func_02078f14(8);
                } else {
                    func_02078f14(9);
                }
            } else {
                func_02078f14(6);
            }
        }
        break;
    case 9:
        if (data_021453dc) {
            func_02078f14(3);
        } else if (data_021453d8) {
            func_02078f14(5);
            func_02077cf4();
        } else if (data_021453a4) {
            func_02077730();
            func_02078f14(10);
        } else if (func_02078908(data_021453cc)) {
            func_02078f14(8);
        }
        break;
    case 8:
        if (data_021453d8) {
            func_02078f14(5);
            func_02077cf4();
        } else if (data_021453c8) {
            func_02077758();
            func_02078f14(11);
        }
        break;
    case 11:
        data_021453cc = -1;
        data_021453c8 = 0;
        if (data_021453d8) {
            func_02078f14(5);
            func_02077cf4();
        } else if (!func_02078988()) {
            func_02078f14(6);
        }
        break;
    }
}

extern "C" void func_02078d04(void) {
    func_02078f14(1);
    func_02077c70();
    func_020780d8();
    data_021453d4 = -1;
    data_021453cc = -1;
    data_021453a4 = 0;
    data_021453c8 = 0;
    data_021453d8 = FALSE;
    data_021453dc = FALSE;
    data_02145384 = FALSE;
}

extern "C" void func_02078d48(void) {
    switch (sState) {
    case 1:
    case 2:
        func_02078f14(3);
        func_02077cf4();
        break;
    case 6:
        if (func_0207895c()) {
            func_02078f14(4);
        }
        data_02145384 = TRUE;
        break;
    }
}

extern "C" void func_02078da4(int index) {
    func_02077ddc(index);
    func_02078f14(2);
}

extern "C" void func_02078db8(u16 index) {
    MiiRecord *record = func_02079ea4(index);
    if (record == NULL || func_02073284(&record->createId)) {
        data_021453c8 = -3;
        func_02077758();
        func_02078f14(11);
        return;
    }
    if (func_0207331c(&record->createId)) {
        data_021453c8 = -1;
        func_02077758();
        func_02078f14(11);
        return;
    }
    data_021453cc = index;
    if (func_02078908(index)) {
        func_02078f14(8);
    } else {
        func_02078f14(9);
    }
}

extern "C" void func_02078e58(u16 index) {
    data_021453a4 = 1;
    data_021453d4 = index;
}

extern "C" void func_02078e70(u16 index) {
    data_021453a4 = 3;
    data_021453d4 = index;
}

extern "C" void func_02078e88(const MiiCreateId *createId) {
    if (func_02073264(createId)) {
        data_021453a4 = -4;
    } else {
        data_021453a4 = -5;
    }
    data_021453d4 = -1;
}

extern "C" void func_02078ebc(u16 index) {
    data_021453a4 = 2;
    data_021453d4 = index;
}

extern "C" void func_02078ed4(void) {
    data_021453c8 = 1;
}

extern "C" void func_02078ee8(void) {
    func_02077cf4();
    data_021453dc = TRUE;
}

extern "C" int func_02078f04(void) {
    return sState;
}

extern "C" void func_02078f14(int state) {
    if (state == sState) {
        return;
    }
    sState = state;
    sStateFrames = 0;
    switch (state) {
    case 10:
        data_021453c0 = TRUE;
        break;
    case 11:
        data_021453bc = TRUE;
        break;
    }
}

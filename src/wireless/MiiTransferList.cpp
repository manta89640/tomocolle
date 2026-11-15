// The Miis that the Mii transfer of wireless play (MiiTransfer.cpp) sends from and receives into: an array of Mii
// records that its caller owns and hands over, which this file looks through by their create IDs.
//
// The file name is descriptive, not the original. The functions keep their address names: no name of this code
// survives in the ROM.

#include "MiiTransfer.h"

static u32 sCount;
static MiiRecord *sRecords;

extern "C" void func_02079dfc(MiiRecord *records, u32 count) {
    sCount = count;
    sRecords = records;
}

// Whether slot index holds a Mii
extern "C" BOOL func_02079e10(int index) {
    if (index >= 0 && index < sCount) {
        return !func_02073284(&sRecords[index].createId);
    }
    return FALSE;
}

// The first empty slot, or -1
extern "C" int func_02079e5c(void) {
    for (u32 i = 0; i < sCount; i++) {
        if (!func_02079e10(i)) {
            return i;
        }
    }
    return -1;
}

extern "C" MiiRecord *func_02079ea4(int index) {
    if (!func_02079e10(index)) {
        return NULL;
    }
    return &sRecords[index];
}

// Stores a Mii into slot index
extern "C" void func_02079ed4(int index, const MiiRecord *record) {
    if (index < 0 || index >= sCount) {
        return;
    }
    MI_CpuCopy16(record, &sRecords[index], sizeof(MiiRecord));
}

extern "C" MiiCreateId *func_02079f0c(int index) {
    return &sRecords[index].createId;
}

// The slot of the Mii with this create ID, or -1
extern "C" int func_02079f28(const MiiCreateId *createId) {
    for (u32 i = 0; i < sCount; i++) {
        MiiCreateId *id = func_02079f0c(i);
        if (!func_02073284(id) && !func_0207331c(id) && func_02073330(id, createId)) {
            return i;
        }
    }
    return -1;
}

#ifndef WIRELESS_MIITRANSFER_H
#define WIRELESS_MIITRANSFER_H

// The Mii transfer of wireless play (overlay 109): MiiTransfer.cpp, the protocol, on the second copy of WH (wh.cpp),
// and MiiTransferList.cpp, the Miis it sends from and receives into. The names of the types and files are
// descriptive, not the original: no name of this code survives in the ROM.

#include <nitro.h>

// A Mii's create ID: 8 bytes, which the main program's functions around 0x02073264 test and compare
typedef struct MiiCreateId {
    u8 data[8];
} MiiCreateId;

// A Mii as it is sent: 0x4A bytes, copied by halfwords, with its create ID at 0x18
typedef struct MiiRecord {
    u16 unk0[0xC];
    MiiCreateId createId;
    u16 unk20[0x15];
} MiiRecord;

extern "C" {

// The main program's functions on create IDs, not decompiled yet
BOOL func_02073264(const MiiCreateId *id);              // bit 7 of the first byte is clear
BOOL func_02073284(const MiiCreateId *id);              // all zero
BOOL func_020732a0(const void *a, const MiiCreateId *b); // TRUE if equal in the low nibble of byte 2 and bytes 3 to 7
BOOL func_0207331c(const MiiCreateId *id);              // bit 5 of the first byte is set
BOOL func_02073330(const MiiCreateId *a, const MiiCreateId *b); // equal, and neither all zero
void func_02073394(MiiCreateId *id);                     // clears it

// MiiTransferList.cpp
void func_02079dfc(MiiRecord *records, u32 count);
BOOL func_02079e10(int index);
int func_02079e5c(void);
MiiRecord *func_02079ea4(int index);
void func_02079ed4(int index, const MiiRecord *record);
MiiCreateId *func_02079f0c(int index);
int func_02079f28(const MiiCreateId *createId);

} // extern "C"

#endif // WIRELESS_MIITRANSFER_H

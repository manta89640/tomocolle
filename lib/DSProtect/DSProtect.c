// DS Protect, Nintendo's anti-piracy library, as the game links it (docs/binary.md, "DS Protect"). The names are the
// project's own, for what the code does. Part of each check is kept encrypted in the ROM and decrypted only while it
// runs: see DSPROTECT_BEGIN below.
#include <nitro.h>

// RC4's state: the two indices and the permutation
typedef struct DSProtectRC4 {
    u32 i;
    u32 j;
    u8 state[256];
} DSProtectRC4;

void DSProtect_SyncCaches(BOOL decrypted);
void DSProtect_DecryptRange(u32 *start);
void DSProtect_EncryptRange(u32 *end);
void DSProtect_RC4Init(DSProtectRC4 *rc4, const u8 *key, u32 keyLength);
u8 DSProtect_RC4NextByte(DSProtectRC4 *rc4);
int DSProtect_InitByteTable(u8 *table);
int DSProtect_CryptInstructions(DSProtectRC4 *rc4, const u8 *src, u8 *dst, u32 size);
int DSProtect_DecryptInstructions(DSProtectRC4 *rc4, const u8 *src, u8 *dst, u32 size);
int DSProtect_EncryptWithKey(const u8 *key, const void *src, void *dst, u32 size);
int DSProtect_DecryptWithKey(const u8 *key, const void *src, void *dst, u32 size);

// Makes code just decrypted (or encrypted again) what the CPU runs: the data cache written back, the instruction cache
// off while decrypted code runs and on again after
void DSProtect_SyncCaches(BOOL decrypted) {
    if (decrypted) {
        DC_StoreAll();
        IC_Disable();
    } else {
        IC_Enable();
        DC_StoreAll();
    }
}

// Decrypts the range that starts at "start" and ends at the next copy of the word before it (the marker), with a key of
// 16 bytes made from the marker: its bytes four times over, the first and the last inverted
void DSProtect_DecryptRange(u32 *start) {
    u8 key[16];
    int i;
    u32 marker = start[-1];
    u32 n;
    for (i = 0; i < 16; i++) {
        key[i] = marker >> ((i % 4) * 8);
        if (i % 15 == 0) {
            key[i] ^= 0xFF;
        }
    }
    n = 0;
    while (marker != start[n]) {
        n++;
    }
    if (n != 0) {
        DSProtect_DecryptWithKey(key, start, start, n * 4);
    }
    DSProtect_SyncCaches(TRUE);
}

// Encrypts again the range that ends at the marker after "end", from the marker before it
void DSProtect_EncryptRange(u32 *end) {
    u8 key[16];
    int i;
    u32 marker = end[1];
    u32 n;
    for (i = 0; i < 16; i++) {
        key[i] = marker >> ((i % 4) * 8);
        if (i % 15 == 0) {
            key[i] ^= 0xFF;
        }
    }
    while (*end != marker) {
        end--;
    }
    n = 0;
    while (marker != end[n + 1]) {
        n++;
    }
    if (n != 0) {
        DSProtect_EncryptWithKey(key, end + 1, end + 1, n * 4);
    }
    DSProtect_SyncCaches(FALSE);
}

// RC4's key schedule, but with the permutation walked from its end. Not matched: the instructions are the original's
// but for the registers of the index, the swap's two values and a constant, and the order of the last two.
void DSProtect_RC4Init(DSProtectRC4 *rc4, const u8 *key, u32 keyLength) {
    u32 k = 0;
    u32 j = 0;
    int i;
    rc4->i = 0;
    rc4->j = 0;
    for (i = 0; i < 256; i++) {
        rc4->state[i] = i;
    }
    for (i = 255; i >= 0; i--) {
        u8 t = rc4->state[i];
        j = (j + key[k] + t) & 0xFF;
        k++;
        if (k >= keyLength) {
            k = 0;
        }
        rc4->state[i] = rc4->state[j];
        rc4->state[j] = t;
    }
}

u8 DSProtect_RC4NextByte(DSProtectRC4 *rc4) {
    u32 i = (rc4->i + 1) & 0xFF;
    u8 a = rc4->state[i];
    u32 j = (a + rc4->j) & 0xFF;
    u8 b = rc4->state[j];
    rc4->i = i;
    rc4->j = j;
    rc4->state[j] = a;
    rc4->state[i] = b;
    return rc4->state[(a + b) & 0xFF];
}

// A table that flips a byte's low bit
int DSProtect_InitByteTable(u8 *table) {
    int i;
    for (i = 0; i < 256; i++) {
        table[i] = (u8)i ^ 1;
    }
    return 0;
}

// The cipher, its own inverse: of each instruction the first two bytes are XORed with RC4's stream, the third has its
// low bit flipped and the fourth (the condition and the instruction's class) is kept, so that encrypted code still
// disassembles as plausible instructions. -1 when the size is not a whole number of instructions.
int DSProtect_CryptInstructions(DSProtectRC4 *rc4, const u8 *src, u8 *dst, u32 size) {
    u8 table[256];
    u32 i;
    if (size & 3) {
        return -1;
    }
    DSProtect_InitByteTable(table);
    for (i = 0; i < size; i += 4) {
        dst[i] = src[i] ^ DSProtect_RC4NextByte(rc4);
        dst[i + 1] = src[i + 1] ^ DSProtect_RC4NextByte(rc4);
        dst[i + 2] = table[src[i + 2]];
        dst[i + 3] = src[i + 3];
    }
    return 0;
}

int DSProtect_DecryptInstructions(DSProtectRC4 *rc4, const u8 *src, u8 *dst, u32 size) {
    return DSProtect_CryptInstructions(rc4, src, dst, size);
}

int DSProtect_EncryptWithKey(const u8 *key, const void *src, void *dst, u32 size) {
    DSProtectRC4 rc4;
    DSProtect_RC4Init(&rc4, key, 16);
    return -(DSProtect_CryptInstructions(&rc4, src, dst, size) == -1);
}

int DSProtect_DecryptWithKey(const u8 *key, const void *src, void *dst, u32 size) {
    DSProtectRC4 rc4;
    DSProtect_RC4Init(&rc4, key, 16);
    return -(DSProtect_DecryptInstructions(&rc4, src, dst, size) == -1);
}

// The encrypted parts. Each is a range of a function between two copies of a marker word, which DSProtect_DecryptRange
// decrypts in place before it runs and DSProtect_EncryptRange encrypts again after it; the ROM holds them encrypted
// (docs/binary.md, "DS Protect").
#define DSPROTECT_BEGIN(marker, label)                                                                                 \
    asm {                                                                                                              \
        stmfd sp!, {r0-r3};                                                                                            \
        mov r1, #12;                                                                                                   \
        add r0, r1, pc;                                                                                                \
        bl DSProtect_DecryptRange;                                                                                     \
        ldmfd sp!, {r0-r3};                                                                                            \
        b label;                                                                                                       \
        dcd marker;                                                                                                    \
    label:                                                                                                             \
    }
#define DSPROTECT_END(marker, label)                                                                                   \
    asm {                                                                                                              \
        b label;                                                                                                       \
        dcd marker;                                                                                                    \
    label:                                                                                                             \
        stmfd sp!, {r0};                                                                                               \
        mov r0, pc;                                                                                                    \
        sub r0, r0, #20;                                                                                               \
        bl DSProtect_EncryptRange;                                                                                     \
        ldmfd sp!, {r0};                                                                                               \
    }

// A MAC address, inverted: 00:09:BF:00:00:31
static const u8 sMacAddress[6] = {0xFF, 0xF6, 0x40, 0xFF, 0xFF, 0xCE};

// TRUE when the DS's MAC address is sMacAddress's and the owner's birthday is January 1 and the nickname empty, or when
// the MAC address is all zeros. "i" holds the result at the end, as the original's register does.
BOOL DSProtect_CheckMacAddress(void) {
    u8 mac[6];
    OSOwnerInfo info;
    int i = 0;
    OS_GetMacAddress(mac);
    DSPROTECT_BEGIN(0xEB003530, begin)
    for (i = 0; i < 6; i++) {
        if (sMacAddress[i] != (mac[i] ^ 0xFF)) {
            break;
        }
    }
    DSPROTECT_END(0xEB003530, end)
    OS_GetOwnerInfo(&info);
    DSPROTECT_BEGIN(0xEB003089, begin2)
    if (i == 6 && info.birthday.month == 1 && info.birthday.day == 1 && info.nickNameLength == 0) {
        i = TRUE;
    } else {
        for (i = 0; i < 6; i++) {
            if (mac[i] != 0) {
                i = FALSE;
                goto done;
            }
        }
        i = TRUE;
    }
done:
    DSPROTECT_END(0xEB003089, end2)
    return i;
}

// Reads "size" bytes of the card's ROM from "src" with command 0xB7 by the card's registers, a block of 512 bytes at a
// time, under the card's lock. "base" is 1 shifted to the registers' address: the original computes the address so,
// which a constant expression does not give.
void DSProtect_ReadRom(void *dst, u32 src, u32 size) {
    u32 base = 1;
    u8 *reg;
    u16 lockId;
    u16 exmemcnt;
    u8 backup[8];
    vu8 *command;
    int offset;
    int i;
    u32 control;
    base <<= 26;
    reg = (u8 *)base;
    lockId = (u16)OS_GetLockID();
    CARD_LockRom(lockId);
    DSPROTECT_BEGIN(0xEB005FDF, begin)
    exmemcnt = *(vu16 *)(reg + 0x204);
    command = (vu8 *)(reg + 0x1A8);
    *(vu16 *)(reg + 0x204) &= ~0x800;
    control = (*(u32 *)(HW_ROM_HEADER_BUF + 0x60) & ~0x07000000) | 0xA1000000;
    offset = -(int)(src & 0x1FF);
    while (*(vu32 *)(reg + 0x1A4) & 0x80000000) {
    }
    *(vu8 *)(reg + 0x1A1) = 0x80;
    for (i = 0; i < 8; i++) {
        backup[i] = command[i];
    }
    src += offset;
    while (offset < (int)size) {
        command[0] = 0xB7;
        command[1] = src >> 24;
        command[2] = src >> 16;
        command[3] = src >> 8;
        command[4] = src;
        command[5] = 0;
        command[6] = 0;
        command[7] = 0;
        *(vu32 *)(reg + 0x1A4) = control;
        do {
            if (*(vu32 *)(reg + 0x1A4) & 0x800000) {
                u32 data = *(vu32 *)(reg + 0x100010);
                if (offset >= 0 && offset < (int)size) {
                    *(u32 *)((u8 *)dst + offset) = data;
                }
                offset += 4;
            }
        } while (*(vu32 *)(reg + 0x1A4) & 0x80000000);
        src += 0x200;
    }
    for (i = 0; i < 8; i++) {
        command[i] = backup[i];
    }
    *(vu16 *)(reg + 0x204) = exmemcnt;
    DSPROTECT_END(0xEB005FDF, end)
    CARD_UnlockRom(lockId);
    OS_ReleaseLockID(lockId);
}

// A CRC-32 (polynomial 0xEDB88320) but for the test of the low bit, which is inverted. Not matched: the original XORs
// the polynomial in four immediates in the loop where this source keeps it in a register.
u32 DSProtect_CalcChecksum(const u8 *data, u32 size) {
    u32 sum;
    int i;
    DSPROTECT_BEGIN(0xEB000D2C, begin)
    sum = 0xFFFFFFFF;
    while (size-- != 0) {
        sum ^= *data++;
        for (i = 0; i < 8; i++) {
            if (sum & 1) {
                sum >>= 1;
            } else {
                sum = (sum >> 1) ^ 0xEDB88320;
            }
        }
    }
    sum = ~sum;
    DSPROTECT_END(0xEB000D2C, end)
    return sum;
}

// Reads six blocks of 512 bytes of the card's ROM, at 0, 0x200, 0x400, 0x8000, 0x8200 and 0x8400, and returns TRUE
// when a checksum of the first three differs from that of the block at 0x8000, or when the last three are equal.
// Inferred: a genuine card reads the first 32 KB as copies of what follows them, so this tells a copy from it.
// "one << 15" for the same reason as DSProtect_ReadRom's address.
BOOL DSProtect_CheckRom(void) {
    u32 sums[7];
    u8 buffer[512];
    BOOL result = FALSE;
    int i;
    u32 address = 0;
    u32 one = 1;
    for (i = 0; i < 6; i++) {
        DSProtect_ReadRom(buffer, address, 512);
        sums[i] = DSProtect_CalcChecksum(buffer, 512);
        if (i == 2) {
            address = one << 15;
        } else {
            address += 512;
        }
    }
    DSPROTECT_BEGIN(0xEB00350E, begin)
    for (i = 0; i < 3; i++) {
        if (sums[3] != sums[i]) {
            result = TRUE;
            goto done;
        }
    }
    if (sums[3] == sums[4] && sums[3] == sums[5]) {
        result = TRUE;
    }
done:
    for (i = 0; i < 512; i++) {
        buffer[i] = 0;
    }
    DSPROTECT_END(0xEB00350E, end)
    return result;
}

typedef BOOL (*DSProtectCheck)(void);

// The three entry points, which only main calls: each runs a list of checks (stored as their addresses plus 800),
// folds their results into a bit string, and returns TRUE and calls "callback" when any check found something.
// The list of DSProtect_RunNoCheck is empty.

BOOL DSProtect_RunRomCheck(void (*callback)(void)) {
    DSProtectCheck checks[32];
    u32 bits;
    int i;
    BOOL result;
    result = FALSE;
    bits = 0;
    i = 0;
    checks[0] = (DSProtectCheck)((u8 *)DSProtect_CheckRom + 800);
    checks[1] = NULL;
    for (i = 0; checks[i] != NULL; i++) {
        checks[i] = (DSProtectCheck)((u8 *)checks[i] - 800);
        bits = (bits + (checks[i]() ? 1 : 0)) << 1;
    }
    DSPROTECT_BEGIN(0xEB0048F8, begin)
    if ((int)bits >> 1) {
        result = TRUE;
    } else if (((int)bits >> 1) == 0) {
        result = FALSE;
    }
    if (callback != NULL && result) {
        callback();
    }
    DSPROTECT_END(0xEB0048F8, end)
    return result;
}

BOOL DSProtect_RunMacCheck(void (*callback)(void)) {
    DSProtectCheck checks[32];
    u32 bits;
    int i;
    BOOL result;
    result = FALSE;
    bits = 0;
    i = 0;
    checks[0] = (DSProtectCheck)((u8 *)DSProtect_CheckMacAddress + 800);
    checks[1] = NULL;
    for (i = 0; checks[i] != NULL; i++) {
        checks[i] = (DSProtectCheck)((u8 *)checks[i] - 800);
        bits = (bits + (checks[i]() ? 1 : 0)) << 1;
    }
    DSPROTECT_BEGIN(0xEB003481, begin)
    if ((int)bits >> 1) {
        result = TRUE;
    } else if (((int)bits >> 1) == 0) {
        result = FALSE;
    }
    if (callback != NULL && result) {
        callback();
    }
    DSPROTECT_END(0xEB003481, end)
    return result;
}

BOOL DSProtect_RunNoCheck(void (*callback)(void)) {
    DSProtectCheck checks[32];
    u32 bits;
    int i;
    BOOL result;
    result = FALSE;
    bits = 0;
    i = 0;
    checks[0] = NULL;
    for (i = 0; checks[i] != NULL; i++) {
        checks[i] = (DSProtectCheck)((u8 *)checks[i] - 800);
        bits = (bits + (checks[i]() ? 1 : 0)) << 1;
    }
    DSPROTECT_BEGIN(0xEB0012F0, begin)
    if ((int)bits >> 1) {
        result = TRUE;
    } else if (((int)bits >> 1) == 0) {
        result = FALSE;
    }
    if (callback != NULL && result) {
        callback();
    }
    DSPROTECT_END(0xEB0012F0, end)
    return result;
}

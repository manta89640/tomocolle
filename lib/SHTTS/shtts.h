#ifndef SHTTS_H
#define SHTTS_H

#include <nitro.h>

// Sharp's text-to-speech engine, SHTTS 1.2.0. None of its names survive in the ROM, so functions are named by
// address, and fields by offset until what they hold is clear.

typedef void (*ShttsCallback)(int samples, s16 *data, void *arg);

// Settings of a voice. The engine has two of these: the ones in use, and the ones a caller has set.
typedef struct ShttsParams {
    /* 0x00 */ u8 unk_00;
    /* 0x02 */ u16 unk_02;
    /* 0x04 */ u16 unk_04;
    /* 0x06 */ u16 unk_06;
    /* 0x08 */ u16 unk_08;
    /* 0x0A */ u16 unk_0A;
    /* 0x0C */ u16 unk_0C;
    /* 0x0E */ u16 unk_0E;
    /* 0x10 */ u32 unk_10;
    /* 0x14 */ u16 unk_14;
    /* 0x16 */ u16 unk_16;
    /* 0x18 */ u32 unk_18;
    /* 0x1C */ u32 unk_1C;
    /* 0x20 */ u32 unk_20;
    /* 0x24 */ u32 unk_24;
    /* 0x28 */ u16 unk_28;
    /* 0x2A */ u16 unk_2A;
    /* 0x2C */ u16 unk_2C;
} ShttsParams; // 0x30

// The engine's whole state, 0x14000 bytes that the caller provides
typedef struct Shtts {
    /* 0x00000 */ u32 unk_00000;
    /* 0x00004 */ u16 unk_00004;
    /* 0x00008 */ u32 unk_00008;
    /* 0x0000C */ u16 unk_0000C;
    /* 0x0000E */ u8 pad_0000E[0x1B21 - 0xE];
    /* 0x01B21 */ u8 unk_01B21;
    /* 0x01B22 */ u8 pad_01B22[0x8722 - 0x1B22];
    /* 0x08722 */ u16 unk_08722;
    /* 0x08724 */ u8 pad_08724[0x8906 - 0x8724];
    /* 0x08906 */ u16 unk_08906;
    /* 0x08908 */ u8 pad_08908[0x8CDC - 0x8908];
    /* 0x08CDC */ u16 unk_08CDC;
    /* 0x08CDE */ u16 unk_08CDE;
    /* 0x08CE0 */ u32 unk_08CE0;
    /* 0x08CE4 */ u16 unk_08CE4;
    /* 0x08CE6 */ u8 pad_08CE6[0xB6FC - 0x8CE6];
    /* 0x0B6FC */ u32 unk_0B6FC;
    /* 0x0B700 */ u8 pad_0B700[0xB71C - 0xB700];
    /* 0x0B71C */ ShttsCallback callback; // Receives the samples
    /* 0x0B720 */ u32 unk_0B720;
    /* 0x0B724 */ void *callbackArg;
    /* 0x0B728 */ u8 unk_0B728;
    /* 0x0B729 */ u8 unk_0B729;
    /* 0x0B72A */ u8 unk_0B72A;
    /* 0x0B72B */ u8 unk_0B72B;
    /* 0x0B72C */ u8 unk_0B72C;
    /* 0x0B72D */ u8 unk_0B72D;
    /* 0x0B72E */ u8 pad_0B72E[2];
    /* 0x0B730 */ u8 unk_0B730[16];
    /* 0x0B740 */ u8 pad_0B740[0xE624 - 0xB740];
    /* 0x0E624 */ u32 unk_0E624;
    /* 0x0E628 */ u8 pad_0E628[0xEE00 - 0xE628];
    /* 0x0EE00 */ u32 unk_0EE00;
    /* 0x0EE04 */ u32 unk_0EE04;
    /* 0x0EE08 */ u32 unk_0EE08;
    /* 0x0EE0C */ u16 unk_0EE0C;
    /* 0x0EE0E */ u8 unk_0EE0E;
    /* 0x0EE0F */ u8 busy;          // 1 while text is being synthesized
    /* 0x0EE10 */ u8 stopRequested; // 1 once a caller has asked for that to stop
    /* 0x0EE11 */ u8 unk_0EE11;
    /* 0x0EE14 */ u32 unk_0EE14;
    /* 0x0EE18 */ u32 unk_0EE18;
    /* 0x0EE1C */ OSThread thread;  // Synthesizes
    /* 0x0EEDC */ u8 pad_0EEDC[0xEEF0 - 0xEEDC];
    /* 0x0EEF0 */ OSMutex mutex;    // Guards busy and stopRequested
    /* 0x0EF08 */ OSMutex unk_0EF08;
    /* 0x0EF20 */ ShttsParams unk_0EF20;
    /* 0x0EF50 */ ShttsParams unk_0EF50;
    /* 0x0EF80 */ s32 error;        // Index of a message, see func_0208daf8
    /* 0x0EF84 */ u8 pad_0EF84[0x14000 - 0xEF84];
} Shtts; // 0x14000

void func_02089cf4(Shtts *engine);
void func_02089d08(Shtts *engine);
int func_02089d1c(Shtts *engine);
void func_0208c398(Shtts *engine);
void func_0208d4f0(Shtts *engine, ShttsCallback callback, void *arg);
int func_0208d878(Shtts *engine);
int func_0208d8fc(Shtts *engine);
int func_0208dab4(Shtts *engine);
int func_0208dac4(Shtts *engine);
const char *func_0208daf8(Shtts *engine);

#endif // SHTTS_H

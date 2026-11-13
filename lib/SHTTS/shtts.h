#ifndef SHTTS_H
#define SHTTS_H

#include <nitro.h>

// Sharp's text-to-speech engine, SHTTS 1.2.0. None of its names survive in the ROM, so functions are named by
// address, and fields by offset until what they hold is clear.

typedef void (*ShttsCallback)(int samples, s16 *data, void *arg);

// Settings of a voice. The engine has two of these: the ones a caller has set, and the ones in use, which start as
// a copy of those and which commands in the text then change. The names of the fields are the engine's own, from
// the strings in func_020873a4, where they are known.
typedef struct ShttsParams {
    /* 0x00 */ s8 unk_00;
    /* 0x02 */ s16 pitch;
    /* 0x04 */ s16 accent;
    /* 0x06 */ s16 phrase;
    /* 0x08 */ s16 volume;
    /* 0x0A */ s16 speed;
    /* 0x0C */ s16 echoGain;
    /* 0x0E */ s16 echoDelay;
    /* 0x10 */ u32 mode;
    /* 0x14 */ s16 unk_14;
    /* 0x16 */ s16 firDelay;
    /* 0x18 */ u32 chorus;
    /* 0x1C */ u32 firGain;
    /* 0x20 */ u32 ring;
    /* 0x24 */ u32 warp;
    /* 0x28 */ u16 unk_28;
    /* 0x2A */ u16 unk_2A;
    /* 0x2C */ s16 pitchTrans;
} ShttsParams; // 0x30

// The engine's whole state, 0x14000 bytes that the caller provides. Its fields are in fields.txt.
#include "shtts_struct.h"

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
int func_0208db2c(Shtts *engine, int param, int value);
int func_0208dda4(Shtts *engine, int value);
int func_0208de90(Shtts *engine, int value);
void func_0208c4dc(int warp, int live, Shtts *engine);
int func_0208dca4(Shtts *engine, int param);
int func_0208dfcc(Shtts *engine, const char *name);
const char *func_0208dff8(const char *name);
int func_0208e04c(Shtts *engine, const char *commands);
int func_0208e338(Shtts *engine);
int func_0208e354(const char *name);
int func_020873a4(const char *command, int live, Shtts *engine);
void func_02086814(Shtts *engine);
int func_02087e44(int value, int min, int max, const char *name);
int func_02087e64(const char *text);
void func_0208a010(Shtts *engine);
void func_0208c404(Shtts *engine);

// A named set of commands, such as a speaker or a way of speaking
typedef struct ShttsPreset {
    /* 0x00 */ char name[20];      // As written in text, such as "<\\Spk_male"
    /* 0x14 */ char commands[100]; // Commands that each start with '#', such as "#Q\n#B4800\n#S90"
} ShttsPreset; // 0x78

// One entry of the table at 0x020e8f08, which is indexed by four numbers
typedef struct ShttsUnit {
    /* 0x0 */ s16 unk_0;
    /* 0x2 */ s16 unk_2;
    /* 0x4 */ u8 pad_4[4];
} ShttsUnit; // 0x8

extern ShttsUnit data_020e8f08[][8][38][8];

typedef struct ShttsUnk_02087d98 {
    /* 0x00 */ s8 unk_00;
    /* 0x01 */ u8 pad_01[3];
    /* 0x04 */ s8 unk_04;
    /* 0x05 */ s8 unk_05;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ u8 pad_0A[2];
    /* 0x0C */ s16 pitch;
    /* 0x10 */ u32 unk_10;
} ShttsUnk_02087d98;

typedef struct ShttsUnk_020856b0 {
    /* 0x000 */ u8 unk_000[0x200];
    /* 0x200 */ s32 unk_200;
    /* 0x204 */ s32 unk_204;
    /* 0x208 */ s32 unk_208;
    /* 0x20C */ s32 unk_20C;
} ShttsUnk_020856b0;

// The first field of each 16-byte entry of a table that func_0208aba8 is given
typedef struct ShttsUnk_0208aba8 {
    /* 0x0 */ s16 unk_0;
    /* 0x2 */ u8 pad_2[14];
} ShttsUnk_0208aba8; // 0x10

// A sorted list of words that func_02084794 searches
typedef struct ShttsWord {
    /* 0x0 */ const char *name;
    /* 0x4 */ s8 value;
} ShttsWord; // 0x8

typedef struct ShttsWordList {
    /* 0x0 */ u8 pad_0[8];
    /* 0x8 */ ShttsWord *words;
    /* 0xC */ s16 count;
} ShttsWordList;

// A sine wave, 256 values to the cycle
extern s16 data_020e8d08[256];

int func_020857e8();
void func_02089e70(int code);
char *func_02089d7c(char *text, int c);
s16 func_02089444(int a, int b);
void func_0208e4e4(int count, s16 *buffer, Shtts *engine);
int func_02089360(const char *text, s16 *out);
void func_0208abf4(ShttsUnk_0208aba8 *table, int count, int a, int b);
void func_0208be9c(int count, s16 *buffer, Shtts *engine);

#define SHTTS_PRESET_COUNT 53
#define SHTTS_PHONEME_COUNT 38

extern ShttsPreset ShttsPresets[SHTTS_PRESET_COUNT];
// The sounds of speech as the engine writes them: "S" for silence, the vowels, "N", "X", then the consonants
extern const char *ShttsPhonemeNames[SHTTS_PHONEME_COUNT];

#endif // SHTTS_H

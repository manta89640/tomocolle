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

// What the chorus effect works with: a ring of samples, and where it is in it
typedef struct ShttsUnk_020856b0 {
    /* 0x000 */ s16 unk_000[0x100];
    /* 0x200 */ s32 unk_200;
    /* 0x204 */ s32 unk_204;
    /* 0x208 */ s32 unk_208;
    /* 0x20C */ s32 unk_20C;
} ShttsUnk_020856b0;

// The engine's whole state, 0x14000 bytes that the caller provides. Its fields are in fields.txt, and the struct
// itself is included at the end of this header, after the types of its fields.
typedef struct Shtts Shtts;

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
s16 func_02087e44(int value, s16 min, s16 max, const char *name);
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
    /* 0x0 */ u16 unk_0;   // The index of its first record in data_020bc26c
    /* 0x2 */ s16 unk_2;   // 0 if there is no such unit
    /* 0x4 */ u8 unk_4[4]; // unk_4[k] and unk_4[k + 1] bound the records of its k-th sound
} ShttsUnit; // 0x8

extern ShttsUnit data_020e8f08[][8][38][8];

typedef struct ShttsUnk_02087d98 {
    /* 0x00 */ char name[4];
    /* 0x04 */ s8 unk_04;
    /* 0x05 */ s8 unk_05;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 unk_0A;
    /* 0x0C */ s16 pitch;
    /* 0x10 */ struct ShttsUnk_02087d98 *next;
} ShttsUnk_02087d98;


// The first field of each 16-byte entry of a table that func_0208aba8 is given
typedef struct ShttsUnk_0208aba8 {
    /* 0x0 */ s16 unk_0;
    /* 0x2 */ s8 unk_2;
    /* 0x3 */ s8 unk_3;
    /* 0x4 */ int kind;
    /* 0x8 */ s8 unk_8[5];
    /* 0xD */ u8 pad_D[3];
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
extern const s16 data_020e8d08[256];

int func_020857e8(const char *text);
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


extern s8 data_020bc22c[]; // Flags of each phoneme
s16 func_02089244(const char *text, const char *table, int count);
s16 func_02089094(char *text, u32 size, const char *line);
int func_020892a8(const char *text, const char *unused);
int func_02089304(const char *text, const char *unused);
void func_020893e4(const char *text, s16 *out);
void func_02089e74(s16 *dst, s16 *src, int count, int gain);
void func_0208e464(int from, int to, int count, s16 *data);
void func_0208b6c0(int *phase, int count, int frequency, s16 *src, s16 *dst);
int func_02089c50(Shtts *engine);
int func_0208ea20(const int *state, const s16 *coefficients);
int func_0208ea84(const int *state, const s16 *coefficients);

// One frame of speech as it is stored: func_020894cc decodes it into samples
typedef struct ShttsFrame {
    /* 0x0 */ s16 unk_0;
    /* 0x2 */ s8 unk_2;
    /* 0x3 */ u8 unk_3;
    /* 0x4 */ s32 unk_4; // Index in data_020bc22c
    /* 0x8 */ s8 unk_8[5]; // Ten indices of four bits each
} ShttsFrame;

// ---- From the decompilation of group a

// One entry of the array that func_02084c1c walks, 16 bytes
typedef struct ShttsUnk_02084c1c {
    /* 0x0 */ s32 phoneme; // Index in data_020bc22c
    /* 0x4 */ s16 length;
    /* 0x6 */ u8 pad_6[8];
    /* 0xE */ u8 first;
    /* 0xF */ u8 last;
} ShttsUnk_02084c1c; // 0x10

// One entry of the array that func_020850e0 spreads over a number of pointers, 16 bytes
typedef struct ShttsUnk_020850e0 {
    /* 0x0 */ s16 unk_0;
    /* 0x2 */ s8 unk_2;
    /* 0x3 */ u8 pad_3[13];
} ShttsUnk_020850e0; // 0x10

typedef struct ShttsFrame ShttsUnk_020858c8;

// The structures that func_02085b30 walks
typedef struct ShttsUnk_02085b30_C {
    /* 0x00 */ u8 pad_00[0x10];
    /* 0x10 */ struct ShttsUnk_02085b30_C *next;
} ShttsUnk_02085b30_C;

typedef struct ShttsUnk_02085b30_B {
    /* 0x00 */ const char *text;
    /* 0x04 */ s8 count; // Of the list at unk_18
    /* 0x05 */ u8 pad_05[5];
    /* 0x0A */ s16 speed;
    /* 0x0C */ u8 pad_0C[10];
    /* 0x16 */ s16 unk_16;
    /* 0x18 */ ShttsUnk_02085b30_C *unk_18;
    /* 0x1C */ struct ShttsUnk_02085b30_B *next;
} ShttsUnk_02085b30_B;

typedef struct ShttsUnk_02085b30_D {
    /* 0x00 */ u8 pad_00[0x12];
    /* 0x12 */ s16 unk_12;
} ShttsUnk_02085b30_D;

typedef struct ShttsUnk_02085b30_A {
    /* 0x0 */ s8 count; // Of the list at first
    /* 0x4 */ ShttsUnk_02085b30_B *first;
    /* 0x8 */ ShttsUnk_02085b30_D *unk_8;
    /* 0xC */ struct ShttsUnk_02085b30_A *next;
} ShttsUnk_02085b30_A; // 0x10

extern ShttsPreset data_020b7fc8[SHTTS_PRESET_COUNT];
extern s16 data_020b9aa0[];
extern s16 data_020b9b04[];
extern s16 data_020b9bbc[];
extern ShttsUnk_02085b30_C data_020b9cbc;
extern ShttsUnk_020850e0 *const data_020bc258;

void func_02084830(char *text);
s16 func_02084c1c(ShttsUnk_02084c1c *first, ShttsUnk_02084c1c *last, ShttsUnk_020850e0 *frames, ShttsUnk_020850e0 **out, Shtts *engine);
void func_020850e0(ShttsUnk_020850e0 *src, s16 srcCount, s16 dstCount, ShttsUnk_020850e0 **dst, Shtts *engine);
void func_02085704(ShttsUnk_020856b0 *unk, s16 *in, s16 *out, int count);
int func_020858c8(ShttsUnk_020858c8 *unk, int b, Shtts *engine);
void func_02085a70(Shtts *engine);
void func_02085b30(ShttsUnk_02085b30_A *list, Shtts *engine);
void func_02085d80(ShttsUnk_02085b30_C **units, int count, int total, int isFirst, int isLast, int position, char *unk, int unk_16, Shtts *engine);


// ---- From the decompilation of group c
// One frame of speech as it is stored: func_020894cc decodes it into samples


extern u16 data_020bb720[10][16];
extern s16 data_020bb860[];
extern s16 data_020bba60[];
extern s16 data_020bbc60[];

char *func_02088fc8(char *out, s16 value);
s16 func_02089118(char *dest, u32 size, const char *text, s16 a, s16 b, s16 c, s16 d);
int func_020894cc(ShttsFrame *frame, s16 *out, Shtts *engine);
const char *func_02087e94(const char *text, char *dest, u32 size);

// How a piece of Japanese text is written in the engine's phonemes
typedef struct ShttsKana {
    /* 0x0 */ char kana[8];
    /* 0x8 */ char phonemes[4];
} ShttsKana; // 0xC

extern const s16 data_020bac48; // The number of entries of data_020bac4c
extern const char data_020bac4c[][12];


// ---- From the decompilation of group f

// The lists that func_0208c8e0 walks: a list of lists of lists of ShttsUnk_02087d98
typedef struct ShttsUnk_0208c8e0_2 {
    /* 0x00 */ u8 pad_00[4];
    /* 0x04 */ s8 count;
    /* 0x05 */ u8 pad_05[0xD];
    /* 0x12 */ s16 unk_12; // If not 0, the length of a silence after this
    /* 0x14 */ u8 pad_14[4];
    /* 0x18 */ ShttsUnk_02087d98 *first;
    /* 0x1C */ struct ShttsUnk_0208c8e0_2 *next;
} ShttsUnk_0208c8e0_2;
typedef struct ShttsUnk_0208c8e0_1 {
    /* 0x00 */ s8 count;
    /* 0x04 */ ShttsUnk_0208c8e0_2 *first;
    /* 0x08 */ u8 pad_08[4];
    /* 0x0C */ struct ShttsUnk_0208c8e0_1 *next;
} ShttsUnk_0208c8e0_1;

typedef struct ShttsUnk_0208c8e0 {
    /* 0x00 */ s8 count;
    /* 0x04 */ ShttsUnk_0208c8e0_1 *first;
} ShttsUnk_0208c8e0;

// One sound to speak. func_0208c8e0 and func_0208cc10 fill an array of these from its second entry on, and end it
// with one whose phoneme is 7.
typedef struct ShttsSound {
    /* 0x0 */ s32 phoneme; // Index in ShttsPhonemeNames
    /* 0x4 */ s16 unk_4;
    /* 0x6 */ s16 unk_6;
    /* 0x8 */ s16 unk_8;
    /* 0xA */ s16 unk_A;
    /* 0xC */ s16 unk_C;
    /* 0xE */ s8 unk_E;
    /* 0xF */ s8 unk_F;
} ShttsSound; // 0x10

int func_0208c8e0(ShttsUnk_0208c8e0 *lists, ShttsSound *sounds, int max);
int func_0208cc10(ShttsSound *sounds, Shtts *engine);
void func_0208cf40(ShttsSound *sounds, int count, int flags, Shtts *engine);
int func_0208d500(Shtts *engine, const char *text, u32 mode, u32 unk, ShttsCallback callback, void *arg);
void func_0208e3b4(int unk, Shtts *engine);
int func_0208e83c(int count, int order, const s16 *coefficients, int *state, s16 *src, s16 *dst, Shtts *engine);
void func_0208c5a8(Shtts *engine);
void func_02089ed4(int count, s16 *src, s16 *dst, Shtts *engine);
void func_0208683c(int count, s16 *src, s16 *dst, Shtts *engine);
// sscanf
int func_020188f4(const char *text, const char *format, ...);

// "[SDK+SHARP:SHTTS-1_2_0]", the engine's tag in the SDK's list of libraries. It has no symbol in symbols.txt.
extern char data_02000bd8[];


int func_0208473c(const char *a, const char *b);
int func_02084794(char *text, ShttsWordList *list);
int func_02084814(void *arg);
void func_020856b0(ShttsUnk_020856b0 *unk, int rate);
int func_02085a64(Shtts *engine);
int func_02086628(s16 value);
int func_02086a20(const int *state, const s16 *coefficients, int input);
void func_02086a74(char *value, char **out);
int func_02086a7c(char *dst, int size, char **cursor);
void func_02086b14(Shtts *engine);
void func_02087d98(ShttsUnk_02087d98 *unk, Shtts *engine);
int func_02087dc8(const char *text, const char *find);
int func_02089b5c(int a, int b);
int func_02089cec(Shtts *engine);
int func_02089d4c(Shtts *engine);
char *func_02089da0(char **cursor, char *delimiters);
void func_02089e48(char *dst, int c, int count);
void func_0208aba8(ShttsUnk_0208aba8 *table, int count, int a, int b);
ShttsUnit *func_0208bdf4(int *index, Shtts *engine);
void func_0208be38(Shtts *engine);
void func_0208c054(Shtts *engine);
int func_0208c568(Shtts *engine);
int func_0208e09c(Shtts *engine, int value);
int func_0208e0cc(Shtts *engine, int value);
int func_0208e0fc(Shtts *engine, int value);
int func_0208e12c(Shtts *engine, int value);
int func_0208e160(Shtts *engine, int value);
int func_0208e190(Shtts *engine, int value);
int func_0208e1c0(Shtts *engine, int value);
int func_0208e1e8(Shtts *engine, int value);
int func_0208e210(Shtts *engine, int value);
int func_0208e238(Shtts *engine, int value);
int func_0208e298(Shtts *engine, int value);
int func_0208e2c0(Shtts *engine, int value);
int func_0208e2e8(Shtts *engine, int value);
int func_0208e310(Shtts *engine, int value);
int func_0208e81c(int a, int b);

// ---- From the decompilation of group d
// The three levels above ShttsUnk_02087d98. Each has a count of the ones below it, which are linked.
typedef struct ShttsUnk_0208a25c {
    /* 0x00 */ void *unk_00; // What func_020857e8 takes
    /* 0x04 */ s8 count;
    /* 0x05 */ s8 unk_05;
    /* 0x06 */ s8 unk_06;
    /* 0x07 */ s8 unk_07;
    /* 0x08 */ u8 pad_08[0xA];
    /* 0x12 */ s16 unk_12;
    /* 0x14 */ u8 pad_14[4];
    /* 0x18 */ ShttsUnk_02087d98 *first;
    /* 0x1C */ struct ShttsUnk_0208a25c *next;
} ShttsUnk_0208a25c;

typedef struct ShttsUnk_0208a5b4 {
    /* 0x0 */ s8 count;
    /* 0x1 */ s8 unk_1;
    /* 0x2 */ s8 unk_2;
    /* 0x4 */ ShttsUnk_0208a25c *first;
    /* 0x8 */ ShttsUnk_0208a25c *last;
    /* 0xC */ struct ShttsUnk_0208a5b4 *next;
} ShttsUnk_0208a5b4; // 0x10

typedef struct ShttsUnk_0208a038 {
    /* 0x0 */ s8 count;
    /* 0x1 */ s8 unk_1;
    /* 0x4 */ ShttsUnk_0208a5b4 *first; // They are also an array
    /* 0x8 */ struct ShttsUnk_0208a038 *next;
} ShttsUnk_0208a038;

void func_020897c8(int order, u16 *in, s16 *out);

void func_02089a3c(int level, s16 *in, s16 *out, int count);

int func_02089bb0(int x);

void func_0208a038(ShttsUnk_0208a038 *list, Shtts *engine);

int func_0208a25c(ShttsUnk_0208a038 *unk, int length, ShttsUnk_02087d98 *last, int kind, s16 notFirst, Shtts *engine);

void func_0208a5b4(ShttsUnk_0208a5b4 *unk, Shtts *engine);

s16 func_0208ab18(int a, int b);

extern s16 *data_020bbf30;

extern s16 data_020bc20c[5];

extern s16 data_020bc216[5];

extern s16 data_020bc220[5];

// ---- From the decompilation of group e
// One sound of the text with its length and pitch, as func_0208b738 reads them. The list ends with one of kind 7.
typedef struct ShttsPhone {
    /* 0x0 */ int kind; // An index in ShttsPhonemeNames (0 is silence), or 7 at the end of the list
    /* 0x4 */ s16 length;
    /* 0x6 */ s16 unk_6;
    /* 0x8 */ s16 unk_8;
    /* 0xA */ s16 unk_A;
    /* 0xC */ s16 unk_C;
    /* 0xE */ s8 unk_E;
    /* 0xF */ s8 unk_F;
} ShttsPhone; // 0x10

// One sound to synthesize, which func_0208ba64 makes from a ShttsPhone and a ShttsUnit
typedef struct ShttsSegment {
    /* 0x0 */ int kind;
    /* 0x4 */ s16 length;
    /* 0x6 */ s16 unk_6;
    /* 0x8 */ s16 unk_8;
    /* 0xA */ s16 unk_A;
    /* 0xC */ u16 unit;  // The unit's unk_0, an index in data_020bc26c
    /* 0xE */ u8 first;  // The first and the last of the unit's entries in data_020bc26c that are this sound's
    /* 0xF */ u8 last;
} ShttsSegment; // 0x10

// What the pointers at 0x5D38 point to
typedef struct ShttsUnk_05D38 {
    /* 0x0 */ u8 pad_0[3];
    /* 0x3 */ u8 unk_3;
    /* 0x4 */ int phoneme;
} ShttsUnk_05D38;

// One entry of the table at 0x020bc26c
typedef struct ShttsUnk_020bc26c {
    /* 0x0 */ s8 unk_0;
    /* 0x1 */ s8 unk_1[5];
} ShttsUnk_020bc26c; // 0x6

// A list that func_0208c690 walks
typedef struct ShttsUnk_01660Item {
    /* 0x0 */ u8 pad_0[8];
    /* 0x8 */ s8 *unk_8;
    /* 0xC */ u8 pad_C[4];
} ShttsUnk_01660Item; // 0x10

typedef struct ShttsUnk_01660 {
    /* 0x0 */ s8 count;
    /* 0x4 */ ShttsUnk_01660Item *items;
    /* 0x8 */ struct ShttsUnk_01660 *next;
} ShttsUnk_01660;

extern void *const data_020bc254[];

extern ShttsUnk_020bc26c data_020bc26c[];

void func_0208ad48(Shtts *engine);

void func_0208b550(s16 *out, s16 count, s16 target, s16 max, Shtts *engine);

void func_0208b738(ShttsPhone *phones, void *unused, Shtts *engine);

int func_0208ba64(int mode, ShttsPhone *first, ShttsPhone *last, void *unused, s16 unused2, Shtts *engine);

void func_0208bf28(int count, Shtts *engine);

void func_0208c0c8(int a, int b, Shtts *engine);

void func_0208c230(ShttsUnk_05D38 **units, s16 *pitches, s16 count, Shtts *engine);

void func_0208c690(void *arg);

int func_02086b90(void *a, void *b, Shtts *engine);

void func_02086fd8(void *a, void *b, void *c);

#include "shtts_struct.h"

#endif // SHTTS_H

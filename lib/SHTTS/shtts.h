#ifndef SHTTS_H
#define SHTTS_H

#include <nitro.h>

// Sharp's text-to-speech engine, SHTTS 1.2.0. None of its names survive in the ROM, so functions are named by
// address, and fields by offset until what they hold is clear.

typedef void (*ShttsCallback)(int samples, s16 *data, void *arg);

// Settings of a voice. The engine has two of these: the ones a caller has set, and the ones in use, which start as
// a copy of those and which commands in the text then change. The names of the fields are the engine's own, from
// the strings in ShttsParam_RunCommand, where they are known.
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
    /* 0x28 */ u16 warpScale;
    /* 0x2A */ u16 warpOrder;
    /* 0x2C */ s16 pitchTrans;
} ShttsParams; // 0x30

// What the chorus effect works with: a ring of samples, and where it is in it
typedef struct ShttsChorus {
    /* 0x000 */ s16 ring[0x100];
    /* 0x200 */ s32 sweepPeriod;
    /* 0x204 */ s32 writePos;
    /* 0x208 */ s32 sweepPos;
    /* 0x20C */ s32 sweepTimer;
} ShttsChorus;

// The engine's whole state, 0x14000 bytes that the caller provides. Its fields are in fields.txt, and the struct
// itself is included at the end of this header, after the types of its fields.
typedef struct Shtts Shtts;

void ShttsEngine_Lock(Shtts *engine);
void ShttsEngine_Unlock(Shtts *engine);
int ShttsEngine_IsBusy(Shtts *engine);
void ShttsParam_SetDefaults(Shtts *engine);
void ShttsEngine_SetCallback(Shtts *engine, ShttsCallback callback, void *arg);
int Shtts_Stop(Shtts *engine);
int Shtts_Init(Shtts *engine);
int Shtts_ResetParams(Shtts *engine);
int Shtts_GetState(Shtts *engine);
const char *Shtts_GetPhonemeName(Shtts *engine);
int Shtts_SetParam(Shtts *engine, int param, int value);
int ShttsParam_SetSpeaker(Shtts *engine, int value);
int ShttsParam_SetStyle(Shtts *engine, int value);
void ShttsParam_StoreWarp(int warp, int live, Shtts *engine);
int Shtts_GetParam(Shtts *engine, int param);
int ShttsParam_ApplyPreset(Shtts *engine, const char *name);
const char *ShttsParam_FindPreset(const char *name);
int ShttsParam_RunCommands(Shtts *engine, const char *commands);
int ShttsParam_GetMode(Shtts *engine);
int ShttsPhone_FindPhoneme(const char *name);
int ShttsParam_RunCommand(const char *command, int live, Shtts *engine);
void ShttsEffect_ResetEcho(Shtts *engine);
s16 ShttsParam_Clamp(int value, s16 min, s16 max, const char *name);
int ShttsParam_ParseShort(const char *text);
void ShttsEffect_ResetFir(Shtts *engine);
void ShttsParam_UseSetParams(Shtts *engine);

// A named set of commands, such as a speaker or a way of speaking
typedef struct ShttsPreset {
    /* 0x00 */ char name[20];      // As written in text, such as "<\\Spk_male"
    /* 0x14 */ char commands[100]; // Commands that each start with '#', such as "#Q\n#B4800\n#S90"
} ShttsPreset; // 0x78

// One entry of the table at 0x020e8f08, which is indexed by four numbers
typedef struct ShttsUnit {
    /* 0x0 */ u16 unk_0;   // The index of its first record in ShttsUnitFrames
    /* 0x2 */ s16 unk_2;   // 0 if there is no such unit
    /* 0x4 */ u8 unk_4[4]; // unk_4[k] and unk_4[k + 1] bound the records of its k-th sound
} ShttsUnit; // 0x8

extern ShttsUnit ShttsUnits[][8][38][8];

typedef struct ShttsSyllable {
    /* 0x00 */ char name[4];
    /* 0x04 */ s8 qMark;
    /* 0x05 */ s8 unk_05;
    /* 0x06 */ s16 unk_06;
    /* 0x08 */ s16 unk_08;
    /* 0x0A */ s16 gain;
    /* 0x0C */ s16 pitch;
    /* 0x10 */ struct ShttsSyllable *next;
} ShttsSyllable;


// The first field of each 16-byte entry of a table that ShttsSynth_FitFrameGains is given
typedef struct ShttsRunFrame {
    /* 0x0 */ s16 unk_0;
    /* 0x2 */ s8 unk_2;
    /* 0x3 */ s8 unk_3;
    /* 0x4 */ int kind;
    /* 0x8 */ s8 unk_8[5];
    /* 0xD */ u8 pad_D[3];
} ShttsRunFrame; // 0x10

// A sorted list of words that ShttsDict_FindWord searches
typedef struct ShttsDictEntry {
    /* 0x0 */ const char *name;
    /* 0x4 */ s8 value;
} ShttsDictEntry; // 0x8

typedef struct ShttsDict {
    /* 0x0 */ u8 pad_0[8];
    /* 0x8 */ ShttsDictEntry *words;
    /* 0xC */ s16 count;
} ShttsDict;

// A sine wave, 256 values to the cycle
extern const s16 ShttsSineTable[256];

int ShttsWord_CountMorae(const char *text);
void ShttsUtil_ReportError(int code);
char *ShttsUtil_FindChar(char *text, int c);
s16 ShttsMath_DivideRounded(int a, int b);
void ShttsOutput_SendBlock(int count, s16 *buffer, Shtts *engine);
int ShttsText_ReadDigitBefore(const char *text, s16 *out);
void ShttsSynth_RampFrameGains(ShttsRunFrame *table, int count, int a, int b);
void ShttsOutput_ClearSamples(int count, s16 *buffer, Shtts *engine);

#define SHTTS_PRESET_COUNT 53
#define SHTTS_PHONEME_COUNT 38

extern ShttsPreset ShttsPresets[SHTTS_PRESET_COUNT];
// The sounds of speech as the engine writes them: "S" for silence, the vowels, "N", "X", then the consonants
extern const char *ShttsPhonemeNames[SHTTS_PHONEME_COUNT];


extern s8 ShttsPhonemeFlags[]; // Flags of each phoneme
s16 ShttsText_FindKana(const char *text, const char *table, int count);
s16 ShttsText_AppendLine(char *text, u32 size, const char *line);
int ShttsText_ParseStrengthDigit(const char *text, const char *unused);
int ShttsText_ParseWideStrengthDigit(const char *text, const char *unused);
void ShttsText_ReadStrengthBefore(const char *text, s16 *out);
void ShttsMath_ScaleSamples(s16 *dst, s16 *src, int count, int gain);
void ShttsSynth_FadeSamples(int from, int to, int count, s16 *data);
void ShttsEffect_ApplyRing(int *phase, int count, int frequency, s16 *src, s16 *dst);
int ShttsEngine_ResetSynthesis(Shtts *engine);
int ShttsSynth_SumFilter6(const int *state, const s16 *coefficients);
int ShttsSynth_SumFilter10(const int *state, const s16 *coefficients);

// One frame of speech as it is stored: ShttsSynth_DecodeFrame decodes it into samples
typedef struct ShttsFrame {
    /* 0x0 */ s16 unk_0;
    /* 0x2 */ s8 unk_2;
    /* 0x3 */ u8 unk_3;
    /* 0x4 */ s32 unk_4; // Index in ShttsPhonemeFlags
    /* 0x8 */ s8 unk_8[5]; // Ten indices of four bits each
} ShttsFrame;

// ---- From the decompilation of group a

// One entry of the array that ShttsSynth_StretchSegments walks, 16 bytes
typedef struct ShttsStretchSegment {
    /* 0x0 */ s32 phoneme; // Index in ShttsPhonemeFlags
    /* 0x4 */ s16 length;
    /* 0x6 */ u8 pad_6[8];
    /* 0xE */ u8 first;
    /* 0xF */ u8 last;
} ShttsStretchSegment; // 0x10

// One entry of the array that ShttsSynth_StretchFrames spreads over a number of pointers, 16 bytes
typedef struct ShttsStretchFrame {
    /* 0x0 */ s16 unk_0;
    /* 0x2 */ s8 unk_2;
    /* 0x3 */ u8 pad_3[13];
} ShttsStretchFrame; // 0x10

typedef struct ShttsFrame ShttsPreparedFrame;

// The structures that ShttsDuration_ProcessGroups walks
typedef struct ShttsDurationSyllable {
    /* 0x00 */ u8 pad_00[0x10];
    /* 0x10 */ struct ShttsDurationSyllable *next;
} ShttsDurationSyllable;

typedef struct ShttsDurationWord {
    /* 0x00 */ const char *text;
    /* 0x04 */ s8 count; // Of the list at first
    /* 0x05 */ u8 pad_05[5];
    /* 0x0A */ s16 speed;
    /* 0x0C */ u8 pad_0C[10];
    /* 0x16 */ s16 volume;
    /* 0x18 */ ShttsDurationSyllable *first;
    /* 0x1C */ struct ShttsDurationWord *next;
} ShttsDurationWord;

typedef struct ShttsDurationLastWord {
    /* 0x00 */ u8 pad_00[0x12];
    /* 0x12 */ s16 unk_12;
} ShttsDurationLastWord;

typedef struct ShttsDurationAccentGroup {
    /* 0x0 */ s8 count; // Of the list at first
    /* 0x4 */ ShttsDurationWord *first;
    /* 0x8 */ ShttsDurationLastWord *unk_8;
    /* 0xC */ struct ShttsDurationAccentGroup *next;
} ShttsDurationAccentGroup; // 0x10

extern ShttsPreset ShttsTextPresets[SHTTS_PRESET_COUNT];
extern s16 ShttsPitchPeriodCoarse[];
extern s16 ShttsGainLevels[];
extern s16 ShttsPitchPeriodFine[];
extern ShttsDurationSyllable ShttsBoundarySyllable;
extern ShttsStretchFrame *const ShttsFillerFrame;

void ShttsText_ExpandPresets(char *text);
s16 ShttsSynth_StretchSegments(ShttsStretchSegment *first, ShttsStretchSegment *last, ShttsStretchFrame *frames, ShttsStretchFrame **out, Shtts *engine);
void ShttsSynth_StretchFrames(ShttsStretchFrame *src, s16 srcCount, s16 dstCount, ShttsStretchFrame **dst, Shtts *engine);
void ShttsEffect_ApplyChorus(ShttsChorus *unk, s16 *in, s16 *out, int count);
int ShttsSynth_PrepareFrame(ShttsPreparedFrame *unk, int b, Shtts *engine);
void ShttsSynth_CopyFrameWave(Shtts *engine);
void ShttsDuration_ProcessGroups(ShttsDurationAccentGroup *list, Shtts *engine);
void ShttsDuration_SetWordSyllables(ShttsSyllable **list, int count, s16 unk3, int first, int last, int unk6, char *vowel, s16 unk8, Shtts *engine);


// ---- From the decompilation of group c
// One frame of speech as it is stored: ShttsSynth_DecodeFrame decodes it into samples


extern u16 ShttsLspCodebook[10][16];
extern s16 ShttsVoicedExcitation[];
extern s16 ShttsVoicedExcitationAlt[];
extern s16 ShttsUnvoicedExcitation[];

char *ShttsText_FormatNumber(char *out, s16 value);
s16 ShttsText_AppendWordLine(char *dest, u32 size, const char *text, s16 a, s16 b, s16 c, s16 d);
int ShttsSynth_DecodeFrame(ShttsFrame *frame, s16 *out, Shtts *engine);
const char *ShttsText_ConvertToLines(const char *text, char *dest, u32 size);

// How a piece of Japanese text is written in the engine's phonemes
typedef struct ShttsKana {
    /* 0x0 */ char kana[8];
    /* 0x8 */ char phonemes[4];
} ShttsKana; // 0xC

extern const s16 ShttsKanaCount; // The number of entries of ShttsKanaTable
extern const char ShttsKanaTable[][12];


// ---- From the decompilation of group f

// The lists that ShttsPhone_BuildFromSyllables walks: a list of lists of lists of ShttsSyllable
typedef struct ShttsPhoneWord {
    /* 0x00 */ u8 pad_00[4];
    /* 0x04 */ s8 count;
    /* 0x05 */ u8 pad_05[0xD];
    /* 0x12 */ s16 unk_12; // If not 0, the length of a silence after this
    /* 0x14 */ u8 pad_14[4];
    /* 0x18 */ ShttsSyllable *first;
    /* 0x1C */ struct ShttsPhoneWord *next;
} ShttsPhoneWord;
typedef struct ShttsPhoneAccentGroup {
    /* 0x00 */ s8 count;
    /* 0x04 */ ShttsPhoneWord *first;
    /* 0x08 */ u8 pad_08[4];
    /* 0x0C */ struct ShttsPhoneAccentGroup *next;
} ShttsPhoneAccentGroup;

typedef struct ShttsPhonePhraseGroup {
    /* 0x00 */ s8 count;
    /* 0x04 */ ShttsPhoneAccentGroup *first;
} ShttsPhonePhraseGroup;

// One sound to speak. ShttsPhone_BuildFromSyllables and ShttsPhone_ReadStdin fill an array of these from its second entry on, and end it
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

int ShttsPhone_BuildFromSyllables(ShttsPhonePhraseGroup *lists, ShttsSound *sounds, int max);
int ShttsPhone_ReadStdin(ShttsSound *sounds, Shtts *engine);
void ShttsPhone_ShapeEnding(ShttsSound *sounds, int count, int flags, Shtts *engine);
int Shtts_Speak(Shtts *engine, const char *text, u32 mode, u32 unk, ShttsCallback callback, void *arg);
void ShttsSynth_FadeWave(int unk, Shtts *engine);
int ShttsSynth_FilterRecursive(int count, int order, const s16 *coefficients, int *state, s16 *src, s16 *dst, Shtts *engine);
void ShttsEngine_Start(Shtts *engine);
void ShttsEffect_ApplyFir(int count, s16 *src, s16 *dst, Shtts *engine);
void ShttsEffect_ApplyEcho(int count, s16 *src, s16 *dst, Shtts *engine);
// sscanf
int func_020188f4(const char *text, const char *format, ...);

// "[SDK+SHARP:SHTTS-1_2_0]", the engine's tag in the SDK's list of libraries. It has no symbol in symbols.txt.
extern char ShttsMiddlewareTag[];


int ShttsDict_CompareWords(const char *a, const char *b);
int ShttsDict_FindWord(char *text, ShttsDict *list);
int ShttsWord_GetDefaultAccent(void *arg);
void ShttsEffect_InitChorus(ShttsChorus *unk, int rate);
int ShttsSynth_GetPitchPeriod(Shtts *engine);
int ShttsDuration_ClassifyMoraCount(s16 value);
int ShttsEffect_SumEchoFilter(const int *state, const s16 *coefficients, int input);
void ShttsLine_SetCursor(char *value, char **out);
int ShttsLine_ReadNext(char *dst, int size, char **cursor);
void ShttsText_ReadStdin(Shtts *engine);
void ShttsWord_ClearSyllable(ShttsSyllable *unk, Shtts *engine);
int ShttsUtil_FindString(const char *text, const char *find);
int ShttsMath_MultiplyFixed(int a, int b);
int ShttsEngine_EndThread(Shtts *engine);
int ShttsEngine_IsStopRequested(Shtts *engine);
char *ShttsUtil_NextToken(char **cursor, char *delimiters);
void ShttsUtil_FillBytes(char *dst, int c, int count);
void ShttsSynth_FitFrameGains(ShttsRunFrame *table, int count, int a, int b);
ShttsUnit *ShttsUnit_Find(int *index, Shtts *engine);
void ShttsOutput_Reset(Shtts *engine);
void ShttsOutput_Flush(Shtts *engine);
int ShttsEngine_SetSampleRate(Shtts *engine);
int ShttsParam_SetSpeed(Shtts *engine, int value);
int ShttsParam_SetPitch(Shtts *engine, int value);
int ShttsParam_SetVolume(Shtts *engine, int value);
int ShttsParam_SetWarp(Shtts *engine, int value);
int ShttsParam_SetRing(Shtts *engine, int value);
int ShttsParam_SetEchoDelay(Shtts *engine, int value);
int ShttsParam_SetEchoGain(Shtts *engine, int value);
int ShttsParam_SetFirDelay(Shtts *engine, int value);
int ShttsParam_SetFirGain(Shtts *engine, int value);
int ShttsParam_SetMode(Shtts *engine, int value);
int ShttsParam_SetChorus(Shtts *engine, int value);
int ShttsParam_SetAccent(Shtts *engine, int value);
int ShttsParam_SetPhrase(Shtts *engine, int value);
int ShttsParam_SetPitchTrans(Shtts *engine, int value);
int ShttsEffect_MultiplyDoubled(int a, int b);

// ---- From the decompilation of group d
// The three levels above ShttsSyllable. Each has a count of the ones below it, which are linked.
typedef struct ShttsPitchWord {
    /* 0x00 */ void *unk_00; // What ShttsWord_CountMorae takes
    /* 0x04 */ s8 count;
    /* 0x05 */ s8 unk_05;
    /* 0x06 */ s8 unk_06;
    /* 0x07 */ s8 unk_07;
    /* 0x08 */ u8 pad_08[0xA];
    /* 0x12 */ s16 unk_12;
    /* 0x14 */ u8 pad_14[4];
    /* 0x18 */ ShttsSyllable *first;
    /* 0x1C */ struct ShttsPitchWord *next;
} ShttsPitchWord;

typedef struct ShttsPitchAccentGroup {
    /* 0x0 */ s8 count;
    /* 0x1 */ s8 unk_1;
    /* 0x2 */ s8 unk_2;
    /* 0x4 */ ShttsPitchWord *first;
    /* 0x8 */ ShttsPitchWord *last;
    /* 0xC */ struct ShttsPitchAccentGroup *next;
} ShttsPitchAccentGroup; // 0x10

typedef struct ShttsPitchPhraseGroup {
    /* 0x0 */ s8 count;
    /* 0x1 */ s8 unk_1;
    /* 0x4 */ ShttsPitchAccentGroup *first; // They are also an array
    /* 0x8 */ struct ShttsPitchPhraseGroup *next;
} ShttsPitchPhraseGroup;

void ShttsSynth_ConvertLspToLpc(int order, u16 *in, s16 *out);

void ShttsSynth_NormalizeLevel(int level, s16 *in, s16 *out, int count);

int ShttsMath_ApproximateSqrt(int x);

void ShttsPitch_ShapeGroups(ShttsPitchPhraseGroup *list, Shtts *engine);

int ShttsPitch_AddPhraseCurve(ShttsPitchPhraseGroup *unk, int length, ShttsSyllable *last, int kind, s16 notFirst, Shtts *engine);

void ShttsPitch_AddAccent(ShttsPitchAccentGroup *unk, Shtts *engine);

s16 ShttsPitch_EvalPhraseCurve(int a, int b);

extern s16 *ShttsCosineTable;

extern s16 ShttsPhraseCurveX[5];

extern s16 ShttsPhraseCurveY[5];

extern s16 ShttsPhraseCurveDX[5];

// ---- From the decompilation of group e
// One sound of the text with its length and pitch, as ShttsUnit_SelectUnits reads them. The list ends with one of kind 7.
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

// One sound to synthesize, which ShttsUnit_AddSegments makes from a ShttsPhone and a ShttsUnit
typedef struct ShttsSegment {
    /* 0x0 */ int kind;
    /* 0x4 */ s16 length;
    /* 0x6 */ s16 unk_6;
    /* 0x8 */ s16 unk_8;
    /* 0xA */ s16 unk_A;
    /* 0xC */ u16 unit;  // The unit's unk_0, an index in ShttsUnitFrames
    /* 0xE */ u8 first;  // The first and the last of the unit's entries in ShttsUnitFrames that are this sound's
    /* 0xF */ u8 last;
} ShttsSegment; // 0x10

// What the pointers at 0x5D38 point to
typedef struct ShttsFrameRef {
    /* 0x0 */ u8 pad_0[3];
    /* 0x3 */ u8 unk_3;
    /* 0x4 */ int phoneme;
} ShttsFrameRef;

// One entry of the table at 0x020bc26c
typedef struct ShttsStoredFrame {
    /* 0x0 */ s8 unk_0;
    /* 0x1 */ s8 unk_1[5];
} ShttsStoredFrame; // 0x6

// A list that ShttsEngine_RunThread walks
typedef struct ShttsThreadAccentGroup {
    /* 0x0 */ u8 pad_0[8];
    /* 0x8 */ s8 *unk_8;
    /* 0xC */ u8 pad_C[4];
} ShttsThreadAccentGroup; // 0x10

typedef struct ShttsThreadPhraseGroup {
    /* 0x0 */ s8 count;
    /* 0x4 */ ShttsThreadAccentGroup *items;
    /* 0x8 */ struct ShttsThreadPhraseGroup *next;
} ShttsThreadPhraseGroup;

extern void *const ShttsFixedFrames[];

extern ShttsStoredFrame ShttsUnitFrames[];

void ShttsSynth_SynthesizeSegments(Shtts *engine);

void ShttsSynth_WritePitchRamp(s16 *out, s16 count, s16 target, s16 max, Shtts *engine);

void ShttsUnit_SelectUnits(ShttsPhone *phones, void *unused, Shtts *engine);

int ShttsUnit_AddSegments(int mode, ShttsPhone *first, ShttsPhone *last, void *unused, s16 unused2, Shtts *engine);

void ShttsOutput_Advance(int count, Shtts *engine);

void ShttsOutput_AddWave(int a, int b, Shtts *engine);

void ShttsSynth_SynthesizeFrames(ShttsFrameRef **units, s16 *pitches, s16 count, Shtts *engine);

void ShttsEngine_RunThread(void *arg);



#include "shtts_struct.h"

// ---- From the decompilation of group b
// One word of the text, with the sounds it is made of
typedef struct ShttsSpokenWord {
    /* 0x00 */ char *text;
    /* 0x04 */ s8 count; // How many sounds it has
    /* 0x05 */ s8 unk_05;
    /* 0x06 */ s8 unk_06;
    /* 0x07 */ s8 unk_07; // How many '?' it has, up to 3, and how many '!' in the next four bits
    /* 0x08 */ s8 unk_08;
    /* 0x09 */ s8 phraseStrength;
    /* 0x0A */ s16 speed;
    /* 0x0C */ s8 unk_0C;
    /* 0x0E */ s16 lineValue6;
    /* 0x10 */ u8 pad_10[2];
    /* 0x12 */ s16 pause;
    /* 0x14 */ s16 unk_14;
    /* 0x16 */ s16 volume;
    /* 0x18 */ ShttsSyllable *first; // Its first sound
    /* 0x1C */ struct ShttsSpokenWord *next;
} ShttsSpokenWord; // 0x20

// A run of words that ShttsWord_GroupWords makes
typedef struct ShttsAccentGroup {
    /* 0x0 */ s8 count; // How many words
    /* 0x1 */ s8 unk_1;
    /* 0x2 */ s8 unk_2;
    /* 0x4 */ ShttsSpokenWord *first;
    /* 0x8 */ ShttsSpokenWord *last;
    /* 0xC */ struct ShttsAccentGroup *next;
} ShttsAccentGroup; // 0x10

// A run of those runs
typedef struct ShttsPhraseGroup {
    /* 0x0 */ s8 count; // How many runs
    /* 0x1 */ s8 unk_1;
    /* 0x4 */ ShttsAccentGroup *first;
    /* 0x8 */ struct ShttsPhraseGroup *next;
} ShttsPhraseGroup; // 0xC

// Tables of numbers that ShttsDuration_SetWordSyllables adds up, each a run of smaller tables that one property of a sound indexes
extern const char ShttsSyllableVowelClass[0x90];

extern const char ShttsSyllableConsonantClass[0x90];

extern const char ShttsSyllableLengthKind[0x90];

typedef struct ShttsConsonantLengthTable {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02[28];
    /* 0x3A */ s16 byPrevVowel[27];
    /* 0x70 */ s16 byVowel[27];
    /* 0xA6 */ s16 byQMark[2];
    /* 0xAA */ s16 byTextPlace[4];
    /* 0xB2 */ s16 byRunPlace[65];
} ShttsConsonantLengthTable; // 0x134

typedef struct ShttsSyllableGainTable {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02[5];
    /* 0x0C */ s16 unk_0C[27];
    /* 0x42 */ s16 unk_42[47];
    /* 0xA0 */ s16 byMoraCount[28];
    /* 0xD8 */ s16 byTextPlace[9];
    /* 0xEA */ s16 byRunPlace[37];
} ShttsSyllableGainTable; // 0x134

typedef struct ShttsVowel5LengthTable {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02[27];
    /* 0x38 */ s16 byNext[64];
    /* 0xB8 */ s16 byTextPlace[5];
    /* 0xC2 */ s16 byRunPlace[63];
} ShttsVowel5LengthTable; // 0x140

typedef struct ShttsVowelLengthTable {
    /* 0x00 */ s16 unk_00;
    /* 0x02 */ s16 unk_02[5];
    /* 0x0C */ s16 unk_0C[27];
    /* 0x42 */ s16 unk_42[68];
    /* 0xCA */ s16 byTextPlace[4];
    /* 0xD2 */ s16 byRunPlace[97];
} ShttsVowelLengthTable; // 0x194

extern const ShttsSyllableGainTable ShttsSyllableGains;

extern const ShttsVowel5LengthTable ShttsVowel5Lengths;

extern const ShttsVowelLengthTable ShttsVowelLengths;

extern const ShttsConsonantLengthTable ShttsConsonantLengths[8];

extern const s16 ShttsEchoFilterCoefs[6];

extern const char ShttsRepeatedVowels[5][3];

extern const char ShttsSyllableNames[0x90][4];

void ShttsDuration_ApplySpeed(ShttsSyllable **list, char *kinds, int count, Shtts *engine);

int ShttsWord_SplitSyllables(ShttsSpokenWord *word, ShttsSyllable *sound, int max, Shtts *engine);

int ShttsWord_PrepareText(char *text, char *out, Shtts *engine);

int ShttsWord_ReadWords(ShttsSpokenWord *words, ShttsSyllable *sounds, Shtts *engine);
void ShttsWord_GroupWords(ShttsSpokenWord *word, ShttsAccentGroup *a, ShttsPhraseGroup *b);

#endif // SHTTS_H

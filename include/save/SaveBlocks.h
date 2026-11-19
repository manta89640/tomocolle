#ifndef SAVE_SAVEBLOCKS_H
#define SAVE_SAVEBLOCKS_H

#include <nitro.h>

#include "character/Character.h"

// The blocks of the save data (src/save/SaveBlocks.cpp), members of gGameData. Each is its data, as it is in the
// card's backup memory, followed by two flags: whether the data was read, and whether it was valid. The data
// starts with a 16-byte magic string and ends with a checksum (GameData::Checksum); reading a block checks both and
// takes the data only when they are right. Each block has its place in the backup memory, in each of two slots of
// 0x40000 bytes.
//
// The names are descriptive, not the original: no name of these classes survives in the ROM. The blocks whose use is
// not known yet are named by their order in the file.

// --- The header: the game's creation id, and settings and flags ---

struct SaveHeaderBody {
    /* 0x00 */ u8 mUnknown00[2];
    /* 0x02 */ u8 mUnknown02;    // A second of the clock, 0 to 3, when the header was made
    /* 0x03 */ u8 mUnknown03;
    /* 0x04 */ int mUnknown04;   // 5000 at first
    /* 0x08 */ u8 mUnknown08[0x34];
    /* 0x3C */ u8 mUnknown3C[6]; // GetUnknown3C and SetUnknown3C copy all six
    /* 0x42 */ u8 mUnknown42[2];
    /* 0x44 */ u8 mUnknown44[0x38];
    /* 0x7C */ u8 mUnknown7C;    // 1 at first
    /* 0x7D */ u8 mUnknown7D[3]; // 3 at first
    /* 0x80 */ u8 mUnknown80[6];
    /* 0x86 */ s8 mUnknown86[10][3]; // The first byte of each is -1 at first
    /* 0xA4 */ s8 mUnknownA4[10][3]; // The same
    /* 0xC2 */ u8 mUnknownC2[0xE];
    /* 0xD0 */ u8 mFlags[13];        // 100 flags, a bit each (TestFlag, SetFlag, ClearFlag)
    /* 0xDD */ u8 mUnknownDD[3];
}; // 0xE0

struct SaveHeaderData {
    /* 0x00 */ char mMagic[16]; // "TOMODACHI-FAIRY"
    /* 0x10 */ u8 mCreateId[8]; // The Mii library's creation id of this copy of the game (NFL_MakeCreateId)
    /* 0x18 */ int mUnknown18;  // 1 at first
    /* 0x1C */ SaveHeaderBody mBody;
    /* 0xFC */ u32 mChecksum;
}; // 0x100

class SaveHeader : public SaveHeaderData {
public:
    void Init();
    void Reset();
    int Load(int slot);
    void Save(int slot, MIDmaCallback callback);
    void GetUnknown3C(u8 *out);
    void SetUnknown3C(const u8 *values);
    u8 GetUnknown44(int index);
    void SetUnknown44(int index, u8 value);
    BOOL TestFlag(int flag);
    void SetFlag(int flag);
    void ClearFlag(int flag);

    /* 0x100 */ BOOL mLoaded; // The block was read and its magic string is right
    /* 0x104 */ BOOL mValid;  // and its checksum too
}; // 0x108

// --- A character: one of the hundred blocks of the residents ---

struct CharacterSaveBody {
    /* 0x000 */ CharacterRecord mRecord;
    /* 0x2A4 */ u8 mUnknown2A4[0x44];
}; // 0x2E8

struct CharacterSaveData {
    /* 0x000 */ char mMagic[16]; // "MaaMiiMuuMeeMoo"
    /* 0x010 */ CharacterSaveBody mBody;
    /* 0x2F8 */ u8 mUnknown2F8;  // 2 or 0 tell the save manager what to do with the character when it is read
    /* 0x2F9 */ u8 mUnknown2F9;
    /* 0x2FA */ u8 mUnknown2FA[2];
    /* 0x2FC */ u32 mChecksum;
}; // 0x300

class CharacterSave : public CharacterSaveData {
public:
    void Init(int index);
    void Reset();
    int Load(int slot);
    void Save(int slot, MIDmaCallback callback);
    BOOL GetUnknown050Bit0();
    u8 GetRoomNumber();
    u8 GetAffinity(int index);
    u8 GetLongActState();
    u8 GetLongActPartner();
    s64 GetLongActEnd();
    s64 GetTroubleEnd();
    s64 GetTroubleStart();
    u8 GetTroubleState();
    u8 GetTroubleProgress();
    u8 GetTroubleId();
    u8 GetTroubleCategory();
    u8 GetMood();
    int GetSex();
    u8 GetUnknown299();
    u8 GetSongSlot(int index);
    u8 GetRelationToPlayer();
    u32 GetBirthYear();
    int GetBirthMonth();
    int GetBirthDay();
    u8 GetUnknown24A();
    u8 GetUnknown24E();
    void CopyRecordTo(CharacterRecord *record);
    void CopyRecordFrom(const CharacterRecord *record);
    u8 GetUnknown2F8();
    void SetUnknown2F8(u8 value);
    int GetUnknown308();
    void SetUnknown308(int value);

    /* 0x300 */ BOOL mLoaded;
    /* 0x304 */ BOOL mValid;
    /* 0x308 */ int mUnknown308;
    /* 0x30C */ int mIndex; // Which of the hundred: where in the backup memory the block is
}; // 0x310

// --- The other blocks, whose use is not known yet ---

// Block 3 (0x3FD00 in each slot): 374 pairs of a value and a signed value, the second -1 at first.
struct SaveBlock3Entry {
    /* 0x0 */ u8 mValue;
    /* 0x1 */ s8 mSignedValue;
};

struct SaveBlock3Body {
    /* 0x000 */ SaveBlock3Entry mEntries[374];
}; // 0x2EC

struct SaveBlock3Data {
    /* 0x000 */ char mMagic[16]; // "KyotoOsakaNara", of which 15 characters are compared
    /* 0x010 */ SaveBlock3Body mBody;
    /* 0x2FC */ u32 mChecksum;
}; // 0x300

class SaveBlock3 : public SaveBlock3Data {
public:
    void Init();
    void Reset();
    int Load(int slot);
    void Save(int slot, MIDmaCallback callback);
    u8 GetValue(int index);
    s8 GetSignedValue(int index);
    void SetValue(int index, u8 value);
    void SetSignedValue(int index, s8 value);

    /* 0x300 */ BOOL mLoaded;
    /* 0x304 */ BOOL mValid;
}; // 0x308

// Block 4 (0x3FC00): 236 signed values, -1 at first; Get returns -1 for an index out of range.
struct SaveBlock4Body {
    /* 0x00 */ s8 mValues[236];
}; // 0xEC

struct SaveBlock4Data {
    /* 0x00 */ char mMagic[16]; // "BusGasExplosion"
    /* 0x10 */ SaveBlock4Body mBody;
    /* 0xFC */ u32 mChecksum;
}; // 0x100

class SaveBlock4 : public SaveBlock4Data {
public:
    void Init();
    void Reset();
    int Load(int slot);
    void Save(int slot, MIDmaCallback callback);
    s8 Get(int index);
    void Set(int index, s8 value);

    /* 0x100 */ BOOL mLoaded;
    /* 0x104 */ BOOL mValid;
}; // 0x108

// Block 5 (0x3FB00): 236 signed values, the first 78 -1 at first.
struct SaveBlock5Body {
    /* 0x00 */ s8 mValues[236];
}; // 0xEC

struct SaveBlock5Data {
    /* 0x00 */ char mMagic[16]; // "123571113171923"
    /* 0x10 */ SaveBlock5Body mBody;
    /* 0xFC */ u32 mChecksum;
}; // 0x100

class SaveBlock5 : public SaveBlock5Data {
public:
    void Init();
    void Reset();
    int Load(int slot);
    void Save(int slot, MIDmaCallback callback);
    s8 Get(int index);
    void Set(int index, s8 value);

    /* 0x100 */ BOOL mLoaded;
    /* 0x104 */ BOOL mValid;
}; // 0x108

// Block 6 (0x3FA00): four records of 0x34 bytes.
struct SaveBlock6Entry {
    /* 0x00 */ u32 mUnknown00[4];
    /* 0x10 */ u16 mUnknown10[5];
    /* 0x1A */ u16 mUnknown1A[5];
    /* 0x24 */ u16 mUnknown24[5];
    /* 0x2E */ u8 mUnknown2E[2];
    /* 0x30 */ u8 mUnknown30;
    /* 0x31 */ u8 mUnknown31;
    /* 0x32 */ u16 mUnknown32;
}; // 0x34

struct SaveBlock6Body {
    /* 0x00 */ SaveBlock6Entry mEntries[4];
    /* 0xD0 */ u8 mUnknownD0[0x1C];
}; // 0xEC

struct SaveBlock6Data {
    /* 0x00 */ char mMagic[16]; // "469121518212630"
    /* 0x10 */ SaveBlock6Body mBody;
    /* 0xFC */ u32 mChecksum;
}; // 0x100

class SaveBlock6 : public SaveBlock6Data {
public:
    void Init();
    void Reset();
    int Load(int slot);
    void Save(int slot, MIDmaCallback callback);
    void GetEntry(int index, SaveBlock6Entry *entry);
    void SetEntry(int index, const SaveBlock6Entry *entry);

    /* 0x100 */ BOOL mLoaded;
    /* 0x104 */ BOOL mValid;
}; // 0x108

// Block 7 (0x3F900): eleven records of 0x14 bytes.
struct SaveBlock7Entry {
    /* 0x00 */ u8 mUnknown00;
    /* 0x01 */ u8 mUnknown01[7];
    /* 0x08 */ u8 mUnknown08;
    /* 0x09 */ u8 mUnknown09[2];
    /* 0x0C */ int mUnknown0C;
    /* 0x10 */ u16 mUnknown10;
    /* 0x12 */ u8 mUnknown12[2];
}; // 0x14

struct SaveBlock7Body {
    /* 0x00 */ SaveBlock7Entry mEntries[11];
    /* 0xDC */ u8 mUnknownDC[0x10];
}; // 0xEC

struct SaveBlock7Data {
    /* 0x00 */ char mMagic[16]; // "N-EAST-WEST-S", of which 14 characters are copied
    /* 0x10 */ SaveBlock7Body mBody;
    /* 0xFC */ u32 mChecksum;
}; // 0x100

class SaveBlock7 : public SaveBlock7Data {
public:
    void Init();
    void Reset();
    int Load(int slot);
    void Save(int slot, MIDmaCallback callback);
    void GetEntry(int index, SaveBlock7Entry *entry);
    void SetEntry(int index, const SaveBlock7Entry *entry);

    /* 0x100 */ BOOL mLoaded;
    /* 0x104 */ BOOL mValid;
}; // 0x108

// Block 8 (0x800): a word and 232 bytes. Its Reset takes the checksum of the first 16 bytes only.
struct SaveBlock8Data {
    /* 0x00 */ char mMagic[16]; // "akstnhmyrwgzdbp"
    /* 0x10 */ u32 mUnknown10;
    /* 0x14 */ u8 mUnknown14[0xE8];
    /* 0xFC */ u32 mChecksum;
}; // 0x100

class SaveBlock8 : public SaveBlock8Data {
public:
    void Init();
    void Reset();
    int Load(int slot);
    void Save(int slot, MIDmaCallback callback);

    /* 0x100 */ BOOL mLoaded;
    /* 0x104 */ BOOL mValid;
}; // 0x108

// Block 9 (0x3DF00): 103 records of two strings of 16-bit characters (10 and 20 characters long, with their ends).
struct SaveBlock9Entry {
    /* 0x00 */ u16 mText0[11];
    /* 0x16 */ u16 mText1[21];
}; // 0x40

struct SaveBlock9Body {
    /* 0x0000 */ SaveBlock9Entry mEntries[103];
    /* 0x19C0 */ u16 mUnknown19C0[0x16];
}; // 0x19EC

struct SaveBlock9Data {
    /* 0x0000 */ char mMagic[16]; // "tsumanogosakata"
    /* 0x0010 */ SaveBlock9Body mBody;
    /* 0x19FC */ u32 mChecksum;
}; // 0x1A00

class SaveBlock9 : public SaveBlock9Data {
public:
    void Init();
    void Reset();
    int Load(int slot);
    void Save(int slot, MIDmaCallback callback);
    void GetEntry(int index, SaveBlock9Entry *entry);
    void SetEntry(int index, const SaveBlock9Entry *entry);

    /* 0x1A00 */ BOOL mLoaded;
    /* 0x1A04 */ BOOL mValid;
}; // 0x1A08

// Block 10 (0x3DC00): two lists of a hundred records of three bytes, the third -1 at first.
struct SaveBlock10Entry {
    /* 0x0 */ u8 mUnknown0;
    /* 0x1 */ u8 mUnknown1;
    /* 0x2 */ s8 mUnknown2;
};

struct SaveBlock10Body {
    /* 0x000 */ SaveBlock10Entry mEntries0[100];
    /* 0x12C */ SaveBlock10Entry mEntries1[100];
    /* 0x258 */ u8 mUnknown258[0x94];
}; // 0x2EC

struct SaveBlock10Data {
    /* 0x000 */ char mMagic[16]; // "aadeeiKlLnnrrst"
    /* 0x010 */ SaveBlock10Body mBody;
    /* 0x2FC */ u32 mChecksum;
}; // 0x300

class SaveBlock10 : public SaveBlock10Data {
public:
    void Init();
    void Reset();
    int Load(int slot);
    void Save(int slot, MIDmaCallback callback);

    /* 0x300 */ BOOL mLoaded;
    /* 0x304 */ BOOL mValid;
}; // 0x308

#endif // SAVE_SAVEBLOCKS_H

#include "save/SaveBlocks.h"

#include <string.h>

#include "GameData.h"

extern "C" {
// The Mii library and the Mii data (lib/NFL, lib/MiiData)
struct NFLCreateId;
BOOL NFL_MakeCreateId(NFLCreateId *id);
struct MiiData;
int MiiData_GetSex(MiiData *mii);
int MiiData_GetBirthMonth(MiiData *mii);
int MiiData_GetBirthDay(MiiData *mii);

// The pad input: reading it is turned off while the card is read
struct PadInputState {
    /* 0x00 */ u8 mUnknown00[0x14];
    /* 0x14 */ BOOL mEnabled;
};
extern PadInputState gPadInput;
}

// Where the blocks are in each slot of the backup memory
#define SLOT_SHIFT 18
#define CHARACTER_SAVE_OFFSET 0x1000
#define SAVE_BLOCK3_OFFSET 0x3FD00
#define SAVE_BLOCK4_OFFSET 0x3FC00
#define SAVE_BLOCK5_OFFSET 0x3FB00
#define SAVE_BLOCK6_OFFSET 0x3FA00
#define SAVE_BLOCK7_OFFSET 0x3F900
#define SAVE_BLOCK8_OFFSET 0x800
#define SAVE_BLOCK9_OFFSET 0x3DF00
#define SAVE_BLOCK10_OFFSET 0x3DC00

// The magic strings, declared in the order that puts them where the ROM has them (the compiler's size sort
// permutes objects of one size)
static const char sHeaderMagic[16] = "TOMODACHI-FAIRY";
static const char sBlock9Magic[16] = "tsumanogosakata";
static const char sCharacterMagic[16] = "MaaMiiMuuMeeMoo";
static const char sBlock4Magic[16] = "BusGasExplosion";
static const char sBlock5Magic[16] = "123571113171923";
static const char sBlock6Magic[16] = "469121518212630";
static const char sBlock8Magic[16] = "akstnhmyrwgzdbp";
static const char sBlock3Magic[16] = "KyotoOsakaNara";
static const char sBlock7Magic[16] = "N-EAST-WEST-S";
static const char sBlock10Magic[16] = "aadeeiKlLnnrrst";

// --- SaveHeader ---

void SaveHeader::Init() {
    Reset();
}

void SaveHeader::Reset() {
    MI_CpuClear16(this, sizeof(SaveHeaderData));
    strncpy(mMagic, sHeaderMagic, sizeof(mMagic));
    NFL_MakeCreateId((NFLCreateId *)mCreateId);
    mUnknown18 = 1;
    mChecksum = gGameData.Checksum(this, sizeof(SaveHeaderData));
    RTCTime time;
    RTC_GetTime(&time);
    mBody.mUnknown02 = time.second & 3;
    mBody.mUnknown04 = 5000;
    mBody.mUnknown7C = 1;
    mBody.mUnknown7D[0] = 3;
    mBody.mUnknown7D[1] = 3;
    mBody.mUnknown7D[2] = 3;
    for (int i = 0; i < 10; i++) {
        mBody.mUnknown86[i][0] = -1;
        mBody.mUnknownA4[i][0] = -1;
    }
    mLoaded = FALSE;
    mValid = FALSE;
}

int SaveHeader::Load(int slot) {
    SaveHeaderData buffer;
    u16 lockId = gGameData.mCardLockId;
    BOOL read;
    CARDResult code;
    int result = 0;
    gPadInput.mEnabled = FALSE;
    mLoaded = FALSE;
    CARD_LockBackup(lockId);
    read = CARD_ReadBackup((u32)slot << SLOT_SHIFT, &buffer, sizeof(buffer));
    code = CARD_GetResultCode();
    CARD_UnlockBackup(lockId);
    if (read) {
        SaveHeaderData *data = &buffer;
        if (strncmp(data->mMagic, sHeaderMagic, sizeof(data->mMagic)) == 0) {
            mLoaded = TRUE;
            if (data->mChecksum == gGameData.Checksum(data, sizeof(*data))) {
                mValid = TRUE;
                (SaveHeaderData &)*this = *data;
                result = 1;
            }
        }
    }
    gPadInput.mEnabled = TRUE;
    return read ? result : code + 16;
}

void SaveHeader::Save(int slot, MIDmaCallback callback) {
    if (slot > 1) {
        slot = 1;
    } else if (slot < 0) {
        slot = 0;
    }
    mChecksum = gGameData.Checksum(this, sizeof(SaveHeaderData));
    CARD_WriteAndVerifyBackupAsync((u32)slot << SLOT_SHIFT, this, sizeof(SaveHeaderData), callback, NULL);
    mLoaded = TRUE;
    mValid = TRUE;
}

void SaveHeader::GetUnknown3C(u8 *out) {
    for (int i = 0; i < 6; i++) {
        out[i] = mBody.mUnknown3C[i];
    }
}

void SaveHeader::SetUnknown3C(const u8 *values) {
    for (int i = 0; i < 6; i++) {
        mBody.mUnknown3C[i] = values[i];
    }
}

u8 SaveHeader::GetUnknown44(int index) {
    return mBody.mUnknown44[index];
}

void SaveHeader::SetUnknown44(int index, u8 value) {
    mBody.mUnknown44[index] = value;
}

BOOL SaveHeader::TestFlag(int flag) {
    if (flag > 99) {
        flag = 99;
    } else if (flag < 0) {
        flag = 0;
    }
    if (mBody.mFlags[flag / 8] & (u8)(1 << (flag & 7))) {
        return TRUE;
    }
    return FALSE;
}

void SaveHeader::SetFlag(int flag) {
    if (flag > 99) {
        flag = 99;
    } else if (flag < 0) {
        flag = 0;
    }
    mBody.mFlags[flag / 8] |= (u8)(1 << (flag & 7));
}

void SaveHeader::ClearFlag(int flag) {
    if (flag > 99) {
        flag = 99;
    } else if (flag < 0) {
        flag = 0;
    }
    mBody.mFlags[flag / 8] &= (u8)((u8)(1 << (flag & 7)) ^ 0xFF);
}

// --- CharacterSave ---

void CharacterSave::Init(int index) {
    Reset();
    mIndex = index;
}

void CharacterSave::Reset() {
    MI_CpuClear16(this, sizeof(CharacterSaveData));
    strncpy(mMagic, sCharacterMagic, sizeof(mMagic));
    mChecksum = gGameData.Checksum(this, sizeof(CharacterSaveData));
    mLoaded = FALSE;
    mValid = FALSE;
}

int CharacterSave::Load(int slot) {
    CharacterSaveData buffer;
    u16 lockId = gGameData.mCardLockId;
    BOOL read;
    CARDResult code;
    int result = 0;
    u32 offset = mIndex * sizeof(CharacterSaveData) + CHARACTER_SAVE_OFFSET;
    gPadInput.mEnabled = FALSE;
    mLoaded = FALSE;
    CARD_LockBackup(lockId);
    read = CARD_ReadBackup(offset + ((u32)slot << SLOT_SHIFT), &buffer, sizeof(buffer));
    code = CARD_GetResultCode();
    CARD_UnlockBackup(lockId);
    if (read) {
        CharacterSaveData *data = &buffer;
        if (strncmp(data->mMagic, sCharacterMagic, sizeof(data->mMagic)) == 0) {
            mLoaded = TRUE;
            if (data->mChecksum == gGameData.Checksum(data, sizeof(*data))) {
                mValid = TRUE;
                (CharacterSaveData &)*this = *data;
                result = 1;
            }
        }
    }
    gPadInput.mEnabled = TRUE;
    return read ? result : code + 16;
}

void CharacterSave::Save(int slot, MIDmaCallback callback) {
    u32 offset = mIndex * sizeof(CharacterSaveData) + CHARACTER_SAVE_OFFSET;
    if (slot < 0) {
        slot = 0;
    }
    if (slot >= 2) {
        slot = 1;
    }
    mChecksum = gGameData.Checksum(this, sizeof(CharacterSaveData));
    CARD_WriteAndVerifyBackupAsync(offset + ((u32)slot << SLOT_SHIFT), this, sizeof(CharacterSaveData), callback,
                                   NULL);
    mLoaded = TRUE;
    mValid = TRUE;
}

BOOL CharacterSave::GetUnknown050Bit0() {
    return mBody.mRecord.mUnknown050Bit0;
}

u8 CharacterSave::GetRoomNumber() {
    return mBody.mRecord.mRoomNumber;
}

u8 CharacterSave::GetAffinity(int index) {
    return mBody.mRecord.mRelationMeter[index];
}

u8 CharacterSave::GetEvent() {
    return mBody.mRecord.mLongActState;
}

u8 CharacterSave::GetEventPartner() {
    return mBody.mRecord.mLongActPartner;
}

s64 CharacterSave::GetEventEnd() {
    return mBody.mRecord.mLongActEnd;
}

s64 CharacterSave::GetAwayUntil() {
    return mBody.mRecord.mTroubleEnd;
}

s64 CharacterSave::GetUnknown228() {
    return mBody.mRecord.mTroubleStart;
}

u8 CharacterSave::GetMoodLocked() {
    return mBody.mRecord.mTroubleState;
}

u8 CharacterSave::GetUnknown23B() {
    return mBody.mRecord.mTroubleProgress;
}

u8 CharacterSave::GetUnknown226() {
    return mBody.mRecord.mTroubleId;
}

u8 CharacterSave::GetUnknown227() {
    return mBody.mRecord.mTroubleCategory;
}

u8 CharacterSave::GetMood() {
    return mBody.mRecord.mMood;
}

int CharacterSave::GetSex() {
    return MiiData_GetSex((MiiData *)mBody.mRecord.mMii);
}

u8 CharacterSave::GetUnknown299() {
    return mBody.mRecord.mUnknown299;
}

u8 CharacterSave::GetSongSlot(int index) {
    return mBody.mRecord.mSongSlots[index];
}

u8 CharacterSave::GetRelationToPlayer() {
    return mBody.mRecord.mRelationBlood;
}

u32 CharacterSave::GetBirthYear() {
    return mBody.mRecord.mBirthYear;
}

int CharacterSave::GetBirthMonth() {
    return MiiData_GetBirthMonth((MiiData *)mBody.mRecord.mMii);
}

int CharacterSave::GetBirthDay() {
    return MiiData_GetBirthDay((MiiData *)mBody.mRecord.mMii);
}

u8 CharacterSave::GetUnknown24A() {
    return mBody.mRecord.mUnknown24A;
}

u8 CharacterSave::GetUnknown24E() {
    return mBody.mRecord.mUnknown24E;
}

void CharacterSave::CopyRecordTo(CharacterRecord *record) {
    MI_CpuCopyFast(&mBody.mRecord, record, sizeof(CharacterRecord));
}

void CharacterSave::CopyRecordFrom(const CharacterRecord *record) {
    MI_CpuCopyFast(record, &mBody.mRecord, sizeof(CharacterRecord));
}

u8 CharacterSave::GetUnknown2F8() {
    return mUnknown2F8;
}

void CharacterSave::SetUnknown2F8(u8 value) {
    mUnknown2F8 = value;
}

int CharacterSave::GetUnknown308() {
    return mUnknown308;
}

void CharacterSave::SetUnknown308(int value) {
    mUnknown308 = value;
}

// --- SaveBlock3 ---

void SaveBlock3::Init() {
    Reset();
}

void SaveBlock3::Reset() {
    MI_CpuClear16(this, sizeof(SaveBlock3Data));
    for (int i = 0; i < 374; i++) {
        mBody.mEntries[i].mSignedValue = -1;
    }
    strncpy(mMagic, sBlock3Magic, 15);
    mChecksum = gGameData.Checksum(this, sizeof(SaveBlock3Data));
    mLoaded = FALSE;
    mValid = FALSE;
}

int SaveBlock3::Load(int slot) {
    SaveBlock3Data buffer;
    u16 lockId = gGameData.mCardLockId;
    BOOL read;
    CARDResult code;
    int result = 0;
    gPadInput.mEnabled = FALSE;
    mLoaded = FALSE;
    CARD_LockBackup(lockId);
    read = CARD_ReadBackup(SAVE_BLOCK3_OFFSET + ((u32)slot << SLOT_SHIFT), &buffer, sizeof(buffer));
    code = CARD_GetResultCode();
    CARD_UnlockBackup(lockId);
    if (read) {
        SaveBlock3Data *data = &buffer;
        if (strncmp(data->mMagic, sBlock3Magic, 15) == 0) {
            mLoaded = TRUE;
            if (data->mChecksum == gGameData.Checksum(data, sizeof(*data))) {
                mValid = TRUE;
                (SaveBlock3Data &)*this = *data;
                result = 1;
            }
        }
    }
    gPadInput.mEnabled = TRUE;
    return read ? result : code + 16;
}

void SaveBlock3::Save(int slot, MIDmaCallback callback) {
    if (slot > 1) {
        slot = 1;
    } else if (slot < 0) {
        slot = 0;
    }
    mChecksum = gGameData.Checksum(this, sizeof(SaveBlock3Data));
    CARD_WriteAndVerifyBackupAsync(SAVE_BLOCK3_OFFSET + ((u32)slot << SLOT_SHIFT), this, sizeof(SaveBlock3Data), callback, NULL);
    mLoaded = TRUE;
    mValid = TRUE;
}

u8 SaveBlock3::GetValue(int index) {
    return mBody.mEntries[index].mValue;
}

s8 SaveBlock3::GetSignedValue(int index) {
    return mBody.mEntries[index].mSignedValue;
}

void SaveBlock3::SetValue(int index, u8 value) {
    mBody.mEntries[index].mValue = value;
}

void SaveBlock3::SetSignedValue(int index, s8 value) {
    mBody.mEntries[index].mSignedValue = value;
}

// --- SaveBlock4 ---

void SaveBlock4::Init() {
    Reset();
}

void SaveBlock4::Reset() {
    MI_CpuClear16(this, sizeof(SaveBlock4Data));
    for (int i = 0; i < 236; i++) {
        mBody.mValues[i] = -1;
    }
    strncpy(mMagic, sBlock4Magic, 16);
    mChecksum = gGameData.Checksum(this, sizeof(SaveBlock4Data));
    mLoaded = FALSE;
    mValid = FALSE;
}

int SaveBlock4::Load(int slot) {
    SaveBlock4Data buffer;
    u16 lockId = gGameData.mCardLockId;
    BOOL read;
    CARDResult code;
    int result = 0;
    gPadInput.mEnabled = FALSE;
    mLoaded = FALSE;
    CARD_LockBackup(lockId);
    read = CARD_ReadBackup(SAVE_BLOCK4_OFFSET + ((u32)slot << SLOT_SHIFT), &buffer, sizeof(buffer));
    code = CARD_GetResultCode();
    CARD_UnlockBackup(lockId);
    if (read) {
        SaveBlock4Data *data = &buffer;
        if (strncmp(data->mMagic, sBlock4Magic, 16) == 0) {
            mLoaded = TRUE;
            if (data->mChecksum == gGameData.Checksum(data, sizeof(*data))) {
                mValid = TRUE;
                (SaveBlock4Data &)*this = *data;
                result = 1;
            }
        }
    }
    gPadInput.mEnabled = TRUE;
    return read ? result : code + 16;
}

void SaveBlock4::Save(int slot, MIDmaCallback callback) {
    mChecksum = gGameData.Checksum(this, sizeof(SaveBlock4Data));
    CARD_WriteAndVerifyBackupAsync(SAVE_BLOCK4_OFFSET + ((u32)slot << SLOT_SHIFT), this, sizeof(SaveBlock4Data), callback, NULL);
    mLoaded = TRUE;
    mValid = TRUE;
}

s8 SaveBlock4::Get(int index) {
    if (index < 236) {
        return mBody.mValues[index];
    }
    return -1;
}

void SaveBlock4::Set(int index, s8 value) {
    if (index < 236) {
        mBody.mValues[index] = value;
    }
}

// --- SaveBlock5 ---

void SaveBlock5::Init() {
    Reset();
}

void SaveBlock5::Reset() {
    MI_CpuClear16(this, sizeof(SaveBlock5Data));
    for (int i = 0; i < 78; i++) {
        mBody.mValues[i] = -1;
    }
    strncpy(mMagic, sBlock5Magic, 16);
    mChecksum = gGameData.Checksum(this, sizeof(SaveBlock5Data));
    mLoaded = FALSE;
    mValid = FALSE;
}

int SaveBlock5::Load(int slot) {
    SaveBlock5Data buffer;
    u16 lockId = gGameData.mCardLockId;
    BOOL read;
    CARDResult code;
    int result = 0;
    gPadInput.mEnabled = FALSE;
    mLoaded = FALSE;
    CARD_LockBackup(lockId);
    read = CARD_ReadBackup(SAVE_BLOCK5_OFFSET + ((u32)slot << SLOT_SHIFT), &buffer, sizeof(buffer));
    code = CARD_GetResultCode();
    CARD_UnlockBackup(lockId);
    if (read) {
        SaveBlock5Data *data = &buffer;
        if (strncmp(data->mMagic, sBlock5Magic, 16) == 0) {
            mLoaded = TRUE;
            if (data->mChecksum == gGameData.Checksum(data, sizeof(*data))) {
                mValid = TRUE;
                (SaveBlock5Data &)*this = *data;
                result = 1;
            }
        }
    }
    gPadInput.mEnabled = TRUE;
    return read ? result : code + 16;
}

void SaveBlock5::Save(int slot, MIDmaCallback callback) {
    mChecksum = gGameData.Checksum(this, sizeof(SaveBlock5Data));
    CARD_WriteAndVerifyBackupAsync(SAVE_BLOCK5_OFFSET + ((u32)slot << SLOT_SHIFT), this, sizeof(SaveBlock5Data), callback, NULL);
    mLoaded = TRUE;
    mValid = TRUE;
}

s8 SaveBlock5::Get(int index) {
    return mBody.mValues[index];
}

void SaveBlock5::Set(int index, s8 value) {
    mBody.mValues[index] = value;
}

// --- SaveBlock6 ---

void SaveBlock6::Init() {
    Reset();
}

void SaveBlock6::Reset() {
    MI_CpuClear16(this, sizeof(SaveBlock6Data));
    strncpy(mMagic, sBlock6Magic, 16);
    mChecksum = gGameData.Checksum(this, sizeof(SaveBlock6Data));
    mLoaded = FALSE;
    mValid = FALSE;
}

int SaveBlock6::Load(int slot) {
    SaveBlock6Data buffer;
    u16 lockId = gGameData.mCardLockId;
    BOOL read;
    CARDResult code;
    int result = 0;
    gPadInput.mEnabled = FALSE;
    mLoaded = FALSE;
    CARD_LockBackup(lockId);
    read = CARD_ReadBackup(SAVE_BLOCK6_OFFSET + ((u32)slot << SLOT_SHIFT), &buffer, sizeof(buffer));
    code = CARD_GetResultCode();
    CARD_UnlockBackup(lockId);
    if (read) {
        SaveBlock6Data *data = &buffer;
        if (strncmp(data->mMagic, sBlock6Magic, 16) == 0) {
            mLoaded = TRUE;
            if (data->mChecksum == gGameData.Checksum(data, sizeof(*data))) {
                mValid = TRUE;
                (SaveBlock6Data &)*this = *data;
                result = 1;
            }
        }
    }
    gPadInput.mEnabled = TRUE;
    return read ? result : code + 16;
}

void SaveBlock6::Save(int slot, MIDmaCallback callback) {
    mChecksum = gGameData.Checksum(this, sizeof(SaveBlock6Data));
    CARD_WriteAndVerifyBackupAsync(SAVE_BLOCK6_OFFSET + ((u32)slot << SLOT_SHIFT), this, sizeof(SaveBlock6Data), callback, NULL);
    mLoaded = TRUE;
    mValid = TRUE;
}

void SaveBlock6::GetEntry(int index, SaveBlock6Entry *entry) {
    *entry = mBody.mEntries[index];
}

void SaveBlock6::SetEntry(int index, const SaveBlock6Entry *entry) {
    mBody.mEntries[index] = *entry;
}

// --- SaveBlock7 ---

void SaveBlock7::Init() {
    Reset();
}

void SaveBlock7::Reset() {
    MI_CpuClear16(this, sizeof(SaveBlock7Data));
    strncpy(mMagic, sBlock7Magic, 14);
    mChecksum = gGameData.Checksum(this, sizeof(SaveBlock7Data));
    mLoaded = FALSE;
    mValid = FALSE;
}

int SaveBlock7::Load(int slot) {
    SaveBlock7Data buffer;
    u16 lockId = gGameData.mCardLockId;
    BOOL read;
    CARDResult code;
    int result = 0;
    gPadInput.mEnabled = FALSE;
    mLoaded = FALSE;
    CARD_LockBackup(lockId);
    read = CARD_ReadBackup(SAVE_BLOCK7_OFFSET + ((u32)slot << SLOT_SHIFT), &buffer, sizeof(buffer));
    code = CARD_GetResultCode();
    CARD_UnlockBackup(lockId);
    if (read) {
        SaveBlock7Data *data = &buffer;
        if (strncmp(data->mMagic, sBlock7Magic, 14) == 0) {
            mLoaded = TRUE;
            if (data->mChecksum == gGameData.Checksum(data, sizeof(*data))) {
                mValid = TRUE;
                (SaveBlock7Data &)*this = *data;
                result = 1;
            }
        }
    }
    gPadInput.mEnabled = TRUE;
    return read ? result : code + 16;
}

void SaveBlock7::Save(int slot, MIDmaCallback callback) {
    mChecksum = gGameData.Checksum(this, sizeof(SaveBlock7Data));
    CARD_WriteAndVerifyBackupAsync(SAVE_BLOCK7_OFFSET + ((u32)slot << SLOT_SHIFT), this, sizeof(SaveBlock7Data), callback, NULL);
    mLoaded = TRUE;
    mValid = TRUE;
}

void SaveBlock7::GetEntry(int index, SaveBlock7Entry *entry) {
    *entry = mBody.mEntries[index];
}

void SaveBlock7::SetEntry(int index, const SaveBlock7Entry *entry) {
    mBody.mEntries[index] = *entry;
}

// --- SaveBlock8 ---

void SaveBlock8::Init() {
    Reset();
}

void SaveBlock8::Reset() {
    MI_CpuClear16(this, sizeof(SaveBlock8Data));
    strncpy(mMagic, sBlock8Magic, 16);
    mChecksum = gGameData.Checksum(this, sizeof(mMagic));
    mLoaded = FALSE;
    mValid = FALSE;
}

int SaveBlock8::Load(int slot) {
    SaveBlock8Data buffer;
    u16 lockId = gGameData.mCardLockId;
    BOOL read;
    CARDResult code;
    int result = 0;
    gPadInput.mEnabled = FALSE;
    mLoaded = FALSE;
    CARD_LockBackup(lockId);
    read = CARD_ReadBackup(SAVE_BLOCK8_OFFSET + ((u32)slot << SLOT_SHIFT), &buffer, sizeof(buffer));
    code = CARD_GetResultCode();
    CARD_UnlockBackup(lockId);
    if (read) {
        SaveBlock8Data *data = &buffer;
        if (strncmp(data->mMagic, sBlock8Magic, 16) == 0) {
            mLoaded = TRUE;
            if (data->mChecksum == gGameData.Checksum(data, sizeof(*data))) {
                mValid = TRUE;
                (SaveBlock8Data &)*this = *data;
                result = 1;
            }
        }
    }
    gPadInput.mEnabled = TRUE;
    return read ? result : code + 16;
}

void SaveBlock8::Save(int slot, MIDmaCallback callback) {
    mChecksum = gGameData.Checksum(this, sizeof(SaveBlock8Data));
    CARD_WriteAndVerifyBackupAsync(SAVE_BLOCK8_OFFSET + ((u32)slot << SLOT_SHIFT), this, sizeof(SaveBlock8Data), callback, NULL);
    mLoaded = TRUE;
    mValid = TRUE;
}

// --- SaveBlock9 ---

void SaveBlock9::Init() {
    Reset();
}

void SaveBlock9::Reset() {
    MI_CpuClear16(this, sizeof(SaveBlock9Data));
    strncpy(mMagic, sBlock9Magic, 16);
    mChecksum = gGameData.Checksum(this, sizeof(SaveBlock9Data));
    mLoaded = FALSE;
    mValid = FALSE;
}

int SaveBlock9::Load(int slot) {
    SaveBlock9Data buffer;
    u16 lockId = gGameData.mCardLockId;
    BOOL read;
    CARDResult code;
    int result = 0;
    gPadInput.mEnabled = FALSE;
    mLoaded = FALSE;
    CARD_LockBackup(lockId);
    read = CARD_ReadBackup(SAVE_BLOCK9_OFFSET + ((u32)slot << SLOT_SHIFT), &buffer, sizeof(buffer));
    code = CARD_GetResultCode();
    CARD_UnlockBackup(lockId);
    if (read) {
        SaveBlock9Data *data = &buffer;
        if (strncmp(data->mMagic, sBlock9Magic, 16) == 0) {
            mLoaded = TRUE;
            if (data->mChecksum == gGameData.Checksum(data, sizeof(*data))) {
                mValid = TRUE;
                (SaveBlock9Data &)*this = *data;
                result = 1;
            }
        }
    }
    gPadInput.mEnabled = TRUE;
    return read ? result : code + 16;
}

void SaveBlock9::Save(int slot, MIDmaCallback callback) {
    mChecksum = gGameData.Checksum(this, sizeof(SaveBlock9Data));
    CARD_WriteAndVerifyBackupAsync(SAVE_BLOCK9_OFFSET + ((u32)slot << SLOT_SHIFT), this, sizeof(SaveBlock9Data), callback, NULL);
    mLoaded = TRUE;
    mValid = TRUE;
}

void SaveBlock9::GetEntry(int index, SaveBlock9Entry *entry) {
    *entry = mBody.mEntries[index];
}

void SaveBlock9::SetEntry(int index, const SaveBlock9Entry *entry) {
    mBody.mEntries[index] = *entry;
}

// --- SaveBlock10 ---

void SaveBlock10::Init() {
    Reset();
}

void SaveBlock10::Reset() {
    MI_CpuClear16(this, sizeof(SaveBlock10Data));
    strncpy(mMagic, sBlock10Magic, 16);
    for (int i = 0; i < 100; i++) {
        mBody.mEntries0[i].mUnknown2 = -1;
        mBody.mEntries1[i].mUnknown2 = -1;
    }
    mChecksum = gGameData.Checksum(this, sizeof(SaveBlock10Data));
    mLoaded = FALSE;
    mValid = FALSE;
}

int SaveBlock10::Load(int slot) {
    SaveBlock10Data buffer;
    u16 lockId = gGameData.mCardLockId;
    BOOL read;
    CARDResult code;
    int result = 0;
    gPadInput.mEnabled = FALSE;
    mLoaded = FALSE;
    CARD_LockBackup(lockId);
    read = CARD_ReadBackup(SAVE_BLOCK10_OFFSET + ((u32)slot << SLOT_SHIFT), &buffer, sizeof(buffer));
    code = CARD_GetResultCode();
    CARD_UnlockBackup(lockId);
    if (read) {
        SaveBlock10Data *data = &buffer;
        if (strncmp(data->mMagic, sBlock10Magic, 16) == 0) {
            mLoaded = TRUE;
            if (data->mChecksum == gGameData.Checksum(data, sizeof(*data))) {
                mValid = TRUE;
                (SaveBlock10Data &)*this = *data;
                result = 1;
            }
        }
    }
    gPadInput.mEnabled = TRUE;
    return read ? result : code + 16;
}

void SaveBlock10::Save(int slot, MIDmaCallback callback) {
    mChecksum = gGameData.Checksum(this, sizeof(SaveBlock10Data));
    CARD_WriteAndVerifyBackupAsync(SAVE_BLOCK10_OFFSET + ((u32)slot << SLOT_SHIFT), this, sizeof(SaveBlock10Data), callback, NULL);
    mLoaded = TRUE;
    mValid = TRUE;
}


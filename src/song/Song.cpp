#include <nitro.h>

#include "GameData.h"
#include "song/Song.h"

// The parts of a slot's table (data_020f9414: 0xE0 bytes a slot, eight parts of 0x1C)
struct SongPart {
    /* 0x00 */ u8 mUnknown00[4];
    /* 0x04 */ u8 mLength;
    /* 0x05 */ u8 mValues[0x17];
}; // 0x1C

struct SongSlotTable {
    SongPart mParts[8];
}; // 0xE0

extern "C" {
extern const u16 data_020f9408[];
extern const SongSlotTable data_020f9414[];
void GameData_GetSong(GameData *game, u8 song, SongEntry *entry);
void GameData_SetSong(GameData *game, u8 song, SongEntry *entry);
}

SongEntry::SongEntry() {
    Clear();
}

void SongEntry::Save(u8 song) {
    GameData_SetSong(&gGameData, song, this);
}

void SongEntry::Load(u8 song) {
    GameData_GetSong(&gGameData, song, this);
}

void SongEntry::Clear() {
    mSong = 0xFF;
    MI_CpuFill8(this, 0, 0x40);
}

BOOL SongEntry::SetNotesFromSlot(int value, int slot) {
    BOOL changed = FALSE;
    for (int i = 0; i < 21; i++) {
        SongNote *note = &mNotes[i];
        if ((u32)(note->mKind - 1) <= 3 && (note->mValue == value || value == -1)) {
            int part;
            int offset;
            FindPart(slot, i, &part, &offset);
            u16 newValue = data_020f9408[data_020f9414[slot].mParts[part].mValues[offset]];
            note->mKind = 0;
            note->mValue = newValue;
            changed = TRUE;
        }
    }
    return changed;
}

void SongEntry::FindPart(int slot, int index, int *part, int *offset) {
    int total = 0;
    for (int i = 0; i < 8; i++) {
        total += data_020f9414[slot].mParts[i].mLength;
    }
    if (index >= total) {
        *part = -1;
        *offset = -1;
        return;
    }
    for (int i = 0; i < 8; i++) {
        int rest = index - data_020f9414[slot].mParts[i].mLength;
        if (rest >= 0) {
            index = rest;
        } else {
            *part = i;
            break;
        }
    }
    *offset = index;
}

#ifndef SONG_SONG_H
#define SONG_SONG_H

#include <nitro.h>

// A song as the saved game keeps it (0x40 bytes, func_0202c920 copies one out of the saved game, func_0202c934 in),
// with the number it is kept under. Overlays 92 and 109 use it.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.

// One of the song's 21 notes (u16 at 0x16 on)
struct SongNote {
    u16 mKind : 3;   // 1 to 4 for the notes SetNotesFromSlot changes
    u16 mValue : 9;
    u16 mRest : 4;
};

class SongEntry {
public:
    SongEntry();
    void Save(u8 song);
    void Load(u8 song);
    void Clear();
    BOOL SetNotesFromSlot(int value, int slot);
    void FindPart(int slot, int index, int *part, int *offset);

    /* 0x00 */ u32 mUnknown00[5];
    /* 0x14 */ u16 mUnknown14;
    /* 0x16 */ SongNote mNotes[21];
    /* 0x40 */ u8 mSong; // The number it is kept under in the saved game, 0xFF for none
}; // 0x44

#endif // SONG_SONG_H

#ifndef TEXT_MESSAGEFILE_H
#define TEXT_MESSAGEFILE_H

#include <nitro.h>
#include <nnsys.h>

// One of the game's message files, in Nintendo's BMG format ("BMG:/tmd_msg_JP.bmg"): it finds the file's blocks
// and gives out messages and their layout attributes by message number.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class MessageFile {
public:
    // The message number that stands for "no message"
    enum { NO_MESSAGE = 10000001 };

    // What the file's INF1 block holds for each message
    struct Info {
        /* 0x0 */ u32 mTextOffset; // Into the DAT1 block
        /* 0x4 */ u16 mUnknown4;
        /* 0x6 */ u16 mWidth;      // In pixels: MessageText rounds it up to 8-pixel cells
        /* 0x8 */ u8 mUnknown8;
        /* 0x9 */ u8 mAlignment;   // MessageText keeps it as its horizontal alignment
        /* 0xA */ u8 mUnknownA;
        /* 0xB */ u8 mLineCount;   // MessageText multiplies it by the line height
    }; // 0xC

    struct BlockHeader {
        /* 0x0 */ u32 mKind;
        /* 0x4 */ u32 mSize;
    };

    struct FileHeader {
        /* 0x00 */ char mSignature[8];
        /* 0x08 */ u32 mFileSize;
        /* 0x0C */ u32 mBlockCount;
        /* 0x10 */ u8 mUnknown10[0x10];
    }; // 0x20

    struct InfoBlock {
        /* 0x0 */ u16 mCount;
        /* 0x2 */ u16 mEntrySize;
        /* 0x4 */ u32 mUnknown4;
        /* 0x8 */ Info mEntries[1];
    };

    struct IdBlock {
        /* 0x0 */ u16 mCount;
    };

    MessageFile();
    ~MessageFile();

    void Open(const char *path);
    const wchar_t *GetText(u32 message);
    u16 GetWidth(u32 message);
    u8 GetAlignment(u32 message);
    u8 GetLineCount(u32 message);
    void Load(int language);

    /* 0x00 */ u8 *mFile;
    /* 0x04 */ FileHeader *mHeader;
    /* 0x08 */ InfoBlock *mInfo;    // INF1
    /* 0x0C */ u8 *mText;           // DAT1
    /* 0x10 */ void *mStrings;      // STR1
    /* 0x14 */ IdBlock *mIds;       // MID1
}; // 0x18

extern MessageFile gMessageFiles[1];

#endif // TEXT_MESSAGEFILE_H

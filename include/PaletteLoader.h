#ifndef PALETTELOADER_H
#define PALETTELOADER_H

#include <nitro.h>
#include <nnsys.h>

// Queues palettes to be copied to palette memory, and copies them at the next V-blank (PaletteLoader_Transfer, from
// main()): standard and extended palettes of both screens' backgrounds and objects, from NCLR files of the mounted
// archives or from memory, and the two backdrop colours.
//
// The name is descriptive, not the original (the overlays' stand-ins use it too): no name of this class survives
// in the ROM. Its functions are C functions on it, as the sources of the scenes and overlays declare them.
class PaletteLoader {
public:
    PaletteLoader() : mCount(0) {}

    // A copy to make: to where (an index of the file's table of loaders: 0 to 3 the main screen's backgrounds, 4 to
    // 7 the sub screen's, 8 and 9 the objects', plus 10 for the extended palettes), from where, how much
    struct Entry {
        /* 0x0 */ int mType;
        /* 0x4 */ const void *mSource;
        /* 0x8 */ u32 mOffset;
        /* 0xC */ u32 mSize;
    };

    /* 0x000 */ Entry mEntries[64];
    /* 0x400 */ int mCount;
    /* 0x404 */ GXRgb mMainBackdrop;
    /* 0x408 */ BOOL mMainBackdropChanged;
    /* 0x40C */ GXRgb mSubBackdrop;
    /* 0x410 */ BOOL mSubBackdropChanged;
    // What PaletteLoader_LoadFile uses for an argument of -1
    /* 0x414 */ int mDefaultOffset;
    /* 0x418 */ int mDefaultSize;
    /* 0x41C */ int mDefaultUnknown;
}; // 0x420

extern "C" {
extern PaletteLoader gPaletteLoader;

void PaletteLoader_Init(PaletteLoader *loader);
// Loads the palette of an NCLR file and hands it to the graphics manager for the given planes; -1 for offset, size
// or the last argument takes the loader's default
void *PaletteLoader_LoadFile(PaletteLoader *loader, const char *name, int planes, int offset, int size, int unknown);
BOOL PaletteLoader_ReadFile(PaletteLoader *loader, const char *name, NNSG2dPaletteData **palette);
// Queues a copy of colours to each plane in planes, to the extended palettes if extended
void PaletteLoader_QueueColors(PaletteLoader *loader, const void *colors, u32 planes, u32 offset, u32 size, BOOL extended);
// Sets the backdrop colour of the screens that planes names (0x3F: the main one, 0x3F00: the sub one)
void PaletteLoader_SetBackdrop(PaletteLoader *loader, GXRgb color, u32 planes);
void PaletteLoader_Transfer(PaletteLoader *loader);
// Clears the defaults
void PaletteLoader_ClearDefaults(PaletteLoader *loader);
}

#endif // PALETTELOADER_H

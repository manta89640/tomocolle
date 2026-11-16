#include "PaletteLoader.h"

struct ArchiveLoader;
struct GraphicsManager;

extern "C" {
extern ArchiveLoader gArchiveManager;
extern GraphicsManager gGraphicsManager;
void *ArchiveManager_GetFileByName(ArchiveLoader *loader, const char *path);
void *func_02025240(GraphicsManager *manager, NNSG2dPaletteData *palette, int planes, int offset, int size);
char *func_02028af4(const char *name, const char *extension); // The path of a file: the name with the extension
u32 func_02028a68(int plane, u32 planes); // The bit of background plane 0 to 7 in planes
u32 func_02028a7c(int plane, u32 planes); // The bit of object plane 0 or 1 in planes
}

static void LoadBGExtPltt0(const void *source, u32 offset, u32 size);
static void LoadBGExtPltt1(const void *source, u32 offset, u32 size);
static void LoadBGExtPltt2(const void *source, u32 offset, u32 size);
static void LoadBGExtPltt3(const void *source, u32 offset, u32 size);
static void LoadSubBGExtPltt0(const void *source, u32 offset, u32 size);
static void LoadSubBGExtPltt1(const void *source, u32 offset, u32 size);
static void LoadSubBGExtPltt2(const void *source, u32 offset, u32 size);
static void LoadSubBGExtPltt3(const void *source, u32 offset, u32 size);
static void LoadOBJExtPltt(const void *source, u32 offset, u32 size);
static void LoadSubOBJExtPltt(const void *source, u32 offset, u32 size);

// The copies of PaletteLoader::Entry::mType
static void (*const sLoaders[])(const void *source, u32 offset, u32 size) = {
    GX_LoadBGPltt,     GX_LoadBGPltt,     GX_LoadBGPltt,     GX_LoadBGPltt,
    GXS_LoadBGPltt,    GXS_LoadBGPltt,    GXS_LoadBGPltt,    GXS_LoadBGPltt,
    GX_LoadOBJPltt,    GXS_LoadOBJPltt,
    LoadBGExtPltt0,    LoadBGExtPltt1,    LoadBGExtPltt2,    LoadBGExtPltt3,
    LoadSubBGExtPltt0, LoadSubBGExtPltt1, LoadSubBGExtPltt2, LoadSubBGExtPltt3,
    LoadOBJExtPltt,    LoadSubOBJExtPltt,
};

PaletteLoader gPaletteLoader;

extern "C" void PaletteLoader_Init(PaletteLoader *loader) {
    PaletteLoader_ClearDefaults(loader);
}

extern "C" void *PaletteLoader_LoadFile(PaletteLoader *loader, const char *name, int planes, int offset, int size,
                               int unknown) {
    NNSG2dPaletteData *palette;
    PaletteLoader_ReadFile(loader, name, &palette);
    if (offset == -1) {
        offset = loader->mDefaultOffset;
    }
    if (size == -1) {
        size = loader->mDefaultSize;
    }
    if (unknown == -1) {
        unknown = loader->mDefaultUnknown;
    }
    if (unknown != 0 && !palette->bExtendedPlt) {
        palette->szByte = unknown;
    }
    return func_02025240(&gGraphicsManager, palette, planes, offset, size);
}

extern "C" BOOL PaletteLoader_ReadFile(PaletteLoader *loader, const char *name, NNSG2dPaletteData **palette) {
    void *file = ArchiveManager_GetFileByName(&gArchiveManager, func_02028af4(name, ".NCLR"));
    NNSG2dPaletteData *data =
        (NNSG2dPaletteData *)((u8 *)NNS_G2dFindBinaryBlock((NNSG2dBinaryFileHeader *)file, 'PLTT') + 8);
    *palette = data;
    if ((u32)data < (u32)data->pRawData) {
        return FALSE;
    }
    NNS_G2dGetUnpackedPaletteData(file, palette);
    return TRUE;
}

static void LoadBGExtPltt0(const void *source, u32 offset, u32 size) {
    GX_BeginLoadBGExtPltt();
    GX_LoadBGExtPltt(source, offset, size);
    GX_EndLoadBGExtPltt();
}

static void LoadBGExtPltt1(const void *source, u32 offset, u32 size) {
    GX_BeginLoadBGExtPltt();
    GX_LoadBGExtPltt(source, offset + 0x2000, size);
    GX_EndLoadBGExtPltt();
}

static void LoadBGExtPltt2(const void *source, u32 offset, u32 size) {
    GX_BeginLoadBGExtPltt();
    GX_LoadBGExtPltt(source, offset + 0x4000, size);
    GX_EndLoadBGExtPltt();
}

static void LoadBGExtPltt3(const void *source, u32 offset, u32 size) {
    GX_BeginLoadBGExtPltt();
    GX_LoadBGExtPltt(source, offset + 0x6000, size);
    GX_EndLoadBGExtPltt();
}

static void LoadSubBGExtPltt0(const void *source, u32 offset, u32 size) {
    GXS_BeginLoadBGExtPltt();
    GXS_LoadBGExtPltt(source, offset, size);
    GXS_EndLoadBGExtPltt();
}

static void LoadSubBGExtPltt1(const void *source, u32 offset, u32 size) {
    GXS_BeginLoadBGExtPltt();
    GXS_LoadBGExtPltt(source, offset + 0x2000, size);
    GXS_EndLoadBGExtPltt();
}

static void LoadSubBGExtPltt2(const void *source, u32 offset, u32 size) {
    GXS_BeginLoadBGExtPltt();
    GXS_LoadBGExtPltt(source, offset + 0x4000, size);
    GXS_EndLoadBGExtPltt();
}

static void LoadSubBGExtPltt3(const void *source, u32 offset, u32 size) {
    GXS_BeginLoadBGExtPltt();
    GXS_LoadBGExtPltt(source, offset + 0x6000, size);
    GXS_EndLoadBGExtPltt();
}

static void LoadOBJExtPltt(const void *source, u32 offset, u32 size) {
    GX_BeginLoadOBJExtPltt();
    GX_LoadOBJExtPltt(source, offset, size);
    GX_EndLoadOBJExtPltt();
}

static void LoadSubOBJExtPltt(const void *source, u32 offset, u32 size) {
    GXS_BeginLoadOBJExtPltt();
    GXS_LoadOBJExtPltt(source, offset, size);
    GXS_EndLoadOBJExtPltt();
}

extern "C" void PaletteLoader_QueueColors(PaletteLoader *loader, const void *colors, u32 planes, u32 offset, u32 size,
                              BOOL extended) {
    int base = extended ? 10 : 0;
    if (colors == NULL) {
        return;
    }
    size = (size + 1) & ~1;
    colors = (const void *)((u32)colors & ~1);
    offset &= ~1;
    u32 bgSize = size > 0x1A0 ? 0x1A0 : size;
    for (int i = 0; i < 8; i++) {
        if (func_02028a68(i, planes)) {
            loader->mEntries[loader->mCount].mType = i + base;
            loader->mEntries[loader->mCount].mSource = colors;
            loader->mEntries[loader->mCount].mOffset = offset;
            loader->mEntries[loader->mCount].mSize = bgSize;
            loader->mCount++;
        }
    }
    if (size > 0x1A0) {
        size = 0x1A0;
    }
    for (int i = 0; i < 2; i++) {
        if (func_02028a7c(i, planes)) {
            loader->mEntries[loader->mCount].mType = i + 8 + base;
            loader->mEntries[loader->mCount].mSource = colors;
            loader->mEntries[loader->mCount].mOffset = offset;
            loader->mEntries[loader->mCount].mSize = size;
            loader->mCount++;
        }
    }
}

extern "C" void PaletteLoader_SetBackdrop(PaletteLoader *loader, GXRgb color, u32 planes) {
    if (planes & 0x3F) {
        loader->mMainBackdrop = color;
        loader->mMainBackdropChanged = TRUE;
    }
    if (planes & 0x3F00) {
        loader->mSubBackdrop = color;
        loader->mSubBackdropChanged = TRUE;
    }
}

extern "C" void PaletteLoader_Transfer(PaletteLoader *loader) {
    const void *source;
    int i;
    for (i = 0; i < loader->mCount; i++) {
        source = loader->mEntries[i].mSource;
        int type = loader->mEntries[i].mType;
        u32 offset = loader->mEntries[i].mOffset;
        u32 size = loader->mEntries[i].mSize;
        DC_FlushRange(source, size);
        if (sLoaders[i] != NULL) {
            sLoaders[type](source, offset, size);
        }
    }
    loader->mCount = 0;
    if (loader->mMainBackdropChanged) {
        GX_LoadBGPltt(&loader->mMainBackdrop, 0, 2);
        loader->mMainBackdropChanged = FALSE;
    }
    if (loader->mSubBackdropChanged) {
        GXS_LoadBGPltt(&loader->mSubBackdrop, 0, 2);
        loader->mSubBackdropChanged = FALSE;
    }
}

extern "C" void PaletteLoader_ClearDefaults(PaletteLoader *loader) {
    loader->mDefaultOffset = 0;
    loader->mDefaultSize = 0;
    loader->mDefaultUnknown = 0;
}

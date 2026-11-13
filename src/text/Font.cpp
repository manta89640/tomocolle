#include "text/TextCanvas.h"

extern "C" {
// Finds a file in a mounted archive (NNS_FndGetArchiveFileByName); not decompiled yet
void *ArchiveManager_GetFileByName(void *archive, const char *fileName);
extern u8 gArchiveManager[];
}

Font gFonts[3];

Font::Font() {
}

void Font::Init(const char *fileName) {
    mResource = ArchiveManager_GetFileByName(gArchiveManager, fileName);
    NNS_G2dFontInitUTF8(&mFont, mResource);
}

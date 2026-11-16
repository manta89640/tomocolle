/*
 * CHT, the SDK's library for finding PictoChat rooms nearby (declared in nitro/cht/ARM9/pictocatch.h). The SDK ships it
 * as a compiled library without sources, so this is the project's own decompilation. The game links one function of
 * it, CHT_IsPictochatParent (0x020144E4), right after MATH_QSort. The file's name is the project's, after the header.
 */
#include <nitro.h>
#include <nitro/cht/ARM9/pictocatch.h>

// Whether a beacon is a PictoChat room's: no game group ID, user data that begins with one of two words and has 4 as
// its fourth half-word
BOOL CHT_IsPictochatParent(const WMBssDesc *pWmBssDesc)
{
    // Read back from memory at each use, as the original does: volatile
    volatile u16 info[4];

    if (pWmBssDesc == NULL) {
        return FALSE;
    }
    if (pWmBssDesc->gameInfoLength == 0) {
        return FALSE;
    }
    MI_CpuCopy8(pWmBssDesc->gameInfo.userGameInfo, (void *)info, sizeof(info));
    DC_StoreRange((void *)info, sizeof(info));
    if (pWmBssDesc->gameInfo.ggid == 0 && (info[0] == 0x2348 || info[0] == 0xBD8A) && info[3] == 4) {
        return TRUE;
    }
    return FALSE;
}

/*
 * The game's mi_dma.c has one function more than the fetched release: MI_StopAllDma, at 0x02009400 between MI_StopDma
 * and MIi_CheckAnotherAutoDMA, which PM_ForceToPowerOff calls before it halts. Written from the game's code; see
 * tools/override_functions.py for how this file is used.
 */

/* override: after MI_StopDma */
void MI_StopAllDma(void);

void MI_StopAllDma(void)
{
    MI_StopDma(0);
    MI_StopDma(1);
    MI_StopDma(2);
    MI_StopDma(3);
}

#include "MiiEditScene.h"

#include "GameData.h"
#include "sound/SoundManager.h"

#include <wchar.h>

// --- BEGIN of the file's .rodata (in an order that the compiler sorts into the ROM's) ---
static const u8 data_ov076_02158748[2] = { 0x6a, 0x6a };
static const u8 data_ov076_0215874c[2] = { 0x69, 0x69 };
static const int data_ov076_0215878c = 107;
static const u8 data_ov076_02158750[4] = { 0x20, 0x22, 0x24, 0x26 };
static const int data_ov076_0215876c = 28;
static const u16 data_ov076_02158788[2] = { 0x10, 0x6b };
static const int data_ov076_02158754 = 76;
static const int data_ov076_02158768 = 2;
static const int data_ov076_02158770 = 2;
static const int data_ov076_0215877c = 2;
static const u16 data_ov076_02158774[2] = { 0x4d, 0x4e };
static const int data_ov076_02158778 = 6;
static const int data_ov076_02158780 = 0;
static const u16 data_ov076_02158784[2] = { 0x2, 0x4 };
static const u16 data_ov076_02158764[2] = { 0x30, 0x31 };
static const int data_ov076_0215875c = 4;
static const int data_ov076_02158760 = 4;
static const int data_ov076_02158758 = 32;
static const WindowPoint data_ov076_021587b0 = { 16, 7 };
static const WindowPoint data_ov076_021587e0 = { 36, 40 };
static const WindowPoint data_ov076_021587f8 = { 65, 38 };
static const WindowPoint data_ov076_021587a8 = { 75, 149 };
static const WindowPoint data_ov076_021587c8 = { 32, 24 };
static const WindowPoint data_ov076_02158798 = { 42, 36 };
static const WindowPoint data_ov076_021587d0 = { 128, 76 };
static const WindowPoint data_ov076_021587e8 = { 8, 7 };
static const WindowPoint data_ov076_02158790 = { 181, 149 };
static const WindowPoint data_ov076_021587a0 = { 75, 149 };
static const WindowPoint data_ov076_021587b8 = { 128, 76 };
static const WindowPoint data_ov076_02158800 = { 38, 33 };
static const WindowPoint data_ov076_02158810 = { 30, 20 };
static const WindowPoint data_ov076_021587c0 = { 94, 30 };
static const WindowPoint data_ov076_02158808 = { 94, 30 };
static const WindowPoint data_ov076_021587d8 = { 36, 40 };
static const u32 data_ov076_021587f0[2] = { 2, 3 };
static const u16 data_ov076_02158822[5] = { 0x1, 0x2, 0x3, 0x4, 0x5 };
static const u16 data_ov076_02158818[5] = { 0x26, 0x26, 0x26, 0x26, 0x26 };
static const VecFx32 data_ov076_02158844 = { 1, 0, 0 };
static const VecFx32 data_ov076_02158868 = { 41, 5, 6 };
static const VecFx32 data_ov076_02158850 = { 0, 7844, 58762 };
static const VecFx32 data_ov076_021588b0 = { 4, 4, 4 };
static const VecFx32 data_ov076_0215882c = { 0, 0, 0 };
static const VecFx32 data_ov076_02158874 = { 0, 2, 2 };
static const u16 sButtonSequences[6] = { 0x58, 0x5a, 0x5c, 0x5e, 0x28, 0x2a };
static const VecFx32 data_ov076_0215885c = { 0, 7248, 105456 };
static const VecFx32 data_ov076_0215888c = { 0, 1, 1 };
static const VecFx32 sLightDirection1 = { 0, -4000, 1500 };
static const VecFx32 sLightDirection0 = { 1600, -600, -4000 };
static const VecFx32 data_ov076_02158838 = { 0, 0, -32768 };
static const int data_ov076_021588cc[4] = { 128, 96, 128, 96 };
static const ScreenRect sWindowRect = { 20, 20, 216, 152 };
static const ButtonSounds data_ov076_021588ec = { { 16, -1, 0, -1 } };
static const int data_ov076_021588bc[4] = { 202, 112, 52, 8 };
static const int data_ov076_021588fc[4] = { 75, 149, 181, 149 };
static const int data_ov076_0215890c[4] = { 145, 65, 145, 119 };
static const int data_ov076_0215891c[4] = { 5, -1, 19, -1 };
static const int data_ov076_0215894c[4] = { 94, 30, 94, 30 };
static const int data_ov076_0215892c[4] = { 80, 77, 123, 77 };
static const int data_ov076_0215893c[4] = { 14, 15, 16, 17 };
static const u16 data_ov076_0215895c[10] = { 0x0, 0x2, 0x4, 0x6, 0xa, 0xc, 0x10, 0x14, 0x16, 0xe };
static const int data_ov076_02158970[5] = { 4, 4, 4, 4, 4 };
static const int data_ov076_02158984[6] = { 62, 22, 116, 30, 116, 30 };
static const int data_ov076_0215899c[6] = { 14, 14, 14, 14, 14, 1 };
static const int sButtonKinds[6] = { 3, 3, 3, 3, 4, 4 };
static const int data_ov076_021589cc[6] = { 136, 132, 62, 175, 194, 175 };
static const int data_ov076_021589e4[6] = { 14, 15, 16, 17, 22, 23 };
static const WindowPoint data_ov076_02158a7c[4] = { { 80, 102 }, { 176, 102 }, { 80, 90 }, { 176, 90 } };
static const WindowPoint data_ov076_02158a9c[4] = { { 58, 54 }, { 58, 54 }, { 58, 54 }, { 58, 54 } };
static const int data_ov076_02158abc[8] = { 158, 76, 138, 76, 118, 76, 98, 76 };
static const int data_ov076_02158adc[8] = { 170, 76, 150, 76, 90, 76, 70, 76 };
static const int data_ov076_02158afc[8] = { 80, 77, 123, 77, 80, 114, 123, 114 };
static const int data_ov076_02158b1c[8] = { 14, 15, 16, 17, 18, 19, 20, 21 };
static const int data_ov076_021589fc[8] = { 207, 48, 239, 48, 207, 74, 239, 74 };
static const int data_ov076_02158a1c[8] = { 14, -1, 0, -1, 16, -1, 0, -1 };
static const int data_ov076_02158a3c[8] = { 15, -1, 0, -1, 16, -1, 0, -1 };
static const WindowPoint data_ov076_02158a5c[4] = { { 80, 90 }, { 176, 90 }, { 80, 90 }, { 176, 90 } };
static const int data_ov076_02158b3c[9] = { 0, 1, 4, 2, 3, 6, 7, 8, 9 };
static const int data_ov076_02158b60[10] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };
static const ScreenPoint sSpriteB0Places[5] = { { 55, 182 }, { 55, 182 }, { 103, 182 }, { 151, 182 }, { 199, 182 } };
static const int data_ov076_02158bb0[10] = { 128, 26, 128, 59, 128, 92, 128, 125, 128, 158 };
static const int data_ov076_02158bd8[10] = { 128, 112, 128, 88, 128, 64, 128, 16, 128, 40 };
static const int data_ov076_02158c00[10] = { 8, 20, 8, 20, 8, 20, 8, 20, 8, 20 };
static const int data_ov076_02158c28[10] = { 128, 26, 128, 59, 128, 92, 128, 125, 128, 158 };
static const int data_ov076_02158c50[10] = { 12, 12, 12, 12, 12, 12, 12, 12, 12, 12 };
static const u16 data_ov076_02158c78[22] = { 0x0, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x20, 0x22, 0x24, 0x28, 0x2a, 0x2c, 0x2e };
static const WindowPoint sButtonSizes[6] = { { 40, 18 }, { 40, 18 }, { 40, 18 }, { 40, 18 }, { 40, 18 }, { 24, 22 } };
static const int data_ov076_02158cd4[12] = { 20, -1, 0, -1, 22, -1, 0, -1, 20, -1, 0, -1 };
static const int data_ov076_02158d04[12] = { 1, 10, 2, 3, 6, 0, 5, 4, 8, 9, 7, 11 };
static const int data_ov076_02158d34[12] = { 21, 19, 21, 21, 22, 22, 23, 23, 24, 24, 23, 22 };
static const int data_ov076_02158d64[12] = { 207, 48, 239, 48, 207, 74, 239, 74, 207, 100, 239, 100 };
static const int data_ov076_02158d94[12] = { 20, -1, 0, -1, 21, -1, 0, -1, 22, -1, 0, -1 };
static const WindowPoint sButtonPositions[6] = { { 32, 181 }, { 80, 181 }, { 128, 181 }, { 176, 181 }, { 224, 181 }, { 242, 13 } };
static const u16 data_ov076_02158df4[30] = { 0x18, 0x1a, 0x1c, 0x1e, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x20, 0x22, 0x3c, 0x3e, 0x3c, 0x3e, 0x40, 0x42, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x0 };
static const char sWindowScreenName[] = "E00:/Window_down.nsc";
static const u8 data_ov076_02158e48[24] = { 0x1a, 0x1a, 0x1b, 0x19, 0x1a, 0x19, 0x1a, 0x19, 0x1c, 0x19, 0x1a, 0x18, 0x1b, 0x1b, 0x1a, 0x1a, 0x19, 0x19, 0x1a, 0x1a, 0x1b, 0x1a, 0x19, 0x1b };
static const u8 data_ov076_02158e60[50] = { 0x1d, 0x1c, 0x1c, 0x1c, 0x1d, 0x1c, 0x1c, 0x1c, 0x1d, 0x1c, 0x1c, 0x1c, 0x1c, 0x1d, 0x1d, 0x1c, 0x1c, 0x1c, 0x1d, 0x1d, 0x1c, 0x1d, 0x1c, 0x1d, 0x1d, 0x1c, 0x1d, 0x1c, 0x1c, 0x1d, 0x1c, 0x1c, 0x1c, 0x1d, 0x1d, 0x1d, 0x1c, 0x1c, 0x1d, 0x1d, 0x1d, 0x1c, 0x1c, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1c, 0x1c };
static const ButtonSounds data_ov076_02158e94[4] = { { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } } };
static const int data_ov076_02158ed4[16] = { 80, 40, 123, 40, 80, 77, 123, 77, 80, 114, 123, 114, 80, 151, 123, 151 };
static const int data_ov076_02158f14[16] = { 207, 48, 239, 48, 207, 74, 239, 74, 207, 100, 239, 100, 207, 126, 239, 126 };
static const u16 data_ov076_02158f54[36] = { 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e, 0x40, 0x42, 0x44, 0x46, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x0, 0x48, 0x4a, 0x4c, 0x4e, 0x50, 0x52, 0x54, 0x56 };
static const int data_ov076_02158f9c[18] = { 59, 77, 102, 77, 145, 77, 59, 114, 102, 114, 145, 114, 59, 151, 102, 151, 145, 151 };
static const WindowPoint data_ov076_02158fe4[10] = { { 70, 40 }, { 186, 40 }, { 128, 112 }, { 160, 76 }, { 68, 76 }, { 202, 112 }, { 216, 76 }, { 68, 149 }, { 192, 149 }, { 54, 112 } };
static const int data_ov076_02159034[20] = { 116, 18, 116, 18, 32, 18, 56, 18, 112, 18, 32, 18, 56, 18, 112, 18, 48, 18, 48, 18 };
static const int data_ov076_02159084[22] = { 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3 };
static const ButtonSounds sButtonSounds[6] = { { { 2, -1, -1, -1 } }, { { 2, -1, -1, -1 } }, { { 2, -1, -1, -1 } }, { { 2, -1, -1, -1 } }, { { 20, -1, 0, -1 } }, { { 16, -1, 0, -1 } } };
static const int data_ov076_0215913c[24] = { 6, 0, 12, 1, 9, 19, 7, 21, 8, 17, 5, 4, 11, 10, 2, 3, 14, 20, 15, 13, 22, 18, 16, 23 };
static const int data_ov076_0215919c[24] = { 23, 1, 19, 21, 22, 5, 0, 8, 10, 16, 6, 13, 7, 9, 2, 17, 3, 4, 15, 11, 20, 18, 14, 12 };
static const int data_ov076_021591fc[24] = { 59, 40, 102, 40, 145, 40, 59, 77, 102, 77, 145, 77, 59, 114, 102, 114, 145, 114, 59, 151, 102, 151, 145, 151 };
static const int data_ov076_0215925c[29] = { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5 };
static const int data_ov076_021592d0[32] = { 73, -6, 12, 12, 87, -6, 12, 12, 101, -6, 12, 12, 115, -6, 12, 12, 129, -6, 12, 12, 143, -6, 12, 12, 157, -6, 12, 12, 171, -6, 12, 12 };
static const int data_ov076_02159350[36] = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3 };
static const int data_ov076_021593e0[40] = { 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1 };
static const int data_ov076_02159480[44] = { 68, 52, 92, 52, 116, 52, 140, 52, 164, 52, 188, 52, 68, 92, 68, 132, 206, 134, 237, 134, 237, 134, 206, 20, 237, 20, 206, 45, 237, 45, 206, 70, 237, 70, 206, 95, 99, 138, 118, 138, 137, 138, 156, 138 };
static const int data_ov076_02159530[44] = { 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 30, 24, 30, 24, 30, 24, 30, 24, 30, 24, 30, 24, 30, 24, 30, 24, 30, 24, 30, 24, 16, 16, 16, 16, 16, 16, 16, 16 };
static const int data_ov076_021595e0[48] = { 2, 4, 0, 8, 39, 17, 1, 26, 16, 15, 27, 20, 33, 11, 19, 32, 9, 12, 23, 34, 21, 25, 40, 35, 5, 41, 13, 36, 37, 6, 24, 30, 31, 18, 28, 46, 7, 44, 38, 42, 45, 29, 3, 43, 22, 10, 14, 47 };
static const int data_ov076_021596a0[58] = { 58, 80, 105, 80, 152, 80, 199, 80, 48, 64, 80, 64, 112, 64, 144, 64, 176, 64, 208, 64, 48, 96, 80, 96, 112, 96, 144, 96, 176, 96, 208, 96, 80, 80, 176, 80, 128, 41, 128, 111, 80, 41, 80, 111, 160, 41, 160, 111, 128, 48, 128, 72, 128, 96, 128, 120, 128, 144 };
static const int data_ov076_02159788[58] = { 42, 36, 42, 36, 42, 36, 42, 36, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 58, 54, 58, 54, 20, 15, 20, 15, 20, 15, 20, 15, 20, 15, 20, 15, 126, 20, 126, 20, 126, 20, 126, 20, 126, 20 };
static const int data_ov076_02159870[72] = { 33, 40, 51, 44, 39, 70, 45, 49, 59, 56, 68, 31, 32, 47, 37, 48, 66, 52, 58, 50, 55, 64, 60, 62, 43, 38, 42, 23, 67, 54, 36, 41, 65, 30, 57, 34, 12, 13, 69, 26, 4, 25, 1, 19, 5, 8, 27, 7, 14, 3, 22, 10, 6, 20, 11, 63, 17, 35, 21, 0, 61, 16, 46, 9, 18, 2, 28, 53, 71, 24, 15, 29 };
static const int data_ov076_02159990[72] = { 17, 159, 17, 12, 17, 33, 17, 54, 17, 75, 17, 96, 17, 117, 17, 138, 77, 11, 127, 11, 77, 11, 127, 11, 77, 30, 127, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 223, 74, 68, 11, 136, 11, 223, 155, 179, 30, 179, 48, 179, 66, 179, 84, 179, 102, 179, 120, 179, 138, 179, 156 };
static const int data_ov076_02159ab0[88] = { 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 14, -1, 0, -1, 1, -1, 0, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1 };
static const int data_ov076_02159c10[116] = { 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 17, -1, -1, -1, 18, -1, -1, -1, 17, -1, -1, -1, 18, -1, -1, -1, 17, -1, -1, -1, 18, -1, -1, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1 };
static const int data_ov076_02159de0[144] = { 3, -1, -1, -1, 3, -1, -1, -1, 3, -1, -1, -1, 3, -1, -1, -1, 3, -1, -1, -1, 3, -1, -1, -1, 3, -1, -1, -1, 3, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 12, -1, -1, -1, 13, -1, -1, -1, 13, -1, -1, -1, -1, -1, -1, -1, 4, -1, 19, -1, 4, -1, 19, -1, 4, -1, 19, -1, 4, -1, 19, -1, 4, -1, 19, -1, 4, -1, 19, -1, 4, -1, 19, -1, 4, -1, 19, -1 };
// --- END of the file's .rodata ---

// --- BEGIN of the file's .data ---
static const wchar_t *data_ov076_0215a10c[5] = { L"♂", L"♀", L"" };
// Filled in by the static initialiser, as its initialiser reads other constants
static const WindowPoint data_ov076_0215a13c[36] = {
    data_ov076_02158810, data_ov076_02158810, data_ov076_02158810, data_ov076_02158810,
    data_ov076_02158810, data_ov076_02158810, data_ov076_02158810, data_ov076_02158810,
    { 48, 18 }, { 48, 18 }, { 48, 18 }, { 48, 18 }, { 48, 18 }, { 48, 18 },
    data_ov076_021587c8, data_ov076_021587c8, data_ov076_021587c8, data_ov076_021587c8, data_ov076_021587c8,
    data_ov076_021587c8, data_ov076_021587c8, data_ov076_021587c8, data_ov076_021587c8, data_ov076_021587c8,
    { 62, 24 }, { 16, 18 }, { 16, 18 }, { 62, 24 }, { 16, 16 }, { 16, 16 },
    { 16, 16 }, { 16, 16 }, { 16, 16 }, { 16, 16 }, { 16, 16 }, { 16, 16 },
};
// --- END of the file's .data ---

// --- The main program (stand-ins, not decompiled) ---
struct ArchiveLoader;
struct ScreenControl;
struct ScreenLoader;
struct ParticleSystem;
extern "C" {
extern ArchiveLoader gArchiveManager;
void ArchiveManager_MountCompressed(ArchiveLoader *loader, const char *name, const char *path, int unknown);
void ArchiveManager_Unmount(ArchiveLoader *loader, const char *name);
extern u32 data_021223e4[];
void func_0202aecc(void *resources, int resourceId);
void func_0202ad84(void *resources);
void func_0202683c(void);
extern ScreenControl data_0211d604;
extern ScreenLoader data_02121834;
void func_02024234(ScreenControl *screens, int planes, int a);
void func_02023f4c(ScreenControl *screens, int planes, int a);
void func_02023eac(ScreenControl *screens, int planes);
void func_02028108(ScreenLoader *loader, int plane, int offset);
void func_0208ee18(CharacterModel *model, const char *archiveName, int a, int b);
void func_02091968(SceneCharacter *character); // Draws it
void func_0208fefc(CharacterBase *character);
void func_0208ff94(CharacterBase *character, void *a, void *b);
void func_02090214(CharacterBase *character);
void func_0207b258(ParticleSystem *system, void *a);
extern u8 data_02142438[];
}

void MiiEditScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "E00", "Scn/Scene/SceneMiiEdit2_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "KBD", "Cmn/Kbd/SoftKeyboard_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TBM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "T02", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "SPL", "Mdl/Spl/SplEditEffect_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "BIC", "Mdl/Ica/ica_mii_edit_LZ.bin", 0);
    func_0208ee18(&mCharacter.mModel, "B0I", 0, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "B0I", mCharacter.mModel.mArchivePath, 0);
}

// The camera's moves last this many frames
#define CAMERA_MOVE_FRAMES 40

void MiiEditScene::ResetCameraGoal() {
    mCameraGoal = mCamera->mPosition;
    mCameraFrame = 0;
}

void MiiEditScene::UpdateCamera() {
    if (mCameraFrame >= CAMERA_MOVE_FRAMES) {
        return;
    }
    fx32 t = (mCameraFrame << FX32_SHIFT) / CAMERA_MOVE_FRAMES;
    t = FX_Mul(t, t);
    t = FX_Mul(t, t);
    mCamera->mPosition.x += FX_Mul(mCameraGoal.x - mCamera->mPosition.x, t);
    mCamera->mPosition.y += FX_Mul(mCameraGoal.y - mCamera->mPosition.y, t);
    mCamera->mPosition.z += FX_Mul(mCameraGoal.z - mCamera->mPosition.z, t);
    mCamera->mTarget.x = mCamera->mPosition.x;
    mCamera->mTarget.y = mCamera->mPosition.y;
    mCamera->mTarget.z = mCamera->mPosition.z - FX32_CONST(8);
    mCameraFrame++;
}

void MiiEditScene::MoveCameraTo(VecFx32 goal) {
    mCameraFrame = 13;
    mCameraGoal.x = goal.x;
    mCameraGoal.y = goal.y;
    mCameraGoal.z = goal.z;
}

// --- Of the main program, for the scene's Init and Update, and the functions after Exit (stand-ins) ---
struct PadInput {
    /* 0x0 */ u32 mTrigger;
};
struct SceneManager;
struct SceneMemory {
    /* 0x00 */ u8 mUnknown00[0x20];
    /* 0x20 */ u8 mCharacters[12]; // The first is the character the scene edits
};
struct Light;
extern "C" {
extern PadInput data_0211f9d4;
extern SceneManager gSceneManager;
extern SceneMemory data_0213ba74;
extern Light data_021227cc;
extern Light data_021227d8;
void func_0203c02c(SceneManager *manager, int sceneId); // Asks for the next scene
BOOL func_02023fa4(ScreenControl *screens, int planes);
void func_0202aa28(Light *light, Camera *camera, VecFx32 direction);
void func_02091578(SceneCharacter *character, u16 angle);
void func_02090a58(SceneCharacter *character, const char *phoneme, int face); // Lip sync
void func_020910a8(SceneCharacter *character);
void func_0207b138(ParticleSystem *system);
void func_0202c264(GameData *game, int index, CharacterRecord *record); // Reads a saved character
void func_0202c28c(GameData *game, int index, CharacterRecord *record); // Writes one
BOOL func_0209d900(CharacterRecord *record, SceneCharacter *character);
void func_02031e88(SpriteButton *button, BOOL a);
void func_0207b3dc(ParticleSystem *system);
ParticleEmitter *func_0207b2fc(ParticleSystem *system, int resource, VecFx32 *position);
void CharacterModel_GetJointMatrices(CharacterModel *model, int joint, MtxFx43 *matrix, MtxFx33 *rotation);
void *func_0202ad04(void *resources, int resourceId, const char *archiveName, int unk);
void func_0202aecc(void *resources, int resourceId);
void func_02029660(void *resourceSet, u32 resType, void *archive);
void ModelResources_SetModel(void *resources, int resourceId, const char *modelPath);
int MiiData_GetSex(CharacterRecord *character);
void func_02091048(SceneCharacter *character, const char *archiveName);
void func_0208ef5c(CharacterModel *model, int resourceId, int a);
void Character_SetAnimation(SceneCharacter *character, u32 resType, int id); // Starts an animation
void Character_SetAngle(SceneCharacter *character, u16 angle);
void func_02091a88(SceneCharacter *character);
void func_02091a24(SceneCharacter *character);
void func_02091a6c(SceneCharacter *character);
void func_0209e01c(CharacterRecord *character);
s8 func_02098f2c(CharacterRecord *character); // Its personality
extern const int data_020fba28[];
void func_0209930c(CharacterRecord *character, int a, int b, s64 c);
void func_0209b1d8(CharacterRecord *character);
void func_02029f10(CharacterModel *model, GXRgb color);
void func_02029f20(CharacterModel *model, GXRgb color);
void func_02029f30(CharacterModel *model, GXRgb color);
void func_02029f40(CharacterModel *model, GXRgb color);
void func_02092178(wchar_t *name);
}

struct Renderer3d;
extern "C" {
extern Renderer3d data_02122824;
void func_0202b0e8(Renderer3d *renderer);
void func_02090fdc(SceneCharacter *character, int slot, int a, int b);
void Character_SetFace(SceneCharacter *character, int face);
void func_0209cbac(CharacterRecord *character, u8 *a, u8 *b);
void func_02091a14(SceneCharacter *character, BOOL visible);
BOOL func_0202c354(GameData *game, int index); // Whether there is a saved character with that index
void func_020979e0(CharacterRecord *character);
extern void *data_02123440[];
void Clock_GetDate(void *clock, RTCDate *date);
void func_020984e0(CharacterRecord *character, int year, int month, int day);
void func_0208fd90(CharacterBase *character, int a);
u32 func_02098dac(CharacterRecord *character, int property);
void func_0202a640(Camera *camera);
extern Camera data_02122764;
void func_0202a96c(Light *light);
void func_02024b10(u32 *manager, void *a, int planes);
int func_02027290(CellLoader *loader, const char *cellName, int plane, int a);
void func_02023ee0(ScreenControl *screens, int a, int b, int c, int d);
void *func_0202680c(int size);
void *func_02026824(int size); // An allocator the particle system is given
ParticleSystem *func_0207ab20(void *(*alloc)(int), int a, int b, int c, int d, int e);
void func_0207ac78(ParticleSystem *system, void *file);
void func_0207b110(ParticleSystem *system);
void func_0207b124(ParticleSystem *system);
BOOL func_02098584(CharacterRecord *character);
}

static inline void SetCanvasPosition(ObjTextCanvas &canvas, WindowPoint position) {
    canvas.mSprite.SetPosition(FxVec2(position.x << FX32_SHIFT, position.y << FX32_SHIFT));
}

// Puts a text canvas at a position in pixels
static inline void SetCanvasPosition(ObjTextCanvas *canvas, WindowPoint position) {
    canvas->mSprite.SetPosition(FxVec2(position.x << FX32_SHIFT, position.y << FX32_SHIFT));
}

void MiiEditScene::Init() {
    mPage = -1;
    mUnknown1B32C = FALSE;
    func_0202b0e8(&data_02122824);
    data_0211d604.SetUnknown44(1);
    data_0211d604.Show(1);
    data_0211d604.SetUnknown48(1);
    G3X_SetClearColor(GX_RGB(1, 1, 1), 0, 0x7FFF, 63, FALSE);
    gSoundManager.LoadGroup(1);
    gSoundManager.StartSeq(0, 2);
    ModelResources_SetModel(data_021223e4, 0, mCharacter.mModel.mUnknown4E8);
    func_02090fdc(&mCharacter, 0, 0, 2);
    Character_SetFace(&mCharacter, 8);
    switch (mSceneId) {
    case 0x42:
        mUnknown5824 = data_0213ba74.mCharacters[0];
        func_ov076_0214b670(mUnknown5824);
        break;
    case 0x44: {
        mUnknown5824 = data_0213ba74.mCharacters[0];
        func_ov076_0214b670(mUnknown5824);
        u8 a;
        u8 b;
        func_0209cbac(&mCharacter, &a, &b);
        mCharacter.mUnknown290 = a;
        mCharacter.mUnknown291 = b;
        break;
    }
    case 0x43:
    case 0x6F: {
        void *animations = func_0202ad04(data_021223e4, 0x1F, "BIC", 0);
        func_02029660((void *)mCharacter.mModel.mUnknown04, RESOURCE_BCA0, animations);
        Character_SetAnimation(&mCharacter, RESOURCE_BCA0, 0);
        Character_SetAngle(&mCharacter, 0x4000);
        func_02091a14(&mCharacter, FALSE);
        int i;
        for (i = 0; i < 100; i++) {
            if (!func_0202c354(&gGameData, i)) {
                break;
            }
        }
        if (i < 100) {
            mUnknown5824 = i;
        } else {
            OS_Terminate();
        }
        func_020979e0(&mCharacter);
        func_0209e01c(&mCharacter);
        RTCDate date;
        Clock_GetDate(data_02123440[0], &date);
        func_020984e0(&mCharacter, date.year, date.month, date.day);
        break;
    }
    }
    func_02091a24(&mCharacter);
    mUnknown5828 = FALSE;
    func_02091a6c(&mCharacter);
    func_0208fd90(&mEditedCharacter, 0);
    *(MiiData *)mEditedCharacter.mMii = *(MiiData *)mCharacter.mMii;
    func_0208fefc(&mEditedCharacter);
    VecFx32 v = mCharacter.mUnknown2568;
    mEditedCharacter.mUnknown2568 = v;
    mEditedCharacter.mUnknown25D4 = mCharacter.mUnknown25D4;
    mEditedCharacter.mUnknown25D6 = mCharacter.mUnknown25D6;
    mEditedCharacter.mUnknown25D8 = mCharacter.mUnknown25D8;
    mEditedCharacter.mUnknown25E0 = mCharacter.mUnknown25E0;
    func_0208ff94(&mEditedCharacter, mCharacter.mUnknown2538, mCharacter.mUnknown2514);
    mUnknown580C = TRUE;
    mCamera = &data_02122764;
    func_0202a640(mCamera);
    mCamera->mUnknown00 = 0x11E;
    mCamera->mUnknown04 = 0xFF6;
    VEC_Set(&mCamera->mPosition, 0, 0x1EA4, 0xE58A);
    mCamera->mPosition.y += (func_02098dac(&mCharacter, 0x29) * 0x1130 >> 7) - 0x898;
    mCamera->mTarget.x = mCamera->mPosition.x;
    mCamera->mTarget.y = mCamera->mPosition.y;
    mCamera->mTarget.z = mCamera->mPosition.z - FX32_CONST(8);
    ResetCameraGoal();
    func_0202a96c(&data_021227cc);
    func_0202a96c(&data_021227d8);
    mTtsPlayer.InitEngine();
    func_02024b10(gGraphicsManager, mUnknown00010, 0x3F00);
    func_02027290(&gObjResourceManager, "E00:/Obj_down.nce", 0x1000, 0);
    func_02028f24(&gVramTransferQueue, "E00:/Obj_down.ncg", 0x1000, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "E00:/Obj_down.ncl", 0x1000, -1, -1, -1);
    func_02027d7c(&data_02121834, "E00:/BG_down.nsc", 0x200, 0, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, "E00:/Window_down.nsc", 0x100, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "E00:/BG_down.ncg", 0x2F00, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "E00:/BG_down.ncl", 0x2F00, -1, -1, -1);
    func_02028f24(&gVramTransferQueue, "E00:/BG_up.ncg", 0x2F, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "E00:/BG_up.ncl", 0x2F, -1, -1, -1);
    func_02027d7c(&data_02121834, "E00:/BG_up.nsc", 4, 0, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, "E00:/BG_up_textframe.nsc", 2, 0, 0, 0, 0, -1);
    func_02028280(&data_02121834, 1, 1);
    func_02028280(&data_02121834, 2, 2);
    func_02028280(&data_02121834, 4, 3);
    func_02028280(&data_02121834, 0x10, 0);
    func_02028280(&data_02121834, 0x100, 0);
    func_02028280(&data_02121834, 0x200, 1);
    func_02028280(&data_02121834, 0x1000, 0);
    func_02023ee0(&data_0211d604, 1, 4, 8, 8);
    func_ov076_0214bbe4();
    for (u16 i = 0; i < 6; i++) {
        Sprite_Init(&mButtons[i], 0, 0, 0x80, 0x60, TRUE);
        func_02031eb0(&mButtons[i], sButtonSizes[i].x, sButtonSizes[i].y);
        func_02031278(&mButtons[i], sButtonSequences[i], sButtonSequences[i] + 1);
        func_02031260(&mButtons[i], sButtonKinds[i]);
        func_02032158(&mButtons[i], &sButtonSounds[i]);
        mButtons[i].mUnknownDC = data_ov076_0215899c[i];
        mButtons[i].SetPosition(FxVec2(sButtonPositions[i].x << FX32_SHIFT, sButtonPositions[i].y << FX32_SHIFT));
        func_0203119c(&mButtons[i]);
    }
    Sprite_Init(&mSprite582C, 0, 0x6D, 0x80, 0x60, TRUE);
    mUnknown58E0 = TRUE;
    mUnknown58E4 = FALSE;
    mUnknown58DC = FALSE;
    func_0202680c(0x4000);
    mParticles = func_0207ab20(func_02026824, 1, 100, 0, 1, 30);
    mParticleFile = ArchiveManager_GetFileByName(&gArchiveManager, "SPL:/SplEditEffect.spa");
    func_0207ac78(mParticles, mParticleFile);
    func_0207b110(mParticles);
    func_0207b124(mParticles);
    int charOffset = 800;
    mMessageFiles[0].Open("TBM:/tmd_etc_msg_JP.bmg");
    mMessageFiles[1].Open("T02:/tmd_common_msg_JP.bmg");
    for (u16 i = 0; i < 9; i++) {
        int width;
        switch (i) {
        case 0:
        case 1:
        case 2:
            width = 0x18;
            break;
        case 3:
            width = 4;
            break;
        case 4:
            width = 6;
            break;
        case 5:
            width = 6;
            break;
        case 6:
            width = 0x10;
            break;
        case 7:
            width = 8;
            break;
        case 8:
            width = 4;
            break;
        }
        mTextBoxes[i].Init(0x1000, width, 2, charOffset, 0, 0, 0, 0, NULL);
        charOffset += mTextBoxes[i].GetCharCount();
        WindowPoint position = data_ov076_02158fe4[data_ov076_02158b3c[i]];
        position.x += -position.x / 2 + 8;
        position.y -= 7;
        mTextBoxes[i].mSprite.mOverwritePalette = 1;
        mTextBoxes[i].mColor = 8;
        SetCanvasPosition(&mTextBoxes[i], position);
        mTextBoxes[i].Update(TRUE);
    }
    mCharBox.Init(0x1000, 0x20, 2, charOffset, 0, 0, NULL);
    mCharBox.mColor = 8;
    SetCanvasPosition(&mCharBox, data_ov076_021587f8);
    MessageText *text = &mText1B35C;
    text->InitObjForMessage(&mMessageFiles[0], 0x20, &gFonts[0].mFont, 0x1000,
                                 charOffset + mCharBox.GetCharCount(), 0, 1, 0, 0, 0xF, NULL);
    SetCanvasPosition(&mText1B35C, data_ov076_021587b0);
    text->DrawAll();
    mText1B35C.mSprite.mOverwritePalette = 0;
    mText1B35C.mHAlign = 1;
    text = &mText1B94C;
    text->InitObjForMessage(&mMessageFiles[0], 0x1C, &gFonts[0].mFont, 0x10, 200, 0, 1, 0, 0, 0xF, NULL);
    SetCanvasPosition(&mText1B94C, data_ov076_021587e8);
    text->DrawAll();
    mText1B94C.mSprite.mOverwritePalette = 2;
    mUnknown5904 = 0;
    for (int i = 0; i < 7; i++) {
        mUnknown58E8[i] = FALSE;
    }
    switch (mSceneId) {
    case 0x42:
        mSprite582C.func_ov076_0214be80(4);
        mUnknown58E0 = FALSE;
        func_ov076_0214b1ec(2);
        if (!func_02098584(&mCharacter)) {
            func_02031e88(&mButtons[0], TRUE);
        }
        break;
    case 0x44:
        func_02031e88(&mButtons[0], TRUE);
        mSprite582C.func_ov076_0214be80(2);
        func_ov076_0214b1ec(2);
        break;
    case 0x43:
        func_02031e88(&mButtons[2], TRUE);
        func_02031e88(&mButtons[3], TRUE);
        func_02031e88(&mButtons[4], TRUE);
        mSprite582C.func_ov076_0214be80(0);
        func_ov076_0214b1ec(0);
        break;
    case 0x6F:
        mButtons[5].mVisible = FALSE;
        func_02031e88(&mButtons[2], TRUE);
        func_02031e88(&mButtons[3], TRUE);
        func_02031e88(&mButtons[4], TRUE);
        mSprite582C.func_ov076_0214be80(0);
        func_ov076_0214b1ec(0);
        break;
    }
    gSoundManager.LoadGroup(0x52);
}

void MiiEditScene::Update() {
    if (mUnknown1B32C) {
        mUnknown1B32C = FALSE;
        for (u16 i = 0; i < 9; i++) {
            mTextBoxes[i].Redraw();
            mTextBoxes[i].mSprite.mVisible = TRUE;
        }
    }
    mTtsPlayer.Update();
    switch (mPage) {
    case 0:
        break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        if (data_0211f9d4.mTrigger & (PAD_BUTTON_R | PAD_BUTTON_L)) {
            if (mUnknown5828) {
                mUnknown5828 = FALSE;
            } else {
                gSoundManager.StartSeqArc(1, 0xE, 0x10);
                mUnknown5828 = TRUE;
            }
        }
        break;
    }
    if (mUnknown5828) {
        func_02091578(&mCharacter, 0xFE00);
        if (mCharacter.mModel.mUnknown526 == 0x4000) {
            mUnknown5828 = FALSE;
        }
    }
    func_02090a58(&mCharacter, mTtsPlayer.GetPhonemeName(), 1);
    func_020910a8(&mCharacter);
    func_0207b138(mParticles);
    UpdateCamera();
    func_0202aa28(&data_021227cc, mCamera, sLightDirection0);
    func_0202aa28(&data_021227d8, mCamera, sLightDirection1);
    mUnknown5904 += 0x4CC;
    func_02028108(&data_02121834, 4, mUnknown5904 >> FX32_SHIFT);
    switch (mPage) {
    case 0:
        mPage0.func_ov076_0214c2e4();
        break;
    case 1:
        mFacePage.func_ov076_0214cc34();
        break;
    case 2:
        mProfilePage.func_ov076_021508ac();
        break;
    case 3:
        mVoicePage.func_ov076_02155094();
        break;
    case 4:
        mCharacterPage.func_ov076_02157e3c();
        break;
    case 6:
        mPage6.func_ov076_0215821c();
        break;
    case 5:
        mPage5.func_ov076_0215852c();
        break;
    }
    if (mPage == 0) return;
    if (mPage == 6) return;
    if (mPage == 5) return;
    if (mPage == 2) {
        if (mProfilePage.mState == 10) return;
        if (mProfilePage.mState == 11) return;
        if (mProfilePage.mState == 12) return;
        if (mProfilePage.mState == 13) return;
        if (mProfilePage.mState == 14) return;
        if (mProfilePage.mState == 15) return;
        if (mProfilePage.mState == 16) return;
    }
    if (func_02023fa4(&data_0211d604, 0x3F00)) {
        return;
    }
    for (u16 i = 0; i < 6; i++) {
        if (!func_0203133c(&mButtons[i])) {
            continue;
        }
        switch (i) {
        case 0:
            func_ov076_0214b1ec(1);
            break;
        case 1:
            func_ov076_0214b1ec(2);
            break;
        case 2:
            func_ov076_0214b1ec(3);
            break;
        case 3:
            func_ov076_0214b1ec(4);
            break;
        case 4:
            func_ov076_0214b1ec(6);
            break;
        case 5:
            if (mSceneId == 0x42) {
                CharacterRecord record;
                func_0202c264(&gGameData, (u16)mUnknown5824, &record);
                if (!func_0209d900(&record, &mCharacter)) {
                    func_0203c02c(&gSceneManager, 0x1A);
                } else {
                    func_ov076_0214b1ec(5);
                }
            } else {
                func_ov076_0214b1ec(5);
            }
            break;
        }
        if (i <= 3) {
            for (int j = 0; j <= 3; j++) {
                if (i != j) {
                    func_02031e28(&mButtons[j], FALSE);
                }
            }
        }
    }
}

void MiiEditScene::Draw() {
    if (mUnknown58DC && mUnknown58E0 && !mUnknown58E4) {
        Sprite_Draw(&mSprite582C);
    }
    mText1B94C.ObjTextCanvas::Draw();
    func_02091968(&mCharacter);
    if (mCharacter.IsUnknown24F8Clear()) {
        if (mUnknown580C == TRUE) {
            *(MiiData *)mEditedCharacter.mMii = *(MiiData *)mCharacter.mMii;
            func_0208fefc(&mEditedCharacter);
            VecFx32 v = mCharacter.mUnknown2568;
            mEditedCharacter.mUnknown2568 = v;
            mEditedCharacter.mUnknown25D4 = mCharacter.mUnknown25D4;
            mEditedCharacter.mUnknown25D6 = mCharacter.mUnknown25D6;
            mEditedCharacter.mUnknown25D8 = mCharacter.mUnknown25D8;
            func_0208ff94(&mEditedCharacter, mCharacter.mUnknown2538, mCharacter.mUnknown2514);
            mUnknown580C = FALSE;
        }
    } else {
        func_0208ff94(&mEditedCharacter, mCharacter.mUnknown2538, mCharacter.mUnknown2514);
        func_02090214(&mEditedCharacter);
    }
    func_0202ad84(data_021223e4);
    NNS_G3dGlbFlushP();
    func_0207b258(mParticles, data_02142438);
    for (u16 i = 0; i < 6; i++) {
        func_02031d7c(&mButtons[i]);
    }
    switch (mPage) {
    case 0:
        mPage0.Draw();
        break;
    case 1:
        mFacePage.Draw();
        break;
    case 2:
        mProfilePage.Draw();
        break;
    case 3:
        mVoicePage.Draw();
        break;
    case 4:
        mCharacterPage.Draw();
        break;
    case 6:
        mPage6.Draw();
        break;
    case 5:
        mPage5.Draw();
        break;
    }
}

void MiiEditScene::Exit() {
    func_0202aecc(data_021223e4, 0);
    func_0202aecc(data_021223e4, 0x1F);
    ArchiveManager_Unmount(&gArchiveManager, "E00");
    ArchiveManager_Unmount(&gArchiveManager, "B0I");
    ArchiveManager_Unmount(&gArchiveManager, "BIC");
    ArchiveManager_Unmount(&gArchiveManager, "SPL");
    func_0202683c();
    ArchiveManager_Unmount(&gArchiveManager, "KBD");
    ArchiveManager_Unmount(&gArchiveManager, "TBM");
    ArchiveManager_Unmount(&gArchiveManager, "T02");
    func_02024234(&data_0211d604, 0x3F00, 0);
    func_02023f4c(&data_0211d604, 0x3F00, 0);
    func_02023eac(&data_0211d604, 0x3F00);
    func_02028108(&data_02121834, 4, 0);
    switch (mPage) {
    case 0:
        mPage0.Exit();
        break;
    case 1:
        mFacePage.Exit();
        break;
    case 2:
        mProfilePage.Exit();
        break;
    case 3:
        mVoicePage.Exit();
        break;
    case 4:
        mCharacterPage.Exit();
        break;
    case 6:
        mPage6.Exit();
        break;
    case 5:
        mPage5.Exit();
        break;
    }
}

void MiiEditScene::func_ov076_0214b1ec(int page) {
    switch (mPage) {
    case 0:
        mPage0.Exit();
        break;
    case 1:
        mFacePage.Exit();
        break;
    case 2:
        mProfilePage.Exit();
        break;
    case 3:
        mVoicePage.Exit();
        break;
    case 4:
        mCharacterPage.Exit();
        break;
    case 6:
        mPage6.Exit();
        break;
    case 5:
        mPage5.Exit();
        break;
    }
    mUnknown58E8[page] = TRUE;
    switch (page) {
    case 0:
        NNS_G2dCharCanvasClear(&mText1B94C.mCanvas, 0);
        mText1B94C.mMessage = 0x1A;
        mText1B94C.DrawAll();
        mText1B94C.mSprite.mVisible = FALSE;
        mPage0.func_ov076_0214bf4c(this);
        break;
    case 1:
        NNS_G2dCharCanvasClear(&mText1B94C.mCanvas, 0);
        mText1B94C.mMessage = 0x1C;
        mText1B94C.DrawAll();
        mText1B94C.mSprite.mVisible = TRUE;
        func_ov076_0214ba88(FALSE);
        mFacePage.func_ov076_0214c6d0(this);
        func_02031e28(&mButtons[page - 1], TRUE);
        Sprite_SetFrame(&mButtons[page - 1], 1);
        break;
    case 2:
        if (mSprite582C.mPlace == 1) {
            mUnknown58DC = FALSE;
        }
        NNS_G2dCharCanvasClear(&mText1B94C.mCanvas, 0);
        mText1B94C.mMessage = 0x1D;
        mText1B94C.DrawAll();
        mText1B94C.mSprite.mVisible = TRUE;
        mProfilePage.func_ov076_02150874(this);
        func_02031e28(&mButtons[page - 1], TRUE);
        Sprite_SetFrame(&mButtons[page - 1], 1);
        break;
    case 3:
        if (mSprite582C.mPlace == 2) {
            mUnknown58DC = FALSE;
        }
        NNS_G2dCharCanvasClear(&mText1B94C.mCanvas, 0);
        mText1B94C.mMessage = 0x21;
        mText1B94C.DrawAll();
        mText1B94C.mSprite.mVisible = TRUE;
        mVoicePage.func_ov076_02154690(this);
        func_02031e28(&mButtons[page - 1], TRUE);
        Sprite_SetFrame(&mButtons[page - 1], 1);
        break;
    case 4:
        if (mSprite582C.mPlace == 3) {
            mUnknown58DC = FALSE;
        }
        NNS_G2dCharCanvasClear(&mText1B94C.mCanvas, 0);
        mText1B94C.mMessage = 0x22;
        mText1B94C.DrawAll();
        mText1B94C.mSprite.mVisible = TRUE;
        mCharacterPage.func_ov076_02157c04(this);
        func_02031e28(&mButtons[page - 1], TRUE);
        Sprite_SetFrame(&mButtons[page - 1], 1);
        break;
    case 6:
        mPage6.func_ov076_02158048(this, mPage);
        break;
    case 5:
        mPage5.func_ov076_0215835c(this, mPage);
        break;
    }
    mPage = page;
    mUnknown00154 = 0;
}

void MiiEditScene::func_ov076_0214b5dc(int resource, fx32 offsetY) {
    func_0207b3dc(mParticles);
    MtxFx43 matrix;
    MtxFx33 rotation;
    CharacterModel_GetJointMatrices(&mCharacter.mModel, 0, &matrix, &rotation);
    mUnknown5914.x = matrix.m[3][0];
    mUnknown5914.y = matrix.m[3][1];
    mUnknown5914.z = matrix.m[3][2];
    VecFx32 position = mUnknown5914;
    position.y += offsetY;
    mUnknown590C = func_0207b2fc(mParticles, resource, &position);
}

// Reads the saved character into mCharacter, and sets it up
void MiiEditScene::func_ov076_0214b670(int index) {
    func_0202c264(&gGameData, index, &mCharacter);
    func_ov076_0214b694();
}

void MiiEditScene::func_ov076_0214b694() {
    if (mCharacter.mUnknown054Bits11 <= 1) {
        switch (MiiData_GetSex(&mCharacter)) {
        case 0:
            func_0202aecc(data_021223e4, 0);
            ArchiveManager_Unmount(&gArchiveManager, "B0I");
            func_02091048(&mCharacter, "B0I");
            ArchiveManager_MountCompressed(&gArchiveManager, "B0I", mCharacter.mModel.mArchivePath, 1);
            ModelResources_SetModel(data_021223e4, 0, mCharacter.mModel.mUnknown4E8);
            func_0208ef5c(&mCharacter.mModel, 0, 0);
            break;
        case 1:
            func_0202aecc(data_021223e4, 0);
            ArchiveManager_Unmount(&gArchiveManager, "B0I");
            func_02091048(&mCharacter, "B0I");
            ArchiveManager_MountCompressed(&gArchiveManager, "B0I", mCharacter.mModel.mArchivePath, 1);
            ModelResources_SetModel(data_021223e4, 0, mCharacter.mModel.mUnknown4E8);
            func_0208ef5c(&mCharacter.mModel, 0, 0);
            break;
        }
    } else {
        func_0202aecc(data_021223e4, 0);
        ArchiveManager_Unmount(&gArchiveManager, "B0I");
        func_02091048(&mCharacter, "B0I");
        ArchiveManager_MountCompressed(&gArchiveManager, "B0I", mCharacter.mModel.mArchivePath, 1);
        ModelResources_SetModel(data_021223e4, 0, mCharacter.mModel.mUnknown4E8);
        func_0208ef5c(&mCharacter.mModel, 0, 0);
    }
    void *animations = func_0202ad04(data_021223e4, 0x1F, "BIC", 0);
    func_02029660((void *)mCharacter.mModel.mUnknown04, RESOURCE_BCA0, animations);
    Character_SetAnimation(&mCharacter, RESOURCE_BCA0, 0);
    Character_SetAngle(&mCharacter, 0x4000);
    func_0208fefc(&mCharacter);
    func_02091a88(&mCharacter);
    func_02091a24(&mCharacter);
    func_02091a6c(&mCharacter);
}

static inline void SetFlag(u32 *flags, u8 flag) {
    flags[flag >> 5] |= 1 << (flag & 0x1F);
}

static inline void ClearFlag(u32 *flags, u8 flag) {
    flags[flag >> 5] &= ~(1 << (flag & 0x1F));
}

void MiiEditScene::func_ov076_0214b890() {
    if (mCharacter.mUnknown050Bit0) {
        mCharacter.mUnknown050Bit0 = 0;
        func_0209e01c(&mCharacter);
    }
    int kind = data_020fba28[func_02098f2c(&mCharacter)];
    int style = 8;
    switch (kind) {
    case 0:
        style = MiiData_GetSex(&mCharacter) == 0 ? 6 : 7;
        break;
    case 1:
        style = MiiData_GetSex(&mCharacter) == 0 ? 2 : 3;
        break;
    case 2:
        style = MiiData_GetSex(&mCharacter) == 0 ? 4 : 5;
        break;
    case 3:
        if (MiiData_GetSex(&mCharacter) == 0) {
            style = 0;
        } else {
            style = 1;
        }
        break;
    }
    for (int i = 0; i < 8; i++) {
        if (i == style) {
            SetFlag(mCharacter.mUnknown210, i);
        } else {
            ClearFlag(mCharacter.mUnknown210, i);
        }
    }
    if (mCharacter.mInterior <= 7) {
        mCharacter.mInterior = (u8)style;
    }
    switch (mSceneId) {
    case 0x6F:
        func_0209930c(&mCharacter, 0x13, 0, 0x7FFFFFFFFFFFFFFF);
        mCharacter.mUnknown23A = 1;
        mCharacter.mUnknown227 = 0;
        mCharacter.mUnknown226 = 0;
        mCharacter.mUnknown238 = 0;
        mCharacter.mUnknown230 = 0x7FFFFFFFFFFFFFFF;
        break;
    case 0x43:
    case 0x44:
        func_0209930c(&mCharacter, 0x13, 0, 0x7FFFFFFFFFFFFFFF);
        func_0209b1d8(&mCharacter);
        mCharacter.mUnknown230 = 0x7FFFFFFFFFFFFFFF;
        break;
    }
    func_0202c28c(&gGameData, mUnknown5824, &mCharacter);
}

void MiiEditScene::func_ov076_0214ba88(BOOL a) {
    if (a) {
        mCharacter.mUnknown02D4 = 0;
        mCharacter.mUnknown02D6 = 0;
        func_02029f10(&mCharacter.mModel, 0);
        func_02029f20(&mCharacter.mModel, 0);
        func_02029f30(&mCharacter.mModel, 0);
        func_02029f40(&mCharacter.mModel, 0);
    } else {
        mCharacter.mUnknown02D4 = GX_RGB(31, 31, 31);
        mCharacter.mUnknown02D6 = GX_RGB(8, 8, 8);
        func_02029f10(&mCharacter.mModel, GX_RGB(31, 31, 31));
        func_02029f20(&mCharacter.mModel, GX_RGB(8, 8, 8));
    }
}

void MiiEditScene::func_ov076_0214bb24() {
    for (u16 i = 0; i < 9; i++) {
        mTextBoxes[i].mSprite.mVisible = TRUE;
    }
    mUnknown58E4 = FALSE;
    for (int i = 0; i < 6; i++) {
        mButtons[i].mVisible = TRUE;
    }
    if (mSceneId == 0x6F) {
        mButtons[5].mVisible = FALSE;
    }
    func_02027d7c(&data_02121834, "E00:/BG_down.nsc", 0x200, 0, 0, 0, 0, -1);
    data_0211d604.mPlanes = 0x1317;
    data_0211d604.mPlanesChanged = TRUE;
}

// --- Page 0 ---

extern "C" {
void func_02033448(MessageWindow *window, int number, int a, int sequence, u8 sequence2, WindowPoint position,
                   WindowPoint size, const ButtonSounds *sounds, int b);
void func_02033844(MessageWindow *window, int number, int a, u8 sequence, u8 sequence2, WindowPoint position,
                   WindowPoint size, MessageFile *file, u32 message, const NNSG2dFont *font, int plane,
                   int *charOffset, const ButtonSounds *sounds, int b);
}

void MiiEditPage0::func_ov076_0214bf4c(MiiEditScene *scene) {
    mState = -1;
    mScene = scene;
    int charOffset = 900;
    mScene->func_ov076_0214bb24();
    ScreenRect rect = sWindowRect;
    for (int i = 0; i < 2; i++) {
        func_02032fb8(&mWindows[i], rect);
        func_02033174(&mWindows[i], sWindowScreenName, 0x100);
        func_020331bc(&mWindows[i], 0x3F00, rect);
        func_02034f4c(&mWindows[i], 1);
    }
    int number = 0;
    for (int i = 0; i <= 1; i++) {
        if (mScene->mSceneId == 0x6F) {
            func_02033448(&mWindows[0], number, 0, data_ov076_02158750[i], data_ov076_02158750[i] + 1,
                          data_ov076_02158a7c[i], data_ov076_02158a9c[i], &data_ov076_02158e94[i], 14);
        } else {
            func_02033448(&mWindows[0], number, 0, data_ov076_02158750[i], data_ov076_02158750[i] + 1,
                          data_ov076_02158a5c[i], data_ov076_02158a9c[i], &data_ov076_02158e94[i], 14);
        }
        number++;
    }
    if (mScene->mSceneId == 0x43) {
        func_02033844(&mWindows[0], number, 0, 0x69, 0x6A, data_ov076_021587a0, data_ov076_02158808,
                      &mScene->mMessageFiles[1], 5, &gFonts[2].mFont, 0x1000, &charOffset, &data_ov076_021588ec, 1);
    }
    charOffset = func_02033a58(&mWindows[0], &mScene->mMessageFiles[0], 0x1A, &gFonts[0].mFont, 0x1000,
                               data_ov076_021587e0, &charOffset);
    func_02034f4c(&mWindows[0], 1);
    func_02034f94(&mWindows[0], 1);
    number = 0;
    for (int i = 2; i <= 3; i++) {
        func_02033448(&mWindows[1], number, 0, data_ov076_02158750[i], data_ov076_02158750[i] + 1,
                      data_ov076_02158a5c[i], data_ov076_02158a9c[i], &data_ov076_02158e94[i], 14);
        number++;
    }
    charOffset = func_02033a58(&mWindows[1], &mScene->mMessageFiles[0], 0x1B, &gFonts[0].mFont, 0x1000,
                               data_ov076_021587d8, &charOffset);
    func_02034f4c(&mWindows[1], 1);
    func_ov076_0214c624(0);
}

extern "C" {
void func_020341d4(MessageWindow *window);
void func_020349cc(MessageWindow *window, int flags);
void func_02098cd0(CharacterRecord *character, int sex);
void func_0208ff44(SceneCharacter *character, int a, int b);
void func_02098d68(CharacterRecord *character, int index, s16 value);
extern const s16 data_020fbaac[];
void func_0209859c(CharacterRecord *character);
void func_0209cbac(CharacterRecord *character, u8 *a, u8 *b);
void func_02098c40(CharacterRecord *character, int kind, int value);
void func_02091a14(SceneCharacter *character, BOOL visible);
void MiiData_GetName(CharacterRecord *character, wchar_t *name);
void MiiData_SetName(CharacterRecord *character, const wchar_t *name);
void MiiData_GetCreatorName(CharacterRecord *character, wchar_t *name);
void MiiData_SetCreatorName(CharacterRecord *character, const wchar_t *name);
}

void MiiEditPage0::func_ov076_0214c2e4() {
    switch (mState) {
    case 0:
        func_020341d4(&mWindows[0]);
        // The original reads mResult again after this test, which only a volatile read gives here (making the
        // field volatile in MessageWindow.h breaks PreStartScene::Ask)
        if (*(volatile int *)&mWindows[0].mResult != -1) {
            int result = mWindows[0].mResult;
            if (result == 2) {
                func_0203c02c(&gSceneManager, 0x5E);
            } else {
                mWindows[0].mUnknownSetBeforeClose = 15;
                func_020349cc(&mWindows[0], 0x24);
                func_02098cd0(&mScene->mCharacter, result);
                switch (MiiData_GetSex(&mScene->mCharacter)) {
                case 0:
                    func_0208ff44(&mScene->mCharacter, 2, 1);
                    for (int i = 0; i < 6; i++) {
                        func_02098d68(&mScene->mCharacter, i, data_020fbaac[i + 12]);
                    }
                    break;
                case 1:
                    func_0208ff44(&mScene->mCharacter, 5, 1);
                    for (int i = 0; i < 6; i++) {
                        func_02098d68(&mScene->mCharacter, i, data_020fbaac[i + 18]);
                    }
                    mScene->mUnknown580C = TRUE;
                    break;
                }
                func_0209859c(&mScene->mCharacter);
                u8 a;
                u8 b;
                func_0209cbac(&mScene->mCharacter, &a, &b);
                mScene->mCharacter.mUnknown290 = a;
                mScene->mCharacter.mUnknown291 = b;
                func_02098c40(&mScene->mCharacter, 0x28, 0);
                func_02098c40(&mScene->mCharacter, 0x24, 0);
                MiiData_SetName(&mScene->mCharacter, L"");
                mScene->func_ov076_0214b694();
                func_02091a14(&mScene->mCharacter, FALSE);
                mScene->func_ov076_0214ba88(TRUE);
                if (mScene->mUnknown5824 != 0) {
                    wchar_t name[11];
                    SavedCharacter first;
                    MI_CpuClear16(name, sizeof(name));
                    func_0202c264(&gGameData, 0, &first);
                    MiiData_GetCreatorName(&first, name);
                    if (mScene->func_ov076_0214bf20(name) == 0) {
                        MI_CpuClear16(name, sizeof(name));
                        MiiData_GetName(&first, name);
                    }
                    MiiData_SetCreatorName(&mScene->mCharacter, name);
                } else {
                    mScene->mCharacter.mUnknown298 = 1;
                }
            }
        }
        if (!mWindows[0].mTransitionDone) {
            return;
        }
        func_02091a14(&mScene->mCharacter, TRUE);
        mScene->func_ov076_0214b1ec(1);
        break;
    case 1:
        func_020341d4(&mWindows[1]);
        if (mWindows[1].mResult != -1) {
            mWindows[1].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindows[1], 0x24);
            switch (mWindows[1].mResult) {
            case 0:
                mNextPage = 1;
                break;
            case 1:
                mNextPage = 2;
                break;
            }
        }
        if (!mWindows[1].mTransitionDone) {
            return;
        }
        func_02091a14(&mScene->mCharacter, TRUE);
        mScene->func_ov076_0214b1ec(mNextPage);
        break;
    }
}

void MiiEditPage0::func_ov076_0214c624(int state) {
    switch (state) {
    case 0:
        func_02034720(&mWindows[0], 0x12, 1);
        break;
    case 1:
        func_02034720(&mWindows[1], 0x12, 1);
        break;
    }
    mState = state;
    mUnknown00004 = 0;
}

void MiiEditPage0::Draw() {
    for (int i = 0; i < 2; i++) {
        func_020345d8(&mWindows[i]);
    }
}

void MiiEditPage0::Exit() {
    for (int i = 0; i < 2; i++) {
        func_020346cc(&mWindows[i]);
    }
}

// Shows the scene's own buttons and texts again, and the lower screen's background (the pages call it when they
// close)
void MiiEditScene::func_ov076_0214bbe4() {
    for (u16 i = 0; i < 9; i++) {
        mTextBoxes[i].mSprite.mVisible = TRUE;
    }
    mUnknown58E4 = FALSE;
    for (int i = 0; i < 6; i++) {
        mButtons[i].mVisible = TRUE;
    }
    if (mSceneId == 0x6F) {
        mButtons[5].mVisible = FALSE;
    }
    func_02027d7c(&data_02121834, "E00:/BG_down.nsc", 0x200, 0, 0, 0, 0, -1);
    data_0211d604.mPlanes = 0x1217;
    data_0211d604.mPlanesChanged = TRUE;
}

void MiiEditScene::func_ov076_0214bca4() {
    for (u16 i = 0; i < 9; i++) {
        mTextBoxes[i].mSprite.mVisible = TRUE;
    }
    mUnknown58E4 = FALSE;
    for (int i = 0; i < 6; i++) {
        mButtons[i].mVisible = TRUE;
    }
    if (mSceneId == 0x6F) {
        mButtons[5].mVisible = FALSE;
    }
    func_02027d7c(&data_02121834, "E00:/BG_down_editface.nsc", 0x200, 0, 0, 0, 0, -1);
    data_0211d604.mPlanes = 0x1217;
    data_0211d604.mPlanesChanged = TRUE;
}

void MiiEditScene::func_ov076_0214bd64() {
    for (u16 i = 0; i < 9; i++) {
        mTextBoxes[i].mSprite.mVisible = FALSE;
    }
    mUnknown58E4 = TRUE;
    for (int i = 0; i < 6; i++) {
        mButtons[i].mVisible = FALSE;
    }
    data_0211d604.mPlanes = 0x1317;
    data_0211d604.mPlanesChanged = TRUE;
}

void MiiEditScene::func_ov076_0214bdd8() {
    for (u16 i = 0; i < 9; i++) {
        mTextBoxes[i].mSprite.mVisible = FALSE;
    }
    mUnknown58E4 = TRUE;
    for (int i = 0; i < 6; i++) {
        mButtons[i].mVisible = FALSE;
    }
    func_02027d7c(&data_02121834, "E00:/BG_down_nobar.nsc", 0x200, 0, 0, 0, 0, -1);
    data_0211d604.mPlanes = 0x1217;
    data_0211d604.mPlanesChanged = TRUE;
}

void MiiEditSpriteB0::func_ov076_0214be80(int place) {
    SetPosition(FxVec2(sSpriteB0Places[place].x << FX32_SHIFT, sSpriteB0Places[place].y << FX32_SHIFT));
    mPlace = place;
}

void MiiEditSpriteB0::func_ov076_0214bec8(int place) {
    if (place <= mPlace) {
        return;
    }
    SetPosition(FxVec2(sSpriteB0Places[place].x << FX32_SHIFT, sSpriteB0Places[place].y << FX32_SHIFT));
    mPlace = place;
}

// Returns the length of the name once func_02092178 has been through it
int MiiEditScene::func_ov076_0214bf20(const wchar_t *name) {
    wchar_t buffer[11];
    wcsncpy(buffer, name, 11);
    func_02092178(buffer);
    return wcslen(buffer);
}

// --- The face page ---

void MiiEditFacePage::Draw() {
    for (u16 i = 0; i < 12; i++) {
        func_02031d7c(&mButtons227C[i]);
    }
    for (int i = 0; i < 2; i++) {
        Sprite_Draw(&mSprites2DF4[i].mSprite);
        Sprite_Draw(&mSprites2FF4[i]);
    }
    Sprite_Draw(&mSprite315C);
    Sprite_Draw(&mSprite32B4);
    Sprite_Draw(&mSprite3208);
    Sprite_Draw(&mSprite3360);
    for (int i = 0; i < 36; i++) {
        func_02031d7c(&mButtons[i]);
    }
}

void MiiEditFacePage::Exit() {}

// --- The profile page ---

void MiiEditProfilePage::Draw() {
    for (u16 i = 0; i < 2; i++) {
        Sprite_Draw(&mSprites[i]);
    }
    for (u16 i = 0; i < 9; i++) {
        mScene->mTextBoxes[i].Draw();
    }
    mScene->mText1B35C.ObjTextCanvas::Draw();
    for (u16 i = 0; i < 10; i++) {
        func_02031d7c(&mButtons[i]);
    }
    for (int i = 0; i < 6; i++) {
        func_020345d8(&mWindows[i]);
    }
    switch (mState) {
    case 3:
    case 5:
    case 7:
    case 9:
        func_02083bcc(&mKeyboard);
        mScene->mText1B35C.ObjTextCanvas::Draw();
        mScene->mCharBox.Draw();
        func_02031d7c(&mButton);
        break;
    }
}

void MiiEditProfilePage::Exit() {
    for (int i = 0; i < 6; i++) {
        func_020346cc(&mWindows[i]);
    }
    switch (mState) {
    case 3:
    case 5:
    case 7:
    case 9:
        func_02027d7c(&data_02121834, "E00:/BG_down_editface.nsc", 0x200, 0, 0, 0, 0, -1);
        func_02027d7c(&data_02121834, "E00:/Window_down.nsc", 0x100, 0, 0, 0, 0, -1);
        func_02028f24(&gVramTransferQueue, "E00:/BG_down.ncg", 0x200, 0, 0, 0, 0);
        func_02027534(&data_021213b8, "E00:/BG_down.ncl", 0x200, -1, -1, -1);
        mScene->func_ov076_0214bbe4();
        break;
    }
}

// --- The voice page ---

void MiiEditVoicePage::Draw() {
    for (u16 i = 0; i < 22; i++) {
        func_02031d7c(&mButtons[i]);
    }
    for (u16 i = 0; i < 3; i++) {
        func_0203206c(&mTextButtons[i]);
    }
    for (int i = 0; i < 5; i++) {
        Sprite_Draw(&mSprites2B2C[i].mSprite);
    }
    for (int i = 0; i < 2; i++) {
        Sprite_Draw(&mSprites29D4[i]);
    }
}

void MiiEditVoicePage::Exit() {
    func_ov076_02157bc0();
    if (mState != 4) {
        return;
    }
    func_02027d7c(&data_02121834, "E00:/BG_down_editface.nsc", 0x200, 0, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, "E00:/Window_down.nsc", 0x100, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "E00:/BG_down.ncg", 0x200, 0, 0, 0, 0);
    func_02027534(&data_021213b8, "E00:/BG_down.ncl", 0x200, -1, -1, -1);
    mScene->func_ov076_0214bbe4();
}

void MiiEditVoicePage::func_ov076_02157bc0() {
    func_ov076_02157508(0);
}

// --- The personality page ---

void MiiEditCharacterPage::Draw() {
    for (u16 i = 0; i < 5; i++) {
        func_02031d7c(&mButtons[i]);
    }
    for (int i = 0; i < 5; i++) {
        Sprite_Draw(&mSprites[i]);
    }
}

void MiiEditCharacterPage::Exit() {}

// --- Pages 5 and 6 ---

void MiiEditPage6::Draw() {
    func_020345d8(&mWindow);
}

void MiiEditPage6::Exit() {
    func_020346cc(&mWindow);
}

void MiiEditPage5::Draw() {
    func_020345d8(&mWindow);
}

void MiiEditPage5::Exit() {
    func_020346cc(&mWindow);
}

// TEMPORARY: refers to every object of .rodata, so that the compiler keeps the ones that only functions not
// written yet use (an unreferenced static is dropped, and the pool would close up). Remove it when they are.
extern "C" void func_ov076_KeepData(const void **p) {
    p[0] = &data_ov076_02158748;
    p[1] = &data_ov076_0215874c;
    p[2] = &data_ov076_02158750;
    p[3] = &data_ov076_02158754;
    p[4] = &data_ov076_02158770;
    p[5] = &data_ov076_0215878c;
    p[6] = &data_ov076_02158758;
    p[7] = &data_ov076_02158764;
    p[8] = &data_ov076_02158774;
    p[9] = &data_ov076_02158780;
    p[10] = &data_ov076_02158778;
    p[11] = &data_ov076_0215877c;
    p[12] = &data_ov076_02158784;
    p[13] = &data_ov076_02158788;
    p[14] = &data_ov076_02158768;
    p[15] = &data_ov076_02158760;
    p[16] = &data_ov076_0215876c;
    p[17] = &data_ov076_0215875c;
    p[18] = &data_ov076_02158790;
    p[19] = &data_ov076_02158798;
    p[20] = &data_ov076_021587a0;
    p[21] = &data_ov076_021587a8;
    p[22] = &data_ov076_021587b0;
    p[23] = &data_ov076_021587b8;
    p[24] = &data_ov076_021587c0;
    p[25] = &data_ov076_021587c8;
    p[26] = &data_ov076_021587d0;
    p[27] = &data_ov076_021587d8;
    p[28] = &data_ov076_021587e0;
    p[29] = &data_ov076_021587e8;
    p[30] = &data_ov076_021587f0;
    p[31] = &data_ov076_021587f8;
    p[32] = &data_ov076_02158800;
    p[33] = &data_ov076_02158808;
    p[34] = &data_ov076_02158810;
    p[35] = &data_ov076_02158818;
    p[36] = &data_ov076_02158822;
    p[37] = &data_ov076_0215882c;
    p[38] = &data_ov076_02158838;
    p[39] = &data_ov076_02158844;
    p[40] = &data_ov076_02158850;
    p[41] = &data_ov076_0215885c;
    p[42] = &data_ov076_02158868;
    p[43] = &data_ov076_02158874;
    p[44] = &sButtonSequences;
    p[45] = &data_ov076_0215888c;
    p[46] = &sLightDirection1;
    p[47] = &sLightDirection0;
    p[48] = &data_ov076_021588b0;
    p[49] = &data_ov076_021588bc;
    p[50] = &data_ov076_021588cc;
    p[51] = &sWindowRect;
    p[52] = &data_ov076_021588ec;
    p[53] = &data_ov076_021588fc;
    p[54] = &data_ov076_0215890c;
    p[55] = &data_ov076_0215891c;
    p[56] = &data_ov076_0215892c;
    p[57] = &data_ov076_0215893c;
    p[58] = &data_ov076_0215894c;
    p[59] = &data_ov076_0215895c;
    p[60] = &data_ov076_02158970;
    p[61] = &data_ov076_02158984;
    p[62] = &data_ov076_0215899c;
    p[63] = &sButtonKinds;
    p[64] = &data_ov076_021589cc;
    p[65] = &data_ov076_021589e4;
    p[66] = &data_ov076_021589fc;
    p[67] = &data_ov076_02158a1c;
    p[68] = &data_ov076_02158a3c;
    p[69] = &data_ov076_02158a5c;
    p[70] = &data_ov076_02158a7c;
    p[71] = &data_ov076_02158a9c;
    p[72] = &data_ov076_02158abc;
    p[73] = &data_ov076_02158adc;
    p[74] = &data_ov076_02158afc;
    p[75] = &data_ov076_02158b1c;
    p[76] = &data_ov076_02158b3c;
    p[77] = &data_ov076_02158b60;
    p[78] = &sSpriteB0Places;
    p[79] = &data_ov076_02158bb0;
    p[80] = &data_ov076_02158bd8;
    p[81] = &data_ov076_02158c00;
    p[82] = &data_ov076_02158c28;
    p[83] = &data_ov076_02158c50;
    p[84] = &data_ov076_02158c78;
    p[85] = &sButtonSizes;
    p[86] = &data_ov076_02158cd4;
    p[87] = &data_ov076_02158d04;
    p[88] = &data_ov076_02158d34;
    p[89] = &data_ov076_02158d64;
    p[90] = &data_ov076_02158d94;
    p[91] = &sButtonPositions;
    p[92] = &data_ov076_02158df4;
    p[93] = &sWindowScreenName;
    p[94] = &data_ov076_02158e48;
    p[95] = &data_ov076_02158e60;
    p[96] = &data_ov076_02158e94;
    p[97] = &data_ov076_02158ed4;
    p[98] = &data_ov076_02158f14;
    p[99] = &data_ov076_02158f54;
    p[100] = &data_ov076_02158f9c;
    p[101] = &data_ov076_02158fe4;
    p[102] = &data_ov076_02159034;
    p[103] = &data_ov076_02159084;
    p[104] = &sButtonSounds;
    p[105] = &data_ov076_0215913c;
    p[106] = &data_ov076_0215919c;
    p[107] = &data_ov076_021591fc;
    p[108] = &data_ov076_0215925c;
    p[109] = &data_ov076_021592d0;
    p[110] = &data_ov076_02159350;
    p[111] = &data_ov076_021593e0;
    p[112] = &data_ov076_02159480;
    p[113] = &data_ov076_02159530;
    p[114] = &data_ov076_021595e0;
    p[115] = &data_ov076_021596a0;
    p[116] = &data_ov076_02159788;
    p[117] = &data_ov076_02159870;
    p[118] = &data_ov076_02159990;
    p[119] = &data_ov076_02159ab0;
    p[120] = &data_ov076_02159c10;
    p[121] = &data_ov076_02159de0;
}

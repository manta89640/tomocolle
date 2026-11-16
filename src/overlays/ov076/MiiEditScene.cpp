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
static const u16 data_ov076_02158880[6] = { 0x58, 0x5a, 0x5c, 0x5e, 0x28, 0x2a };
static const VecFx32 data_ov076_0215885c = { 0, 7248, 105456 };
static const VecFx32 data_ov076_0215888c = { 0, 1, 1 };
static const VecFx32 sLightDirection1 = { 0, -4000, 1500 };
static const VecFx32 sLightDirection0 = { 1600, -600, -4000 };
static const VecFx32 data_ov076_02158838 = { 0, 0, -32768 };
static const int data_ov076_021588cc[4] = { 128, 96, 128, 96 };
static const ScreenRect data_ov076_021588dc = { 20, 20, 216, 152 };
static const int data_ov076_021588ec[4] = { 16, -1, 0, -1 };
static const ScreenPoint data_ov076_021588bc[2] = { { 202, 112 }, { 52, 8 } };
static const int data_ov076_021588fc[4] = { 75, 149, 181, 149 };
static const int data_ov076_0215890c[4] = { 145, 65, 145, 119 };
static const int data_ov076_0215891c[4] = { 5, -1, 19, -1 };
static const int data_ov076_0215894c[4] = { 94, 30, 94, 30 };
static const ScreenPoint data_ov076_0215892c[2] = { { 80, 77 }, { 123, 77 } };
static const int data_ov076_0215893c[4] = { 14, 15, 16, 17 };
static const u16 data_ov076_0215895c[10] = { 0x0, 0x2, 0x4, 0x6, 0xa, 0xc, 0x10, 0x14, 0x16, 0xe };
static const int data_ov076_02158970[5] = { 4, 4, 4, 4, 4 };
static const int data_ov076_02158984[6] = { 62, 22, 116, 30, 116, 30 };
static const int data_ov076_0215899c[6] = { 14, 14, 14, 14, 14, 1 };
static const int data_ov076_021589b4[6] = { 3, 3, 3, 3, 4, 4 };
static const int data_ov076_021589cc[6] = { 136, 132, 62, 175, 194, 175 };
static const int data_ov076_021589e4[6] = { 14, 15, 16, 17, 22, 23 };
static const int data_ov076_02158a7c[8] = { 80, 102, 176, 102, 80, 90, 176, 90 };
static const int data_ov076_02158a9c[8] = { 58, 54, 58, 54, 58, 54, 58, 54 };
static const WindowPoint data_ov076_02158abc[4] = { { 158, 76 }, { 138, 76 }, { 118, 76 }, { 98, 76 } };
static const WindowPoint data_ov076_02158adc[4] = { { 170, 76 }, { 150, 76 }, { 90, 76 }, { 70, 76 } };
static const ScreenPoint data_ov076_02158afc[4] = { { 80, 77 }, { 123, 77 }, { 80, 114 }, { 123, 114 } };
static const int data_ov076_02158b1c[8] = { 14, 15, 16, 17, 18, 19, 20, 21 };
static const ScreenPoint data_ov076_021589fc[4] = { { 207, 48 }, { 239, 48 }, { 207, 74 }, { 239, 74 } };
static const int data_ov076_02158a1c[8] = { 14, -1, 0, -1, 16, -1, 0, -1 };
static const int data_ov076_02158a3c[8] = { 15, -1, 0, -1, 16, -1, 0, -1 };
static const int data_ov076_02158a5c[8] = { 80, 90, 176, 90, 80, 90, 176, 90 };
static const int data_ov076_02158b3c[9] = { 0, 1, 4, 2, 3, 6, 7, 8, 9 };
static const int data_ov076_02158b60[10] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };
static const ScreenPoint sSpriteB0Places[5] = { { 55, 182 }, { 55, 182 }, { 103, 182 }, { 151, 182 }, { 199, 182 } };
static const int data_ov076_02158bb0[10] = { 128, 26, 128, 59, 128, 92, 128, 125, 128, 158 };
static const int data_ov076_02158bd8[10] = { 128, 112, 128, 88, 128, 64, 128, 16, 128, 40 };
static const int data_ov076_02158c00[10] = { 8, 20, 8, 20, 8, 20, 8, 20, 8, 20 };
static const int data_ov076_02158c28[10] = { 128, 26, 128, 59, 128, 92, 128, 125, 128, 158 };
static const int data_ov076_02158c50[10] = { 12, 12, 12, 12, 12, 12, 12, 12, 12, 12 };
static const u16 data_ov076_02158c78[22] = { 0x0, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x20, 0x22, 0x24, 0x28, 0x2a, 0x2c, 0x2e };
static const int data_ov076_02158ca4[12] = { 40, 18, 40, 18, 40, 18, 40, 18, 40, 18, 24, 22 };
static const int data_ov076_02158cd4[12] = { 20, -1, 0, -1, 22, -1, 0, -1, 20, -1, 0, -1 };
static const int data_ov076_02158d04[12] = { 1, 10, 2, 3, 6, 0, 5, 4, 8, 9, 7, 11 };
static const int data_ov076_02158d34[12] = { 21, 19, 21, 21, 22, 22, 23, 23, 24, 24, 23, 22 };
static const ScreenPoint data_ov076_02158d64[6] = { { 207, 48 }, { 239, 48 }, { 207, 74 }, { 239, 74 }, { 207, 100 }, { 239, 100 } };
static const ButtonSounds data_ov076_02158d94[3] = { { { 20, -1, 0, -1 } }, { { 21, -1, 0, -1 } }, { { 22, -1, 0, -1 } } };
static const int data_ov076_02158dc4[12] = { 32, 181, 80, 181, 128, 181, 176, 181, 224, 181, 242, 13 };
static const u16 data_ov076_02158df4[30] = { 0x18, 0x1a, 0x1c, 0x1e, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x20, 0x22, 0x3c, 0x3e, 0x3c, 0x3e, 0x40, 0x42, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x0 };
static const char data_ov076_02158e30[] = "E00:/Window_down.nsc";
static const u8 data_ov076_02158e48[24] = { 0x1a, 0x1a, 0x1b, 0x19, 0x1a, 0x19, 0x1a, 0x19, 0x1c, 0x19, 0x1a, 0x18, 0x1b, 0x1b, 0x1a, 0x1a, 0x19, 0x19, 0x1a, 0x1a, 0x1b, 0x1a, 0x19, 0x1b };
static const u8 data_ov076_02158e60[50] = { 0x1d, 0x1c, 0x1c, 0x1c, 0x1d, 0x1c, 0x1c, 0x1c, 0x1d, 0x1c, 0x1c, 0x1c, 0x1c, 0x1d, 0x1d, 0x1c, 0x1c, 0x1c, 0x1d, 0x1d, 0x1c, 0x1d, 0x1c, 0x1d, 0x1d, 0x1c, 0x1d, 0x1c, 0x1c, 0x1d, 0x1c, 0x1c, 0x1c, 0x1d, 0x1d, 0x1d, 0x1c, 0x1c, 0x1d, 0x1d, 0x1d, 0x1c, 0x1c, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1c, 0x1c };
static const int data_ov076_02158e94[16] = { 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1 };
static const ScreenPoint data_ov076_02158ed4[8] = { { 80, 40 }, { 123, 40 }, { 80, 77 }, { 123, 77 }, { 80, 114 }, { 123, 114 }, { 80, 151 }, { 123, 151 } };
static const int data_ov076_02158f14[16] = { 207, 48, 239, 48, 207, 74, 239, 74, 207, 100, 239, 100, 207, 126, 239, 126 };
static const u16 data_ov076_02158f54[36] = { 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e, 0x40, 0x42, 0x44, 0x46, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x0, 0x48, 0x4a, 0x4c, 0x4e, 0x50, 0x52, 0x54, 0x56 };
static const ScreenPoint data_ov076_02158f9c[9] = { { 59, 77 }, { 102, 77 }, { 145, 77 }, { 59, 114 }, { 102, 114 }, { 145, 114 }, { 59, 151 }, { 102, 151 }, { 145, 151 } };
static const WindowPoint data_ov076_02158fe4[10] = { { 70, 40 }, { 186, 40 }, { 128, 112 }, { 160, 76 }, { 68, 76 }, { 202, 112 }, { 216, 76 }, { 68, 149 }, { 192, 149 }, { 54, 112 } };
static const WindowPoint data_ov076_02159034[10] = { { 116, 18 }, { 116, 18 }, { 32, 18 }, { 56, 18 }, { 112, 18 }, { 32, 18 }, { 56, 18 }, { 112, 18 }, { 48, 18 }, { 48, 18 } };
static const int data_ov076_02159084[22] = { 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3 };
static const int data_ov076_021590dc[24] = { 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 20, -1, 0, -1, 16, -1, 0, -1 };
static const int data_ov076_0215913c[24] = { 6, 0, 12, 1, 9, 19, 7, 21, 8, 17, 5, 4, 11, 10, 2, 3, 14, 20, 15, 13, 22, 18, 16, 23 };
static const int data_ov076_0215919c[24] = { 23, 1, 19, 21, 22, 5, 0, 8, 10, 16, 6, 13, 7, 9, 2, 17, 3, 4, 15, 11, 20, 18, 14, 12 };
static const ScreenPoint data_ov076_021591fc[12] = { { 59, 40 }, { 102, 40 }, { 145, 40 }, { 59, 77 }, { 102, 77 }, { 145, 77 }, { 59, 114 }, { 102, 114 }, { 145, 114 }, { 59, 151 }, { 102, 151 }, { 145, 151 } };
static const int data_ov076_0215925c[29] = { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5 };
static const int data_ov076_021592d0[32] = { 73, -6, 12, 12, 87, -6, 12, 12, 101, -6, 12, 12, 115, -6, 12, 12, 129, -6, 12, 12, 143, -6, 12, 12, 157, -6, 12, 12, 171, -6, 12, 12 };
static const int data_ov076_02159350[36] = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3 };
static const ButtonSounds data_ov076_021593e0[10] = { { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } } };
static const int data_ov076_02159480[44] = { 68, 52, 92, 52, 116, 52, 140, 52, 164, 52, 188, 52, 68, 92, 68, 132, 206, 134, 237, 134, 237, 134, 206, 20, 237, 20, 206, 45, 237, 45, 206, 70, 237, 70, 206, 95, 99, 138, 118, 138, 137, 138, 156, 138 };
static const int data_ov076_02159530[44] = { 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 30, 24, 30, 24, 30, 24, 30, 24, 30, 24, 30, 24, 30, 24, 30, 24, 30, 24, 30, 24, 16, 16, 16, 16, 16, 16, 16, 16 };
static const int data_ov076_021595e0[48] = { 2, 4, 0, 8, 39, 17, 1, 26, 16, 15, 27, 20, 33, 11, 19, 32, 9, 12, 23, 34, 21, 25, 40, 35, 5, 41, 13, 36, 37, 6, 24, 30, 31, 18, 28, 46, 7, 44, 38, 42, 45, 29, 3, 43, 22, 10, 14, 47 };
static const WindowPoint data_ov076_021596a0[29] = { { 58, 80 }, { 105, 80 }, { 152, 80 }, { 199, 80 }, { 48, 64 }, { 80, 64 }, { 112, 64 }, { 144, 64 }, { 176, 64 }, { 208, 64 }, { 48, 96 }, { 80, 96 }, { 112, 96 }, { 144, 96 }, { 176, 96 }, { 208, 96 }, { 80, 80 }, { 176, 80 }, { 128, 41 }, { 128, 111 }, { 80, 41 }, { 80, 111 }, { 160, 41 }, { 160, 111 }, { 128, 48 }, { 128, 72 }, { 128, 96 }, { 128, 120 }, { 128, 144 } };
static const WindowPoint data_ov076_02159788[29] = { { 42, 36 }, { 42, 36 }, { 42, 36 }, { 42, 36 }, { 24, 24 }, { 24, 24 }, { 24, 24 }, { 24, 24 }, { 24, 24 }, { 24, 24 }, { 24, 24 }, { 24, 24 }, { 24, 24 }, { 24, 24 }, { 24, 24 }, { 24, 24 }, { 58, 54 }, { 58, 54 }, { 20, 15 }, { 20, 15 }, { 20, 15 }, { 20, 15 }, { 20, 15 }, { 20, 15 }, { 126, 20 }, { 126, 20 }, { 126, 20 }, { 126, 20 }, { 126, 20 } };
static const int data_ov076_02159870[72] = { 33, 40, 51, 44, 39, 70, 45, 49, 59, 56, 68, 31, 32, 47, 37, 48, 66, 52, 58, 50, 55, 64, 60, 62, 43, 38, 42, 23, 67, 54, 36, 41, 65, 30, 57, 34, 12, 13, 69, 26, 4, 25, 1, 19, 5, 8, 27, 7, 14, 3, 22, 10, 6, 20, 11, 63, 17, 35, 21, 0, 61, 16, 46, 9, 18, 2, 28, 53, 71, 24, 15, 29 };
static const int data_ov076_02159990[72] = { 17, 159, 17, 12, 17, 33, 17, 54, 17, 75, 17, 96, 17, 117, 17, 138, 77, 11, 127, 11, 77, 11, 127, 11, 77, 30, 127, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 223, 74, 68, 11, 136, 11, 223, 155, 179, 30, 179, 48, 179, 66, 179, 84, 179, 102, 179, 120, 179, 138, 179, 156 };
static const int data_ov076_02159ab0[88] = { 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 14, -1, 0, -1, 1, -1, 0, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1 };
static const ButtonSounds data_ov076_02159c10[29] = { { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 17, -1, -1, -1 } }, { { 18, -1, -1, -1 } }, { { 17, -1, -1, -1 } }, { { 18, -1, -1, -1 } }, { { 17, -1, -1, -1 } }, { { 18, -1, -1, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } } };
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
    /* 0x00 */ u32 mTrigger;
    /* 0x04 */ u8 mUnknown04[0x1C];
    /* 0x20 */ u32 mUnknown20;
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

// --- The face page's parts ---

extern "C" {
BOOL func_02031d00(SpriteButton *button); // Whether it is selected (mUnknownB0)
void func_02031ca4(SpriteButton *button); // Shows its pressed sequence, playing
// A Mii parameter of the character (numbered as the Mii data's getters and setters are, data_020fbc2c)
int func_02098dac(CharacterBase *character, int parameter);
void func_020986c4(CharacterBase *character, int parameter, int *min, int *max); // Its range
void func_0208fed8(CharacterBase *character, int parameter, int value, BOOL a);  // Sets it
void ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
}

// The part type that each of the parts' buttons shows, in the order of the buttons, is in data_ov076_02159870
// (hair, Mii parameter 0x19), data_ov076_0215913c (eyebrows, 6), data_ov076_021595e0 (eyes, 0), data_ov076_02158d04
// (nose, 0x1C) and data_ov076_0215919c (mouth, 0xC). By eye type and by eyebrow type, data_ov076_02158e60 and
// data_ov076_02158e48: SetParameter turns the eyes or the eyebrows by the difference between the old type's entry
// and the new one's.

// Shows which of the eight colour buttons (28 to 35) is selected, and loads the parts' palette for the category
void MiiEditFacePage::func_ov076_0214ee7c(int selected, int category, int tab) {
    for (int i = 0; i <= 7; i++) {
        if (i == selected) {
            if (!func_02031d00(&mButtons[i + 28])) {
                func_02031e28(&mButtons[i + 28], TRUE);
            }
        } else {
            func_02031e28(&mButtons[i + 28], FALSE);
        }
    }
    int row;
    switch (category) {
    case 1:
        switch (tab) {
        case 0:
            row = 1;
            break;
        case 1:
            row = 0;
            break;
        }
        break;
    case 0:
    case 2:
    case 3:
    case 5:
    case 7:
        row = 1;
        break;
    case 4:
        row = selected + 2;
        break;
    case 6:
        row = selected + 8;
        break;
    }
    func_02027534(&data_021213b8, "E00:/Fw_parts.ncl", 0x1000, row << 5, 0, 0x20);
}

// Shows a page of the parts' buttons: "count" of them, from the page's first
void MiiEditFacePage::func_ov076_0214ef88(int page, u32 count, int category) {
    for (u16 i = 0; i < count; i++) {
        mButtons227C[i].mVisible = TRUE;
        int type = GetPartType(i + count * page, category);
        func_02031278(&mButtons227C[i], type * 2, type * 2 + 1);
        func_0203119c(&mButtons227C[i]);
    }
}

int MiiEditFacePage::GetPartType(int button, int category) {
    switch (category) {
    case 2:
        button = data_ov076_02159870[button];
        break;
    case 3:
        button = data_ov076_0215913c[button];
        break;
    case 4:
        button = data_ov076_021595e0[button];
        break;
    case 5:
        button = data_ov076_02158d04[button];
        break;
    case 6:
        button = data_ov076_0215919c[button];
        break;
    case 7:
        break;
    }
    return button;
}

int MiiEditFacePage::GetPartButton(int type, int category) {
    int max, min;
    switch (category) {
    case 2:
        func_020986c4(&mScene->mCharacter, 0x19, &min, &max);
        for (int i = min; i <= max; i++) {
            if (type == data_ov076_02159870[i]) {
                type = i;
                break;
            }
        }
        break;
    case 3:
        func_020986c4(&mScene->mCharacter, 6, &min, &max);
        for (int i = min; i <= max; i++) {
            if (type == data_ov076_0215913c[i]) {
                type = i;
                break;
            }
        }
        break;
    case 4:
        func_020986c4(&mScene->mCharacter, 0, &min, &max);
        for (int i = min; i <= max; i++) {
            if (type == data_ov076_021595e0[i]) {
                type = i;
                break;
            }
        }
        break;
    case 5:
        func_020986c4(&mScene->mCharacter, 0x1C, &min, &max);
        for (int i = min; i <= max; i++) {
            if (type == data_ov076_02158d04[i]) {
                type = i;
                break;
            }
        }
        break;
    case 6:
        func_020986c4(&mScene->mCharacter, 0xC, &min, &max);
        for (int i = min; i <= max; i++) {
            if (type == data_ov076_0215919c[i]) {
                type = i;
                break;
            }
        }
        break;
    case 7:
        break;
    }
    return type;
}

// One of the parts' buttons was chosen
void MiiEditFacePage::func_ov076_0214f264(int parameter, int selected, int page, u16 count) {
    for (int i = 0; i < count; i++) {
        if (i != (u16)selected) {
            func_02031e28(&mButtons227C[i], FALSE);
        }
    }
    SetParameter(parameter, GetPartType(page * count + selected, mCategory));
}

#define CLAMP(x, low, high) ((x) > (high) ? (high) : ((x) < (low) ? (low) : (x)))

// Compiled without common subexpressions: the range's ends are read from the stack after each call of the getter,
// where the default hoists one of them into a register before it
#pragma opt_common_subs off
// Sets a Mii parameter of the character, and plays the particle effect at a height that depends on the part
void MiiEditFacePage::SetParameter(int parameter, int value) {
    switch (parameter) {
    case 0x00:
    case 0x06:
    case 0x0C:
    case 0x10:
    case 0x12:
    case 0x15:
    case 0x19:
    case 0x1C:
    case 0x1F:
    case 0x23:
    case 0x25:
        mUnknown2DF0 = parameter;
        mUnknown2DEC = func_02098dac(&mScene->mCharacter, parameter);
        func_ov076_0214fa14(1);
        break;
    }
    switch (parameter) {
    case 0: {
        int delta = data_ov076_02158e60[mUnknown2DEC] - data_ov076_02158e60[value];
        int min, max;
        func_020986c4(&mScene->mCharacter, 3, &min, &max);
        func_0208fed8(&mScene->mCharacter, 3, CLAMP(func_02098dac(&mScene->mCharacter, 3) + delta, min, max),
                      FALSE);
        break;
    }
    case 6: {
        int delta = data_ov076_02158e48[mUnknown2DEC] - data_ov076_02158e48[value];
        int min, max;
        func_020986c4(&mScene->mCharacter, 9, &min, &max);
        func_0208fed8(&mScene->mCharacter, 9, CLAMP(func_02098dac(&mScene->mCharacter, 9) + delta, min, max),
                      FALSE);
        break;
    }
    }
    func_0208fed8(&mScene->mCharacter, parameter, value, TRUE);
    func_02091a88(&mScene->mCharacter);
    mScene->mUnknown580C = TRUE;
    switch (parameter) {
    case 0:
        mScene->func_ov076_0214b5dc(0, (12 - func_02098dac(&mScene->mCharacter, 5)) * FX32_ONE / 30 + 0x333);
        break;
    case 1:
        mScene->func_ov076_0214b5dc(0, 0x333);
        break;
    case 6:
        mScene->func_ov076_0214b5dc(0, (10 - func_02098dac(&mScene->mCharacter, 0xB)) * FX32_ONE / 30 + 0x4CC);
        break;
    case 7:
        if (func_02098dac(&mScene->mCharacter, 6) == 0x17) {
            break;
        }
        mScene->func_ov076_0214b5dc(0, 0x4CC);
        break;
    case 12:
        mScene->func_ov076_0214b5dc(0, (13 - func_02098dac(&mScene->mCharacter, 0xF)) * FX32_ONE / 30 - 0x333);
        break;
    case 13:
        mScene->func_ov076_0214b5dc(0, -0x333);
        break;
    case 16:
        mScene->func_ov076_0214b5dc(0, -0x666);
        break;
    case 17:
        if (func_02098dac(&mScene->mCharacter, 0x10) == 0 && func_02098dac(&mScene->mCharacter, 0x12) == 0) {
            break;
        }
        mScene->func_ov076_0214b5dc(0, -0x666);
        break;
    case 18:
        mScene->func_ov076_0214b5dc(0, (10 - func_02098dac(&mScene->mCharacter, 0x14)) * FX32_ONE / 30 - 0x333);
        break;
    case 21:
        mScene->func_ov076_0214b5dc(0, (20 - func_02098dac(&mScene->mCharacter, 0x18)) * FX32_ONE / 30);
        break;
    case 25:
        mScene->func_ov076_0214b5dc(0, 0x800);
        break;
    case 26:
        if (func_02098dac(&mScene->mCharacter, 0x19) == 0x1E) {
            break;
        }
        mScene->func_ov076_0214b5dc(0, 0x800);
        break;
    case 28:
        mScene->func_ov076_0214b5dc(0, (9 - func_02098dac(&mScene->mCharacter, 0x1E)) * FX32_ONE / 30);
        break;
    case 31:
        mScene->func_ov076_0214b5dc(0, (10 - func_02098dac(&mScene->mCharacter, 0x22)) * FX32_ONE / 30 + 0x333);
        break;
    case 32:
        if (func_02098dac(&mScene->mCharacter, 0x1F) == 0) {
            break;
        }
        mScene->func_ov076_0214b5dc(0, 0x333);
        break;
    case 35:
    case 36:
    case 37:
        mScene->func_ov076_0214b5dc(0, 0);
        break;
    }
}

#pragma opt_common_subs reset
// Moves a Mii parameter by a step, around its range or up to its ends; returns whether it stopped at an end
BOOL MiiEditFacePage::func_ov076_0214f930(int parameter, int step, BOOL wrap) {
    BOOL stopped = FALSE;
    int value = func_02098dac(&mScene->mCharacter, parameter);
    int min, max;
    value += step;
    func_020986c4(&mScene->mCharacter, parameter, &min, &max);
    if (wrap) {
        if (value < min) {
            value += max - min + 1;
        } else if (max < value) {
            value -= max - min + 1;
        }
    } else {
        if (value < min) {
            value = min;
            stopped = TRUE;
        } else if (max < value) {
            value = max;
            stopped = TRUE;
        }
    }
    func_0208fed8(&mScene->mCharacter, parameter, (u16)value, TRUE);
    mScene->mUnknown580C = TRUE;
    return stopped;
}

// Shows button 27 or hides it, and which way it faces
void MiiEditFacePage::func_ov076_0214fa14(int mode) {
    switch (mode) {
    case 0:
        mButtons[27].mVisible = FALSE;
        break;
    case 1:
        mButtons[27].mVisible = TRUE;
        mButtons[27].mFlipH = FALSE;
        mButtons[27].mSounds.mSounds[0] = 14;
        break;
    case 2:
        mButtons[27].mVisible = TRUE;
        mButtons[27].mFlipH = TRUE;
        mButtons[27].mSounds.mSounds[0] = 15;
        break;
    }
    mUnknown340C = mode;
}

void MiiEditFaceTab::Init(MiiEditFacePage *page) {
    mPage = page;
    mTab = 0;
    func_02031e28(&mPage->mButtons[8], TRUE);
}

void MiiEditFaceTab::func_ov076_0214faac() {}

// Where the face tab's buttons are: the face types' (data_ov076_02158ed4) and the face textures'
// (data_ov076_021591fc)
void MiiEditFaceTab::SelectTab(int tab, BOOL pressed) {
    ObjResourceManager_Load(&gObjResourceManager, "E00:/Face.nce", "E00:/Face.ncg", 0x1000, 1, 0x10000);
    mPage->func_ov076_0214fa14(0);
    mPage->mButtons[8].mVisible = TRUE;
    mPage->mButtons[9].mVisible = TRUE;
    switch (tab) {
    case 0:
        for (u16 i = 0; i < 8; i++) {
            u16 sequence = i * 2;
            mPage->mButtons227C[i].mVisible = TRUE;
            func_02031278(&mPage->mButtons227C[i], sequence, sequence + 1);
            func_0203119c(&mPage->mButtons227C[i]);
            mPage->mButtons227C[i].SetPositionFromVec(data_ov076_02158ed4[i].x << FX32_SHIFT,
                                                      data_ov076_02158ed4[i].y << FX32_SHIFT);
        }
        for (u16 i = 8; i < 12; i++) {
            mPage->mButtons227C[i].mVisible = FALSE;
        }
        break;
    case 1:
        for (u16 i = 0; i < 12; i++) {
            mPage->mButtons227C[i].mVisible = TRUE;
            u16 sequence = i * 2 + 0x10;
            func_02031278(&mPage->mButtons227C[i], sequence, sequence + 1);
            func_0203119c(&mPage->mButtons227C[i]);
            mPage->mButtons227C[i].SetPositionFromVec(data_ov076_021591fc[i].x << FX32_SHIFT,
                                                      data_ov076_021591fc[i].y << FX32_SHIFT);
        }
        break;
    }
    int color = func_02098dac(&mPage->mScene->mCharacter, 0x24);
    switch (tab) {
    case 0: {
        u32 button = mPage->GetPartButton(func_02098dac(&mPage->mScene->mCharacter, 0x23), 1);
        button %= 8;
        if (!pressed) {
            func_02031e28(&mPage->mButtons227C[button], TRUE);
        } else {
            func_02031ca4(&mPage->mButtons227C[button]);
        }
        break;
    }
    case 1: {
        u32 button = mPage->GetPartButton(func_02098dac(&mPage->mScene->mCharacter, 0x25), 1);
        button %= 12;
        if (!pressed) {
            func_02031e28(&mPage->mButtons227C[button], TRUE);
        } else {
            func_02031ca4(&mPage->mButtons227C[button]);
        }
        break;
    }
    }
    func_02027534(&data_021213b8, "E00:/Fw_color.ncl", 0x1000, 0x60, 0x40, 0x60);
    for (u16 i = 28; i <= 35; i++) {
        if (i - 28 < 6) {
            mPage->mButtons[i].mVisible = TRUE;
        } else {
            mPage->mButtons[i].mVisible = FALSE;
        }
    }
    mPage->func_ov076_0214ee7c(color, 1, tab);
    mTab = tab;
    mUnknown4 = 0;
}

void MiiEditEtcTab::Init(MiiEditFacePage *page) {
    mPage = page;
    mTab = 0;
    func_02031e28(&mPage->mButtons[10], TRUE);
}

// Where the etc tab's buttons are: the glasses types' (data_ov076_02158f9c), the mustache types' and the beard
// types' (data_ov076_02158afc), the mole types' (data_ov076_0215892c). The face page's buttons that it places
// beside the types are data_ov076_0215893c and, for the mole, data_ov076_021589e4, at data_ov076_021589fc and
// data_ov076_02158d64 (inferred: the arrows that change a part's size and place, as the glasses and the mustache
// have two such parameters and the mole three).
void MiiEditEtcTab::SelectTab(int tab, BOOL pressed) {
    mPage->mButtons[10].mVisible = TRUE;
    mPage->mButtons[11].mVisible = TRUE;
    mPage->mButtons[12].mVisible = TRUE;
    mPage->mButtons[13].mVisible = TRUE;
    mPage->func_ov076_0214fa14(0);
    ObjResourceManager_Load(&gObjResourceManager, "E00:/Etc.nce", "E00:/Etc.ncg", 0x1000, 1, 0x10000);
    switch (tab) {
    case 0:
        for (u16 i = 0; i < 9; i++) {
            u16 sequence = i * 2;
            mPage->mButtons227C[i].mVisible = TRUE;
            func_02031278(&mPage->mButtons227C[i], sequence, sequence + 1);
            func_0203119c(&mPage->mButtons227C[i]);
            mPage->mButtons227C[i].SetPositionFromVec(data_ov076_02158f9c[i].x << FX32_SHIFT,
                                                      data_ov076_02158f9c[i].y << FX32_SHIFT);
        }
        for (u16 i = 9; i < 12; i++) {
            mPage->mButtons227C[i].mVisible = FALSE;
        }
        break;
    case 1:
        for (u16 i = 0; i < 4; i++) {
            mPage->mButtons227C[i].mVisible = TRUE;
            u16 sequence = i * 2 + 0x12;
            func_02031278(&mPage->mButtons227C[i], sequence, sequence + 1);
            func_0203119c(&mPage->mButtons227C[i]);
            mPage->mButtons227C[i].SetPositionFromVec(data_ov076_02158afc[i].x << FX32_SHIFT,
                                                      data_ov076_02158afc[i].y << FX32_SHIFT);
        }
        for (u16 i = 4; i < 12; i++) {
            mPage->mButtons227C[i].mVisible = FALSE;
        }
        break;
    case 2:
        for (u16 i = 0; i < 2; i++) {
            mPage->mButtons227C[i].mVisible = TRUE;
            u16 sequence = i * 2 + 0x1A;
            func_02031278(&mPage->mButtons227C[i], sequence, sequence + 1);
            func_0203119c(&mPage->mButtons227C[i]);
            mPage->mButtons227C[i].SetPositionFromVec(data_ov076_0215892c[i].x << FX32_SHIFT,
                                                      data_ov076_0215892c[i].y << FX32_SHIFT);
        }
        for (u16 i = 2; i < 12; i++) {
            mPage->mButtons227C[i].mVisible = FALSE;
        }
        break;
    case 3:
        for (u16 i = 0; i < 4; i++) {
            mPage->mButtons227C[i].mVisible = TRUE;
            u16 sequence = i * 2 + 0x1E;
            func_02031278(&mPage->mButtons227C[i], sequence, sequence + 1);
            func_0203119c(&mPage->mButtons227C[i]);
            mPage->mButtons227C[i].SetPositionFromVec(data_ov076_02158afc[i].x << FX32_SHIFT,
                                                      data_ov076_02158afc[i].y << FX32_SHIFT);
        }
        for (u16 i = 4; i < 12; i++) {
            mPage->mButtons227C[i].mVisible = FALSE;
        }
        break;
    }
    for (u16 i = 0; i < 6; i++) {
        mPage->mButtons[data_ov076_021589e4[i]].mVisible = FALSE;
    }
    switch (tab) {
    case 0: {
        u32 button = mPage->GetPartButton(func_02098dac(&mPage->mScene->mCharacter, 0x1F), 7);
        button %= 9;
        int color = func_02098dac(&mPage->mScene->mCharacter, 0x20);
        if (!pressed) {
            func_02031e28(&mPage->mButtons227C[button], TRUE);
        } else {
            func_02031ca4(&mPage->mButtons227C[button]);
        }
        func_02027534(&data_021213b8, "E00:/Fw_color.ncl", 0x1000, 0x140, 0x40, 0x60);
        for (u16 i = 28; i <= 35; i++) {
            if (i - 28 < 6) {
                mPage->mButtons[i].mVisible = TRUE;
            } else {
                mPage->mButtons[i].mVisible = FALSE;
            }
        }
        mPage->func_ov076_0214ee7c(color, 7, -1);
        for (u16 i = 0; i < 4; i++) {
            mPage->mButtons[data_ov076_0215893c[i]].mVisible = TRUE;
            mPage->mButtons[data_ov076_0215893c[i]].SetPositionFromVec(data_ov076_021589fc[i].x << FX32_SHIFT,
                                                                  data_ov076_021589fc[i].y << FX32_SHIFT);
        }
        break;
    }
    case 1: {
        u32 button = mPage->GetPartButton(func_02098dac(&mPage->mScene->mCharacter, 0x12), 7);
        button %= 4;
        int color = func_02098dac(&mPage->mScene->mCharacter, 0x11);
        if (!pressed) {
            func_02031e28(&mPage->mButtons227C[button], TRUE);
        } else {
            func_02031ca4(&mPage->mButtons227C[button]);
        }
        func_02027534(&data_021213b8, "E00:/Fw_color.ncl", 0x1000, 0xA0, 0x40, 0x60);
        for (u16 i = 28; i <= 35; i++) {
            if (i - 28 < 8) {
                mPage->mButtons[i].mVisible = TRUE;
            } else {
                mPage->mButtons[i].mVisible = FALSE;
            }
        }
        mPage->func_ov076_0214ee7c(color, 7, -1);
        for (u16 i = 0; i < 4; i++) {
            mPage->mButtons[data_ov076_0215893c[i]].mVisible = TRUE;
            mPage->mButtons[data_ov076_0215893c[i]].SetPositionFromVec(data_ov076_021589fc[i].x << FX32_SHIFT,
                                                                  data_ov076_021589fc[i].y << FX32_SHIFT);
        }
        break;
    }
    case 2: {
        u32 button = mPage->GetPartButton(func_02098dac(&mPage->mScene->mCharacter, 0x15), 7);
        button %= 2;
        if (!pressed) {
            func_02031e28(&mPage->mButtons227C[button], TRUE);
        } else {
            func_02031ca4(&mPage->mButtons227C[button]);
        }
        func_02027534(&data_021213b8, "E00:/Fw_color.ncl", 0x1000, 0xA0, 0x40, 0x60);
        for (u16 i = 28; i <= 35; i++) {
            if (i - 28 < 0) {
                mPage->mButtons[i].mVisible = TRUE;
            } else {
                mPage->mButtons[i].mVisible = FALSE;
            }
        }
        mPage->func_ov076_0214ee7c(0, 7, -1);
        for (u16 i = 0; i < 6; i++) {
            mPage->mButtons[data_ov076_021589e4[i]].mVisible = TRUE;
            mPage->mButtons[data_ov076_021589e4[i]].SetPositionFromVec(
                data_ov076_02158d64[i].x << FX32_SHIFT, data_ov076_02158d64[i].y << FX32_SHIFT);
        }
        break;
    }
    case 3: {
        u32 button = mPage->GetPartButton(func_02098dac(&mPage->mScene->mCharacter, 0x10), 7);
        button %= 4;
        int color = func_02098dac(&mPage->mScene->mCharacter, 0x11);
        if (!pressed) {
            func_02031e28(&mPage->mButtons227C[button], TRUE);
        } else {
            func_02031ca4(&mPage->mButtons227C[button]);
        }
        func_02027534(&data_021213b8, "E00:/Fw_color.ncl", 0x1000, 0xA0, 0x40, 0x60);
        for (u16 i = 28; i <= 35; i++) {
            if (i - 28 < 8) {
                mPage->mButtons[i].mVisible = TRUE;
            } else {
                mPage->mButtons[i].mVisible = FALSE;
            }
        }
        mPage->func_ov076_0214ee7c(color, 7, -1);
        break;
    }
    }
    mTab = tab;
    mUnknown4 = 0;
}

// --- The profile page ---

// The touch screen as the main program keeps it (gTouchInput)
struct TouchInput {
    /* 0x00 */ u8 mUnknown00[0x28];
    /* 0x28 */ TPData mTp;
    /* 0x30 */ ScreenPoint mLastPoint;
    /* 0x38 */ ScreenPoint mPoint;
    /* 0x40 */ u8 mUnknown40[8];
    /* 0x48 */ BOOL mTouched; // The stylus went down this frame
    /* 0x4C */ BOOL mUnknown4C;
};

extern "C" {
extern TouchInput gTouchInput;
BOOL func_02024004(ScreenControl *screens, int planes, int level);
BOOL func_02024118(ScreenControl *screens, int planes, int level);
// The keyboard: its update with the touch screen, ...
void func_02083244(SoftKeyboard *keyboard, int a, TPData tp);
void func_020831a4(SoftKeyboard *keyboard);
BOOL func_02083614(void *key, ScreenPoint point);
BOOL func_02083308(void *key);
u16 func_02082560(void); // The character typed (8 erases, 7 and 9 add the voicing marks, 11 makes it small)
BOOL func_02031294(SpriteButton *button);
void func_02098cf4(CharacterBase *character, u16 year);
void func_02098d44(CharacterBase *character, int a);
wchar_t *wcscpy(wchar_t *dst, const wchar_t *src);
// Of the profile page, after this
void func_ov076_02153cf4(MiiEditProfilePage *page, const wchar_t *text);
void func_ov076_02153e54(MiiEditProfilePage *page, int year);
void func_ov076_02153fb8(MiiEditProfilePage *page, int day);
void func_ov076_021540a4(MiiEditProfilePage *page, int month);
void func_ov076_02154574(MiiEditProfilePage *page);
void func_ov076_021545dc(MiiEditProfilePage *page, int state);
}

// The sequences of the windows' choices (each choice shows the next sequence while it is chosen)

void MiiEditProfilePage::func_ov076_02150874(MiiEditScene *scene) {
    mState = -1;
    mScene = scene;
    scene->MoveCameraTo(data_ov076_0215885c);
    func_ov076_02151f0c(1);
}

// The original reads the windows' choice and result (MessageWindow::mUnknownAF7C, mResult) anew at each use here,
// as it would a volatile. Declaring them volatile in MessageWindow.h does the same, but costs overlay 106's
// PreStartScene::Ask its match, so the volatile is in this file's reads only (a STAND-IN for whatever the
// original's declaration was).
#define READ(x) (*(volatile int *)&(x))

void MiiEditProfilePage::func_ov076_021508ac() {
    switch (mState) {
    case 0:
        if (mUnknown00004++ <= 2) {
            return;
        }
        func_ov076_02151f0c(mState + 1);
        return;
    case 2:
    case 4:
    case 6:
    case 8:
        if (!func_02024118(&data_0211d604, 0x3F00, 2)) {
            return;
        }
        func_ov076_02151f0c(mState + 1);
        return;
    case 1:
        if (!func_02024004(&data_0211d604, 0x3F00, -2)) {
            return;
        }
        for (int i = 0; i < 10; i++) {
            if (func_0203133c(&mButtons[i])) {
                switch (i) {
                case 0:
                    func_ov076_02151f0c(4);
                    break;
                case 1:
                    func_ov076_02151f0c(2);
                    break;
                case 2:
                    func_ov076_02151f0c(13);
                    break;
                case 3:
                    func_ov076_02151f0c(11);
                    break;
                case 4:
                    func_ov076_02151f0c(6);
                    break;
                case 5:
                    func_ov076_02151f0c(14);
                    break;
                case 6:
                    func_ov076_02151f0c(12);
                    break;
                case 7:
                    func_ov076_02151f0c(8);
                    break;
                case 8:
                    func_ov076_02151f0c(10);
                    break;
                case 9:
                    func_ov076_02151f0c(16);
                    break;
                }
            }
        }
        return;
    case 3:
    case 5:
    case 7:
    case 9: {
        func_02083244(&mKeyboard, 0, gTouchInput.mTp);
        if (!func_02024004(&data_0211d604, 0x3F00, -2)) {
            return;
        }
        if (func_0203133c(&mButton)) {
            func_ov076_02151f0c(0);
        } else if (gTouchInput.mTp.touch != 0 || gTouchInput.mUnknown4C != 0 || data_0211f9d4.mUnknown20 == 0) {
            func_020831a4(&mKeyboard);
        }
        if (!func_02031294(&mButton) && gTouchInput.mUnknown4C) {
            func_ov076_021545dc(this, mState);
        }
        if (gTouchInput.mTouched) {
            ScreenPoint point = gTouchInput.mPoint;

            // The second is reached from the first by a cast, which the original's address computation shows
            if (!func_02083614(&mKeyboard.mUnknown5178, point) &&
                !func_02083614((u8 *)&mKeyboard.mUnknown5178 + 0x6B8, point)) {
                int x;
                u16 c = func_02082560();
                if (c >= 0x20) {
                    if (mKeyboard.mLength >= 10) {
                        return;
                    }
                    mKeyboard.mText[mKeyboard.mLength] = c;
                    x = mKeyboard.mLength * 13 + 5;
                    const NNSG2dFont *font = mScene->mCharBox.mFont;
                    const NNSG2dCharWidths *widths = NNS_G2dFontGetCharWidths(font, c);
                    x -= widths->charWidth >> 1;
                    mScene->mCharBox.mCharX = x;
                    mScene->mCharBox.SetChar(c);
                    mScene->mCharBox.Redraw();
                    mKeyboard.mLength++;
                    if (mKeyboard.mLength >= 10) {
                        return;
                    }
                    mKeyboard.mUnknown5F8C += 0xD000;
                } else if (c == 8) {
                    if (mKeyboard.mLength <= 0) {
                        return;
                    }
                    if (mKeyboard.mLength < 10) {
                        mKeyboard.mUnknown5F8C -= 0xD000;
                    }
                    mKeyboard.mLength--;
                    mKeyboard.mText[mKeyboard.mLength] = 0;
                    wchar_t text[10];
                    wcscpy(text, mKeyboard.mText);
                    func_ov076_02153cf4(this, text);
                } else if (c == 7) {
                    int last = mKeyboard.mLength - 1;
                    if (last < 0) {
                        return;
                    }
                    u16 ch = mKeyboard.mText[last];
                    BOOL changed = FALSE;
                    // ka ki ku ke ko, KA to KO, sa to so, SA to SO, ta chi tsu te to, TA to TO, ha to ho, HA to HO
                    if (ch == 0x304B || ch == 0x304D || ch == 0x304F || ch == 0x3051 || ch == 0x3053 || ch == 0x30AB ||
                        ch == 0x30AD || ch == 0x30AF || ch == 0x30B1 || ch == 0x30B3 || ch == 0x3055 || ch == 0x3057 ||
                        ch == 0x3059 || ch == 0x305B || ch == 0x305D || ch == 0x30B5 || ch == 0x30B7 || ch == 0x30B9 ||
                        ch == 0x30BB || ch == 0x30BD || ch == 0x305F || ch == 0x3061 || ch == 0x3064 || ch == 0x3066 ||
                        ch == 0x3068 || ch == 0x30BF || ch == 0x30C1 || ch == 0x30C4 || ch == 0x30C6 || ch == 0x30C8 ||
                        ch == 0x306F || ch == 0x3072 || ch == 0x3075 || ch == 0x3078 || ch == 0x307B || ch == 0x30CF ||
                        ch == 0x30D2 || ch == 0x30D5 || ch == 0x30D8 || ch == 0x30DB) {
                        mKeyboard.mText[last] = ch + 1;
                        changed = TRUE;
                    } else if (ch == 0x304C || ch == 0x304E || ch == 0x3050 || ch == 0x3052 || ch == 0x3054 ||
                               ch == 0x30AC || ch == 0x30AE || ch == 0x30B0 || ch == 0x30B2 || ch == 0x30B4 ||
                               ch == 0x3056 || ch == 0x3058 || ch == 0x305A || ch == 0x305C || ch == 0x305E ||
                               ch == 0x30B6 || ch == 0x30B8 || ch == 0x30BA || ch == 0x30BC || ch == 0x30BE ||
                               ch == 0x3060 || ch == 0x3062 || ch == 0x3065 || ch == 0x3067 || ch == 0x3069 ||
                               ch == 0x30C0 || ch == 0x30C2 || ch == 0x30C5 || ch == 0x30C7 || ch == 0x30C9 ||
                               ch == 0x3070 || ch == 0x3073 || ch == 0x3076 || ch == 0x3079 || ch == 0x307C ||
                               ch == 0x30D0 || ch == 0x30D3 || ch == 0x30D6 || ch == 0x30D9 || ch == 0x30DC) {
                        mKeyboard.mText[last] = ch - 1;
                        changed = TRUE;
                    } else if (ch == 0x30A6) { // U, VU
                        mKeyboard.mText[last] = 0x30F4;
                        changed = TRUE;
                    } else if (ch == 0x30F4) {
                        mKeyboard.mText[last] = 0x30A6;
                        changed = TRUE;
                    }
                    if (!changed) {
                        return;
                    }
                    wchar_t text[10];
                    wcscpy(text, mKeyboard.mText);
                    func_ov076_02153cf4(this, text);
                } else if (c == 9) {
                    int last = mKeyboard.mLength - 1;
                    if (last < 0) {
                        return;
                    }
                    u16 ch = mKeyboard.mText[last];
                    BOOL changed = FALSE;
                    // ha hi fu he ho, HA to HO
                    if (ch == 0x306F || ch == 0x3072 || ch == 0x3075 || ch == 0x3078 || ch == 0x307B || ch == 0x30CF ||
                        ch == 0x30D2 || ch == 0x30D5 || ch == 0x30D8 || ch == 0x30DB) {
                        mKeyboard.mText[last] = ch + 2;
                        changed = TRUE;
                    } else if (ch == 0x3071 || ch == 0x3074 || ch == 0x3077 || ch == 0x307A || ch == 0x307D ||
                               ch == 0x30D1 || ch == 0x30D4 || ch == 0x30D7 || ch == 0x30DA || ch == 0x30DD) {
                        mKeyboard.mText[last] = ch - 2;
                        changed = TRUE;
                    }
                    if (!changed) {
                        return;
                    }
                    wchar_t text[10];
                    wcscpy(text, mKeyboard.mText);
                    func_ov076_02153cf4(this, text);
                } else if (c == 11) {
                    int last = mKeyboard.mLength - 1;
                    if (last < 0) {
                        return;
                    }
                    u16 ch = mKeyboard.mText[last];
                    BOOL changed = FALSE;
                    // a i u e o, A to O, tsu, TSU, ya, YA, yu, YU, yo, YO, wa, WA
                    if (ch == 0x3042 || ch == 0x3044 || ch == 0x3046 || ch == 0x3048 || ch == 0x304A || ch == 0x30A2 ||
                        ch == 0x30A4 || ch == 0x30A6 || ch == 0x30A8 || ch == 0x30AA || ch == 0x3064 || ch == 0x30C4 ||
                        ch == 0x3084 || ch == 0x30E4 || ch == 0x3086 || ch == 0x30E6 || ch == 0x3088 || ch == 0x30E8 ||
                        ch == 0x308F || ch == 0x30EF) {
                        mKeyboard.mText[last] = ch - 1;
                        changed = TRUE;
                    } else if (ch == 0x3041 || ch == 0x3043 || ch == 0x3045 || ch == 0x3047 || ch == 0x3049 ||
                               ch == 0x30A1 || ch == 0x30A3 || ch == 0x30A5 || ch == 0x30A7 || ch == 0x30A9 ||
                               ch == 0x3063 || ch == 0x30C3 || ch == 0x3083 || ch == 0x30E3 || ch == 0x3085 ||
                               ch == 0x30E5 || ch == 0x3087 || ch == 0x30E7 || ch == 0x308E || ch == 0x30EE) {
                        mKeyboard.mText[last] = ch + 1;
                        changed = TRUE;
                    } else if (ch == 0x30F5) { // Small KA, KA
                        mKeyboard.mText[last] = 0x30AB;
                        changed = TRUE;
                    } else if (ch == 0x30AB) {
                        mKeyboard.mText[last] = 0x30F5;
                        changed = TRUE;
                    } else if (ch == 0x30F6) { // Small KE, KE
                        mKeyboard.mText[last] = 0x30B1;
                        changed = TRUE;
                    } else if (ch == 0x30B1) {
                        mKeyboard.mText[last] = 0x30F6;
                        changed = TRUE;
                    }
                    if (!changed) {
                        return;
                    }
                    wchar_t text[10];
                    wcscpy(text, mKeyboard.mText);
                    func_ov076_02153cf4(this, text);
                }
                return;
            }
        }
        if (!func_02083308(mKeyboard.mUnknown5178[0])) {
            return;
        }
        switch (mState) {
        case 7:
            MiiData_SetName(&mScene->mCharacter, mKeyboard.mText);
            break;
        case 3:
            wcscpy((wchar_t *)&mScene->mCharacter.mUnknown058[0], mKeyboard.mText);
            break;
        case 5:
            wcscpy((wchar_t *)&mScene->mCharacter.mUnknown058[0x16], mKeyboard.mText);
            break;
        case 9:
            MiiData_SetCreatorName(&mScene->mCharacter, mKeyboard.mText);
            break;
        }
        func_ov076_02151f0c(0);
        return;
    }
    case 11: {
        func_020341d4(&mWindows[2]);
        int result = READ(mWindows[2].mResult);
        if (result != -1) {
            switch (result) {
            case 0: {
                int year = mScene->mCharacter.mYear + 1;
                if (year > 2099) {
                    year = 1900;
                }
                func_ov076_02153e54(this, year);
                func_02098cf4(&mScene->mCharacter, year);
                func_ov076_02154574(this);
                break;
            }
            case 1: {
                int year = mScene->mCharacter.mYear - 1;
                if (year < 1900) {
                    year = 2099;
                }
                func_ov076_02153e54(this, year);
                func_02098cf4(&mScene->mCharacter, year);
                func_ov076_02154574(this);
                break;
            }
            }
        }
        if (READ(mWindows[2].mResult) != -1) {
            switch (READ(mWindows[2].mResult)) {
            case 2:
                func_02098cf4(&mScene->mCharacter, 0xFFF);
                mWindows[2].mUnknownSetBeforeClose = 15;
                func_020349cc(&mWindows[2], 0x24);
                break;
            case 3:
                mWindows[2].mUnknownSetBeforeClose = 15;
                func_020349cc(&mWindows[2], 0x24);
                break;
            }
        }
        if (mWindows[2].mTransitionDone) {
            func_ov076_02151f0c(1);
        }
        return;
    }
    case 12: {
        func_020341d4(&mWindows[3]);
        int result = READ(mWindows[3].mResult);
        if (result != -1) {
            switch (result) {
            case 0: {
                int month = func_02098dac(&mScene->mCharacter, 0x26) + 1;
                if (month > 12) {
                    month = 1;
                }
                func_ov076_021540a4(this, month);
                func_02098c40(&mScene->mCharacter, 0x26, month);
                int min, max;
                func_020986c4(&mScene->mCharacter, 0x27, &min, &max);
                int day = func_02098dac(&mScene->mCharacter, 0x27);
                if (day > max) {
                    day = max;
                }
                func_ov076_02153fb8(this, day);
                func_02098c40(&mScene->mCharacter, 0x27, day);
                break;
            }
            case 1: {
                int month = func_02098dac(&mScene->mCharacter, 0x26) - 1;
                if (month < 1) {
                    month = 12;
                }
                func_ov076_021540a4(this, month);
                func_02098c40(&mScene->mCharacter, 0x26, month);
                int min, max;
                func_020986c4(&mScene->mCharacter, 0x27, &min, &max);
                int day = func_02098dac(&mScene->mCharacter, 0x27);
                if (day > max) {
                    day = max;
                }
                func_ov076_02153fb8(this, day);
                func_02098c40(&mScene->mCharacter, 0x27, day);
                break;
            }
            case 2: {
                int min, max;
                func_020986c4(&mScene->mCharacter, 0x27, &min, &max);
                int day = func_02098dac(&mScene->mCharacter, 0x27) + 1;
                if (day > max) {
                    day = 1;
                }
                func_ov076_02153fb8(this, day);
                func_02098c40(&mScene->mCharacter, 0x27, day);
                break;
            }
            case 3: {
                int min, max;
                func_020986c4(&mScene->mCharacter, 0x27, &min, &max);
                int day = func_02098dac(&mScene->mCharacter, 0x27) - 1;
                if (day < 1) {
                    day = max;
                }
                func_ov076_02153fb8(this, day);
                func_02098c40(&mScene->mCharacter, 0x27, day);
                break;
            }
            }
        }
        if (READ(mWindows[3].mResult) != -1) {
            switch (READ(mWindows[3].mResult)) {
            case 4:
                func_02098c40(&mScene->mCharacter, 0x26, 0);
                func_02098c40(&mScene->mCharacter, 0x27, 0);
                mWindows[3].mUnknownSetBeforeClose = 15;
                func_020349cc(&mWindows[3], 0x24);
                break;
            case 5:
                mWindows[3].mUnknownSetBeforeClose = 15;
                func_020349cc(&mWindows[3], 0x24);
                break;
            }
        }
        if (mWindows[3].mTransitionDone) {
            func_ov076_02151f0c(1);
        }
        return;
    }
    case 13: {
        func_020341d4(&mWindows[0]);
        int i;
        int choice;
        if (READ(mWindows[0].mUnknownAF7C) != -1 && (choice = READ(mWindows[0].mUnknownAF7C)) != 4) {
            switch (choice) {
            case 0:
                func_02098d44(&mScene->mCharacter, 0);
                break;
            case 1:
                func_02098d44(&mScene->mCharacter, 1);
                break;
            case 2:
                func_02098d44(&mScene->mCharacter, 2);
                break;
            case 3:
                func_02098d44(&mScene->mCharacter, 3);
                break;
            case 4:
                func_02098d44(&mScene->mCharacter, 4);
                break;
            }
            for (i = 0; i < 4; i++) {
                if (i == choice) {
                    Sprite_SetSequence(&mWindows[0].mSprites[i].mSprite, (u16)(data_ov076_02158df4[i] + 1), TRUE);
                } else {
                    Sprite_SetSequence(&mWindows[0].mSprites[i].mSprite, data_ov076_02158df4[i], TRUE);
                }
            }
            mWindows[0].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindows[0], 0x24);
        }
        if (READ(mWindows[0].mResult) != -1 && READ(mWindows[0].mResult) == 4) {
            func_02098d44(&mScene->mCharacter, 4);
            mWindows[0].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindows[0], 0x24);
        }
        if (mWindows[0].mTransitionDone) {
            func_ov076_02151f0c(1);
        }
        return;
    }
    case 14:
        func_020341d4(&mWindows[1]);
        if (mWindows[1].mUnknownAF88) {
            func_ov076_02151f0c(15);
        }
        return;
    case 15: {
        func_020341d4(&mWindows[1]);
        int i;
        int choice;
        if (READ(mWindows[1].mUnknownAF7C) != -1 && (choice = READ(mWindows[1].mUnknownAF7C)) < 12) {
            if (choice >= 0 && choice < 12) {
                func_02098c40(&mScene->mCharacter, 0x28, choice);
                func_02091a88(&mScene->mCharacter);
                mScene->func_ov076_0214b694();
            }
            for (i = 0; i < 12; i++) {
                if (i == choice) {
                    Sprite_SetSequence(&mWindows[1].mSprites[i].mSprite, (u16)(data_ov076_02158df4[i + 4] + 1), TRUE);
                } else {
                    Sprite_SetSequence(&mWindows[1].mSprites[i].mSprite, data_ov076_02158df4[i + 4], TRUE);
                }
            }
            mWindows[1].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindows[1], 0x24);
            for (int i = 0; i < 9; i++) {
                mScene->mTextBoxes[i].mSprite.mVisible = TRUE;
            }
        }
        if (READ(mWindows[1].mResult) != -1 && READ(mWindows[1].mResult) == 12) {
            mWindows[1].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindows[1], 0x24);
            for (int i = 0; i < 9; i++) {
                mScene->mTextBoxes[i].mSprite.mVisible = TRUE;
            }
        }
        if (mWindows[1].mTransitionDone) {
            func_ov076_02151f0c(1);
        }
        return;
    }
    case 10: {
        func_020341d4(&mWindows[4]);
        if (READ(mWindows[4].mUnknownAF7C) != -1) {
            int i;
            int choice = READ(mWindows[4].mUnknownAF7C);
            switch (choice) {
            case 0:
                mScene->mCharacter.mUnknown298 = 2;
                break;
            case 1:
                mScene->mCharacter.mUnknown298 = 3;
                break;
            case 2:
                mScene->mCharacter.mUnknown298 = 4;
                break;
            case 3:
                mScene->mCharacter.mUnknown298 = 5;
                break;
            case 4:
                mScene->mCharacter.mUnknown298 = 0;
                break;
            }
            for (i = 0; i < 5; i++) {
                if (i == choice) {
                    Sprite_SetSequence(&mWindows[4].mButtons[i].mSprite, (u16)(data_ov076_02158df4[i + 24] + 1), TRUE);
                } else {
                    Sprite_SetSequence(&mWindows[4].mButtons[i].mSprite, data_ov076_02158df4[i + 24], TRUE);
                }
            }
            mWindows[4].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindows[4], 0x24);
        }
        if (mWindows[4].mTransitionDone) {
            func_ov076_02151f0c(1);
        }
        return;
    }
    case 16: {
        func_020341d4(&mWindows[5]);
        int i;
        int choice;
        if (READ(mWindows[5].mUnknownAF7C) != -1 && (choice = READ(mWindows[5].mUnknownAF7C)) < 2) {
            switch (choice) {
            case 0:
                func_02098cd0(&mScene->mCharacter, 0);
                mScene->func_ov076_0214b694();
                break;
            case 1:
                func_02098cd0(&mScene->mCharacter, 1);
                mScene->func_ov076_0214b694();
                break;
            }
            for (i = 0; i < 2; i++) {
                if (i == choice) {
                    Sprite_SetSequence(&mWindows[5].mSprites[i].mSprite, (u16)(data_ov076_02158df4[i + 16] + 1), TRUE);
                } else {
                    Sprite_SetSequence(&mWindows[5].mSprites[i].mSprite, data_ov076_02158df4[i + 16], TRUE);
                }
            }
            mWindows[5].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindows[5], 0x24);
        }
        if (READ(mWindows[5].mResult) != -1 && READ(mWindows[5].mResult) == 2) {
            mWindows[5].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindows[5], 0x24);
        }
        if (mWindows[5].mTransitionDone) {
            func_ov076_02151f0c(1);
        }
        return;
    }
    }
}

extern "C" {
void func_02024c14(u32 *manager, void *a, int planes);
void func_02023f4c(ScreenControl *screens, int planes, int a);
BOOL func_02098584(CharacterRecord *character);
int func_02098e50(CharacterRecord *character, int parameter);
// Adds a button to a window: (window, number, unknown, sequence, sequence when on, position, size, its sounds,
// unknown); func_02033844 a button with a text: the same with the text's message file and message, font, plane and
// character offset
void func_02033448(MessageWindow *window, int button, int a, u8 sequence, u8 onSequence, WindowPoint position,
                   WindowPoint size, const ButtonSounds *sounds, int b);
void func_02033844(MessageWindow *window, int button, int a, u8 sequence, u8 onSequence, WindowPoint position,
                   WindowPoint size, MessageFile *file, u32 message, const NNSG2dFont *font, int plane,
                   int *charOffset, const ButtonSounds *sounds, int b);
void func_02033bb4(MessageWindow *window, int a, int sequence, WindowPoint position); // Adds a sprite
void func_02032140(SpriteButton *button, int a, int b, int c, int d); // Its four sounds
// The keyboard: set up with the archive's name, ...
void func_02082fb4(SoftKeyboard *keyboard, const char *archiveName);
void func_02083078(SoftKeyboard *keyboard, int a, int b);
void func_02083288(SoftKeyboard *keyboard, int a, int b);
void func_0208382c(SoftKeyboard *keyboard, int a, MessageFile *file, int b, int c, int d, int e);
void func_02083c30(SoftKeyboard *keyboard);
void func_02083740(SoftKeyboard *keyboard);
void func_020836d0(SoftKeyboard *keyboard, int a, int b, int c, int d, int e);
void func_02083608(void *key, int a);
size_t wcslen(const wchar_t *s);
void func_ov076_02154190(MiiEditProfilePage *page);
}

// Whether the profile has all it needs: the two texts and the name accepted by func_ov076_0214bf20, and for a new
// character (mUnknown5824 0) the year and the birthday, for one being edited a creator's name accepted or none
// The same as overlay 76's other branch has it: a position passed by value, which leaves the original's copies
static inline void SetCanvasPosition(ObjTextCanvas *canvas, WindowPoint position) {
    canvas->mSprite.SetPosition(FxVec2(position.x << FX32_SHIFT, position.y << FX32_SHIFT));
}

// Sets the page's state up: 0 leaves the keyboard or a window, 1 the profile's buttons and the six windows, 3, 5,
// 7 and 9 the keyboard for one of the four texts, 10 to 16 open a window.
// NOT MATCHING (99.97%): the instructions are the original's, but six stack slots are not. The original keeps the
// WindowPoints of the windows' OK buttons (okSize, okPosition, clearPosition, an unread second copy of the size,
// yearSize, dateSize, from sp+0x64 down) below every other local; here okSize and clearPosition get slots above
// the inline copies, and the unread copy is missing.
void MiiEditProfilePage::func_ov076_02151f0c(int state) {
    wchar_t name[11];
    wchar_t creator[11];
    wchar_t text[11];
    switch (mState) {
    case 10:
    case 11:
    case 12:
    case 13:
    case 15:
    case 16:
        for (int i = 0; i < 6; i++) {
            func_020346cc(&mWindows[i]);
        }
        break;
    }
    switch (state) {
    case 0:
        func_02024c14(gGraphicsManager, mScene->mUnknown00010, 0x3F00);
        func_02027d7c(&data_02121834, "E00:/Window_down.nsc", 0x100, 0, 0, 0, 0, -1);
        func_02027d7c(&data_02121834, "E00:/BG_down_editface.nsc", 0x200, 0, 0, 0, 0, -1);
        func_02027d7c(&data_02121834, "E00:/BG_down.nsc", 0x400, 0, 0, 0, 0, -1);
        func_02028f24(&gVramTransferQueue, "E00:/BG_down.ncg", 0x2F00, 0, 0, 0, -1);
        func_02027534(&data_021213b8, "E00:/BG_down.ncl", 0x2F00, -1, -1, -1);
        data_0211d604.mPlanes = 0x17;
        data_0211d604.mPlanesChanged = TRUE;
        func_02023f4c(&data_0211d604, 0x3F00, 0x10);
        break;
    case 1: {
        int charOffset = 0x384;
        mScene->func_ov076_0214bbe4();
        func_02027534(&data_021213b8, "E00:/Obj_down.ncl", 0x1000, -1, -1, -1);
        ObjResourceManager_Load(&gObjResourceManager, "E00:/Profile.nce", "E00:/Profile.ncg", 0x1000, 1, 0x10000);
        for (u16 i = 0; i < 10; i++) {
            u16 sequence = data_ov076_0215895c[i];
            Sprite_Init(&mButtons[i], 1, sequence, 0x80, 0x60, TRUE);
            func_02031eb0(&mButtons[i], data_ov076_02159034[i].x, data_ov076_02159034[i].y);
            func_02031278(&mButtons[i], sequence, sequence + 1);
            func_02031260(&mButtons[i], data_ov076_02158b60[i]);
            func_02032158(&mButtons[i], &data_ov076_021593e0[i]);
            mButtons[i].mUnknownDC = 14;
            mButtons[i].SetPositionFromVec(data_ov076_02158fe4[i].x << FX32_SHIFT,
                                           data_ov076_02158fe4[i].y << FX32_SHIFT);
            func_0203119c(&mButtons[i]);
        }
        for (u16 i = 0; i < 2; i++) {
            Sprite_Init(&mSprites[i], 1, data_ov076_02158774[i], 0x80, 0x60, TRUE);
            mSprites[i].SetPositionFromVec(data_ov076_021588bc[i].x << FX32_SHIFT,
                                           data_ov076_021588bc[i].y << FX32_SHIFT);
            NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&mSprites[i].mAnim));
        }
        if (mScene->mUnknown5824 == 0) {
            func_02031278(&mButtons[3], 8, 9);
            func_02031278(&mButtons[6], 0x12, 0x13);
            func_02031e88(&mButtons[8], TRUE);
        }
        for (int i = 0; i < 10; i++) {
            if (i >= 0 && i < 10) {
                mButtons[i].mVisible = TRUE;
            } else {
                mButtons[i].mVisible = FALSE;
            }
        }
        if (mScene->mSceneId != 0x43 && mScene->mSceneId != 0x6F) {
            func_02031e88(&mButtons[9], TRUE);
        }
        if (!func_02098584(&mScene->mCharacter)) {
            func_02031e88(&mButtons[4], TRUE);
            func_02031e88(&mButtons[5], TRUE);
            func_02031e88(&mButtons[7], TRUE);
            int month = func_02098e50(&mScene->mCharacter, 0x26);
            int day = func_02098e50(&mScene->mCharacter, 0x27);
            if (month != 0 && day != 0) {
                func_02031e88(&mButtons[6], TRUE);
            }
        }
        ScreenRect rect = data_ov076_021588dc;
        for (int i = 0; i < 6; i++) {
            func_02032fb8(&mWindows[i], rect);
            func_02033174(&mWindows[i], data_ov076_02158e30, 0x100);
            func_020331bc(&mWindows[i], 0x3F00, rect);
            func_02034f4c(&mWindows[i], 1);
        }
        int n = 0;
        for (int i = 0; i <= 3; i++) {
            u16 sequence = data_ov076_02158df4[i];
            func_02033448(&mWindows[0], n, 1, sequence, sequence + 1, data_ov076_021596a0[i], data_ov076_02159788[i],
                          &data_ov076_02159c10[i], 14);
            func_02031260((SpriteButton *)&mWindows[0].mSprites[n], data_ov076_0215925c[i]);
            n++;
        }
        WindowPoint okPosition = data_ov076_021587a8;
        WindowPoint okSize = data_ov076_021587c0;
        func_02033844(&mWindows[0], n, 0, 0x69, 0x6A, okPosition, okSize, &mScene->mMessageFiles[1], 7,
                      &gFonts[2].mFont, 0x1000, &charOffset, &data_ov076_02158d94[2], 14);
        n = 0;
        for (int i = 4; i <= 15; i++) {
            u16 sequence = data_ov076_02158df4[i];
            func_02033448(&mWindows[1], n, 1, sequence, sequence + 1, data_ov076_021596a0[i], data_ov076_02159788[i],
                          &data_ov076_02159c10[i], 14);
            func_02031260((SpriteButton *)&mWindows[1].mSprites[n], data_ov076_0215925c[i]);
            n++;
        }
        WindowPoint okPosition1 = data_ov076_021587a8;
        func_02033844(&mWindows[1], n, 0, 0x69, 0x6A, okPosition1, okSize, &mScene->mMessageFiles[1], 5,
                      &gFonts[2].mFont, 0x1000, &charOffset, &data_ov076_02158d94[2], 14);
        n = 0;
        for (int i = 18; i <= 19; i++) {
            u16 sequence = data_ov076_02158df4[i];
            func_02033448(&mWindows[2], n, 1, sequence, sequence + 1, data_ov076_021596a0[i], data_ov076_02159788[i],
                          &data_ov076_02159c10[i], 14);
            func_02031260((SpriteButton *)&mWindows[2].mSprites[n], 1);
            mWindows[2].mSprites[n].mUnknownE0 = 6;
            func_0203119c((SpriteButton *)&mWindows[2].mSprites[n]);
            mWindows[2].mSprites[n].mSprite.mVisible = FALSE;
            n++;
        }
        WindowPoint okPosition2 = data_ov076_021587a8;
        WindowPoint yearSize = data_ov076_021587c0;
        func_02033844(&mWindows[2], n, 0, 0x69, 0x6A, okPosition2, yearSize, &mScene->mMessageFiles[1], 7,
                      &gFonts[2].mFont, 0x1000, &charOffset, &data_ov076_02158d94[2], 14);
        WindowPoint clearPosition = data_ov076_02158790;
        func_02033844(&mWindows[2], n + 1, 0, 0x69, 0x6A, clearPosition, yearSize, &mScene->mMessageFiles[1], 6,
                      &gFonts[2].mFont, 0x1000, &charOffset, &data_ov076_02158d94[0], 14);
        for (int i = 0; i < 4; i++) {
            func_02033bb4(&mWindows[2], 1, 0x4A, data_ov076_02158abc[i]);
            mWindows[2].mSpritesA9F4[i].mOverwritePalette = 0;
            NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&mWindows[2].mSpritesA9F4[i].mAnim));
            mWindows[2].mSpritesA9F4[i].mVisible = FALSE;
        }
        func_02033bb4(&mWindows[2], 1, 0x4B, data_ov076_021587b8);
        n = 0;
        for (int i = 20; i <= 23; i++) {
            u16 sequence = data_ov076_02158df4[i];
            func_02033448(&mWindows[3], n, 1, sequence, sequence + 1, data_ov076_021596a0[i], data_ov076_02159788[i],
                          &data_ov076_02159c10[i], 14);
            func_02031260((SpriteButton *)&mWindows[3].mSprites[n], 1);
            mWindows[3].mSprites[n].mUnknownE0 = 6;
            func_0203119c((SpriteButton *)&mWindows[3].mSprites[n]);
            mWindows[3].mSprites[n].mSprite.mVisible = FALSE;
            n++;
        }
        WindowPoint okPosition3 = data_ov076_021587a8;
        WindowPoint dateSize = data_ov076_021587c0;
        func_02033844(&mWindows[3], n, 0, 0x69, 0x6A, okPosition3, dateSize, &mScene->mMessageFiles[1], 7,
                      &gFonts[2].mFont, 0x1000, &charOffset, &data_ov076_02158d94[2], 14);
        func_02033844(&mWindows[3], n + 1, 0, 0x69, 0x6A, clearPosition, dateSize, &mScene->mMessageFiles[1], 6,
                      &gFonts[2].mFont, 0x1000, &charOffset, &data_ov076_02158d94[0], 14);
        for (int i = 0; i < 4; i++) {
            func_02033bb4(&mWindows[3], 1, 0x4A, data_ov076_02158adc[i]);
            mWindows[3].mSpritesA9F4[i].mOverwritePalette = 0;
            NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&mWindows[3].mSpritesA9F4[i].mAnim));
            mWindows[3].mSpritesA9F4[i].mVisible = FALSE;
        }
        func_02033bb4(&mWindows[3], 1, 0x4C, data_ov076_021587d0);
        n = 0;
        for (int i = 24; i <= 28; i++) {
            int value;
            switch (i - 24) {
            case 0:
                value = 2;
                break;
            case 1:
                value = 3;
                break;
            case 2:
                value = 4;
                break;
            case 3:
                value = 5;
                break;
            case 4:
                value = 0;
                break;
            }
            u16 sequence = data_ov076_02158df4[i];
            func_02033844(&mWindows[4], n, 1, sequence, sequence + 1, data_ov076_021596a0[i], data_ov076_02159788[i],
                          &mScene->mMessageFiles[0], value + 0x54, &gFonts[0].mFont, 0x1000, &charOffset,
                          &data_ov076_02159c10[i], 14);
            func_02031260((SpriteButton *)&mWindows[4].mButtons[i - 24], data_ov076_0215925c[i - 24]);
            mWindows[4].mButtons[i - 24].mMoveText = FALSE;
            n++;
        }
        n = 0;
        for (int i = 16; i <= 17; i++) {
            u16 sequence = data_ov076_02158df4[i];
            func_02033448(&mWindows[5], n, 1, sequence, sequence + 1, data_ov076_021596a0[i], data_ov076_02159788[i],
                          &data_ov076_02159c10[i], 14);
            func_02031260((SpriteButton *)&mWindows[5].mSprites[n], data_ov076_0215925c[i]);
            n++;
        }
        WindowPoint okPosition5 = data_ov076_021587a8;
        func_02033844(&mWindows[5], n, 0, 0x69, 0x6A, okPosition5, okSize, &mScene->mMessageFiles[1], 5,
                      &gFonts[2].mFont, 0x1000, &charOffset, &data_ov076_02158d94[2], 14);
        mScene->mText1B35C.mSprite.mVisible = FALSE;
        func_ov076_02154190(this);
        MiiData_GetName(&mScene->mCharacter, name);
        MiiData_GetCreatorName(&mScene->mCharacter, creator);
        BOOL complete;
        if (mScene->mUnknown5824 == 0) {
            if (mScene->func_ov076_0214bf20((wchar_t *)&mScene->mCharacter.mUnknown058[0x16]) &&
                mScene->func_ov076_0214bf20((wchar_t *)&mScene->mCharacter.mUnknown058[0]) &&
                mScene->func_ov076_0214bf20(name) && mScene->mCharacter.mYear != 0xFFF &&
                func_02098dac(&mScene->mCharacter, 0x26) != 0 && func_02098dac(&mScene->mCharacter, 0x27) != 0) {
                complete = TRUE;
            } else {
                complete = FALSE;
            }
        } else {
            if (mScene->func_ov076_0214bf20((wchar_t *)&mScene->mCharacter.mUnknown058[0x16]) &&
                mScene->func_ov076_0214bf20((wchar_t *)&mScene->mCharacter.mUnknown058[0]) &&
                mScene->func_ov076_0214bf20(name) &&
                (wcslen(creator) == 0 || mScene->func_ov076_0214bf20(creator))) {
                complete = TRUE;
            } else {
                complete = FALSE;
            }
        }
        if (complete) {
            int place = mScene->mSprite582C.mPlace;
            if (place == 4) {
                func_02031e88(&mScene->mButtons[4], FALSE);
                func_02031e88(&mScene->mButtons[2], FALSE);
                func_02031e88(&mScene->mButtons[3], FALSE);
                if (!mScene->mUnknown58E8[6]) {
                    mScene->mUnknown58DC = TRUE;
                }
            } else if (place == 3) {
                func_02031e88(&mScene->mButtons[3], FALSE);
                func_02031e88(&mScene->mButtons[2], FALSE);
                if (!mScene->mUnknown58E8[4]) {
                    mScene->mUnknown58DC = TRUE;
                }
            } else if (place <= 2) {
                mScene->mSprite582C.func_ov076_0214bec8(2);
                func_02031e88(&mScene->mButtons[2], FALSE);
                if (!mScene->mUnknown58E8[3]) {
                    mScene->mUnknown58DC = TRUE;
                }
            }
        } else {
            func_02031e88(&mScene->mButtons[2], TRUE);
            func_02031e88(&mScene->mButtons[3], TRUE);
            func_02031e88(&mScene->mButtons[4], TRUE);
            mScene->mUnknown58DC = FALSE;
        }
        break;
    }
    case 3:
    case 5:
    case 7:
    case 9: {
        mScene->func_ov076_0214bd64();
        MI_CpuClear16(mKeyboard.mText, sizeof(mKeyboard.mText));
        mKeyboard.mLength = 0;
        for (int i = 0; i < 10; i++) {
            mButtons[i].mVisible = FALSE;
        }
        for (u16 i = 0; i < 2; i++) {
            mSprites[i].mVisible = FALSE;
        }
        func_02024c14(gGraphicsManager, mScene->mUnknown00010, 0x3F00);
        func_02028f24(&gVramTransferQueue, "KBD:/bg_keyboard.ncg", 0x200, 0, 0, 0, -1);
        func_02027534(&data_021213b8, "KBD:/keyboardColor.ncl", 0x2F00, -1, -1, -1);
        func_02027d7c(&data_02121834, "KBD:/bg_keyboard_10.nsc", 0x200, 0, 0, 0, 0, -1);
        G2S_SetBG0Control(GX_BG_SCRSIZE_TEXT_256x256, GX_BG_COLORMODE_16, GX_BG_SCRBASE_0x3000, GX_BG_CHARBASE_0x08000,
                          GX_BG_EXTPLTT_01);
        func_02082fb4(&mKeyboard, "KBD");
        func_02083078(&mKeyboard, 0x100, 0);
        func_02083288(&mKeyboard, 0, 0);
        func_0208382c(&mKeyboard, 3, &mScene->mMessageFiles[1], 0x384, 0x14000, 0, 13);
        func_02083c30(&mKeyboard);
        func_02083740(&mKeyboard);
        func_020836d0(&mKeyboard, 0, 1, 14, 0, 20);
        func_02083608((u8 *)&mKeyboard.mUnknown5178 + 0x6B8, 0);
        switch (state) {
        case 3: {
            mScene->mText1B35C.mSprite.mVisible = TRUE;
            NNS_G2dCharCanvasClear(&mScene->mText1B35C.mCanvas, 0);
            mScene->mText1B35C.mMessage = 0x1F;
            mScene->mText1B35C.DrawAll();
            SetCanvasPosition(&mScene->mCharBox, data_ov076_021587f8);
            func_ov076_02153cf4(this, (wchar_t *)&mScene->mCharacter.mUnknown058[0]);
            break;
        }
        case 5: {
            mScene->mText1B35C.mSprite.mVisible = TRUE;
            NNS_G2dCharCanvasClear(&mScene->mText1B35C.mCanvas, 0);
            mScene->mText1B35C.mMessage = 0x1E;
            mScene->mText1B35C.DrawAll();
            SetCanvasPosition(&mScene->mCharBox, data_ov076_021587f8);
            func_ov076_02153cf4(this, (wchar_t *)&mScene->mCharacter.mUnknown058[0x16]);
            break;
        }
        case 7: {
            mScene->mText1B35C.mSprite.mVisible = TRUE;
            NNS_G2dCharCanvasClear(&mScene->mText1B35C.mCanvas, 0);
            mScene->mText1B35C.mMessage = 0x20;
            mScene->mText1B35C.DrawAll();
            SetCanvasPosition(&mScene->mCharBox, data_ov076_021587f8);
            MiiData_GetName(&mScene->mCharacter, text);
            func_ov076_02153cf4(this, text);
            break;
        }
        case 9: {
            mScene->mText1B35C.mSprite.mVisible = TRUE;
            NNS_G2dCharCanvasClear(&mScene->mText1B35C.mCanvas, 0);
            mScene->mText1B35C.mMessage = 0x24;
            mScene->mText1B35C.DrawAll();
            SetCanvasPosition(&mScene->mCharBox, data_ov076_021587f8);
            MiiData_GetCreatorName(&mScene->mCharacter, text);
            func_ov076_02153cf4(this, text);
            break;
        }
        }
        func_ov076_021545dc(this, state);
        int x = mKeyboard.mLength * 13;
        x = CLAMP(x, 0, 0x75);
        mKeyboard.mUnknown5F8C += x << FX32_SHIFT;
        Sprite_Init(&mButton, 3, 0, 0x80, 0x60, TRUE);
        func_02031eb0(&mButton, 0x18, 0x16);
        func_02031278(&mButton, 10, 11);
        func_02031260(&mButton, 4);
        func_02032140(&mButton, 0x16, -1, 0, -1);
        mButton.mUnknownDC = 14;
        mButton.SetPositionFromVec(0xF2000, 0xD000);
        func_0203119c(&mButton);
        break;
    }
    case 11:
        func_02034720(&mWindows[2], 0x12, 1);
        if (0xFFF == mScene->mCharacter.mYear) {
            func_02098cf4(&mScene->mCharacter, 1990);
        }
        func_ov076_02153e54(this, mScene->mCharacter.mYear);
        func_ov076_02154574(this);
        break;
    case 12:
        func_02034720(&mWindows[3], 0x12, 1);
        if (func_02098dac(&mScene->mCharacter, 0x26) == 0 || func_02098dac(&mScene->mCharacter, 0x27) == 0) {
            func_02098c40(&mScene->mCharacter, 0x26, 1);
            func_02098c40(&mScene->mCharacter, 0x27, 1);
        }
        func_ov076_021540a4(this, func_02098dac(&mScene->mCharacter, 0x26));
        func_ov076_02153fb8(this, func_02098dac(&mScene->mCharacter, 0x27));
        break;
    case 13: {
        func_02034720(&mWindows[0], 0x12, 1);
        int choice = mScene->mCharacter.mUnknown050Bits29;
        if (choice >= 0 && choice <= 3) {
            Sprite_SetSequence(&mWindows[0].mSprites[choice].mSprite, (u16)(data_ov076_02158df4[choice] + 1), TRUE);
        }
        break;
    }
    case 14: {
        func_02034720(&mWindows[1], 0x12, 1);
        int color = func_02098dac(&mScene->mCharacter, 0x28);
        if (color >= 0 && color <= 11) {
            Sprite_SetSequence(&mWindows[1].mSprites[color].mSprite, (u16)(data_ov076_02158df4[color + 4] + 1), TRUE);
        }
        break;
    }
    case 15:
        for (int i = 0; i < 9; i++) {
            mScene->mTextBoxes[i].mSprite.mVisible = FALSE;
        }
        break;
    case 10: {
        func_02034720(&mWindows[4], 0x12, 1);
        int value = mScene->mCharacter.mUnknown298;
        if (value == 1) {
            break;
        }
        int choice;
        switch (value) {
        case 2:
            choice = 0;
            break;
        case 3:
            choice = 1;
            break;
        case 4:
            choice = 2;
            break;
        case 5:
            choice = 3;
            break;
        case 0:
            choice = 4;
            break;
        }
        Sprite_SetSequence(&mWindows[4].mButtons[choice].mSprite, (u16)(data_ov076_02158df4[choice + 24] + 1), TRUE);
        break;
    }
    case 16: {
        func_02034720(&mWindows[5], 0x12, 1);
        int sex = MiiData_GetSex(&mScene->mCharacter);
        if (sex >= 0 && sex <= 1) {
            Sprite_SetSequence(&mWindows[5].mSprites[sex].mSprite, (u16)(data_ov076_02158df4[sex + 16] + 1), TRUE);
        }
        break;
    }
    }
    mState = state;
    mUnknown00004 = 0;
}

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
    p[44] = &data_ov076_02158880;
    p[45] = &data_ov076_0215888c;
    p[46] = &sLightDirection1;
    p[47] = &sLightDirection0;
    p[48] = &data_ov076_021588b0;
    p[49] = &data_ov076_021588bc;
    p[50] = &data_ov076_021588cc;
    p[51] = &data_ov076_021588dc;
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
    p[63] = &data_ov076_021589b4;
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
    p[85] = &data_ov076_02158ca4;
    p[86] = &data_ov076_02158cd4;
    p[87] = &data_ov076_02158d04;
    p[88] = &data_ov076_02158d34;
    p[89] = &data_ov076_02158d64;
    p[90] = &data_ov076_02158d94;
    p[91] = &data_ov076_02158dc4;
    p[92] = &data_ov076_02158df4;
    p[93] = &data_ov076_02158e30;
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
    p[104] = &data_ov076_021590dc;
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

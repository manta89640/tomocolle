#include "MiiEditScene.h"
#include "sound/SoundManager.h"

#include <wchar.h>

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
static const WindowPoint data_ov076_021587b8 = { 128, 76 };
static const WindowPoint data_ov076_02158790 = { 181, 149 };
static const WindowPoint data_ov076_021587a8 = { 75, 149 };
static const WindowPoint data_ov076_021587d0 = { 128, 76 };
static const WindowPoint data_ov076_021587d8 = { 36, 40 };
static const WindowPoint data_ov076_021587c0 = { 94, 30 };
static const WindowPoint data_ov076_021587c8 = { 32, 24 };
static const WindowPoint data_ov076_021587f8 = { 65, 38 };
static const WindowPoint data_ov076_02158808 = { 94, 30 };
static const WindowPoint data_ov076_02158798 = { 42, 36 };
static const WindowPoint data_ov076_02158800 = { 38, 33 };
static const WindowPoint data_ov076_021587e8 = { 8, 7 };
static const u32 data_ov076_021587f0[2] = { 2, 3 };
static const WindowPoint data_ov076_021587b0 = { 16, 7 };
static const WindowPoint data_ov076_02158810 = { 30, 20 };
static const WindowPoint data_ov076_021587a0 = { 75, 149 };
static const WindowPoint data_ov076_021587e0 = { 36, 40 };
static const u16 data_ov076_02158822[5] = { 0x1, 0x2, 0x3, 0x4, 0x5 };
static const u16 data_ov076_02158818[5] = { 0x26, 0x26, 0x26, 0x26, 0x26 };
static const VecFx32 sLightDirection1 = { 0, -4000, 1500 };
static const int data_ov076_021588b0[3] = { 4, 4, 4 };
static const VecFx32 data_ov076_02158850 = { 0, 7844, 58762 };
static const int data_ov076_02158874[3] = { 0, 2, 2 };
static const VecFx32 data_ov076_0215885c = { 0, 7248, 105456 };
static const VecFx32 data_ov076_02158838 = { 0, 0, -32768 };
static const u32 data_ov076_02158868[3] = { 41, 5, 6 };
static const int data_ov076_02158844[3] = { 1, 0, 0 };
static const u16 data_ov076_02158880[6] = { 0x58, 0x5a, 0x5c, 0x5e, 0x28, 0x2a };
static const int data_ov076_0215888c[3] = { 0, 1, 1 };
static const VecFx32 sLightDirection0 = { 1600, -600, -4000 };
static const VecFx32 data_ov076_0215882c = { 0, 0, 0 };
static const WindowPoint data_ov076_021588cc[2] = { { 128, 96 }, { 128, 96 } };
static const ScreenRect data_ov076_021588dc = { 20, 20, 216, 152 };
static const int data_ov076_021588ec[4] = { 16, -1, 0, -1 };
static const int data_ov076_021588bc[4] = { 202, 112, 52, 8 };
static const WindowPoint data_ov076_021588fc[2] = { { 75, 149 }, { 181, 149 } };
static const int data_ov076_0215890c[4] = { 145, 65, 145, 119 };
static const int data_ov076_0215891c[4] = { 5, -1, 19, -1 };
static const WindowPoint data_ov076_0215894c[2] = { { 94, 30 }, { 94, 30 } };
static const int data_ov076_0215892c[4] = { 80, 77, 123, 77 };
static const int data_ov076_0215893c[4] = { 14, 15, 16, 17 };
static const u16 data_ov076_0215895c[10] = { 0x0, 0x2, 0x4, 0x6, 0xa, 0xc, 0x10, 0x14, 0x16, 0xe };
static const int data_ov076_02158970[5] = { 4, 4, 4, 4, 4 };
static const WindowPoint data_ov076_02158984[3] = { { 62, 22 }, { 116, 30 }, { 116, 30 } };
static const int data_ov076_0215899c[6] = { 14, 14, 14, 14, 14, 1 };
static const int data_ov076_021589b4[6] = { 3, 3, 3, 3, 4, 4 };
static const WindowPoint data_ov076_021589cc[3] = { { 136, 132 }, { 62, 175 }, { 194, 175 } };
static const int data_ov076_021589e4[6] = { 14, 15, 16, 17, 22, 23 };
static const int data_ov076_021589fc[8] = { 207, 48, 239, 48, 207, 74, 239, 74 };
static const ButtonSounds data_ov076_02158a1c[2] = { { 14, -1, 0, -1 }, { 16, -1, 0, -1 } };
static const ButtonSounds data_ov076_02158a3c[2] = { { 15, -1, 0, -1 }, { 16, -1, 0, -1 } };
static const int data_ov076_02158a5c[8] = { 80, 90, 176, 90, 80, 90, 176, 90 };
static const int data_ov076_02158a7c[8] = { 80, 102, 176, 102, 80, 90, 176, 90 };
static const int data_ov076_02158a9c[8] = { 58, 54, 58, 54, 58, 54, 58, 54 };
static const int data_ov076_02158abc[8] = { 158, 76, 138, 76, 118, 76, 98, 76 };
static const int data_ov076_02158adc[8] = { 170, 76, 150, 76, 90, 76, 70, 76 };
static const int data_ov076_02158afc[8] = { 80, 77, 123, 77, 80, 114, 123, 114 };
static const int data_ov076_02158b1c[8] = { 14, 15, 16, 17, 18, 19, 20, 21 };
static const int data_ov076_02158b3c[9] = { 0, 1, 4, 2, 3, 6, 7, 8, 9 };
static const int data_ov076_02158b60[10] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };
static const ScreenPoint sSpriteB0Places[5] = { { 55, 182 }, { 55, 182 }, { 103, 182 }, { 151, 182 }, { 199, 182 } };
static const WindowPoint data_ov076_02158bb0[5] = { { 128, 26 }, { 128, 59 }, { 128, 92 }, { 128, 125 }, { 128, 158 } };
static const WindowPoint data_ov076_02158bd8[5] = { { 128, 112 }, { 128, 88 }, { 128, 64 }, { 128, 16 }, { 128, 40 } };
static const WindowPoint data_ov076_02158c00[5] = { { 8, 20 }, { 8, 20 }, { 8, 20 }, { 8, 20 }, { 8, 20 } };
static const WindowPoint data_ov076_02158c28[5] = { { 128, 26 }, { 128, 59 }, { 128, 92 }, { 128, 125 }, { 128, 158 } };
static const WindowPoint data_ov076_02158c50[5] = { { 12, 12 }, { 12, 12 }, { 12, 12 }, { 12, 12 }, { 12, 12 } };
static const u16 data_ov076_02158c78[22] = { 0x0, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x20, 0x22, 0x24, 0x28, 0x2a, 0x2c, 0x2e };
static const int data_ov076_02158ca4[12] = { 40, 18, 40, 18, 40, 18, 40, 18, 40, 18, 24, 22 };
static const ButtonSounds data_ov076_02158cd4[3] = { { 20, -1, 0, -1 }, { 22, -1, 0, -1 }, { 20, -1, 0, -1 } };
static const int data_ov076_02158d04[12] = { 1, 10, 2, 3, 6, 0, 5, 4, 8, 9, 7, 11 };
static const int data_ov076_02158d34[12] = { 21, 19, 21, 21, 22, 22, 23, 23, 24, 24, 23, 22 };
static const int data_ov076_02158d64[12] = { 207, 48, 239, 48, 207, 74, 239, 74, 207, 100, 239, 100 };
static const int data_ov076_02158d94[12] = { 20, -1, 0, -1, 21, -1, 0, -1, 22, -1, 0, -1 };
static const int data_ov076_02158dc4[12] = { 32, 181, 80, 181, 128, 181, 176, 181, 224, 181, 242, 13 };
static const u16 data_ov076_02158df4[30] = { 0x18, 0x1a, 0x1c, 0x1e, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x20, 0x22, 0x3c, 0x3e, 0x3c, 0x3e, 0x40, 0x42, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x0 };
static const char data_ov076_02158e30[] = "E00:/Window_down.nsc";
static const u8 data_ov076_02158e48[24] = { 0x1a, 0x1a, 0x1b, 0x19, 0x1a, 0x19, 0x1a, 0x19, 0x1c, 0x19, 0x1a, 0x18, 0x1b, 0x1b, 0x1a, 0x1a, 0x19, 0x19, 0x1a, 0x1a, 0x1b, 0x1a, 0x19, 0x1b };
static const u8 data_ov076_02158e60[50] = { 0x1d, 0x1c, 0x1c, 0x1c, 0x1d, 0x1c, 0x1c, 0x1c, 0x1d, 0x1c, 0x1c, 0x1c, 0x1c, 0x1d, 0x1d, 0x1c, 0x1c, 0x1c, 0x1d, 0x1d, 0x1c, 0x1d, 0x1c, 0x1d, 0x1d, 0x1c, 0x1d, 0x1c, 0x1c, 0x1d, 0x1c, 0x1c, 0x1c, 0x1d, 0x1d, 0x1d, 0x1c, 0x1c, 0x1d, 0x1d, 0x1d, 0x1c, 0x1c, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1c, 0x1c };
static const int data_ov076_02158e94[16] = { 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1 };
static const int data_ov076_02158ed4[16] = { 80, 40, 123, 40, 80, 77, 123, 77, 80, 114, 123, 114, 80, 151, 123, 151 };
static const int data_ov076_02158f14[16] = { 207, 48, 239, 48, 207, 74, 239, 74, 207, 100, 239, 100, 207, 126, 239, 126 };
static const u16 data_ov076_02158f54[36] = { 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e, 0x40, 0x42, 0x44, 0x46, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x0, 0x48, 0x4a, 0x4c, 0x4e, 0x50, 0x52, 0x54, 0x56 };
static const int data_ov076_02158f9c[18] = { 59, 77, 102, 77, 145, 77, 59, 114, 102, 114, 145, 114, 59, 151, 102, 151, 145, 151 };
static const WindowPoint data_ov076_02158fe4[10] = { { 70, 40 }, { 186, 40 }, { 128, 112 }, { 160, 76 }, { 68, 76 }, { 202, 112 }, { 216, 76 }, { 68, 149 }, { 192, 149 }, { 54, 112 } };
static const int data_ov076_02159034[20] = { 116, 18, 116, 18, 32, 18, 56, 18, 112, 18, 32, 18, 56, 18, 112, 18, 48, 18, 48, 18 };
static const int data_ov076_02159084[22] = { 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3 };
static const int data_ov076_021590dc[24] = { 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 2, -1, -1, -1, 20, -1, 0, -1, 16, -1, 0, -1 };
static const int data_ov076_0215913c[24] = { 6, 0, 12, 1, 9, 19, 7, 21, 8, 17, 5, 4, 11, 10, 2, 3, 14, 20, 15, 13, 22, 18, 16, 23 };
static const int data_ov076_0215919c[24] = { 23, 1, 19, 21, 22, 5, 0, 8, 10, 16, 6, 13, 7, 9, 2, 17, 3, 4, 15, 11, 20, 18, 14, 12 };
static const int data_ov076_021591fc[24] = { 59, 40, 102, 40, 145, 40, 59, 77, 102, 77, 145, 77, 59, 114, 102, 114, 145, 114, 59, 151, 102, 151, 145, 151 };
static const int data_ov076_0215925c[29] = { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5 };
static const ScreenRect data_ov076_021592d0[8] = { { 73, -6, 12, 12 }, { 87, -6, 12, 12 }, { 101, -6, 12, 12 }, { 115, -6, 12, 12 }, { 129, -6, 12, 12 }, { 143, -6, 12, 12 }, { 157, -6, 12, 12 }, { 171, -6, 12, 12 } };
static const int data_ov076_02159350[36] = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3 };
static const int data_ov076_021593e0[40] = { 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1, 20, -1, 0, -1 };
static const WindowPoint data_ov076_02159480[22] = { { 68, 52 }, { 92, 52 }, { 116, 52 }, { 140, 52 }, { 164, 52 }, { 188, 52 }, { 68, 92 }, { 68, 132 }, { 206, 134 }, { 237, 134 }, { 237, 134 }, { 206, 20 }, { 237, 20 }, { 206, 45 }, { 237, 45 }, { 206, 70 }, { 237, 70 }, { 206, 95 }, { 99, 138 }, { 118, 138 }, { 137, 138 }, { 156, 138 } };
static const WindowPoint data_ov076_02159530[22] = { { 22, 22 }, { 22, 22 }, { 22, 22 }, { 22, 22 }, { 22, 22 }, { 22, 22 }, { 22, 22 }, { 22, 22 }, { 30, 24 }, { 30, 24 }, { 30, 24 }, { 30, 24 }, { 30, 24 }, { 30, 24 }, { 30, 24 }, { 30, 24 }, { 30, 24 }, { 30, 24 }, { 16, 16 }, { 16, 16 }, { 16, 16 }, { 16, 16 } };
static const int data_ov076_021595e0[48] = { 2, 4, 0, 8, 39, 17, 1, 26, 16, 15, 27, 20, 33, 11, 19, 32, 9, 12, 23, 34, 21, 25, 40, 35, 5, 41, 13, 36, 37, 6, 24, 30, 31, 18, 28, 46, 7, 44, 38, 42, 45, 29, 3, 43, 22, 10, 14, 47 };
static const int data_ov076_021596a0[58] = { 58, 80, 105, 80, 152, 80, 199, 80, 48, 64, 80, 64, 112, 64, 144, 64, 176, 64, 208, 64, 48, 96, 80, 96, 112, 96, 144, 96, 176, 96, 208, 96, 80, 80, 176, 80, 128, 41, 128, 111, 80, 41, 80, 111, 160, 41, 160, 111, 128, 48, 128, 72, 128, 96, 128, 120, 128, 144 };
static const int data_ov076_02159788[58] = { 42, 36, 42, 36, 42, 36, 42, 36, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 58, 54, 58, 54, 20, 15, 20, 15, 20, 15, 20, 15, 20, 15, 20, 15, 126, 20, 126, 20, 126, 20, 126, 20, 126, 20 };
static const int data_ov076_02159870[72] = { 33, 40, 51, 44, 39, 70, 45, 49, 59, 56, 68, 31, 32, 47, 37, 48, 66, 52, 58, 50, 55, 64, 60, 62, 43, 38, 42, 23, 67, 54, 36, 41, 65, 30, 57, 34, 12, 13, 69, 26, 4, 25, 1, 19, 5, 8, 27, 7, 14, 3, 22, 10, 6, 20, 11, 63, 17, 35, 21, 0, 61, 16, 46, 9, 18, 2, 28, 53, 71, 24, 15, 29 };
static const int data_ov076_02159990[72] = { 17, 159, 17, 12, 17, 33, 17, 54, 17, 75, 17, 96, 17, 117, 17, 138, 77, 11, 127, 11, 77, 11, 127, 11, 77, 30, 127, 30, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 223, 74, 68, 11, 136, 11, 223, 155, 179, 30, 179, 48, 179, 66, 179, 84, 179, 102, 179, 120, 179, 138, 179, 156 };
static const ButtonSounds data_ov076_02159ab0[22] = { { 2, -1, -1, -1 }, { 2, -1, -1, -1 }, { 2, -1, -1, -1 }, { 2, -1, -1, -1 }, { 2, -1, -1, -1 }, { 2, -1, -1, -1 }, { 2, -1, -1, -1 }, { 2, -1, -1, -1 }, { 14, -1, 0, -1 }, { 1, -1, 0, -1 }, { 0, -1, -1, -1 }, { 0, -1, -1, -1 }, { 0, -1, -1, -1 }, { 0, -1, -1, -1 }, { 0, -1, -1, -1 }, { 0, -1, -1, -1 }, { 0, -1, -1, -1 }, { 0, -1, -1, -1 }, { 2, -1, -1, -1 }, { 2, -1, -1, -1 }, { 2, -1, -1, -1 }, { 2, -1, -1, -1 } };
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
        mKeyboard.Draw();
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

// What the profile page uses of the main program (stand-ins)
extern "C" {
void func_02037478(wchar_t *text); // Applied to the texts of numbers before they are shown
void ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
void func_02031e88(SpriteButton *button, BOOL disabled); // Sets mUnknownB8, and draws the button darkened while it is set
int func_02098dac(SavedCharacter *character, int param);  // A parameter of the character (0x26, 0x27: birth month, day)
BOOL func_02098e50(SavedCharacter *character, int param);
void func_020986c4(SavedCharacter *character, int param, int *min, int *max); // The range of a parameter
BOOL func_02098584(SavedCharacter *character);
}

// Puts a text in mText (up to 10 characters) and shows it in the scene's character box, 13 pixels a character
void MiiEditProfilePage::SetText(const wchar_t *text) {
    MI_CpuClear16(mText, sizeof(mText));
    mTextLength = 0;
    NNS_G2dCharCanvasClear(&mScene->mCharBox.mCanvas, 0);
    while (mTextLength < 10) {
        u16 c = text[mTextLength];
        if (c == 0) {
            return;
        }
        mText[mTextLength] = c;
        int x = mTextLength * 13 + 5;
        x -= NNS_G2dFontGetCharWidths(mScene->mCharBox.mFont, c)->charWidth >> 1;
        mScene->mCharBox.mCharX = x;
        mScene->mCharBox.SetChar(c);
        mScene->mCharBox.Redraw();
        mScene->mCharBox.Transfer();
        mTextLength++;
    }
}

// Shows a number of up to four digits with the sprites of the third window, without its leading zeros
void MiiEditProfilePage::SetNumberDigits(int value) {
    int ones = value % 10;
    int tens = value % 100 / 10;
    int hundreds = value % 1000 / 100;
    int thousands = value % 10000 / 1000;
    Sprite_SetFrame(&mWindows[2].mPlainSprites[0], ones);
    Sprite_SetFrame(&mWindows[2].mPlainSprites[1], tens);
    Sprite_SetFrame(&mWindows[2].mPlainSprites[2], hundreds);
    Sprite_SetFrame(&mWindows[2].mPlainSprites[3], thousands);
    BOOL visible[4];
    for (int i = 0; i < 4; i++) {
        visible[i] = TRUE;
    }
    if (value / 10000 <= 0 && thousands == 0) {
        visible[3] = FALSE;
        if (hundreds == 0) {
            visible[2] = FALSE;
            if (tens == 0) {
                visible[1] = FALSE;
            }
        }
    }
    for (int i = 0; i < 4; i++) {
        mWindows[2].mPlainSprites[i].mVisible = visible[i];
    }
}

// Shows the birth day in two digits with the first two sprites of the fourth window
void MiiEditProfilePage::SetDayDigits(int day) {
    int ones = day % 10;
    int tens = day % 100 / 10;
    Sprite_SetFrame(&mWindows[3].mPlainSprites[0], ones);
    Sprite_SetFrame(&mWindows[3].mPlainSprites[1], tens);
    BOOL visible[2];
    for (int i = 0; i < 2; i++) {
        visible[i] = TRUE;
    }
    if (day / 100 <= 0 && tens == 0) {
        visible[1] = FALSE;
        if (ones == 0) {
            visible[0] = FALSE;
        }
    }
    for (int i = 0; i < 2; i++) {
        mWindows[3].mPlainSprites[i].mVisible = visible[i];
    }
}

// Shows the birth month in two digits with the next two sprites of the fourth window
void MiiEditProfilePage::SetMonthDigits(int month) {
    int ones = month % 10;
    int tens = month % 100 / 10;
    Sprite_SetFrame(&mWindows[3].mPlainSprites[2], ones);
    Sprite_SetFrame(&mWindows[3].mPlainSprites[3], tens);
    BOOL visible[2];
    for (int i = 0; i < 2; i++) {
        visible[i] = TRUE;
    }
    if (month / 100 <= 0 && tens == 0) {
        visible[1] = FALSE;
        if (ones == 0) {
            visible[0] = FALSE;
        }
    }
    for (int i = 0; i < 2; i++) {
        mWindows[3].mPlainSprites[i + 2].mVisible = visible[i];
    }
}

// Puts a text box at a place in pixels
static inline void SetTextBoxPosition(ObjTextBox *box, WindowPoint point) {
    box->SetPosition(point.x << FX32_SHIFT, point.y << FX32_SHIFT);
}

// The scene's buffers for the texts that UpdateTexts writes
#define NUMBER_TEXT(scene) ((wchar_t *)&(scene)->mUnknown1B304[0x00])
#define BIRTHDAY_TEXT(scene) ((wchar_t *)&(scene)->mUnknown1B304[0x14])
#define NAME_TEXT(scene) ((wchar_t *)&(scene)->mUnknown1B330[0x00])
#define CREATOR_NAME_TEXT(scene) ((wchar_t *)&(scene)->mUnknown1B330[0x16])

// Writes the character's profile into the scene's nine text boxes, each centred on its place
void MiiEditProfilePage::UpdateTexts() {
    u32 number = mScene->mCharacter.mUnknown050Bits17;
    if (number == 0xFFF) {
        swprintf(NUMBER_TEXT(mScene), 5, L"", number);
    } else {
        swprintf(NUMBER_TEXT(mScene), 5, L"%d", number);
        func_02037478(NUMBER_TEXT(mScene));
    }
    int month = func_02098dac(&mScene->mCharacter, 0x26);
    int day = func_02098dac(&mScene->mCharacter, 0x27);
    if (month > 0 && month <= 12 && day > 0 && day <= 31) {
        swprintf(BIRTHDAY_TEXT(mScene), 6, L"%d/%d", month, day);
        func_02037478(BIRTHDAY_TEXT(mScene));
    } else {
        swprintf(BIRTHDAY_TEXT(mScene), 6, L"");
    }
    mScene->mTextBoxes[0].SetText((const NNSG2dChar *)mScene->mCharacter.mUnknown06E);
    mScene->mTextBoxes[1].SetText((const NNSG2dChar *)mScene->mCharacter.mName);
    MiiData_GetName(&mScene->mCharacter, NAME_TEXT(mScene));
    mScene->mTextBoxes[2].SetText((const NNSG2dChar *)NAME_TEXT(mScene));
    mScene->mTextBoxes[8].SetText((const NNSG2dChar *)data_ov076_0215a10c[MiiData_GetSex(&mScene->mCharacter)]);
    mScene->mTextBoxes[3].SetText(
        (const NNSG2dChar *)mScene->mMessageFiles[0].GetText(mScene->mCharacter.mUnknown050Bits29 + 0x5A));
    mScene->mTextBoxes[4].SetText((const NNSG2dChar *)NUMBER_TEXT(mScene));
    mScene->mTextBoxes[5].SetText((const NNSG2dChar *)BIRTHDAY_TEXT(mScene));
    MiiData_GetCreatorName(&mScene->mCharacter, CREATOR_NAME_TEXT(mScene));
    mScene->mTextBoxes[6].SetText((const NNSG2dChar *)CREATOR_NAME_TEXT(mScene));
    mScene->mTextBoxes[7].SetText(
        (const NNSG2dChar *)mScene->mMessageFiles[0].GetText(mScene->mCharacter.mUnknown298 + 0x54));
    if (!func_02098584(&mScene->mCharacter)) {
        mScene->mTextBoxes[2].mColor = 9;
        mScene->mTextBoxes[6].mColor = 9;
        BOOL monthSet = func_02098e50(&mScene->mCharacter, 0x26);
        BOOL daySet = func_02098e50(&mScene->mCharacter, 0x27);
        if (monthSet && daySet) {
            mScene->mTextBoxes[5].mColor = 9;
        }
    }
    for (u16 i = 0; i < 9; i++) {
        mScene->mUnknown1B32C = 1;
        int width = NNS_G2dTextCanvasGetTextWidth(&mScene->mTextBoxes[i].mTextCanvas, mScene->mTextBoxes[i].mText);
        const WindowPoint *position = &data_ov076_02158fe4[data_ov076_02158b3c[i]];
        WindowPoint point;
        point.x = position->x + -width / 2;
        point.y = position->y - 7;
        SetTextBoxPosition(&mScene->mTextBoxes[i], point);
    }
    Sprite_SetFrame(&mSprites[0], func_02098dac(&mScene->mCharacter, 0x28));
}

// Darkens the second button of the third window while the birth day is past the last day the month has
void MiiEditProfilePage::UpdateDayButton() {
    int min, max;
    func_020986c4(&mScene->mCharacter, 0x27, &min, &max);
    if (func_02098dac(&mScene->mCharacter, 0x27) > max) {
        func_02031e88((SpriteButton *)&mWindows[2].mButtons[1], TRUE);
    } else {
        func_02031e88((SpriteButton *)&mWindows[2].mButtons[1], FALSE);
    }
}

// Enables the keyboard's first key in the states that type a text, when func_ov076_0214bf20 accepts the text
// (state 9 also with no text at all)
void MiiEditProfilePage::UpdateKeyboardKey(int state) {
    BOOL enabled = FALSE;
    switch (state) {
    case 3:
    case 5:
    case 7:
        enabled = mScene->func_ov076_0214bf20(mText) ? TRUE : FALSE;
        break;
    case 9:
        if (wcslen(mText) == 0 || mScene->func_ov076_0214bf20(mText)) {
            enabled = TRUE;
        } else {
            enabled = FALSE;
        }
        break;
    }
    mKeyboard.mButtons[0].SetVisible(enabled);
}

// --- The voice page ---

// What the voice page uses of the main program (stand-ins)
struct VoiceParamRange {
    /* 0x0 */ s16 mUnknown0;
    /* 0x2 */ s16 mMin;
    /* 0x4 */ s16 mMax;
    /* 0x6 */ s16 mUnknown6;
};

extern "C" {
extern const VoiceParamRange data_020fb940[]; // Of the five values of CharacterRecord::mVoice
void func_0203217c(MiiEditSprite100 *slider, int x, int a, int b, int left, int top, int width, int height);
void func_020325a8(MiiEditSprite100 *slider, u16 sequence, u16 pressedSequence);
void func_020325c4(MiiEditSprite100 *slider, int a, int b, int c, int d);
void func_02032514(MiiEditSprite100 *slider, int min, int max, int value);
BOOL func_02031d00(SpriteButton *button);
void func_0204e4b4(CharacterSpeechTexts *texts, SavedCharacter *character); // Fills them from the character
void func_0204e62c(wchar_t *text, int value);                               // Writes a number
void func_0209d898(SceneCharacter *character, TtsPlayer *player);
void Character_SetFace(SceneCharacter *character, int face);
void func_020372bc(wchar_t *text, int size);
}

// The scene's buffer of the text to speak, in Shift JIS (0x102 bytes)
#define SPEECH_TEXT(scene) ((char *)&(scene)->mUnknownAfterTts[0x1A264 - 0x5920 - sizeof(TtsPlayer)])

void MiiEditVoicePage::func_ov076_02154690(MiiEditScene *scene) {
    mState = -1;
    mScene = scene;
    func_02027534(&data_021213b8, "E00:/Obj_down.ncl", 0x1000, -1, -1, -1);
    func_02027d7c(&data_02121834, "E00:/Window_down.nsc", 0x100, 0, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, "E00:/BG_down_editface.nsc", 0x200, 0, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, "E00:/BG_down.nsc", 0x400, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "E00:/BG_down.ncg", 0x2F00, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "E00:/BG_down.ncl", 0x2F00, -1, -1, -1);
    mScene->func_ov076_0214bbe4();
    ObjResourceManager_Load(&gObjResourceManager, "E00:/Voice.nce", "E00:/Voice.ncg", 0x1000, 1, 0x10000);
    for (u16 i = 0; i < 22; i++) {
        u16 sequence = data_ov076_02158c78[i];
        Sprite_Init(&mButtons[i], 1, sequence, 0x80, 0x60, TRUE);
        func_02031eb0(&mButtons[i], data_ov076_02159530[i].x, data_ov076_02159530[i].y);
        func_02031278(&mButtons[i], sequence, sequence + 1);
        func_02031260(&mButtons[i], data_ov076_02159084[i]);
        func_02032158(&mButtons[i], &data_ov076_02159ab0[i]);
        mButtons[i].mUnknownDC = 14;
        mButtons[i].SetPositionFromVec(data_ov076_02159480[i].x << FX32_SHIFT,
                                       data_ov076_02159480[i].y << FX32_SHIFT);
        func_0203119c(&mButtons[i]);
        mButtons[i].mVisible = FALSE;
    }
    for (u16 i = 0; i < 3; i++) {
        u16 sequence = data_ov076_02158788[i];
        Sprite_Init(&mTextButtons[i], data_ov076_02158844[i], sequence, 0x80, 0x60, TRUE);
        func_02031eb0(&mTextButtons[i], data_ov076_02158984[i].x, data_ov076_02158984[i].y);
        func_02031278(&mTextButtons[i], sequence, sequence + 1);
        func_02031260(&mTextButtons[i], data_ov076_021588b0[i]);
        func_02032158(&mTextButtons[i], &data_ov076_02158cd4[i]);
        mTextButtons[i].mUnknownDC = 14;
        mTextButtons[i].SetPositionFromVec(data_ov076_021589cc[i].x << FX32_SHIFT,
                                           data_ov076_021589cc[i].y << FX32_SHIFT);
        func_0203119c(&mTextButtons[i]);
        mTextButtons[i].mVisible = FALSE;
    }
    for (u16 i = 0; i < 3; i++) {
        mTextButtons[i].mText.mSprite.mVisible = FALSE;
    }
    for (u16 i = 0; i < 2; i++) {
        Sprite_Init(&mSprites29D4[i], 1, data_ov076_02158764[i], 0x80, 0x60, TRUE);
        mSprites29D4[i].SetPositionFromVec(data_ov076_021588cc[i].x << FX32_SHIFT,
                                           data_ov076_021588cc[i].y << FX32_SHIFT);
    }
    for (int i = 0; i < 5; i++) {
        u16 sequence = data_ov076_02158818[i];
        Sprite_Init(&mSprites2B2C[i].mSprite, 1, sequence, 0x80, 0x60, TRUE);
        func_0203217c(&mSprites2B2C[i], data_ov076_02158bd8[i].x, 0x50, 1, -data_ov076_02158c00[i].x / 2,
                      -data_ov076_02158c00[i].y / 2, data_ov076_02158c00[i].x, data_ov076_02158c00[i].y);
        mSprites2B2C[i].mUnknownD8 = 1;
        func_020325a8(&mSprites2B2C[i], sequence, sequence + 1);
        func_020325c4(&mSprites2B2C[i], -1, 6, 7, 10);
        mSprites2B2C[i].mUnknownF4 = 14;
        mSprites2B2C[i].mSprite.mPosition.y = data_ov076_02158bd8[i].y << FX32_SHIFT;
    }
    mUnknown3B5C = mScene->mCharacter;
    mScene->MoveCameraTo(data_ov076_0215885c);
    func_0204e4b4(&mSpeechTexts, &mScene->mCharacter);
    mText.mMessageFile = &mScene->mMessageFiles[0];
    int voice = mScene->mCharacter.mUnknown054Bits0;
    if (voice >= 0 && voice <= 7) {
        func_ov076_02157b94();
    }
    SetState(1);
}

extern "C" {
BOOL func_02024004(ScreenControl *screens, int planes, int level); // The screens' fades (as overlay 72 calls them)
BOOL func_02024118(ScreenControl *screens, int planes, int level);
extern MATHRandContext16 gRandom;
void func_020321ec(MiiEditSprite100 *slider);                // Updates it
int func_020324e0(MiiEditSprite100 *slider, int min, int max); // Its value in the range
BOOL func_02032414(MiiEditSprite100 *slider);
void func_02032578(MiiEditSprite100 *slider, BOOL a);
}

// A random number from min to max (max not included)
static inline s16 RandomInRange(int min, int max) {
    return MATH_Rand16(&gRandom, max - min) + min;
}

// The voice presets: the six values of CharacterRecord::mVoice for each
#define VOICE_PRESETS ((const s16(*)[6])data_020fbaac)

void MiiEditVoicePage::func_ov076_02155094() {
    switch (mState) {
    case 0:
    case 3:
        if (func_02024118(&data_0211d604, 0x3F00, 2)) {
            SetState(mState + 1);
        }
        break;
    case 1:
    case 2:
        if (mState == 1) {
            SetState(2);
        }
        if (func_02024004(&data_0211d604, 0x3F00, -2)) {
            for (int i = 0; i < 22; i++) {
                if (!func_0203133c(&mButtons[i])) {
                    continue;
                }
                switch (i) {
                case 0:
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    for (int k = 0; k < 6; k++) {
                        func_02098d68(&mScene->mCharacter, k, VOICE_PRESETS[i][k]);
                    }
                    ShowVoiceSettings();
                    func_ov076_02157b94();
                    mScene->mCharacter.mUnknown054Bits0 = i;
                    for (int k = 0; k <= 7; k++) {
                        if (i != k) {
                            func_02031e28(&mButtons[k], FALSE);
                        }
                    }
                    mTextButtons[0].mVisible = FALSE;
                    break;
                case 6:
                    for (int k = 0; k < 5; k++) {
                        func_02098d68(&mScene->mCharacter, k, RandomInRange(data_020fb940[k].mMin, data_020fb940[k].mMax));
                    }
                    ShowVoiceSettings();
                    func_ov076_02157b94();
                    mScene->mCharacter.mUnknown054Bits0 = i;
                    for (int k = 0; k <= 7; k++) {
                        if (i != k) {
                            func_02031e28(&mButtons[k], FALSE);
                        }
                    }
                    mTextButtons[0].mVisible = FALSE;
                    break;
                case 7:
                    (CharacterRecord &)mScene->mCharacter = mUnknown3B5C;
                    func_ov076_02157b94();
                    mScene->mCharacter.mUnknown054Bits0 = i;
                    for (int k = 0; k <= 7; k++) {
                        if (i != k) {
                            func_02031e28(&mButtons[k], FALSE);
                        }
                    }
                    mTextButtons[0].mVisible = TRUE;
                    break;
                }
            }
        }
        if (func_0203133c(&mTextButtons[0])) {
            func_ov076_02157b94();
            func_02031e28(&mButtons[7], TRUE);
            mScene->mCharacter.mUnknown054Bits0 = 7;
            for (int k = 0; k <= 7; k++) {
                if (k != 7) {
                    func_02031e28(&mButtons[k], FALSE);
                }
            }
            SetState(3);
        }
        {
            int voice = mScene->mCharacter.mUnknown054Bits0;
            if (voice >= 0 && voice <= 7) {
                func_02031e28(&mButtons[voice], TRUE);
            }
        }
        if (mScene->mButtons[3].mUnknownB8) {
            BOOL selected = FALSE;
            for (int k = 0; k <= 7; k++) {
                if (func_02031d00(&mButtons[k])) {
                    selected = TRUE;
                    break;
                }
            }
            if (selected) {
                func_02031e88(&mScene->mButtons[3], FALSE);
                mScene->mUnknown58DC = TRUE;
                mScene->mSprite582C.func_ov076_0214bec8(3);
            }
        }
        break;
    case 4:
        if (!func_02024004(&data_0211d604, 0x3F00, -2)) {
            break;
        }
        for (int i = 0; i < 22; i++) {
            if (!func_0203133c(&mButtons[i])) {
                continue;
            }
            switch (i) {
            case 8:
                mButtons[8].mVisible = FALSE;
                (CharacterRecord &)mScene->mCharacter = mUnknown302C;
                ShowVoiceSettings();
                func_ov076_02157b94();
                break;
            case 9:
                func_ov076_02157b94();
                mButtons[9].mVisible = FALSE;
                mButtons[10].mVisible = TRUE;
                func_0203119c(&mButtons[9]);
                func_0203119c(&mButtons[10]);
                break;
            case 10:
                func_ov076_02157bc0();
                mButtons[9].mVisible = TRUE;
                mButtons[10].mVisible = FALSE;
                func_0203119c(&mButtons[9]);
                func_0203119c(&mButtons[10]);
                break;
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:
                func_ov076_0215707c();
                for (int k = 0; k < 6; k++) {
                    func_02098d68(&mScene->mCharacter, k, VOICE_PRESETS[i - 11][k]);
                }
                ShowVoiceSettings();
                func_ov076_02157b94();
                break;
            case 17:
                func_ov076_0215707c();
                for (int k = 0; k < 5; k++) {
                    func_02098d68(&mScene->mCharacter, k, RandomInRange(data_020fb940[k].mMin, data_020fb940[k].mMax));
                }
                func_02098d68(&mScene->mCharacter, 5, MATH_Rand16(&gRandom, 4));
                ShowVoiceSettings();
                func_ov076_02157b94();
                break;
            case 18:
            case 19:
            case 20:
            case 21:
                func_ov076_0215707c();
                func_02098d68(&mScene->mCharacter, 5, i - 18);
                for (int k = 18; k <= 21; k++) {
                    if (i != k) {
                        func_02031e28(&mButtons[k], FALSE);
                    }
                }
                func_ov076_02157b94();
                break;
            }
        }
        for (int i = 0; i < 3; i++) {
            if (!func_0203133c(&mTextButtons[i])) {
                continue;
            }
            switch (i) {
            case 2:
                mUnknown3B5C = mScene->mCharacter;
                SetState(0);
                break;
            case 1:
                (CharacterRecord &)mScene->mCharacter = mUnknown3B5C;
                mScene->mCharacter.mUnknown054Bits0 = 7;
                mScene->mTtsPlayer.StopEngine();
                func_0209d898(&mScene->mCharacter, &mScene->mTtsPlayer);
                SetState(0);
                break;
            }
        }
        for (int i = 0; i < 5; i++) {
            func_020321ec(&mSprites2B2C[i]);
            if (mSprites2B2C[i].mUnknownBC) {
                int value;
                if (i == 4) {
                    value = func_020324e0(&mSprites2B2C[i], data_020fb940[i].mMax, data_020fb940[i].mMin);
                } else {
                    value = func_020324e0(&mSprites2B2C[i], data_020fb940[i].mMin, data_020fb940[i].mMax);
                }
                func_02098d68(&mScene->mCharacter, i, value);
                func_ov076_02157b94();
            }
            if (func_02032414(&mSprites2B2C[i])) {
                for (int k = 0; k < 5; k++) {
                    if (k != i) {
                        func_02032578(&mSprites2B2C[k], FALSE);
                    }
                }
            }
            if (mSprites2B2C[i].mUnknownC8) {
                func_ov076_0215707c();
            }
        }
        break;
    }
    switch (mSpeakStep) {
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
        if (mScene->mTtsPlayer.IsEngineIdle() != TRUE) {
            return;
        }
        func_ov076_02157508(mSpeakStep + 1);
        return;
    case 6:
        if (mScene->mTtsPlayer.IsEngineIdle() != TRUE) {
            return;
        }
        func_ov076_02157508(1);
        break;
    }
}

// Shows the page's controls for a state: 1 the first, 2 the second, 4 the sliders
void MiiEditVoicePage::SetState(int state) {
    switch (state) {
    case 1: {
        mScene->func_ov076_0214bbe4();
        for (int i = 0; i < 22; i++) {
            mButtons[i].mVisible = FALSE;
        }
        for (u16 i = 0; i < 3; i++) {
            mTextButtons[i].mVisible = FALSE;
        }
        mSprites29D4[0].mVisible = FALSE;
        mSprites29D4[1].mVisible = FALSE;
        for (int i = 0; i < 5; i++) {
            mSprites2B2C[i].mSprite.mVisible = FALSE;
        }
        int voice = mScene->mCharacter.mUnknown054Bits0;
        if (voice >= 0 && voice <= 7) {
            func_02031e28(&mButtons[voice], TRUE);
        }
        break;
    }
    case 2: {
        for (int i = 0; i <= 7; i++) {
            mButtons[i].mVisible = TRUE;
        }
        for (int i = 8; i <= 21; i++) {
            mButtons[i].mVisible = FALSE;
        }
        mSprites29D4[0].mVisible = TRUE;
        mSprites29D4[1].mVisible = FALSE;
        int charOffset = 0x320;
        for (u16 i = 0; i < 3; i++) {
            mTextButtons[i].mText.mSprite.mVisible = TRUE;
            charOffset += func_02031f58(&mTextButtons[i], &mScene->mMessageFiles[data_ov076_0215888c[i]],
                                        data_ov076_02158868[i], &gFonts[data_ov076_02158874[i]].mFont,
                                        0x1000, charOffset);
        }
        if (mScene->mCharacter.mUnknown054Bits0 == 7) {
            mTextButtons[0].mVisible = TRUE;
        } else {
            mTextButtons[0].mVisible = FALSE;
        }
        break;
    }
    case 4:
        mScene->func_ov076_0214bdd8();
        for (int i = 0; i <= 7; i++) {
            mButtons[i].mVisible = FALSE;
        }
        for (int i = 8; i <= 21; i++) {
            mButtons[i].mVisible = TRUE;
        }
        for (u16 i = 0; i <= 0; i++) {
            mTextButtons[i].mVisible = FALSE;
        }
        for (u16 i = 1; i < 3; i++) {
            mTextButtons[i].mVisible = TRUE;
        }
        mSprites29D4[0].mVisible = FALSE;
        mSprites29D4[1].mVisible = TRUE;
        for (int i = 0; i < 5; i++) {
            mSprites2B2C[i].mSprite.mVisible = TRUE;
        }
        ShowVoiceSettings();
        mButtons[9].mVisible = FALSE;
        mButtons[10].mVisible = FALSE;
        func_0203119c(&mButtons[9]);
        func_0203119c(&mButtons[10]);
        mButtons[8].mVisible = FALSE;
        break;
    }
    mState = state;
    mUnknown0004 = 0;
}


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

// Puts the sliders at the character's voice values, and selects the button of its voice type
void MiiEditVoicePage::ShowVoiceSettings() {
    for (int i = 0; i < 5; i++) {
        if (i == 4) {
            func_02032514(&mSprites2B2C[i], data_020fb940[i].mMax, data_020fb940[i].mMin,
                          mScene->mCharacter.mVoice[i]);
        } else {
            func_02032514(&mSprites2B2C[i], data_020fb940[i].mMin, data_020fb940[i].mMax,
                          mScene->mCharacter.mVoice[i]);
        }
    }
    for (u16 i = 18; i <= 21; i++) {
        if (i - 18 == mScene->mCharacter.mVoice[5]) {
            if (!func_02031d00(&mButtons[i])) {
                func_02031e28(&mButtons[i], TRUE);
            }
        } else {
            func_02031e28(&mButtons[i], FALSE);
        }
    }
}

void MiiEditVoicePage::func_ov076_0215707c() {
    mUnknown302C = mScene->mCharacter;
    mButtons[8].mVisible = TRUE;
}

// Speaks a part of the character's profile with the scene's speech player, from the given step on: 0 stops, 1 the
// two texts, 2 the name, 3 the number and birthday, 4 the creator's name, 5 and 6 two messages. A step with nothing
// to say goes on to the next.
void MiiEditVoicePage::func_ov076_02157508(int step) {
    int length = 0x102;
    switch (step) {
    case 0:
        mScene->mTtsPlayer.StopEngine();
        Character_SetFace(&mScene->mCharacter, 8);
        break;
    case 1:
        if (wcslen(mSpeechTexts.mUnknown0BA) + wcslen(mSpeechTexts.mUnknown10C) == 0) {
            func_ov076_02157508(step + 1);
            return;
        }
        MI_CpuFill8(SPEECH_TEXT(mScene), 0, 0x102);
        mText.SetStringArg(mSpeechTexts.mUnknown0BA, 0);
        mText.SetStringArg(mSpeechTexts.mUnknown10C, 1);
        mText.mMessage = 0x2F;
        mText.ExpandMessage();
        STD_ConvertStringUnicodeToSjis(SPEECH_TEXT(mScene), &length, (const u16 *)mText.mText, NULL, NULL);
        mScene->mTtsPlayer.Speak(SPEECH_TEXT(mScene), 0);
        break;
    case 2:
        if (!mScene->func_ov076_0214bf20(mSpeechTexts.mUnknown068)) {
            func_ov076_02157508(step + 1);
            return;
        }
        MI_CpuFill8(SPEECH_TEXT(mScene), 0, 0x102);
        mText.SetStringArg(mSpeechTexts.mUnknown068, 0);
        mText.mMessage = 0x36;
        mText.ExpandMessage();
        STD_ConvertStringUnicodeToSjis(SPEECH_TEXT(mScene), &length, (const u16 *)mText.mText, NULL, NULL);
        mScene->mTtsPlayer.Speak(SPEECH_TEXT(mScene), 0);
        break;
    case 3: {
        MI_CpuFill8(SPEECH_TEXT(mScene), 0, 0x102);
        u32 number = mScene->mCharacter.mUnknown050Bits17;
        int month = func_02098dac(&mScene->mCharacter, 0x26);
        int day = func_02098dac(&mScene->mCharacter, 0x27);
        wchar_t numberText[32];
        wchar_t monthText[32];
        wchar_t dayText[32];
        func_0204e62c(numberText, number);
        func_0204e62c(monthText, month);
        func_0204e62c(dayText, day);
        if (number != 0xFFF && month > 0 && month <= 12 && day > 0 && day <= 31) {
            mText.SetStringArg(numberText, 0);
            mText.SetStringArg(monthText, 1);
            mText.SetStringArg(dayText, 2);
            mText.SetMessageArg(mScene->mMessageFiles[0].GetText(GetStarSignMessage(month, day)), 0);
            mText.mMessage = 0x30;
        } else if (month > 0 && month <= 12 && day > 0 && day <= 31) {
            mText.SetStringArg(monthText, 0);
            mText.SetStringArg(dayText, 1);
            mText.SetMessageArg(mScene->mMessageFiles[0].GetText(GetStarSignMessage(month, day)), 0);
            mText.mMessage = 0x31;
        } else if (number != 0xFFF) {
            mText.SetStringArg(numberText, 0);
            mText.mMessage = 0x32;
        } else {
            func_ov076_02157508(step + 1);
            return;
        }
        mText.ExpandMessage();
        STD_ConvertStringUnicodeToSjis(SPEECH_TEXT(mScene), &length, (const u16 *)mText.mText, NULL, NULL);
        mScene->mTtsPlayer.Speak(SPEECH_TEXT(mScene), 0);
        break;
    }
    case 4: {
        MI_CpuFill8(SPEECH_TEXT(mScene), 0, 0x102);
        wchar_t creatorName[41];
        MI_CpuClear16(creatorName, sizeof(creatorName));
        MiiData_GetCreatorName(&mScene->mCharacter, creatorName);
        func_020372bc(creatorName, sizeof(creatorName));
        if (!mScene->func_ov076_0214bf20(creatorName)) {
            func_ov076_02157508(step + 1);
            return;
        }
        mText.SetStringArg(creatorName, 0);
        mText.mMessage = 0x33;
        mText.ExpandMessage();
        STD_ConvertStringUnicodeToSjis(SPEECH_TEXT(mScene), &length, (const u16 *)mText.mText, NULL, NULL);
        mScene->mTtsPlayer.Speak(SPEECH_TEXT(mScene), 0);
        break;
    }
    case 5: {
        MI_CpuFill8(SPEECH_TEXT(mScene), 0, 0x102);
        u32 value = mScene->mCharacter.mUnknown050Bits29;
        if (value == 4) {
            func_ov076_02157508(step + 1);
            return;
        }
        mText.SetMessageArg(mScene->mMessageFiles[0].GetText(value + 0x50), 0);
        mText.mMessage = 0x34;
        mText.ExpandMessage();
        STD_ConvertStringUnicodeToSjis(SPEECH_TEXT(mScene), &length, (const u16 *)mText.mText, NULL, NULL);
        mScene->mTtsPlayer.Speak(SPEECH_TEXT(mScene), 0);
        break;
    }
    case 6:
        MI_CpuFill8(SPEECH_TEXT(mScene), 0, 0x102);
        mText.SetMessageArg(mScene->mMessageFiles[0].GetText(func_02098dac(&mScene->mCharacter, 0x28) + 0x44), 0);
        mText.mMessage = 0x35;
        mText.ExpandMessage();
        STD_ConvertStringUnicodeToSjis(SPEECH_TEXT(mScene), &length, (const u16 *)mText.mText, NULL, NULL);
        mScene->mTtsPlayer.Speak(SPEECH_TEXT(mScene), 0);
        break;
    }
    mSpeakStep = step;
}

void MiiEditVoicePage::func_ov076_02157b94() {
    func_0209d898(&mScene->mCharacter, &mScene->mTtsPlayer);
    func_ov076_02157508(1);
}

void MiiEditVoicePage::func_ov076_02157bc0() {
    func_ov076_02157508(0);
}

// The etc message of the star sign of a birthday (0x38 for the sign that begins in January, Aquarius): its table,
// data_ov076_02158d34, holds the first day of each sign in its month, from January's (21, 19, 21, 21, 22, ...)
int MiiEditVoicePage::GetStarSignMessage(int month, int day) {
    int sign;
    if (day >= data_ov076_02158d34[month - 1]) {
        sign = month - 1;
        if (sign < 0) {
            sign += 12;
        }
    } else {
        sign = month - 2;
        if (sign < 0) {
            sign += 12;
        }
    }
    return sign + 0x38;
}

// --- The personality page ---

// What this page uses of the main program (stand-ins)
struct TouchInput {
    /* 0x00 */ u8 mUnknown00[0x2C];
    /* 0x2C */ u16 mTouching;
    /* 0x2E */ u8 mUnknown2E[2];
    /* 0x30 */ ScreenPoint mLastPoint; // Where it was touched last
    /* 0x38 */ ScreenPoint mPoint;     // Where it is touched
    /* 0x40 */ u8 mUnknown40[8];
    /* 0x48 */ BOOL mTouched;          // The stylus went down this frame
    /* 0x4C */ BOOL mUnknown4C;        // Which of the two points to test
};

extern "C" {
extern TouchInput gTouchInput;
void func_02098d5c(SavedCharacter *character, int index, u8 value); // Sets mUnknown04A[index]
void func_02091a6c(SceneCharacter *character);
}

void MiiEditCharacterPage::func_ov076_02157c04(MiiEditScene *scene) {
    mScene = scene;
    scene->func_ov076_0214bbe4();
    ObjResourceManager_Load(&gObjResourceManager, "E00:/Character.nce", "E00:/Character.ncg", 0x1000, 1, 0x10000);
    for (u16 i = 0; i < 5; i++) {
        u16 sequence = ((const u16 *)&data_ov076_0215882c)[i];
        Sprite_Init(&mButtons[i], 1, sequence, 0x80, 0x60, TRUE);
        func_02031eb0(&mButtons[i], data_ov076_02158c50[i].x, data_ov076_02158c50[i].y);
        func_02031278(&mButtons[i], sequence, sequence + 1);
        func_02031260(&mButtons[i], data_ov076_02158970[i]);
        mButtons[i].SetPositionFromVec(data_ov076_02158c28[i].x << FX32_SHIFT,
                                       data_ov076_02158c28[i].y << FX32_SHIFT);
        func_0203119c(&mButtons[i]);
        mButtons[i].mVisible = FALSE;
    }
    for (int i = 0; i < 5; i++) {
        u8 value = mScene->mCharacter.mUnknown04A[i];
        if (value == 0) {
            mButtons[i].mVisible = FALSE;
        } else {
            u8 index = value - 1;
            mButtons[i].mVisible = TRUE;
            mButtons[i].mPosition.x = (0x87 - (4 - index) * 14) << FX32_SHIFT;
        }
    }
    for (u16 i = 0; i < 5; i++) {
        Sprite_Init(&mSprites[i], 1, data_ov076_02158822[i], 0x80, 0x60, TRUE);
        mSprites[i].SetPositionFromVec(data_ov076_02158bb0[i].x << FX32_SHIFT,
                                       data_ov076_02158bb0[i].y << FX32_SHIFT);
    }
    mScene->MoveCameraTo(data_ov076_0215885c);
}

// Whether all five values are set
static inline BOOL IsPersonalitySet(MiiEditScene *scene) {
    BOOL set = TRUE;
    for (int i = 0; i < 5; i++) {
        if (scene->mCharacter.mUnknown04A[i] == 0) {
            set = FALSE;
            break;
        }
    }
    return set;
}

void MiiEditCharacterPage::func_ov076_02157e3c() {
    if (gTouchInput.mTouching == 0) {
        return;
    }
    ScreenPoint point;
    int i;
    int j;
    int y;
    point = *(gTouchInput.mUnknown4C ? &gTouchInput.mLastPoint : &gTouchInput.mPoint);
    for (i = 0; i < 5; i++) {
        y = data_ov076_02158c28[i].y;
        for (j = 0; j < 8; j++) {
            ScreenRect rect = data_ov076_021592d0[j];
            rect.y += y;
            if (func_02028d88(&rect, &point) && j + 1 != mScene->mCharacter.mUnknown04A[i]) {
                func_02098d5c(&mScene->mCharacter, i, j + 1);
                func_02091a6c(&mScene->mCharacter);
                mButtons[i].mVisible = TRUE;
                mButtons[i].mPosition.x = (0x87 - (4 - j) * 14) << FX32_SHIFT;
                gSoundManager.StartSeqArc(1, 14, 1);
            }
        }
    }
    if (mScene->mButtons[4].mUnknownB8 == 0) {
        return;
    }
    BOOL set = TRUE;
    for (int i = 0; i < 5; i++) {
        if (mScene->mCharacter.mUnknown04A[i] == 0) {
            set = FALSE;
            break;
        }
    }
    if (!set) {
        return;
    }
    func_02031e88(&mScene->mButtons[4], FALSE);
    mScene->mUnknown58DC = TRUE;
    mScene->mSprite582C.func_ov076_0214bec8(4);
}


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

// What these pages use of the main program (stand-ins)
extern "C" {
void func_02033844(MessageWindow *window, int number, int a, u8 sequence, u8 pressedSequence, WindowPoint position,
                   WindowPoint size, MessageFile *file, u32 message, const NNSG2dFont *font, int plane,
                   int *charOffset, const ButtonSounds *sounds, int b);
}

// The two pages are windows with two buttons, common messages 2 and 3 ("はい", "いいえ")
void MiiEditPage6::func_ov076_02158048(MiiEditScene *scene, int previousPage) {
    mScene = scene;
    mPreviousPage = previousPage;
    ScreenRect rect = data_ov076_021588dc;
    int charOffset = 0x384;
    func_02032fb8(&mWindow, rect);
    func_02033174(&mWindow, "E00:/Window_down.nsc", 0x100);
    func_020331bc(&mWindow, 0x3F00, rect);
    func_02034f4c(&mWindow, 1);
    int number = 0;
    for (int i = 0; i <= 1; i++) {
        func_02033844(&mWindow, number, 0, data_ov076_0215874c[i], data_ov076_02158748[i],
                      data_ov076_021588fc[i], data_ov076_0215894c[i], &mScene->mMessageFiles[1],
                      data_ov076_021587f0[i], &gFonts[2].mFont, 0x1000, &charOffset, &data_ov076_02158a3c[i], 1);
        number++;
    }
    charOffset = func_02033a58(&mWindow, &mScene->mMessageFiles[0], 0x2D, &gFonts[0].mFont, 0x1000,
                               data_ov076_02158800, &charOffset);
    func_02034f4c(&mWindow, 1);
    func_02034f94(&mWindow, 1);
    func_02034720(&mWindow, MessageWindow::FLAG_OPEN | MessageWindow::FLAG_FROM_BELOW, 1);
}

void MiiEditPage6::func_ov076_0215821c() {
    func_020341d4(&mWindow);
    if (mWindow.mResult != -1) {
        mWindow.mUnknownSetBeforeClose = 15;
        func_020349cc(&mWindow, MessageWindow::FLAG_CLOSE | 0x20);
        switch (mWindow.mResult) {
        case 0:
            mNextPage = -1;
            break;
        case 1:
            mNextPage = mPreviousPage;
            break;
        }
    }
    if (!mWindow.mTransitionDone) {
        return;
    }
    if (mNextPage == -1) {
        mScene->func_ov076_0214b890();
        switch (mScene->mSceneId) {
        case 0x42:
            func_0203c02c(&gSceneManager, 0x1A);
            break;
        case 0x43:
        case 0x44:
            data_0213ba74.mCharacters[0] = mScene->mUnknown5824;
            func_0203c02c(&gSceneManager, 0x74);
            break;
        case 0x6F:
            data_0213ba74.mCharacters[0] = mScene->mUnknown5824;
            func_0203c02c(&gSceneManager, 0x69);
            break;
        }
    } else {
        mScene->func_ov076_0214b1ec(mNextPage);
    }
}

void MiiEditPage6::Draw() {
    func_020345d8(&mWindow);
}

void MiiEditPage6::Exit() {
    func_020346cc(&mWindow);
}

void MiiEditPage5::func_ov076_0215835c(MiiEditScene *scene, int previousPage) {
    mScene = scene;
    mPreviousPage = previousPage;
    ScreenRect rect = data_ov076_021588dc;
    int charOffset = 0x384;
    func_02032fb8(&mWindow, rect);
    func_02033174(&mWindow, "E00:/Window_down.nsc", 0x100);
    func_020331bc(&mWindow, 0x3F00, rect);
    func_02034f4c(&mWindow, 1);
    int number = 0;
    for (int i = 0; i <= 1; i++) {
        func_02033844(&mWindow, number, 0, data_ov076_0215874c[i], data_ov076_02158748[i],
                      data_ov076_021588fc[i], data_ov076_0215894c[i], &mScene->mMessageFiles[1],
                      data_ov076_021587f0[i], &gFonts[2].mFont, 0x1000, &charOffset, &data_ov076_02158a1c[i], 1);
        number++;
    }
    func_02033a58(&mWindow, &mScene->mMessageFiles[0], 0x2E, &gFonts[0].mFont, 0x1000, data_ov076_02158800,
                  &charOffset);
    func_02034f4c(&mWindow, 1);
    func_02034f94(&mWindow, 1);
    func_02034720(&mWindow, MessageWindow::FLAG_OPEN | MessageWindow::FLAG_FROM_BELOW, 1);
}

void MiiEditPage5::func_ov076_0215852c() {
    func_020341d4(&mWindow);
    if (mWindow.mResult != -1) {
        mWindow.mUnknownSetBeforeClose = 15;
        func_020349cc(&mWindow, MessageWindow::FLAG_CLOSE | 0x20);
        switch (mWindow.mResult) {
        case 0:
            mNextPage = -1;
            break;
        case 1:
            mNextPage = mPreviousPage;
            break;
        }
    }
    if (!mWindow.mTransitionDone) {
        return;
    }
    if (mNextPage == -1) {
        switch (mScene->mSceneId) {
        case 0x42:
        case 0x44:
            func_0203c02c(&gSceneManager, 0x1A);
            break;
        case 0x43:
            func_0203c02c(&gSceneManager, 0x5E);
            break;
        }
    } else {
        mScene->func_ov076_0214b1ec(mNextPage);
    }
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

#include "MiiEditScene.h"
#include "text/TextUtil.h"
#include "MainProgram.h"
#include "sound/SoundManager.h"

#include <wchar.h>

#include "GameData.h"
#include "sound/SoundManager.h"

#include <wchar.h>

// --- BEGIN of the file's .rodata (in an order that the compiler sorts into the ROM's) ---
static const u8 data_ov076_02158748[2] = { 0x6a, 0x6a };
static const u8 data_ov076_0215874c[2] = { 0x69, 0x69 };
static const u8 data_ov076_02158750[4] = { 0x20, 0x22, 0x24, 0x26 };
static const int data_ov076_02158754[1] = { 76 };
static const int data_ov076_02158770[1] = { 2 };
static const int data_ov076_0215878c[1] = { 107 };
static const int data_ov076_0215875c[1] = { 4 };
static const int data_ov076_02158780[1] = { 0 };
static const int data_ov076_02158758[1] = { 32 };
static const int data_ov076_02158760[1] = { 4 };
static const int data_ov076_0215876c[1] = { 28 };
static const u16 data_ov076_02158788[2] = { 0x10, 0x6b };
static const u16 data_ov076_02158774[2] = { 0x4d, 0x4e };
static const int data_ov076_02158768[1] = { 2 };
static const u16 data_ov076_02158764[2] = { 0x30, 0x31 };
static const u16 data_ov076_02158784[2] = { 0x2, 0x4 };
static const int data_ov076_02158778[1] = { 6 };
static const int data_ov076_0215877c[1] = { 2 };
static const WindowPoint data_ov076_021587b0 = { 16, 7 };
static const WindowPoint data_ov076_021587d8 = { 36, 40 };
static const WindowPoint data_ov076_021587a8 = { 75, 149 };
static const WindowPoint data_ov076_021587d0 = { 128, 76 };
static const WindowPoint data_ov076_021587c8 = { 32, 24 };
static const WindowPoint data_ov076_02158810 = { 30, 20 };
static const WindowPoint data_ov076_02158798 = { 42, 36 };
static const u32 data_ov076_021587f0[2] = { 2, 3 };
static const WindowPoint data_ov076_02158790 = { 181, 149 };
static const WindowPoint data_ov076_02158800 = { 38, 33 };
static const WindowPoint data_ov076_021587b8 = { 128, 76 };
static const WindowPoint data_ov076_02158808 = { 94, 30 };
static const WindowPoint sCharBoxPosition = { 65, 38 };
static const WindowPoint data_ov076_021587a0 = { 75, 149 };
static const WindowPoint data_ov076_021587c0 = { 94, 30 };
static const WindowPoint data_ov076_021587e8 = { 8, 7 };
static const WindowPoint data_ov076_021587e0 = { 36, 40 };
static const u16 data_ov076_02158818[5] = { 0x26, 0x26, 0x26, 0x26, 0x26 };
static const u16 data_ov076_02158822[5] = { 0x1, 0x2, 0x3, 0x4, 0x5 };
static const int data_ov076_021588b0[3] = { 4, 4, 4 };
static const int data_ov076_02158844[3] = { 1, 0, 0 };
static const VecFx32 data_ov076_02158850 = { 0, 7844, 58762 };
static const VecFx32 sProfileCameraGoal = { 0, 7248, 105456 };
static const VecFx32 data_ov076_0215882c = { 0, 0, 0 };
static const u32 data_ov076_02158868[3] = { 41, 5, 6 };
static const int data_ov076_02158874[3] = { 0, 2, 2 };
static const u16 sButtonSequences[6] = { 0x58, 0x5a, 0x5c, 0x5e, 0x28, 0x2a };
static const VecFx32 data_ov076_02158838 = { 0, 0, -32768 };
static const int data_ov076_0215888c[3] = { 0, 1, 1 };
static const VecFx32 sLightDirection1 = { 0, -4000, 1500 };
static const VecFx32 sLightDirection0 = { 1600, -600, -4000 };
static const WindowPoint data_ov076_021588cc[2] = { { 128, 96 }, { 128, 96 } };
static const ScreenRect sWindowRect = { 20, 20, 216, 152 };
static const ButtonSounds data_ov076_021588ec = { { 16, -1, 0, -1 } };
static const WindowPoint data_ov076_021588fc[2] = { { 75, 149 }, { 181, 149 } };
static const WindowPoint data_ov076_021588bc[2] = { { 202, 112 }, { 52, 8 } };
static const WindowPoint data_ov076_0215890c[2] = { { 145, 65 }, { 145, 119 } };
static const ButtonSounds data_ov076_0215891c = { { 5, -1, 19, -1 } };
static const WindowPoint sMolePositions[2] = { { 80, 77 }, { 123, 77 } };
static const WindowPoint data_ov076_0215894c[2] = { { 94, 30 }, { 94, 30 } };
static const int sAdjustButtons[4] = { 14, 15, 16, 17 };
static const u16 sProfileButtonSequences[10] = { 0x0, 0x2, 0x4, 0x6, 0xa, 0xc, 0x10, 0x14, 0x16, 0xe };
static const int data_ov076_02158970[5] = { 4, 4, 4, 4, 4 };
static const int data_ov076_0215899c[6] = { 14, 14, 14, 14, 14, 1 };
static const int sButtonKinds[6] = { 3, 3, 3, 3, 4, 4 };
static const WindowPoint data_ov076_021589cc[3] = { { 136, 132 }, { 62, 175 }, { 194, 175 } };
static const int sMoleAdjustButtons[6] = { 14, 15, 16, 17, 22, 23 };
static const WindowPoint data_ov076_02158984[3] = { { 62, 22 }, { 116, 30 }, { 116, 30 } };
static const WindowPoint sAdjustButtonPositions[4] = { { 207, 48 }, { 239, 48 }, { 207, 74 }, { 239, 74 } };
static const ButtonSounds data_ov076_02158a1c[2] = { { 14, -1, 0, -1 }, { 16, -1, 0, -1 } };
static const ButtonSounds data_ov076_02158a3c[2] = { { 15, -1, 0, -1 }, { 16, -1, 0, -1 } };
static const WindowPoint data_ov076_02158a5c[4] = { { 80, 90 }, { 176, 90 }, { 80, 90 }, { 176, 90 } };
static const WindowPoint data_ov076_02158a7c[4] = { { 80, 102 }, { 176, 102 }, { 80, 90 }, { 176, 90 } };
static const WindowPoint data_ov076_02158a9c[4] = { { 58, 54 }, { 58, 54 }, { 58, 54 }, { 58, 54 } };
static const WindowPoint data_ov076_02158abc[4] = { { 158, 76 }, { 138, 76 }, { 118, 76 }, { 98, 76 } };
static const WindowPoint data_ov076_02158adc[4] = { { 170, 76 }, { 150, 76 }, { 90, 76 }, { 70, 76 } };
static const WindowPoint sMustachePositions[4] = { { 80, 77 }, { 123, 77 }, { 80, 114 }, { 123, 114 } };
static const int data_ov076_02158b1c[8] = { 14, 15, 16, 17, 18, 19, 20, 21 };
static const int data_ov076_02158b3c[9] = { 0, 1, 4, 2, 3, 6, 7, 8, 9 };
static const int sProfileButtonKinds[10] = { 4, 4, 4, 4, 4, 4, 4, 4, 4, 4 };
static const ScreenPoint sSpriteB0Places[5] = { { 55, 182 }, { 55, 182 }, { 103, 182 }, { 151, 182 }, { 199, 182 } };
static const WindowPoint data_ov076_02158bb0[5] = { { 128, 26 }, { 128, 59 }, { 128, 92 }, { 128, 125 }, { 128, 158 } };
static const WindowPoint data_ov076_02158bd8[5] = { { 128, 112 }, { 128, 88 }, { 128, 64 }, { 128, 16 }, { 128, 40 } };
static const WindowPoint data_ov076_02158c00[5] = { { 8, 20 }, { 8, 20 }, { 8, 20 }, { 8, 20 }, { 8, 20 } };
static const WindowPoint data_ov076_02158c28[5] = { { 128, 26 }, { 128, 59 }, { 128, 92 }, { 128, 125 }, { 128, 158 } };
static const WindowPoint data_ov076_02158c50[5] = { { 12, 12 }, { 12, 12 }, { 12, 12 }, { 12, 12 }, { 12, 12 } };
static const u16 data_ov076_02158c78[22] = { 0x0, 0x2, 0x4, 0x6, 0x8, 0xa, 0xc, 0xe, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x20, 0x22, 0x24, 0x28, 0x2a, 0x2c, 0x2e };
static const WindowPoint sButtonSizes[6] = { { 40, 18 }, { 40, 18 }, { 40, 18 }, { 40, 18 }, { 40, 18 }, { 24, 22 } };
static const ButtonSounds data_ov076_02158cd4[3] = { { 20, -1, 0, -1 }, { 22, -1, 0, -1 }, { 20, -1, 0, -1 } };
static const int sNoseTypes[12] = { 1, 10, 2, 3, 6, 0, 5, 4, 8, 9, 7, 11 };
static const int sStarSignFirstDays[12] = { 21, 19, 21, 21, 22, 22, 23, 23, 24, 24, 23, 22 };
static const WindowPoint sMoleAdjustButtonPositions[6] = { { 207, 48 }, { 239, 48 }, { 207, 74 }, { 239, 74 }, { 207, 100 }, { 239, 100 } };
static const ButtonSounds data_ov076_02158d94[3] = { { { 20, -1, 0, -1 } }, { { 21, -1, 0, -1 } }, { { 22, -1, 0, -1 } } };
static const WindowPoint sButtonPositions[6] = { { 32, 181 }, { 80, 181 }, { 128, 181 }, { 176, 181 }, { 224, 181 }, { 242, 13 } };
static const u16 sChoiceSequences[30] = { 0x18, 0x1a, 0x1c, 0x1e, 0x24, 0x26, 0x28, 0x2a, 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x20, 0x22, 0x3c, 0x3e, 0x3c, 0x3e, 0x40, 0x42, 0x4f, 0x4f, 0x4f, 0x4f, 0x4f, 0x0 };
static const char sWindowScreenName[] = "E00:/Window_down.nsc";
static const u8 sEyebrowRotations[24] = { 0x1a, 0x1a, 0x1b, 0x19, 0x1a, 0x19, 0x1a, 0x19, 0x1c, 0x19, 0x1a, 0x18, 0x1b, 0x1b, 0x1a, 0x1a, 0x19, 0x19, 0x1a, 0x1a, 0x1b, 0x1a, 0x19, 0x1b };
static const u8 sEyeRotations[50] = { 0x1d, 0x1c, 0x1c, 0x1c, 0x1d, 0x1c, 0x1c, 0x1c, 0x1d, 0x1c, 0x1c, 0x1c, 0x1c, 0x1d, 0x1d, 0x1c, 0x1c, 0x1c, 0x1d, 0x1d, 0x1c, 0x1d, 0x1c, 0x1d, 0x1d, 0x1c, 0x1d, 0x1c, 0x1c, 0x1d, 0x1c, 0x1c, 0x1c, 0x1d, 0x1d, 0x1d, 0x1c, 0x1c, 0x1d, 0x1d, 0x1d, 0x1c, 0x1c, 0x1d, 0x1d, 0x1d, 0x1d, 0x1d, 0x1c, 0x1c };
static const WindowPoint data_ov076_02158f14[8] = { { 207, 48 }, { 239, 48 }, { 207, 74 }, { 239, 74 }, { 207, 100 }, { 239, 100 }, { 207, 126 }, { 239, 126 } };
static const ButtonSounds data_ov076_02158e94[4] = { { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } } };
static const WindowPoint sFaceTypePositions[8] = { { 80, 40 }, { 123, 40 }, { 80, 77 }, { 123, 77 }, { 80, 114 }, { 123, 114 }, { 80, 151 }, { 123, 151 } };
static const u16 data_ov076_02158f54[36] = { 0x2c, 0x2e, 0x30, 0x32, 0x34, 0x36, 0x38, 0x3a, 0x3c, 0x3e, 0x40, 0x42, 0x44, 0x46, 0x6, 0x8, 0xa, 0xc, 0xe, 0x10, 0x12, 0x14, 0x16, 0x18, 0x1a, 0x1c, 0x1e, 0x0, 0x48, 0x4a, 0x4c, 0x4e, 0x50, 0x52, 0x54, 0x56 };
static const WindowPoint sGlassesPositions[9] = { { 59, 77 }, { 102, 77 }, { 145, 77 }, { 59, 114 }, { 102, 114 }, { 145, 114 }, { 59, 151 }, { 102, 151 }, { 145, 151 } };
static const WindowPoint sProfileButtonPositions[10] = { { 70, 40 }, { 186, 40 }, { 128, 112 }, { 160, 76 }, { 68, 76 }, { 202, 112 }, { 216, 76 }, { 68, 149 }, { 192, 149 }, { 54, 112 } };
static const WindowPoint sProfileButtonSizes[10] = { { 116, 18 }, { 116, 18 }, { 32, 18 }, { 56, 18 }, { 112, 18 }, { 32, 18 }, { 56, 18 }, { 112, 18 }, { 48, 18 }, { 48, 18 } };
static const int data_ov076_02159084[22] = { 5, 5, 5, 5, 5, 5, 5, 5, 4, 4, 4, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 3 };
static const ButtonSounds sButtonSounds[6] = { { { 2, -1, -1, -1 } }, { { 2, -1, -1, -1 } }, { { 2, -1, -1, -1 } }, { { 2, -1, -1, -1 } }, { { 20, -1, 0, -1 } }, { { 16, -1, 0, -1 } } };
static const int sEyebrowTypes[24] = { 6, 0, 12, 1, 9, 19, 7, 21, 8, 17, 5, 4, 11, 10, 2, 3, 14, 20, 15, 13, 22, 18, 16, 23 };
static const int sMouthTypes[24] = { 23, 1, 19, 21, 22, 5, 0, 8, 10, 16, 6, 13, 7, 9, 2, 17, 3, 4, 15, 11, 20, 18, 14, 12 };
static const WindowPoint sFaceTexturePositions[12] = { { 59, 40 }, { 102, 40 }, { 145, 40 }, { 59, 77 }, { 102, 77 }, { 145, 77 }, { 59, 114 }, { 102, 114 }, { 145, 114 }, { 59, 151 }, { 102, 151 }, { 145, 151 } };
static const int sChoiceKinds[29] = { 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 0, 0, 0, 0, 0, 0, 5, 5, 5, 5, 5 };
static const ScreenRect data_ov076_021592d0[8] = { { 73, -6, 12, 12 }, { 87, -6, 12, 12 }, { 101, -6, 12, 12 }, { 115, -6, 12, 12 }, { 129, -6, 12, 12 }, { 143, -6, 12, 12 }, { 157, -6, 12, 12 }, { 171, -6, 12, 12 } };
static const int data_ov076_02159350[36] = { 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 3, 3, 3, 3, 3, 3, 3, 3 };
static const ButtonSounds sProfileButtonSounds[10] = { { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } } };
static const WindowPoint data_ov076_02159530[22] = { { 22, 22 }, { 22, 22 }, { 22, 22 }, { 22, 22 }, { 22, 22 }, { 22, 22 }, { 22, 22 }, { 22, 22 }, { 30, 24 }, { 30, 24 }, { 30, 24 }, { 30, 24 }, { 30, 24 }, { 30, 24 }, { 30, 24 }, { 30, 24 }, { 30, 24 }, { 30, 24 }, { 16, 16 }, { 16, 16 }, { 16, 16 }, { 16, 16 } };
static const WindowPoint data_ov076_02159480[22] = { { 68, 52 }, { 92, 52 }, { 116, 52 }, { 140, 52 }, { 164, 52 }, { 188, 52 }, { 68, 92 }, { 68, 132 }, { 206, 134 }, { 237, 134 }, { 237, 134 }, { 206, 20 }, { 237, 20 }, { 206, 45 }, { 237, 45 }, { 206, 70 }, { 237, 70 }, { 206, 95 }, { 99, 138 }, { 118, 138 }, { 137, 138 }, { 156, 138 } };
static const int sEyeTypes[48] = { 2, 4, 0, 8, 39, 17, 1, 26, 16, 15, 27, 20, 33, 11, 19, 32, 9, 12, 23, 34, 21, 25, 40, 35, 5, 41, 13, 36, 37, 6, 24, 30, 31, 18, 28, 46, 7, 44, 38, 42, 45, 29, 3, 43, 22, 10, 14, 47 };
static const WindowPoint sChoicePositions[29] = { { 58, 80 }, { 105, 80 }, { 152, 80 }, { 199, 80 }, { 48, 64 }, { 80, 64 }, { 112, 64 }, { 144, 64 }, { 176, 64 }, { 208, 64 }, { 48, 96 }, { 80, 96 }, { 112, 96 }, { 144, 96 }, { 176, 96 }, { 208, 96 }, { 80, 80 }, { 176, 80 }, { 128, 41 }, { 128, 111 }, { 80, 41 }, { 80, 111 }, { 160, 41 }, { 160, 111 }, { 128, 48 }, { 128, 72 }, { 128, 96 }, { 128, 120 }, { 128, 144 } };
static const WindowPoint sChoiceSizes[29] = { { 42, 36 }, { 42, 36 }, { 42, 36 }, { 42, 36 }, { 24, 24 }, { 24, 24 }, { 24, 24 }, { 24, 24 }, { 24, 24 }, { 24, 24 }, { 24, 24 }, { 24, 24 }, { 24, 24 }, { 24, 24 }, { 24, 24 }, { 24, 24 }, { 58, 54 }, { 58, 54 }, { 20, 15 }, { 20, 15 }, { 20, 15 }, { 20, 15 }, { 20, 15 }, { 20, 15 }, { 126, 20 }, { 126, 20 }, { 126, 20 }, { 126, 20 }, { 126, 20 } };
static const int sHairTypes[72] = { 33, 40, 51, 44, 39, 70, 45, 49, 59, 56, 68, 31, 32, 47, 37, 48, 66, 52, 58, 50, 55, 64, 60, 62, 43, 38, 42, 23, 67, 54, 36, 41, 65, 30, 57, 34, 12, 13, 69, 26, 4, 25, 1, 19, 5, 8, 27, 7, 14, 3, 22, 10, 6, 20, 11, 63, 17, 35, 21, 0, 61, 16, 46, 9, 18, 2, 28, 53, 71, 24, 15, 29 };
static const WindowPoint data_ov076_02159990[36] = { { 17, 159 }, { 17, 12 }, { 17, 33 }, { 17, 54 }, { 17, 75 }, { 17, 96 }, { 17, 117 }, { 17, 138 }, { 77, 11 }, { 127, 11 }, { 77, 11 }, { 127, 11 }, { 77, 30 }, { 127, 30 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 }, { 223, 74 }, { 68, 11 }, { 136, 11 }, { 223, 155 }, { 179, 30 }, { 179, 48 }, { 179, 66 }, { 179, 84 }, { 179, 102 }, { 179, 120 }, { 179, 138 }, { 179, 156 } };
static const ButtonSounds data_ov076_02159ab0[22] = { { 2, -1, -1, -1 }, { 2, -1, -1, -1 }, { 2, -1, -1, -1 }, { 2, -1, -1, -1 }, { 2, -1, -1, -1 }, { 2, -1, -1, -1 }, { 2, -1, -1, -1 }, { 2, -1, -1, -1 }, { 14, -1, 0, -1 }, { 1, -1, 0, -1 }, { 0, -1, -1, -1 }, { 0, -1, -1, -1 }, { 0, -1, -1, -1 }, { 0, -1, -1, -1 }, { 0, -1, -1, -1 }, { 0, -1, -1, -1 }, { 0, -1, -1, -1 }, { 0, -1, -1, -1 }, { 2, -1, -1, -1 }, { 2, -1, -1, -1 }, { 2, -1, -1, -1 }, { 2, -1, -1, -1 } };
static const ButtonSounds sChoiceSounds[29] = { { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 17, -1, -1, -1 } }, { { 18, -1, -1, -1 } }, { { 17, -1, -1, -1 } }, { { 18, -1, -1, -1 } }, { { 17, -1, -1, -1 } }, { { 18, -1, -1, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } }, { { 20, -1, 0, -1 } } };
static const ButtonSounds data_ov076_02159de0[36] = { { { 3, -1, -1, -1 } }, { { 3, -1, -1, -1 } }, { { 3, -1, -1, -1 } }, { { 3, -1, -1, -1 } }, { { 3, -1, -1, -1 } }, { { 3, -1, -1, -1 } }, { { 3, -1, -1, -1 } }, { { 3, -1, -1, -1 } }, { { 2, -1, -1, -1 } }, { { 2, -1, -1, -1 } }, { { 2, -1, -1, -1 } }, { { 2, -1, -1, -1 } }, { { 2, -1, -1, -1 } }, { { 2, -1, -1, -1 } }, { { -1, -1, -1, -1 } }, { { -1, -1, -1, -1 } }, { { -1, -1, -1, -1 } }, { { -1, -1, -1, -1 } }, { { -1, -1, -1, -1 } }, { { -1, -1, -1, -1 } }, { { -1, -1, -1, -1 } }, { { -1, -1, -1, -1 } }, { { -1, -1, -1, -1 } }, { { -1, -1, -1, -1 } }, { { 12, -1, -1, -1 } }, { { 13, -1, -1, -1 } }, { { 13, -1, -1, -1 } }, { { -1, -1, -1, -1 } }, { { 4, -1, 19, -1 } }, { { 4, -1, 19, -1 } }, { { 4, -1, 19, -1 } }, { { 4, -1, 19, -1 } }, { { 4, -1, 19, -1 } }, { { 4, -1, 19, -1 } }, { { 4, -1, 19, -1 } }, { { 4, -1, 19, -1 } } };
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
void func_0207b258(ParticleSystem *system, void *a);
}

void MiiEditScene::StartLoading() {
    ArchiveManager_MountCompressed(&gArchiveManager, "E00", "Scn/Scene/SceneMiiEdit2_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "KBD", "Cmn/Kbd/SoftKeyboard_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "TBM", "Msg/TmdEtcMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "T02", "Msg/TmdCommonMsgJP_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "SPL", "Mdl/Spl/SplEditEffect_LZ.bin", 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "BIC", "Mdl/Ica/ica_mii_edit_LZ.bin", 0);
    mCharacter.CharacterModel::SetArchive("B0I", 0, 0);
    ArchiveManager_MountCompressed(&gArchiveManager, "B0I", mCharacter.mArchivePath, 0);
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
struct SceneManager {
    void RequestScene(int sceneId); // Asks for the next scene
};
struct SceneMemory {
    /* 0x00 */ u8 mUnknown00[0x20];
    /* 0x20 */ u8 mCharacters[12]; // The first is the character the scene edits
};
struct Light;
extern "C" {
extern PadInput data_0211f9d4;
extern SceneManager gSceneManager;
extern SceneMemory gSceneMemory;
extern Light data_021227cc;
extern Light data_021227d8;
BOOL func_02023fa4(ScreenControl *screens, int planes);
void func_0202aa28(Light *light, Camera *camera, VecFx32 direction);
void func_0207b138(ParticleSystem *system);
void func_0202c264(GameData *game, int index, CharacterRecord *record); // Reads a saved character
void func_0202c28c(GameData *game, int index, CharacterRecord *record); // Writes one
BOOL func_0209d900(CharacterRecord *record, SceneCharacter *character);
void func_02031e88(SpriteButton *button, BOOL a);
void func_0207b3dc(ParticleSystem *system);
ParticleEmitter *func_0207b2fc(ParticleSystem *system, int resource, VecFx32 *position);
void *func_0202ad04(void *resources, int resourceId, const char *archiveName, int unk);
void func_0202aecc(void *resources, int resourceId);
void func_02029660(void *resourceSet, u32 resType, void *archive);
void ModelResources_SetModel(void *resources, int resourceId, const char *modelPath);
int MiiData_GetSex(CharacterRecord *character);
void func_0209e01c(CharacterRecord *character);
extern const int data_020fba28[];
void func_02029f10(CharacterModel *model, GXRgb color);
void func_02029f20(CharacterModel *model, GXRgb color);
void func_02029f30(CharacterModel *model, GXRgb color);
void func_02029f40(CharacterModel *model, GXRgb color);
}

struct Renderer3d;
extern "C" {
extern Renderer3d data_02122824;
void func_0202b0e8(Renderer3d *renderer);
void func_0209cbac(CharacterRecord *character, u8 *a, u8 *b);
BOOL func_0202c354(GameData *game, int index); // Whether there is a saved character with that index
extern void *data_02123440[];
void Clock_GetDate(void *clock, RTCDate *date);
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
}

static inline void SetCanvasPosition(ObjTextCanvas &canvas, WindowPoint position) {
    canvas.mSprite.SetPosition(FxVec2(position.x << FX32_SHIFT, position.y << FX32_SHIFT));
}

// Puts a text canvas at a position in pixels
static inline void SetCanvasPosition(ObjTextCanvas *canvas, WindowPoint position) {
    canvas->mSprite.SetPosition(FxVec2(position.x << FX32_SHIFT, position.y << FX32_SHIFT));
}

// 96.4%: in the loops over the buttons and the text boxes the counter (and the text boxes' width and character
// offset) sit one register over from the original's, and the text box's character count is read in another order.
// The canvas through the reference overload and y adjusted before x brought the position code to the original's.
// Tried: one u16 counter for both loops at the top or before them, the offset declared at every place between the
// loops or as a u32, the width at function scope, the position through a pointer or two ints, pragma sets
// (lifetimes, common subs, loop invariants, strength reduction).
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
    ModelResources_SetModel(data_021223e4, 0, mCharacter.mModelPath);
    mCharacter.Character::Init(0, 0, 2);
    mCharacter.CharacterBase::SetFace(8);
    switch (mSceneId) {
    case 0x42:
        mUnknown5824 = gSceneMemory.mCharacters[0];
        LoadCharacter(mUnknown5824);
        break;
    case 0x44: {
        mUnknown5824 = gSceneMemory.mCharacters[0];
        LoadCharacter(mUnknown5824);
        u8 a;
        u8 b;
        func_0209cbac(&mCharacter, &a, &b);
        mCharacter.mFavoriteItem = a;
        mCharacter.mDislikedItem = b;
        break;
    }
    case 0x43:
    case 0x6F: {
        void *animations = func_0202ad04(data_021223e4, 0x1F, "BIC", 0);
        func_02029660((void *)mCharacter.mAnim.mUnknown04, RESOURCE_BCA0, animations);
        mCharacter.Character::SetAnimation(RESOURCE_BCA0, 0);
        mCharacter.Character::SetAngle(0x4000);
        mCharacter.Character::SetVisible(FALSE);
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
        ((SavedCharacter *)(&mCharacter))->Reset();
        func_0209e01c(&mCharacter);
        RTCDate date;
        Clock_GetDate(data_02123440[0], &date);
        ((SavedCharacter *)(&mCharacter))->SetRegisteredDate(date.year, date.month, date.day);
        break;
    }
    }
    ((Character *)(&mCharacter))->UpdateBodyScale();
    mUnknown5828 = FALSE;
    ((Character *)(&mCharacter))->UpdateAnimSpeed();
    ((CharacterBase *)(&mEditedCharacter))->InitFace(0);
    *(MiiData *)mEditedCharacter.mMii = *(MiiData *)mCharacter.mMii;
    mEditedCharacter.CharacterBase::MakeFace();
    VecFx32 v = mCharacter.mFaceOffset;
    mEditedCharacter.mFaceOffset = v;
    mEditedCharacter.mHeadPitchNow = mCharacter.mHeadPitchNow;
    mEditedCharacter.mHeadYawNow = mCharacter.mHeadYawNow;
    mEditedCharacter.mHeadRollNow = mCharacter.mHeadRollNow;
    mEditedCharacter.mFaceRoll = mCharacter.mFaceRoll;
    mEditedCharacter.CharacterBase::SetHeadMatrices(&mCharacter.mHeadMatrix, &mCharacter.mHeadRotation);
    mUnknown580C = TRUE;
    mCamera = &data_02122764;
    func_0202a640(mCamera);
    mCamera->mUnknown00 = 0x11E;
    mCamera->mUnknown04 = 0xFF6;
    VEC_Set(&mCamera->mPosition, 0, 0x1EA4, 0xE58A);
    mCamera->mPosition.y += ((u32)((SavedCharacter *)(&mCharacter))->GetParameter(0x29) * 0x1130 >> 7) - 0x898;
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
    ShowControls();
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
    Sprite_Init(&mProgressSprite, 0, 0x6D, 0x80, 0x60, TRUE);
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
        WindowPoint position = sProfileButtonPositions[data_ov076_02158b3c[i]];
        position.y -= 7;
        position.x += -position.x / 2 + 8;
        mTextBoxes[i].mSprite.mOverwritePalette = 1;
        mTextBoxes[i].mColor = 8;
        SetCanvasPosition(mTextBoxes[i], position);
        mTextBoxes[i].Update(TRUE);
    }
    mCharBox.Init(0x1000, 0x20, 2, charOffset, 0, 0, NULL);
    mCharBox.mColor = 8;
    SetCanvasPosition(&mCharBox, sCharBoxPosition);
    MessageText *text = &mText1B35C[0];
    text->InitObjForMessage(&mMessageFiles[0], 0x20, &gFonts[0].mFont, 0x1000,
                                 charOffset + mCharBox.GetCharCount(), 0, 1, 0, 0, 0xF, NULL);
    SetCanvasPosition(&mText1B35C[0], data_ov076_021587b0);
    text->DrawAll();
    mText1B35C[0].mSprite.mOverwritePalette = 0;
    mText1B35C[0].mHAlign = 1;
    text = &mText1B94C[0];
    text->InitObjForMessage(&mMessageFiles[0], 0x1C, &gFonts[0].mFont, 0x10, 200, 0, 1, 0, 0, 0xF, NULL);
    SetCanvasPosition(&mText1B94C[0], data_ov076_021587e8);
    text->DrawAll();
    mText1B94C[0].mSprite.mOverwritePalette = 2;
    mUnknown5904 = 0;
    for (int i = 0; i < 7; i++) {
        mUnknown58E8[i] = FALSE;
    }
    switch (mSceneId) {
    case 0x42:
        mProgressSprite.SetPlace(4);
        mUnknown58E0 = FALSE;
        SetPage(2);
        if (!((SavedCharacter *)(&mCharacter))->IsMiiFromThisConsole()) {
            func_02031e88(&mButtons[0], TRUE);
        }
        break;
    case 0x44:
        func_02031e88(&mButtons[0], TRUE);
        mProgressSprite.SetPlace(2);
        SetPage(2);
        break;
    case 0x43:
        func_02031e88(&mButtons[2], TRUE);
        func_02031e88(&mButtons[3], TRUE);
        func_02031e88(&mButtons[4], TRUE);
        mProgressSprite.SetPlace(0);
        SetPage(0);
        break;
    case 0x6F:
        mButtons[5].mVisible = FALSE;
        func_02031e88(&mButtons[2], TRUE);
        func_02031e88(&mButtons[3], TRUE);
        func_02031e88(&mButtons[4], TRUE);
        mProgressSprite.SetPlace(0);
        SetPage(0);
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
        mCharacter.Character::Turn(0xFE00);
        if (mCharacter.mAngle == 0x4000) {
            mUnknown5828 = FALSE;
        }
    }
    ((CharacterBase *)(&mCharacter))->LipSync(mTtsPlayer.GetPhonemeName(), 1);
    ((Character *)(&mCharacter))->Update();
    func_0207b138(mParticles);
    UpdateCamera();
    func_0202aa28(&data_021227cc, mCamera, sLightDirection0);
    func_0202aa28(&data_021227d8, mCamera, sLightDirection1);
    mUnknown5904 += 0x4CC;
    func_02028108(&data_02121834, 4, mUnknown5904 >> FX32_SHIFT);
    switch (mPage) {
    case 0:
        mSexPage.Update();
        break;
    case 1:
        mFacePage.Update();
        break;
    case 2:
        mProfilePage.Update();
        break;
    case 3:
        mVoicePage.Update();
        break;
    case 4:
        mCharacterPage.Update();
        break;
    case 6:
        mRegisterPage.Update();
        break;
    case 5:
        mQuitPage.Update();
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
            SetPage(1);
            break;
        case 1:
            SetPage(2);
            break;
        case 2:
            SetPage(3);
            break;
        case 3:
            SetPage(4);
            break;
        case 4:
            SetPage(6);
            break;
        case 5:
            if (mSceneId == 0x42) {
                CharacterRecord record;
                func_0202c264(&gGameData, (u16)mUnknown5824, &record);
                if (!func_0209d900(&record, &mCharacter)) {
                    gSceneManager.RequestScene(0x1A);
                } else {
                    SetPage(5);
                }
            } else {
                SetPage(5);
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
        Sprite_Draw(&mProgressSprite);
    }
    mText1B94C[0].ObjTextCanvas::Draw();
    mCharacter.Character::Draw();
    if (IsFaceClear(&mCharacter)) {
        if (mUnknown580C == TRUE) {
            *(MiiData *)mEditedCharacter.mMii = *(MiiData *)mCharacter.mMii;
            mEditedCharacter.CharacterBase::MakeFace();
            VecFx32 v = mCharacter.mFaceOffset;
            mEditedCharacter.mFaceOffset = v;
            mEditedCharacter.mHeadPitchNow = mCharacter.mHeadPitchNow;
            mEditedCharacter.mHeadYawNow = mCharacter.mHeadYawNow;
            mEditedCharacter.mHeadRollNow = mCharacter.mHeadRollNow;
            mEditedCharacter.CharacterBase::SetHeadMatrices(&mCharacter.mHeadMatrix, &mCharacter.mHeadRotation);
            mUnknown580C = FALSE;
        }
    } else {
        mEditedCharacter.CharacterBase::SetHeadMatrices(&mCharacter.mHeadMatrix, &mCharacter.mHeadRotation);
        mEditedCharacter.CharacterBase::Draw();
    }
    func_0202ad84(data_021223e4);
    NNS_G3dGlbFlushP();
    func_0207b258(mParticles, &NNS_G3dGlb.cameraMtx);
    for (u16 i = 0; i < 6; i++) {
        func_02031d7c(&mButtons[i]);
    }
    switch (mPage) {
    case 0:
        mSexPage.Draw();
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
        mRegisterPage.Draw();
        break;
    case 5:
        mQuitPage.Draw();
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
        mSexPage.Exit();
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
        mRegisterPage.Exit();
        break;
    case 5:
        mQuitPage.Exit();
        break;
    }
}

void MiiEditScene::SetPage(int page) {
    switch (mPage) {
    case 0:
        mSexPage.Exit();
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
        mRegisterPage.Exit();
        break;
    case 5:
        mQuitPage.Exit();
        break;
    }
    mUnknown58E8[page] = TRUE;
    switch (page) {
    case 0:
        NNS_G2dCharCanvasClear(&mText1B94C[0].mCanvas, 0);
        mText1B94C[0].mMessage = 0x1A;
        mText1B94C[0].DrawAll();
        mText1B94C[0].mSprite.mVisible = FALSE;
        mSexPage.Init(this);
        break;
    case 1:
        NNS_G2dCharCanvasClear(&mText1B94C[0].mCanvas, 0);
        mText1B94C[0].mMessage = 0x1C;
        mText1B94C[0].DrawAll();
        mText1B94C[0].mSprite.mVisible = TRUE;
        SetCharacterColors(FALSE);
        mFacePage.Init(this);
        func_02031e28(&mButtons[page - 1], TRUE);
        Sprite_SetFrame(&mButtons[page - 1], 1);
        break;
    case 2:
        if (mProgressSprite.mPlace == 1) {
            mUnknown58DC = FALSE;
        }
        NNS_G2dCharCanvasClear(&mText1B94C[0].mCanvas, 0);
        mText1B94C[0].mMessage = 0x1D;
        mText1B94C[0].DrawAll();
        mText1B94C[0].mSprite.mVisible = TRUE;
        mProfilePage.Init(this);
        func_02031e28(&mButtons[page - 1], TRUE);
        Sprite_SetFrame(&mButtons[page - 1], 1);
        break;
    case 3:
        if (mProgressSprite.mPlace == 2) {
            mUnknown58DC = FALSE;
        }
        NNS_G2dCharCanvasClear(&mText1B94C[0].mCanvas, 0);
        mText1B94C[0].mMessage = 0x21;
        mText1B94C[0].DrawAll();
        mText1B94C[0].mSprite.mVisible = TRUE;
        mVoicePage.Init(this);
        func_02031e28(&mButtons[page - 1], TRUE);
        Sprite_SetFrame(&mButtons[page - 1], 1);
        break;
    case 4:
        if (mProgressSprite.mPlace == 3) {
            mUnknown58DC = FALSE;
        }
        NNS_G2dCharCanvasClear(&mText1B94C[0].mCanvas, 0);
        mText1B94C[0].mMessage = 0x22;
        mText1B94C[0].DrawAll();
        mText1B94C[0].mSprite.mVisible = TRUE;
        mCharacterPage.Init(this);
        func_02031e28(&mButtons[page - 1], TRUE);
        Sprite_SetFrame(&mButtons[page - 1], 1);
        break;
    case 6:
        mRegisterPage.Init(this, mPage);
        break;
    case 5:
        mQuitPage.Init(this, mPage);
        break;
    }
    mPage = page;
    mUnknown00154 = 0;
}

void MiiEditScene::PlayEffectAtHead(int resource, fx32 offsetY) {
    func_0207b3dc(mParticles);
    MtxFx43 matrix;
    MtxFx33 rotation;
    mCharacter.GetJointMatrices(0, &matrix, &rotation);
    mUnknown5914.x = matrix.m[3][0];
    mUnknown5914.y = matrix.m[3][1];
    mUnknown5914.z = matrix.m[3][2];
    VecFx32 position = mUnknown5914;
    position.y += offsetY;
    mUnknown590C = func_0207b2fc(mParticles, resource, &position);
}

// Reads the saved character into mCharacter, and sets it up
void MiiEditScene::LoadCharacter(int index) {
    func_0202c264(&gGameData, index, &mCharacter);
    LoadModel();
}

void MiiEditScene::LoadModel() {
    if (mCharacter.mClothesId <= 1) {
        switch (MiiData_GetSex(&mCharacter)) {
        case 0:
            func_0202aecc(data_021223e4, 0);
            ArchiveManager_Unmount(&gArchiveManager, "B0I");
            mCharacter.Character::SetArchive("B0I");
            ArchiveManager_MountCompressed(&gArchiveManager, "B0I", mCharacter.mArchivePath, 1);
            ModelResources_SetModel(data_021223e4, 0, mCharacter.mModelPath);
            mCharacter.MakeModel(0, 0);
            break;
        case 1:
            func_0202aecc(data_021223e4, 0);
            ArchiveManager_Unmount(&gArchiveManager, "B0I");
            mCharacter.Character::SetArchive("B0I");
            ArchiveManager_MountCompressed(&gArchiveManager, "B0I", mCharacter.mArchivePath, 1);
            ModelResources_SetModel(data_021223e4, 0, mCharacter.mModelPath);
            mCharacter.MakeModel(0, 0);
            break;
        }
    } else {
        func_0202aecc(data_021223e4, 0);
        ArchiveManager_Unmount(&gArchiveManager, "B0I");
        mCharacter.Character::SetArchive("B0I");
        ArchiveManager_MountCompressed(&gArchiveManager, "B0I", mCharacter.mArchivePath, 1);
        ModelResources_SetModel(data_021223e4, 0, mCharacter.mModelPath);
        mCharacter.MakeModel(0, 0);
    }
    void *animations = func_0202ad04(data_021223e4, 0x1F, "BIC", 0);
    func_02029660((void *)mCharacter.mAnim.mUnknown04, RESOURCE_BCA0, animations);
    mCharacter.Character::SetAnimation(RESOURCE_BCA0, 0);
    mCharacter.Character::SetAngle(0x4000);
    mCharacter.CharacterBase::MakeFace();
    mCharacter.Character::UpdateBodyColors();
    mCharacter.Character::UpdateBodyScale();
    mCharacter.Character::UpdateAnimSpeed();
}

static inline void SetFlag(u32 *flags, u8 flag) {
    flags[flag >> 5] |= 1 << (flag & 0x1F);
}

static inline void ClearFlag(u32 *flags, u8 flag) {
    flags[flag >> 5] &= ~(1 << (flag & 0x1F));
}

void MiiEditScene::SaveCharacter() {
    if (mCharacter.mUnknown050Bit0) {
        mCharacter.mUnknown050Bit0 = 0;
        func_0209e01c(&mCharacter);
    }
    int kind = data_020fba28[((SavedCharacter *)(&mCharacter))->GetPersonality()];
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
        mCharacter.SetLongAct(0x13, 0, 0x7FFFFFFFFFFFFFFF);
        mCharacter.mTroubleState = 1;
        mCharacter.mTroubleCategory = 0;
        mCharacter.mTroubleId = 0;
        mCharacter.mTroublePartner = 0;
        mCharacter.mTroubleEnd = 0x7FFFFFFFFFFFFFFF;
        break;
    case 0x43:
    case 0x44:
        ((SavedCharacter *)(&mCharacter))->SetLongAct(0x13, 0, 0x7FFFFFFFFFFFFFFF);
        mCharacter.ClearTrouble();
        mCharacter.mTroubleEnd = 0x7FFFFFFFFFFFFFFF;
        break;
    }
    func_0202c28c(&gGameData, mUnknown5824, &mCharacter);
}

void MiiEditScene::SetCharacterColors(BOOL a) {
    if (a) {
        mCharacter.mFaceDrawParam.diffuse = 0;
        mCharacter.mFaceDrawParam.ambient = 0;
        func_02029f10(&mCharacter, 0);
        func_02029f20(&mCharacter, 0);
        func_02029f30(&mCharacter, 0);
        func_02029f40(&mCharacter, 0);
    } else {
        mCharacter.mFaceDrawParam.diffuse = GX_RGB(31, 31, 31);
        mCharacter.mFaceDrawParam.ambient = GX_RGB(8, 8, 8);
        func_02029f10(&mCharacter, GX_RGB(31, 31, 31));
        func_02029f20(&mCharacter, GX_RGB(8, 8, 8));
    }
}

void MiiEditScene::ShowControlsWithWindow() {
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

void MiiEditSexPage::Init(MiiEditScene *scene) {
    mState = -1;
    mScene = scene;
    int charOffset = 900;
    mScene->ShowControlsWithWindow();
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
    SetState(0);
}

extern "C" {
void func_020341d4(MessageWindow *window);
void func_020349cc(MessageWindow *window, int flags);
extern const s16 data_020fbaac[];
void func_0209cbac(CharacterRecord *character, u8 *a, u8 *b);
void MiiData_GetName(CharacterRecord *character, wchar_t *name);
void MiiData_SetName(CharacterRecord *character, const wchar_t *name);
void MiiData_GetCreatorName(CharacterRecord *character, wchar_t *name);
void MiiData_SetCreatorName(CharacterRecord *character, const wchar_t *name);
}

void MiiEditSexPage::Update() {
    switch (mState) {
    case 0:
        func_020341d4(&mWindows[0]);
        // The original reads mResult again after this test, which only a volatile read gives here (making the
        // field volatile in MessageWindow.h breaks PreStartScene::Ask)
        if (*(volatile int *)&mWindows[0].mResult != -1) {
            int result = mWindows[0].mResult;
            if (result == 2) {
                gSceneManager.RequestScene(0x5E);
            } else {
                mWindows[0].mUnknownSetBeforeClose = 15;
                func_020349cc(&mWindows[0], 0x24);
                ((SavedCharacter *)(&mScene->mCharacter))->SetSex(result);
                switch (MiiData_GetSex(&mScene->mCharacter)) {
                case 0:
                    ((CharacterBase *)(&mScene->mCharacter))->SetDefaultMii(2, 1);
                    for (int i = 0; i < 6; i++) {
                        ((SavedCharacter *)(&mScene->mCharacter))->SetVoice(i, data_020fbaac[i + 12]);
                    }
                    break;
                case 1:
                    ((CharacterBase *)(&mScene->mCharacter))->SetDefaultMii(5, 1);
                    for (int i = 0; i < 6; i++) {
                        ((SavedCharacter *)(&mScene->mCharacter))->SetVoice(i, data_020fbaac[i + 18]);
                    }
                    mScene->mUnknown580C = TRUE;
                    break;
                }
                ((SavedCharacter *)(&mScene->mCharacter))->AssignCreateId();
                u8 a;
                u8 b;
                func_0209cbac(&mScene->mCharacter, &a, &b);
                mScene->mCharacter.mFavoriteItem = a;
                mScene->mCharacter.mDislikedItem = b;
                ((SavedCharacter *)(&mScene->mCharacter))->SetParameter(0x28, 0);
                ((SavedCharacter *)(&mScene->mCharacter))->SetParameter(0x24, 0);
                MiiData_SetName(&mScene->mCharacter, L"");
                mScene->LoadModel();
                ((Character *)(&mScene->mCharacter))->SetVisible(FALSE);
                mScene->SetCharacterColors(TRUE);
                if (mScene->mUnknown5824 != 0) {
                    wchar_t name[11];
                    SavedCharacter first;
                    MI_CpuClear16(name, sizeof(name));
                    func_0202c264(&gGameData, 0, &first);
                    MiiData_GetCreatorName(&first, name);
                    if (mScene->GetNameLength(name) == 0) {
                        MI_CpuClear16(name, sizeof(name));
                        MiiData_GetName(&first, name);
                    }
                    MiiData_SetCreatorName(&mScene->mCharacter, name);
                } else {
                    mScene->mCharacter.mRelationBlood = 1;
                }
            }
        }
        if (!mWindows[0].mTransitionDone) {
            return;
        }
        ((Character *)(&mScene->mCharacter))->SetVisible(TRUE);
        mScene->SetPage(1);
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
        ((Character *)(&mScene->mCharacter))->SetVisible(TRUE);
        mScene->SetPage(mNextPage);
        break;
    }
}

void MiiEditSexPage::SetState(int state) {
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

void MiiEditSexPage::Draw() {
    for (int i = 0; i < 2; i++) {
        func_020345d8(&mWindows[i]);
    }
}

void MiiEditSexPage::Exit() {
    for (int i = 0; i < 2; i++) {
        func_020346cc(&mWindows[i]);
    }
}

// Shows the scene's own buttons and texts again, and the lower screen's background (the pages call it when they
// close)
void MiiEditScene::ShowControls() {
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

void MiiEditScene::ShowControlsEditFace() {
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

void MiiEditScene::HideControls() {
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

void MiiEditScene::HideControlsNoBar() {
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

void MiiEditProgressSprite::SetPlace(int place) {
    SetPosition(FxVec2(sSpriteB0Places[place].x << FX32_SHIFT, sSpriteB0Places[place].y << FX32_SHIFT));
    mPlace = place;
}

void MiiEditProgressSprite::AdvanceTo(int place) {
    if (place <= mPlace) {
        return;
    }
    SetPosition(FxVec2(sSpriteB0Places[place].x << FX32_SHIFT, sSpriteB0Places[place].y << FX32_SHIFT));
    mPlace = place;
}

// Returns the length of the name once func_02092178 has been through it
int MiiEditScene::GetNameLength(const wchar_t *name) {
    wchar_t buffer[11];
    wcsncpy(buffer, name, 11);
    CharacterSort::RemoveSpaces(buffer);
    return wcslen(buffer);
}

// --- The face page ---

extern "C" {
void ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
void func_0203217c(MiiEditSprite100 *slider, int x, int a, int b, int left, int top, int width, int height);
void func_020325a8(MiiEditSprite100 *slider, u16 sequence, u16 pressedSequence);
void func_020325c4(MiiEditSprite100 *slider, int a, int b, int c, int d);
}

void MiiEditFacePage::Init(MiiEditScene *scene) {
    mCategory = -1;
    mScene = scene;
    scene->ShowControlsEditFace();
    for (int i = 0; i < 36; i++) {
        u16 sequence = data_ov076_02158f54[i];
        Sprite_Init(&mButtons[i], 0, sequence, 0x80, 0x60, TRUE);
        func_02031eb0(&mButtons[i], data_ov076_0215a13c[i].x, data_ov076_0215a13c[i].y);
        func_02031278(&mButtons[i], sequence, sequence + 1);
        func_02031260(&mButtons[i], data_ov076_02159350[i]);
        func_02032158(&mButtons[i], &data_ov076_02159de0[i]);
        mButtons[i].mUnknownDC = 14;
        mButtons[i].SetPositionFromVec(data_ov076_02159990[i].x << FX32_SHIFT, data_ov076_02159990[i].y << FX32_SHIFT);
        func_0203119c(&mButtons[i]);
    }
    for (int i = 0; i < 8; i++) {
        mButtons[data_ov076_02158b1c[i]].mVisible = TRUE;
        mButtons[data_ov076_02158b1c[i]].SetPositionFromVec(data_ov076_02158f14[i].x << FX32_SHIFT, data_ov076_02158f14[i].y << FX32_SHIFT);
    }
    ObjResourceManager_Load(&gObjResourceManager, "E00:/Face.nce", "E00:/Face.ncg", 0x1000, 1, 0x10000);
    for (u16 i = 0; i < 12; i++) {
        Sprite_Init(&mButtons227C[i], 1, (u16)(i * 2), 0x80, 0x60, TRUE);
        func_02031eb0(&mButtons227C[i], 0x2A, 0x24);
        func_02031278(&mButtons227C[i], (u16)(i * 2), (u16)(i * 2 + 1));
        func_02031260(&mButtons227C[i], 3);
        func_02032158(&mButtons227C[i], &data_ov076_0215891c);
        mButtons227C[i].mUnknownDC = 14;
        mButtons227C[i].SetPositionFromVec(sFaceTexturePositions[i].x << FX32_SHIFT, sFaceTexturePositions[i].y << FX32_SHIFT);
    }
    Sprite_Init(&mSprites2FF4[0], 0, 0x60, 0x80, 0x60, TRUE);
    Sprite_Init(&mSprites2FF4[1], 0, 0x61, 0x80, 0x60, TRUE);
    for (int i = 0; i < 2; i++) {
        u16 sequence = data_ov076_02158784[i];
        Sprite_Init(&mSprites2DF4[i].mSprite, 0, sequence, 0x80, 0x60, TRUE);
        func_0203217c(&mSprites2DF4[i], 0x91, 0x9A, 2, -8, -22, 0x10, 0x2C);
        mSprites2DF4[i].mUnknownD8 = 1;
        func_020325a8(&mSprites2DF4[i], sequence, sequence + 1);
        func_020325c4(&mSprites2DF4[i], -1, 6, 7, 10);
        mSprites2DF4[i].mUnknownF4 = 14;
        mSprites2DF4[i].mSprite.mPosition.y = data_ov076_0215890c[i].y << FX32_SHIFT;
        mSprites2FF4[i].mVisible = FALSE;
        mSprites2FF4[i].SetPositionFromVec(data_ov076_0215890c[i].x << FX32_SHIFT, data_ov076_0215890c[i].y << FX32_SHIFT);
    }
    Sprite_Init(&mSprite315C, 0, 0x62, 0x66, 0xB, TRUE);
    Sprite_Init(&mSprite32B4, 0, 0x63, 0x66, 0xB, TRUE);
    Sprite_Init(&mSprite3208, 0, 0x64, 0x66, 0xB, TRUE);
    Sprite_Init(&mSprite3360, 0, 0x65, 0x66, 0xB, TRUE);
    mSprite315C.mVisible = FALSE;
    mSprite32B4.mVisible = FALSE;
    mSprite3208.mVisible = FALSE;
    mSprite3360.mVisible = FALSE;
    mSprite315C.mAnim.animCtrl.bActive = FALSE;
    mSprite32B4.mAnim.animCtrl.bActive = FALSE;
    mSprite3208.mAnim.animCtrl.bActive = FALSE;
    mSprite3360.mAnim.animCtrl.bActive = FALSE;
    mUnknown314C[3] = 0;
    mUnknown314C[1] = 0;
    mUnknown314C[2] = 0;
    mUnknown314C[0] = 0;
    Sprite_SetFrame(&mSprite315C, mUnknown314C[0]);
    Sprite_SetFrame(&mSprite32B4, mUnknown314C[2]);
    Sprite_SetFrame(&mSprite3208, mUnknown314C[1]);
    Sprite_SetFrame(&mSprite3360, mUnknown314C[3]);
    mFaceTab.Init(this);
    mEtcTab.Init(this);
    for (int i = 0; i < 8; i++) {
        mUnknown0000[i] = FALSE;
    }
    ShowPart(1, 0);
}

extern "C" {
void func_020321ec(MiiEditSprite100 *slider);
int func_020324e0(MiiEditSprite100 *slider, int min, int max);
BOOL func_02032414(MiiEditSprite100 *slider);
void func_02032578(MiiEditSprite100 *slider, BOOL a);
}

void MiiEditFacePage::Update() {
    for (u16 i = 0; i < 36; i++) {
        if (!func_0203133c(&mButtons[i])) {
            continue;
        }
        int parameter;
        switch (i) {
        case 0:
            ShowPart(0, 0);
            break;
        case 1:
            ShowPart(1, 0);
            break;
        case 2:
            ShowPart(2, 0);
            break;
        case 3:
            ShowPart(3, 0);
            break;
        case 4:
            ShowPart(4, 0);
            break;
        case 5:
            ShowPart(5, 0);
            break;
        case 6:
            ShowPart(6, 0);
            break;
        case 7:
            ShowPart(7, 0);
            break;
        case 8:
            mFaceTab.SelectTab(0, FALSE);
            break;
        case 9:
            mFaceTab.SelectTab(1, FALSE);
            break;
        case 10:
            mEtcTab.SelectTab(0, FALSE);
            break;
        case 11:
            mEtcTab.SelectTab(1, FALSE);
            break;
        case 12:
            mEtcTab.SelectTab(2, FALSE);
            break;
        case 13:
            mEtcTab.SelectTab(3, FALSE);
            break;
        case 27:
            switch (mUndoMode) {
            case 1:
                SetParameter(mUndoParameter, mUndoValue);
                ((Character *)(&mScene->mCharacter))->UpdateBodyScale();
                ((Character *)(&mScene->mCharacter))->UpdateAnimSpeed();
                ShowPart(mCategory, 1);
                SetUndoMode(2);
                break;
            case 2:
                SetParameter(mUndoParameter, mUndoValue);
                ((Character *)(&mScene->mCharacter))->UpdateBodyScale();
                ((Character *)(&mScene->mCharacter))->UpdateAnimSpeed();
                ShowPart(mCategory, 1);
                SetUndoMode(1);
                break;
            }
            break;
        case 25:
        case 26:
            switch (mCategory) {
            case 2: {
                if (i == 26) {
                    if (++mUnknown314C[0] >= 6) {
                        mUnknown314C[0] -= 6;
                    }
                } else if (--mUnknown314C[0] < 0) {
                    mUnknown314C[0] += 6;
                }
                Sprite_SetFrame(&mSprite315C, mUnknown314C[0]);
                ShowPartButtons(mUnknown314C[0], 12, mCategory);
                u32 button = GetPartButton(((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x19), mCategory);
                int index = button % 12;
                int page = button / 12;
                if (page == mUnknown314C[0]) {
                    func_02031e28(&mButtons227C[index], TRUE);
                }
                break;
            }
            case 3: {
                if (i == 26) {
                    if (++mUnknown314C[1] >= 2) {
                        mUnknown314C[1] -= 2;
                    }
                } else if (--mUnknown314C[1] < 0) {
                    mUnknown314C[1] += 2;
                }
                Sprite_SetFrame(&mSprite3208, mUnknown314C[1]);
                ShowPartButtons(mUnknown314C[1], 12, mCategory);
                u32 button = GetPartButton(((SavedCharacter *)(&mScene->mCharacter))->GetParameter(6), mCategory);
                int index = button % 12;
                int page = button / 12;
                if (page == mUnknown314C[1]) {
                    func_02031e28(&mButtons227C[index], TRUE);
                }
                break;
            }
            case 4: {
                if (i == 26) {
                    if (++mUnknown314C[2] >= 4) {
                        mUnknown314C[2] -= 4;
                    }
                } else if (--mUnknown314C[2] < 0) {
                    mUnknown314C[2] += 4;
                }
                Sprite_SetFrame(&mSprite32B4, mUnknown314C[2]);
                ShowPartButtons(mUnknown314C[2], 12, mCategory);
                u32 button = GetPartButton(((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0), mCategory);
                int index = button % 12;
                int page = button / 12;
                if (page == mUnknown314C[2]) {
                    func_02031e28(&mButtons227C[index], TRUE);
                }
                break;
            }
            case 6: {
                if (i == 26) {
                    if (++mUnknown314C[3] >= 2) {
                        mUnknown314C[3] -= 2;
                    }
                } else if (--mUnknown314C[3] < 0) {
                    mUnknown314C[3] += 2;
                }
                Sprite_SetFrame(&mSprite3360, mUnknown314C[3]);
                ShowPartButtons(mUnknown314C[3], 12, mCategory);
                u32 button = GetPartButton(((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0xC), mCategory);
                int index = button % 12;
                int page = button / 12;
                if (page == mUnknown314C[3]) {
                    func_02031e28(&mButtons227C[index], TRUE);
                }
                break;
            }
            }
            break;
        case 28:
        case 29:
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
        case 35: {
            int color = i - 28;
            SelectColorButton(color, mCategory, mFaceTab.mTab);
            switch (mCategory) {
            case 1:
                parameter = 0x24;
                break;
            case 2:
                parameter = 0x1A;
                break;
            case 3:
                parameter = 7;
                break;
            case 4:
                parameter = 1;
                break;
            case 6:
                parameter = 0xD;
                break;
            case 7:
                switch (mEtcTab.mTab) {
                case 0:
                    parameter = 0x20;
                    break;
                case 1:
                    parameter = 0x11;
                    break;
                case 2:
                    parameter = 0x11;
                    break;
                case 3:
                    parameter = 0x11;
                    break;
                }
                break;
            }
            SetParameter(parameter, color);
            break;
        }
        }
        if (i <= 7) {
            for (int j = 0; j <= 7; j++) {
                if (i != j) {
                    func_02031e28(&mButtons[j], FALSE);
                }
            }
        }
        if (i >= 8 && i <= 9) {
            for (int j = 8; j <= 9; j++) {
                if (i != j) {
                    func_02031e28(&mButtons[j], FALSE);
                }
            }
        }
        if (i >= 10 && i <= 13) {
            for (int j = 10; j <= 13; j++) {
                if (i != j) {
                    func_02031e28(&mButtons[j], FALSE);
                }
            }
        }
        BOOL limit = FALSE;
        switch (mCategory) {
        case 2:
            if (i == 24) {
                limit = StepParameter(0x1B, 1, TRUE);
            }
            break;
        case 3:
            if (((SavedCharacter *)(&mScene->mCharacter))->GetParameter(6) == 0x17) {
                limit = TRUE;
                break;
            }
            switch (i - 14) {
            case 0:
                limit = StepParameter(0xB, -1, limit);
                break;
            case 1:
                limit = StepParameter(0xB, 1, limit);
                break;
            case 2:
                limit = StepParameter(8, 1, limit);
                break;
            case 3:
                limit = StepParameter(8, -1, limit);
                break;
            case 4:
                limit = StepParameter(9, -1, limit);
                break;
            case 5:
                limit = StepParameter(9, 1, limit);
                break;
            case 6:
                limit = StepParameter(0xA, -1, limit);
                break;
            case 7:
                limit = StepParameter(0xA, 1, limit);
                break;
            }
            break;
        case 4:
            switch (i - 14) {
            case 0:
                limit = StepParameter(5, -1, limit);
                break;
            case 1:
                limit = StepParameter(5, 1, limit);
                break;
            case 2:
                limit = StepParameter(2, 1, limit);
                break;
            case 3:
                limit = StepParameter(2, -1, limit);
                break;
            case 4:
                limit = StepParameter(3, -1, limit);
                break;
            case 5:
                limit = StepParameter(3, 1, limit);
                break;
            case 6:
                limit = StepParameter(4, -1, limit);
                break;
            case 7:
                limit = StepParameter(4, 1, limit);
                break;
            }
            break;
        case 5:
            switch (i - 14) {
            case 0:
                limit = StepParameter(0x1E, -1, limit);
                break;
            case 1:
                limit = StepParameter(0x1E, 1, limit);
                break;
            case 2:
                limit = StepParameter(0x1D, 1, limit);
                break;
            case 3:
                limit = StepParameter(0x1D, -1, limit);
                break;
            }
            break;
        case 6:
            switch (i - 14) {
            case 0:
                limit = StepParameter(0xF, -1, limit);
                break;
            case 1:
                limit = StepParameter(0xF, 1, limit);
                break;
            case 2:
                limit = StepParameter(0xE, 1, limit);
                break;
            case 3:
                limit = StepParameter(0xE, -1, limit);
                break;
            }
            break;
        case 7:
            switch (mEtcTab.mTab) {
            case 0:
                if (((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x1F) == 0) {
                    limit = TRUE;
                    break;
                }
                switch (i - 14) {
                case 0:
                    limit = StepParameter(0x22, -1, limit);
                    break;
                case 1:
                    limit = StepParameter(0x22, 1, limit);
                    break;
                case 2:
                    limit = StepParameter(0x21, 1, limit);
                    break;
                case 3:
                    limit = StepParameter(0x21, -1, limit);
                    break;
                }
                break;
            case 1:
                if (((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x12) == 0) {
                    limit = TRUE;
                    break;
                }
                switch (i - 14) {
                case 0:
                    limit = StepParameter(0x14, -1, limit);
                    break;
                case 1:
                    limit = StepParameter(0x14, 1, limit);
                    break;
                case 2:
                    limit = StepParameter(0x13, 1, limit);
                    break;
                case 3:
                    limit = StepParameter(0x13, -1, limit);
                    break;
                }
                break;
            case 2:
                if (((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x15) == 0) {
                    limit = TRUE;
                    break;
                }
                switch (i - 14) {
                case 0:
                    limit = StepParameter(0x18, -1, limit);
                    break;
                case 1:
                    limit = StepParameter(0x18, 1, limit);
                    break;
                case 2:
                    limit = StepParameter(0x16, 1, limit);
                    break;
                case 3:
                    limit = StepParameter(0x16, -1, limit);
                    break;
                case 9:
                    limit = StepParameter(0x17, 1, limit);
                    break;
                case 8:
                    limit = StepParameter(0x17, -1, limit);
                    break;
                }
                break;
            case 3:
                break;
            }
            break;
        }
        switch (i) {
        case 14:
        case 15:
        case 16:
        case 17:
        case 18:
        case 19:
        case 20:
        case 21:
        case 22:
        case 23: {
            int sound;
            switch (i - 14) {
            case 0:
            case 2:
            case 4:
            case 6:
            case 9:
                sound = limit ? 0xB : 9;
                break;
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
                sound = limit ? 0xA : 8;
                break;
            }
            gSoundManager.StartSeqArc(1, 0xE, sound);
            break;
        }
        }
    }
    for (u16 i = 0; i < 12; i++) {
        if (!func_0203133c(&mButtons227C[i])) {
            continue;
        }
        switch (mCategory) {
        case 1:
            switch (mFaceTab.mTab) {
            case 0:
                ChoosePart(0x23, i, 0, 8);
                break;
            case 1:
                ChoosePart(0x25, i, 0, 12);
                break;
            }
            break;
        case 2:
            ChoosePart(0x19, i, (u16)mUnknown314C[0], 12);
            break;
        case 3:
            ChoosePart(6, i, (u16)mUnknown314C[1], 12);
            break;
        case 4:
            ChoosePart(0, i, (u16)mUnknown314C[2], 12);
            break;
        case 5:
            ChoosePart(0x1C, i, 0, 12);
            break;
        case 6:
            ChoosePart(0xC, i, (u16)mUnknown314C[3], 12);
            break;
        case 7:
            switch (mEtcTab.mTab) {
            case 0:
                ChoosePart(0x1F, i, 0, 9);
                break;
            case 1:
                ChoosePart(0x12, i, 0, 4);
                break;
            case 2:
                ChoosePart(0x15, i, 0, 2);
                break;
            case 3:
                ChoosePart(0x10, i, 0, 4);
                break;
            }
            break;
        }
    }
    switch (mCategory) {
    case 0:
        for (int i = 0; i < 2; i++) {
            func_020321ec(&mSprites2DF4[i]);
            if (mSprites2DF4[i].mUnknownBC) {
                switch (i) {
                case 0: {
                    int value = func_020324e0(&mSprites2DF4[i], 0, 0x7F);
                    ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x2A);
                    ((Character *)(&mScene->mCharacter))->UpdateBodyScale();
                    ((SavedCharacter *)(&mScene->mCharacter))->SetParameter(0x29, value);
                    break;
                }
                case 1: {
                    ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x29);
                    int value = func_020324e0(&mSprites2DF4[i], 0, 0x7F);
                    ((Character *)(&mScene->mCharacter))->UpdateBodyScale();
                    ((SavedCharacter *)(&mScene->mCharacter))->SetParameter(0x2A, value);
                    break;
                }
                }
            }
            if (func_02032414(&mSprites2DF4[i])) {
                for (int j = 0; j < 2; j++) {
                    if (j != i) {
                        func_02032578(&mSprites2DF4[j], FALSE);
                    }
                }
            }
        }
        break;
    case 1:
        mFaceTab.Update();
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
        break;
    }
}

extern "C" {
void func_02032514(MiiEditSprite100 *slider, int min, int max, int value);
void func_02031ca4(SpriteButton *button);
}

void MiiEditFacePage::ShowPart(int part, BOOL a) {
    mUnknown0000[part] = TRUE;
    if (mScene->mProgressSprite.mPlace == 0 && mScene->mUnknown58E8[2] == FALSE) {
        BOOL all = TRUE;
        for (int i = 0; i < 8; i++) {
            if (i != 7 && mUnknown0000[i] == FALSE) {
                all = FALSE;
                break;
            }
        }
        if (all) {
            mScene->mUnknown58DC = TRUE;
        }
    }
    for (int i = 0; i < 2; i++) {
        mSprites2DF4[i].mSprite.mVisible = FALSE;
        mSprites2FF4[i].mVisible = FALSE;
    }
    mButtons[25].mVisible = FALSE;
    mButtons[26].mVisible = FALSE;
    mButtons[8].mVisible = FALSE;
    mButtons[9].mVisible = FALSE;
    mButtons[10].mVisible = FALSE;
    mButtons[11].mVisible = FALSE;
    mButtons[12].mVisible = FALSE;
    mButtons[13].mVisible = FALSE;
    mSprite315C.mVisible = FALSE;
    mSprite3208.mVisible = FALSE;
    mSprite32B4.mVisible = FALSE;
    mSprite3360.mVisible = FALSE;
    for (int i = 0; i <= 7; i++) {
        if (i != part) {
            func_02031e28(&mButtons[i], FALSE);
        }
    }
    mButtons[24].mVisible = FALSE;
    mButtons[14].mVisible = FALSE;
    mButtons[15].mVisible = FALSE;
    mButtons[16].mVisible = FALSE;
    mButtons[17].mVisible = FALSE;
    mButtons[18].mVisible = FALSE;
    mButtons[19].mVisible = FALSE;
    mButtons[20].mVisible = FALSE;
    mButtons[21].mVisible = FALSE;
    mButtons[22].mVisible = FALSE;
    mButtons[23].mVisible = FALSE;
    for (u16 i = 28; i <= 35; i++) {
        mButtons[i].mVisible = FALSE;
    }
    SetUndoMode(0);
    switch (part) {
    case 0:
        mScene->MoveCameraTo(sProfileCameraGoal);
        for (u16 i = 0; i < 12; i++) {
            mButtons227C[i].mVisible = FALSE;
        }
        for (int i = 0; i < 2; i++) {
            mSprites2DF4[i].mSprite.mVisible = TRUE;
            mSprites2FF4[i].mVisible = TRUE;
        }
        func_02032514(&mSprites2DF4[0], 0, 0x7F, ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x29));
        func_02032514(&mSprites2DF4[1], 0, 0x7F, ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x2A));
        SelectColorButton(0, part, -1);
        break;
    case 1: {
        VecFx32 goal = data_ov076_02158850;
        goal.y += ((u32)((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x29) * 0x1130 >> 7) - 0x898;
        mScene->MoveCameraTo(goal);
        mFaceTab.SelectTab(mFaceTab.mTab, a);
        break;
    }
    case 2: {
        VecFx32 goal = data_ov076_02158850;
        goal.y += ((u32)((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x29) * 0x1130 >> 7) - 0x898;
        mScene->MoveCameraTo(goal);
        u32 button = GetPartButton(((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x19), part);
        int page = button / 12;
        int index = button % 12;
        int color = ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x1A);
        func_02027534(&data_021213b8, "E00:/Fw_color.ncl", 0x1000, 0xA0, 0x40, 0x60);
        for (u16 i = 28; i <= 35; i++) {
            if (i - 28 < 8) {
                mButtons[i].mVisible = TRUE;
            } else {
                mButtons[i].mVisible = FALSE;
            }
        }
        SelectColorButton(color, part, -1);
        mButtons[24].mVisible = TRUE;
        mUnknown314C[0] = page;
        mSprite315C.mVisible = TRUE;
        Sprite_SetFrame(&mSprite315C, mUnknown314C[0]);
        mButtons[25].mVisible = TRUE;
        mButtons[26].mVisible = TRUE;
        ObjResourceManager_Load(&gObjResourceManager, "E00:/Hair.nce", "E00:/Hair.ncg", 0x1000, 1, 0x10000);
        for (u16 i = 0; i < 12; i++) {
            mButtons227C[i].SetPositionFromVec(sFaceTexturePositions[i].x << FX32_SHIFT, sFaceTexturePositions[i].y << FX32_SHIFT);
        }
        ShowPartButtons(mUnknown314C[0], 12, part);
        if (!a) {
            func_02031e28(&mButtons227C[index], TRUE);
        } else {
            func_02031ca4(&mButtons227C[index]);
        }
        break;
    }
    case 3: {
        VecFx32 goal = data_ov076_02158850;
        goal.y += ((u32)((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x29) * 0x1130 >> 7) - 0x898;
        mScene->MoveCameraTo(goal);
        u32 button = GetPartButton(((SavedCharacter *)(&mScene->mCharacter))->GetParameter(6), part);
        int page = button / 12;
        int index = button % 12;
        int color = ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(7);
        func_02027534(&data_021213b8, "E00:/Fw_color.ncl", 0x1000, 0xA0, 0x40, 0x60);
        for (u16 i = 28; i <= 35; i++) {
            if (i - 28 < 8) {
                mButtons[i].mVisible = TRUE;
            } else {
                mButtons[i].mVisible = FALSE;
            }
        }
        SelectColorButton(color, part, -1);
        for (u16 i = 0; i < 8; i++) {
            mButtons[data_ov076_02158b1c[i]].mVisible = TRUE;
            mButtons[data_ov076_02158b1c[i]].SetPositionFromVec(data_ov076_02158f14[i].x << FX32_SHIFT, data_ov076_02158f14[i].y << FX32_SHIFT);
        }
        mUnknown314C[1] = page;
        mSprite3208.mVisible = TRUE;
        Sprite_SetFrame(&mSprite3208, mUnknown314C[1]);
        mButtons[25].mVisible = TRUE;
        mButtons[26].mVisible = TRUE;
        ObjResourceManager_Load(&gObjResourceManager, "E00:/Eyebrow.nce", "E00:/Eyebrow.ncg", 0x1000, 1, 0x10000);
        for (u16 i = 0; i < 12; i++) {
            mButtons227C[i].SetPositionFromVec(sFaceTexturePositions[i].x << FX32_SHIFT, sFaceTexturePositions[i].y << FX32_SHIFT);
        }
        ShowPartButtons(mUnknown314C[1], 12, part);
        if (!a) {
            func_02031e28(&mButtons227C[index], TRUE);
        } else {
            func_02031ca4(&mButtons227C[index]);
        }
        break;
    }
    case 4: {
        VecFx32 goal = data_ov076_02158850;
        goal.y += ((u32)((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x29) * 0x1130 >> 7) - 0x898;
        mScene->MoveCameraTo(goal);
        u32 button = GetPartButton(((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0), part);
        int page = button / 12;
        int index = button % 12;
        int color = ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(1);
        func_02027534(&data_021213b8, "E00:/Fw_color.ncl", 0x1000, 0xE0, 0x40, 0x60);
        for (u16 i = 28; i <= 35; i++) {
            if (i - 28 < 6) {
                mButtons[i].mVisible = TRUE;
            } else {
                mButtons[i].mVisible = FALSE;
            }
        }
        SelectColorButton(color, part, -1);
        for (u16 i = 0; i < 8; i++) {
            mButtons[data_ov076_02158b1c[i]].mVisible = TRUE;
            mButtons[data_ov076_02158b1c[i]].SetPositionFromVec(data_ov076_02158f14[i].x << FX32_SHIFT, data_ov076_02158f14[i].y << FX32_SHIFT);
        }
        mUnknown314C[2] = page;
        mSprite32B4.mVisible = TRUE;
        Sprite_SetFrame(&mSprite32B4, mUnknown314C[2]);
        mButtons[25].mVisible = TRUE;
        mButtons[26].mVisible = TRUE;
        ObjResourceManager_Load(&gObjResourceManager, "E00:/Eye.nce", "E00:/Eye.ncg", 0x1000, 1, 0x10000);
        for (u16 i = 0; i < 12; i++) {
            mButtons227C[i].SetPositionFromVec(sFaceTexturePositions[i].x << FX32_SHIFT, sFaceTexturePositions[i].y << FX32_SHIFT);
        }
        ShowPartButtons(mUnknown314C[2], 12, part);
        if (!a) {
            func_02031e28(&mButtons227C[index], TRUE);
        } else {
            func_02031ca4(&mButtons227C[index]);
        }
        break;
    }
    case 5: {
        VecFx32 goal = data_ov076_02158850;
        goal.y += ((u32)((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x29) * 0x1130 >> 7) - 0x898;
        mScene->MoveCameraTo(goal);
        u32 button = GetPartButton(((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x1C), part);
        int index = button % 12;
        for (u16 i = 28; i <= 35; i++) {
            if (i - 28 < 0) {
                mButtons[i].mVisible = TRUE;
            } else {
                mButtons[i].mVisible = FALSE;
            }
        }
        SelectColorButton(0, part, -1);
        for (u16 i = 0; i < 4; i++) {
            mButtons[sAdjustButtons[i]].mVisible = TRUE;
            mButtons[sAdjustButtons[i]].SetPositionFromVec(sAdjustButtonPositions[i].x << FX32_SHIFT, sAdjustButtonPositions[i].y << FX32_SHIFT);
        }
        ObjResourceManager_Load(&gObjResourceManager, "E00:/Nose.nce", "E00:/Nose.ncg", 0x1000, 1, 0x10000);
        for (u16 i = 0; i < 12; i++) {
            mButtons227C[i].SetPositionFromVec(sFaceTexturePositions[i].x << FX32_SHIFT, sFaceTexturePositions[i].y << FX32_SHIFT);
        }
        ShowPartButtons(0, 12, part);
        if (!a) {
            func_02031e28(&mButtons227C[index], TRUE);
        } else {
            func_02031ca4(&mButtons227C[index]);
        }
        break;
    }
    case 6: {
        VecFx32 goal = data_ov076_02158850;
        goal.y += ((u32)((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x29) * 0x1130 >> 7) - 0x898;
        mScene->MoveCameraTo(goal);
        u32 button = GetPartButton(((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0xC), part);
        int page = button / 12;
        int index = button % 12;
        int color = ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0xD);
        func_02027534(&data_021213b8, "E00:/Fw_color.ncl", 0x1000, 0x120, 0x40, 0x60);
        for (u16 i = 28; i <= 35; i++) {
            if (i - 28 < 3) {
                mButtons[i].mVisible = TRUE;
            } else {
                mButtons[i].mVisible = FALSE;
            }
        }
        SelectColorButton(color, part, -1);
        for (u16 i = 0; i < 4; i++) {
            mButtons[sAdjustButtons[i]].mVisible = TRUE;
            mButtons[sAdjustButtons[i]].SetPositionFromVec(sAdjustButtonPositions[i].x << FX32_SHIFT, sAdjustButtonPositions[i].y << FX32_SHIFT);
        }
        mUnknown314C[3] = page;
        mSprite3360.mVisible = TRUE;
        Sprite_SetFrame(&mSprite3360, mUnknown314C[3]);
        mButtons[25].mVisible = TRUE;
        mButtons[26].mVisible = TRUE;
        ObjResourceManager_Load(&gObjResourceManager, "E00:/Mouth.nce", "E00:/Mouth.ncg", 0x1000, 1, 0x10000);
        for (u16 i = 0; i < 12; i++) {
            mButtons227C[i].SetPositionFromVec(sFaceTexturePositions[i].x << FX32_SHIFT, sFaceTexturePositions[i].y << FX32_SHIFT);
        }
        ShowPartButtons(mUnknown314C[3], 12, part);
        if (!a) {
            func_02031e28(&mButtons227C[index], TRUE);
        } else {
            func_02031ca4(&mButtons227C[index]);
        }
        break;
    }
    case 7: {
        VecFx32 goal = data_ov076_02158850;
        goal.y += ((u32)((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x29) * 0x1130 >> 7) - 0x898;
        mScene->MoveCameraTo(goal);
        mEtcTab.SelectTab(mEtcTab.mTab, a);
        break;
    }
    }
    mCategory = part;
    mUnknown0024 = 0;
}

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
void ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
}

// The part type that each of the parts' buttons shows, in the order of the buttons, is in sHairTypes
// (hair, Mii parameter 0x19), sEyebrowTypes (eyebrows, 6), sEyeTypes (eyes, 0), sNoseTypes
// (nose, 0x1C) and sMouthTypes (mouth, 0xC). By eye type and by eyebrow type, sEyeRotations and
// sEyebrowRotations: SetParameter turns the eyes or the eyebrows by the difference between the old type's entry
// and the new one's.

// Shows which of the eight colour buttons (28 to 35) is selected, and loads the parts' palette for the category
void MiiEditFacePage::SelectColorButton(int selected, int category, int tab) {
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
void MiiEditFacePage::ShowPartButtons(int page, u32 count, int category) {
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
        button = sHairTypes[button];
        break;
    case 3:
        button = sEyebrowTypes[button];
        break;
    case 4:
        button = sEyeTypes[button];
        break;
    case 5:
        button = sNoseTypes[button];
        break;
    case 6:
        button = sMouthTypes[button];
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
        ((SavedCharacter *)(&mScene->mCharacter))->GetParameterRange(0x19, &min, &max);
        for (int i = min; i <= max; i++) {
            if (type == sHairTypes[i]) {
                type = i;
                break;
            }
        }
        break;
    case 3:
        ((SavedCharacter *)(&mScene->mCharacter))->GetParameterRange(6, &min, &max);
        for (int i = min; i <= max; i++) {
            if (type == sEyebrowTypes[i]) {
                type = i;
                break;
            }
        }
        break;
    case 4:
        ((SavedCharacter *)(&mScene->mCharacter))->GetParameterRange(0, &min, &max);
        for (int i = min; i <= max; i++) {
            if (type == sEyeTypes[i]) {
                type = i;
                break;
            }
        }
        break;
    case 5:
        ((SavedCharacter *)(&mScene->mCharacter))->GetParameterRange(0x1C, &min, &max);
        for (int i = min; i <= max; i++) {
            if (type == sNoseTypes[i]) {
                type = i;
                break;
            }
        }
        break;
    case 6:
        ((SavedCharacter *)(&mScene->mCharacter))->GetParameterRange(0xC, &min, &max);
        for (int i = min; i <= max; i++) {
            if (type == sMouthTypes[i]) {
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
void MiiEditFacePage::ChoosePart(int parameter, int selected, int page, u16 count) {
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
        mUndoParameter = parameter;
        mUndoValue = ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(parameter);
        SetUndoMode(1);
        break;
    }
    switch (parameter) {
    case 0: {
        int delta = sEyeRotations[mUndoValue] - sEyeRotations[value];
        int min, max;
        ((SavedCharacter *)(&mScene->mCharacter))->GetParameterRange(3, &min, &max);
        ((CharacterBase *)(&mScene->mCharacter))->SetMiiParameter(3, CLAMP(((SavedCharacter *)(&mScene->mCharacter))->GetParameter(3) + delta, min, max), FALSE);
        break;
    }
    case 6: {
        int delta = sEyebrowRotations[mUndoValue] - sEyebrowRotations[value];
        int min, max;
        ((SavedCharacter *)(&mScene->mCharacter))->GetParameterRange(9, &min, &max);
        ((CharacterBase *)(&mScene->mCharacter))->SetMiiParameter(9, CLAMP(((SavedCharacter *)(&mScene->mCharacter))->GetParameter(9) + delta, min, max), FALSE);
        break;
    }
    }
    ((CharacterBase *)(&mScene->mCharacter))->SetMiiParameter(parameter, value, TRUE);
    ((Character *)(&mScene->mCharacter))->UpdateBodyColors();
    mScene->mUnknown580C = TRUE;
    switch (parameter) {
    case 0:
        mScene->PlayEffectAtHead(0, (12 - ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(5)) * FX32_ONE / 30 + 0x333);
        break;
    case 1:
        mScene->PlayEffectAtHead(0, 0x333);
        break;
    case 6:
        mScene->PlayEffectAtHead(0, (10 - ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0xB)) * FX32_ONE / 30 + 0x4CC);
        break;
    case 7:
        if (((SavedCharacter *)(&mScene->mCharacter))->GetParameter(6) == 0x17) {
            break;
        }
        mScene->PlayEffectAtHead(0, 0x4CC);
        break;
    case 12:
        mScene->PlayEffectAtHead(0, (13 - ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0xF)) * FX32_ONE / 30 - 0x333);
        break;
    case 13:
        mScene->PlayEffectAtHead(0, -0x333);
        break;
    case 16:
        mScene->PlayEffectAtHead(0, -0x666);
        break;
    case 17:
        if (((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x10) == 0 && ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x12) == 0) {
            break;
        }
        mScene->PlayEffectAtHead(0, -0x666);
        break;
    case 18:
        mScene->PlayEffectAtHead(0, (10 - ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x14)) * FX32_ONE / 30 - 0x333);
        break;
    case 21:
        mScene->PlayEffectAtHead(0, (20 - ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x18)) * FX32_ONE / 30);
        break;
    case 25:
        mScene->PlayEffectAtHead(0, 0x800);
        break;
    case 26:
        if (((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x19) == 0x1E) {
            break;
        }
        mScene->PlayEffectAtHead(0, 0x800);
        break;
    case 28:
        mScene->PlayEffectAtHead(0, (9 - ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x1E)) * FX32_ONE / 30);
        break;
    case 31:
        mScene->PlayEffectAtHead(0, (10 - ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x22)) * FX32_ONE / 30 + 0x333);
        break;
    case 32:
        if (((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x1F) == 0) {
            break;
        }
        mScene->PlayEffectAtHead(0, 0x333);
        break;
    case 35:
    case 36:
    case 37:
        mScene->PlayEffectAtHead(0, 0);
        break;
    }
}

#pragma opt_common_subs reset
// Moves a Mii parameter by a step, around its range or up to its ends; returns whether it stopped at an end
BOOL MiiEditFacePage::StepParameter(int parameter, int step, BOOL wrap) {
    BOOL stopped = FALSE;
    int value = ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(parameter);
    int min, max;
    value += step;
    ((SavedCharacter *)(&mScene->mCharacter))->GetParameterRange(parameter, &min, &max);
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
    ((CharacterBase *)(&mScene->mCharacter))->SetMiiParameter(parameter, (u16)value, TRUE);
    mScene->mUnknown580C = TRUE;
    return stopped;
}

// Shows button 27 or hides it, and which way it faces
void MiiEditFacePage::SetUndoMode(int mode) {
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
    mUndoMode = mode;
}

void MiiEditFaceTab::Init(MiiEditFacePage *page) {
    mPage = page;
    mTab = 0;
    func_02031e28(&mPage->mButtons[8], TRUE);
}

void MiiEditFaceTab::Update() {}

// Where the face tab's buttons are: the face types' (sFaceTypePositions) and the face textures'
// (sFaceTexturePositions)
void MiiEditFaceTab::SelectTab(int tab, BOOL pressed) {
    ObjResourceManager_Load(&gObjResourceManager, "E00:/Face.nce", "E00:/Face.ncg", 0x1000, 1, 0x10000);
    mPage->SetUndoMode(0);
    mPage->mButtons[8].mVisible = TRUE;
    mPage->mButtons[9].mVisible = TRUE;
    switch (tab) {
    case 0:
        for (u16 i = 0; i < 8; i++) {
            u16 sequence = i * 2;
            mPage->mButtons227C[i].mVisible = TRUE;
            func_02031278(&mPage->mButtons227C[i], sequence, sequence + 1);
            func_0203119c(&mPage->mButtons227C[i]);
            mPage->mButtons227C[i].SetPositionFromVec(sFaceTypePositions[i].x << FX32_SHIFT,
                                                      sFaceTypePositions[i].y << FX32_SHIFT);
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
            mPage->mButtons227C[i].SetPositionFromVec(sFaceTexturePositions[i].x << FX32_SHIFT,
                                                      sFaceTexturePositions[i].y << FX32_SHIFT);
        }
        break;
    }
    int color = ((SavedCharacter *)(&mPage->mScene->mCharacter))->GetParameter(0x24);
    switch (tab) {
    case 0: {
        u32 button = mPage->GetPartButton(((SavedCharacter *)(&mPage->mScene->mCharacter))->GetParameter(0x23), 1);
        button %= 8;
        if (!pressed) {
            func_02031e28(&mPage->mButtons227C[button], TRUE);
        } else {
            func_02031ca4(&mPage->mButtons227C[button]);
        }
        break;
    }
    case 1: {
        u32 button = mPage->GetPartButton(((SavedCharacter *)(&mPage->mScene->mCharacter))->GetParameter(0x25), 1);
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
    mPage->SelectColorButton(color, 1, tab);
    mTab = tab;
    mUnknown4 = 0;
}

void MiiEditEtcTab::Init(MiiEditFacePage *page) {
    mPage = page;
    mTab = 0;
    func_02031e28(&mPage->mButtons[10], TRUE);
}

// Where the etc tab's buttons are: the glasses types' (sGlassesPositions), the mustache types' and the beard
// types' (sMustachePositions), the mole types' (sMolePositions). The face page's buttons that it places
// beside the types are sAdjustButtons and, for the mole, sMoleAdjustButtons, at sAdjustButtonPositions and
// sMoleAdjustButtonPositions (inferred: the arrows that change a part's size and place, as the glasses and the mustache
// have two such parameters and the mole three).
void MiiEditEtcTab::SelectTab(int tab, BOOL pressed) {
    mPage->mButtons[10].mVisible = TRUE;
    mPage->mButtons[11].mVisible = TRUE;
    mPage->mButtons[12].mVisible = TRUE;
    mPage->mButtons[13].mVisible = TRUE;
    mPage->SetUndoMode(0);
    ObjResourceManager_Load(&gObjResourceManager, "E00:/Etc.nce", "E00:/Etc.ncg", 0x1000, 1, 0x10000);
    switch (tab) {
    case 0:
        for (u16 i = 0; i < 9; i++) {
            u16 sequence = i * 2;
            mPage->mButtons227C[i].mVisible = TRUE;
            func_02031278(&mPage->mButtons227C[i], sequence, sequence + 1);
            func_0203119c(&mPage->mButtons227C[i]);
            mPage->mButtons227C[i].SetPositionFromVec(sGlassesPositions[i].x << FX32_SHIFT,
                                                      sGlassesPositions[i].y << FX32_SHIFT);
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
            mPage->mButtons227C[i].SetPositionFromVec(sMustachePositions[i].x << FX32_SHIFT,
                                                      sMustachePositions[i].y << FX32_SHIFT);
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
            mPage->mButtons227C[i].SetPositionFromVec(sMolePositions[i].x << FX32_SHIFT,
                                                      sMolePositions[i].y << FX32_SHIFT);
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
            mPage->mButtons227C[i].SetPositionFromVec(sMustachePositions[i].x << FX32_SHIFT,
                                                      sMustachePositions[i].y << FX32_SHIFT);
        }
        for (u16 i = 4; i < 12; i++) {
            mPage->mButtons227C[i].mVisible = FALSE;
        }
        break;
    }
    for (u16 i = 0; i < 6; i++) {
        mPage->mButtons[sMoleAdjustButtons[i]].mVisible = FALSE;
    }
    switch (tab) {
    case 0: {
        u32 button = mPage->GetPartButton(((SavedCharacter *)(&mPage->mScene->mCharacter))->GetParameter(0x1F), 7);
        button %= 9;
        int color = ((SavedCharacter *)(&mPage->mScene->mCharacter))->GetParameter(0x20);
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
        mPage->SelectColorButton(color, 7, -1);
        for (u16 i = 0; i < 4; i++) {
            mPage->mButtons[sAdjustButtons[i]].mVisible = TRUE;
            mPage->mButtons[sAdjustButtons[i]].SetPositionFromVec(sAdjustButtonPositions[i].x << FX32_SHIFT,
                                                                  sAdjustButtonPositions[i].y << FX32_SHIFT);
        }
        break;
    }
    case 1: {
        u32 button = mPage->GetPartButton(((SavedCharacter *)(&mPage->mScene->mCharacter))->GetParameter(0x12), 7);
        button %= 4;
        int color = ((SavedCharacter *)(&mPage->mScene->mCharacter))->GetParameter(0x11);
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
        mPage->SelectColorButton(color, 7, -1);
        for (u16 i = 0; i < 4; i++) {
            mPage->mButtons[sAdjustButtons[i]].mVisible = TRUE;
            mPage->mButtons[sAdjustButtons[i]].SetPositionFromVec(sAdjustButtonPositions[i].x << FX32_SHIFT,
                                                                  sAdjustButtonPositions[i].y << FX32_SHIFT);
        }
        break;
    }
    case 2: {
        u32 button = mPage->GetPartButton(((SavedCharacter *)(&mPage->mScene->mCharacter))->GetParameter(0x15), 7);
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
        mPage->SelectColorButton(0, 7, -1);
        for (u16 i = 0; i < 6; i++) {
            mPage->mButtons[sMoleAdjustButtons[i]].mVisible = TRUE;
            mPage->mButtons[sMoleAdjustButtons[i]].SetPositionFromVec(
                sMoleAdjustButtonPositions[i].x << FX32_SHIFT, sMoleAdjustButtonPositions[i].y << FX32_SHIFT);
        }
        break;
    }
    case 3: {
        u32 button = mPage->GetPartButton(((SavedCharacter *)(&mPage->mScene->mCharacter))->GetParameter(0x10), 7);
        button %= 4;
        int color = ((SavedCharacter *)(&mPage->mScene->mCharacter))->GetParameter(0x11);
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
        mPage->SelectColorButton(color, 7, -1);
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

extern "C" u16 KeyboardCore_GetChar(void); // The character typed (8 erases, 7 and 9 add the voicing marks, 11 makes it small)
BOOL func_02031294(SpriteButton *button);
wchar_t *wcscpy(wchar_t *dst, const wchar_t *src);
}

void MiiEditProfilePage::Init(MiiEditScene *scene) {
    mState = -1;
    mScene = scene;
    scene->MoveCameraTo(sProfileCameraGoal);
    SetState(1);
}

// The original reads the windows' choice and result (MessageWindow::mTouchedButton, mResult) anew at each use here,
// as it would a volatile. Declaring them volatile in MessageWindow.h does the same, but costs overlay 106's
// PreStartScene::Ask its match, so the volatile is in this file's reads only (a STAND-IN for whatever the
// original's declaration was).
#define READ(x) (*(volatile int *)&(x))

void MiiEditProfilePage::Update() {
    switch (mState) {
    case 0:
        if (mUnknown00004++ <= 2) {
            return;
        }
        SetState(mState + 1);
        return;
    case 2:
    case 4:
    case 6:
    case 8:
        if (!func_02024118(&data_0211d604, 0x3F00, 2)) {
            return;
        }
        SetState(mState + 1);
        return;
    case 1:
        if (!func_02024004(&data_0211d604, 0x3F00, -2)) {
            return;
        }
        for (int i = 0; i < 10; i++) {
            if (func_0203133c(&mButtons[i])) {
                switch (i) {
                case 0:
                    SetState(4);
                    break;
                case 1:
                    SetState(2);
                    break;
                case 2:
                    SetState(13);
                    break;
                case 3:
                    SetState(11);
                    break;
                case 4:
                    SetState(6);
                    break;
                case 5:
                    SetState(14);
                    break;
                case 6:
                    SetState(12);
                    break;
                case 7:
                    SetState(8);
                    break;
                case 8:
                    SetState(10);
                    break;
                case 9:
                    SetState(16);
                    break;
                }
            }
        }
        return;
    case 3:
    case 5:
    case 7:
    case 9: {
        mKeyboard.Update(0, gTouchInput.mTp);
        if (!func_02024004(&data_0211d604, 0x3F00, -2)) {
            return;
        }
        if (func_0203133c(&mButton)) {
            SetState(0);
        } else if (gTouchInput.mTp.touch != 0 || gTouchInput.mUnknown4C != 0 || data_0211f9d4.mUnknown20 == 0) {
            mKeyboard.TransferCharData();
        }
        if (!func_02031294(&mButton) && gTouchInput.mUnknown4C) {
            UpdateKeyboardKey(mState);
        }
        if (gTouchInput.mTouched) {
            ScreenPoint point = gTouchInput.mPoint;

            // The second is reached from the first by a cast, which the original's address computation shows
            if (!mKeyboard.mButtons[0].Contains(point) &&
                !((KeyboardButton *)((u8 *)&mKeyboard.mButtons + 0x6B8))->Contains(point)) {
                int x;
                u16 c = KeyboardCore_GetChar();
                if (c >= 0x20) {
                    if (mTextLength >= 10) {
                        return;
                    }
                    mText[mTextLength] = c;
                    x = mTextLength * 13 + 5;
                    const NNSG2dFont *font = mScene->mCharBox.mFont;
                    const NNSG2dCharWidths *widths = NNS_G2dFontGetCharWidths(font, c);
                    x -= widths->charWidth >> 1;
                    mScene->mCharBox.mCharX = x;
                    mScene->mCharBox.SetChar(c);
                    mScene->mCharBox.Redraw();
                    mTextLength++;
                    if (mTextLength >= 10) {
                        return;
                    }
                    mKeyboard.mLayoutSprite.mPosition.x += 0xD000;
                } else if (c == 8) {
                    if (mTextLength <= 0) {
                        return;
                    }
                    if (mTextLength < 10) {
                        mKeyboard.mLayoutSprite.mPosition.x -= 0xD000;
                    }
                    mTextLength--;
                    mText[mTextLength] = 0;
                    wchar_t text[10];
                    wcscpy(text, mText);
                    SetText(text);
                } else if (c == 7) {
                    int last = mTextLength - 1;
                    if (last < 0) {
                        return;
                    }
                    u16 ch = mText[last];
                    BOOL changed = FALSE;
                    // ka ki ku ke ko, KA to KO, sa to so, SA to SO, ta chi tsu te to, TA to TO, ha to ho, HA to HO
                    if (ch == 0x304B || ch == 0x304D || ch == 0x304F || ch == 0x3051 || ch == 0x3053 || ch == 0x30AB ||
                        ch == 0x30AD || ch == 0x30AF || ch == 0x30B1 || ch == 0x30B3 || ch == 0x3055 || ch == 0x3057 ||
                        ch == 0x3059 || ch == 0x305B || ch == 0x305D || ch == 0x30B5 || ch == 0x30B7 || ch == 0x30B9 ||
                        ch == 0x30BB || ch == 0x30BD || ch == 0x305F || ch == 0x3061 || ch == 0x3064 || ch == 0x3066 ||
                        ch == 0x3068 || ch == 0x30BF || ch == 0x30C1 || ch == 0x30C4 || ch == 0x30C6 || ch == 0x30C8 ||
                        ch == 0x306F || ch == 0x3072 || ch == 0x3075 || ch == 0x3078 || ch == 0x307B || ch == 0x30CF ||
                        ch == 0x30D2 || ch == 0x30D5 || ch == 0x30D8 || ch == 0x30DB) {
                        mText[last] = ch + 1;
                        changed = TRUE;
                    } else if (ch == 0x304C || ch == 0x304E || ch == 0x3050 || ch == 0x3052 || ch == 0x3054 ||
                               ch == 0x30AC || ch == 0x30AE || ch == 0x30B0 || ch == 0x30B2 || ch == 0x30B4 ||
                               ch == 0x3056 || ch == 0x3058 || ch == 0x305A || ch == 0x305C || ch == 0x305E ||
                               ch == 0x30B6 || ch == 0x30B8 || ch == 0x30BA || ch == 0x30BC || ch == 0x30BE ||
                               ch == 0x3060 || ch == 0x3062 || ch == 0x3065 || ch == 0x3067 || ch == 0x3069 ||
                               ch == 0x30C0 || ch == 0x30C2 || ch == 0x30C5 || ch == 0x30C7 || ch == 0x30C9 ||
                               ch == 0x3070 || ch == 0x3073 || ch == 0x3076 || ch == 0x3079 || ch == 0x307C ||
                               ch == 0x30D0 || ch == 0x30D3 || ch == 0x30D6 || ch == 0x30D9 || ch == 0x30DC) {
                        mText[last] = ch - 1;
                        changed = TRUE;
                    } else if (ch == 0x30A6) { // U, VU
                        mText[last] = 0x30F4;
                        changed = TRUE;
                    } else if (ch == 0x30F4) {
                        mText[last] = 0x30A6;
                        changed = TRUE;
                    }
                    if (!changed) {
                        return;
                    }
                    wchar_t text[10];
                    wcscpy(text, mText);
                    SetText(text);
                } else if (c == 9) {
                    int last = mTextLength - 1;
                    if (last < 0) {
                        return;
                    }
                    u16 ch = mText[last];
                    BOOL changed = FALSE;
                    // ha hi fu he ho, HA to HO
                    if (ch == 0x306F || ch == 0x3072 || ch == 0x3075 || ch == 0x3078 || ch == 0x307B || ch == 0x30CF ||
                        ch == 0x30D2 || ch == 0x30D5 || ch == 0x30D8 || ch == 0x30DB) {
                        mText[last] = ch + 2;
                        changed = TRUE;
                    } else if (ch == 0x3071 || ch == 0x3074 || ch == 0x3077 || ch == 0x307A || ch == 0x307D ||
                               ch == 0x30D1 || ch == 0x30D4 || ch == 0x30D7 || ch == 0x30DA || ch == 0x30DD) {
                        mText[last] = ch - 2;
                        changed = TRUE;
                    }
                    if (!changed) {
                        return;
                    }
                    wchar_t text[10];
                    wcscpy(text, mText);
                    SetText(text);
                } else if (c == 11) {
                    int last = mTextLength - 1;
                    if (last < 0) {
                        return;
                    }
                    u16 ch = mText[last];
                    BOOL changed = FALSE;
                    // a i u e o, A to O, tsu, TSU, ya, YA, yu, YU, yo, YO, wa, WA
                    if (ch == 0x3042 || ch == 0x3044 || ch == 0x3046 || ch == 0x3048 || ch == 0x304A || ch == 0x30A2 ||
                        ch == 0x30A4 || ch == 0x30A6 || ch == 0x30A8 || ch == 0x30AA || ch == 0x3064 || ch == 0x30C4 ||
                        ch == 0x3084 || ch == 0x30E4 || ch == 0x3086 || ch == 0x30E6 || ch == 0x3088 || ch == 0x30E8 ||
                        ch == 0x308F || ch == 0x30EF) {
                        mText[last] = ch - 1;
                        changed = TRUE;
                    } else if (ch == 0x3041 || ch == 0x3043 || ch == 0x3045 || ch == 0x3047 || ch == 0x3049 ||
                               ch == 0x30A1 || ch == 0x30A3 || ch == 0x30A5 || ch == 0x30A7 || ch == 0x30A9 ||
                               ch == 0x3063 || ch == 0x30C3 || ch == 0x3083 || ch == 0x30E3 || ch == 0x3085 ||
                               ch == 0x30E5 || ch == 0x3087 || ch == 0x30E7 || ch == 0x308E || ch == 0x30EE) {
                        mText[last] = ch + 1;
                        changed = TRUE;
                    } else if (ch == 0x30F5) { // Small KA, KA
                        mText[last] = 0x30AB;
                        changed = TRUE;
                    } else if (ch == 0x30AB) {
                        mText[last] = 0x30F5;
                        changed = TRUE;
                    } else if (ch == 0x30F6) { // Small KE, KE
                        mText[last] = 0x30B1;
                        changed = TRUE;
                    } else if (ch == 0x30B1) {
                        mText[last] = 0x30F6;
                        changed = TRUE;
                    }
                    if (!changed) {
                        return;
                    }
                    wchar_t text[10];
                    wcscpy(text, mText);
                    SetText(text);
                }
                return;
            }
        }
        if (!mKeyboard.mButtons[0].Update()) {
            return;
        }
        switch (mState) {
        case 7:
            MiiData_SetName(&mScene->mCharacter, mText);
            break;
        case 3:
            wcscpy(mScene->mCharacter.mName, mText);
            break;
        case 5:
            wcscpy(mScene->mCharacter.mFamilyName, mText);
            break;
        case 9:
            MiiData_SetCreatorName(&mScene->mCharacter, mText);
            break;
        }
        SetState(0);
        return;
    }
    case 11: {
        func_020341d4(&mWindows[2]);
        int result = READ(mWindows[2].mResult);
        if (result != -1) {
            switch (result) {
            case 0: {
                int year = mScene->mCharacter.mBirthYear + 1;
                if (year > 2099) {
                    year = 1900;
                }
                SetNumberDigits(year);
                ((SavedCharacter *)(&mScene->mCharacter))->SetBirthYear(year);
                UpdateDayButton();
                break;
            }
            case 1: {
                int year = mScene->mCharacter.mBirthYear - 1;
                if (year < 1900) {
                    year = 2099;
                }
                SetNumberDigits(year);
                ((SavedCharacter *)(&mScene->mCharacter))->SetBirthYear(year);
                UpdateDayButton();
                break;
            }
            }
        }
        if (READ(mWindows[2].mResult) != -1) {
            switch (READ(mWindows[2].mResult)) {
            case 2:
                ((SavedCharacter *)(&mScene->mCharacter))->SetBirthYear(0xFFF);
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
            SetState(1);
        }
        return;
    }
    case 12: {
        func_020341d4(&mWindows[3]);
        int result = READ(mWindows[3].mResult);
        if (result != -1) {
            switch (result) {
            case 0: {
                int month = ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x26) + 1;
                if (month > 12) {
                    month = 1;
                }
                SetMonthDigits(month);
                ((SavedCharacter *)(&mScene->mCharacter))->SetParameter(0x26, month);
                int min, max;
                ((SavedCharacter *)(&mScene->mCharacter))->GetParameterRange(0x27, &min, &max);
                int day = ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x27);
                if (day > max) {
                    day = max;
                }
                SetDayDigits(day);
                ((SavedCharacter *)(&mScene->mCharacter))->SetParameter(0x27, day);
                break;
            }
            case 1: {
                int month = ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x26) - 1;
                if (month < 1) {
                    month = 12;
                }
                SetMonthDigits(month);
                ((SavedCharacter *)(&mScene->mCharacter))->SetParameter(0x26, month);
                int min, max;
                ((SavedCharacter *)(&mScene->mCharacter))->GetParameterRange(0x27, &min, &max);
                int day = ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x27);
                if (day > max) {
                    day = max;
                }
                SetDayDigits(day);
                ((SavedCharacter *)(&mScene->mCharacter))->SetParameter(0x27, day);
                break;
            }
            case 2: {
                int min, max;
                ((SavedCharacter *)(&mScene->mCharacter))->GetParameterRange(0x27, &min, &max);
                int day = ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x27) + 1;
                if (day > max) {
                    day = 1;
                }
                SetDayDigits(day);
                ((SavedCharacter *)(&mScene->mCharacter))->SetParameter(0x27, day);
                break;
            }
            case 3: {
                int min, max;
                ((SavedCharacter *)(&mScene->mCharacter))->GetParameterRange(0x27, &min, &max);
                int day = ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x27) - 1;
                if (day < 1) {
                    day = max;
                }
                SetDayDigits(day);
                ((SavedCharacter *)(&mScene->mCharacter))->SetParameter(0x27, day);
                break;
            }
            }
        }
        if (READ(mWindows[3].mResult) != -1) {
            switch (READ(mWindows[3].mResult)) {
            case 4:
                ((SavedCharacter *)(&mScene->mCharacter))->SetParameter(0x26, 0);
                ((SavedCharacter *)(&mScene->mCharacter))->SetParameter(0x27, 0);
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
            SetState(1);
        }
        return;
    }
    case 13: {
        func_020341d4(&mWindows[0]);
        int i;
        int choice;
        if (READ(mWindows[0].mTouchedButton) != -1 && (choice = READ(mWindows[0].mTouchedButton)) != 4) {
            switch (choice) {
            case 0:
                ((SavedCharacter *)(&mScene->mCharacter))->SetBloodType(0);
                break;
            case 1:
                ((SavedCharacter *)(&mScene->mCharacter))->SetBloodType(1);
                break;
            case 2:
                ((SavedCharacter *)(&mScene->mCharacter))->SetBloodType(2);
                break;
            case 3:
                ((SavedCharacter *)(&mScene->mCharacter))->SetBloodType(3);
                break;
            case 4:
                ((SavedCharacter *)(&mScene->mCharacter))->SetBloodType(4);
                break;
            }
            for (i = 0; i < 4; i++) {
                if (i == choice) {
                    Sprite_SetSequence(&mWindows[0].mSprites[i], (u16)(sChoiceSequences[i] + 1), TRUE);
                } else {
                    Sprite_SetSequence(&mWindows[0].mSprites[i], sChoiceSequences[i], TRUE);
                }
            }
            mWindows[0].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindows[0], 0x24);
        }
        if (READ(mWindows[0].mResult) != -1 && READ(mWindows[0].mResult) == 4) {
            ((SavedCharacter *)(&mScene->mCharacter))->SetBloodType(4);
            mWindows[0].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindows[0], 0x24);
        }
        if (mWindows[0].mTransitionDone) {
            SetState(1);
        }
        return;
    }
    case 14:
        func_020341d4(&mWindows[1]);
        if (mWindows[1].mUnknownAF88) {
            SetState(15);
        }
        return;
    case 15: {
        func_020341d4(&mWindows[1]);
        int i;
        int choice;
        if (READ(mWindows[1].mTouchedButton) != -1 && (choice = READ(mWindows[1].mTouchedButton)) < 12) {
            if (choice >= 0 && choice < 12) {
                ((SavedCharacter *)(&mScene->mCharacter))->SetParameter(0x28, choice);
                ((Character *)(&mScene->mCharacter))->UpdateBodyColors();
                mScene->LoadModel();
            }
            for (i = 0; i < 12; i++) {
                if (i == choice) {
                    Sprite_SetSequence(&mWindows[1].mSprites[i], (u16)(sChoiceSequences[i + 4] + 1), TRUE);
                } else {
                    Sprite_SetSequence(&mWindows[1].mSprites[i], sChoiceSequences[i + 4], TRUE);
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
            SetState(1);
        }
        return;
    }
    case 10: {
        func_020341d4(&mWindows[4]);
        if (READ(mWindows[4].mTouchedButton) != -1) {
            int i;
            int choice = READ(mWindows[4].mTouchedButton);
            switch (choice) {
            case 0:
                mScene->mCharacter.mRelationBlood = 2;
                break;
            case 1:
                mScene->mCharacter.mRelationBlood = 3;
                break;
            case 2:
                mScene->mCharacter.mRelationBlood = 4;
                break;
            case 3:
                mScene->mCharacter.mRelationBlood = 5;
                break;
            case 4:
                mScene->mCharacter.mRelationBlood = 0;
                break;
            }
            for (i = 0; i < 5; i++) {
                if (i == choice) {
                    Sprite_SetSequence(&mWindows[4].mButtons[i], (u16)(sChoiceSequences[i + 24] + 1), TRUE);
                } else {
                    Sprite_SetSequence(&mWindows[4].mButtons[i], sChoiceSequences[i + 24], TRUE);
                }
            }
            mWindows[4].mUnknownSetBeforeClose = 15;
            func_020349cc(&mWindows[4], 0x24);
        }
        if (mWindows[4].mTransitionDone) {
            SetState(1);
        }
        return;
    }
    case 16: {
        func_020341d4(&mWindows[5]);
        int i;
        int choice;
        if (READ(mWindows[5].mTouchedButton) != -1 && (choice = READ(mWindows[5].mTouchedButton)) < 2) {
            switch (choice) {
            case 0:
                ((SavedCharacter *)(&mScene->mCharacter))->SetSex(0);
                mScene->LoadModel();
                break;
            case 1:
                ((SavedCharacter *)(&mScene->mCharacter))->SetSex(1);
                mScene->LoadModel();
                break;
            }
            for (i = 0; i < 2; i++) {
                if (i == choice) {
                    Sprite_SetSequence(&mWindows[5].mSprites[i], (u16)(sChoiceSequences[i + 16] + 1), TRUE);
                } else {
                    Sprite_SetSequence(&mWindows[5].mSprites[i], sChoiceSequences[i + 16], TRUE);
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
            SetState(1);
        }
        return;
    }
    }
}

extern "C" {
void func_02024c14(u32 *manager, void *a, int planes);
void func_02023f4c(ScreenControl *screens, int planes, int a);
void func_02033bb4(MessageWindow *window, int a, int sequence, WindowPoint position); // Adds a sprite
void func_02032140(SpriteButton *button, int a, int b, int c, int d); // Its four sounds
// The keyboard




size_t wcslen(const wchar_t *s);
}

// Sets the page's state up: 0 leaves the keyboard or a window, 1 the profile's buttons and the six windows, 3, 5,
// 7 and 9 the keyboard for one of the four texts, 10 to 16 open a window.
// NOT MATCHING (99.97%): the instructions are the original's, but six stack slots are not. The original keeps the
// WindowPoints of the windows' OK buttons (okSize, okPosition, clearPosition, an unread second copy of the size,
// yearSize, dateSize, from sp+0x64 down) below every other local; here okSize and clearPosition get slots above
// the inline copies, and the unread copy is missing.
void MiiEditProfilePage::SetState(int state) {
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
        mScene->ShowControls();
        func_02027534(&data_021213b8, "E00:/Obj_down.ncl", 0x1000, -1, -1, -1);
        ObjResourceManager_Load(&gObjResourceManager, "E00:/Profile.nce", "E00:/Profile.ncg", 0x1000, 1, 0x10000);
        for (u16 i = 0; i < 10; i++) {
            u16 sequence = sProfileButtonSequences[i];
            Sprite_Init(&mButtons[i], 1, sequence, 0x80, 0x60, TRUE);
            func_02031eb0(&mButtons[i], sProfileButtonSizes[i].x, sProfileButtonSizes[i].y);
            func_02031278(&mButtons[i], sequence, sequence + 1);
            func_02031260(&mButtons[i], sProfileButtonKinds[i]);
            func_02032158(&mButtons[i], &sProfileButtonSounds[i]);
            mButtons[i].mUnknownDC = 14;
            mButtons[i].SetPositionFromVec(sProfileButtonPositions[i].x << FX32_SHIFT,
                                           sProfileButtonPositions[i].y << FX32_SHIFT);
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
        if (!((SavedCharacter *)(&mScene->mCharacter))->IsMiiFromThisConsole()) {
            func_02031e88(&mButtons[4], TRUE);
            func_02031e88(&mButtons[5], TRUE);
            func_02031e88(&mButtons[7], TRUE);
            int month = ((SavedCharacter *)(&mScene->mCharacter))->GetRawParameter(0x26);
            int day = ((SavedCharacter *)(&mScene->mCharacter))->GetRawParameter(0x27);
            if (month != 0 && day != 0) {
                func_02031e88(&mButtons[6], TRUE);
            }
        }
        ScreenRect rect = sWindowRect;
        for (int i = 0; i < 6; i++) {
            func_02032fb8(&mWindows[i], rect);
            func_02033174(&mWindows[i], sWindowScreenName, 0x100);
            func_020331bc(&mWindows[i], 0x3F00, rect);
            func_02034f4c(&mWindows[i], 1);
        }
        int n = 0;
        for (int i = 0; i <= 3; i++) {
            u16 sequence = sChoiceSequences[i];
            func_02033448(&mWindows[0], n, 1, (u8)sequence, sequence + 1, sChoicePositions[i], sChoiceSizes[i],
                          &sChoiceSounds[i], 14);
            func_02031260((SpriteButton *)&mWindows[0].mSprites[n], sChoiceKinds[i]);
            n++;
        }
        WindowPoint okPosition = data_ov076_021587a8;
        WindowPoint okSize = data_ov076_021587c0;
        func_02033844(&mWindows[0], n, 0, 0x69, 0x6A, okPosition, okSize, &mScene->mMessageFiles[1], 7,
                      &gFonts[2].mFont, 0x1000, &charOffset, &data_ov076_02158d94[2], 14);
        n = 0;
        for (int i = 4; i <= 15; i++) {
            u16 sequence = sChoiceSequences[i];
            func_02033448(&mWindows[1], n, 1, (u8)sequence, sequence + 1, sChoicePositions[i], sChoiceSizes[i],
                          &sChoiceSounds[i], 14);
            func_02031260((SpriteButton *)&mWindows[1].mSprites[n], sChoiceKinds[i]);
            n++;
        }
        WindowPoint okPosition1 = data_ov076_021587a8;
        func_02033844(&mWindows[1], n, 0, 0x69, 0x6A, okPosition1, okSize, &mScene->mMessageFiles[1], 5,
                      &gFonts[2].mFont, 0x1000, &charOffset, &data_ov076_02158d94[2], 14);
        n = 0;
        for (int i = 18; i <= 19; i++) {
            u16 sequence = sChoiceSequences[i];
            func_02033448(&mWindows[2], n, 1, (u8)sequence, sequence + 1, sChoicePositions[i], sChoiceSizes[i],
                          &sChoiceSounds[i], 14);
            func_02031260((SpriteButton *)&mWindows[2].mSprites[n], 1);
            mWindows[2].mSprites[n].mUnknownE0 = 6;
            func_0203119c((SpriteButton *)&mWindows[2].mSprites[n]);
            mWindows[2].mSprites[n].mVisible = FALSE;
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
            mWindows[2].mPlainSprites[i].mOverwritePalette = 0;
            NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&mWindows[2].mPlainSprites[i].mAnim));
            mWindows[2].mPlainSprites[i].mVisible = FALSE;
        }
        func_02033bb4(&mWindows[2], 1, 0x4B, data_ov076_021587b8);
        n = 0;
        for (int i = 20; i <= 23; i++) {
            u16 sequence = sChoiceSequences[i];
            func_02033448(&mWindows[3], n, 1, (u8)sequence, sequence + 1, sChoicePositions[i], sChoiceSizes[i],
                          &sChoiceSounds[i], 14);
            func_02031260((SpriteButton *)&mWindows[3].mSprites[n], 1);
            mWindows[3].mSprites[n].mUnknownE0 = 6;
            func_0203119c((SpriteButton *)&mWindows[3].mSprites[n]);
            mWindows[3].mSprites[n].mVisible = FALSE;
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
            mWindows[3].mPlainSprites[i].mOverwritePalette = 0;
            NNS_G2dStopAnimCtrl(NNS_G2dGetCellAnimationAnimCtrl(&mWindows[3].mPlainSprites[i].mAnim));
            mWindows[3].mPlainSprites[i].mVisible = FALSE;
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
            u16 sequence = sChoiceSequences[i];
            func_02033844(&mWindows[4], n, 1, sequence, sequence + 1, sChoicePositions[i], sChoiceSizes[i],
                          &mScene->mMessageFiles[0], value + 0x54, &gFonts[0].mFont, 0x1000, &charOffset,
                          &sChoiceSounds[i], 14);
            func_02031260((SpriteButton *)&mWindows[4].mButtons[i - 24], sChoiceKinds[i - 24]);
            mWindows[4].mButtons[i - 24].mMoveText = FALSE;
            n++;
        }
        n = 0;
        for (int i = 16; i <= 17; i++) {
            u16 sequence = sChoiceSequences[i];
            func_02033448(&mWindows[5], n, 1, (u8)sequence, sequence + 1, sChoicePositions[i], sChoiceSizes[i],
                          &sChoiceSounds[i], 14);
            func_02031260((SpriteButton *)&mWindows[5].mSprites[n], sChoiceKinds[i]);
            n++;
        }
        WindowPoint okPosition5 = data_ov076_021587a8;
        func_02033844(&mWindows[5], n, 0, 0x69, 0x6A, okPosition5, okSize, &mScene->mMessageFiles[1], 5,
                      &gFonts[2].mFont, 0x1000, &charOffset, &data_ov076_02158d94[2], 14);
        mScene->mText1B35C[0].mSprite.mVisible = FALSE;
        UpdateTexts();
        MiiData_GetName(&mScene->mCharacter, name);
        MiiData_GetCreatorName(&mScene->mCharacter, creator);
        BOOL complete;
        if (mScene->mUnknown5824 == 0) {
            if (mScene->GetNameLength(mScene->mCharacter.mFamilyName) &&
                mScene->GetNameLength(mScene->mCharacter.mName) &&
                mScene->GetNameLength(name) && mScene->mCharacter.mBirthYear != 0xFFF &&
                ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x26) != 0 && ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x27) != 0) {
                complete = TRUE;
            } else {
                complete = FALSE;
            }
        } else {
            if (mScene->GetNameLength(mScene->mCharacter.mFamilyName) &&
                mScene->GetNameLength(mScene->mCharacter.mName) &&
                mScene->GetNameLength(name) &&
                (wcslen(creator) == 0 || mScene->GetNameLength(creator))) {
                complete = TRUE;
            } else {
                complete = FALSE;
            }
        }
        if (complete) {
            int place = mScene->mProgressSprite.mPlace;
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
                mScene->mProgressSprite.AdvanceTo(2);
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
        mScene->HideControls();
        MI_CpuClear16(mText, sizeof(mText));
        mTextLength = 0;
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
        mKeyboard.SetArchiveName("KBD");
        mKeyboard.InitBg(0x100, 0);
        mKeyboard.SetMode(0, 0);
        mKeyboard.InitObj(3, &mScene->mMessageFiles[1], 0x384, 0x14000, 0, 13);
        mKeyboard.EnableTabs();
        mKeyboard.GetButtonCharCount();
        mKeyboard.SetButtonSounds(0, 1, 14, 0, 20);
        ((KeyboardButton *)((u8 *)&mKeyboard.mButtons + 0x6B8))->SetVisible(FALSE);
        switch (state) {
        case 3: {
            mScene->mText1B35C[0].mSprite.mVisible = TRUE;
            NNS_G2dCharCanvasClear(&mScene->mText1B35C[0].mCanvas, 0);
            mScene->mText1B35C[0].mMessage = 0x1F;
            mScene->mText1B35C[0].DrawAll();
            SetCanvasPosition(&mScene->mCharBox, sCharBoxPosition);
            SetText(mScene->mCharacter.mName);
            break;
        }
        case 5: {
            mScene->mText1B35C[0].mSprite.mVisible = TRUE;
            NNS_G2dCharCanvasClear(&mScene->mText1B35C[0].mCanvas, 0);
            mScene->mText1B35C[0].mMessage = 0x1E;
            mScene->mText1B35C[0].DrawAll();
            SetCanvasPosition(&mScene->mCharBox, sCharBoxPosition);
            SetText(mScene->mCharacter.mFamilyName);
            break;
        }
        case 7: {
            mScene->mText1B35C[0].mSprite.mVisible = TRUE;
            NNS_G2dCharCanvasClear(&mScene->mText1B35C[0].mCanvas, 0);
            mScene->mText1B35C[0].mMessage = 0x20;
            mScene->mText1B35C[0].DrawAll();
            SetCanvasPosition(&mScene->mCharBox, sCharBoxPosition);
            MiiData_GetName(&mScene->mCharacter, text);
            SetText(text);
            break;
        }
        case 9: {
            mScene->mText1B35C[0].mSprite.mVisible = TRUE;
            NNS_G2dCharCanvasClear(&mScene->mText1B35C[0].mCanvas, 0);
            mScene->mText1B35C[0].mMessage = 0x24;
            mScene->mText1B35C[0].DrawAll();
            SetCanvasPosition(&mScene->mCharBox, sCharBoxPosition);
            MiiData_GetCreatorName(&mScene->mCharacter, text);
            SetText(text);
            break;
        }
        }
        UpdateKeyboardKey(state);
        int x = mTextLength * 13;
        x = CLAMP(x, 0, 0x75);
        mKeyboard.mLayoutSprite.mPosition.x += x << FX32_SHIFT;
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
        if (0xFFF == mScene->mCharacter.mBirthYear) {
            ((SavedCharacter *)(&mScene->mCharacter))->SetBirthYear(1990);
        }
        SetNumberDigits(mScene->mCharacter.mBirthYear);
        UpdateDayButton();
        break;
    case 12:
        func_02034720(&mWindows[3], 0x12, 1);
        if (((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x26) == 0 || ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x27) == 0) {
            ((SavedCharacter *)(&mScene->mCharacter))->SetParameter(0x26, 1);
            ((SavedCharacter *)(&mScene->mCharacter))->SetParameter(0x27, 1);
        }
        SetMonthDigits(((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x26));
        SetDayDigits(((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x27));
        break;
    case 13: {
        func_02034720(&mWindows[0], 0x12, 1);
        int choice = mScene->mCharacter.mBloodType;
        if (choice >= 0 && choice <= 3) {
            Sprite_SetSequence(&mWindows[0].mSprites[choice], (u16)(sChoiceSequences[choice] + 1), TRUE);
        }
        break;
    }
    case 14: {
        func_02034720(&mWindows[1], 0x12, 1);
        int color = ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x28);
        if (color >= 0 && color <= 11) {
            Sprite_SetSequence(&mWindows[1].mSprites[color], (u16)(sChoiceSequences[color + 4] + 1), TRUE);
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
        int value = mScene->mCharacter.mRelationBlood;
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
        Sprite_SetSequence(&mWindows[4].mButtons[choice], (u16)(sChoiceSequences[choice + 24] + 1), TRUE);
        break;
    }
    case 16: {
        func_02034720(&mWindows[5], 0x12, 1);
        int sex = MiiData_GetSex(&mScene->mCharacter);
        if (sex >= 0 && sex <= 1) {
            Sprite_SetSequence(&mWindows[5].mSprites[sex], (u16)(sChoiceSequences[sex + 16] + 1), TRUE);
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
    mScene->mText1B35C[0].ObjTextCanvas::Draw();
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
        mScene->mText1B35C[0].ObjTextCanvas::Draw();
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
        mScene->ShowControls();
        break;
    }
}

// What the profile page uses of the main program (stand-ins)
extern "C" {
void ObjResourceManager_Load(CellLoader *loader, const char *cellName, const char *charName, int plane, int a, int b);
void func_02031e88(SpriteButton *button, BOOL disabled); // Sets mUnknownB8, and draws the button darkened while it is set
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

// Writes the character's profile into the scene's nine text boxes, each centred on its place.
// 99.0%: the original loads the scene into r0 before reading mRelationBlood (ours r1), and holds the two tables of
// the last loop in each other's registers. Tried: the relation through a local (int, u32), a MessageFile pointer,
// the operands swapped; the index or the position through locals, the tables indexed at each use (which gives the
// registers but moves -width before the position's address, and scores lower), the point through an inline,
// -width / 2 in other forms; pragma sets (propagation, common subs, lifetimes, level 2).
void MiiEditProfilePage::UpdateTexts() {
    u32 number = mScene->mCharacter.mBirthYear;
    if (number == 0xFFF) {
        swprintf(NUMBER_TEXT(mScene), 5, L"", number);
    } else {
        swprintf(NUMBER_TEXT(mScene), 5, L"%d", number);
        ToFullWidthDigits(NUMBER_TEXT(mScene));
    }
    int month = ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x26);
    int day = ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x27);
    if (month >= 1 && month <= 12 && day >= 1 && day <= 31) {
        swprintf(BIRTHDAY_TEXT(mScene), 6, L"%d/%d", month, day);
        ToFullWidthDigits(BIRTHDAY_TEXT(mScene));
    } else {
        swprintf(BIRTHDAY_TEXT(mScene), 6, L"");
    }
    mScene->mTextBoxes[0].SetText((const NNSG2dChar *)mScene->mCharacter.mFamilyName);
    mScene->mTextBoxes[1].SetText((const NNSG2dChar *)mScene->mCharacter.mName);
    MiiData_GetName(&mScene->mCharacter, NAME_TEXT(mScene));
    mScene->mTextBoxes[2].SetText((const NNSG2dChar *)NAME_TEXT(mScene));
    mScene->mTextBoxes[8].SetText((const NNSG2dChar *)data_ov076_0215a10c[MiiData_GetSex(&mScene->mCharacter)]);
    mScene->mTextBoxes[3].SetText(
        (const NNSG2dChar *)mScene->mMessageFiles[0].GetText(mScene->mCharacter.mBloodType + 0x5A));
    mScene->mTextBoxes[4].SetText((const NNSG2dChar *)NUMBER_TEXT(mScene));
    mScene->mTextBoxes[5].SetText((const NNSG2dChar *)BIRTHDAY_TEXT(mScene));
    MiiData_GetCreatorName(&mScene->mCharacter, CREATOR_NAME_TEXT(mScene));
    mScene->mTextBoxes[6].SetText((const NNSG2dChar *)CREATOR_NAME_TEXT(mScene));
    mScene->mTextBoxes[7].SetText(
        (const NNSG2dChar *)mScene->mMessageFiles[0].GetText(mScene->mCharacter.mRelationBlood + 0x54));
    if (!mScene->mCharacter.IsMiiFromThisConsole()) {
        mScene->mTextBoxes[2].mColor = 9;
        mScene->mTextBoxes[6].mColor = 9;
        BOOL monthSet = ((SavedCharacter *)(&mScene->mCharacter))->GetRawParameter(0x26);
        BOOL daySet = ((SavedCharacter *)(&mScene->mCharacter))->GetRawParameter(0x27);
        if (monthSet && daySet) {
            mScene->mTextBoxes[5].mColor = 9;
        }
    }
    for (u16 i = 0; i < 9; i++) {
        mScene->mUnknown1B32C = 1;
        int width = NNS_G2dTextCanvasGetTextWidth(&mScene->mTextBoxes[i].mTextCanvas, mScene->mTextBoxes[i].mText);
        const WindowPoint *position = &sProfileButtonPositions[data_ov076_02158b3c[i]];
        WindowPoint point;
        point.x = position->x + -width / 2;
        point.y = position->y - 7;
        SetTextBoxPosition(&mScene->mTextBoxes[i], point);
    }
    Sprite_SetFrame(&mSprites[0], ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x28));
}

// Darkens the second button of the third window while the birth day is past the last day the month has
void MiiEditProfilePage::UpdateDayButton() {
    int min, max;
    ((SavedCharacter *)(&mScene->mCharacter))->GetParameterRange(0x27, &min, &max);
    if (((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x27) > max) {
        func_02031e88((SpriteButton *)&mWindows[2].mButtons[1], TRUE);
    } else {
        func_02031e88((SpriteButton *)&mWindows[2].mButtons[1], FALSE);
    }
}

// Enables the keyboard's first key in the states that type a text, when GetNameLength accepts the text
// (state 9 also with no text at all)
void MiiEditProfilePage::UpdateKeyboardKey(int state) {
    BOOL enabled = FALSE;
    switch (state) {
    case 3:
    case 5:
    case 7:
        enabled = mScene->GetNameLength(mText) ? TRUE : FALSE;
        break;
    case 9:
        if (wcslen(mText) == 0 || mScene->GetNameLength(mText)) {
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
void func_0209d898(SceneCharacter *character, TtsPlayer *player);
}
// The reading of a number, in the synthesizer's notation (src/scene/SceneHelpers.cpp)
void MakeNumberReading(wchar_t *text, int number);

// The scene's buffer of the text to speak, in Shift JIS (0x102 bytes)
#define SPEECH_TEXT(scene) ((char *)&(scene)->mUnknownAfterTts[0x1A264 - 0x5920 - sizeof(TtsPlayer)])

void MiiEditVoicePage::Init(MiiEditScene *scene) {
    mState = -1;
    mScene = scene;
    func_02027534(&data_021213b8, "E00:/Obj_down.ncl", 0x1000, -1, -1, -1);
    func_02027d7c(&data_02121834, "E00:/Window_down.nsc", 0x100, 0, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, "E00:/BG_down_editface.nsc", 0x200, 0, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, "E00:/BG_down.nsc", 0x400, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "E00:/BG_down.ncg", 0x2F00, 0, 0, 0, -1);
    func_02027534(&data_021213b8, "E00:/BG_down.ncl", 0x2F00, -1, -1, -1);
    mScene->ShowControls();
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
    mScene->MoveCameraTo(sProfileCameraGoal);
    MakeCharacterSpeechTexts(&mSpeechTexts, &mScene->mCharacter);
    mText.mMessageFile = &mScene->mMessageFiles[0];
    int voice = mScene->mCharacter.mUnknown054Bits0;
    if (voice >= 0 && voice <= 7) {
        StartSpeaking();
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

void MiiEditVoicePage::Update() {
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
                        ((SavedCharacter *)(&mScene->mCharacter))->SetVoice(k, VOICE_PRESETS[i][k]);
                    }
                    ShowVoiceSettings();
                    StartSpeaking();
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
                        ((SavedCharacter *)(&mScene->mCharacter))->SetVoice(k, RandomInRange(data_020fb940[k].mMin, data_020fb940[k].mMax));
                    }
                    ShowVoiceSettings();
                    StartSpeaking();
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
                    StartSpeaking();
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
            StartSpeaking();
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
                mScene->mProgressSprite.AdvanceTo(3);
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
                (CharacterRecord &)mScene->mCharacter = mUndoRecord;
                ShowVoiceSettings();
                StartSpeaking();
                break;
            case 9:
                StartSpeaking();
                mButtons[9].mVisible = FALSE;
                mButtons[10].mVisible = TRUE;
                func_0203119c(&mButtons[9]);
                func_0203119c(&mButtons[10]);
                break;
            case 10:
                StopSpeaking();
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
                RememberForUndo();
                for (int k = 0; k < 6; k++) {
                    ((SavedCharacter *)(&mScene->mCharacter))->SetVoice(k, VOICE_PRESETS[i - 11][k]);
                }
                ShowVoiceSettings();
                StartSpeaking();
                break;
            case 17:
                RememberForUndo();
                for (int k = 0; k < 5; k++) {
                    ((SavedCharacter *)(&mScene->mCharacter))->SetVoice(k, RandomInRange(data_020fb940[k].mMin, data_020fb940[k].mMax));
                }
                ((SavedCharacter *)(&mScene->mCharacter))->SetVoice(5, MATH_Rand16(&gRandom, 4));
                ShowVoiceSettings();
                StartSpeaking();
                break;
            case 18:
            case 19:
            case 20:
            case 21:
                RememberForUndo();
                ((SavedCharacter *)(&mScene->mCharacter))->SetVoice(5, i - 18);
                for (int k = 18; k <= 21; k++) {
                    if (i != k) {
                        func_02031e28(&mButtons[k], FALSE);
                    }
                }
                StartSpeaking();
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
                ((SavedCharacter *)(&mScene->mCharacter))->SetVoice(i, value);
                StartSpeaking();
            }
            if (func_02032414(&mSprites2B2C[i])) {
                for (int k = 0; k < 5; k++) {
                    if (k != i) {
                        func_02032578(&mSprites2B2C[k], FALSE);
                    }
                }
            }
            if (mSprites2B2C[i].mUnknownC8) {
                RememberForUndo();
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
        SpeakProfile(mSpeakStep + 1);
        return;
    case 6:
        if (mScene->mTtsPlayer.IsEngineIdle() != TRUE) {
            return;
        }
        SpeakProfile(1);
        break;
    }
}

// Shows the page's controls for a state: 1 the first, 2 the second, 4 the sliders
void MiiEditVoicePage::SetState(int state) {
    switch (state) {
    case 1: {
        mScene->ShowControls();
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
        mScene->HideControlsNoBar();
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
    StopSpeaking();
    if (mState != 4) {
        return;
    }
    func_02027d7c(&data_02121834, "E00:/BG_down_editface.nsc", 0x200, 0, 0, 0, 0, -1);
    func_02027d7c(&data_02121834, "E00:/Window_down.nsc", 0x100, 0, 0, 0, 0, -1);
    func_02028f24(&gVramTransferQueue, "E00:/BG_down.ncg", 0x200, 0, 0, 0, 0);
    func_02027534(&data_021213b8, "E00:/BG_down.ncl", 0x200, -1, -1, -1);
    mScene->ShowControls();
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

void MiiEditVoicePage::RememberForUndo() {
    mUndoRecord = mScene->mCharacter;
    mButtons[8].mVisible = TRUE;
}

// Speaks a part of the character's profile with the scene's speech player, from the given step on: 0 stops, 1 the
// two texts, 2 the name, 3 the number and birthday, 4 the creator's name, 5 and 6 two messages. A step with nothing
// to say goes on to the next.
void MiiEditVoicePage::SpeakProfile(int step) {
    int length = 0x102;
    switch (step) {
    case 0:
        mScene->mTtsPlayer.StopEngine();
        ((CharacterBase *)(&mScene->mCharacter))->SetFace(8);
        break;
    case 1:
        if (wcslen(mSpeechTexts.mFamilyNameReading) + wcslen(mSpeechTexts.mGivenNameReading) == 0) {
            SpeakProfile(step + 1);
            return;
        }
        MI_CpuFill8(SPEECH_TEXT(mScene), 0, 0x102);
        mText.SetStringArg(mSpeechTexts.mFamilyNameReading, 0);
        mText.SetStringArg(mSpeechTexts.mGivenNameReading, 1);
        mText.mMessage = 0x2F;
        mText.ExpandMessage();
        STD_ConvertStringUnicodeToSjis(SPEECH_TEXT(mScene), &length, (const u16 *)mText.mText, NULL, NULL);
        mScene->mTtsPlayer.Speak(SPEECH_TEXT(mScene), 0);
        break;
    case 2:
        if (!mScene->GetNameLength(mSpeechTexts.mNameReading)) {
            SpeakProfile(step + 1);
            return;
        }
        MI_CpuFill8(SPEECH_TEXT(mScene), 0, 0x102);
        mText.SetStringArg(mSpeechTexts.mNameReading, 0);
        mText.mMessage = 0x36;
        mText.ExpandMessage();
        STD_ConvertStringUnicodeToSjis(SPEECH_TEXT(mScene), &length, (const u16 *)mText.mText, NULL, NULL);
        mScene->mTtsPlayer.Speak(SPEECH_TEXT(mScene), 0);
        break;
    case 3: {
        MI_CpuFill8(SPEECH_TEXT(mScene), 0, 0x102);
        u32 number = mScene->mCharacter.mBirthYear;
        int month = ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x26);
        int day = ((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x27);
        wchar_t numberText[32];
        wchar_t monthText[32];
        wchar_t dayText[32];
        MakeNumberReading(numberText, number);
        MakeNumberReading(monthText, month);
        MakeNumberReading(dayText, day);
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
            SpeakProfile(step + 1);
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
        MakeSpeechTextInPlace(creatorName, sizeof(creatorName));
        if (!mScene->GetNameLength(creatorName)) {
            SpeakProfile(step + 1);
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
        u32 value = mScene->mCharacter.mBloodType;
        if (value == 4) {
            SpeakProfile(step + 1);
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
        mText.SetMessageArg(mScene->mMessageFiles[0].GetText(((SavedCharacter *)(&mScene->mCharacter))->GetParameter(0x28) + 0x44), 0);
        mText.mMessage = 0x35;
        mText.ExpandMessage();
        STD_ConvertStringUnicodeToSjis(SPEECH_TEXT(mScene), &length, (const u16 *)mText.mText, NULL, NULL);
        mScene->mTtsPlayer.Speak(SPEECH_TEXT(mScene), 0);
        break;
    }
    mSpeakStep = step;
}

void MiiEditVoicePage::StartSpeaking() {
    func_0209d898(&mScene->mCharacter, &mScene->mTtsPlayer);
    SpeakProfile(1);
}

void MiiEditVoicePage::StopSpeaking() {
    SpeakProfile(0);
}

// The etc message of the star sign of a birthday (0x38 for the sign that begins in January, Aquarius): its table,
// sStarSignFirstDays, holds the first day of each sign in its month, from January's (21, 19, 21, 21, 22, ...)
int MiiEditVoicePage::GetStarSignMessage(int month, int day) {
    int sign;
    if (day >= sStarSignFirstDays[month - 1]) {
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
extern "C" {
extern TouchInput gTouchInput;
}

void MiiEditCharacterPage::Init(MiiEditScene *scene) {
    mScene = scene;
    scene->ShowControls();
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
        u8 value = mScene->mCharacter.mAnswers[i];
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
    mScene->MoveCameraTo(sProfileCameraGoal);
}

// Whether all five values are set
static inline BOOL IsPersonalitySet(MiiEditScene *scene) {
    BOOL set = TRUE;
    for (int i = 0; i < 5; i++) {
        if (scene->mCharacter.mAnswers[i] == 0) {
            set = FALSE;
            break;
        }
    }
    return set;
}

void MiiEditCharacterPage::Update() {
    if (gTouchInput.mTp.touch == 0) {
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
            if (func_02028d88(&rect, &point) && j + 1 != mScene->mCharacter.mAnswers[i]) {
                mScene->mCharacter.SetAnswer(i, j + 1);
                mScene->mCharacter.Character::UpdateAnimSpeed();
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
        if (mScene->mCharacter.mAnswers[i] == 0) {
            set = FALSE;
            break;
        }
    }
    if (!set) {
        return;
    }
    func_02031e88(&mScene->mButtons[4], FALSE);
    mScene->mUnknown58DC = TRUE;
    mScene->mProgressSprite.AdvanceTo(4);
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
void MiiEditRegisterPage::Init(MiiEditScene *scene, int previousPage) {
    mScene = scene;
    mPreviousPage = previousPage;
    ScreenRect rect = sWindowRect;
    int charOffset = 0x384;
    func_02032fb8(&mWindow[0], rect);
    func_02033174(&mWindow[0], "E00:/Window_down.nsc", 0x100);
    func_020331bc(&mWindow[0], 0x3F00, rect);
    func_02034f4c(&mWindow[0], 1);
    int number = 0;
    for (int i = 0; i <= 1; i++) {
        func_02033844(&mWindow[0], number, 0, data_ov076_0215874c[i], data_ov076_02158748[i],
                      data_ov076_021588fc[i], data_ov076_0215894c[i], &mScene->mMessageFiles[1],
                      data_ov076_021587f0[i], &gFonts[2].mFont, 0x1000, &charOffset, &data_ov076_02158a3c[i], 1);
        number++;
    }
    charOffset = func_02033a58(&mWindow[0], &mScene->mMessageFiles[0], 0x2D, &gFonts[0].mFont, 0x1000,
                               data_ov076_02158800, &charOffset);
    func_02034f4c(&mWindow[0], 1);
    func_02034f94(&mWindow[0], 1);
    func_02034720(&mWindow[0], MessageWindow::FLAG_OPEN | MessageWindow::FLAG_FROM_BELOW, 1);
}

void MiiEditRegisterPage::Update() {
    func_020341d4(&mWindow[0]);
    if (mWindow[0].mResult != -1) {
        mWindow[0].mUnknownSetBeforeClose = 15;
        func_020349cc(&mWindow[0], MessageWindow::FLAG_CLOSE | 0x20);
        switch (mWindow[0].mResult) {
        case 0:
            mNextPage = -1;
            break;
        case 1:
            mNextPage = mPreviousPage;
            break;
        }
    }
    if (!mWindow[0].mTransitionDone) {
        return;
    }
    if (mNextPage == -1) {
        mScene->SaveCharacter();
        switch (mScene->mSceneId) {
        case 0x42:
            gSceneManager.RequestScene(0x1A);
            break;
        case 0x43:
        case 0x44:
            gSceneMemory.mCharacters[0] = mScene->mUnknown5824;
            gSceneManager.RequestScene(0x74);
            break;
        case 0x6F:
            gSceneMemory.mCharacters[0] = mScene->mUnknown5824;
            gSceneManager.RequestScene(0x69);
            break;
        }
    } else {
        mScene->SetPage(mNextPage);
    }
}

void MiiEditRegisterPage::Draw() {
    func_020345d8(&mWindow[0]);
}

void MiiEditRegisterPage::Exit() {
    func_020346cc(&mWindow[0]);
}

void MiiEditQuitPage::Init(MiiEditScene *scene, int previousPage) {
    mScene = scene;
    mPreviousPage = previousPage;
    ScreenRect rect = sWindowRect;
    int charOffset = 0x384;
    func_02032fb8(&mWindow[0], rect);
    func_02033174(&mWindow[0], "E00:/Window_down.nsc", 0x100);
    func_020331bc(&mWindow[0], 0x3F00, rect);
    func_02034f4c(&mWindow[0], 1);
    int number = 0;
    for (int i = 0; i <= 1; i++) {
        func_02033844(&mWindow[0], number, 0, data_ov076_0215874c[i], data_ov076_02158748[i],
                      data_ov076_021588fc[i], data_ov076_0215894c[i], &mScene->mMessageFiles[1],
                      data_ov076_021587f0[i], &gFonts[2].mFont, 0x1000, &charOffset, &data_ov076_02158a1c[i], 1);
        number++;
    }
    func_02033a58(&mWindow[0], &mScene->mMessageFiles[0], 0x2E, &gFonts[0].mFont, 0x1000, data_ov076_02158800,
                  &charOffset);
    func_02034f4c(&mWindow[0], 1);
    func_02034f94(&mWindow[0], 1);
    func_02034720(&mWindow[0], MessageWindow::FLAG_OPEN | MessageWindow::FLAG_FROM_BELOW, 1);
}

void MiiEditQuitPage::Update() {
    func_020341d4(&mWindow[0]);
    if (mWindow[0].mResult != -1) {
        mWindow[0].mUnknownSetBeforeClose = 15;
        func_020349cc(&mWindow[0], MessageWindow::FLAG_CLOSE | 0x20);
        switch (mWindow[0].mResult) {
        case 0:
            mNextPage = -1;
            break;
        case 1:
            mNextPage = mPreviousPage;
            break;
        }
    }
    if (!mWindow[0].mTransitionDone) {
        return;
    }
    if (mNextPage == -1) {
        switch (mScene->mSceneId) {
        case 0x42:
        case 0x44:
            gSceneManager.RequestScene(0x1A);
            break;
        case 0x43:
            gSceneManager.RequestScene(0x5E);
            break;
        }
    } else {
        mScene->SetPage(mNextPage);
    }
}

void MiiEditQuitPage::Draw() {
    func_020345d8(&mWindow[0]);
}

void MiiEditQuitPage::Exit() {
    func_020346cc(&mWindow[0]);
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
    p[31] = &sCharBoxPosition;
    p[32] = &data_ov076_02158800;
    p[33] = &data_ov076_02158808;
    p[34] = &data_ov076_02158810;
    p[35] = &data_ov076_02158818;
    p[36] = &data_ov076_02158822;
    p[37] = &data_ov076_0215882c;
    p[38] = &data_ov076_02158838;
    p[39] = &data_ov076_02158844;
    p[40] = &data_ov076_02158850;
    p[41] = &sProfileCameraGoal;
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
    p[56] = &sMolePositions;
    p[57] = &sAdjustButtons;
    p[58] = &data_ov076_0215894c;
    p[59] = &sProfileButtonSequences;
    p[60] = &data_ov076_02158970;
    p[61] = &data_ov076_02158984;
    p[62] = &data_ov076_0215899c;
    p[63] = &sButtonKinds;
    p[64] = &data_ov076_021589cc;
    p[65] = &sMoleAdjustButtons;
    p[66] = &sAdjustButtonPositions;
    p[67] = &data_ov076_02158a1c;
    p[68] = &data_ov076_02158a3c;
    p[69] = &data_ov076_02158a5c;
    p[70] = &data_ov076_02158a7c;
    p[71] = &data_ov076_02158a9c;
    p[72] = &data_ov076_02158abc;
    p[73] = &data_ov076_02158adc;
    p[74] = &sMustachePositions;
    p[75] = &data_ov076_02158b1c;
    p[76] = &data_ov076_02158b3c;
    p[77] = &sProfileButtonKinds;
    p[78] = &sSpriteB0Places;
    p[79] = &data_ov076_02158bb0;
    p[80] = &data_ov076_02158bd8;
    p[81] = &data_ov076_02158c00;
    p[82] = &data_ov076_02158c28;
    p[83] = &data_ov076_02158c50;
    p[84] = &data_ov076_02158c78;
    p[85] = &sButtonSizes;
    p[86] = &data_ov076_02158cd4;
    p[87] = &sNoseTypes;
    p[88] = &sStarSignFirstDays;
    p[89] = &sMoleAdjustButtonPositions;
    p[90] = &data_ov076_02158d94;
    p[91] = &sButtonPositions;
    p[92] = &sChoiceSequences;
    p[93] = &sWindowScreenName;
    p[94] = &sEyebrowRotations;
    p[95] = &sEyeRotations;
    p[96] = &data_ov076_02158e94;
    p[97] = &sFaceTypePositions;
    p[98] = &data_ov076_02158f14;
    p[99] = &data_ov076_02158f54;
    p[100] = &sGlassesPositions;
    p[101] = &sProfileButtonPositions;
    p[102] = &sProfileButtonSizes;
    p[103] = &data_ov076_02159084;
    p[104] = &sButtonSounds;
    p[105] = &sEyebrowTypes;
    p[106] = &sMouthTypes;
    p[107] = &sFaceTexturePositions;
    p[108] = &sChoiceKinds;
    p[109] = &data_ov076_021592d0;
    p[110] = &data_ov076_02159350;
    p[111] = &sProfileButtonSounds;
    p[112] = &data_ov076_02159480;
    p[113] = &data_ov076_02159530;
    p[114] = &sEyeTypes;
    p[115] = &sChoicePositions;
    p[116] = &sChoiceSizes;
    p[117] = &sHairTypes;
    p[118] = &data_ov076_02159990;
    p[119] = &data_ov076_02159ab0;
    p[120] = &sChoiceSounds;
    p[121] = &data_ov076_02159de0;
}

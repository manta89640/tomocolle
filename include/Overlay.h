#ifndef OVERLAY_H
#define OVERLAY_H

#include <nitro.h>

// Overlay ids as the code has them: the address of a symbol that the linker script sets to the id (dsd's
// OVERLAY_<n>_ID), so that each is a relocation of kind overlay_id in relocs.txt, as the SDK's FS_OVERLAY_ID
// makes them. Pass OVERLAY_ID(n) to FS_LoadOverlay and FS_UnloadOverlay.

#define OVERLAY_ID(n) ((FSOverlayID)OVERLAY_##n##_ID)

extern "C" {
extern u32 OVERLAY_0_ID[];
extern u32 OVERLAY_1_ID[];
extern u32 OVERLAY_2_ID[];
extern u32 OVERLAY_3_ID[];
extern u32 OVERLAY_4_ID[];
extern u32 OVERLAY_5_ID[];
extern u32 OVERLAY_6_ID[];
extern u32 OVERLAY_7_ID[];
extern u32 OVERLAY_8_ID[];
extern u32 OVERLAY_9_ID[];
extern u32 OVERLAY_10_ID[];
extern u32 OVERLAY_11_ID[];
extern u32 OVERLAY_12_ID[];
extern u32 OVERLAY_13_ID[];
extern u32 OVERLAY_14_ID[];
extern u32 OVERLAY_15_ID[];
extern u32 OVERLAY_16_ID[];
extern u32 OVERLAY_17_ID[];
extern u32 OVERLAY_18_ID[];
extern u32 OVERLAY_19_ID[];
extern u32 OVERLAY_20_ID[];
extern u32 OVERLAY_21_ID[];
extern u32 OVERLAY_22_ID[];
extern u32 OVERLAY_23_ID[];
extern u32 OVERLAY_24_ID[];
extern u32 OVERLAY_25_ID[];
extern u32 OVERLAY_26_ID[];
extern u32 OVERLAY_27_ID[];
extern u32 OVERLAY_28_ID[];
extern u32 OVERLAY_29_ID[];
extern u32 OVERLAY_30_ID[];
extern u32 OVERLAY_31_ID[];
extern u32 OVERLAY_32_ID[];
extern u32 OVERLAY_33_ID[];
extern u32 OVERLAY_34_ID[];
extern u32 OVERLAY_35_ID[];
extern u32 OVERLAY_36_ID[];
extern u32 OVERLAY_37_ID[];
extern u32 OVERLAY_38_ID[];
extern u32 OVERLAY_39_ID[];
extern u32 OVERLAY_40_ID[];
extern u32 OVERLAY_41_ID[];
extern u32 OVERLAY_42_ID[];
extern u32 OVERLAY_43_ID[];
extern u32 OVERLAY_44_ID[];
extern u32 OVERLAY_45_ID[];
extern u32 OVERLAY_46_ID[];
extern u32 OVERLAY_47_ID[];
extern u32 OVERLAY_48_ID[];
extern u32 OVERLAY_49_ID[];
extern u32 OVERLAY_50_ID[];
extern u32 OVERLAY_51_ID[];
extern u32 OVERLAY_52_ID[];
extern u32 OVERLAY_53_ID[];
extern u32 OVERLAY_54_ID[];
extern u32 OVERLAY_55_ID[];
extern u32 OVERLAY_56_ID[];
extern u32 OVERLAY_57_ID[];
extern u32 OVERLAY_58_ID[];
extern u32 OVERLAY_59_ID[];
extern u32 OVERLAY_60_ID[];
extern u32 OVERLAY_61_ID[];
extern u32 OVERLAY_62_ID[];
extern u32 OVERLAY_63_ID[];
extern u32 OVERLAY_64_ID[];
extern u32 OVERLAY_65_ID[];
extern u32 OVERLAY_66_ID[];
extern u32 OVERLAY_67_ID[];
extern u32 OVERLAY_68_ID[];
extern u32 OVERLAY_69_ID[];
extern u32 OVERLAY_70_ID[];
extern u32 OVERLAY_71_ID[];
extern u32 OVERLAY_72_ID[];
extern u32 OVERLAY_73_ID[];
extern u32 OVERLAY_74_ID[];
extern u32 OVERLAY_75_ID[];
extern u32 OVERLAY_76_ID[];
extern u32 OVERLAY_77_ID[];
extern u32 OVERLAY_78_ID[];
extern u32 OVERLAY_79_ID[];
extern u32 OVERLAY_80_ID[];
extern u32 OVERLAY_81_ID[];
extern u32 OVERLAY_82_ID[];
extern u32 OVERLAY_83_ID[];
extern u32 OVERLAY_84_ID[];
extern u32 OVERLAY_85_ID[];
extern u32 OVERLAY_86_ID[];
extern u32 OVERLAY_87_ID[];
extern u32 OVERLAY_88_ID[];
extern u32 OVERLAY_89_ID[];
extern u32 OVERLAY_90_ID[];
extern u32 OVERLAY_91_ID[];
extern u32 OVERLAY_92_ID[];
extern u32 OVERLAY_93_ID[];
extern u32 OVERLAY_94_ID[];
extern u32 OVERLAY_95_ID[];
extern u32 OVERLAY_96_ID[];
extern u32 OVERLAY_97_ID[];
extern u32 OVERLAY_98_ID[];
extern u32 OVERLAY_99_ID[];
extern u32 OVERLAY_100_ID[];
extern u32 OVERLAY_101_ID[];
extern u32 OVERLAY_102_ID[];
extern u32 OVERLAY_103_ID[];
extern u32 OVERLAY_104_ID[];
extern u32 OVERLAY_105_ID[];
extern u32 OVERLAY_106_ID[];
extern u32 OVERLAY_107_ID[];
extern u32 OVERLAY_108_ID[];
extern u32 OVERLAY_109_ID[];
extern u32 OVERLAY_110_ID[];
}

#endif // OVERLAY_H

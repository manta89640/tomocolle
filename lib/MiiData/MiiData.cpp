// A Mii's data, as the game keeps it (the first 0x4A bytes of a character's save record): a get and a set function
// for each of its bit fields, which the character code calls through two tables (setters at 0x020FBB7C, getters at
// 0x020FBC2C, 44 each), and its names and create ID.
//
// The layout is the Wii's Mii data (RFLCharData): every field below has the place and width of the Wii's field of
// that name, the record is 0x4A bytes with the create ID at 0x18, the birth month indexes a table of the days in each
// month, and height and build are capped at 127. The field names are taken from that correspondence; the file and
// the struct name are the project's own. See docs/binary.md, "The Mii data".

#include <nitro.h>

struct MiiData {
    /* 0x00 */ u16 mUnknown00Bit0 : 1;
               u16 mFavoriteColor : 4;
               u16 mBirthDay : 5;
               u16 mBirthMonth : 4;
               u16 mSex : 1;
               u16 mUnknown00Bit15 : 1;
    /* 0x02 */ u16 mName[10];
    /* 0x16 */ u8 mHeight;
    /* 0x17 */ u8 mBuild;
    /* 0x18 */ u8 mCreateId[8];
    /* 0x20 */ u16 mUnknown20Bits0 : 2;
               u16 mUnknown20Bit2 : 1;
               u16 mUnknown20Bits3 : 3;
               u16 mFaceTexture : 4;
               u16 mFaceColor : 3;
               u16 mFaceType : 3;
    /* 0x22 */ u16 mUnknown22Bits0 : 5;
               u16 mHairFlip : 1;
               u16 mHairColor : 3;
               u16 mHairType : 7;
    /* 0x24 */ u16 mUnknown24Bits0 : 6;
               u16 mEyebrowRotation : 5;
               u16 mEyebrowType : 5;
    /* 0x26 */ u16 mEyebrowX : 4;
               u16 mEyebrowY : 5;
               u16 mEyebrowScale : 4;
               u16 mEyebrowColor : 3;
    /* 0x28 */ u16 mEyeY : 5;
               u16 mEyeRotation : 5;
               u16 mEyeType : 6;
    /* 0x2A */ u16 mUnknown2ABits0 : 5;
               u16 mEyeX : 4;
               u16 mEyeScale : 4;
               u16 mEyeColor : 3;
    /* 0x2C */ u16 mUnknown2CBits0 : 3;
               u16 mNoseY : 5;
               u16 mNoseScale : 4;
               u16 mNoseType : 4;
    /* 0x2E */ u16 mMouthY : 5;
               u16 mMouthScale : 4;
               u16 mMouthColor : 2;
               u16 mMouthType : 5;
    /* 0x30 */ u16 mGlassesY : 5;
               u16 mGlassesScale : 4;
               u16 mGlassesColor : 3;
               u16 mGlassesType : 4;
    /* 0x32 */ u16 mBeardY : 5;
               u16 mBeardScale : 4;
               u16 mBeardColor : 3;
               u16 mBeardType : 2;
               u16 mMustacheType : 2;
    /* 0x34 */ u16 mUnknown34Bit0 : 1;
               u16 mMoleX : 5;
               u16 mMoleY : 5;
               u16 mMoleScale : 4;
               u16 mMoleType : 1;
    /* 0x36 */ u16 mCreatorName[10];
}; // 0x4A

extern "C" int func_02079fa0(MiiData *mii) {
    return mii->mEyeType;
}

extern "C" void func_02079fb0(MiiData *mii, int value) {
    mii->mEyeType = value;
}

extern "C" int func_02079fd0(MiiData *mii) {
    return mii->mEyeColor;
}

extern "C" void func_02079fe0(MiiData *mii, int value) {
    mii->mEyeColor = value;
}

extern "C" int func_0207a000(MiiData *mii) {
    return mii->mEyeScale;
}

extern "C" void func_0207a010(MiiData *mii, int value) {
    mii->mEyeScale = value;
}

extern "C" int func_0207a030(MiiData *mii) {
    return mii->mEyeRotation;
}

extern "C" void func_0207a040(MiiData *mii, int value) {
    mii->mEyeRotation = value;
}

extern "C" int func_0207a060(MiiData *mii) {
    return mii->mEyeX;
}

extern "C" void func_0207a070(MiiData *mii, int value) {
    mii->mEyeX = value;
}

extern "C" int func_0207a090(MiiData *mii) {
    return mii->mEyeY;
}

extern "C" void func_0207a0a0(MiiData *mii, int value) {
    mii->mEyeY = value;
}

extern "C" int func_0207a0c0(MiiData *mii) {
    return mii->mEyebrowType;
}

extern "C" void func_0207a0d0(MiiData *mii, int value) {
    mii->mEyebrowType = value;
}

extern "C" int func_0207a0f0(MiiData *mii) {
    return mii->mEyebrowColor;
}

extern "C" void func_0207a100(MiiData *mii, int value) {
    mii->mEyebrowColor = value;
}

extern "C" int func_0207a120(MiiData *mii) {
    return mii->mEyebrowScale;
}

extern "C" void func_0207a130(MiiData *mii, int value) {
    mii->mEyebrowScale = value;
}

extern "C" int func_0207a150(MiiData *mii) {
    return mii->mEyebrowRotation;
}

extern "C" void func_0207a160(MiiData *mii, int value) {
    mii->mEyebrowRotation = value;
}

extern "C" int func_0207a180(MiiData *mii) {
    return mii->mEyebrowX;
}

extern "C" void func_0207a190(MiiData *mii, int value) {
    mii->mEyebrowX = value;
}

extern "C" int func_0207a1b0(MiiData *mii) {
    return mii->mEyebrowY;
}

extern "C" void func_0207a1c0(MiiData *mii, int value) {
    mii->mEyebrowY = value;
}

extern "C" int func_0207a1e0(MiiData *mii) {
    return mii->mMouthType;
}

extern "C" void func_0207a1f0(MiiData *mii, int value) {
    mii->mMouthType = value;
}

extern "C" int func_0207a210(MiiData *mii) {
    return mii->mMouthColor;
}

extern "C" void func_0207a220(MiiData *mii, int value) {
    mii->mMouthColor = value;
}

extern "C" int func_0207a240(MiiData *mii) {
    return mii->mMouthScale;
}

extern "C" void func_0207a250(MiiData *mii, int value) {
    mii->mMouthScale = value;
}

extern "C" int func_0207a270(MiiData *mii) {
    return mii->mMouthY;
}

extern "C" void func_0207a280(MiiData *mii, int value) {
    mii->mMouthY = value;
}

extern "C" int func_0207a2a0(MiiData *mii) {
    return mii->mBeardType;
}

extern "C" void func_0207a2b0(MiiData *mii, int value) {
    mii->mBeardType = value;
}

extern "C" int func_0207a2d0(MiiData *mii) {
    return mii->mMustacheType;
}

extern "C" void func_0207a2e0(MiiData *mii, int value) {
    mii->mMustacheType = value;
}

extern "C" int func_0207a300(MiiData *mii) {
    return mii->mBeardColor;
}

extern "C" void func_0207a310(MiiData *mii, int value) {
    mii->mBeardColor = value;
}

extern "C" int func_0207a330(MiiData *mii) {
    return mii->mBeardScale;
}

extern "C" void func_0207a340(MiiData *mii, int value) {
    mii->mBeardScale = value;
}

extern "C" int func_0207a360(MiiData *mii) {
    return mii->mBeardY;
}

extern "C" void func_0207a370(MiiData *mii, int value) {
    mii->mBeardY = value;
}

extern "C" int func_0207a390(MiiData *mii) {
    return mii->mMoleType;
}

extern "C" void func_0207a3a0(MiiData *mii, int value) {
    mii->mMoleType = value;
}

extern "C" int func_0207a3c0(MiiData *mii) {
    return mii->mMoleScale;
}

extern "C" void func_0207a3d0(MiiData *mii, int value) {
    mii->mMoleScale = value;
}

extern "C" int func_0207a3f0(MiiData *mii) {
    return mii->mMoleX;
}

extern "C" void func_0207a400(MiiData *mii, int value) {
    mii->mMoleX = value;
}

extern "C" int func_0207a420(MiiData *mii) {
    return mii->mMoleY;
}

extern "C" void func_0207a430(MiiData *mii, int value) {
    mii->mMoleY = value;
}

extern "C" int func_0207a450(MiiData *mii) {
    return mii->mHairType;
}

extern "C" void func_0207a460(MiiData *mii, int value) {
    mii->mHairType = value;
}

extern "C" int func_0207a480(MiiData *mii) {
    return mii->mHairColor;
}

extern "C" void func_0207a490(MiiData *mii, int value) {
    mii->mHairColor = value;
}

extern "C" int func_0207a4b0(MiiData *mii) {
    return mii->mHairFlip;
}

extern "C" void func_0207a4c0(MiiData *mii, int value) {
    mii->mHairFlip = value;
}

extern "C" int func_0207a4e0(MiiData *mii) {
    return mii->mNoseType;
}

extern "C" void func_0207a4f0(MiiData *mii, int value) {
    mii->mNoseType = value;
}

extern "C" int func_0207a510(MiiData *mii) {
    return mii->mNoseScale;
}

extern "C" void func_0207a520(MiiData *mii, int value) {
    mii->mNoseScale = value;
}

extern "C" int func_0207a540(MiiData *mii) {
    return mii->mNoseY;
}

extern "C" void func_0207a550(MiiData *mii, int value) {
    mii->mNoseY = value;
}

extern "C" int func_0207a570(MiiData *mii) {
    return mii->mGlassesType;
}

extern "C" void func_0207a580(MiiData *mii, int value) {
    mii->mGlassesType = value;
}

extern "C" int func_0207a5a0(MiiData *mii) {
    return mii->mGlassesColor;
}

extern "C" void func_0207a5b0(MiiData *mii, int value) {
    mii->mGlassesColor = value;
}

extern "C" int func_0207a5d0(MiiData *mii) {
    return mii->mGlassesScale;
}

extern "C" void func_0207a5e0(MiiData *mii, int value) {
    mii->mGlassesScale = value;
}

extern "C" int func_0207a600(MiiData *mii) {
    return mii->mGlassesY;
}

extern "C" void func_0207a610(MiiData *mii, int value) {
    mii->mGlassesY = value;
}

extern "C" int func_0207a630(MiiData *mii) {
    return mii->mFaceType;
}

extern "C" void func_0207a640(MiiData *mii, int value) {
    mii->mFaceType = value;
}

extern "C" int func_0207a660(MiiData *mii) {
    return mii->mFaceColor;
}

extern "C" void func_0207a670(MiiData *mii, int value) {
    mii->mFaceColor = value;
}

extern "C" int func_0207a690(MiiData *mii) {
    return mii->mFaceTexture;
}

extern "C" void func_0207a6a0(MiiData *mii, int value) {
    mii->mFaceTexture = value;
}

extern "C" int func_0207a6c0(MiiData *mii) {
    return mii->mFavoriteColor;
}

extern "C" void func_0207a6d0(MiiData *mii, int value) {
    mii->mFavoriteColor = value;
}

extern "C" void func_0207a6f0(MiiData *mii, int value) {
    mii->mUnknown20Bit2 = value;
}

extern "C" int func_0207a710(MiiData *mii) {
    return mii->mSex;
}

extern "C" void func_0207a720(MiiData *mii, int value) {
    mii->mSex = value;
}

extern "C" int func_0207a740(MiiData *mii) {
    return mii->mBirthMonth;
}

extern "C" void func_0207a750(MiiData *mii, int value) {
    mii->mBirthMonth = value;
}

extern "C" int func_0207a770(MiiData *mii) {
    return mii->mBirthDay;
}

extern "C" void func_0207a780(MiiData *mii, int value) {
    mii->mBirthDay = value;
}

// A male and a female Mii (mSex 0 and 1), with the Wii's defaults: height and build 64, hair 33 for him and 12 for
// her. No function of the ROM refers to them: they are kept by being in one pool with sDaysInMonth, which a function
// that the link left out read them from (see sDefaultMiis' only use, below).
static const MiiData sDefaultMiis[2] = {
    {0, 0, 0, 0, 0, 1, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 64, 64, {0, 0, 0, 0, 0, 0, 0, 0}, 0, 1, 0, 0, 0, 0, 0, 0, 1, 33, 44, 6, 6, 2, 10, 4, 1, 12, 4, 2, 0, 2, 4, 0, 3, 9, 4, 1, 13, 4, 0, 23, 10, 4, 0, 0, 10, 4, 0, 0, 0, 0, 2, 20, 4, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {0, 0, 0, 0, 1, 1, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, 64, 64, {0, 0, 0, 0, 0, 0, 0, 0}, 0, 1, 0, 0, 0, 0, 0, 0, 1, 12, 44, 6, 0, 2, 10, 4, 1, 12, 3, 4, 0, 2, 4, 0, 3, 9, 4, 1, 13, 4, 0, 23, 10, 4, 0, 0, 10, 4, 0, 0, 0, 0, 2, 20, 4, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
};

static const int sDaysInMonth[13] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

// The number of days of the birth month
extern "C" int func_0207a7a0(MiiData *mii) {
    return sDaysInMonth[mii->mBirthMonth];
}

extern "C" void func_0207a7bc(MiiData *mii, int value) {
    mii->mUnknown00Bit0 = value;
}

extern "C" int func_0207a7dc(MiiData *mii) {
    u32 build = mii->mBuild;
    if (build > 0x7F) {
        build = 0x7F;
    }
    return build;
}

extern "C" void func_0207a7ec(MiiData *mii, u8 value) {
    mii->mBuild = value;
}

extern "C" int func_0207a7f4(MiiData *mii) {
    u32 height = mii->mHeight;
    if (height > 0x7F) {
        height = 0x7F;
    }
    return height;
}

extern "C" void func_0207a804(MiiData *mii, u8 value) {
    mii->mHeight = value;
}

// Copies the name out as a string of 11 characters
extern "C" void func_0207a80c(MiiData *mii, u16 *name) {
    MI_CpuClear16(name, 22);
    MI_CpuCopy16(mii->mName, name, 20);
}

// Sets the name from a string, and clears what follows its end
extern "C" void func_0207a838(MiiData *mii, const u16 *name) {
    MI_CpuCopy16(name, mii->mName, 20);
    int i;
    for (i = 0; i < 10; i++) {
        if (mii->mName[i] == 0) {
            break;
        }
    }
    for (i++; i < 10; i++) {
        mii->mName[i] = 0;
    }
}

// Whether the name is empty (all ten characters zero)
extern "C" BOOL func_0207a898(MiiData *mii) {
    for (int i = 0; i < 10; i++) {
        if (mii->mName[i] != 0) {
            return FALSE;
        }
    }
    return TRUE;
}

// Copies the creator's name out as a string of 11 characters
extern "C" void func_0207a8c4(MiiData *mii, u16 *name) {
    MI_CpuClear16(name, 22);
    MI_CpuCopy16(mii->mCreatorName, name, 20);
}

extern "C" void func_0207a8f0(MiiData *mii, const u16 *name) {
    MI_CpuCopy16(name, mii->mCreatorName, 20);
}

struct MiiIdSource {
    /* 0x0 */ u16 mCounter;
    /* 0x2 */ u8 mBytes[6];
};

extern "C" BOOL func_02073550(MiiData *mii);
extern "C" u16 func_020743a0(int max);

// Gives the Mii a new create ID, from a counter (which it advances) and six bytes of source
extern "C" BOOL func_0207a90c(MiiIdSource *source, MiiData *mii) {
    if (!func_02073550(mii) || func_0207a898(mii)) {
        return FALSE;
    }
    mii->mCreateId[0] = (func_020743a0(4) << 2) | 0xC0 | ((source->mCounter & 0x3000) >> 12);
    mii->mCreateId[1] = (source->mCounter & 0xFF0) >> 4;
    mii->mCreateId[2] = (((u32)source->mCounter << 28) >> 24) | source->mBytes[0];
    mii->mCreateId[3] = source->mBytes[1];
    mii->mCreateId[4] = source->mBytes[2];
    mii->mCreateId[5] = source->mBytes[3];
    mii->mCreateId[6] = source->mBytes[4];
    mii->mCreateId[7] = source->mBytes[5];
    source->mCounter++;
    return TRUE;
}

// Not in the ROM: the link left it out, as nothing calls it. Something of this file read sDefaultMiis, or they would
// not be in the ROM; what it did with them is not known, and this stands in for it.
extern "C" void MiiData_UnusedCopyDefault(MiiData *mii, int sex) {
    *mii = sDefaultMiis[sex];
}

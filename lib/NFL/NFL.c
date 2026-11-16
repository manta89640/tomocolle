// NFL, the Mii face library: the main program's code from the end of NitroSystem (0x02069BF4) to the Mii transfer
// (0x020776E4), with its data. This one file holds the library's whole range, which is not yet divided into the
// files it was built from (docs/binary.md, "The Mii library").

#include "NFL.h"

NFLWork *sNFLWork;

// The reader of the library's resource streams: one byte, advancing the stream
u8 NFLi_ReadU8(const u8 **stream) {
    const u8 *p = *stream;
    u8 value = *p++;
    *stream = p;
    return value;
}

u16 NFLi_ReadU16(const u8 **stream) {
    u8 lo = NFLi_ReadU8(stream);
    u8 hi = NFLi_ReadU8(stream);
    return lo | (hi << 8);
}

s16 NFLi_ReadS16(const u8 **stream) {
    u8 lo = NFLi_ReadU8(stream);
    u8 hi = NFLi_ReadU8(stream);
    return lo | (hi << 8);
}

u32 NFLi_ReadU32(const u8 **stream) {
    u8 b0 = NFLi_ReadU8(stream);
    u8 b1 = NFLi_ReadU8(stream);
    u8 b2 = NFLi_ReadU8(stream);
    u8 b3 = NFLi_ReadU8(stream);
    return b0 | ((b1 << 8) | ((b3 << 24) | (b2 << 16)));
}

s32 NFLi_ReadS32(const u8 **stream) {
    u8 b0 = NFLi_ReadU8(stream);
    u8 b1 = NFLi_ReadU8(stream);
    u8 b2 = NFLi_ReadU8(stream);
    u8 b3 = NFLi_ReadU8(stream);
    return b0 | ((b1 << 8) | ((b3 << 24) | (b2 << 16)));
}

// A count, then three halfwords for each
u16 NFLi_ReadVec16Array(const u8 **stream, s16 *out) {
    int i;
    u16 count = NFLi_ReadU16(stream);
    int n = count * 3;

    for (i = 0; i < n; i++) {
        out[i] = NFLi_ReadS16(stream);
    }
    return count;
}

// A count, then two halfwords for each
u16 NFLi_ReadTexCoordArray(const u8 **stream, s16 *out) {
    int i;
    u16 count = NFLi_ReadU16(stream);
    int n = count * 2;

    for (i = 0; i < n; i++) {
        out[i] = NFLi_ReadS16(stream);
    }
    return count;
}

int NFLi_ReadPrimitive(const u8 **stream) {
    int type = 2;

    switch (NFLi_ReadU8(stream)) {
    case 0x90:
        type = 0;
        break;
    case 0x80:
        type = 1;
        break;
    case 0x98:
        break;
    case 0xA0:
    case 0xA8:
    case 0xB0:
    case 0xB8:
    default:
        OS_Terminate();
    }
    return type;
}

// A colour of the library's tables: 8 bits a component
typedef struct NFLColor {
    u32 r;
    u32 g;
    u32 b;
} NFLColor;

extern const NFLColor sNFLHairColors[];
extern const NFLColor sNFLEyeColors[];
extern const NFLColor sNFLMouthColors[];

#define NFL_RGB(r, g, b) GX_RGB((r) >> 3, (g) >> 3, (b) >> 3)

// A palette of 16: colour index of sNFLHairColors, then darker by 5% a step down to 60%
void NFLi_MakeHairPltt(u16 *palette, int index) {
    u32 r = sNFLHairColors[index].r;
    u32 g = sNFLHairColors[index].g;
    u32 b = sNFLHairColors[index].b;

    palette[0] = 0;
    palette[1] = NFL_RGB(r, g, b);
    palette[2] = NFL_RGB(r * 95 / 100, g * 95 / 100, b * 95 / 100);
    palette[3] = NFL_RGB(r * 90 / 100, g * 90 / 100, b * 90 / 100);
    palette[4] = NFL_RGB(r * 85 / 100, g * 85 / 100, b * 85 / 100);
    palette[5] = NFL_RGB(r * 80 / 100, g * 80 / 100, b * 80 / 100);
    palette[6] = NFL_RGB(r * 75 / 100, g * 75 / 100, b * 75 / 100);
    palette[7] = NFL_RGB(r * 70 / 100, g * 70 / 100, b * 70 / 100);
    palette[8] = NFL_RGB(r * 65 / 100, g * 65 / 100, b * 65 / 100);
    palette[9] = NFL_RGB(r * 60 / 100, g * 60 / 100, b * 60 / 100);
    palette[10] = 0;
    palette[11] = 0;
    palette[12] = 0;
    palette[13] = 0;
    palette[14] = 0;
    palette[15] = 0;
}

// A palette of 16: colour index of sNFLEyeColors, 70% and 30% of it, 30% and 70% of the way to white, then fixed
// colours
void NFLi_MakeEyePltt(u16 *palette, int index) {
    u32 r = sNFLEyeColors[index].r;
    u32 g = sNFLEyeColors[index].g;
    u32 b = sNFLEyeColors[index].b;

    palette[0] = 0;
    palette[1] = NFL_RGB(r, g, b);
    palette[2] = NFL_RGB(r * 7 / 10, g * 7 / 10, b * 7 / 10);
    palette[3] = NFL_RGB(r * 3 / 10, g * 3 / 10, b * 3 / 10);
    palette[4] = NFL_RGB(r + (255 - r) * 3 / 10, g + (255 - g) * 3 / 10, b + (255 - b) * 3 / 10);
    palette[5] = NFL_RGB(r + (255 - r) * 7 / 10, g + (255 - g) * 7 / 10, b + (255 - b) * 7 / 10);
    palette[6] = 0;
    palette[7] = GX_RGB(8, 8, 8);
    palette[8] = GX_RGB(16, 16, 16);
    palette[9] = GX_RGB(24, 24, 24);
    palette[10] = GX_RGB(31, 31, 31);
    palette[11] = GX_RGB(0, 31, 31);
    palette[12] = GX_RGB(0, 22, 22);
    palette[13] = GX_RGB(0, 9, 9);
    palette[14] = 0;
    palette[15] = 0;
}

// A palette of 16: the colours 2 * index and 2 * index + 1 of sNFLMouthColors and their mean, then fixed colours
void NFLi_MakeMouthPltt(u16 *palette, int index) {
    u32 r0 = sNFLMouthColors[index * 2].r;
    u32 g0 = sNFLMouthColors[index * 2].g;
    u32 b0 = sNFLMouthColors[index * 2].b;
    u32 r1 = sNFLMouthColors[index * 2 + 1].r;
    u32 g1 = sNFLMouthColors[index * 2 + 1].g;
    u32 b1 = sNFLMouthColors[index * 2 + 1].b;

    palette[0] = 0;
    palette[1] = NFL_RGB(r0, g0, b0);
    palette[2] = NFL_RGB((r0 + r1) / 2, (g0 + g1) / 2, (b0 + b1) / 2);
    palette[3] = NFL_RGB(r1, g1, b1);
    palette[4] = 0;
    palette[5] = GX_RGB(9, 9, 9);
    palette[6] = GX_RGB(22, 22, 22);
    palette[7] = GX_RGB(31, 31, 31);
    palette[8] = 0;
    palette[9] = 0;
    palette[10] = 0;
    palette[11] = 0;
    palette[12] = 0;
    palette[13] = 0;
    palette[14] = 0;
    palette[15] = 0;
}

// from + (to - from) * weight / (rest + weight), in either direction
u32 NFLi_BlendComponent(u32 from, u32 to, u32 rest, u32 weight) {
    if (from < to) {
        return from + (to - from) * weight / (rest + weight);
    }
    return from - (from - to) * weight / (rest + weight);
}

extern const NFLColor sNFLFaceColors[];

#define NFL_LERP_RGB(r, g, b, r2, g2, b2, rest, weight)                                                              \
    NFL_RGB(NFLi_BlendComponent(r, r2, rest, weight), NFLi_BlendComponent(g, g2, rest, weight), NFLi_BlendComponent(b, b2, rest, weight))

// A palette of 16: colour index of sNFLFaceColors, and four ramps from it to four colours made from it
void NFLi_MakeFacePltt(u16 *palette, int index) {
    u32 r = sNFLFaceColors[index].r;
    u32 g = sNFLFaceColors[index].g;
    u32 b = sNFLFaceColors[index].b;
    u32 r1 = NFLi_BlendComponent(r, 0, 3, 1);
    u32 g1 = NFLi_BlendComponent(g, 0, 3, 1);
    u32 b1 = NFLi_BlendComponent(b, 0, 3, 1);
    u32 r2 = NFLi_BlendComponent(r, 0, 3, 1);
    u32 g2 = NFLi_BlendComponent(g, 0xFF, 3, 1);
    u32 b2 = NFLi_BlendComponent(b, 0xDE, 3, 1);
    u32 r3 = NFLi_BlendComponent(r, 0xFF, 3, 1);
    u32 g3 = NFLi_BlendComponent(g, 0x92, 3, 1);
    u32 b3 = NFLi_BlendComponent(b, 0x88, 3, 1);
    u32 r4 = NFLi_BlendComponent(r, 0x55, 3, 2);
    u32 g4 = NFLi_BlendComponent(g, 0x6D, 3, 2);
    u32 b4 = NFLi_BlendComponent(b, 0x66, 3, 2);

    palette[0] = NFL_RGB(r, g, b);
    palette[1] = NFL_LERP_RGB(r, g, b, r1, g1, b1, 75, 25);
    palette[2] = NFL_LERP_RGB(r, g, b, r1, g1, b1, 50, 50);
    palette[3] = NFL_LERP_RGB(r, g, b, r1, g1, b1, 25, 75);
    palette[4] = NFL_RGB(r1, g1, b1);
    palette[5] = NFL_LERP_RGB(r, g, b, r2, g2, b2, 75, 25);
    palette[6] = NFL_LERP_RGB(r, g, b, r2, g2, b2, 50, 50);
    palette[7] = NFL_LERP_RGB(r, g, b, r2, g2, b2, 25, 75);
    palette[8] = NFL_RGB(r2, g2, b2);
    palette[9] = NFL_LERP_RGB(r, g, b, r3, g3, b3, 7, 3);
    palette[10] = NFL_LERP_RGB(r, g, b, r3, g3, b3, 3, 7);
    palette[11] = NFL_RGB(r3, g3, b3);
    palette[12] = NFL_LERP_RGB(r, g, b, r4, g4, b4, 7, 3);
    palette[13] = NFL_LERP_RGB(r, g, b, r4, g4, b4, 3, 7);
    palette[14] = NFL_RGB(r4, g4, b4);
    palette[15] = 0;
}

extern const NFLColor sNFLFavoriteColors[];

// A palette of 16: colour index of sNFLFavoriteColors, then 84%, 68% and 50% of it
void NFLi_MakeFavoriteColorPltt(u16 *palette, int index) {
    u32 r = sNFLFavoriteColors[index].r;
    u32 g = sNFLFavoriteColors[index].g;
    u32 b = sNFLFavoriteColors[index].b;

    palette[0] = NFL_RGB(r, g, b);
    palette[1] = NFL_RGB(r * 84 / 100, g * 84 / 100, b * 84 / 100);
    palette[2] = NFL_RGB(r * 68 / 100, g * 68 / 100, b * 68 / 100);
    palette[3] = NFL_RGB(r / 2, g / 2, b / 2);
    palette[4] = 0;
    palette[5] = 0;
    palette[6] = 0;
    palette[7] = 0;
    palette[8] = 0;
    palette[9] = 0;
    palette[10] = 0;
    palette[11] = 0;
    palette[12] = 0;
    palette[13] = 0;
    palette[14] = 0;
    palette[15] = 0;
}

extern const NFLColor sNFLGlassesColors[];

// A palette of 16: colour index of sNFLGlassesColors, half of it, and 50% and 25% of the way to white
void NFLi_MakeGlassesPltt(u16 *palette, int index) {
    u32 r = sNFLGlassesColors[index].r;
    u32 g = sNFLGlassesColors[index].g;
    u32 b = sNFLGlassesColors[index].b;

    palette[0] = 0;
    palette[1] = NFL_RGB(r, g, b);
    palette[2] = NFL_RGB(r / 2, g / 2, b / 2);
    palette[3] = 0;
    palette[4] = NFL_RGB(r + (255 - r) * 50 / 100, g + (255 - g) * 50 / 100, b + (255 - b) * 50 / 100);
    palette[5] = NFL_RGB(r + (255 - r) * 25 / 100, g + (255 - g) * 25 / 100, b + (255 - b) * 25 / 100);
    palette[6] = 0;
    palette[7] = 0;
    palette[8] = 0;
    palette[9] = 0;
    palette[10] = 0;
    palette[11] = 0;
    palette[12] = 0;
    palette[13] = 0;
    palette[14] = 0;
    palette[15] = 0;
}

// The glasses
int NFLi_PrepareGlasses(NFLMakeCtx *ctx) {
    NFLObj *obj = ctx->unk1C8;
    const NFLCharData *mii = ctx->unk1CC;
    s16 *params = obj->unk180;
    int type = mii->glassesType;
    int color = mii->glassesColor;

    if (params[13] != type || (obj->unk1A4[6].state >= 1 && obj->unk1A4[6].state <= 6)) {
        obj->unk1A4[6].state = 0;
        obj->unk1A4[6].value = type;
    }
    if (color != params[12] || (obj->unk228[5].state >= 1 && obj->unk228[5].state <= 6)) {
        if (obj->unk90 != 7) {
            obj->unk228[5].value = color;
            obj->unk228[5].state = 0;
        } else {
            obj->unk1A4[6].state = 0;
            obj->unk1A4[6].value = type;
        }
    }
    if (obj->unk90 == 7) {
        ctx->unk2C[4].unk0 = obj->unkD4 + 0x2880;
    } else {
        ctx->unk2C[4].unk0 = obj->unkD4 + 0xA10;
        ctx->unk2C[4].unk4 = obj->unkD8 + 0x90;
    }
    return 0;
}

// The beard
int NFLi_PrepareBeard(NFLMakeCtx *ctx) {
    NFLObj *obj = ctx->unk1C8;
    const NFLCharData *mii = ctx->unk1CC;
    s16 *params = obj->unk180;
    int type = mii->beardType;
    int color = mii->beardColor;

    if (params[15] != type || (obj->unk1A4[7].state >= 1 && obj->unk1A4[7].state <= 6)) {
        if (type < 2) {
            ctx->unk2C[3].unk0 = obj->unk8;
            obj->unk1A4[7].state = 7;
        } else {
            obj->unk1A4[7].state = 0;
            obj->unk1A4[7].value = type;
        }
    }
    if (color != params[14] || (obj->unk228[6].state >= 1 && obj->unk228[6].state <= 6)) {
        if (obj->unk90 != 7) {
            obj->unk228[6].value = color;
            obj->unk228[6].state = 0;
        } else if (type >= 2) {
            obj->unk1A4[7].state = 0;
            obj->unk1A4[7].value = type;
        }
    }
    if (type < 2) {
        ctx->unk2C[3].unk0 = obj->unk8;
        ctx->unk2C[3].unk4 = obj->unkC;
    } else if (obj->unk90 == 7) {
        ctx->unk2C[3].unk0 = obj->unkD4 + 0x3880;
    } else {
        ctx->unk2C[3].unk0 = obj->unkD4 + 0xE10;
        ctx->unk2C[3].unk4 = obj->unkD8 + 0xB0;
    }
    return 0;
}

// The hair
int NFLi_PrepareHair(NFLMakeCtx *ctx) {
    NFLObj *obj = ctx->unk1C8;
    const NFLCharData *mii = ctx->unk1CC;
    s16 *params = obj->unk180;
    int type = mii->hairType;
    int color = mii->hairColor;

    if ((params[3] != type || color != params[4]) || (obj->unk1A4[9].state >= 1 && obj->unk1A4[9].state <= 6) ||
        (obj->unk228[8].state >= 1 && obj->unk228[8].state <= 6)) {
        if (type == 5 || type == 7 || type == 0x18 || type == 0x1D || type == 0x2A || type == 0x35 || type == 0x3A ||
            type == 0x45 || type == 0x47) {
            obj->unk1A4[9].state = 0;
            if (obj->unk90 != 7) {
                obj->unk228[8].state = 0;
                obj->unk228[8].value = color;
            }
        } else if (type != 0x22 && type != 0x39) {
            obj->unk1A4[9].state = 7;
            obj->unk228[8].state = 7;
        }
        if (type != 0x22 && type != 0x39) {
            obj->unk1A4[9].value = type;
            if (obj->unk90 != 7) {
                obj->unk228[8].value = color;
            }
        }
    }
    if (type == 5 || type == 7 || type == 0x18 || type == 0x1D || type == 0x2A || type == 0x35 || type == 0x3A ||
        type == 0x45 || type == 0x47) {
        if (obj->unk90 == 7) {
            ctx->unk2C[2].unk0 = obj->unkD4 + 0x5080;
            ctx->unk2C[2].unk4 = obj->unkC;
        } else {
            ctx->unk2C[2].unk0 = obj->unkD4 + 0x1410;
            ctx->unk2C[2].unk4 = obj->unkD8 + 0xF0;
        }
    } else {
        ctx->unk2C[2].unk0 = obj->unk8;
        ctx->unk2C[2].unk4 = obj->unkC;
    }
    return 0;
}

// The hair again, for the two types (0x22 and 0x39) that the other leaves alone
int NFLi_PrepareFavoriteColorHair(NFLMakeCtx *ctx) {
    NFLObj *obj = ctx->unk1C8;
    const NFLCharData *mii = ctx->unk1CC;
    s16 *params = obj->unk180;
    int type = mii->hairType;
    int color = mii->favoriteColor;

    if (type == 0x22 || type == 0x39) {
        if ((type != params[3] || color != params[0]) || (obj->unk1A4[9].state >= 1 && obj->unk1A4[9].state <= 6) ||
            (obj->unk228[8].state >= 1 && obj->unk228[8].state <= 6)) {
            obj->unk1A4[9].state = 0;
            obj->unk1A4[9].value = type;
            if (obj->unk90 == 7) {
                obj->unk228[8].state = 7;
            } else {
                obj->unk228[8].state = 0;
                obj->unk228[8].value = type;
            }
        }
    }
    if (type == 0x22 || type == 0x39) {
        if (obj->unk90 == 7) {
            ctx->unk2C[5].unk0 = obj->unkD4 + 0x5080;
        } else {
            ctx->unk2C[5].unk0 = obj->unkD4 + 0x1410;
            ctx->unk2C[5].unk4 = obj->unkD8 + 0xF0;
        }
    } else if (obj->unk90 == 7) {
        ctx->unk2C[5].unk0 = obj->unk8;
    } else {
        ctx->unk2C[5].unk0 = obj->unk8;
        ctx->unk2C[5].unk4 = obj->unkC;
    }
    return 0;
}

// The face's texture
int NFLi_PrepareFaceTexture(NFLMakeCtx *ctx) {
    NFLObj *obj = ctx->unk1C8;
    const NFLCharData *mii = ctx->unk1CC;
    s16 *params = obj->unk180;
    int texture = mii->faceTexture;
    int color = mii->faceColor;

    if (params[1] != texture || (obj->unk1A4[8].state >= 1 && obj->unk1A4[8].state <= 6)) {
        obj->unk1A4[8].state = texture == 0 ? 7 : 0;
        obj->unk1A4[8].value = texture;
    }
    if (color != params[2] || (obj->unk228[7].state >= 1 && obj->unk228[7].state <= 6)) {
        if (obj->unk90 != 7) {
            obj->unk228[7].value = color;
            obj->unk228[7].state = 0;
        } else if (texture != 0) {
            obj->unk1A4[8].state = 0;
            obj->unk1A4[8].value = texture;
        }
    }
    if (texture == 0) {
        ctx->unk2C[1].unk0 = obj->unk8;
        ctx->unk2C[1].unk4 = obj->unkC;
    } else if (obj->unk90 == 7) {
        ctx->unk2C[1].unk0 = obj->unkD4 + 0x4080;
        ctx->unk2C[1].unk4 = obj->unkC;
    } else {
        ctx->unk2C[1].unk0 = obj->unkD4 + 0x1010;
        ctx->unk2C[1].unk4 = obj->unkD8 + 0xD0;
    }
    return 0;
}

// The nose
int NFLi_PrepareNose(NFLMakeCtx *ctx) {
    NFLObj *obj = ctx->unk1C8;
    const NFLCharData *mii = ctx->unk1CC;
    s16 *params = obj->unk180;
    int type = mii->noseType;

    if (params[9] != type || (obj->unk1A4[10].state >= 1 && obj->unk1A4[10].state <= 6) ||
        (obj->unk228[9].state >= 1 && obj->unk228[9].state <= 6)) {
        obj->unk1A4[10].state = 0;
        obj->unk1A4[10].value = type;
        if (obj->unk90 == 7) {
            obj->unk228[9].state = 7;
        } else {
            obj->unk228[9].state = 0;
            obj->unk228[9].value = type;
        }
    }
    if (obj->unk90 == 7) {
        ctx->unk2C[6].unk0 = obj->unkD4 + 0x5880;
        ctx->unk2C[6].unk4 = obj->unkD8;
    } else {
        ctx->unk2C[6].unk0 = obj->unkD4 + 0x1610;
        ctx->unk2C[6].unk4 = obj->unkD8 + 0x110;
    }
    return 0;
}

// The beard's mesh
int NFLi_MakeBeardMesh(NFLMakeCtx *ctx) {
    s16 vertices[0x4B0];
    s16 normals[0x5C4];
    s16 texCoords[0x5C4];
    VecFx32 position;
    VecFx32 translation;
    const u8 *stream;
    VecFx16 vertex;
    VecFx16 normal;
    NFLObj *obj = ctx->unk1C8;
    u32 type = ctx->unk1CC->beardType;
    GXTexSizeS s;
    GXTexSizeT t;
    GXTexFmt format;
    int i;
    int polygons;
    int j;
    int count;
    int v;
    int n;
    fx16 x;
    fx16 y;
    fx16 z;
    int tc;

    stream = NFLi_GetResourceItem(0, type);
    s = type > 1 ? GX_TEXSIZE_S32 : GX_TEXSIZE_S8;
    t = type > 1 ? GX_TEXSIZE_T32 : GX_TEXSIZE_T8;
    if (NFLi_ReadVec16Array(&stream, vertices) == 0) {
        return 0;
    }
    NFLi_ReadVec16Array(&stream, normals);
    NFLi_ReadTexCoordArray(&stream, texCoords);
    polygons = NFLi_ReadU8(&stream);
    if (obj->unk90 == 7) {
        format = GX_TEXFMT_DIRECT;
    } else {
        format = type == 1 ? GX_TEXFMT_PLTT4 : GX_TEXFMT_PLTT16;
    }
    G3C_TexImageParam(&ctx->unk64, format, GX_TEXGEN_NONE, s, t, GX_TEXREPEAT_S, GX_TEXFLIP_S, GX_TEXPLTTCOLOR0_TRNS,
                      ctx->unk2C[3].unk0);
    if (obj->unk90 != 7) {
        G3C_TexPlttBase(&ctx->unk64, ctx->unk2C[3].unk4, format);
    }
    G3C_PushMtx(&ctx->unk64);
    position.x = ctx->unk0[1].x;
    position.y = ctx->unk0[1].y;
    position.z = ctx->unk0[1].z;
    NFLi_MapVecFx32(obj, &translation, &position);
    G3C_Translate(&ctx->unk64, translation.x, translation.y, translation.z);
    for (i = 0; i < polygons; i++) {
        count = NFLi_ReadU8(&stream);
        func_02005124(&ctx->unk64, NFLi_ReadPrimitive(&stream));
        if (type < 2) {
            G3C_TexCoord(&ctx->unk64, 0, 0x8000);
        }
        for (j = 0; j < count; j++) {
            v = NFLi_ReadU8(&stream) * 3;
            n = NFLi_ReadU8(&stream) * 3;
            tc = NFLi_ReadU8(&stream) * 2;
            if (type > 1) {
                G3C_TexCoord(&ctx->unk64, texCoords[tc] << 5, texCoords[tc + 1] << 5);
            }
            NFLi_MapVecFx16(obj, &normal, (const VecFx16 *)&normals[n]);
            G3C_Normal(&ctx->unk64, normal.x, normal.y, normal.z);
            NFLi_MapVecFx16(obj, &vertex, (const VecFx16 *)&vertices[v]);
            if (j > 0) {
                if (x == vertex.x) {
                    G3C_VtxYZ(&ctx->unk64, vertex.y, vertex.z);
                } else if (y == vertex.y) {
                    G3C_VtxXZ(&ctx->unk64, vertex.x, vertex.z);
                } else if (z == vertex.z) {
                    G3C_VtxXY(&ctx->unk64, vertex.x, vertex.y);
                } else {
                    G3C_Vtx(&ctx->unk64, vertex.x, vertex.y, vertex.z);
                }
            } else {
                G3C_Vtx(&ctx->unk64, vertex.x, vertex.y, vertex.z);
            }
            x = vertex.x;
            y = vertex.y;
            z = vertex.z;
        }
        func_02005148(&ctx->unk64);
    }
    G3C_PopMtx(&ctx->unk64, 1);
    return 1;
}

// The face's mesh, and the three positions the other meshes are placed at
int NFLi_MakeFaceMesh(NFLMakeCtx *ctx) {
    s16 vertices[0x4B0];
    s16 normals[0x5C4];
    s16 texCoords[0x17C];
    const u8 *stream;
    VecFx16 vertex;
    VecFx16 normal;
    NFLObj *obj = ctx->unk1C8;
    const NFLCharData *mii = ctx->unk1CC;
    GXTexSizeS s;
    GXTexSizeT t;
    GXTexFmt format;
    int i;
    int polygons;
    int j;
    int count;
    int v;
    int n;
    fx16 x;
    fx16 y;
    fx16 z;
    int tc;

    stream = NFLi_GetResourceItem(4, mii->faceType);
    s = mii->faceTexture != 0 ? GX_TEXSIZE_S32 : GX_TEXSIZE_S8;
    t = mii->faceTexture != 0 ? GX_TEXSIZE_T64 : GX_TEXSIZE_T8;
    ctx->unk0[0].x = NFLi_ReadS32(&stream);
    ctx->unk0[0].y = NFLi_ReadS32(&stream);
    ctx->unk0[0].z = NFLi_ReadS32(&stream);
    ctx->unk0[1].x = NFLi_ReadS32(&stream);
    ctx->unk0[1].y = NFLi_ReadS32(&stream);
    ctx->unk0[1].z = NFLi_ReadS32(&stream);
    ctx->unk0[2].x = NFLi_ReadS32(&stream);
    ctx->unk0[2].y = NFLi_ReadS32(&stream);
    ctx->unk0[2].z = NFLi_ReadS32(&stream);
    if (NFLi_ReadVec16Array(&stream, vertices) == 0) {
        return 0;
    }
    NFLi_ReadVec16Array(&stream, normals);
    NFLi_ReadTexCoordArray(&stream, texCoords);
    polygons = NFLi_ReadU8(&stream);
    if (obj->unk90 == 7) {
        format = GX_TEXFMT_DIRECT;
    } else {
        format = mii->faceTexture != 0 ? GX_TEXFMT_PLTT16 : GX_TEXFMT_PLTT4;
    }
    G3C_TexImageParam(&ctx->unk64, format, GX_TEXGEN_NONE, s, t, GX_TEXREPEAT_S, GX_TEXFLIP_S, GX_TEXPLTTCOLOR0_USE,
                      ctx->unk2C[1].unk0);
    if (obj->unk90 != 7) {
        G3C_TexPlttBase(&ctx->unk64, ctx->unk2C[1].unk4, format);
    }
    for (i = 0; i < polygons; i++) {
        count = NFLi_ReadU8(&stream);
        func_02005124(&ctx->unk64, NFLi_ReadPrimitive(&stream));
        if (mii->faceTexture == 0) {
            G3C_TexCoord(&ctx->unk64, 0x8000, 0);
        }
        for (j = 0; j < count; j++) {
            v = NFLi_ReadU8(&stream) * 3;
            n = NFLi_ReadU8(&stream) * 3;
            tc = NFLi_ReadU8(&stream) * 2;
            if (mii->faceTexture != 0) {
                G3C_TexCoord(&ctx->unk64, texCoords[tc] << 5, texCoords[tc + 1] << 6);
            }
            NFLi_MapVecFx16(obj, &normal, (const VecFx16 *)&normals[n]);
            G3C_Normal(&ctx->unk64, normal.x, normal.y, normal.z);
            NFLi_MapVecFx16(obj, &vertex, (const VecFx16 *)&vertices[v]);
            if (j > 0) {
                if (x == vertex.x) {
                    G3C_VtxYZ(&ctx->unk64, vertex.y, vertex.z);
                } else if (y == vertex.y) {
                    G3C_VtxXZ(&ctx->unk64, vertex.x, vertex.z);
                } else if (z == vertex.z) {
                    G3C_VtxXY(&ctx->unk64, vertex.x, vertex.y);
                } else {
                    G3C_Vtx(&ctx->unk64, vertex.x, vertex.y, vertex.z);
                }
            } else {
                G3C_Vtx(&ctx->unk64, vertex.x, vertex.y, vertex.z);
            }
            x = vertex.x;
            y = vertex.y;
            z = vertex.z;
        }
        func_02005148(&ctx->unk64);
    }
    return 1;
}

// The hair's mesh
#pragma optimization_level 2
int NFLi_MakeFavoriteColorHairMesh(NFLMakeCtx *ctx) {
    s16 vertices[0x4B0];
    s16 normals[0x5C4];
    s16 texCoords[0x17C];
    VecFx32 position;
    VecFx32 translation;
    MtxFx43 mtx;
    const u8 *stream;
    VecFx16 prev;
    VecFx16 vertex;
    VecFx16 normal;
    int i;
    int polygons;
    NFLObj *obj = ctx->unk1C8;
    const NFLCharData *mii = ctx->unk1CC;
    BOOL bald;
    GXTexSizeS s;
    GXTexSizeT t;
    GXTexFmt format;
    int j;
    int count;
    int v;
    int n;
    int tc;

    stream = NFLi_GetResourceItem(0x12, mii->hairType);
    bald = mii->hairType == 0x22 || mii->hairType == 0x39;
    s = bald ? GX_TEXSIZE_S32 : GX_TEXSIZE_S8;
    t = bald ? GX_TEXSIZE_T32 : GX_TEXSIZE_T8;
    obj->unk94[1].cmd = NULL;
    obj->unk94[1].param = NULL;
    obj->unk94[1].unk8 = 0;
    if (NFLi_ReadVec16Array(&stream, vertices) == 0) {
        return 0;
    }
    NFLi_ReadVec16Array(&stream, normals);
    NFLi_ReadTexCoordArray(&stream, texCoords);
    polygons = NFLi_ReadU8(&stream);
    if (obj->unk90 == 7) {
        format = GX_TEXFMT_DIRECT;
    } else {
        format = bald ? GX_TEXFMT_PLTT16 : GX_TEXFMT_PLTT4;
    }
    obj->unk94[1].cmd = ctx->unk64.curr_cmd;
    obj->unk94[1].param = ctx->unk64.curr_param;
    obj->unk94[1].unk8 = mii->hairFlip;
    G3C_PolygonAttr(&ctx->unk64, 1, GX_POLYGONMODE_MODULATE, mii->hairFlip ? GX_CULL_BACK : GX_CULL_FRONT, 0, 0, 0);
    G3C_TexImageParam(&ctx->unk64, format, GX_TEXGEN_NONE, s, t, GX_TEXREPEAT_ST, GX_TEXFLIP_ST, GX_TEXPLTTCOLOR0_USE,
                      ctx->unk2C[5].unk0);
    if (format != GX_TEXFMT_DIRECT) {
        G3C_TexPlttBase(&ctx->unk64, ctx->unk2C[5].unk4, format);
    }
    G3C_PushMtx(&ctx->unk64);
    position.x = ctx->unk0[2].x;
    position.y = ctx->unk0[2].y;
    position.z = ctx->unk0[2].z;
    NFLi_MapVecFx32(obj, &translation, &position);
    G3C_Translate(&ctx->unk64, translation.x, translation.y, translation.z);
    if (mii->hairFlip) {
        position.x = -FX32_ONE;
        position.y = FX32_ONE;
        position.z = FX32_ONE;
        ((fx32 *)&translation)[obj->unkD2] = position.x;
        ((fx32 *)&translation)[obj->unkD0] = position.y;
        ((fx32 *)&translation)[obj->unkD1] = position.z;
        MTX_Scale43(&mtx, translation.x, translation.y, translation.z);
        G3C_LoadMtx43(&ctx->unk64, &mtx);
    }
    for (i = 0; i < polygons; i++) {
        count = NFLi_ReadU8(&stream);
        func_02005124(&ctx->unk64, NFLi_ReadPrimitive(&stream));
        if (!bald) {
            G3C_TexCoord(&ctx->unk64, 0x8000, 0x8000);
        }
        for (j = 0; j < count; j++) {
            v = NFLi_ReadU8(&stream) * 3;
            n = NFLi_ReadU8(&stream) * 3;
            tc = NFLi_ReadU8(&stream) * 2;
            if (bald) {
                G3C_TexCoord(&ctx->unk64, texCoords[tc] << 5, texCoords[tc + 1] << 5);
            }
            NFLi_MapVecFx16(obj, &normal, (const VecFx16 *)&normals[n]);
            G3C_Normal(&ctx->unk64, normal.x, normal.y, normal.z);
            NFLi_MapVecFx16(obj, &vertex, (const VecFx16 *)&vertices[v]);
            if (j > 0) {
                if (prev.x == vertex.x) {
                    G3C_VtxYZ(&ctx->unk64, vertex.y, vertex.z);
                } else if (prev.y == vertex.y) {
                    G3C_VtxXZ(&ctx->unk64, vertex.x, vertex.z);
                } else if (prev.z == vertex.z) {
                    G3C_VtxXY(&ctx->unk64, vertex.x, vertex.y);
                } else {
                    G3C_Vtx(&ctx->unk64, vertex.x, vertex.y, vertex.z);
                }
            } else {
                G3C_Vtx(&ctx->unk64, vertex.x, vertex.y, vertex.z);
            }
            prev = vertex;
        }
        func_02005148(&ctx->unk64);
    }
    G3C_PopMtx(&ctx->unk64, 1);
    return 1;
}
#pragma optimization_level reset

// The hair's other mesh, for the types with a texture of their own
#pragma optimization_level 2
int NFLi_MakeHairMesh(NFLMakeCtx *ctx) {
    s16 vertices[0x4B0];
    s16 normals[0x5C4];
    s16 texCoords[0x5C4];
    VecFx32 position;
    VecFx32 translation;
    MtxFx43 mtx;
    const u8 *stream;
    VecFx16 prev;
    VecFx16 vertex;
    VecFx16 normal;
    int i;
    int polygons;
    NFLObj *obj = ctx->unk1C8;
    const NFLCharData *mii = ctx->unk1CC;
    int type;
    BOOL bald;
    GXTexSizeS s;
    GXTexSizeT t;
    GXTexFmt format;
    int j;
    int count;
    int v;
    int n;
    int tc;

    type = mii->hairType;
    stream = NFLi_GetResourceItem(9, type);
    bald = type == 5 || type == 7 || type == 0x18 || type == 0x1D || type == 0x2A || type == 0x35 || type == 0x3A ||
           type == 0x45 || type == 0x47;
    s = bald ? GX_TEXSIZE_S32 : GX_TEXSIZE_S8;
    t = bald ? GX_TEXSIZE_T32 : GX_TEXSIZE_T8;
    obj->unk94[0].cmd = NULL;
    obj->unk94[0].param = NULL;
    obj->unk94[0].unk8 = 0;
    if (NFLi_ReadVec16Array(&stream, vertices) == 0) {
        return 0;
    }
    NFLi_ReadVec16Array(&stream, normals);
    NFLi_ReadTexCoordArray(&stream, texCoords);
    polygons = NFLi_ReadU8(&stream);
    if (obj->unk90 == 7) {
        format = GX_TEXFMT_DIRECT;
    } else {
        format = bald ? GX_TEXFMT_PLTT16 : GX_TEXFMT_PLTT4;
    }
    obj->unk94[0].cmd = ctx->unk64.curr_cmd;
    obj->unk94[0].param = ctx->unk64.curr_param;
    obj->unk94[0].unk8 = mii->hairFlip;
    G3C_PolygonAttr(&ctx->unk64, 1, GX_POLYGONMODE_MODULATE, mii->hairFlip ? GX_CULL_BACK : GX_CULL_FRONT, 0, 0, 0);
    G3C_TexImageParam(&ctx->unk64, format, GX_TEXGEN_NONE, s, t, GX_TEXREPEAT_ST, GX_TEXFLIP_NONE,
                      type == 0x1D ? GX_TEXPLTTCOLOR0_TRNS : GX_TEXPLTTCOLOR0_USE, ctx->unk2C[2].unk0);
    if (obj->unk90 != 7) {
        G3C_TexPlttBase(&ctx->unk64, ctx->unk2C[2].unk4, format);
    }
    G3C_PushMtx(&ctx->unk64);
    position.x = ctx->unk0[2].x;
    position.y = ctx->unk0[2].y;
    position.z = ctx->unk0[2].z;
    NFLi_MapVecFx32(obj, &translation, &position);
    G3C_Translate(&ctx->unk64, translation.x, translation.y, translation.z);
    if (mii->hairFlip) {
        position.x = -FX32_ONE;
        position.y = FX32_ONE;
        position.z = FX32_ONE;
        ((fx32 *)&translation)[obj->unkD2] = position.x;
        ((fx32 *)&translation)[obj->unkD0] = position.y;
        ((fx32 *)&translation)[obj->unkD1] = position.z;
        MTX_Scale43(&mtx, translation.x, translation.y, translation.z);
        G3C_LoadMtx43(&ctx->unk64, &mtx);
    }
    for (i = 0; i < polygons; i++) {
        count = NFLi_ReadU8(&stream);
        func_02005124(&ctx->unk64, NFLi_ReadPrimitive(&stream));
        if (!bald) {
            G3C_TexCoord(&ctx->unk64, 0, 0);
        }
        for (j = 0; j < count; j++) {
            v = NFLi_ReadU8(&stream) * 3;
            n = NFLi_ReadU8(&stream) * 3;
            tc = NFLi_ReadU8(&stream) * 2;
            if (bald) {
                G3C_TexCoord(&ctx->unk64, texCoords[tc] << 5, texCoords[tc + 1] << 5);
            }
            NFLi_MapVecFx16(obj, &normal, (const VecFx16 *)&normals[n]);
            G3C_Normal(&ctx->unk64, normal.x, normal.y, normal.z);
            NFLi_MapVecFx16(obj, &vertex, (const VecFx16 *)&vertices[v]);
            if (j > 0) {
                if (prev.x == vertex.x) {
                    G3C_VtxYZ(&ctx->unk64, vertex.y, vertex.z);
                } else if (prev.y == vertex.y) {
                    G3C_VtxXZ(&ctx->unk64, vertex.x, vertex.z);
                } else if (prev.z == vertex.z) {
                    G3C_VtxXY(&ctx->unk64, vertex.x, vertex.y);
                } else {
                    G3C_Vtx(&ctx->unk64, vertex.x, vertex.y, vertex.z);
                }
            } else {
                G3C_Vtx(&ctx->unk64, vertex.x, vertex.y, vertex.z);
            }
            prev = vertex;
        }
        func_02005148(&ctx->unk64);
    }
    G3C_PopMtx(&ctx->unk64, 1);
    return 1;
}
#pragma optimization_level reset

// The hair's third mesh, untextured
#pragma optimization_level 2
int NFLi_MakeHairSkinMesh(NFLMakeCtx *ctx) {
    s16 vertices[0x4B0];
    s16 normals[0x5C4];
    VecFx32 position;
    VecFx32 translation;
    MtxFx43 mtx;
    const u8 *stream;
    VecFx16 prev;
    VecFx16 vertex;
    VecFx16 normal;
    int i;
    int polygons;
    NFLObj *obj = ctx->unk1C8;
    const NFLCharData *mii = ctx->unk1CC;
    GXTexFmt format;
    int j;
    int count;
    int v;
    int n;

    stream = NFLi_GetResourceItem(6, mii->hairType);
    format = obj->unk90 == 7 ? GX_TEXFMT_DIRECT : GX_TEXFMT_PLTT4;
    obj->unk94[2].cmd = NULL;
    obj->unk94[2].param = NULL;
    obj->unk94[2].unk8 = 0;
    if (NFLi_ReadVec16Array(&stream, vertices) == 0) {
        return 0;
    }
    NFLi_ReadVec16Array(&stream, normals);
    polygons = NFLi_ReadU8(&stream);
    obj->unk94[2].cmd = ctx->unk64.curr_cmd;
    obj->unk94[2].param = ctx->unk64.curr_param;
    obj->unk94[2].unk8 = mii->hairFlip;
    G3C_PolygonAttr(&ctx->unk64, 1, GX_POLYGONMODE_MODULATE, mii->hairFlip ? GX_CULL_BACK : GX_CULL_FRONT, 0, 0, 0);
    G3C_TexImageParam(&ctx->unk64, format, GX_TEXGEN_NONE, GX_TEXSIZE_S8, GX_TEXSIZE_T8, GX_TEXREPEAT_NONE,
                      GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, obj->unk8);
    if (obj->unk90 != 7) {
        G3C_TexPlttBase(&ctx->unk64, obj->unkC, format);
    }
    G3C_PushMtx(&ctx->unk64);
    position.x = ctx->unk0[2].x;
    position.y = ctx->unk0[2].y;
    position.z = ctx->unk0[2].z;
    NFLi_MapVecFx32(obj, &translation, &position);
    G3C_Translate(&ctx->unk64, translation.x, translation.y, translation.z);
    if (mii->hairFlip) {
        position.x = -FX32_ONE;
        position.y = FX32_ONE;
        position.z = FX32_ONE;
        ((fx32 *)&translation)[obj->unkD2] = position.x;
        ((fx32 *)&translation)[obj->unkD0] = position.y;
        ((fx32 *)&translation)[obj->unkD1] = position.z;
        MTX_Scale43(&mtx, translation.x, translation.y, translation.z);
        G3C_LoadMtx43(&ctx->unk64, &mtx);
    }
    for (i = 0; i < polygons; i++) {
        count = NFLi_ReadU8(&stream);
        func_02005124(&ctx->unk64, NFLi_ReadPrimitive(&stream));
        G3C_TexCoord(&ctx->unk64, 0x8000, 0);
        for (j = 0; j < count; j++) {
            v = NFLi_ReadU8(&stream) * 3;
            n = NFLi_ReadU8(&stream) * 3;
            NFLi_MapVecFx16(obj, &normal, (const VecFx16 *)&normals[n]);
            G3C_Normal(&ctx->unk64, normal.x, normal.y, normal.z);
            NFLi_MapVecFx16(obj, &vertex, (const VecFx16 *)&vertices[v]);
            if (j > 0) {
                if (prev.x == vertex.x) {
                    G3C_VtxYZ(&ctx->unk64, vertex.y, vertex.z);
                } else if (prev.y == vertex.y) {
                    G3C_VtxXZ(&ctx->unk64, vertex.x, vertex.z);
                } else if (prev.z == vertex.z) {
                    G3C_VtxXY(&ctx->unk64, vertex.x, vertex.y);
                } else {
                    G3C_Vtx(&ctx->unk64, vertex.x, vertex.y, vertex.z);
                }
            } else {
                G3C_Vtx(&ctx->unk64, vertex.x, vertex.y, vertex.z);
            }
            prev = vertex;
        }
        func_02005148(&ctx->unk64);
    }
    G3C_PopMtx(&ctx->unk64, 1);
    return 1;
}
#pragma optimization_level reset

// The glasses' mesh
int NFLi_MakeGlassesMesh(NFLMakeCtx *ctx) {
    s16 vertices[0x4B0];
    s16 normals[0x5C4];
    s16 texCoords[0x17C];
    VecFx32 position;
    VecFx32 translation;
    const u8 *stream;
    VecFx16 vertex;
    VecFx16 normal;
    NFLObj *obj = ctx->unk1C8;
    const NFLCharData *mii = ctx->unk1CC;
    fx32 y0;
    fx32 scale;
    GXTexFmt format;
    int i;
    int polygons;
    int j;
    int count;
    int v;
    int n;
    fx16 x;
    fx16 y;
    fx16 z;
    int tc;

    stream = NFLi_GetResourceItem(7, 0);
    format = obj->unk90 == 7 ? GX_TEXFMT_DIRECT : GX_TEXFMT_PLTT16;
    y0 = (11 - mii->glassesY) * 114 + 387;
    scale = (mii->glassesScale - 4) * 633 + 3987;
    obj->unk94[3].cmd = NULL;
    obj->unk94[3].param = NULL;
    obj->unk94[3].unk8 = 0;
    if (mii->glassesType == 0) {
        return 0;
    }
    if (NFLi_ReadVec16Array(&stream, vertices) == 0) {
        return 0;
    }
    NFLi_ReadVec16Array(&stream, normals);
    NFLi_ReadTexCoordArray(&stream, texCoords);
    polygons = NFLi_ReadU8(&stream);
    obj->unk94[3].cmd = ctx->unk64.curr_cmd;
    obj->unk94[3].param = ctx->unk64.curr_param;
    obj->unk94[3].unk8 = 0;
    G3C_PolygonAttr(&ctx->unk64, 1, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 0, 0, 0);
    G3C_TexImageParam(&ctx->unk64, format, GX_TEXGEN_NONE, GX_TEXSIZE_S32, GX_TEXSIZE_T64, GX_TEXREPEAT_S, GX_TEXFLIP_S,
                      GX_TEXPLTTCOLOR0_TRNS, ctx->unk2C[4].unk0);
    if (format != GX_TEXFMT_DIRECT) {
        G3C_TexPlttBase(&ctx->unk64, ctx->unk2C[4].unk4, GX_TEXFMT_PLTT16);
    }
    G3C_PushMtx(&ctx->unk64);
    position.x = ctx->unk0[0].x;
    position.y = ctx->unk0[0].y + y0;
    position.z = ctx->unk0[0].z + 0xA3;
    NFLi_MapVecFx32(obj, &translation, &position);
    G3C_Translate(&ctx->unk64, translation.x, translation.y, translation.z);
    G3C_Scale(&ctx->unk64, scale, scale, scale);
    for (i = 0; i < polygons; i++) {
        count = NFLi_ReadU8(&stream);
        func_02005124(&ctx->unk64, NFLi_ReadPrimitive(&stream));
        for (j = 0; j < count; j++) {
            v = NFLi_ReadU8(&stream) * 3;
            n = NFLi_ReadU8(&stream) * 3;
            tc = NFLi_ReadU8(&stream) * 2;
            G3C_TexCoord(&ctx->unk64, texCoords[tc] << 5, texCoords[tc + 1] << 6);
            NFLi_MapVecFx16(obj, &normal, (const VecFx16 *)&normals[n]);
            G3C_Normal(&ctx->unk64, normal.x, normal.y, normal.z);
            NFLi_MapVecFx16(obj, &vertex, (const VecFx16 *)&vertices[v]);
            if (j > 0) {
                if (x == vertex.x) {
                    G3C_VtxYZ(&ctx->unk64, vertex.y, vertex.z);
                } else if (y == vertex.y) {
                    G3C_VtxXZ(&ctx->unk64, vertex.x, vertex.z);
                } else if (z == vertex.z) {
                    G3C_VtxXY(&ctx->unk64, vertex.x, vertex.y);
                } else {
                    G3C_Vtx(&ctx->unk64, vertex.x, vertex.y, vertex.z);
                }
            } else {
                G3C_Vtx(&ctx->unk64, vertex.x, vertex.y, vertex.z);
            }
            x = vertex.x;
            y = vertex.y;
            z = vertex.z;
        }
        func_02005148(&ctx->unk64);
    }
    G3C_PopMtx(&ctx->unk64, 1);
    return 1;
}

// The nose's mesh
int NFLi_MakeNoseMesh(NFLMakeCtx *ctx) {
    s16 vertices[0x4B0];
    s16 normals[0x5C4];
    s16 texCoords[0x17C];
    VecFx32 position;
    VecFx32 translation;
    const u8 *stream;
    VecFx16 vertex;
    VecFx16 normal;
    NFLObj *obj = ctx->unk1C8;
    const NFLCharData *mii = ctx->unk1CC;
    fx32 y0;
    fx32 scale;
    GXTexFmt format;
    int i;
    int polygons;
    int j;
    int count;
    int v;
    int n;
    fx16 x;
    fx16 y;
    fx16 z;
    int tc;

    stream = NFLi_GetResourceItem(0x10, mii->noseType);
    format = obj->unk90 == 7 ? GX_TEXFMT_DIRECT : GX_TEXFMT_PLTT16;
    y0 = (mii->noseY - 8) * -114 + 32;
    scale = (mii->noseScale - 4) * 685 + 4377;
    if (NFLi_ReadVec16Array(&stream, vertices) == 0) {
        return 0;
    }
    NFLi_ReadVec16Array(&stream, normals);
    NFLi_ReadTexCoordArray(&stream, texCoords);
    polygons = NFLi_ReadU8(&stream);
    G3C_TexImageParam(&ctx->unk64, format, GX_TEXGEN_NONE, GX_TEXSIZE_S32, GX_TEXSIZE_T64, GX_TEXREPEAT_S, GX_TEXFLIP_S,
                      GX_TEXPLTTCOLOR0_TRNS, ctx->unk2C[6].unk0);
    if (format != GX_TEXFMT_DIRECT) {
        G3C_TexPlttBase(&ctx->unk64, ctx->unk2C[6].unk4, GX_TEXFMT_PLTT16);
    }
    G3C_PushMtx(&ctx->unk64);
    position.x = ctx->unk0[0].x;
    position.y = ctx->unk0[0].y + y0;
    position.z = ctx->unk0[0].z;
    NFLi_MapVecFx32(obj, &translation, &position);
    G3C_Translate(&ctx->unk64, translation.x, translation.y, translation.z);
    G3C_Scale(&ctx->unk64, scale, scale, scale);
    for (i = 0; i < polygons; i++) {
        count = NFLi_ReadU8(&stream);
        func_02005124(&ctx->unk64, NFLi_ReadPrimitive(&stream));
        for (j = 0; j < count; j++) {
            v = NFLi_ReadU8(&stream) * 3;
            n = NFLi_ReadU8(&stream) * 3;
            tc = NFLi_ReadU8(&stream) * 2;
            G3C_TexCoord(&ctx->unk64, texCoords[tc] << 5, texCoords[tc + 1] << 6);
            NFLi_MapVecFx16(obj, &normal, (const VecFx16 *)&normals[n]);
            G3C_Normal(&ctx->unk64, normal.x, normal.y, normal.z);
            NFLi_MapVecFx16(obj, &vertex, (const VecFx16 *)&vertices[v]);
            if (j > 0) {
                if (x == vertex.x) {
                    G3C_VtxYZ(&ctx->unk64, vertex.y, vertex.z);
                } else if (y == vertex.y) {
                    G3C_VtxXZ(&ctx->unk64, vertex.x, vertex.z);
                } else if (z == vertex.z) {
                    G3C_VtxXY(&ctx->unk64, vertex.x, vertex.y);
                } else {
                    G3C_Vtx(&ctx->unk64, vertex.x, vertex.y, vertex.z);
                }
            } else {
                G3C_Vtx(&ctx->unk64, vertex.x, vertex.y, vertex.z);
            }
            x = vertex.x;
            y = vertex.y;
            z = vertex.z;
        }
        func_02005148(&ctx->unk64);
    }
    G3C_PopMtx(&ctx->unk64, 1);
    return 1;
}

// The nose's other mesh, untextured
int NFLi_MakeNoseSkinMesh(NFLMakeCtx *ctx) {
    s16 vertices[0x4B0];
    s16 normals[0x5C4];
    VecFx32 position;
    VecFx32 translation;
    const u8 *stream;
    VecFx16 vertex;
    VecFx16 normal;
    NFLObj *obj = ctx->unk1C8;
    const NFLCharData *mii = ctx->unk1CC;
    fx32 y0;
    fx32 scale;
    GXTexFmt format;
    int i;
    int polygons;
    int j;
    int count;
    int v;
    fx16 x;
    fx16 y;
    fx16 z;

    stream = NFLi_GetResourceItem(0xF, mii->noseType);
    format = obj->unk90 == 7 ? GX_TEXFMT_DIRECT : GX_TEXFMT_PLTT4;
    y0 = (mii->noseY - 8) * -114 + 32;
    scale = (mii->noseScale - 4) * 685 + 4377;
    if (NFLi_ReadVec16Array(&stream, vertices) == 0) {
        return 0;
    }
    NFLi_ReadVec16Array(&stream, normals);
    polygons = NFLi_ReadU8(&stream);
    G3C_TexImageParam(&ctx->unk64, format, GX_TEXGEN_NONE, GX_TEXSIZE_S8, GX_TEXSIZE_T8, GX_TEXREPEAT_NONE,
                      GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, obj->unk8);
    if (obj->unk90 != 7) {
        G3C_TexPlttBase(&ctx->unk64, obj->unkC, format);
    }
    G3C_PushMtx(&ctx->unk64);
    position.x = ctx->unk0[0].x;
    position.y = ctx->unk0[0].y + y0;
    position.z = ctx->unk0[0].z;
    NFLi_MapVecFx32(obj, &translation, &position);
    G3C_Translate(&ctx->unk64, translation.x, translation.y, translation.z);
    G3C_Scale(&ctx->unk64, scale, scale, scale);
    for (i = 0; i < polygons; i++) {
        count = NFLi_ReadU8(&stream);
        func_02005124(&ctx->unk64, NFLi_ReadPrimitive(&stream));
        G3C_TexCoord(&ctx->unk64, 0x8000, 0);
        for (j = 0; j < count; j++) {
            v = NFLi_ReadU8(&stream) * 3;
            NFLi_MapVecFx16(obj, &normal, (const VecFx16 *)&normals[NFLi_ReadU8(&stream) * 3]);
            G3C_Normal(&ctx->unk64, normal.x, normal.y, normal.z);
            NFLi_MapVecFx16(obj, &vertex, (const VecFx16 *)&vertices[v]);
            if (j > 0) {
                if (x == vertex.x) {
                    G3C_VtxYZ(&ctx->unk64, vertex.y, vertex.z);
                } else if (y == vertex.y) {
                    G3C_VtxXZ(&ctx->unk64, vertex.x, vertex.z);
                } else if (z == vertex.z) {
                    G3C_VtxXY(&ctx->unk64, vertex.x, vertex.y);
                } else {
                    G3C_Vtx(&ctx->unk64, vertex.x, vertex.y, vertex.z);
                }
            } else {
                G3C_Vtx(&ctx->unk64, vertex.x, vertex.y, vertex.z);
            }
            x = vertex.x;
            y = vertex.y;
            z = vertex.z;
        }
        func_02005148(&ctx->unk64);
    }
    G3C_PopMtx(&ctx->unk64, 1);
    return 1;
}

extern const u16 sNFLFaceGXColors[];
extern const u16 sNFLHairGXColors[];
extern const u16 sNFLFavoriteGXColors[];

void NFLi_MakeColorTexture(u16 *palette) {
    int i;

    for (i = 0; i < 4; i++) {
        palette[i] = 0x5500;
        palette[i + 4] = 0xFFAA;
    }
    DC_FlushRange(palette, 0x10);
}

void NFLi_MakeColorTexturePltt(u16 *palette, const s16 *params) {
    palette[0] = sNFLHairGXColors[params[4]];
    palette[1] = sNFLFaceGXColors[params[2]];
    palette[2] = sNFLHairGXColors[params[14]];
    palette[3] = sNFLFavoriteGXColors[params[0]];
    DC_FlushRange(palette, 0x10);
}

void NFLi_MakeColorTextureDirect(u16 *palette, const s16 *params) {
    int i;
    int j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            palette[i * 8 + j] = sNFLHairGXColors[params[4]];
        }
        for (j = 4; j < 8; j++) {
            palette[i * 8 + j] = sNFLFaceGXColors[params[2]];
        }
    }
    for (i = 4; i < 8; i++) {
        for (j = 0; j < 4; j++) {
            palette[i * 8 + j] = sNFLHairGXColors[params[14]];
        }
        for (j = 4; j < 8; j++) {
            palette[i * 8 + j] = sNFLFavoriteGXColors[params[0]];
        }
    }
    DC_FlushRange(palette, 0x80);
}

int NFLi_PrepareColorTexturePltt(NFLMakeCtx *ctx) {
    NFLObj *obj = ctx->unk1C8;
    const NFLCharData *mii = ctx->unk1CC;
    s16 *params = obj->unk180;

    if (params[4] != mii->hairColor || params[2] != mii->faceColor || params[14] != mii->beardColor ||
        params[0] != mii->favoriteColor || (obj->unk1A4[0].state >= 1 && obj->unk1A4[0].state <= 6) ||
        (obj->unk228[0].state >= 1 && obj->unk228[0].state <= 6)) {
        obj->unk1A4[0].state = 0;
        obj->unk1A4[0].value = mii->favoriteColor;
        obj->unk228[0].state = 0;
        obj->unk228[0].value = mii->favoriteColor;
    }
    obj->unk8 = obj->unkD4;
    obj->unkC = obj->unkD8;
    return 0;
}

int NFLi_PrepareColorTextureDirect(NFLMakeCtx *ctx) {
    NFLObj *obj = ctx->unk1C8;
    const NFLCharData *mii = ctx->unk1CC;
    s16 *params = obj->unk180;

    if (params[4] != mii->hairColor || params[2] != mii->faceColor || params[14] != mii->beardColor ||
        params[0] != mii->favoriteColor || (obj->unk1A4[0].state >= 1 && obj->unk1A4[0].state <= 6)) {
        obj->unk1A4[0].state = 0;
        obj->unk1A4[0].value = mii->favoriteColor;
    }
    obj->unk8 = obj->unkD4;
    obj->unk228[0].state = 7;
    return 0;
}

int NFLi_PrepareColorTexture(NFLMakeCtx *ctx) {
    if (ctx->unk1C8->unk90 == 7) {
        return NFLi_PrepareColorTextureDirect(ctx);
    }
    return NFLi_PrepareColorTexturePltt(ctx);
}

// The stream of item index of category in the library's resource
const u8 *NFLi_GetResourceItem(int category, u32 index) {
    const u8 *header = sNFLWork->unk64;
    const u8 *items;
    u32 offsets[20];
    int i;
    u16 count;
    u32 offset;

    NFLi_ReadU16(&header);
    NFLi_ReadU16(&header);
    for (i = 0; i < 20; i++) {
        offsets[i] = NFLi_ReadU32(&header);
    }
    items = sNFLWork->unk64 + offsets[category];
    count = NFLi_ReadU16(&items);
    NFLi_ReadU16(&items);
    offset = 0;
    if (count < index) {
        return NULL;
    }
    for (i = 0; i < count; i++) {
        u32 value = NFLi_ReadU32(&items);
        if (i == index) {
            offset = value;
        }
    }
    return items + offset;
}

// Whether every field of a Mii's data is in its range
BOOL NFLi_IsFaceDataInRange(const NFLCharData *mii) {
    if (mii->faceType > 7) {
        return FALSE;
    }
    if (mii->faceColor > 5) {
        return FALSE;
    }
    if (mii->faceTexture > 11) {
        return FALSE;
    }
    if (mii->hairType > 71) {
        return FALSE;
    }
    if (mii->hairColor > 7) {
        return FALSE;
    }
    if (mii->hairFlip > 1) {
        return FALSE;
    }
    if (mii->eyeType > 47) {
        return FALSE;
    }
    if (mii->eyeColor > 5) {
        return FALSE;
    }
    if (mii->eyeScale > 7) {
        return FALSE;
    }
    if (mii->eyeRotation > 7) {
        return FALSE;
    }
    if (mii->eyeX > 12) {
        return FALSE;
    }
    if (mii->eyeY > 18) {
        return FALSE;
    }
    if (mii->eyebrowType > 23) {
        return FALSE;
    }
    if (mii->eyebrowColor > 7) {
        return FALSE;
    }
    if (mii->eyebrowScale > 8) {
        return FALSE;
    }
    if (mii->eyebrowRotation > 11) {
        return FALSE;
    }
    if (mii->eyebrowX > 12) {
        return FALSE;
    }
    if (mii->eyebrowY < 3) {
        return FALSE;
    }
    if (mii->eyebrowY > 18) {
        return FALSE;
    }
    if (mii->noseType > 11) {
        return FALSE;
    }
    if (mii->noseScale > 8) {
        return FALSE;
    }
    if (mii->noseY > 18) {
        return FALSE;
    }
    if (mii->mouthType > 23) {
        return FALSE;
    }
    if (mii->mouthColor > 2) {
        return FALSE;
    }
    if (mii->mouthScale > 8) {
        return FALSE;
    }
    if (mii->mouthY > 18) {
        return FALSE;
    }
    if (mii->mustacheType > 3) {
        return FALSE;
    }
    if (mii->beardType > 3) {
        return FALSE;
    }
    if (mii->beardColor > 7) {
        return FALSE;
    }
    if (mii->beardScale > 8) {
        return FALSE;
    }
    if (mii->beardY > 16) {
        return FALSE;
    }
    if (mii->glassesType > 8) {
        return FALSE;
    }
    if (mii->glassesColor > 5) {
        return FALSE;
    }
    if (mii->glassesScale > 7) {
        return FALSE;
    }
    if (mii->glassesY > 20) {
        return FALSE;
    }
    if (mii->moleType > 1) {
        return FALSE;
    }
    if (mii->moleScale > 8) {
        return FALSE;
    }
    if (mii->moleX > 16) {
        return FALSE;
    }
    if (mii->moleY > 30) {
        return FALSE;
    }
    return TRUE;
}

void NFLi_ResetKeptFields(NFLObj *obj) {
    s16 *params = obj->unk180;

    params[0] = -1;
    params[1] = -1;
    params[2] = -1;
    params[3] = -1;
    params[4] = -1;
    params[5] = -1;
    params[6] = -1;
    params[7] = -1;
    params[8] = -1;
    params[9] = -1;
    params[10] = -1;
    params[11] = -1;
    params[12] = -1;
    params[13] = -1;
    params[14] = -1;
    params[15] = -1;
    params[16] = -1;
    params[17] = -1;
}

// Keeps the fields of the Mii's data that its display lists were made from
void NFLi_KeepFields(NFLObj *obj, const NFLCharData *mii) {
    s16 *params = obj->unk180;

    params[0] = mii->favoriteColor;
    params[1] = mii->faceTexture;
    params[2] = mii->faceColor;
    params[3] = mii->hairType;
    params[4] = mii->hairColor;
    params[5] = mii->eyebrowType;
    params[6] = mii->eyebrowColor;
    params[7] = mii->eyeType;
    params[8] = mii->eyeColor;
    params[9] = mii->noseType;
    params[10] = mii->mouthColor;
    params[11] = mii->mouthType;
    params[12] = mii->glassesColor;
    params[13] = mii->glassesType;
    params[14] = mii->beardColor;
    params[15] = mii->beardType;
    params[16] = mii->mustacheType;
    params[17] = mii->moleType;
}

// The axes of the library's coordinates (0x0211215C), which NFLObj copies at 0xC4
typedef struct NFLAxes {
    VecFx32 unk0;
    u8 unkC;
    u8 unkD;
    u8 unkE;
} NFLAxes;

extern int (*const sNFLPartPreparers[])(NFLMakeCtx *ctx);
extern const NFLAxes sNFLDefaultAxes;

// Makes object index's display lists from a Mii's data
#pragma optimization_level 2
#pragma opt_common_subs off
int NFL_MakeFace(int index, const NFLCharData *mii, u32 unk) {
    OSIntrMode enabled;
    NFLMakeCtx ctx;
    NFLObj *obj;
    NFLObjEntry *entries;
    NFLObjEntry *entries2;
    BOOL found = FALSE;
    int i;
    int result;

    if (mii == NULL) {
        return 8;
    }
    if (!NFLi_IsInitialized()) {
        return 4;
    }
    if (!NFLi_HasResource() || !NFLi_HasBuffer()) {
        return 4;
    }
    if (sNFLWork->unk8C == 1 || sNFLWork->unk8C == 3) {
        return 6;
    }
    obj = NFLi_GetFace(index);
    if (obj == NULL) {
        return 9;
    }
    entries = obj->unk1A4;
    entries2 = obj->unk228;
    if (!NFLi_IsFaceDataInRange(mii) || !NFLi_IsValidCharData(mii)) {
        return 7;
    }
    ctx.unk1C8 = obj;
    ctx.unk1CC = mii;
    obj->unk90 = sNFLWork->unk68;
    obj->unk168 = 0;
    obj->unk16C = sNFLWork->unk71;
    obj->unk0 = 0;
    obj->unk4 = 0;
    obj->unk28 = unk;
    obj->unkDC = mii;
    obj->unk174 = 0;
    obj->unk178 = 0;
    if (obj->unk170 == 4) {
        if (obj->unk90 == 7) {
            obj->unkD4 = ctx.unk24 = sNFLWork->unk8 + index * 0x6880;
            obj->unkD8 = ctx.unk28 = sNFLWork->unkC;
        } else {
            obj->unkD4 = ctx.unk24 = sNFLWork->unk8 + index * 0x1A10;
            obj->unkD8 = ctx.unk28 = sNFLWork->unkC + index * 0x130;
        }
        for (i = 0; i < 11; i++) {
            entries[i].value = -1;
            entries[i].state = 7;
        }
        for (i = 0; i < 10; i++) {
            entries2[i].value = -1;
            entries2[i].state = 7;
        }
        NFLi_ResetKeptFields(obj);
    } else {
        ctx.unk24 = obj->unkD4;
        ctx.unk28 = obj->unkD8;
    }
    obj->unkD0 = sNFLDefaultAxes.unkC;
    obj->unkD1 = sNFLDefaultAxes.unkD;
    obj->unkD2 = sNFLDefaultAxes.unkE;
    obj->unkC4.x = sNFLDefaultAxes.unk0.x;
    obj->unkC4.y = sNFLDefaultAxes.unk0.y;
    obj->unkC4.z = sNFLDefaultAxes.unk0.z;
    enabled = OS_DisableInterrupts();
    NFLi_PrepareColorTexture(&ctx);
    for (i = 0; i < 7; i++) {
        result = sNFLPartPreparers[i](&ctx);
        if (result != 0) {
            return result;
        }
    }
    for (i = 0; i < 11; i++) {
        if (entries[i].state == 0) {
            obj->unk170 = 0;
            found = TRUE;
            break;
        }
    }
    if (!found) {
        for (i = 0; i < 10; i++) {
            if (entries2[i].state == 0) {
                found = TRUE;
                obj->unk170 = found;
                break;
            }
        }
    }
    if (found == TRUE) {
        NFLObj *p;

        if (sNFLWork->unk74 == obj) {
            sNFLWork->unk74 = obj->unk17C;
        }
        p = sNFLWork->unk74;
        if (p == NULL) {
            sNFLWork->unk74 = obj;
        } else {
            while (p->unk17C != NULL) {
                if (p->unk17C == obj) {
                    p->unk17C = obj->unk17C;
                    if (p->unk17C == NULL) {
                        break;
                    }
                }
                p = p->unk17C;
            }
            p->unk17C = obj;
        }
    } else {
        obj->unk170 = 2;
    }
    obj->unk17C = NULL;
    if (sNFLWork->unk8C == 4) {
        sNFLWork->unk8C = 0;
    }
    OS_RestoreInterrupts(enabled);
    G3_BeginMakeDL(&ctx.unk64, obj->unk2A4, sizeof(obj->unk2A4));
    NFLi_MakeFaceMesh(&ctx);
    NFLi_MakeBeardMesh(&ctx);
    NFLi_MakeNoseSkinMesh(&ctx);
    NFLi_MakeNoseMesh(&ctx);
    NFLi_MakeFavoriteColorHairMesh(&ctx);
    NFLi_MakeHairMesh(&ctx);
    NFLi_MakeHairSkinMesh(&ctx);
    NFLi_MakeGlassesMesh(&ctx);
    obj->unk0 = G3_EndMakeDL(&ctx.unk64);
    G3_BeginMakeDL(&ctx.unk64, obj->unk1D84, sizeof(obj->unk1D84));
    NFLi_MakeFacePartsDL(&ctx);
    obj->unk4 = G3_EndMakeDL(&ctx.unk64);
    NFLi_KeepFields(obj, mii);
    NFL_SetExpression(index, 0);
    return 0;
}
#pragma opt_common_subs reset
#pragma optimization_level reset

int NFL_SetFaceMatrices(int index, const MtxFx43 *mtx43, const MtxFx33 *mtx33) {
    NFLObj *obj;

    if (mtx33 == NULL || mtx43 == NULL) {
        return 8;
    }
    if (!NFLi_IsInitialized()) {
        return 4;
    }
    obj = NFLi_GetFace(index);
    if (obj == NULL) {
        return 9;
    }
    MI_Copy48B(mtx43, &obj->unk2C);
    MI_Copy36B(mtx33, &obj->unk5C);
    return 0;
}

// Sets the polygon attributes of an object's display list (at the places its makers kept) and the material colours
int NFL_SetFaceDrawParam(int index, const NFLDrawParam *param) {
    NFLObj *obj;
    GXDLInfo info;

    if (param == NULL) {
        return 8;
    }
    if (!NFLi_IsInitialized()) {
        return 4;
    }
    obj = NFLi_GetFace(index);
    if (obj == NULL) {
        return 9;
    }
    G3_MaterialColorDiffAmb(param->diffuse, param->ambient, FALSE);
    G3_MaterialColorSpecEmi(param->specular, param->emission, FALSE);
    G3_PolygonAttr(param->light, param->polyMode, GX_CULL_FRONT, param->polygonID, param->alpha, param->misc);
    if (obj->unk94[0].cmd != NULL && obj->unk94[0].param != NULL) {
        info.curr_cmd = obj->unk94[0].cmd;
        info.curr_param = obj->unk94[0].param;
        G3CS_PolygonAttr(&info, param->light, param->polyMode, obj->unk94[0].unk8 ? GX_CULL_BACK : GX_CULL_FRONT,
                         param->polygonID, param->alpha, param->misc);
    }
    if (obj->unk94[1].cmd != NULL && obj->unk94[1].param != NULL) {
        info.curr_cmd = obj->unk94[1].cmd;
        info.curr_param = obj->unk94[1].param;
        G3CS_PolygonAttr(&info, param->light, param->polyMode, obj->unk94[1].unk8 ? GX_CULL_BACK : GX_CULL_FRONT,
                         param->polygonID, param->alpha, param->misc);
    }
    if (obj->unk94[2].cmd != NULL && obj->unk94[2].param != NULL) {
        info.curr_cmd = obj->unk94[2].cmd;
        info.curr_param = obj->unk94[2].param;
        G3CS_PolygonAttr(&info, param->light, param->polyMode, obj->unk94[2].unk8 ? GX_CULL_BACK : GX_CULL_FRONT,
                         param->polygonID, param->alpha, param->misc);
    }
    if (obj->unk94[3].cmd != NULL && obj->unk94[3].param != NULL) {
        info.curr_cmd = obj->unk94[3].cmd;
        info.curr_param = obj->unk94[3].param;
        G3CS_PolygonAttr(&info, param->light, param->polyMode, GX_CULL_NONE, param->polygonID, param->alpha,
                         param->misc);
    }
    return 0;
}

// Sends an object's display lists to the geometry engine, under its matrices
int NFL_DrawFace(int index, BOOL withSecond) {
    NFLObj *obj;

    if (!NFLi_IsInitialized()) {
        return 4;
    }
    obj = NFLi_GetFace(index);
    if (obj == NULL) {
        return 9;
    }
    if (NFL_GetFaceStatus(index) != 0) {
        return 0xF;
    }
    G3_PushMtx();
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    G3_LoadMtx43((const MtxFx43 *)&obj->unk5C);
    G3_MtxMode(GX_MTXMODE_POSITION);
    G3_LoadMtx43(&obj->unk2C);
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    MI_WaitDma(GXi_DmaId);
    if (withSecond && obj->unk4 != 0) {
        MI_CpuSend32(obj->unk1D84, &reg_G3X_GXFIFO, obj->unk4);
    }
    if (obj->unk0 != 0) {
        MI_CpuSend32(obj->unk2A4, &reg_G3X_GXFIFO, obj->unk0);
    }
    G3_PopMtx(1);
    return 0;
}

int NFL_GetFaceStatus(int index) {
    NFLObj *obj;

    if (!NFLi_IsInitialized()) {
        return 4;
    }
    obj = NFLi_GetFace(index);
    if (obj == NULL) {
        return 9;
    }
    if (obj->unk170 == 3) {
        return 0xA;
    }
    if (obj->unk170 != 2) {
        return 0xF;
    }
    return 0;
}

extern const NFLColor sNFLFaceColors[];

GXRgb NFLi_GetFaceColor(int index) {
    return GX_RGB(sNFLFaceColors[index].r >> 3, sNFLFaceColors[index].g >> 3, sNFLFaceColors[index].b >> 3);
}

GXRgb NFLi_GetFavoriteColor(int index) {
    return sNFLFavoriteGXColors[index] & 0x7FFF;
}

// Expands count 4-bit pixels into direct colours from palette, colour 0 transparent if asked
void NFLi_Convert4BitToDirect(u16 *dst, const u8 *src, const u16 *palette, u32 count, BOOL transparent) {
    int i;

    for (i = 0; i < count / 2; i++) {
        u32 index;

        if (i % 2 != 0) {
            index = (u32)src[i / 2] >> 4;
        } else {
            index = src[i / 2];
        }
        index &= 0xF;
        if (transparent && index == 0) {
            dst[i] = 0;
        } else {
            dst[i] = palette[index] | 0x8000;
        }
    }
}

// The eyes, eyebrows, mouth, mustache and mole: their textures and states, by part
int NFLi_PrepareFacePartTextures(NFLMakeCtx *ctx) {
    NFLObj *obj = ctx->unk1C8;
    const NFLCharData *mii = ctx->unk1CC;
    s16 *params = obj->unk180;
    int i;

    obj->unk80[0] = mii->eyeColor;
    obj->unk80[1] = mii->mouthColor;
    obj->unk80[2] = mii->eyebrowY;
    obj->unk80[3] = mii->eyeRotation;
    obj->unk80[4] = mii->eyeType;
    obj->unk80[5] = mii->faceType;
    obj->unk80[7] = mii->eyebrowType;
    obj->unk80[6] = mii->eyebrowRotation;
    for (i = 0; i < 5; i++) {
        switch (i) {
        case 0:
            ctx->unk78[0].unk28 = 2;
            ctx->unk78[1].unk28 = 2;
            ctx->unk78[0].unk2C = 2;
            ctx->unk78[1].unk2C = 2;
            if (params[7] != mii->eyeType || (obj->unk1A4[1].state >= 1 && obj->unk1A4[1].state <= 6)) {
                obj->unk1A4[1].state = 0;
                obj->unk1A4[1].value = mii->eyeType;
            }
            if (params[8] != mii->eyeColor || (obj->unk228[1].state >= 1 && obj->unk228[1].state <= 6)) {
                if (obj->unk90 == 7) {
                    obj->unk1A4[1].state = 0;
                    obj->unk228[1].state = 7;
                } else {
                    obj->unk228[1].state = 0;
                }
                obj->unk228[1].value = mii->eyeColor;
            }
            if (obj->unk90 == 7) {
                ctx->unk78[0].unk20 = obj->unkD4 + 0x80;
                ctx->unk78[1].unk20 = obj->unkD4 + 0x80;
                ctx->unk78[0].unk24 = obj->unkD8;
                ctx->unk78[1].unk24 = obj->unkD8;
                obj->unk10 = obj->unkD4 + 0x80;
            } else {
                ctx->unk78[0].unk20 = obj->unkD4 + 0x10;
                ctx->unk78[1].unk20 = obj->unkD4 + 0x10;
                ctx->unk78[0].unk24 = obj->unkD8 + 0x10;
                ctx->unk78[1].unk24 = obj->unkD8 + 0x10;
                obj->unk10 = obj->unkD4 + 0x10;
                obj->unk14 = obj->unkD8 + 0x10;
            }
            break;
        case 1:
            ctx->unk78[2].unk28 = 2;
            ctx->unk78[3].unk28 = 2;
            ctx->unk78[2].unk2C = 2;
            ctx->unk78[3].unk2C = 2;
            if (params[5] != mii->eyebrowType || (obj->unk1A4[2].state >= 1 && obj->unk1A4[2].state <= 6)) {
                obj->unk1A4[2].state = 0;
                obj->unk1A4[2].value = mii->eyebrowType;
            }
            if (params[6] != mii->eyebrowColor || (obj->unk228[2].state >= 1 && obj->unk228[2].state <= 6)) {
                if (obj->unk90 == 7) {
                    obj->unk1A4[2].state = 0;
                    obj->unk228[2].state = 7;
                } else {
                    obj->unk228[2].state = 0;
                }
                obj->unk228[2].value = mii->eyebrowColor;
            }
            if (obj->unk90 == 7) {
                ctx->unk78[2].unk20 = obj->unkD4 + 0x880;
                ctx->unk78[3].unk20 = obj->unkD4 + 0x880;
                ctx->unk78[2].unk24 = obj->unkD8;
                ctx->unk78[3].unk24 = obj->unkD8;
            } else {
                ctx->unk78[2].unk20 = obj->unkD4 + 0x210;
                ctx->unk78[3].unk20 = obj->unkD4 + 0x210;
                ctx->unk78[2].unk24 = obj->unkD8 + 0x30;
                ctx->unk78[3].unk24 = obj->unkD8 + 0x30;
            }
            break;
        case 2:
            ctx->unk78[4].unk28 = 2;
            ctx->unk78[4].unk2C = 2;
            if (params[11] != mii->mouthType || (obj->unk1A4[3].state >= 1 && obj->unk1A4[3].state <= 6)) {
                obj->unk1A4[3].state = 0;
                obj->unk1A4[3].value = mii->mouthType;
            }
            if (params[10] != mii->mouthColor || (obj->unk228[3].state >= 1 && obj->unk228[3].state <= 6)) {
                if (obj->unk90 == 7) {
                    obj->unk1A4[3].state = 0;
                    obj->unk228[3].state = 7;
                } else {
                    obj->unk228[3].state = 0;
                }
                obj->unk228[3].value = mii->mouthColor;
            }
            if (obj->unk90 == 7) {
                ctx->unk78[4].unk20 = obj->unkD4 + 0x1080;
                ctx->unk78[4].unk24 = obj->unkD8;
                obj->unk20 = obj->unkD4 + 0x1080;
            } else {
                ctx->unk78[4].unk20 = obj->unkD4 + 0x410;
                ctx->unk78[4].unk24 = obj->unkD8 + 0x50;
                obj->unk20 = obj->unkD4 + 0x410;
                obj->unk24 = obj->unkD8 + 0x50;
            }
            break;
        case 3:
            ctx->unk78[5].unk28 = 1;
            ctx->unk78[5].unk2C = 3;
            if (params[16] != mii->mustacheType || (obj->unk1A4[4].state >= 1 && obj->unk1A4[4].state <= 6)) {
                obj->unk1A4[4].state = 0;
                obj->unk1A4[4].value = mii->mustacheType;
            }
            if (params[14] != mii->beardColor || (obj->unk228[4].state >= 1 && obj->unk228[4].state <= 6)) {
                if (obj->unk90 != 7) {
                    obj->unk228[4].state = 0;
                } else {
                    obj->unk228[4].state = 7;
                    obj->unk1A4[4].state = 0;
                }
                obj->unk228[4].value = mii->beardColor;
            }
            if (obj->unk90 == 7) {
                ctx->unk78[5].unk20 = obj->unkD4 + 0x1880;
                ctx->unk78[5].unk24 = obj->unkD8;
            } else {
                ctx->unk78[5].unk20 = obj->unkD4 + 0x610;
                ctx->unk78[5].unk24 = obj->unkD8 + 0x70;
            }
            break;
        case 4:
            ctx->unk78[6].unk28 = 1;
            ctx->unk78[6].unk2C = 1;
            if (sNFLWork->unk10[5].unk8 == 0) {
                ctx->unk78[6].unk20 = sNFLWork->unk0;
                ctx->unk78[6].unk24 = sNFLWork->unk4;
                sNFLWork->unk10[5].unk0 = sNFLWork->unk0;
                sNFLWork->unk10[5].unk4 = sNFLWork->unk4;
                sNFLWork->unk90 = 0;
                sNFLWork->unkA0[5].unk8 = 0;
                if (obj->unk90 == 7) {
                    sNFLWork->unk0 += 0x200;
                    sNFLWork->unk12C = 7;
                } else {
                    sNFLWork->unk0 += 0x80;
                    sNFLWork->unk4 += 0x20;
                }
                sNFLWork->unk10[5].unk8 = 1;
                return 0;
            }
            ctx->unk78[6].unk20 = sNFLWork->unk10[5].unk0;
            ctx->unk78[6].unk24 = sNFLWork->unk10[5].unk4;
            return 0;
        }
    }
    return 0;
}

int NFLi_AllocExpression5Texture(NFLObj *obj, u32 *address) {
    NFLWork *work = sNFLWork;

    if (work->unk10[0].unk8 == 0) {
        work->unk10[0].unk0 = *address;
        if (obj->unk90 == 7) {
            *address += 0x800;
        } else {
            *address += 0x200;
            work->unk10[0].unk4 = obj->unk14;
        }
        sNFLWork->unkA0[0].unk8 = 0;
        work->unk10[0].unk8 = 1;
    }
    return 0;
}

int NFLi_AllocExpression6Texture(NFLObj *obj, u32 *address) {
    NFLWork *work = sNFLWork;

    if (work->unk10[1].unk8 == 0) {
        work->unk10[1].unk0 = *address;
        if (obj->unk90 == 7) {
            *address += 0x800;
        } else {
            *address += 0x200;
            work->unk10[1].unk4 = obj->unk24;
        }
        sNFLWork->unkA0[1].unk8 = 0;
        work->unk10[1].unk8 = 1;
    }
    return 0;
}

int NFLi_AllocExpression1Texture(NFLObj *obj, u32 *address) {
    NFLWork *work = sNFLWork;

    if (work->unk10[2].unk8 == 0) {
        work->unk10[2].unk0 = *address;
        if (obj->unk90 == 7) {
            *address += 0x800;
        } else {
            *address += 0x200;
            work->unk10[2].unk4 = obj->unk14;
        }
        sNFLWork->unkA0[2].unk8 = 0;
        work->unk10[2].unk8 = 1;
    }
    return 0;
}

int NFLi_AllocExpression2Texture(NFLObj *obj, u32 *address) {
    NFLWork *work = sNFLWork;

    if (work->unk10[3].unk8 == 0) {
        work->unk10[3].unk0 = *address;
        if (obj->unk90 == 7) {
            *address += 0x800;
        } else {
            *address += 0x200;
            work->unk10[3].unk4 = obj->unk14;
        }
        sNFLWork->unkA0[3].unk8 = 0;
        work->unk10[3].unk8 = 1;
    }
    return 0;
}

int NFLi_AllocExpression3Texture(NFLObj *obj, u32 *address) {
    NFLWork *work = sNFLWork;

    if (work->unk10[4].unk8 == 0) {
        work->unk10[4].unk0 = *address;
        if (obj->unk90 == 7) {
            *address += 0x800;
        } else {
            *address += 0x200;
            work->unk10[4].unk4 = obj->unk14;
        }
        sNFLWork->unkA0[4].unk8 = 0;
        work->unk10[4].unk8 = 1;
    }
    return 0;
}

int NFLi_PrepareExpression4Texture(NFLObj *obj, u32 *address) {
    s16 *params = obj->unk180;
    const NFLCharData *mii = obj->unkDC;

    if (obj->unk16C & 0x10) {
        if (obj->unk1A4[5].value != 0x31 || (obj->unk90 == 7 && params[8] != mii->eyeColor) ||
            (obj->unk1A4[5].state >= 1 && obj->unk1A4[5].state <= 6)) {
            obj->unk1A4[5].state = 0;
            obj->unk1A4[5].value = 0x31;
        }
        if (obj->unk90 == 7) {
            obj->unk18 = obj->unkD4 + 0x2080;
        } else {
            obj->unk18 = obj->unkD4 + 0x810;
            obj->unk1C = obj->unk14;
        }
    }
    return 0;
}

int NFLi_AllocExpressionTextures(NFLMakeCtx *ctx) {
    NFLObj *obj = ctx->unk1C8;
    int result;

    if (obj->unk16C & 0x20) {
        result = NFLi_AllocExpression5Texture(obj, &sNFLWork->unk0);
        if (result != 0) {
            return result;
        }
    }
    if (obj->unk16C & 0x40) {
        result = NFLi_AllocExpression6Texture(obj, &sNFLWork->unk0);
        if (result != 0) {
            return result;
        }
    }
    if (obj->unk16C & 0x2) {
        result = NFLi_AllocExpression1Texture(obj, &sNFLWork->unk0);
        if (result != 0) {
            return result;
        }
    }
    if (obj->unk16C & 0x4) {
        result = NFLi_AllocExpression2Texture(obj, &sNFLWork->unk0);
        if (result != 0) {
            return result;
        }
    }
    if (obj->unk16C & 0x8) {
        result = NFLi_AllocExpression3Texture(obj, &sNFLWork->unk0);
        if (result != 0) {
            return result;
        }
    }
    return NFLi_PrepareExpression4Texture(obj, &ctx->unk24);
}


extern const u8 sNFLEyebrowTypeRotations[];
extern const u8 sNFLEyeTypeRotations[];

// The places, sizes and rotations of the seven parts (NFLMakeCtx 0x78), from the Mii's data
void NFLi_PlaceFaceParts(NFLMakeCtx *ctx) {
    const NFLCharData *mii = ctx->unk1CC;
    fx32 eyeX = mii->eyeX * 0xE95 - 0x280;
    fx32 eyeY = -(mii->eyeY * 0x11A1 - 0x21149);
    fx32 eyeScale = FX_Mul(mii->eyeScale * 0x700 + 0x1000, 0x4C00);
    int eyeRotation = sNFLEyeTypeRotations[mii->eyeType] + mii->eyeRotation - 0x20;
    fx32 eyebrowX = mii->eyebrowX * 0xE95 - 0x2E0;
    fx32 eyebrowY = -(mii->eyebrowY * 0x10E1 - 0x22490);
    fx32 eyebrowScale = FX_Mul(mii->eyebrowScale * 0x680 + 0x1000, 0x4C40);
    int eyebrowRotation = sNFLEyebrowTypeRotations[mii->eyebrowType] + mii->eyebrowRotation - 0x20;
    fx32 mouthY = -(mii->mouthY * 0x10A1 - 0x161BA);
    fx32 mouthScale = mii->mouthScale * 0x6E0 + 0x1000;
    fx32 beardY = -(mii->beardY * 0x10E1 - 0x13DB7);
    fx32 beardScale = mii->beardScale * 0x740 + 0x1000;
    fx32 moleX = (mii->moleX - 8) * 0x1BCA;
    fx32 moleY = -(mii->moleY * 0x10A1 - 0x20CE9);
    fx32 moleScale = mii->moleScale * 0x540 + 0x1000;
    fx32 eyeHeight = 0xD100;
    fx32 eyebrowHeight = 0xC828;
    fx32 beardHeight = 0x147A8;
    fx32 moleHeight = 0x25DE;

    ctx->unk78[0].unk0[0] = eyeX;
    ctx->unk78[0].unk0[1] = eyeY;
    ctx->unk78[0].unk0[2] = eyeScale;
    ctx->unk78[0].unk0[3] = -eyeScale;
    ctx->unk78[0].unk0[4] = eyeHeight;
    ctx->unk78[0].unk0[5] = -eyeHeight;
    ctx->unk78[0].unk0[6] = -(eyeRotation * 0x10000 / 32);
    ctx->unk78[0].unk0[7] = 4;
    ctx->unk78[1].unk0[0] = eyeX;
    ctx->unk78[1].unk0[1] = eyeY;
    ctx->unk78[1].unk0[2] = eyeScale;
    ctx->unk78[1].unk0[3] = -eyeScale;
    ctx->unk78[1].unk0[4] = eyeHeight;
    ctx->unk78[1].unk0[5] = -eyeHeight;
    ctx->unk78[1].unk0[6] = -(eyeRotation * 0x10000 / 32);
    ctx->unk78[1].unk0[7] = 3;
    ctx->unk78[2].unk0[0] = eyebrowX;
    ctx->unk78[2].unk0[1] = eyebrowY;
    ctx->unk78[2].unk0[2] = eyebrowScale;
    ctx->unk78[2].unk0[3] = -eyebrowScale;
    ctx->unk78[2].unk0[4] = eyebrowHeight;
    ctx->unk78[2].unk0[5] = -eyebrowHeight;
    ctx->unk78[2].unk0[6] = -(eyebrowRotation << 11);
    ctx->unk78[2].unk0[7] = 4;
    ctx->unk78[3].unk0[0] = eyebrowX;
    ctx->unk78[3].unk0[1] = eyebrowY;
    ctx->unk78[3].unk0[2] = eyebrowScale;
    ctx->unk78[3].unk0[3] = -eyebrowScale;
    ctx->unk78[3].unk0[4] = eyebrowHeight;
    ctx->unk78[3].unk0[5] = -eyebrowHeight;
    ctx->unk78[3].unk0[6] = -(eyebrowRotation << 11);
    ctx->unk78[3].unk0[7] = 3;
    ctx->unk78[4].unk0[0] = 0;
    ctx->unk78[4].unk0[1] = mouthY;
    ctx->unk78[4].unk0[2] = FX_Mul(mouthScale, 0x5180);
    ctx->unk78[4].unk0[3] = -FX_Mul(mouthScale, 0x28C0);
    ctx->unk78[4].unk0[4] = 0xDD94;
    ctx->unk78[4].unk0[5] = -0x6ECA;
    ctx->unk78[4].unk0[6] = 0;
    ctx->unk78[4].unk0[7] = 1;
    ctx->unk78[5].unk0[0] = 0;
    ctx->unk78[5].unk0[1] = beardY;
    ctx->unk78[5].unk0[2] = FX_Mul(beardScale, 0x7480);
    ctx->unk78[5].unk0[3] = -FX_Mul(beardScale, 0x7480);
    ctx->unk78[5].unk0[4] = beardHeight;
    ctx->unk78[5].unk0[5] = -beardHeight;
    ctx->unk78[5].unk0[6] = 0;
    ctx->unk78[5].unk0[7] = 2;
    ctx->unk78[6].unk0[0] = moleX;
    ctx->unk78[6].unk0[1] = moleY;
    ctx->unk78[6].unk0[2] = FX_Mul(moleScale, 0x1060);
    ctx->unk78[6].unk0[3] = -FX_Mul(moleScale, 0x1060);
    ctx->unk78[6].unk0[4] = moleHeight;
    ctx->unk78[6].unk0[5] = -moleHeight;
    ctx->unk78[6].unk0[6] = 0;
    ctx->unk78[6].unk0[7] = 0;
}

// Four planes, z = -(x * unk0 + y * unk4 + unkC) / unk8, by the quadrant a point is in (NFLi_GetPartCorner)
typedef struct NFLPlane {
    fx32 unk0;
    fx32 unk4;
    fx32 unk8;
    fx32 unkC;
} NFLPlane;

extern const NFLPlane sNFLFacePlanes[2][2];

// A part's place on the face, from the face's origin, its X and Y and the sine and cosine of its rotation; its
// Z from the plane of its quadrant
void NFLi_GetPartCorner(VecFx32 *out, const VecFx32 *origin, u32 kind, int x, int y, fx32 scaleX, fx32 scaleY, fx16 sin,
                   fx16 cos) {
    int i;
    int j;

    if (kind <= 2) {
        out->x = origin->x + (x * 2 - 1) * scaleX * 5 / 128;
        out->y = origin->y + (y * 2 - 1) * scaleY * 5 / 128;
    } else {
        fx32 dx = x * scaleX * 5 / 64;
        fx32 dy = (y * 2 - 1) * scaleY * 5 / 128;

        out->x = origin->x + FX_Mul(sin, dx) + FX_Mul(cos, dy);
        out->y = origin->y - FX_Mul(cos, dx) + FX_Mul(sin, dy);
    }
    i = out->x >= 0x1B33;
    j = out->y <= 0x167B;
    out->z = -FX_Div(sNFLFacePlanes[j][i].unkC + (FX_Mul(sNFLFacePlanes[j][i].unk0, out->x) +
                                                  FX_Mul(sNFLFacePlanes[j][i].unk4, out->y)),
                     sNFLFacePlanes[j][i].unk8) *
             0x47 / 200;
}

// The state that NFLi_MakeMouthMolePart and NFLi_MakeEyeBrowPart share across calls (NFLi_MakeFacePartsDL clears unk20 between pairs)
typedef struct NFLMakeState {
    fx32 unk0;
    fx32 unk4;
    fx32 unk8;
    fx32 unkC;
    VecFx32 unk10;
    fx32 unk1C;
    int unk20;
} NFLMakeState;

void NFLi_MakeMouthMolePart(GXDLInfo *dl, NFLMakePart *part, NFLObj *obj, const NFLCharData *data, NFLPatch *patch,
                   NFLDLRef *ref, int n, NFLMakeState *state);
void NFLi_MakeEyeBrowPart(GXDLInfo *dl, NFLMakePart *part, NFLObj *obj, const NFLCharData *data, NFLPatch *patch,
                   NFLDLRef *ref, int n, NFLMakeState *state);

void NFLi_MakeRotZMtx(const NFLDrawContext *ctx, MtxFx43 *mtx, fx32 sinVal, fx32 cosVal);
void NFLi_DLTranslate(const NFLDrawContext *ctx, GXDLInfo *info, fx32 x, fx32 y, fx32 z);
void NFLi_DLScale(const NFLDrawContext *ctx, GXDLInfo *info, fx32 x, fx32 y, fx32 z);
void NFLi_DLVtx(const NFLDrawContext *ctx, GXDLInfo *info, fx16 x, fx16 y, fx16 z);
void NFLi_DLNormal(const NFLDrawContext *ctx, GXDLInfo *info, fx16 x, fx16 y, fx16 z);
// Not matching yet, a first draft from the code: the instructions are close (0xF98 bytes against 0xFA4) but the
// registers and stack slots are not the original's. Records 0 to 3 of NFLi_MakeFacePartsDL: a part placed as a quad on the
// face, its 2x2 points' Z from the planes of sNFLFacePlanes, their normals interpolated over the 3x3 grids of points
// (sNFLFaceGridPoints) and normals (sNFLFaceGridNormals), the quad turned onto their sum.
extern const VecFx32 sNFLFaceGridPoints[3][3];
extern const VecFx32 sNFLFaceGridNormals[3][3];

#define NFL_PLANE_Z(px, py)                                                                                          \
    (-FX_Div(sNFLFacePlanes[(py) <= 0x167B][(px) >= 0x1B33].unkC +                                                     \
                 (FX_Mul(sNFLFacePlanes[(py) <= 0x167B][(px) >= 0x1B33].unk0, (px)) +                                  \
                  FX_Mul(sNFLFacePlanes[(py) <= 0x167B][(px) >= 0x1B33].unk4, (py))),                                  \
             sNFLFacePlanes[(py) <= 0x167B][(px) >= 0x1B33].unk8) *                                                    \
     0x47 / 200)

void NFLi_MakeEyeBrowPart(GXDLInfo *dl, NFLMakePart *part, NFLObj *obj, const NFLCharData *data, NFLPatch *patch,
                   NFLDLRef *ref, int n, NFLMakeState *state) {
    MtxFx43 mtx;
    VecFx32 grid[2][2];
    VecFx32 normals[2][2];
    VecFx32 axis;
    VecFx32 center;
    VecFx32 normal;
    VecFx32 up;
    VecFx32 point;
    VecFx32 point2;
    VecFx32 sum;
    VecFx32 rotated;
    fx32 s0;
    fx32 s1;
    fx32 x0;
    fx32 x;
    fx32 y;
    fx32 w;
    fx32 h;
    int kind;
    int fmt;
    int texW;
    int texH;
    int faceType;
    int i;
    int j;
    fx32 scaleX;
    fx32 scaleY;
    fx16 sin;
    fx16 cos;
    fx32 dot;
    fx32 cross;
    int k;
    int a;
    int b;
    fx32 size;
    fx32 u;
    fx32 v;

    texW = 1;
    texH = 1;
    x = part->unk0[0];
    y = part->unk0[1];
    scaleX = part->unk0[2];
    scaleY = part->unk0[3];
    w = part->unk0[4];
    h = part->unk0[5];
    kind = part->unk0[7];
    fmt = obj->unk90;
    G3C_PushMtx(dl);
    sin = FX_CosIdx((u16)part->unk0[6]);
    cos = FX_SinIdx((u16)part->unk0[6]);
    faceType = data->faceType;
    center.x = x * 5 / 64;
    center.y = y * 5 / 64;
    if (center.x < 0) {
        center.x = -center.x;
    }
    center.z = NFL_PLANE_Z(center.x, center.y);
    for (i = 0; i < 2; i++) {
        fx32 t1 = i * w * 5 / 64;
        fx32 ct = FX_Mul(sin, t1);
        fx32 st = FX_Mul(cos, t1);

        for (j = 0; j < 2; j++) {
            fx32 t2 = h * (j * 2 - 1) * 5 / 128;
            int pi;
            int pj;
            const VecFx32 *p;
            const VecFx32 *q;

            grid[i][j].x = center.x + ct + FX_Mul(cos, t2);
            grid[i][j].y = center.y - st + FX_Mul(sin, t2);
            pi = grid[i][j].x >= 0x1B33;
            pj = grid[i][j].y <= 0x167B;
            grid[i][j].z = -FX_Div(sNFLFacePlanes[pj][pi].unkC + (FX_Mul(sNFLFacePlanes[pj][pi].unk0, grid[i][j].x) +
                                                                  FX_Mul(sNFLFacePlanes[pj][pi].unk4, grid[i][j].y)),
                                   sNFLFacePlanes[pj][pi].unk8) *
                           0x47 / 200;
            p = &sNFLFaceGridPoints[pj][pi];
            u = FX_Div(grid[i][j].x - p[0].x, p[1].x - p[0].x);
            v = FX_Div(grid[i][j].y - p[0].y, p[3].y - p[0].y);
            q = &sNFLFaceGridNormals[pj][pi];
            {
                fx32 top = q[0].x + FX_Mul(q[1].x - q[0].x, u);
                fx32 bottom = q[3].x + FX_Mul(q[4].x - q[3].x, u);
                normals[i][j].x = top + FX_Mul(bottom - top, v);
            }
            {
                fx32 top = q[0].y + FX_Mul(q[1].y - q[0].y, u);
                fx32 bottom = q[3].y + FX_Mul(q[4].y - q[3].y, u);
                normals[i][j].y = top + FX_Mul(bottom - top, v);
            }
            {
                fx32 top = q[0].z + FX_Mul(q[1].z - q[0].z, u);
                fx32 bottom = q[3].z + FX_Mul(q[4].z - q[3].z, u);
                normals[i][j].z = top + FX_Mul(bottom - top, v);
            }
            VEC_Normalize(&normals[i][j], &normals[i][j]);
        }
    }
    normal.x = 0;
    normal.y = 0;
    normal.z = 0;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            if (kind == 4) {
                normal.x -= normals[i][j].x;
            } else {
                normal.x += normals[i][j].x;
            }
            normal.y += normals[i][j].y;
            normal.z += normals[i][j].z;
        }
    }
    if (kind == 0) {
        center.z /= 2;
        if (x < 0) {
            normal.x = -normal.x;
        }
    }
    VEC_Normalize(&normal, &normal);
    if (state != NULL && state->unk20 == 0) {
        state->unk10.x = 0;
        state->unk10.y = 0;
        state->unk10.z = 0;
        for (a = 0; a < 2; a++) {
            for (b = 0; b < 2; b++) {
                NFLi_GetPartCorner(&point, &center, kind, a, b, scaleX, scaleY, sin, cos);
                state->unk10.x += point.x;
                state->unk10.y += point.y;
            }
        }
        state->unk10.x /= 4;
        state->unk10.y /= 4;
        state->unk10.z = 0;
        state->unk1C = (scaleX > scaleY ? scaleX : scaleY) * 5 / 128;
        state->unk0 = normal.x < 0 ? -normal.x : normal.x;
        state->unk4 = normal.y;
        state->unk8 = normal.z * 200 / 0x47;
        state->unkC = -(FX_Mul(state->unk8, center.z) + (FX_Mul(state->unk0, x / 32 < 0 ? -(x / 32) : x / 32) +
                                                          FX_Mul(normal.y, y / 32)));
    }
    if (state != NULL) {
        if (state->unk20 == 1) {
            sum.x = 0;
            sum.y = 0;
            sum.z = 0;
            for (k = 0; k < 4; k++) {
                NFLi_GetPartCorner(&point2, &center, kind, k / 2, k % 2, scaleX, scaleY, sin, cos);
                sum.x += point2.x;
                sum.y += point2.y;
            }
            sum.x /= 4;
            sum.y /= 4;
            sum.z = 0;
            size = (scaleX > scaleY ? scaleX : scaleY) * 5 / 128;
            if (VEC_Distance(&sum, &state->unk10) > (size + state->unk1C) * 3 / 5) {
                n = 0;
            } else {
                center.z = -FX_Div(state->unkC + (FX_Mul(state->unk0, x / 32 < 0 ? -(x / 32) : x / 32) +
                                                  FX_Mul(state->unk4, y / 32)),
                                   state->unk8);
            }
        } else {
            state->unk20 = 1;
        }
    }
    up.x = 0;
    up.y = 0;
    up.z = FX32_ONE;
    dot = VEC_DotProduct(&normal, &up);
    cross = FX_Sqrt(FX32_ONE - FX_Mul(dot, dot));
    VEC_CrossProduct(&up, &normal, &axis);
    switch (faceType) {
    case 2:
    case 6:
    case 7:
        y += 0x1E60;
        break;
    }
    if (ref != NULL) {
        ref->cmd = dl->curr_cmd;
        ref->param = dl->curr_param;
    }
    if (kind == 4) {
        NFLi_DLTranslate(obj, dl, -x / 32, y / 32, obj->unk28 + (center.z + n));
    } else {
        NFLi_DLTranslate(obj, dl, x / 32, y / 32, obj->unk28 + (center.z + n));
    }
    NFLi_MapVecFx32(obj, &rotated, &axis);
    axis = rotated;
    MTX_RotAxis43(&mtx, &axis, cross, dot);
    G3C_MultMtx43(dl, &mtx);
    patch->mtx.cmd = dl->curr_cmd;
    patch->mtx.param = dl->curr_param;
    NFLi_MakeRotZMtx(obj, &mtx, cos, sin);
    G3C_MultMtx43(dl, &mtx);
    NFLi_DLScale(obj, dl, scaleX / 32, scaleY / 32, FX32_ONE);
    if (fmt == GX_TEXFMT_DIRECT) {
        patch->tex.cmd = dl->curr_cmd;
        patch->tex.param = dl->curr_param;
        G3C_TexImageParam(dl, GX_TEXFMT_DIRECT, GX_TEXGEN_NONE, part->unk28, part->unk2C, GX_TEXREPEAT_ST,
                          GX_TEXFLIP_ST, GX_TEXPLTTCOLOR0_TRNS, part->unk20);
    } else {
        patch->tex.cmd = dl->curr_cmd;
        patch->tex.param = dl->curr_param;
        G3C_TexImageParam(dl, GX_TEXFMT_PLTT16, GX_TEXGEN_NONE, part->unk28, part->unk2C, GX_TEXREPEAT_ST,
                          GX_TEXFLIP_ST, GX_TEXPLTTCOLOR0_TRNS, part->unk20);
        patch->pltt.cmd = dl->curr_cmd;
        patch->pltt.param = dl->curr_param;
        G3C_TexPlttBase(dl, part->unk24, GX_TEXFMT_PLTT16);
    }
    switch (part->unk28) {
    case 1:
        texW = 16;
        break;
    case 2:
        texW = 32;
        break;
    case 3:
        texW = 64;
        break;
    }
    switch (part->unk2C) {
    case 1:
        texH = 16;
        break;
    case 2:
        texH = 32;
        break;
    case 3:
        texH = 64;
        break;
    }
    if (kind == 3) {
        x0 = 0;
        s1 = 0;
        s0 = texW << 12;
    } else if (kind == 4) {
        x0 = -FX32_ONE;
        s1 = texW << 12;
        s0 = 0;
    }
    func_02005124(dl, GX_BEGIN_QUADS);
    G3C_TexCoord(dl, s0, 0);
    NFLi_DLNormal(obj, dl, 0, 0, 0xFEA);
    NFLi_DLVtx(obj, dl, x0, -0x800, 0);
    G3C_TexCoord(dl, s1, 0);
    NFLi_DLVtx(obj, dl, x0 + FX32_ONE, -0x800, 0);
    G3C_TexCoord(dl, s1, texH << 12);
    NFLi_DLVtx(obj, dl, x0 + FX32_ONE, 0x800, 0);
    G3C_TexCoord(dl, s0, texH << 12);
    NFLi_DLVtx(obj, dl, x0, 0x800, 0);
    func_02005148(dl);
    G3C_PopMtx(dl, 1);
}

void NFLi_MakeFacePartsDL(NFLMakeCtx *ctx) {
    NFLObj *obj = ctx->unk1C8;
    const NFLCharData *data = ctx->unk1CC;
    NFLMakeState state;

    state.unk20 = 0;
    NFLi_MakeMouthMolePart(&ctx->unk64, &ctx->unk78[4], obj, data, &obj->unk140, NULL, 0, &state);
    NFLi_MakeMouthMolePart(&ctx->unk64, &ctx->unk78[5], obj, data, NULL, NULL, 0x20, &state);
    state.unk20 = 0;
    NFLi_MakeEyeBrowPart(&ctx->unk64, &ctx->unk78[0], obj, data, &obj->unk128, NULL, 0, &state);
    NFLi_MakeEyeBrowPart(&ctx->unk64, &ctx->unk78[2], obj, data, &obj->unkF8, &obj->unk160, 0x20, &state);
    state.unk20 = 0;
    NFLi_MakeEyeBrowPart(&ctx->unk64, &ctx->unk78[1], obj, data, &obj->unk110, NULL, 0, &state);
    NFLi_MakeEyeBrowPart(&ctx->unk64, &ctx->unk78[3], obj, data, &obj->unkE0, &obj->unk158, 0x20, &state);
    if (data->moleType == 1) {
        NFLi_MakeMouthMolePart(&ctx->unk64, &ctx->unk78[6], obj, data, NULL, NULL, 0, NULL);
    }
}


// Patches a face part's display list: its rotation, if it has a matrix command, and its texture and palette
void NFLi_PatchFacePart(NFLObj *obj, NFLPatch *patch, fx16 sin, fx16 cos, GXTexFmt fmt, GXTexSizeS s, GXTexSizeT t,
                   u32 texAddr, u32 plttAddr) {
    GXDLInfo dl;
    MtxFx43 mtx;

    if (patch->mtx.cmd != NULL && patch->mtx.param != NULL) {
        dl.curr_cmd = patch->mtx.cmd;
        dl.curr_param = patch->mtx.param;
        NFLi_MakeRotZMtx(obj, &mtx, sin, cos);
        G3CS_MultMtx43(&dl, &mtx);
    }
    dl.curr_cmd = patch->tex.cmd;
    dl.curr_param = patch->tex.param;
    if (texAddr == 0) {
        return;
    }
    if (fmt == GX_TEXFMT_DIRECT) {
        G3CS_TexImageParam(&dl, fmt, GX_TEXGEN_NONE, s, t, GX_TEXREPEAT_ST, GX_TEXFLIP_ST, GX_TEXPLTTCOLOR0_TRNS,
                           texAddr);
        return;
    }
    G3CS_TexImageParam(&dl, GX_TEXFMT_PLTT16, GX_TEXGEN_NONE, s, t, GX_TEXREPEAT_ST, GX_TEXFLIP_ST,
                       GX_TEXPLTTCOLOR0_TRNS, texAddr);
    dl.curr_cmd = patch->pltt.cmd;
    dl.curr_param = patch->pltt.param;
    G3CS_TexPlttBase(&dl, plttAddr, fmt);
}

extern const u8 sNFLEyeTypeRotations[];
extern const u8 sNFLEyebrowTypeRotations[];

#pragma optimization_level 2
#pragma opt_common_subs off
void NFLi_ApplyExpression0(NFLObj *obj) {
    fx16 sin;
    fx16 cos;
    fx32 y;
    int type;
    u16 idx;
    fx32 v;

    idx = (u16)(((obj->unk80[3] + sNFLEyeTypeRotations[obj->unk80[4]] - 0x20) << 16) / 32);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    y = -(obj->unk80[2] * 0x10E1 - 0x22490);
    type = obj->unk80[5];
    NFLi_PatchFacePart(obj, &obj->unk110, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->unk10, obj->unk14);
    NFLi_PatchFacePart(obj, &obj->unk128, -sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->unk10, obj->unk14);
    idx = (u16)((obj->unk80[6] + sNFLEyebrowTypeRotations[obj->unk80[7]] - 0x20) << 11);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->unkE0, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    NFLi_PatchFacePart(obj, &obj->unkF8, -sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    switch (type) {
    case 2:
    case 6:
    case 7:
        y += 0x1E60;
        break;
    }
    if (obj->unkC4.x != 0) {
        v = -y / 32;
    } else {
        v = y / 32;
    }
    obj->unk158.param[obj->unkD0] = v;
    obj->unk160.param[obj->unkD0] = v;
    sin = (fx16)FX_SinIdx(0);
    cos = (fx16)FX_CosIdx(0);
    NFLi_PatchFacePart(obj, &obj->unk140, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->unk20, obj->unk24);
}
#pragma opt_common_subs reset
#pragma optimization_level reset

// Not matching yet: the original keeps the compare and branch of this clamp, whose result nothing uses (the eye's
// rotation goes in unclamped); here the compiler drops the whole clamp.
#pragma optimization_level 2
#pragma opt_common_subs off
void NFLi_ApplyExpression5(NFLObj *obj) {
    fx16 sin;
    fx16 cos;
    fx32 y;
    int type;
    u16 idx;
    fx32 v;
    int rot;

    idx = (u16)(((obj->unk80[3] + sNFLEyeTypeRotations[obj->unk80[4]] - 0x20) << 16) / 32);
    rot = obj->unk80[3] + (sNFLEyeTypeRotations[obj->unk80[3]] - sNFLEyeTypeRotations[26]);
    if (rot < 0) {
        rot = 0;
    } else if (rot > 7) {
        rot = 7;
    }
    y = -(obj->unk80[2] * 0x10E1 - 0x22490);
    type = obj->unk80[5];
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->unk110, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, sNFLWork->unk10[0].unk0, obj->unk14);
    NFLi_PatchFacePart(obj, &obj->unk128, -sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, sNFLWork->unk10[0].unk0, obj->unk14);
    idx = (u16)((obj->unk80[6] + sNFLEyebrowTypeRotations[obj->unk80[7]] - 0x20) << 11);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->unkE0, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    NFLi_PatchFacePart(obj, &obj->unkF8, -sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    switch (type) {
    case 2:
    case 6:
    case 7:
        y += 0x1E60;
        break;
    }
    if (obj->unkC4.x != 0) {
        v = -y / 32;
    } else {
        v = y / 32;
    }
    obj->unk158.param[obj->unkD0] = v;
    obj->unk160.param[obj->unkD0] = v;
    sin = (fx16)FX_SinIdx(0);
    cos = (fx16)FX_CosIdx(0);
    NFLi_PatchFacePart(obj, &obj->unk140, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->unk20, obj->unk24);
}
#pragma opt_common_subs reset
#pragma optimization_level reset

#pragma optimization_level 2
#pragma opt_common_subs off
void NFLi_ApplyExpression6(NFLObj *obj) {
    fx16 sin;
    fx16 cos;
    fx32 y;
    int type;
    u16 idx;
    fx32 v;

    idx = (u16)(((obj->unk80[3] + sNFLEyeTypeRotations[obj->unk80[4]] - 0x20) << 16) / 32);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    y = -(obj->unk80[2] * 0x10E1 - 0x22490);
    type = obj->unk80[5];
    NFLi_PatchFacePart(obj, &obj->unk110, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->unk10, obj->unk14);
    NFLi_PatchFacePart(obj, &obj->unk128, -sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->unk10, obj->unk14);
    idx = (u16)((obj->unk80[6] + sNFLEyebrowTypeRotations[obj->unk80[7]] - 0x20) << 11);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->unkE0, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    NFLi_PatchFacePart(obj, &obj->unkF8, -sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    switch (type) {
    case 2:
    case 6:
    case 7:
        y += 0x1E60;
        break;
    }
    if (obj->unkC4.x != 0) {
        v = -y / 32;
    } else {
        v = y / 32;
    }
    obj->unk158.param[obj->unkD0] = v;
    obj->unk160.param[obj->unkD0] = v;
    sin = (fx16)FX_SinIdx(0);
    cos = (fx16)FX_CosIdx(0);
    NFLi_PatchFacePart(obj, &obj->unk140, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, sNFLWork->unk10[1].unk0, obj->unk24);
}
#pragma opt_common_subs reset
#pragma optimization_level reset

// Not matching yet: the code is the original's, but the clamped rotation is kept in r3 where the original reuses r4.
#pragma optimization_level 2
#pragma opt_common_subs off
#pragma opt_propagation off
void NFLi_ApplyExpression1(NFLObj *obj) {
    fx16 sin;
    fx16 cos;
    fx32 y;
    int type;
    u16 idx;
    fx32 v;
    int rot;

    rot = obj->unk80[3] + (sNFLEyeTypeRotations[obj->unk80[3]] - sNFLEyeTypeRotations[48]);
    y = -(obj->unk80[2] * 0x10E1 - 0x22490);
    type = obj->unk80[5];
    if (rot < 0) {
        rot = 0;
    } else if (rot > 7) {
        rot = 7;
    }
    idx = (u16)(((rot + sNFLEyeTypeRotations[obj->unk80[4]] - 0x20) << 16) / 32);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->unk110, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, sNFLWork->unk10[2].unk0, obj->unk14);
    NFLi_PatchFacePart(obj, &obj->unk128, -sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, sNFLWork->unk10[2].unk0, obj->unk14);
    idx = (u16)((obj->unk80[6] + sNFLEyebrowTypeRotations[obj->unk80[7]] - 0x20) << 11);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->unkE0, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    NFLi_PatchFacePart(obj, &obj->unkF8, -sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    switch (type) {
    case 2:
    case 6:
    case 7:
        y += 0x1E60;
        break;
    }
    if (obj->unkC4.x != 0) {
        v = -y / 32;
    } else {
        v = y / 32;
    }
    obj->unk158.param[obj->unkD0] = v;
    obj->unk160.param[obj->unkD0] = v;
    sin = (fx16)FX_SinIdx(0);
    cos = (fx16)FX_CosIdx(0);
    NFLi_PatchFacePart(obj, &obj->unk140, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->unk20, obj->unk24);
}
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma optimization_level reset

// Not matching yet: for the eyebrow the original adds d twice (once to compare, once in the else), where this folds
// the sum into brow before the compares. opt_propagation off keeps d in a register, as the original does.
#pragma optimization_level 2
#pragma opt_common_subs off
#pragma opt_propagation off
void NFLi_ApplyExpression2(NFLObj *obj) {
    fx16 sin;
    fx16 cos;
    fx32 y;
    int type;
    u16 idx;
    fx32 v;
    int rot;
    int brow;
    int d;

    y = -(obj->unk80[2] * 0x10E1 - 0x22490);
    type = obj->unk80[5];
    brow = obj->unk80[6];
    d = 2;
    rot = obj->unk80[3] + 2;
    if (rot < 0) {
        rot = 0;
    } else if (rot > 7) {
        rot = 7;
    }
    idx = (u16)(((rot + sNFLEyeTypeRotations[obj->unk80[4]] - 0x20) << 16) / 32);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->unk110, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->unk10, obj->unk14);
    NFLi_PatchFacePart(obj, &obj->unk128, -sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->unk10, obj->unk14);
    if (brow + d < 0) {
        brow = 0;
    } else if (brow + d > 11) {
        brow = 11;
    } else {
        brow += d;
    }
    idx = (u16)((brow + sNFLEyebrowTypeRotations[obj->unk80[7]] - 0x20) << 11);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->unkE0, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    NFLi_PatchFacePart(obj, &obj->unkF8, -sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    switch (type) {
    case 2:
    case 6:
    case 7:
        y += 0x1E60;
        break;
    }
    if (obj->unkC4.x != 0) {
        v = -y / 32;
    } else {
        v = y / 32;
    }
    obj->unk158.param[obj->unkD0] = v;
    obj->unk160.param[obj->unkD0] = v;
    sin = (fx16)FX_SinIdx(0);
    cos = (fx16)FX_CosIdx(0);
    NFLi_PatchFacePart(obj, &obj->unk140, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, sNFLWork->unk10[3].unk0, obj->unk24);
}
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma optimization_level reset

// Not matching yet: as NFLi_ApplyExpression2, with d = -2 and an eyebrow rotation past 11 set to 0.
#pragma optimization_level 2
#pragma opt_common_subs off
#pragma opt_propagation off
void NFLi_ApplyExpression3(NFLObj *obj) {
    fx16 sin;
    fx16 cos;
    fx32 y;
    int type;
    u16 idx;
    fx32 v;
    int rot;
    int brow;
    int d;

    y = -(obj->unk80[2] * 0x10E1 - 0x22490);
    type = obj->unk80[5];
    brow = obj->unk80[6];
    d = -2;
    rot = obj->unk80[3] + -2;
    if (rot < 0) {
        rot = 0;
    } else if (rot > 7) {
        rot = 7;
    }
    idx = (u16)(((rot + sNFLEyeTypeRotations[obj->unk80[4]] - 0x20) << 16) / 32);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->unk110, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->unk10, obj->unk14);
    NFLi_PatchFacePart(obj, &obj->unk128, -sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->unk10, obj->unk14);
    if (brow + d < 0) {
        brow = 0;
    } else if (brow + d > 11) {
        brow = 0;
    } else {
        brow += d;
    }
    idx = (u16)((brow + sNFLEyebrowTypeRotations[obj->unk80[7]] - 0x20) << 11);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->unkE0, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    NFLi_PatchFacePart(obj, &obj->unkF8, -sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    switch (type) {
    case 2:
    case 6:
    case 7:
        y += 0x1E60;
        break;
    }
    if (obj->unkC4.x != 0) {
        v = -y / 32;
    } else {
        v = y / 32;
    }
    obj->unk158.param[obj->unkD0] = v;
    obj->unk160.param[obj->unkD0] = v;
    sin = (fx16)FX_SinIdx(0);
    cos = (fx16)FX_CosIdx(0);
    NFLi_PatchFacePart(obj, &obj->unk140, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, sNFLWork->unk10[4].unk0, obj->unk24);
}
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma optimization_level reset

// Not matching yet: as NFLi_ApplyExpression1.
#pragma optimization_level 2
#pragma opt_common_subs off
#pragma opt_propagation off
void NFLi_ApplyExpression4(NFLObj *obj) {
    fx16 sin;
    fx16 cos;
    fx32 y;
    int type;
    u16 idx;
    fx32 v;
    int rot;

    rot = obj->unk80[3] + (sNFLEyeTypeRotations[obj->unk80[3]] - sNFLEyeTypeRotations[49]);
    y = -((obj->unk80[2] - 2) * 0x10E1 - 0x22490);
    type = obj->unk80[5];
    if (rot < 0) {
        rot = 0;
    } else if (rot > 7) {
        rot = 7;
    }
    idx = (u16)(((rot + sNFLEyeTypeRotations[obj->unk80[4]] - 0x20) << 16) / 32);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->unk110, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->unk18, obj->unk1C);
    NFLi_PatchFacePart(obj, &obj->unk128, -sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->unk18, obj->unk1C);
    idx = (u16)((obj->unk80[6] + sNFLEyebrowTypeRotations[obj->unk80[7]] - 0x20) << 11);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->unkE0, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    NFLi_PatchFacePart(obj, &obj->unkF8, -sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    switch (type) {
    case 2:
    case 6:
    case 7:
        y += 0x1E60;
        break;
    }
    if (obj->unkC4.x != 0) {
        v = -y / 32;
    } else {
        v = y / 32;
    }
    obj->unk158.param[obj->unkD0] = v;
    obj->unk160.param[obj->unkD0] = v;
    sin = (fx16)FX_SinIdx(0);
    cos = (fx16)FX_CosIdx(0);
    NFLi_PatchFacePart(obj, &obj->unk140, sin, cos, obj->unk90, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->unk20, obj->unk24);
}
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma optimization_level reset

int NFLi_PrepareFacePartTextures(NFLMakeCtx *ctx);
int NFLi_AllocExpressionTextures(NFLMakeCtx *ctx);
void NFLi_PlaceFaceParts(NFLMakeCtx *ctx);

int NFLi_PrepareFaceParts(NFLMakeCtx *ctx) {
    int result = NFLi_PrepareFacePartTextures(ctx);

    if (result != 0) {
        return result;
    }
    result = NFLi_AllocExpressionTextures(ctx);
    if (result != 0) {
        return result;
    }
    NFLi_PlaceFaceParts(ctx);
    return result;
}

NFLObj *NFLi_GetFace(int index);
void NFLi_ApplyExpression0(NFLObj *obj);
void NFLi_ApplyExpression5(NFLObj *obj);
void NFLi_ApplyExpression6(NFLObj *obj);
void NFLi_ApplyExpression1(NFLObj *obj);
void NFLi_ApplyExpression2(NFLObj *obj);
void NFLi_ApplyExpression3(NFLObj *obj);
void NFLi_ApplyExpression4(NFLObj *obj);

int NFL_SetExpression(int index, int type) {
    NFLObj *obj;

    if (!NFLi_IsInitialized()) {
        return 4;
    }
    obj = NFLi_GetFace(index);
    if (obj == NULL) {
        return 9;
    }
    switch (type) {
    case 5:
        if (obj->unk16C & 0x20) {
            NFLi_ApplyExpression5(obj);
            obj->unk168 = 5;
            return 0;
        }
        break;
    case 6:
        if (obj->unk16C & 0x40) {
            NFLi_ApplyExpression6(obj);
            obj->unk168 = 6;
            return 0;
        }
        break;
    case 1:
        if (obj->unk16C & 0x2) {
            NFLi_ApplyExpression1(obj);
            obj->unk168 = 1;
            return 0;
        }
        break;
    case 2:
        if (obj->unk16C & 0x4) {
            NFLi_ApplyExpression2(obj);
            obj->unk168 = 2;
            return 0;
        }
        break;
    case 3:
        if (obj->unk16C & 0x8) {
            NFLi_ApplyExpression3(obj);
            obj->unk168 = 3;
            return 0;
        }
        break;
    case 4:
        if (obj->unk16C & 0x10) {
            NFLi_ApplyExpression4(obj);
            obj->unk168 = 4;
            return 0;
        }
        break;
    }
    NFLi_ApplyExpression0(obj);
    obj->unk168 = 0;
    return 0;
}

int NFL_GetExpression(int index) {
    NFLObj *obj;

    if (!NFLi_IsInitialized()) {
        return -1;
    }
    obj = NFLi_GetFace(index);
    if (obj == NULL) {
        return -1;
    }
    return obj->unk168;
}

// The resource stream's layout: where the work memory goes and how (unk8 is 3 or 7 in places)
typedef struct NFLDesc {
    u32 unk0;
    u32 unk4;
    u32 unk8;
} NFLDesc;

int NFLi_ResetSharedTextures(u8 *p);
int NFLi_SetResource(void *p);
extern const u8 sNFLSharedStreamFlags[];

// The library's initialisation: main passes NFL_Res.dat's data as res
int NFL_Init(NFLWork *work, void *res, void *buf, u32 size, const NFLDesc *desc, u8 flags, u8 max) {
    u32 seed = 0;
    int n = 0;
    int i;
    RTCDate date;
    RTCTime time;
    OSIntrMode enabled;
    NFLWorkFrom74 *sub;

    if (work == NULL || res == NULL || buf == NULL || desc == NULL) {
        return 8;
    }
    if (size & 3) {
        return 8;
    }
    NFLi_ResetSharedTextures((u8 *)work);
    if (RTC_GetDateTime(&date, &time) == RTC_RESULT_SUCCESS) {
        seed = RTC_ConvertDateTimeToSecond(&date, &time);
    }
    work->unk58 = seed;
    work->unk5C = 0x5D588B65;
    work->unk60 = 0x269EC3;
    work->unk0 = desc->unk0;
    work->unk4 = desc->unk4;
    work->unk68 = desc->unk8;
    work->unk71 = flags;
    work->unk74 = NULL;
    work->unk8C = 4;
    work->unk90 = 2;
    work->unk78 = buf;
    work->unk7C = size;
    work->unk80 = 0;
    work->unk84 = 0;
    work->unk88 = 0;
    work->unk94 = 0;
    work->unk98 = 0;
    sub = (NFLWorkFrom74 *)&work->unk74;
    for (i = 0; i < 5; i++) {
        if (flags & sNFLSharedStreamFlags[i]) {
            sub->unkA0[i].unk8 = 0;
            n++;
        } else {
            sub->unkA0[i].unk8 = 7;
        }
    }
    work->unk70 = max;
    work->unk6C = NULL;
    work->unkA0[0].unk8 = 0;
    if (desc->unk8 == 7) {
        work->unkA0[6].unk8 = 7;
    } else {
        work->unkA0[6].unk8 = 0;
    }
    if (desc->unk8 == 3) {
        work->unk8 = desc->unk0 + ((n << 11) + 0x200) / 4;
        work->unkC = desc->unk4 + 0x20;
    } else {
        work->unk8 = desc->unk0 + (n << 11) + 0x200;
        work->unkC = desc->unk4;
    }
    enabled = OS_DisableInterrupts();
    sNFLWork = work;
    NFLi_SetResource(res);
    OS_RestoreInterrupts(enabled);
    return 0;
}

BOOL NFLi_IsInitialized(void) {
    return sNFLWork != NULL;
}

int NFLi_ResetSharedTextures(u8 *p) {
    *(u32 *)(p + 0x18) = 0;
    *(u32 *)(p + 0x24) = 0;
    *(u32 *)(p + 0x30) = 0;
    *(u32 *)(p + 0x3c) = 0;
    *(u32 *)(p + 0x48) = 0;
    *(u32 *)(p + 0x54) = 0;
    return 0;
}

int NFL_AddFace(NFLObj *obj) {
    OSIntrMode enabled;
    NFLWork *work;
    NFLObj *last;
    int n;

    if (obj == NULL) {
        return 8;
    }
    enabled = OS_DisableInterrupts();
    if (!NFLi_IsInitialized()) {
        OS_RestoreInterrupts(enabled);
        return 4;
    }
    work = sNFLWork;
    if (work->unk8C == 1 || work->unk8C == 3) {
        OS_RestoreInterrupts(enabled);
        return 6;
    }
    if (work->unk70 == 0) {
        OS_RestoreInterrupts(enabled);
        return 0x10;
    }
    last = work->unk6C;
    if (last == NULL) {
        work->unk6C = obj;
        obj->next = NULL;
        obj->unk170 = 4;
    } else {
        if (last == obj) {
            OS_RestoreInterrupts(enabled);
            return 0x11;
        }
        n = 1;
        while (last->next != NULL) {
            if (last->next == obj) {
                OS_RestoreInterrupts(enabled);
                return 0x11;
            }
            last = last->next;
            n++;
        }
        if (n >= work->unk70) {
            OS_RestoreInterrupts(enabled);
            return 0x10;
        }
        last->next = obj;
        obj->next = NULL;
        obj->unk170 = 4;
    }
    OS_RestoreInterrupts(enabled);
    return 0;
}

int NFL_AddFaceSlot(void) {
    if (!NFLi_IsInitialized()) {
        return 4;
    }
    sNFLWork->unk70++;
    return 0;
}

int NFL_RemoveFaceSlot(NFLObj **removed) {
    OSIntrMode enabled;
    NFLObj *obj = NULL;
    NFLWork *work;
    NFLObj *prev;
    int n;
    NFLObj *cur;

    enabled = OS_DisableInterrupts();
    if (!NFLi_IsInitialized()) {
        OS_RestoreInterrupts(enabled);
        return 4;
    }
    work = sNFLWork;
    if (work->unk8C == 1 || work->unk8C == 3) {
        OS_RestoreInterrupts(enabled);
        return 6;
    }
    if (work->unk70 != 0) {
        work->unk70--;
    }
    if (removed != NULL) {
        *removed = NULL;
    }
    prev = sNFLWork->unk6C;
    if (prev != NULL) {
        if (sNFLWork->unk70 == 0) {
            prev->unk170 = 4;
            obj = prev;
            sNFLWork->unk6C = NULL;
        } else {
            cur = prev->next;
            n = 1;
            while (cur != NULL) {
                if (cur->next == NULL) {
                    if (n >= sNFLWork->unk70) {
                        cur->unk170 = 4;
                        obj = cur;
                        prev->next = NULL;
                    }
                    break;
                }
                prev = cur;
                cur = cur->next;
                n++;
            }
        }
    }
    if (obj != NULL) {
        if (removed != NULL) {
            *removed = obj;
        }
        work = sNFLWork;
        prev = work->unk74;
        if (prev != NULL) {
            if (prev == obj) {
                work->unk74 = obj->unk17C;
            } else {
                for (cur = prev->unk17C; cur != NULL; cur = cur->unk17C) {
                    if (cur == obj) {
                        prev->unk17C = obj->unk17C;
                        break;
                    }
                    prev = cur;
                }
            }
        }
    }
    OS_RestoreInterrupts(enabled);
    return 0;
}

int NFL_ResetFace(int index) {
    OSIntrMode enabled = OS_DisableInterrupts();
    NFLObj *obj;

    if (!NFLi_IsInitialized()) {
        OS_RestoreInterrupts(enabled);
        return 4;
    }
    if (sNFLWork->unk8C == 1 || sNFLWork->unk8C == 3) {
        OS_RestoreInterrupts(enabled);
        return 6;
    }
    obj = NFLi_GetFace(index);
    if (obj == NULL) {
        OS_RestoreInterrupts(enabled);
        return 9;
    }
    obj->unk170 = 4;
    OS_RestoreInterrupts(enabled);
    return 0;
}

int NFL_FindResetFace(void) {
    int n;
    NFLObj *obj;
    NFLWork *work;
    OSIntrMode enabled = OS_DisableInterrupts();

    if (!NFLi_IsInitialized()) {
        OS_RestoreInterrupts(enabled);
        return -1;
    }
    work = sNFLWork;
    if (work->unk70 == 0) {
        OS_RestoreInterrupts(enabled);
        return -1;
    }
    n = 0;
    for (obj = work->unk6C; obj != NULL; obj = obj->next) {
        if (obj->unk170 == 4) {
            OS_RestoreInterrupts(enabled);
            return n;
        }
        n++;
    }
    OS_RestoreInterrupts(enabled);
    return -1;
}

NFLObj *NFLi_GetFace(int index) {
    int i;
    NFLObj *obj;
    NFLWork *work;
    OSIntrMode enabled = OS_DisableInterrupts();

    if (!NFLi_IsInitialized()) {
        OS_RestoreInterrupts(enabled);
        return NULL;
    }
    work = sNFLWork;
    if (index >= work->unk70) {
        OS_RestoreInterrupts(enabled);
        return NULL;
    }
    i = 0;
    for (obj = work->unk6C; obj != NULL; obj = obj->next) {
        if (i == index) {
            OS_RestoreInterrupts(enabled);
            return obj;
        }
        i++;
    }
    OS_RestoreInterrupts(enabled);
    return NULL;
}

u16 NFL_Random(u16 max);

// Makes a create ID from the time, a random number and the MAC address
BOOL NFL_MakeCreateId(NFLCreateId *id) {
    u16 random;
    s64 seconds;
    u32 t;
    RTCDate date;
    RTCTime time;
    u8 mac[6];

    if (RTC_GetDateTime(&date, &time) == RTC_RESULT_SUCCESS) {
        random = NFL_Random(4);
        seconds = RTC_ConvertDateTimeToSecond(&date, &time);

        if (seconds == -1) {
            return FALSE;
        }
        t = seconds >> 5;
        *(u16 *)&id->data[0] = 0;
        id->data[2] = ((t & 0x30000) >> 16) | (random << 2);
        id->data[3] = (t & 0xFF00) >> 8;
        id->data[4] = t;
        OS_GetMacAddress(mac);
        id->data[5] = mac[3];
        id->data[6] = mac[4];
        id->data[7] = mac[5];
    } else {
        return FALSE;
    }
    return TRUE;
}

void NFLi_SetUnassignedCreateId(u8 *p) {
    int i;

    p[0x18] = 0xE0;
    for (i = 1; i < 8; i++) {
        p[0x18 + i] = 0;
    }
}

// The days of each month, from month 0, with February's 29: the same table as lib/MiiData's sDaysInMonth
extern const int sNFLDaysInMonth[];

// Whether a Mii's data is valid: height and build at most 128, a favourite colour of the 12, a date that exists, and
// a create ID
BOOL NFLi_IsValidCharData(const NFLCharData *data) {
    if (data->height > 0x80) {
        return FALSE;
    }
    if (data->build > 0x80) {
        return FALSE;
    }
    if (data->favoriteColor > 11) {
        return FALSE;
    }
    if (data->birthMonth > 12) {
        return FALSE;
    }
    if (data->birthDay > sNFLDaysInMonth[data->birthMonth]) {
        return FALSE;
    }
    return !NFL_IsCreateIdZero(&data->createId);
}

BOOL NFL_IsCreateIdBit7Clear(const NFLCreateId *id) {
    return !(id->data[0] & 0x80);
}

u32 NFLi_GetCreateIdBit6(const NFLCreateId *id) {
    return id->data[0] & 0x40;
}

// Whether the create ID is all zero
BOOL NFL_IsCreateIdZero(const NFLCreateId *id) {
    return *(u32 *)&id->data[0] == 0 && *(u32 *)&id->data[4] == 0;
}

BOOL NFL_IsSameCreateIdSource(const NFLCreateId *a, const NFLCreateId *b) {
    int i;

    if (NFL_IsCreateIdZero(b) || NFL_IsCreateIdUnassigned(b)) {
        return FALSE;
    }
    if ((a->data[2] & 0xF) != (b->data[2] & 0xF)) {
        return FALSE;
    }
    for (i = 3; i < 8; i++) {
        if (a->data[i] != b->data[i]) {
            return FALSE;
        }
    }
    return TRUE;
}

BOOL NFL_IsCreateIdUnassigned(const NFLCreateId *id) {
    return (id->data[0] & 0x20) ? TRUE : FALSE;
}

// Whether two create IDs are equal and neither is all zero
BOOL NFL_AreCreateIdsEqual(const NFLCreateId *a, const NFLCreateId *b) {
    const u8 *p = a->data;
    const u8 *q = b->data;
    int i;

    if (NFL_IsCreateIdZero(a) || NFL_IsCreateIdZero(b)) {
        return FALSE;
    }
    for (i = 0; i < 8; i++) {
        if (*p != *q) {
            return FALSE;
        }
        p++;
        q++;
    }
    return TRUE;
}

void NFL_ClearCreateId(NFLCreateId *id) {
    *(u32 *)&id->data[0] = 0;
    *(u32 *)&id->data[4] = 0;
}

// What NFL_GetCharInfo makes of a Mii's data
typedef struct NFLCharInfo {
    /* 0x00 */ u16 name[11];
    /* 0x16 */ u16 creatorName[11];
    /* 0x2C */ NFLCreateId createId;
    /* 0x34 */ u32 sex : 1;
               u32 birthMonth : 4;
               u32 birthDay : 5;
               u32 favoriteColor : 4;
               u32 unknown00Bit0 : 1;
               u32 height : 7;
               u32 build : 7;
               u32 unk34Bits29 : 2;
    /* 0x38 */ u16 unk38;
} NFLCharInfo;

u16 NFLi_GetFaceColor(int faceColor);

int NFL_GetCharInfo(NFLCharInfo *info, const NFLCharData *data) {
    u8 height;
    u8 build;

    if (!NFLi_IsValidCharData(data)) {
        return 7;
    }
    MI_CpuClear16(info->name, sizeof(info->name));
    MI_CpuClear16(info->creatorName, sizeof(info->creatorName));
    MI_CpuCopy16(data->name, info->name, sizeof(data->name));
    MI_CpuCopy16(data->creatorName, info->creatorName, sizeof(data->creatorName));
    MI_CpuCopy8(&data->createId, &info->createId, sizeof(NFLCreateId));
    height = data->height;
    build = data->build;
    if (height > 0x7F) {
        height = 0x7F;
    }
    if (build > 0x7F) {
        build = 0x7F;
    }
    info->sex = data->sex;
    info->birthMonth = data->birthMonth;
    info->birthDay = data->birthDay;
    info->favoriteColor = data->favoriteColor;
    info->unknown00Bit0 = data->unk00Bit0;
    info->height = height;
    info->build = build;
    info->unk34Bits29 = (*(const u32 *)&data->createId & 0x80000000) ? 0 : 3;
    info->unk38 = NFLi_GetFaceColor(data->faceColor);
    return 0;
}

u16 NFLi_GetFavoriteColor(int index);

u16 NFL_GetFavoriteColor(int index) {
    return NFLi_GetFavoriteColor(index);
}

// The create-ID helpers above, on a Mii's data
BOOL NFL_IsMiiCreateIdBit7Clear(const NFLCharData *data) {
    return NFL_IsCreateIdBit7Clear(&data->createId);
}

u32 NFL_GetMiiCreateIdBit6(const NFLCharData *data) {
    return NFLi_GetCreateIdBit6(&data->createId);
}

BOOL NFL_IsSameMiiCreateIdSource(const NFLCreateId *a, const NFLCharData *b) {
    return NFL_IsSameCreateIdSource(a, &b->createId);
}

BOOL NFL_IsMiiCreateIdUnassigned(const NFLCharData *data) {
    return NFL_IsCreateIdUnassigned(&data->createId);
}

BOOL NFL_AreMiiCreateIdsEqual(const NFLCharData *a, const NFLCharData *b) {
    return NFL_AreCreateIdsEqual(&a->createId, &b->createId);
}

void NFL_ClearMiiCreateId(NFLCharData *data) {
    NFL_ClearCreateId(&data->createId);
}

int NFLi_SetResource(void *p) {
    OSIntrMode enabled = OS_DisableInterrupts();
    NFLWork *work = sNFLWork;

    if (work == NULL) {
        OS_RestoreInterrupts(enabled);
        return 4;
    }
    if (work->unk8C == 1 || work->unk8C == 3) {
        OS_RestoreInterrupts(enabled);
        return 6;
    }
    work->unk64 = p;
    OS_RestoreInterrupts(enabled);
    return 0;
}

BOOL NFLi_HasResource(void) {
    return sNFLWork->unk64 != NULL;
}

BOOL NFLi_HasBuffer(void) {
    return sNFLWork->unk78 != NULL;
}

extern const NFLCharData sNFLDefaultMiis[];

// Copies Mii index (0 to 5) of the library's table into data, and gives it a blank create ID
int NFL_CopyDefaultMii(NFLCharData *data, u32 index) {
    if (index > 5) {
        return 8;
    }
    if (data == NULL) {
        return 8;
    }
    MI_CpuCopy8(&sNFLDefaultMiis[index], data, sizeof(NFLCharData));
    NFLi_SetUnassignedCreateId((u8 *)data);
    return 0;
}

void NFLi_MakeRandomMiiOf(NFLCharData *data, int sex, int b, int c);

int NFL_MakeRandomMii(NFLCharData *data, int sex, int b, int c) {
    if (sNFLWork == NULL) {
        return 4;
    }
    if (data == NULL) {
        return 8;
    }
    if (sex < 0 || sex > 2) {
        return 8;
    }
    // The original tests sex again where b and c would be meant
    if (b < 0 || sex > 3) {
        return 8;
    }
    if (c < 0 || sex > 3) {
        return 8;
    }
    NFLi_MakeRandomMiiOf(data, sex, b, c);
    NFLi_SetUnassignedCreateId((u8 *)data);
    return 0;
}

u16 NFL_Random(u16 max);
void NFLi_MakeRandomMii(NFLCharData *data, int sex, int b, int c);

// Picks at random what is 2 (sex) or 3 (b and c) on entry, then makes the Mii
void NFLi_MakeRandomMiiOf(NFLCharData *data, int sex, int b, int c) {
    int d;
    u16 r;

    if (sex == 2) {
        if (NFL_Random(2) == 0) {
            sex = 0;
        } else {
            sex = 1;
        }
    }
    if (b == 3) {
        r = NFL_Random(10);
        if (r < 4) {
            b = 0;
        } else if (r < 8) {
            b = 1;
        } else {
            b = 2;
        }
    }
    switch (c) {
    case 0:
        d = 0;
        break;
    case 1:
        d = 1;
        break;
    case 2:
        d = 2;
        break;
    case 3:
        r = NFL_Random(10);
        if (r < 4) {
            d = 2;
        } else if (r < 8) {
            d = 1;
        } else {
            d = 0;
        }
        break;
    }
    NFLi_MakeRandomMii(data, sex, b, d);
}


u8 NFLi_RandomFaceType(int sex, int b, int c);
u8 NFLi_RandomHairType(int sex, int b, int c);
u8 NFLi_RandomEyeType(int sex, int b, int c);
u8 NFLi_RandomEyebrowType(int sex, int b, int c);
u8 NFLi_RandomNoseType(int sex, int b, int c);
u8 NFLi_RandomMouthType(int sex, int b, int c);
u8 NFLi_RandomFaceTexture(int sex, int b, int c);
u8 NFLi_RandomGlassesType(int b);
u8 NFLi_RandomFaceColor(int sex, int c);
u8 NFLi_RandomHairColor(int b, int c);
u8 NFLi_RandomEyeColor(int c);

extern const u8 sNFLEyeTypeRotations[];
extern const u8 sNFLEyebrowTypeRotations[];

// Makes a Mii's data at random, for sex, b and c: the parts from the tables of the functions after it, the face's
// parts placed lower by up to 4 for sex 1 and for b 0, a beard on one man in 5 of b 1 and 2
void NFLi_MakeRandomMii(NFLCharData *data, int sex, int b, int c) {
    u8 offset = 0;
    int i;

    if (sex == 1) {
        offset += NFL_Random(3);
    }
    if (b == 0) {
        offset += NFL_Random(3);
    }
    data->faceType = NFLi_RandomFaceType(sex, b, c);
    data->faceColor = NFLi_RandomFaceColor(sex, c);
    data->faceTexture = NFLi_RandomFaceTexture(sex, b, c);
    data->hairType = NFLi_RandomHairType(sex, b, c);
    data->hairColor = NFLi_RandomHairColor(b, c);
    data->hairFlip = NFL_Random(2);
    data->eyeType = NFLi_RandomEyeType(sex, b, c);
    data->eyeColor = NFLi_RandomEyeColor(c);
    data->eyeScale = 4;
    if (sex == 0) {
        data->eyeRotation = 4;
        data->eyeRotation += sNFLEyeTypeRotations[2] - sNFLEyeTypeRotations[data->eyeType];
    } else {
        data->eyeRotation = 3;
        data->eyeRotation += sNFLEyeTypeRotations[4] - sNFLEyeTypeRotations[data->eyeType];
    }
    data->eyeX = 2;
    data->eyeY = offset + 12;
    data->eyebrowType = NFLi_RandomEyebrowType(sex, b, c);
    data->eyebrowColor = data->hairColor;
    data->eyebrowScale = 4;
    data->eyebrowRotation = 6;
    data->eyebrowX = 2;
    if (c == 2) {
        data->eyebrowY = offset + 9;
        data->eyebrowRotation += sNFLEyebrowTypeRotations[6] - sNFLEyebrowTypeRotations[data->eyebrowType];
    } else {
        data->eyebrowY = offset + 10;
        data->eyebrowRotation += sNFLEyebrowTypeRotations[0] - sNFLEyebrowTypeRotations[data->eyebrowType];
    }
    data->noseType = NFLi_RandomNoseType(sex, b, c);
    if (sex == 0) {
        data->noseScale = 4;
    } else {
        data->noseScale = 3;
    }
    data->noseY = offset + 9;
    data->mouthType = NFLi_RandomMouthType(sex, b, c);
    if (sex == 0) {
        data->mouthColor = 0;
    } else {
        data->mouthColor = NFL_Random(3);
    }
    data->mouthScale = 4;
    data->mouthY = offset + 13;
    if (sex == 0 && (b == 1 || b == 2) && NFL_Random(10) < 2) {
        u16 r = NFL_Random(3);

        if (r == 0) {
            data->mustacheType = 0;
            data->beardType = NFL_Random(3) + 1;
        } else if (r == 1) {
            data->mustacheType = NFL_Random(3) + 1;
            data->beardType = 0;
        } else if (r == 2) {
            data->mustacheType = NFL_Random(3) + 1;
            data->beardType = NFL_Random(3) + 1;
        }
        data->beardColor = data->hairColor;
        data->beardScale = 4;
        data->beardY = 10;
    } else {
        data->mustacheType = 0;
        data->beardType = 0;
        data->beardColor = data->hairColor;
        data->beardScale = 4;
        data->beardY = offset + 10;
    }
    data->glassesType = NFLi_RandomGlassesType(b);
    data->glassesColor = 0;
    data->glassesScale = 4;
    data->glassesY = offset + 10;
    data->moleType = 0;
    data->moleScale = 4;
    data->moleX = 2;
    data->moleY = 20;
    data->height = 64;
    data->build = 64;
    for (i = 0; i < 10; i++) {
        data->name[i] = 0;
    }
    for (i = 0; i < 10; i++) {
        data->creatorName[i] = 0;
    }
    for (i = 0; i < 8; i++) {
        data->createId.data[i] = 0;
    }
    data->sex = sex;
    data->birthMonth = 0;
    data->birthDay = 0;
    data->favoriteColor = NFL_Random(12);
    data->unk00Bit0 = 0;
    data->unk20Bit2 = 0;
}

extern const u8 sNFLRandomFaceTypes[][10];

u8 NFLi_RandomFaceType(int sex, int b, int c) {
    return sNFLRandomFaceTypes[sex * 9 + b * 3 + c][NFL_Random(10)];
}

// The hair type of a random Mii: one of the choices of row sex * 9 + b * 3 + c
u8 NFLi_RandomHairType(int sex, int b, int c) {
    u8 choices[18][72] = {
        {13, 23, 30, 31, 32, 33, 34, 35, 36, 37, 38, 40, 43, 44, 45, 47, 48, 49, 50, 51, 52, 54, 56, 57, 64, 66},
        {13, 23, 30, 31, 32, 33, 34, 35, 36, 37, 38, 40, 43, 44, 45, 47, 48, 49, 50, 51, 52, 54, 56, 57, 64, 66},
        {13, 23, 30, 31, 32, 33, 34, 35, 36, 37, 38, 40, 43, 44, 45, 47, 48, 49, 50, 51, 52, 54, 56, 57, 64, 66},
        {13, 23, 30, 31, 32, 33, 34, 36, 37, 38, 40, 42, 43, 44, 45, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 58, 59, 60, 64, 65, 66, 67, 68, 70},
        {13, 23, 30, 31, 32, 33, 34, 36, 37, 38, 39, 40, 43, 44, 45, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 58, 59, 60, 64, 65, 66, 67, 68, 70},
        {13, 23, 30, 31, 32, 33, 34, 36, 37, 38, 39, 40, 43, 44, 45, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 58, 59, 60, 64, 65, 66, 67, 68, 70},
        {13, 23, 30, 36, 37, 41, 45, 47, 51, 53, 54, 55, 58, 59, 65, 67},
        {13, 23, 30, 36, 37, 39, 41, 45, 47, 51, 53, 54, 55, 58, 59, 65, 67},
        {13, 23, 30, 36, 37, 39, 41, 45, 47, 51, 53, 54, 55, 58, 59, 65, 67},
        {0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24, 25, 26, 28, 46, 50, 61, 62, 63, 64, 69},
        {0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24, 25, 26, 28, 46, 50, 61, 62, 63, 64, 69},
        {0, 1, 2, 3, 4, 5, 6, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24, 25, 26, 28, 46, 50, 61, 62, 63, 64, 69},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24, 25, 26, 27, 29, 42, 50, 58, 60, 62, 63, 64, 69, 71},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24, 25, 26, 27, 29, 50, 58, 60, 62, 63, 64, 69, 71},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 24, 25, 26, 27, 29, 50, 58, 60, 62, 63, 64, 69, 71},
        {0, 1, 2, 3, 4, 5, 6, 10, 11, 12, 13, 14, 16, 17, 18, 20, 21, 24, 25, 58, 62, 69},
        {0, 1, 2, 3, 4, 5, 6, 10, 11, 12, 13, 14, 16, 17, 18, 20, 21, 24, 25, 58, 62, 69},
        {0, 1, 2, 3, 4, 5, 6, 10, 11, 12, 13, 14, 16, 17, 18, 20, 21, 24, 25, 58, 62, 69},
    };
    u8 counts[18] = {26, 26, 26, 34, 34, 34, 16, 17, 17, 33, 33, 33, 37, 36, 36, 22, 22, 22};
    u8 index = sex * 9 + b * 3 + c;

    return choices[index][NFL_Random(counts[index])];
}

// The eye type of a random Mii: one of the choices of row sex * 9 + b * 3 + c
u8 NFLi_RandomEyeType(int sex, int b, int c) {
    u8 choices[18][48] = {
        {2, 3, 5, 7, 8, 9, 11, 12, 13, 15, 16, 18, 27, 29, 32, 34, 36, 38, 39, 41, 43, 47},
        {2, 3, 5, 7, 8, 9, 11, 12, 13, 15, 16, 18, 27, 29, 32, 34, 36, 38, 39, 41, 43, 47},
        {2, 3, 5, 7, 8, 9, 11, 12, 13, 15, 16, 18, 26, 27, 29, 32, 34, 36, 38, 39, 41, 43, 47},
        {2, 3, 5, 6, 7, 8, 9, 11, 12, 13, 15, 16, 17, 18, 21, 22, 27, 29, 31, 32, 34, 36, 37, 38, 39, 41, 43, 44, 47},
        {2, 3, 5, 6, 7, 8, 9, 11, 12, 13, 15, 16, 17, 18, 21, 22, 27, 29, 31, 32, 34, 36, 37, 38, 39, 41, 43, 44, 47},
        {2, 3, 5, 6, 7, 8, 9, 11, 12, 13, 15, 16, 18, 21, 22, 26, 27, 29, 31, 32, 34, 36, 37, 38, 39, 41, 43, 44, 47},
        {2, 3, 5, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 21, 22, 31, 32, 34, 36, 37, 39, 41, 44},
        {2, 3, 5, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 21, 22, 31, 32, 34, 36, 37, 39, 41, 44},
        {2, 3, 5, 7, 8, 9, 11, 12, 13, 14, 15, 16, 18, 21, 22, 26, 31, 32, 34, 36, 37, 39, 41, 44},
        {0, 1, 2, 4, 5, 7, 8, 9, 10, 11, 12, 13, 15, 16, 18, 19, 23, 24, 25, 27, 28, 29, 32, 33, 34, 35, 38, 39, 40, 41, 42, 45, 46, 47},
        {0, 1, 2, 4, 5, 7, 8, 9, 10, 11, 12, 13, 15, 16, 18, 19, 23, 24, 25, 27, 28, 29, 32, 33, 34, 35, 38, 39, 40, 41, 42, 45, 46, 47},
        {0, 1, 2, 4, 5, 7, 8, 9, 10, 11, 12, 13, 15, 16, 18, 19, 23, 24, 25, 26, 27, 28, 29, 32, 33, 34, 35, 38, 39, 40, 41, 42, 45, 46, 47},
        {0, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 18, 19, 20, 21, 23, 24, 25, 27, 28, 29, 30, 32, 33, 34, 35, 37, 38, 39, 40, 41, 42, 45, 46, 47},
        {0, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 16, 17, 18, 19, 20, 21, 23, 24, 25, 27, 28, 29, 30, 32, 33, 34, 35, 37, 38, 39, 40, 41, 42, 45, 46, 47},
        {0, 1, 2, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 15, 16, 18, 19, 20, 21, 23, 24, 25, 26, 27, 28, 29, 30, 32, 33, 34, 35, 37, 38, 39, 40, 41, 42, 45, 46, 47},
        {1, 2, 5, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 21, 32, 34, 37, 39, 41},
        {1, 2, 5, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 21, 32, 34, 37, 39, 41},
        {1, 2, 5, 7, 8, 9, 11, 12, 13, 14, 15, 16, 18, 21, 26, 32, 34, 37, 39, 41},
    };
    u8 counts[18] = {22, 22, 23, 29, 29, 29, 24, 24, 24, 34, 34, 35, 40, 40, 40, 20, 20, 20};
    u8 index = sex * 9 + b * 3 + c;

    return choices[index][NFL_Random(counts[index])];
}

extern const u8 sNFLRandomEyebrowTypes[][24];

// The eyebrow type of a random Mii: one of the choices of row sex * 9 + b * 3 + c
u8 NFLi_RandomEyebrowType(int sex, int b, int c) {
    u8 counts[18] = {18, 18, 18, 23, 23, 23, 21, 21, 21, 9, 9, 9, 11, 11, 11, 9, 9, 9};
    u8 index = sex * 9 + b * 3 + c;

    return sNFLRandomEyebrowTypes[index][NFL_Random(counts[index])];
}

// The nose type of a random Mii: one of the choices of row sex * 9 + b * 3 + c
u8 NFLi_RandomNoseType(int sex, int b, int c) {
    u8 choices[18][12] = {
        {0, 1, 2, 3, 4, 5, 7, 8, 10},
        {0, 1, 2, 3, 4, 5, 7, 8, 10},
        {0, 1, 2, 3, 4, 5, 7, 8, 10},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 11},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 11},
        {0, 1, 3, 4, 8, 10},
        {0, 1, 3, 4, 8, 10},
        {0, 1, 3, 4, 8, 10},
        {0, 1, 3, 4, 6, 8, 9, 10, 11},
        {0, 1, 3, 4, 6, 8, 9, 10, 11},
        {0, 1, 3, 4, 6, 8, 10, 11},
        {0, 1, 3, 4, 6, 8, 9, 10, 11},
        {0, 1, 3, 4, 6, 8, 9, 10, 11},
        {0, 1, 3, 4, 6, 8, 10, 11},
    };
    u8 counts[18] = {9, 9, 9, 12, 12, 11, 12, 12, 11, 6, 6, 6, 9, 9, 8, 9, 9, 8};
    u8 index = sex * 9 + b * 3 + c;

    return choices[index][NFL_Random(counts[index])];
}

// The mouth type of a random Mii: one of the choices of row sex * 9 + b * 3 + c
u8 NFLi_RandomMouthType(int sex, int b, int c) {
    u8 choices[18][24] = {
        {0, 2, 3, 6, 7, 8, 9, 10, 12, 14, 15, 17, 18, 19, 21, 22, 23},
        {0, 2, 3, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 19, 21, 22, 23},
        {0, 2, 3, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 17, 18, 19, 21, 22, 23},
        {0, 2, 3, 6, 7, 8, 9, 10, 12, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23},
        {0, 2, 3, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23},
        {0, 2, 3, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23},
        {0, 2, 3, 6, 7, 8, 9, 10, 12, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23},
        {0, 2, 3, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23},
        {0, 2, 3, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 14, 15, 17, 18, 19, 21, 22, 23},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 13, 14, 15, 17, 18, 19, 21, 22, 23},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 13, 14, 15, 17, 18, 19, 21, 22, 23},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 14, 15, 17, 18, 19, 21, 22, 23},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 13, 14, 15, 17, 18, 19, 21, 22, 23},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 13, 14, 15, 17, 18, 19, 21, 22, 23},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 14, 15, 16, 17, 18, 19, 21, 22, 23},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23},
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 12, 13, 14, 15, 16, 17, 18, 19, 21, 22, 23},
    };
    u8 counts[18] = {17, 19, 19, 19, 21, 21, 19, 21, 21, 19, 20, 20, 19, 20, 20, 20, 21, 21};
    u8 index = sex * 9 + b * 3 + c;

    return choices[index][NFL_Random(counts[index])];
}

extern const u8 sNFLRandomFaceTextures[][20];

u8 NFLi_RandomFaceTexture(int sex, int b, int c) {
    return sNFLRandomFaceTextures[sex * 9 + b * 3 + c][NFL_Random(20)];
}

u8 NFLi_RandomGlassesType(int b) {
    u16 r = NFL_Random(100);

    switch (b) {
    case 0:
        if (r < 90) {
            return 0;
        }
        if (r < 94) {
            return 1;
        }
        if (r < 96) {
            return 2;
        }
        return 3;
    case 1:
        if (r < 83) {
            return 0;
        }
        if (r < 86) {
            return 1;
        }
        if (r < 90) {
            return 2;
        }
        if (r < 93) {
            return 3;
        }
        if (r < 94) {
            return 4;
        }
        if (r < 96) {
            return 5;
        }
        if (r < 98) {
            return 6;
        }
        return 7;
    case 2:
        if (r < 78) {
            return 0;
        }
        if (r < 83) {
            return 1;
        }
        if (r < 93) {
            return 3;
        }
        if (r < 98) {
            return 6;
        }
        return 7;
    }
    return 0;
}

extern const u8 sNFLRandomFaceColors[][10];

u8 NFLi_RandomFaceColor(int sex, int c) {
    return sNFLRandomFaceColors[sex * 3 + c][NFL_Random(10)];
}

extern const u8 sNFLRandomHairColors[][20];

u8 NFLi_RandomHairColor(int b, int c) {
    return sNFLRandomHairColors[c * 3 + b][NFL_Random(20)];
}

extern const u8 sNFLRandomEyeColors[][10];

u8 NFLi_RandomEyeColor(int c) {
    return sNFLRandomEyeColors[c][NFL_Random(10)];
}

// A random number below max (a 16-bit one when max is 0), from the LCG in the work area
u16 NFL_Random(u16 max) {
    NFLWork *work = sNFLWork;

    u32 r;

    work->unk58 = work->unk5C * work->unk58 + work->unk60;
    if (max == 0) {
        r = work->unk58;
    } else {
        r = (work->unk58 >> 16) * max;
    }
    return r >> 16;
}

// The coordinate helpers: a vector in the library's axes to the drawing context's, each component to the axis its
// index says, negated where its flag says
void NFLi_MapVecFx16(const NFLDrawContext *ctx, VecFx16 *out, const VecFx16 *in) {
    ((fx16 *)out)[ctx->unkD2] = ctx->unkC4.z ? -in->x : in->x;
    ((fx16 *)out)[ctx->unkD0] = ctx->unkC4.x ? -in->y : in->y;
    ((fx16 *)out)[ctx->unkD1] = ctx->unkC4.y ? -in->z : in->z;
}

void NFLi_MapVecFx32(const NFLDrawContext *ctx, VecFx32 *out, const VecFx32 *in) {
    ((fx32 *)out)[ctx->unkD2] = ctx->unkC4.z ? -in->x : in->x;
    ((fx32 *)out)[ctx->unkD0] = ctx->unkC4.x ? -in->y : in->y;
    ((fx32 *)out)[ctx->unkD1] = ctx->unkC4.y ? -in->z : in->z;
}

void NFLi_MapVecFx32NoNegate(const NFLDrawContext *ctx, VecFx32 *out, const VecFx32 *in) {
    ((fx32 *)out)[ctx->unkD2] = in->x;
    ((fx32 *)out)[ctx->unkD0] = in->y;
    ((fx32 *)out)[ctx->unkD1] = in->z;
}

void NFLi_MapVecFx32Copy(const NFLDrawContext *ctx, VecFx32 *out, const VecFx32 *in) {
    fx32 v[3];

    v[ctx->unkD2] = ctx->unkC4.z ? -in->x : in->x;
    v[ctx->unkD0] = ctx->unkC4.x ? -in->y : in->y;
    v[ctx->unkD1] = ctx->unkC4.y ? -in->z : in->z;
    out->x = v[0];
    out->y = v[1];
    out->z = v[2];
}

// Rotation about the context's z axis
void NFLi_MakeRotZMtx(const NFLDrawContext *ctx, MtxFx43 *mtx, fx32 sinVal, fx32 cosVal) {
    fx32 a[3];
    VecFx32 axis;

    a[ctx->unkD2] = 0;
    a[ctx->unkD0] = 0;
    a[ctx->unkD1] = ctx->unkC4.y ? -FX32_ONE : FX32_ONE;
    axis.x = a[0];
    axis.y = a[1];
    axis.z = a[2];
    MTX_RotAxis43(mtx, &axis, sinVal, cosVal);
}

void NFLi_DLTranslate(const NFLDrawContext *ctx, GXDLInfo *info, fx32 x, fx32 y, fx32 z) {
    VecFx32 in;
    VecFx32 out;

    in.x = x;
    in.y = y;
    in.z = z;
    NFLi_MapVecFx32(ctx, &out, &in);
    G3C_Translate(info, out.x, out.y, out.z);
}

// Not matching: the original loads G3C_Scale's arguments last to first (out[2], out[1], out[0]). The stores to in
// are the original's too, which keeps them only while its address is taken: hence the index i
void NFLi_DLScale(const NFLDrawContext *ctx, GXDLInfo *info, fx32 x, fx32 y, fx32 z) {
    VecFx32 in;
    fx32 out[3];
    int i = 0;

    in.x = x;
    in.y = y;
    in.z = z;
    out[ctx->unkD2] = ((fx32 *)&in)[i];
    out[ctx->unkD0] = ((fx32 *)&in)[i + 1];
    out[ctx->unkD1] = ((fx32 *)&in)[i + 2];
    G3C_Scale(info, out[0], out[1], out[2]);
}

void NFLi_DLVtx(const NFLDrawContext *ctx, GXDLInfo *info, fx16 x, fx16 y, fx16 z) {
    VecFx16 in;
    VecFx16 out;

    in.x = x;
    in.y = y;
    in.z = z;
    NFLi_MapVecFx16(ctx, &out, &in);
    G3C_Vtx(info, out.x, out.y, out.z);
}

void NFLi_DLNormal(const NFLDrawContext *ctx, GXDLInfo *info, fx16 x, fx16 y, fx16 z) {
    VecFx16 in;
    VecFx16 out;

    in.x = x;
    in.y = y;
    in.z = z;
    NFLi_MapVecFx16(ctx, &out, &in);
    G3C_Normal(info, out.x, out.y, out.z);
}

// Whether end bytes fit in the texture palette slots of a GXVRamTexPltt: 14 an unknown one, 13 too many
int NFLi_CheckTexPlttBank(int bank, u32 offset, u32 end) {
    switch (bank) {
    case 0x20:
    case 0x40:
        if (end > 0x4000) {
            return 13;
        }
        break;
    case 0x60:
        if (end > 0x8000) {
            return 13;
        }
        break;
    case 0x10:
        if (end > 0x10000) {
            return 13;
        }
        break;
    case 0x30:
        if (end > 0x14000) {
            return 13;
        }
        break;
    case 0x70:
        if (end > 0x18000) {
            return 13;
        }
        break;
    default:
        return 14;
    }
    return 0;
}

// Whether end bytes fit in the texture slots of a GXVRamTex: 12 none, 11 too many
int NFLi_CheckTexBank(int bank, u32 offset, u32 end) {
    switch (bank) {
    case 1:
    case 2:
    case 4:
    case 8:
        if (end > 0x20000) {
            return 11;
        }
        break;
    case 3:
    case 5:
    case 6:
    case 9:
    case 10:
    case 12:
        if (end > 0x40000) {
            return 11;
        }
        break;
    case 7:
    case 11:
    case 13:
    case 14:
        if (end > 0x60000) {
            return 11;
        }
        break;
    case 15:
        if (end > 0x80000) {
            return 11;
        }
        break;
    case 0:
    default:
        return 12;
    }
    return 0;
}

// Where each GXVRamTex's texture slots are in the LCDC's memory, in 4 KB: the first part, the second part, and the
// size of the first
typedef struct NFLTexBank {
    u16 unk0;
    u16 unk2;
    u16 unk4;
} NFLTexBank;

extern const NFLTexBank sNFLTexBankParts[];

// Copies size bytes of texture image to offset of the texture slots, through the LCDC, as GX_LoadTex does
// Not matching: register allocation only (the original keeps the second part's address in r4, which becomes dest)
int NFLi_CopyToTexBank(int bank, const void *src, u32 offset, u32 size) {
    u32 n;
    void *dest;
    int result;
    u32 second = sNFLTexBankParts[bank].unk2 << 12;
    u32 first = sNFLTexBankParts[bank].unk0;
    u32 split = sNFLTexBankParts[bank].unk4;
    u32 end = offset + size;

    result = NFLi_CheckTexBank(bank, offset, end);
    if (result != 0) {
        return result;
    }
    if (second == 0) {
        dest = (void *)(offset + (first << 12));
    } else if (end < split << 12) {
        dest = (void *)(offset + (first << 12));
    } else if (offset >= split << 12) {
        dest = (void *)(second + offset - (split << 12));
    } else {
        n = (split << 12) - offset;
        dest = (void *)(offset + (first << 12));
        MIi_CpuCopy32(src, dest, n);
        DC_FlushRange(dest, n);
        size -= n;
        MIi_CpuCopy32((const u8 *)src + n, (void *)second, size);
        DC_FlushRange((void *)second, size);
        return 0;
    }
    MIi_CpuCopy32(src, dest, size);
    DC_FlushRange(dest, size);
    return 0;
}

extern const u16 sNFLTexPlttBankBases[];

// Copies size bytes of palettes to offset of the texture palette slots, through the LCDC, as GX_LoadTexPltt does
int NFLi_CopyToTexPlttBank(int bank, const void *src, u32 offset, u32 size) {
    u32 base = sNFLTexPlttBankBases[bank >> 4] << 12;
    int result = NFLi_CheckTexPlttBank(bank, offset, offset + size);
    void *dest;

    if (result != 0) {
        return result;
    }
    dest = (void *)(offset + base);
    MIi_CpuCopy32(src, dest, size);
    DC_FlushRange(dest, size);
    return 0;
}

int func_0200473c(void);
int func_0200474c(void);

int NFLi_LoadTex(const void *src, u32 offset, u32 size) {
    int bank = func_0200473c();
    int result;

    GX_BeginLoadTex();
    result = NFLi_CopyToTexBank(bank, src, offset, size);
    GX_EndLoadTex();
    return result;
}

int NFLi_LoadTexPltt(const void *src, u32 offset, u32 size) {
    int bank = func_0200474c();
    int result;

    GX_BeginLoadTexPltt();
    result = NFLi_CopyToTexPlttBank(bank, src, offset, size);
    GX_EndLoadTexPltt();
    return result;
}

BOOL NFLi_CopySharedTextures(u32 *arg);
BOOL NFLi_CopySharedPltt(u32 *arg);
BOOL NFLi_CopyFaceTextures(NFLObj *obj, u32 *arg);
BOOL NFLi_CopyFacePltts(NFLObj *obj, u32 *arg);
void NFLi_RemoveFromLoadList(NFLObj *obj);

// The library's work of a frame, which main calls every frame: 4 not set up, 0x14 and 0x13 busy, 3 nothing to do
int NFL_FillBuffer(void) {
    NFLWork *work = sNFLWork;
    u32 arg;

    if (work == NULL) {
        return 4;
    }
    arg = work->unk88;
    if (work->unk8C == 3) {
        return 0x14;
    }
    if (work->unk8C == 1) {
        return 0x13;
    }
    if (work->unk8C == 4) {
        return 3;
    }
    work->unk8C = 1;
    work->unk9C = 0;
    switch (work->unk90) {
    case 0:
        NFLi_CopySharedTextures(&arg);
        work->unk8C = 2;
        break;
    case 1:
        NFLi_CopySharedPltt(&arg);
        work->unk8C = 2;
        break;
    case 2:
        if (work->unk74 == NULL) {
            work->unk8C = 4;
            work->unk88 = 0;
            work->unk80 = 0;
            work->unk84 = 0;
            return 3;
        }
        switch (work->unk74->unk170) {
        case 0:
            NFLi_CopyFaceTextures(work->unk74, &arg);
            work->unk8C = 2;
            break;
        case 1:
            NFLi_CopyFacePltts(work->unk74, &arg);
            work->unk8C = 2;
            break;
        case 2:
        case 4:
            NFLi_RemoveFromLoadList(work->unk74);
            work->unk8C = 0;
            break;
        }
        break;
    }
    work->unk88 = arg;
    return 0;
}



// Copies size bytes, or converts them through palette when there is one
void NFLi_CopyTexData(const void *src, void *dest, const u16 *palette, u32 size, BOOL arg4) {
    if (palette != NULL) {
        NFLi_Convert4BitToDirect(dest, src, palette, size, arg4);
    } else {
        MIi_CpuCopy32(src, dest, size);
    }
}

// What NFLi_CopySharedTextures copies to each of its six streams: a resource (NFLi_GetResourceItem's kind and index) and its size
typedef struct NFLStreamSource {
    int unk0;
    int unk4;
    u32 unk8;
} NFLStreamSource;

extern NFLStreamSource sNFLSharedStreamSources[];



// Not matching: register allocation only (the original has pal in r5 before stream, source, extra and size)
BOOL NFLi_CopySharedTextures(u32 *pos) {
    NFLWork *work = (NFLWork *)sNFLWork;
    NFLWorkUnkA0 *stream = work->unkA0;
    NFLStreamSource *source = sNFLSharedStreamSources;
    int i;
    u16 palette[16];

    for (i = 0; i < 6; stream++, source++, i++) {
        u32 extra;
        u16 *pal;
        u8 *src;
        int size;
        BOOL overflowB;
        BOOL overflowA;

        if (*pos >= work->unk7C) {
            return TRUE;
        }
        if (!work->unk9C) {
            if (stream->unk8 >= 2 && stream->unk8 <= 5) {
                work->unk9C = TRUE;
            } else if (stream->unk8 <= 1) {
                work->unk88 = 0;
                work->unk80 = 0;
                work->unk84 = 0;
                *pos = 0;
                work->unk9C = TRUE;
            }
        }
        if (stream->unk8 == 0 || stream->unk8 == 1 || stream->unk8 == 3 || stream->unk8 == 4) {
        extra = 0;
        overflowB = FALSE;
        overflowA = FALSE;
        if (sNFLWork->unk68 == 7) {
            size = source->unk8 * 4;
            if (source->unk0 == 2) {
                NFLi_MakeEyePltt(palette, 0);
                pal = palette;
            } else if (source->unk0 == 13) {
                NFLi_MakeMouthPltt(palette, 0);
                pal = palette;
            } else {
                pal = NFLi_GetResourceItem(12, 0);
            }
        } else {
            size = source->unk8;
            pal = NULL;
        }
        if (stream->unk8 == 0) {
            u32 offset;

            if (*pos + size > work->unk7C) {
                overflowA = TRUE;
                size = work->unk7C - *pos;
            }
            offset = work->unk84;
            if (offset + size > work->unk7C) {
                overflowB = TRUE;
            }
            if (!overflowB) {
                src = NFLi_GetResourceItem(source->unk0, source->unk4);
                NFLi_CopyTexData(src, work->unk78 + offset, pal, size, TRUE);
                DC_FlushRange(work->unk78 + work->unk84, size);
            } else {
                int over = work->unk84 + size - work->unk7C;
                int n;

                src = NFLi_GetResourceItem(source->unk0, source->unk4);
                n = size - over;
                NFLi_CopyTexData(src, work->unk78 + work->unk84, pal, n, TRUE);
                DC_FlushRange(work->unk78 + work->unk84, n);
                if (sNFLWork->unk68 == 7) {
                    src += n / 4;
                } else {
                    src += n;
                }
                NFLi_CopyTexData(src, work->unk78, pal, over, TRUE);
                DC_FlushRange(work->unk78, over);
            }
            if (!overflowA) {
                stream->unk8 = 2;
            } else {
                work->unk98 = size;
                stream->unk8 = 3;
            }
            stream->unk0 = work->unk84;
        } else {
            u8 *p = NFLi_GetResourceItem(source->unk0, source->unk4);

            switch (stream->unk8) {
            case 3:
                if (sNFLWork->unk68 == 7) {
                    p += (int)work->unk98 / 4;
                } else {
                    p += work->unk98;
                }
                size -= work->unk98;
                extra = 0;
                break;
            case 4:
                if (sNFLWork->unk68 == 7) {
                    p += (int)work->unk98 / 4;
                } else {
                    p += work->unk98;
                }
                size -= work->unk98;
                extra = work->unk94;
                break;
            case 1:
                extra = work->unk94;
                if (sNFLWork->unk68 == 7) {
                    p += (int)extra / 4;
                } else {
                    p += extra;
                }
                size -= extra;
                stream->unk0 = work->unk84;
                break;
            }
            if (*pos + size > work->unk7C) {
                overflowA = TRUE;
                size = work->unk7C - *pos;
            }
            if (work->unk84 + size > work->unk7C) {
                overflowB = TRUE;
            }
            if (!overflowA) {
                stream->unk8 = stream->unk8 == 3 ? 2 : 5;
                work->unk98 = 0;
            } else {
                switch (stream->unk8) {
                case 3:
                case 4:
                    work->unk98 += size;
                    break;
                case 1:
                    work->unk98 = work->unk94 + size;
                    stream->unk8 = 4;
                    break;
                }
            }
            if (!overflowB) {
                NFLi_CopyTexData(p, work->unk78 + work->unk84, palette, size, TRUE);
                DC_FlushRange(work->unk78 + work->unk84, size);
            } else {
                int over = work->unk84 + size - work->unk7C;
                int n = size - over;

                NFLi_CopyTexData(p, work->unk78 + work->unk84, palette, n, TRUE);
                DC_FlushRange(work->unk78 + work->unk84, n);
                if (sNFLWork->unk68 == 7) {
                    p += n / 4;
                } else {
                    p += n;
                }
                NFLi_CopyTexData(p, work->unk78, palette, over, TRUE);
                DC_FlushRange(work->unk78, over);
            }
        }
        switch (i) {
        case 0:
            stream->unk4 = sNFLWork->unk10[0].unk0 + extra;
            break;
        case 1:
            stream->unk4 = sNFLWork->unk10[1].unk0 + extra;
            break;
        case 2:
            stream->unk4 = sNFLWork->unk10[2].unk0 + extra;
            break;
        case 3:
            stream->unk4 = sNFLWork->unk10[3].unk0 + extra;
            break;
        case 4:
            stream->unk4 = sNFLWork->unk10[4].unk0 + extra;
            break;
        case 5:
            stream->unk4 = sNFLWork->unk10[5].unk0 + extra;
            break;
        }
        work->unk84 = (work->unk84 + size) % work->unk7C;
        *pos += size;
        }
    }
    if (sNFLWork->unk68 != 7) {
        NFLi_CopySharedPltt(pos);
    } else {
        if (work->unk74 == NULL) {
            return TRUE;
        }
        switch (work->unk74->unk170) {
        case 0:
            NFLi_CopyFaceTextures(work->unk74, pos);
            break;
        case 1:
            NFLi_CopyFacePltts(work->unk74, pos);
            break;
        }
    }
    return TRUE;
}

// The seventh stream of the work area: 32 bytes of resource 12, 0 (the palette NFLi_CopySharedTextures uses)
BOOL NFLi_CopySharedPltt(u32 *pos) {
    NFLWork *work = (NFLWork *)sNFLWork;

    if (work->unk68 != 7) {
        if (*pos >= work->unk7C) {
            return TRUE;
        }
        if (!work->unk9C) {
            if (work->unkA0[6].unk8 >= 2 && work->unkA0[6].unk8 <= 5) {
                work->unk9C = TRUE;
            } else if (work->unkA0[6].unk8 <= 1) {
                work->unk88 = 0;
                work->unk80 = 0;
                work->unk84 = 0;
                *pos = 0;
                work->unk9C = TRUE;
            }
        }
        if (work->unkA0[6].unk8 == 0 || work->unkA0[6].unk8 == 1 || work->unkA0[6].unk8 == 3 ||
            work->unkA0[6].unk8 == 4) {
            u32 extra = 0;
            BOOL overflowB = FALSE;
            BOOL overflowA = FALSE;
            int size = 32;
            u8 *src;

            if (work->unkA0[6].unk8 == 0) {
                u32 offset;

                if (*pos + size > work->unk7C) {
                    size = work->unk7C - *pos;
                    overflowA = TRUE;
                }
                offset = work->unk84;
                if (offset + size > work->unk7C) {
                    overflowB = TRUE;
                }
                if (!overflowB) {
                    u8 *buf = work->unk78;

                    src = NFLi_GetResourceItem(12, 0);
                    MIi_CpuCopy32(src, buf + offset, size);
                    DC_FlushRange(work->unk78 + work->unk84, size);
                } else {
                    int over = offset + size - work->unk7C;
                    int n;

                    src = NFLi_GetResourceItem(12, 0);
                    n = size - over;
                    MIi_CpuCopy32(src, work->unk78 + work->unk84, n);
                    DC_FlushRange(work->unk78 + work->unk84, n);
                    MIi_CpuCopy32(src + n, work->unk78, over);
                    DC_FlushRange(work->unk78, over);
                }
                if (!overflowA) {
                    work->unkA0[6].unk8 = 2;
                } else {
                    work->unk98 = size;
                    work->unkA0[6].unk8 = 3;
                }
                work->unkA0[6].unk0 = work->unk84;
            } else {
                u8 *p = NFLi_GetResourceItem(12, 0);

                switch (work->unkA0[6].unk8) {
                case 3:
                    p += work->unk98;
                    size -= work->unk98;
                    break;
                case 4:
                    p += work->unk98;
                    extra = work->unk94;
                    size -= work->unk98;
                    break;
                case 1:
                    extra = work->unk94;
                    work->unkA0[6].unk0 = work->unk84;
                    p += extra;
                    size -= extra;
                    break;
                }
                if (*pos + size > work->unk7C) {
                    size = work->unk7C - *pos;
                    overflowA = TRUE;
                }
                if (work->unk84 + size > work->unk7C) {
                    overflowB = TRUE;
                }
                if (!overflowA) {
                    work->unkA0[6].unk8 = work->unkA0[6].unk8 == 3 ? 2 : 5;
                    work->unk98 = 0;
                } else {
                    switch (work->unkA0[6].unk8) {
                    case 3:
                    case 4:
                        work->unk98 += size;
                        break;
                    case 1:
                        work->unk98 = work->unk94 + size;
                        work->unkA0[6].unk8 = 4;
                        break;
                    }
                }
                if (!overflowB) {
                    MIi_CpuCopy32(p, work->unk78 + work->unk84, size);
                    DC_FlushRange(work->unk78 + work->unk84, size);
                } else {
                    int over = work->unk84 + size - work->unk7C;
                    int n = size - over;

                    MIi_CpuCopy32(p, work->unk78 + work->unk84, n);
                    DC_FlushRange(work->unk78 + work->unk84, n);
                    if (sNFLWork->unk68 == 7) {
                        p += n / 4;
                    } else {
                        p += n;
                    }
                    MIi_CpuCopy32(p, work->unk78, over);
                    DC_FlushRange(work->unk78, over);
                }
            }
            work->unk84 = (work->unk84 + size) % work->unk7C;
            work->unkA0[6].unk4 = sNFLWork->unk10[5].unk4 + extra;
            *pos += size;
        }
    }
    if (*pos >= work->unk7C) {
        return TRUE;
    }
    if (work->unk74 == NULL) {
        return TRUE;
    }
    switch (work->unk74->unk170) {
    case 0:
        NFLi_CopyFaceTextures(work->unk74, pos);
        break;
    case 1:
        NFLi_CopyFacePltts(work->unk74, pos);
        break;
    }
    return TRUE;
}

// What NFLi_CopyFaceTextures copies to each of an object's eleven streams: a resource kind (NFLi_GetResourceItem's) and its size
typedef struct NFLObjSource {
    int unk0;
    u32 unk4;
} NFLObjSource;

extern NFLObjSource sNFLFaceStreamSources[];

void NFLi_MakeColorTextureDirect(u16 *palette, const s16 *params);
void NFLi_MakeColorTexture(u16 *palette);

// Not matching: register allocation only (the original has stream in r4, then size, pal, obj, work)
BOOL NFLi_CopyFaceTextures(NFLObj *obj, u32 *pos) {
    NFLWorkUnkA0 *stream;
    NFLWork *work = sNFLWork;
    s16 *params = obj->unk180;
    int i;
    u16 palette[64];

    stream = obj->unk1A4;

    for (i = 0; i < 11; stream++, i++) {
        int kind;
        int index;
        u16 *pal;
        u8 *src;
        int size;
        BOOL overflowB;
        BOOL overflowA;
        BOOL arg4;

        if (*pos >= work->unk7C) {
            return TRUE;
        }
        if (!work->unk9C) {
            if (stream->unk8 >= 2 && stream->unk8 <= 5) {
                work->unk9C = TRUE;
            } else if (stream->unk8 <= 1) {
                work->unk88 = 0;
                work->unk80 = 0;
                work->unk84 = 0;
                *pos = 0;
                work->unk9C = TRUE;
            }
        }
        if (stream->unk8 == 0 || stream->unk8 == 1 || stream->unk8 == 3 || stream->unk8 == 4) {
            kind = sNFLFaceStreamSources[i].unk0;
            index = stream->unk4;
            overflowB = FALSE;
            overflowA = FALSE;
            arg4 = TRUE;
            pal = sNFLWork->unk68 == 7 ? palette : NULL;
            if (sNFLWork->unk68 == 7) {
                if (kind == 21) {
                    size = 0x80;
                } else {
                    size = sNFLFaceStreamSources[i].unk4 * 4;
                }
            } else {
                size = sNFLFaceStreamSources[i].unk4;
            }
            switch (kind) {
            case 21:
                if (sNFLWork->unk68 == 7) {
                    NFLi_MakeColorTextureDirect(palette, params);
                } else {
                    NFLi_MakeColorTexture(palette);
                }
                if (stream->unk8 == 0) {
                    if (*pos + size > work->unk7C) {
                        overflowA = TRUE;
                        size = work->unk7C - *pos;
                    }
                    if (work->unk84 + size > work->unk7C) {
                        overflowB = TRUE;
                    }
                    if (!overflowB) {
                        MIi_CpuCopy32(palette, work->unk78 + work->unk84, size);
                        DC_FlushRange(work->unk78 + work->unk84, size);
                    } else {
                        int over = work->unk84 + size - work->unk7C;
                        int n = size - over;

                        MIi_CpuCopy32(palette, work->unk78 + work->unk84, n);
                        DC_FlushRange(work->unk78 + work->unk84, n);
                        MIi_CpuCopy32((u8 *)palette + over, work->unk78, over);
                        DC_FlushRange(work->unk78, over);
                    }
                    if (!overflowA) {
                        stream->unk8 = 2;
                    } else {
                        obj->unk178 = size;
                        stream->unk8 = 3;
                    }
                    stream->unk0 = work->unk84;
                } else {
                    u8 *p = (u8 *)palette;

                    switch (stream->unk8) {
                    case 3:
                    case 4:
                        size -= obj->unk178;
                        p += obj->unk178;
                        break;
                    case 1:
                        p += obj->unk174;
                        size -= obj->unk174;
                        stream->unk0 = work->unk84;
                        break;
                    }
                    if (*pos + size > work->unk7C) {
                        overflowA = TRUE;
                        size = work->unk7C - *pos;
                    }
                    if (work->unk84 + size > work->unk7C) {
                        overflowB = TRUE;
                    }
                    if (!overflowA) {
                        stream->unk8 = stream->unk8 == 3 ? 2 : 5;
                        obj->unk178 = 0;
                    } else {
                        switch (stream->unk8) {
                        case 3:
                        case 4:
                            obj->unk178 += size;
                            break;
                        case 1:
                            obj->unk178 = obj->unk174 + size;
                            stream->unk8 = 4;
                            break;
                        }
                    }
                    if (!overflowB) {
                        MIi_CpuCopy32(p, work->unk78 + work->unk84, size);
                        DC_FlushRange(work->unk78 + work->unk84, size);
                    } else {
                        int over = work->unk84 + size - work->unk7C;
                        int n = size - over;

                        MIi_CpuCopy32(p, work->unk78 + work->unk84, n);
                        DC_FlushRange(work->unk78 + work->unk84, n);
                        MIi_CpuCopy32(p + n, work->unk78, over);
                        DC_FlushRange(work->unk78, over);
                    }
                }
                work->unk84 = (work->unk84 + size) % work->unk7C;
                *pos += size;
                break;
            case 2:
                if (sNFLWork->unk68 == 7) {
                    NFLi_MakeEyePltt(pal, params[8]);
                }
                break;
            case 3:
                if (sNFLWork->unk68 == 7) {
                    NFLi_MakeHairPltt(pal, params[6]);
                }
                break;
            case 13:
                if (sNFLWork->unk68 == 7) {
                    NFLi_MakeMouthPltt(pal, params[10]);
                }
                break;
            case 14:
                if (sNFLWork->unk68 == 7) {
                    NFLi_MakeHairPltt(pal, params[14]);
                }
                break;
            case 8:
                if (sNFLWork->unk68 == 7) {
                    NFLi_MakeGlassesPltt(pal, params[12]);
                }
                break;
            case 1:
                switch (index) {
                case 2:
                    index = 0;
                    break;
                case 3:
                    index = 1;
                    break;
                }
                if (sNFLWork->unk68 == 7) {
                    NFLi_MakeHairPltt(pal, params[14]);
                }
                break;
            case 5:
                if (sNFLWork->unk68 == 7) {
                    NFLi_MakeFacePltt(pal, params[2]);
                }
                arg4 = FALSE;
                break;
            case 10:
                if (index == 34 || index == 57) {
                    switch (index) {
                    case 34:
                        index = 0;
                        break;
                    case 57:
                        index = 1;
                        break;
                    }
                    if (sNFLWork->unk68 == 7) {
                        NFLi_MakeFavoriteColorPltt(pal, params[0]);
                    }
                    kind = 19;
                    arg4 = FALSE;
                } else {
                    switch (index) {
                    case 5:
                        index = 0;
                        break;
                    case 7:
                        index = 1;
                        break;
                    case 24:
                        index = 2;
                        break;
                    case 29:
                        index = 3;
                        break;
                    case 42:
                        index = 4;
                        break;
                    case 53:
                        index = 5;
                        break;
                    case 58:
                        index = 6;
                        break;
                    case 69:
                        index = 7;
                        break;
                    case 71:
                        index = 8;
                        break;
                    }
                    if (sNFLWork->unk68 == 7) {
                        NFLi_MakeHairPltt(pal, params[4]);
                    }
                    if (params[3] == 29) {
                        arg4 = TRUE;
                    } else {
                        arg4 = FALSE;
                    }
                }
                break;
            case 17:
                if (sNFLWork->unk68 == 7) {
                    pal = NFLi_GetResourceItem(12, 0);
                }
                break;
            }
            if (kind != 21) {
                if (stream->unk8 == 0) {
                    u32 offset;

                    stream->unk0 = work->unk84;
                    if (*pos + size > work->unk7C) {
                        overflowA = TRUE;
                        size = work->unk7C - *pos;
                    }
                    offset = work->unk84;
                    if (offset + size > work->unk7C) {
                        overflowB = TRUE;
                    }
                    if (!overflowB) {
                        src = NFLi_GetResourceItem(kind, index);
                        NFLi_CopyTexData(src, work->unk78 + offset, pal, size, arg4);
                        DC_FlushRange(work->unk78 + work->unk84, size);
                    } else {
                        int over = work->unk84 + size - work->unk7C;
                        int n;

                        src = NFLi_GetResourceItem(kind, index);
                        n = size - over;
                        NFLi_CopyTexData(src, work->unk78 + work->unk84, pal, n, arg4);
                        DC_FlushRange(work->unk78 + work->unk84, n);
                        if (sNFLWork->unk68 == 7) {
                            src += n / 4;
                        } else {
                            src += n;
                        }
                        NFLi_CopyTexData(src, work->unk78, pal, over, arg4);
                        DC_FlushRange(work->unk78, over);
                    }
                    if (!overflowA) {
                        stream->unk8 = 2;
                    } else {
                        obj->unk178 = size;
                        stream->unk8 = 3;
                    }
                } else {
                    u8 *p2 = NFLi_GetResourceItem(kind, index);

                    switch (stream->unk8) {
                    case 3:
                    case 4:
                        if (sNFLWork->unk68 == 7) {
                            p2 += (int)obj->unk178 / 4;
                        } else {
                            p2 += obj->unk178;
                        }
                        size -= obj->unk178;
                        break;
                    case 1:
                        if (sNFLWork->unk68 == 7) {
                            p2 += (int)obj->unk174 / 4;
                        } else {
                            p2 += obj->unk174;
                        }
                        size -= obj->unk174;
                        stream->unk0 = work->unk84;
                        break;
                    }
                    if (*pos + size > work->unk7C) {
                        overflowA = TRUE;
                        size = work->unk7C - *pos;
                    }
                    if (work->unk84 + size > work->unk7C) {
                        overflowB = TRUE;
                    }
                    if (!overflowB) {
                        NFLi_CopyTexData(p2, work->unk78 + work->unk84, pal, size, arg4);
                        DC_FlushRange(work->unk78 + work->unk84, size);
                    } else {
                        int over = work->unk84 + size - work->unk7C;
                        int n = size - over;

                        NFLi_CopyTexData(p2, work->unk78 + work->unk84, pal, n, arg4);
                        DC_FlushRange(work->unk78 + work->unk84, n);
                        if (sNFLWork->unk68 == 7) {
                            p2 += n / 4;
                        } else {
                            p2 += n;
                        }
                        NFLi_CopyTexData(p2, work->unk78, pal, over, arg4);
                        DC_FlushRange(work->unk78, over);
                    }
                    if (!overflowA) {
                        stream->unk8 = stream->unk8 == 3 ? 2 : 5;
                        obj->unk178 = 0;
                    } else {
                        switch (stream->unk8) {
                        case 3:
                        case 4:
                            obj->unk178 += size;
                            break;
                        case 1:
                            obj->unk178 = obj->unk174 + size;
                            stream->unk8 = 4;
                            break;
                        }
                    }
                }
                work->unk84 = (work->unk84 + size) % work->unk7C;
                *pos += size;
            }
        }
    }
    if (sNFLWork->unk68 == 7) {
        if (obj->unk17C != NULL) {
            NFLi_CopyFaceTextures(obj->unk17C, pos);
        }
    } else {
        NFLi_CopyFacePltts(obj, pos);
    }
    return TRUE;
}

void NFLi_MakeColorTexturePltt(u16 *palette, const s16 *params);

// The palettes of an object's eleven palette streams (unk228), made from its parameters at unk180
BOOL NFLi_CopyFacePltts(NFLObj *obj, u32 *pos) {
    NFLWork *work = (NFLWork *)sNFLWork;
    s16 *params = obj->unk180;

    if (work->unk68 != 7) {
        u16 palette[16];
        NFLWorkUnkA0 *stream = obj->unk228;
        u16 *pal = palette;
        int i;

        for (i = 0; i < 11; stream++, i++) {
            int size;
            BOOL overflowB;
            BOOL overflowA;
            u8 *p;

            if (*pos >= work->unk7C) {
                return TRUE;
            }
            if (!work->unk9C) {
                if (stream->unk8 >= 2 && stream->unk8 <= 5) {
                    work->unk9C = TRUE;
                } else if (stream->unk8 <= 1) {
                    work->unk88 = 0;
                    work->unk80 = 0;
                    work->unk84 = 0;
                    *pos = 0;
                    work->unk9C = TRUE;
                }
            }
            if (stream->unk8 == 0 || stream->unk8 == 1 || stream->unk8 == 3 || stream->unk8 == 4) {
                overflowB = FALSE;
                overflowA = FALSE;
                if (i == 0) {
                    size = 16;
                } else {
                    size = 32;
                }
                switch (i) {
                case 0:
                    NFLi_MakeColorTexturePltt(pal, params);
                    break;
                case 1:
                    NFLi_MakeEyePltt(pal, params[8]);
                    break;
                case 2:
                    NFLi_MakeHairPltt(pal, params[6]);
                    break;
                case 3:
                    NFLi_MakeMouthPltt(pal, params[10]);
                    break;
                case 4:
                    NFLi_MakeHairPltt(pal, params[14]);
                    break;
                case 5:
                    NFLi_MakeGlassesPltt(pal, params[12]);
                    break;
                case 6:
                    NFLi_MakeHairPltt(pal, params[14]);
                    break;
                case 7:
                    NFLi_MakeFacePltt(pal, params[2]);
                    break;
                case 8:
                    if (params[3] == 34 || params[3] == 57) {
                        NFLi_MakeFavoriteColorPltt(pal, params[0]);
                    } else {
                        NFLi_MakeHairPltt(pal, params[4]);
                    }
                    break;
                case 9:
                    MIi_CpuCopy32(NFLi_GetResourceItem(12, 0), pal, size);
                    DC_FlushRange(pal, size);
                    break;
                }
                if (stream->unk8 == 0) {
                    if (*pos + size > work->unk7C) {
                        overflowA = TRUE;
                        size = work->unk7C - *pos;
                    }
                    if (work->unk84 + size > work->unk7C) {
                        overflowB = TRUE;
                    }
                    if (!overflowB) {
                        MIi_CpuCopy32(pal, work->unk78 + work->unk84, size);
                        DC_FlushRange(work->unk78 + work->unk84, size);
                    } else {
                        int over = work->unk84 + size - work->unk7C;
                        int n = size - over;

                        MIi_CpuCopy32(pal, work->unk78 + work->unk84, n);
                        DC_FlushRange(work->unk78 + work->unk84, n);
                        MIi_CpuCopy32((u8 *)pal + n, work->unk78, over);
                        DC_FlushRange(work->unk78, over);
                    }
                    if (!overflowA) {
                        stream->unk8 = 2;
                    } else {
                        obj->unk178 = size;
                        stream->unk8 = 3;
                    }
                    stream->unk0 = work->unk84;
                } else {
                    p = (u8 *)pal;
                    switch (stream->unk8) {
                    case 3:
                    case 4:
                        p += obj->unk178;
                        size -= obj->unk178;
                        break;
                    case 1:
                        p = (u8 *)pal + obj->unk174;
                        size -= obj->unk174;
                        stream->unk0 = work->unk84;
                        break;
                    }
                    if (*pos + size > work->unk7C) {
                        overflowA = TRUE;
                        size = work->unk7C - *pos;
                    }
                    if (work->unk84 + size > work->unk7C) {
                        overflowB = TRUE;
                    }
                    if (!overflowA) {
                        stream->unk8 = stream->unk8 == 3 ? 2 : 5;
                        obj->unk178 = 0;
                    } else {
                        switch (stream->unk8) {
                        case 3:
                        case 4:
                            obj->unk178 += size;
                            break;
                        case 1:
                            obj->unk178 = obj->unk174 + size;
                            stream->unk8 = 5;
                            break;
                        }
                    }
                    if (!overflowB) {
                        MIi_CpuCopy32(p, work->unk78 + work->unk84, size);
                        DC_FlushRange(work->unk78 + work->unk84, size);
                    } else {
                        int over = work->unk84 + size - work->unk7C;
                        int n = size - over;

                        MIi_CpuCopy32(p, work->unk78 + work->unk84, n);
                        DC_FlushRange(work->unk78 + work->unk84, n);
                        MIi_CpuCopy32(p + n, work->unk78, over);
                        DC_FlushRange(work->unk78, over);
                    }
                }
                work->unk84 = (work->unk84 + size) % work->unk7C;
                *pos += size;
            }
        }
    }
    if (obj->unk17C != NULL) {
        NFLi_CopyFaceTextures(obj->unk17C, pos);
    }
    return TRUE;
}

// Takes obj out of the list at unk74 of the work area
void NFLi_RemoveFromLoadList(NFLObj *obj) {
    NFLWork *work = sNFLWork;
    NFLObj *prev = work->unk74;
    NFLObj *cur;

    if (prev == obj) {
        work->unk74 = obj->unk17C;
        obj->unk17C = NULL;
        return;
    }
    for (cur = prev->unk17C; cur != NULL; cur = cur->unk17C) {
        if (cur == obj) {
            prev->unk17C = obj->unk17C;
            obj->unk17C = NULL;
            return;
        }
        prev = cur;
    }
}

int NFLi_LoadSharedTextures(u32 *size);
int NFLi_LoadSharedPltt(u32 *size);
int NFLi_LoadFaceTextures(NFLObj *obj, u32 *size);
int NFLi_LoadFacePltts(NFLObj *obj, u32 *size);

// The library's work after the frame's interrupt, which main calls with 0x4000: 8 a size not a multiple of 4, 4 not
// set up, 0x13 and 0x14 busy, 2 and 3 nothing to do
int NFL_LoadVram(u32 size) {
    u32 rest = size;
    NFLWork *work;
    NFLObj *obj;
    int result;

    if (rest & 3) {
        return 8;
    }
    work = sNFLWork;
    if (work == NULL) {
        return 4;
    }
    if (work->unk8C == 1) {
        return 0x13;
    }
    if (work->unk8C == 3) {
        return 0x14;
    }
    if (work->unk8C == 0) {
        return 2;
    }
    if (work->unk8C == 4) {
        return 3;
    }
    work->unk8C = 3;
    switch (work->unk90) {
    case 0:
        result = NFLi_LoadSharedTextures(&rest);
        if (result != 0 && result != 2) {
            work->unk90 = 3;
            work->unk8C = 4;
            return result;
        }
        if (result == 2) {
            work->unk8C = 0;
        } else {
            work->unk8C = 2;
        }
        break;
    case 1:
        result = NFLi_LoadSharedPltt(&rest);
        if (result != 0 && result != 2) {
            work->unk90 = 3;
            work->unk8C = 4;
            return result;
        }
        if (result == 2) {
            work->unk8C = 0;
        } else {
            work->unk8C = 2;
        }
        break;
    case 2:
        obj = work->unk74;
        if (obj == NULL) {
            work->unk8C = 4;
            return 2;
        }
        switch (obj->unk170) {
        case 0:
            result = NFLi_LoadFaceTextures(obj, &rest);
            if (result != 0 && result != 2) {
                obj->unk170 = 3;
                NFLi_RemoveFromLoadList(work->unk74);
                work->unk8C = 0;
                return result;
            }
            if (result == 2) {
                work->unk8C = 0;
            } else {
                work->unk8C = 2;
            }
            break;
        case 1:
            result = NFLi_LoadFacePltts(obj, &rest);
            if (result != 0 && result != 2) {
                obj->unk170 = 3;
                NFLi_RemoveFromLoadList(work->unk74);
                work->unk8C = 0;
                return result;
            }
            if (result == 2) {
                work->unk8C = 0;
            } else {
                work->unk8C = 2;
            }
            break;
        case 2:
        case 4:
            NFLi_RemoveFromLoadList(obj);
            work->unk8C = 0;
            break;
        }
        break;
    }
    if (result != 0) {
        return result;
    }
    return 1;
}

// The VRAM side of NFLi_CopySharedTextures's streams, after the frame's interrupt: loads up to *rest bytes of what the six
// streams have put in the ring buffer into texture VRAM (NFLi_LoadTex), then hands on to the object streams
// (NFLi_LoadFaceTextures) or the seventh stream (NFLi_LoadSharedPltt)
int NFLi_LoadSharedTextures(u32 *rest) {
    NFLWork *work = (NFLWork *)sNFLWork;
    u32 dest;
    NFLWorkUnkA0 *stream = work->unkA0;
    NFLStreamSource *source = sNFLSharedStreamSources;
    int i;

    for (i = 0; i < 6; stream++, source++, i++) {
        BOOL go = FALSE;
        u8 *src;
        u32 size;
        int result;

        switch (stream->unk8) {
        case 2:
            if (sNFLWork->unk68 == 7) {
                size = source->unk8 * 4;
            } else {
                size = source->unk8;
            }
            src = (u8 *)stream->unk0 + (u32)work->unk78;
            dest = stream->unk4;
            DC_FlushRange(src, size);
            go = TRUE;
            break;
        case 3:
            size = work->unk98;
            src = (u8 *)stream->unk0 + (u32)work->unk78;
            dest = stream->unk4;
            DC_FlushRange(src, size);
            go = TRUE;
            break;
        case 4:
            size = work->unk98 - work->unk94;
            src = (u8 *)stream->unk0 + (u32)work->unk78;
            dest = stream->unk4;
            DC_FlushRange(src, size);
            go = TRUE;
            break;
        case 5:
            if (sNFLWork->unk68 == 7) {
                size = source->unk8 * 4;
            } else {
                size = source->unk8;
            }
            size -= work->unk94;
            src = (u8 *)stream->unk0 + (u32)work->unk78;
            dest = stream->unk4;
            DC_FlushRange(src, size);
            go = TRUE;
            break;
        case 0:
        case 1:
            work->unk88 = 0;
            work->unk80 = 0;
            work->unk84 = 0;
            return 2;
        }
        if (*rest == 0) {
            return 0;
        }
        if (go == TRUE) {
            if (*rest >= size) {
                if (stream->unk0 + size <= work->unk7C) {
                    result = NFLi_LoadTex(src, dest, size);
                } else {
                    int over = stream->unk0 + size - work->unk7C;
                    int n = size - over;

                    result = NFLi_LoadTex(src, dest, n);
                    if (result == 0) {
                        result = NFLi_LoadTex(work->unk78, dest + n, over);
                    }
                }
                if (result != 0) {
                    stream->unk8 = 6;
                    return result;
                }
                work->unk80 = (stream->unk0 + size) % work->unk7C;
                if (work->unk84 >= work->unk80) {
                    work->unk88 = work->unk84 - work->unk80;
                } else {
                    work->unk88 = work->unk7C + work->unk84 - work->unk80;
                }
                if (stream->unk8 == 2) {
                    stream->unk8 = 7;
                } else if (stream->unk8 == 5) {
                    stream->unk8 = 7;
                    work->unk94 = 0;
                } else {
                    stream->unk8 = 1;
                    work->unk94 = work->unk98;
                    work->unk98 = 0;
                }
                *rest -= size;
            } else {
                if (stream->unk0 + *rest <= work->unk7C) {
                    result = NFLi_LoadTex(src, dest, *rest);
                } else {
                    int over = stream->unk0 + *rest - work->unk7C;
                    int n = *rest - over;

                    result = NFLi_LoadTex(src, dest, n);
                    if (result == 0) {
                        result = NFLi_LoadTex(work->unk78, dest + n, over);
                    }
                }
                if (result != 0) {
                    stream->unk8 = 6;
                    return result;
                }
                work->unk80 = (stream->unk0 + *rest) % work->unk7C;
                if (work->unk84 >= work->unk80) {
                    work->unk88 = work->unk84 - work->unk80;
                } else {
                    work->unk88 = work->unk7C + work->unk84 - work->unk80;
                }
                if (stream->unk8 == 3 || stream->unk8 == 4) {
                    stream->unk8 = 4;
                } else {
                    stream->unk8 = 5;
                }
                work->unk94 += *rest;
                stream->unk0 = (stream->unk0 + *rest) % work->unk7C;
                stream->unk4 += *rest;
                *rest = 0;
                return 0;
            }
        }
    }
    if (sNFLWork->unk68 == 7) {
        work->unk90 = 2;
        if (*rest == 0) {
            return 2;
        }
        if (work->unk74 != NULL) {
            switch (work->unk74->unk170) {
            case 0:
                return NFLi_LoadFaceTextures(work->unk74, rest);
            case 2:
            case 4:
                NFLi_RemoveFromLoadList(work->unk74);
                break;
            }
        }
    } else {
        work->unk90 = 1;
        if (*rest == 0) {
            return 2;
        }
        return NFLi_LoadSharedPltt(rest);
    }
    return 2;
}

// The VRAM side of the seventh stream (NFLi_CopySharedPltt): loads it into texture palette VRAM (NFLi_LoadTexPltt), then
// hands on to the object streams
int NFLi_LoadSharedPltt(u32 *rest) {
    NFLWork *work = (NFLWork *)sNFLWork;
    BOOL go = FALSE;
    u32 dest;
    int size;
    u8 *src;
    int result;

    if (work->unk68 == 7) {
        return 0;
    }
    switch (work->unkA0[6].unk8) {
    case 2:
        dest = work->unkA0[6].unk4;
        src = (u8 *)work->unkA0[6].unk0 + (u32)work->unk78;
        size = 32;
        DC_FlushRange(src, size);
        go = TRUE;
        break;
    case 3:
        dest = work->unkA0[6].unk4;
        src = (u8 *)work->unkA0[6].unk0 + (u32)work->unk78;
        size = work->unk98;
        DC_FlushRange(src, size);
        go = TRUE;
        break;
    case 4:
        size = work->unk98 - work->unk94;
        src = (u8 *)work->unkA0[6].unk0 + (u32)work->unk78;
        dest = work->unkA0[6].unk4;
        DC_FlushRange(src, size);
        go = TRUE;
        break;
    case 5:
        size = 32 - work->unk94;
        src = (u8 *)work->unkA0[6].unk0 + (u32)work->unk78;
        dest = work->unkA0[6].unk4;
        DC_FlushRange(src, size);
        go = TRUE;
        break;
    case 0:
    case 1:
        work->unk88 = go;
        work->unk80 = go;
        work->unk84 = go;
        return 2;
    }
    if (*rest == 0) {
        return 0;
    }
    if (go == TRUE) {
        if (*rest >= size) {
            if (work->unkA0[6].unk0 + size <= work->unk7C) {
                result = NFLi_LoadTexPltt(src, dest, size);
            } else {
                u32 over = work->unkA0[6].unk0 + size - work->unk7C;
                u32 n = size - over;

                result = NFLi_LoadTexPltt(src, dest, n);
                if (result == 0) {
                    result = NFLi_LoadTexPltt(work->unk78, dest + n, over);
                }
            }
            if (result != 0) {
                work->unkA0[6].unk8 = 6;
                return result;
            }
            work->unk80 = (work->unkA0[6].unk0 + size) % work->unk7C;
            if (work->unk84 >= work->unk80) {
                work->unk88 = work->unk84 - work->unk80;
            } else {
                work->unk88 = work->unk7C + work->unk84 - work->unk80;
            }
            if (work->unkA0[6].unk8 == 2) {
                work->unkA0[6].unk8 = 7;
            } else if (work->unkA0[6].unk8 == 5) {
                work->unkA0[6].unk8 = 7;
                work->unk94 = 0;
            } else {
                work->unkA0[6].unk8 = 1;
                work->unk94 = work->unk98;
                work->unk98 = 0;
            }
            *rest -= size;
        } else {
            if (work->unkA0[6].unk0 + *rest <= (u32)(work->unk78 + work->unk7C)) {
                result = NFLi_LoadTexPltt(work->unk78 + work->unkA0[6].unk0, work->unkA0[6].unk4, *rest);
            } else {
                u32 over = work->unkA0[6].unk0 + *rest - work->unk7C;
                u32 n = *rest - over;

                result = NFLi_LoadTexPltt(work->unk78 + work->unkA0[6].unk0, work->unkA0[6].unk4, n);
                if (result == 0) {
                    result = NFLi_LoadTexPltt(work->unk78, work->unkA0[6].unk4 + n, over);
                }
            }
            if (result != 0) {
                work->unkA0[6].unk8 = 6;
                return result;
            }
            work->unk80 = (work->unkA0[6].unk0 + *rest) % work->unk7C;
            if (work->unk84 >= work->unk80) {
                work->unk88 = work->unk84 - work->unk80;
            } else {
                work->unk88 = work->unk7C + work->unk84 - work->unk80;
            }
            if (work->unkA0[6].unk8 == 3 || work->unkA0[6].unk8 == 4) {
                work->unkA0[6].unk8 = 4;
            } else {
                work->unkA0[6].unk8 = 5;
            }
            work->unk94 += *rest;
            work->unkA0[6].unk0 = (work->unkA0[6].unk0 + *rest) % work->unk7C;
            work->unkA0[6].unk4 += *rest;
            *rest = 0;
            return 0;
        }
    }
    work->unk90 = 2;
    if (*rest == 0) {
        return 2;
    }
    if (work->unk74 != NULL) {
        switch (work->unk74->unk170) {
        case 0:
            return NFLi_LoadFaceTextures(work->unk74, rest);
        case 1:
            return NFLi_LoadFacePltts(work->unk74, rest);
        case 2:
        case 4:
            NFLi_RemoveFromLoadList(work->unk74);
            break;
        }
    }
    return 2;
}

// Where each of an object's eleven streams goes in texture VRAM, from the object's base (unkD4): the first offset in
// the other modes, the second in mode 7
typedef struct NFLObjDest {
    u32 unk0;
    u32 unk4;
} NFLObjDest;

extern const NFLObjDest sNFLFaceTexOffsets[];

// The VRAM side of an object's eleven streams (NFLi_CopyFaceTextures)
int NFLi_LoadFaceTextures(NFLObj *obj, u32 *rest) {
    NFLWork *work = sNFLWork;
    int i;
    u32 dest;
    NFLWorkUnkA0 *stream = obj->unk1A4;
    int result;

    for (i = 0; i < 11; stream++, i++) {
        BOOL go = FALSE;
        u8 *src;
        u32 size;

        switch (stream->unk8) {
        case 2:
            if (sNFLWork->unk68 == 7) {
                if (i == 0) {
                    size = 0x80;
                } else {
                    size = sNFLFaceStreamSources[i].unk4 * 4;
                }
                dest = obj->unkD4 + sNFLFaceTexOffsets[i].unk4;
            } else {
                size = sNFLFaceStreamSources[i].unk4;
                dest = obj->unkD4 + sNFLFaceTexOffsets[i].unk0;
            }
            src = (u8 *)stream->unk0 + (u32)work->unk78;
            go = TRUE;
            break;
        case 3:
            size = obj->unk178;
            src = (u8 *)stream->unk0 + (u32)work->unk78;
            if (sNFLWork->unk68 == 7) {
                dest = obj->unkD4 + sNFLFaceTexOffsets[i].unk4;
            } else {
                dest = obj->unkD4 + sNFLFaceTexOffsets[i].unk0;
            }
            go = TRUE;
            break;
        case 4:
            size = obj->unk178 - obj->unk174;
            src = (u8 *)stream->unk0 + (u32)work->unk78;
            if (sNFLWork->unk68 == 7) {
                dest = obj->unk174 + (obj->unkD4 + sNFLFaceTexOffsets[i].unk4);
            } else {
                dest = obj->unk174 + (obj->unkD4 + sNFLFaceTexOffsets[i].unk0);
            }
            go = TRUE;
            break;
        case 5:
            if (sNFLWork->unk68 == 7) {
                if (i == 0) {
                    size = 0x80;
                } else {
                    size = sNFLFaceStreamSources[i].unk4 * 4;
                }
                dest = obj->unk174 + (obj->unkD4 + sNFLFaceTexOffsets[i].unk4);
            } else {
                size = sNFLFaceStreamSources[i].unk4;
                dest = obj->unk174 + (obj->unkD4 + sNFLFaceTexOffsets[i].unk0);
            }
            size -= obj->unk174;
            src = (u8 *)stream->unk0 + (u32)work->unk78;
            go = TRUE;
            break;
        case 0:
        case 1:
            work->unk88 = go;
            work->unk80 = go;
            work->unk84 = go;
            return 2;
        }
        if (*rest == 0) {
            return 0;
        }
        if (go == TRUE) {
            if (*rest >= size) {
                if (stream->unk0 + size <= work->unk7C) {
                    result = NFLi_LoadTex(src, dest, size);
                } else {
                    int over = stream->unk0 + size - work->unk7C;
                    int n = size - over;

                    result = NFLi_LoadTex(src, dest, n);
                    if (result == 0) {
                        result = NFLi_LoadTex(work->unk78, dest + n, over);
                    }
                }
                if (result != 0) {
                    stream->unk8 = 6;
                    return result;
                }
                work->unk80 = (stream->unk0 + size) % work->unk7C;
                if (work->unk84 >= work->unk80) {
                    work->unk88 = work->unk84 - work->unk80;
                } else {
                    work->unk88 = work->unk7C + work->unk84 - work->unk80;
                }
                if (stream->unk8 == 2) {
                    stream->unk8 = 7;
                } else if (stream->unk8 == 5) {
                    stream->unk8 = 7;
                    obj->unk174 = 0;
                } else {
                    stream->unk8 = 1;
                    obj->unk174 = obj->unk178;
                    obj->unk178 = 0;
                    *rest -= size;
                    return 2;
                }
                *rest -= size;
            } else {
                if (stream->unk0 + *rest <= work->unk7C) {
                    result = NFLi_LoadTex(src, dest, *rest);
                } else {
                    int over = stream->unk0 + *rest - work->unk7C;
                    int n = *rest - over;

                    result = NFLi_LoadTex(src, dest, n);
                    if (result == 0) {
                        result = NFLi_LoadTex(work->unk78, dest + n, over);
                    }
                }
                if (result != 0) {
                    stream->unk8 = 6;
                    return result;
                }
                work->unk80 = (stream->unk0 + *rest) % work->unk7C;
                if (work->unk84 >= work->unk80) {
                    work->unk88 = work->unk84 - work->unk80;
                } else {
                    work->unk88 = work->unk7C + work->unk84 - work->unk80;
                }
                if (stream->unk8 == 3 || stream->unk8 == 4) {
                    stream->unk8 = 4;
                } else {
                    stream->unk8 = 5;
                }
                obj->unk174 += *rest;
                stream->unk0 = (stream->unk0 + *rest) % work->unk7C;
                *rest = 0;
                return 0;
            }
        }
    }
    result = 2;
    if (sNFLWork->unk68 == 7) {
        if (obj->unk17C != NULL && *rest != 0) {
            result = NFLi_LoadFaceTextures(obj->unk17C, rest);
            if (result != 0 && result != 2) {
                return result;
            }
        }
        if (obj->unk170 != 4) {
            obj->unk170 = 2;
        }
        NFLi_RemoveFromLoadList(obj);
    } else {
        obj->unk170 = 1;
        if (*rest == 0) {
            return result;
        }
        result = NFLi_LoadFacePltts(obj, rest);
        return result;
    }
    return result;
}

// Where each of an object's palette streams goes in texture palette VRAM, from the object's base (unkD8)
extern const u32 sNFLFacePlttOffsets[];

// The VRAM side of an object's palette streams (NFLi_CopyFacePltts): ten of them here, where NFLi_CopyFacePltts fills eleven
int NFLi_LoadFacePltts(NFLObj *obj, u32 *rest) {
    NFLWork *work = (NFLWork *)sNFLWork;
    int result;

    if (work->unk68 != 7) {
        int i;
        u8 *src;
        NFLWorkUnkA0 *stream = obj->unk228;

        for (i = 0; i < 10; stream++, i++) {
            BOOL go = FALSE;
            u32 dest;
            u32 size;

            switch (stream->unk8) {
            case 2:
                dest = obj->unkD8 + sNFLFacePlttOffsets[i];
                if (i == 0) {
                    size = 16;
                } else {
                    size = 32;
                }
                src = (u8 *)stream->unk0 + (u32)work->unk78;
                DC_FlushRange(src, size);
                go = TRUE;
                break;
            case 3:
                src = (u8 *)stream->unk0 + (u32)work->unk78;
                size = obj->unk178;
                dest = obj->unkD8 + sNFLFacePlttOffsets[i];
                DC_FlushRange(src, size);
                go = TRUE;
                break;
            case 4:
                size = obj->unk178 - obj->unk174;
                src = (u8 *)stream->unk0 + (u32)work->unk78;
                dest = obj->unk174 + (obj->unkD8 + sNFLFacePlttOffsets[i]);
                DC_FlushRange(src, size);
                go = TRUE;
                break;
            case 5:
                if (i == 0) {
                    size = 16 - obj->unk174;
                } else {
                    size = 32 - obj->unk174;
                }
                src = (u8 *)stream->unk0 + (u32)work->unk78;
                dest = obj->unk174 + (obj->unkD8 + sNFLFacePlttOffsets[i]);
                DC_FlushRange(src, size);
                go = TRUE;
                break;
            case 0:
            case 1:
                work->unk88 = go;
                work->unk80 = go;
                work->unk84 = go;
                return 2;
            }
            if (*rest == 0) {
                return 0;
            }
            if (go == TRUE) {
                if (*rest >= size) {
                    if (stream->unk0 + size <= work->unk7C) {
                        result = NFLi_LoadTexPltt(src, dest, size);
                    } else {
                        int over = stream->unk0 + size - work->unk7C;
                        int n = size - over;

                        result = NFLi_LoadTexPltt(src, dest, n);
                        if (result == 0) {
                            result = NFLi_LoadTexPltt(work->unk78, dest + n, over);
                        }
                    }
                    if (result != 0) {
                        stream->unk8 = 6;
                        return result;
                    }
                    work->unk80 = (stream->unk0 + size) % work->unk7C;
                    if (work->unk84 >= work->unk80) {
                        work->unk88 = work->unk84 - work->unk80;
                    } else {
                        work->unk88 = work->unk7C + work->unk84 - work->unk80;
                    }
                    if (stream->unk8 == 2) {
                        stream->unk8 = 7;
                    } else if (stream->unk8 == 5) {
                        stream->unk8 = 7;
                        obj->unk174 = 0;
                    } else {
                        stream->unk8 = 1;
                        obj->unk174 = obj->unk178;
                        obj->unk178 = 0;
                    }
                    *rest -= size;
                } else {
                    if (stream->unk0 + *rest <= work->unk7C) {
                        result = NFLi_LoadTexPltt(src, dest, *rest);
                    } else {
                        int over = stream->unk0 + *rest - work->unk7C;
                        int n = *rest - over;

                        result = NFLi_LoadTexPltt(src, dest, n);
                        if (result == 0) {
                            result = NFLi_LoadTexPltt(work->unk78, dest + n, over);
                        }
                    }
                    if (result != 0) {
                        stream->unk8 = 6;
                        return result;
                    }
                    work->unk80 = (stream->unk0 + *rest) % work->unk7C;
                    if (work->unk84 >= work->unk80) {
                        work->unk88 = work->unk84 - work->unk80;
                    } else {
                        work->unk88 = work->unk7C + work->unk84 - work->unk80;
                    }
                    if (stream->unk8 == 3 || stream->unk8 == 4) {
                        stream->unk8 = 4;
                    } else {
                        stream->unk8 = 5;
                    }
                    obj->unk174 += *rest;
                    stream->unk0 = (stream->unk0 + *rest) % work->unk7C;
                    *rest = 0;
                    return 0;
                }
            }
        }
    }
    result = 2;
    if (obj->unk17C != NULL && *rest != 0) {
        result = NFLi_LoadFaceTextures(obj->unk17C, rest);
        if (result != 0 && result != 2) {
            return result;
        }
    }
    if (obj->unk170 != 4) {
        obj->unk170 = 2;
    }
    NFLi_RemoveFromLoadList(obj);
    return result;
}

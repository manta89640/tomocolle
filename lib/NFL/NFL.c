// NFL, the Mii face library: the main program's code from the end of NitroSystem (0x02069BF4) to the Mii transfer
// (0x020776E4), with its data. This one file holds the library's whole range, which is not yet divided into the
// files it was built from (docs/binary.md, "The Mii library"). The names are the project's own (see NFL.h).

#include "NFL.h"

NFLWork *sNFLWork;

// The reader of the library's resource streams: one byte, advancing the stream
u8 NFLi_ReadU8(const u8 **stream) {
    const u8 *p = *stream;
    u8 value = *p++;
    *stream = p;
    return value;
}

// Two or four bytes, little-endian, unsigned or signed
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

// A primitive byte of a mesh as the GX_BEGIN to give G3C_Begin: 0x90 triangles, 0x80 quads, 0x98
// triangle strips; the others stop the program
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
    NFL_RGB(NFLi_BlendComponent(r, r2, rest, weight), NFLi_BlendComponent(g, g2, rest, weight),                     \
            NFLi_BlendComponent(b, b2, rest, weight))

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
    NFLFace *obj = ctx->face;
    const NFLCharData *mii = ctx->mii;
    s16 *params = obj->keptFields;
    int type = mii->glassesType;
    int color = mii->glassesColor;

    if (params[13] != type || (obj->texStreams[6].state >= 1 && obj->texStreams[6].state <= 6)) {
        obj->texStreams[6].state = 0;
        obj->texStreams[6].value = type;
    }
    if (color != params[12] || (obj->plttStreams[5].state >= 1 && obj->plttStreams[5].state <= 6)) {
        if (obj->texFmt != 7) {
            obj->plttStreams[5].value = color;
            obj->plttStreams[5].state = 0;
        } else {
            obj->texStreams[6].state = 0;
            obj->texStreams[6].value = type;
        }
    }
    if (obj->texFmt == 7) {
        ctx->partAddr[4].texAddr = obj->texBase + 0x2880;
    } else {
        ctx->partAddr[4].texAddr = obj->texBase + 0xA10;
        ctx->partAddr[4].plttAddr = obj->plttBase + 0x90;
    }
    return 0;
}

// The beard
int NFLi_PrepareBeard(NFLMakeCtx *ctx) {
    NFLFace *obj = ctx->face;
    const NFLCharData *mii = ctx->mii;
    s16 *params = obj->keptFields;
    int type = mii->beardType;
    int color = mii->beardColor;

    if (params[15] != type || (obj->texStreams[7].state >= 1 && obj->texStreams[7].state <= 6)) {
        if (type < 2) {
            ctx->partAddr[3].texAddr = obj->colorTexAddr;
            obj->texStreams[7].state = 7;
        } else {
            obj->texStreams[7].state = 0;
            obj->texStreams[7].value = type;
        }
    }
    if (color != params[14] || (obj->plttStreams[6].state >= 1 && obj->plttStreams[6].state <= 6)) {
        if (obj->texFmt != 7) {
            obj->plttStreams[6].value = color;
            obj->plttStreams[6].state = 0;
        } else if (type >= 2) {
            obj->texStreams[7].state = 0;
            obj->texStreams[7].value = type;
        }
    }
    if (type < 2) {
        ctx->partAddr[3].texAddr = obj->colorTexAddr;
        ctx->partAddr[3].plttAddr = obj->colorPlttAddr;
    } else if (obj->texFmt == 7) {
        ctx->partAddr[3].texAddr = obj->texBase + 0x3880;
    } else {
        ctx->partAddr[3].texAddr = obj->texBase + 0xE10;
        ctx->partAddr[3].plttAddr = obj->plttBase + 0xB0;
    }
    return 0;
}

// The hair
int NFLi_PrepareHair(NFLMakeCtx *ctx) {
    NFLFace *obj = ctx->face;
    const NFLCharData *mii = ctx->mii;
    s16 *params = obj->keptFields;
    int type = mii->hairType;
    int color = mii->hairColor;

    if ((params[3] != type || color != params[4]) || (obj->texStreams[9].state >= 1 && obj->texStreams[9].state <= 6) ||
        (obj->plttStreams[8].state >= 1 && obj->plttStreams[8].state <= 6)) {
        if (type == 5 || type == 7 || type == 0x18 || type == 0x1D || type == 0x2A || type == 0x35 || type == 0x3A ||
            type == 0x45 || type == 0x47) {
            obj->texStreams[9].state = 0;
            if (obj->texFmt != 7) {
                obj->plttStreams[8].state = 0;
                obj->plttStreams[8].value = color;
            }
        } else if (type != 0x22 && type != 0x39) {
            obj->texStreams[9].state = 7;
            obj->plttStreams[8].state = 7;
        }
        if (type != 0x22 && type != 0x39) {
            obj->texStreams[9].value = type;
            if (obj->texFmt != 7) {
                obj->plttStreams[8].value = color;
            }
        }
    }
    if (type == 5 || type == 7 || type == 0x18 || type == 0x1D || type == 0x2A || type == 0x35 || type == 0x3A ||
        type == 0x45 || type == 0x47) {
        if (obj->texFmt == 7) {
            ctx->partAddr[2].texAddr = obj->texBase + 0x5080;
            ctx->partAddr[2].plttAddr = obj->colorPlttAddr;
        } else {
            ctx->partAddr[2].texAddr = obj->texBase + 0x1410;
            ctx->partAddr[2].plttAddr = obj->plttBase + 0xF0;
        }
    } else {
        ctx->partAddr[2].texAddr = obj->colorTexAddr;
        ctx->partAddr[2].plttAddr = obj->colorPlttAddr;
    }
    return 0;
}

// The hair again, for the two types (0x22 and 0x39) that the other leaves alone
int NFLi_PrepareFavoriteColorHair(NFLMakeCtx *ctx) {
    NFLFace *obj = ctx->face;
    const NFLCharData *mii = ctx->mii;
    s16 *params = obj->keptFields;
    int type = mii->hairType;
    int color = mii->favoriteColor;

    if (type == 0x22 || type == 0x39) {
        if ((type != params[3] || color != params[0]) ||
            (obj->texStreams[9].state >= 1 && obj->texStreams[9].state <= 6) ||
            (obj->plttStreams[8].state >= 1 && obj->plttStreams[8].state <= 6)) {
            obj->texStreams[9].state = 0;
            obj->texStreams[9].value = type;
            if (obj->texFmt == 7) {
                obj->plttStreams[8].state = 7;
            } else {
                obj->plttStreams[8].state = 0;
                obj->plttStreams[8].value = type;
            }
        }
    }
    if (type == 0x22 || type == 0x39) {
        if (obj->texFmt == 7) {
            ctx->partAddr[5].texAddr = obj->texBase + 0x5080;
        } else {
            ctx->partAddr[5].texAddr = obj->texBase + 0x1410;
            ctx->partAddr[5].plttAddr = obj->plttBase + 0xF0;
        }
    } else if (obj->texFmt == 7) {
        ctx->partAddr[5].texAddr = obj->colorTexAddr;
    } else {
        ctx->partAddr[5].texAddr = obj->colorTexAddr;
        ctx->partAddr[5].plttAddr = obj->colorPlttAddr;
    }
    return 0;
}

// The face's texture
int NFLi_PrepareFaceTexture(NFLMakeCtx *ctx) {
    NFLFace *obj = ctx->face;
    const NFLCharData *mii = ctx->mii;
    s16 *params = obj->keptFields;
    int texture = mii->faceTexture;
    int color = mii->faceColor;

    if (params[1] != texture || (obj->texStreams[8].state >= 1 && obj->texStreams[8].state <= 6)) {
        obj->texStreams[8].state = texture == 0 ? 7 : 0;
        obj->texStreams[8].value = texture;
    }
    if (color != params[2] || (obj->plttStreams[7].state >= 1 && obj->plttStreams[7].state <= 6)) {
        if (obj->texFmt != 7) {
            obj->plttStreams[7].value = color;
            obj->plttStreams[7].state = 0;
        } else if (texture != 0) {
            obj->texStreams[8].state = 0;
            obj->texStreams[8].value = texture;
        }
    }
    if (texture == 0) {
        ctx->partAddr[1].texAddr = obj->colorTexAddr;
        ctx->partAddr[1].plttAddr = obj->colorPlttAddr;
    } else if (obj->texFmt == 7) {
        ctx->partAddr[1].texAddr = obj->texBase + 0x4080;
        ctx->partAddr[1].plttAddr = obj->colorPlttAddr;
    } else {
        ctx->partAddr[1].texAddr = obj->texBase + 0x1010;
        ctx->partAddr[1].plttAddr = obj->plttBase + 0xD0;
    }
    return 0;
}

// The nose
int NFLi_PrepareNose(NFLMakeCtx *ctx) {
    NFLFace *obj = ctx->face;
    const NFLCharData *mii = ctx->mii;
    s16 *params = obj->keptFields;
    int type = mii->noseType;

    if (params[9] != type || (obj->texStreams[10].state >= 1 && obj->texStreams[10].state <= 6) ||
        (obj->plttStreams[9].state >= 1 && obj->plttStreams[9].state <= 6)) {
        obj->texStreams[10].state = 0;
        obj->texStreams[10].value = type;
        if (obj->texFmt == 7) {
            obj->plttStreams[9].state = 7;
        } else {
            obj->plttStreams[9].state = 0;
            obj->plttStreams[9].value = type;
        }
    }
    if (obj->texFmt == 7) {
        ctx->partAddr[6].texAddr = obj->texBase + 0x5880;
        ctx->partAddr[6].plttAddr = obj->plttBase;
    } else {
        ctx->partAddr[6].texAddr = obj->texBase + 0x1610;
        ctx->partAddr[6].plttAddr = obj->plttBase + 0x110;
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
    NFLFace *obj = ctx->face;
    u32 type = ctx->mii->beardType;
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
    if (obj->texFmt == 7) {
        format = GX_TEXFMT_DIRECT;
    } else {
        format = type == 1 ? GX_TEXFMT_PLTT4 : GX_TEXFMT_PLTT16;
    }
    G3C_TexImageParam(&ctx->dl, format, GX_TEXGEN_NONE, s, t, GX_TEXREPEAT_S, GX_TEXFLIP_S, GX_TEXPLTTCOLOR0_TRNS,
                      ctx->partAddr[3].texAddr);
    if (obj->texFmt != 7) {
        G3C_TexPlttBase(&ctx->dl, ctx->partAddr[3].plttAddr, format);
    }
    G3C_PushMtx(&ctx->dl);
    position.x = ctx->anchors[1].x;
    position.y = ctx->anchors[1].y;
    position.z = ctx->anchors[1].z;
    NFLi_MapVecFx32(obj, &translation, &position);
    G3C_Translate(&ctx->dl, translation.x, translation.y, translation.z);
    for (i = 0; i < polygons; i++) {
        count = NFLi_ReadU8(&stream);
        func_02005124(&ctx->dl, NFLi_ReadPrimitive(&stream));
        if (type < 2) {
            G3C_TexCoord(&ctx->dl, 0, 0x8000);
        }
        for (j = 0; j < count; j++) {
            v = NFLi_ReadU8(&stream) * 3;
            n = NFLi_ReadU8(&stream) * 3;
            tc = NFLi_ReadU8(&stream) * 2;
            if (type > 1) {
                G3C_TexCoord(&ctx->dl, texCoords[tc] << 5, texCoords[tc + 1] << 5);
            }
            NFLi_MapVecFx16(obj, &normal, (const VecFx16 *)&normals[n]);
            G3C_Normal(&ctx->dl, normal.x, normal.y, normal.z);
            NFLi_MapVecFx16(obj, &vertex, (const VecFx16 *)&vertices[v]);
            if (j > 0) {
                if (x == vertex.x) {
                    G3C_VtxYZ(&ctx->dl, vertex.y, vertex.z);
                } else if (y == vertex.y) {
                    G3C_VtxXZ(&ctx->dl, vertex.x, vertex.z);
                } else if (z == vertex.z) {
                    G3C_VtxXY(&ctx->dl, vertex.x, vertex.y);
                } else {
                    G3C_Vtx(&ctx->dl, vertex.x, vertex.y, vertex.z);
                }
            } else {
                G3C_Vtx(&ctx->dl, vertex.x, vertex.y, vertex.z);
            }
            x = vertex.x;
            y = vertex.y;
            z = vertex.z;
        }
        func_02005148(&ctx->dl);
    }
    G3C_PopMtx(&ctx->dl, 1);
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
    NFLFace *obj = ctx->face;
    const NFLCharData *mii = ctx->mii;
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
    ctx->anchors[0].x = NFLi_ReadS32(&stream);
    ctx->anchors[0].y = NFLi_ReadS32(&stream);
    ctx->anchors[0].z = NFLi_ReadS32(&stream);
    ctx->anchors[1].x = NFLi_ReadS32(&stream);
    ctx->anchors[1].y = NFLi_ReadS32(&stream);
    ctx->anchors[1].z = NFLi_ReadS32(&stream);
    ctx->anchors[2].x = NFLi_ReadS32(&stream);
    ctx->anchors[2].y = NFLi_ReadS32(&stream);
    ctx->anchors[2].z = NFLi_ReadS32(&stream);
    if (NFLi_ReadVec16Array(&stream, vertices) == 0) {
        return 0;
    }
    NFLi_ReadVec16Array(&stream, normals);
    NFLi_ReadTexCoordArray(&stream, texCoords);
    polygons = NFLi_ReadU8(&stream);
    if (obj->texFmt == 7) {
        format = GX_TEXFMT_DIRECT;
    } else {
        format = mii->faceTexture != 0 ? GX_TEXFMT_PLTT16 : GX_TEXFMT_PLTT4;
    }
    G3C_TexImageParam(&ctx->dl, format, GX_TEXGEN_NONE, s, t, GX_TEXREPEAT_S, GX_TEXFLIP_S, GX_TEXPLTTCOLOR0_USE,
                      ctx->partAddr[1].texAddr);
    if (obj->texFmt != 7) {
        G3C_TexPlttBase(&ctx->dl, ctx->partAddr[1].plttAddr, format);
    }
    for (i = 0; i < polygons; i++) {
        count = NFLi_ReadU8(&stream);
        func_02005124(&ctx->dl, NFLi_ReadPrimitive(&stream));
        if (mii->faceTexture == 0) {
            G3C_TexCoord(&ctx->dl, 0x8000, 0);
        }
        for (j = 0; j < count; j++) {
            v = NFLi_ReadU8(&stream) * 3;
            n = NFLi_ReadU8(&stream) * 3;
            tc = NFLi_ReadU8(&stream) * 2;
            if (mii->faceTexture != 0) {
                G3C_TexCoord(&ctx->dl, texCoords[tc] << 5, texCoords[tc + 1] << 6);
            }
            NFLi_MapVecFx16(obj, &normal, (const VecFx16 *)&normals[n]);
            G3C_Normal(&ctx->dl, normal.x, normal.y, normal.z);
            NFLi_MapVecFx16(obj, &vertex, (const VecFx16 *)&vertices[v]);
            if (j > 0) {
                if (x == vertex.x) {
                    G3C_VtxYZ(&ctx->dl, vertex.y, vertex.z);
                } else if (y == vertex.y) {
                    G3C_VtxXZ(&ctx->dl, vertex.x, vertex.z);
                } else if (z == vertex.z) {
                    G3C_VtxXY(&ctx->dl, vertex.x, vertex.y);
                } else {
                    G3C_Vtx(&ctx->dl, vertex.x, vertex.y, vertex.z);
                }
            } else {
                G3C_Vtx(&ctx->dl, vertex.x, vertex.y, vertex.z);
            }
            x = vertex.x;
            y = vertex.y;
            z = vertex.z;
        }
        func_02005148(&ctx->dl);
    }
    return 1;
}

// Display-list commands through inline functions. The hair meshes and NFLi_DLScale were compiled without the IR
// optimiser's passes (the pragmas around them), and there the arguments of an inlined call are bound to its
// parameters from the last to the first: the original loads every argument of these commands last to first, which
// a direct call does not give.
static inline void NFLi_PutTranslate(GXDLInfo *dl, fx32 x, fx32 y, fx32 z) {
    G3C_Translate(dl, x, y, z);
}

static inline void NFLi_PutScale(GXDLInfo *dl, fx32 x, fx32 y, fx32 z) {
    G3C_Scale(dl, x, y, z);
}

static inline void NFLi_PutNormal(GXDLInfo *dl, fx16 x, fx16 y, fx16 z) {
    G3C_Normal(dl, x, y, z);
}

static inline void NFLi_PutTexCoord(GXDLInfo *dl, fx32 s, fx32 t) {
    G3C_TexCoord(dl, s, t);
}

static inline void NFLi_PutVtx(GXDLInfo *dl, fx16 x, fx16 y, fx16 z) {
    G3C_Vtx(dl, x, y, z);
}

static inline void NFLi_PutVtxXY(GXDLInfo *dl, fx16 x, fx16 y) {
    G3C_VtxXY(dl, x, y);
}

static inline void NFLi_PutVtxXZ(GXDLInfo *dl, fx16 x, fx16 z) {
    G3C_VtxXZ(dl, x, z);
}

static inline void NFLi_PutVtxYZ(GXDLInfo *dl, fx16 y, fx16 z) {
    G3C_VtxYZ(dl, y, z);
}

static inline void NFLi_PutPolygonAttr(GXDLInfo *dl, int light, GXPolygonMode polyMode, GXCull cullMode, int polygonID,
                                       int alpha, int misc) {
    G3C_PolygonAttr(dl, light, polyMode, cullMode, polygonID, alpha, misc);
}

static inline void NFLi_PutTexImageParam(GXDLInfo *dl, GXTexFmt texFmt, GXTexGen texGen, GXTexSizeS s, GXTexSizeT t,
                                         GXTexRepeat repeat, GXTexFlip flip, GXTexPlttColor0 pltt0, u32 addr) {
    G3C_TexImageParam(dl, texFmt, texGen, s, t, repeat, flip, pltt0, addr);
}

static inline void NFLi_PutTexPlttBase(GXDLInfo *dl, u32 addr, GXTexFmt texFmt) {
    G3C_TexPlttBase(dl, addr, texFmt);
}

// The hair's mesh of category 0x12, untextured in the favourite colour; hair types 0x22 and 0x39 take a texture
// Not matching by one instruction: the original forms &ctx->dl between the loads of y and x for the first
// vertex's G3C_Vtx, where ours forms it after them (NFLi_MakeHairSkinMesh, the same code, matches)
#pragma push
#pragma opt_common_subs off
#pragma opt_propagation off
#pragma opt_loop_invariants off
#pragma opt_strength_reduction off
#pragma opt_rotateloops off
#pragma opt_scalarize off
#pragma opt_rebuildlogicals off
#pragma opt_collapselogicalselectionintobittest off
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
    u8 polygons;
    NFLFace *obj = ctx->face;
    const NFLCharData *mii = ctx->mii;
    BOOL textured;
    GXTexSizeS s;
    GXTexSizeT t;
    GXTexFmt format;
    int j;
    int count;
    int v;
    int n;
    int tc;

    stream = NFLi_GetResourceItem(0x12, mii->hairType);
    textured = mii->hairType == 0x22 || mii->hairType == 0x39;
    s = textured ? GX_TEXSIZE_S32 : GX_TEXSIZE_S8;
    t = textured ? GX_TEXSIZE_T32 : GX_TEXSIZE_T8;
    obj->polyPatches[1].cmd = NULL;
    obj->polyPatches[1].param = NULL;
    obj->polyPatches[1].flip = 0;
    if (NFLi_ReadVec16Array(&stream, vertices) == 0) {
        return 0;
    }
    NFLi_ReadVec16Array(&stream, normals);
    NFLi_ReadTexCoordArray(&stream, texCoords);
    polygons = NFLi_ReadU8(&stream);
    if (obj->texFmt == 7) {
        format = GX_TEXFMT_DIRECT;
    } else {
        format = textured ? GX_TEXFMT_PLTT16 : GX_TEXFMT_PLTT4;
    }
    obj->polyPatches[1].cmd = ctx->dl.curr_cmd;
    obj->polyPatches[1].param = ctx->dl.curr_param;
    obj->polyPatches[1].flip = mii->hairFlip;
    NFLi_PutPolygonAttr(&ctx->dl, 1, GX_POLYGONMODE_MODULATE, mii->hairFlip ? GX_CULL_BACK : GX_CULL_FRONT, 0, 0, 0);
    NFLi_PutTexImageParam(&ctx->dl, format, GX_TEXGEN_NONE, s, t, GX_TEXREPEAT_ST, GX_TEXFLIP_ST, GX_TEXPLTTCOLOR0_USE,
                      ctx->partAddr[5].texAddr);
    if (format != GX_TEXFMT_DIRECT) {
        NFLi_PutTexPlttBase(&ctx->dl, ctx->partAddr[5].plttAddr, format);
    }
    G3C_PushMtx(&ctx->dl);
    position.x = ctx->anchors[2].x;
    position.y = ctx->anchors[2].y;
    position.z = ctx->anchors[2].z;
    NFLi_MapVecFx32(obj, &translation, &position);
    NFLi_PutTranslate(&ctx->dl, translation.x, translation.y, translation.z);
    if (mii->hairFlip) {
        position.x = -FX32_ONE;
        position.y = FX32_ONE;
        position.z = FX32_ONE;
        ((fx32 *)&translation)[obj->xAxis] = position.x;
        ((fx32 *)&translation)[obj->yAxis] = position.y;
        ((fx32 *)&translation)[obj->zAxis] = position.z;
        MTX_Scale43(&mtx, translation.x, translation.y, translation.z);
        G3C_MultMtx43(&ctx->dl, &mtx);
    }
    for (i = 0; i < polygons; i++) {
        count = NFLi_ReadU8(&stream);
        func_02005124(&ctx->dl, NFLi_ReadPrimitive(&stream));
        if (!textured) {
            NFLi_PutTexCoord(&ctx->dl, 0x8000, 0x8000);
        }
        for (j = 0; j < count; j++) {
            v = NFLi_ReadU8(&stream) * 3;
            n = NFLi_ReadU8(&stream) * 3;
            tc = NFLi_ReadU8(&stream) * 2;
            if (textured) {
                NFLi_PutTexCoord(&ctx->dl, texCoords[tc] << 5, texCoords[tc + 1] << 5);
            }
            NFLi_MapVecFx16(obj, &normal, (const VecFx16 *)&normals[n]);
            NFLi_PutNormal(&ctx->dl, normal.x, normal.y, normal.z);
            NFLi_MapVecFx16(obj, &vertex, (const VecFx16 *)&vertices[v]);
            if (j > 0) {
                if (prev.x == vertex.x) {
                    NFLi_PutVtxYZ(&ctx->dl, vertex.y, vertex.z);
                } else if (prev.y == vertex.y) {
                    NFLi_PutVtxXZ(&ctx->dl, vertex.x, vertex.z);
                } else if (prev.z == vertex.z) {
                    NFLi_PutVtxXY(&ctx->dl, vertex.x, vertex.y);
                } else {
                    NFLi_PutVtx(&ctx->dl, vertex.x, vertex.y, vertex.z);
                }
            } else {
                NFLi_PutVtx(&ctx->dl, vertex.x, vertex.y, vertex.z);
            }
            prev.x = vertex.x;
            prev.y = vertex.y;
            prev.z = vertex.z;
        }
        func_02005148(&ctx->dl);
    }
    G3C_PopMtx(&ctx->dl, 1);
    return 1;
}
#pragma pop

// The hair's other mesh, for the types with a texture of their own
// Not matching by one instruction: the original forms &ctx->dl between the loads of y and x for the first
// vertex's G3C_Vtx, where ours forms it after them (NFLi_MakeHairSkinMesh, the same code, matches)
#pragma push
#pragma opt_common_subs off
#pragma opt_propagation off
#pragma opt_loop_invariants off
#pragma opt_strength_reduction off
#pragma opt_rotateloops off
#pragma opt_scalarize off
#pragma opt_rebuildlogicals off
#pragma opt_collapselogicalselectionintobittest off
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
    u8 polygons;
    NFLFace *obj = ctx->face;
    const NFLCharData *mii = ctx->mii;
    int type;
    BOOL textured;
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
    textured = type == 5 || type == 7 || type == 0x18 || type == 0x1D || type == 0x2A || type == 0x35 || type == 0x3A ||
           type == 0x45 || type == 0x47;
    s = textured ? GX_TEXSIZE_S32 : GX_TEXSIZE_S8;
    t = textured ? GX_TEXSIZE_T32 : GX_TEXSIZE_T8;
    obj->polyPatches[0].cmd = NULL;
    obj->polyPatches[0].param = NULL;
    obj->polyPatches[0].flip = 0;
    if (NFLi_ReadVec16Array(&stream, vertices) == 0) {
        return 0;
    }
    NFLi_ReadVec16Array(&stream, normals);
    NFLi_ReadTexCoordArray(&stream, texCoords);
    polygons = NFLi_ReadU8(&stream);
    if (obj->texFmt == 7) {
        format = GX_TEXFMT_DIRECT;
    } else {
        format = textured ? GX_TEXFMT_PLTT16 : GX_TEXFMT_PLTT4;
    }
    obj->polyPatches[0].cmd = ctx->dl.curr_cmd;
    obj->polyPatches[0].param = ctx->dl.curr_param;
    obj->polyPatches[0].flip = mii->hairFlip;
    NFLi_PutPolygonAttr(&ctx->dl, 1, GX_POLYGONMODE_MODULATE, mii->hairFlip ? GX_CULL_BACK : GX_CULL_FRONT, 0, 0, 0);
    NFLi_PutTexImageParam(&ctx->dl, format, GX_TEXGEN_NONE, s, t, GX_TEXREPEAT_ST, GX_TEXFLIP_NONE,
                      type == 0x1D ? GX_TEXPLTTCOLOR0_TRNS : GX_TEXPLTTCOLOR0_USE, ctx->partAddr[2].texAddr);
    if (obj->texFmt != 7) {
        NFLi_PutTexPlttBase(&ctx->dl, ctx->partAddr[2].plttAddr, format);
    }
    G3C_PushMtx(&ctx->dl);
    position.x = ctx->anchors[2].x;
    position.y = ctx->anchors[2].y;
    position.z = ctx->anchors[2].z;
    NFLi_MapVecFx32(obj, &translation, &position);
    NFLi_PutTranslate(&ctx->dl, translation.x, translation.y, translation.z);
    if (mii->hairFlip) {
        position.x = -FX32_ONE;
        position.y = FX32_ONE;
        position.z = FX32_ONE;
        ((fx32 *)&translation)[obj->xAxis] = position.x;
        ((fx32 *)&translation)[obj->yAxis] = position.y;
        ((fx32 *)&translation)[obj->zAxis] = position.z;
        MTX_Scale43(&mtx, translation.x, translation.y, translation.z);
        G3C_MultMtx43(&ctx->dl, &mtx);
    }
    for (i = 0; i < polygons; i++) {
        count = NFLi_ReadU8(&stream);
        func_02005124(&ctx->dl, NFLi_ReadPrimitive(&stream));
        if (!textured) {
            NFLi_PutTexCoord(&ctx->dl, 0, 0);
        }
        for (j = 0; j < count; j++) {
            v = NFLi_ReadU8(&stream) * 3;
            n = NFLi_ReadU8(&stream) * 3;
            tc = NFLi_ReadU8(&stream) * 2;
            if (textured) {
                NFLi_PutTexCoord(&ctx->dl, texCoords[tc] << 5, texCoords[tc + 1] << 5);
            }
            NFLi_MapVecFx16(obj, &normal, (const VecFx16 *)&normals[n]);
            NFLi_PutNormal(&ctx->dl, normal.x, normal.y, normal.z);
            NFLi_MapVecFx16(obj, &vertex, (const VecFx16 *)&vertices[v]);
            if (j > 0) {
                if (prev.x == vertex.x) {
                    NFLi_PutVtxYZ(&ctx->dl, vertex.y, vertex.z);
                } else if (prev.y == vertex.y) {
                    NFLi_PutVtxXZ(&ctx->dl, vertex.x, vertex.z);
                } else if (prev.z == vertex.z) {
                    NFLi_PutVtxXY(&ctx->dl, vertex.x, vertex.y);
                } else {
                    NFLi_PutVtx(&ctx->dl, vertex.x, vertex.y, vertex.z);
                }
            } else {
                NFLi_PutVtx(&ctx->dl, vertex.x, vertex.y, vertex.z);
            }
            prev.x = vertex.x;
            prev.y = vertex.y;
            prev.z = vertex.z;
        }
        func_02005148(&ctx->dl);
    }
    G3C_PopMtx(&ctx->dl, 1);
    return 1;
}
#pragma pop

// The hair's third mesh, untextured
#pragma push
#pragma opt_common_subs off
#pragma opt_propagation off
#pragma opt_loop_invariants off
#pragma opt_strength_reduction off
#pragma opt_rotateloops off
#pragma opt_scalarize off
#pragma opt_rebuildlogicals off
#pragma opt_collapselogicalselectionintobittest off
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
    u8 polygons;
    NFLFace *obj = ctx->face;
    const NFLCharData *mii = ctx->mii;
    GXTexFmt format;
    int j;
    int count;
    int v;
    int n;

    stream = NFLi_GetResourceItem(6, mii->hairType);
    format = obj->texFmt == 7 ? GX_TEXFMT_DIRECT : GX_TEXFMT_PLTT4;
    obj->polyPatches[2].cmd = NULL;
    obj->polyPatches[2].param = NULL;
    obj->polyPatches[2].flip = 0;
    if (NFLi_ReadVec16Array(&stream, vertices) == 0) {
        return 0;
    }
    NFLi_ReadVec16Array(&stream, normals);
    polygons = NFLi_ReadU8(&stream);
    obj->polyPatches[2].cmd = ctx->dl.curr_cmd;
    obj->polyPatches[2].param = ctx->dl.curr_param;
    obj->polyPatches[2].flip = mii->hairFlip;
    NFLi_PutPolygonAttr(&ctx->dl, 1, GX_POLYGONMODE_MODULATE, mii->hairFlip ? GX_CULL_BACK : GX_CULL_FRONT, 0, 0, 0);
    NFLi_PutTexImageParam(&ctx->dl, format, GX_TEXGEN_NONE, GX_TEXSIZE_S8, GX_TEXSIZE_T8, GX_TEXREPEAT_NONE,
                      GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, obj->colorTexAddr);
    if (obj->texFmt != 7) {
        NFLi_PutTexPlttBase(&ctx->dl, obj->colorPlttAddr, format);
    }
    G3C_PushMtx(&ctx->dl);
    position.x = ctx->anchors[2].x;
    position.y = ctx->anchors[2].y;
    position.z = ctx->anchors[2].z;
    NFLi_MapVecFx32(obj, &translation, &position);
    NFLi_PutTranslate(&ctx->dl, translation.x, translation.y, translation.z);
    if (mii->hairFlip) {
        position.x = -FX32_ONE;
        position.y = FX32_ONE;
        position.z = FX32_ONE;
        ((fx32 *)&translation)[obj->xAxis] = position.x;
        ((fx32 *)&translation)[obj->yAxis] = position.y;
        ((fx32 *)&translation)[obj->zAxis] = position.z;
        MTX_Scale43(&mtx, translation.x, translation.y, translation.z);
        G3C_MultMtx43(&ctx->dl, &mtx);
    }
    for (i = 0; i < polygons; i++) {
        count = NFLi_ReadU8(&stream);
        func_02005124(&ctx->dl, NFLi_ReadPrimitive(&stream));
        NFLi_PutTexCoord(&ctx->dl, 0x8000, 0);
        for (j = 0; j < count; j++) {
            v = NFLi_ReadU8(&stream) * 3;
            n = NFLi_ReadU8(&stream) * 3;
            NFLi_MapVecFx16(obj, &normal, (const VecFx16 *)&normals[n]);
            NFLi_PutNormal(&ctx->dl, normal.x, normal.y, normal.z);
            NFLi_MapVecFx16(obj, &vertex, (const VecFx16 *)&vertices[v]);
            if (j > 0) {
                if (prev.x == vertex.x) {
                    NFLi_PutVtxYZ(&ctx->dl, vertex.y, vertex.z);
                } else if (prev.y == vertex.y) {
                    NFLi_PutVtxXZ(&ctx->dl, vertex.x, vertex.z);
                } else if (prev.z == vertex.z) {
                    NFLi_PutVtxXY(&ctx->dl, vertex.x, vertex.y);
                } else {
                    NFLi_PutVtx(&ctx->dl, vertex.x, vertex.y, vertex.z);
                }
            } else {
                NFLi_PutVtx(&ctx->dl, vertex.x, vertex.y, vertex.z);
            }
            prev.x = vertex.x;
            prev.y = vertex.y;
            prev.z = vertex.z;
        }
        func_02005148(&ctx->dl);
    }
    G3C_PopMtx(&ctx->dl, 1);
    return 1;
}
#pragma pop

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
    NFLFace *obj = ctx->face;
    const NFLCharData *mii = ctx->mii;
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
    format = obj->texFmt == 7 ? GX_TEXFMT_DIRECT : GX_TEXFMT_PLTT16;
    y0 = (11 - mii->glassesY) * 114 + 387;
    scale = (mii->glassesScale - 4) * 633 + 3987;
    obj->polyPatches[3].cmd = NULL;
    obj->polyPatches[3].param = NULL;
    obj->polyPatches[3].flip = 0;
    if (mii->glassesType == 0) {
        return 0;
    }
    if (NFLi_ReadVec16Array(&stream, vertices) == 0) {
        return 0;
    }
    NFLi_ReadVec16Array(&stream, normals);
    NFLi_ReadTexCoordArray(&stream, texCoords);
    polygons = NFLi_ReadU8(&stream);
    obj->polyPatches[3].cmd = ctx->dl.curr_cmd;
    obj->polyPatches[3].param = ctx->dl.curr_param;
    obj->polyPatches[3].flip = 0;
    G3C_PolygonAttr(&ctx->dl, 1, GX_POLYGONMODE_MODULATE, GX_CULL_NONE, 0, 0, 0);
    G3C_TexImageParam(&ctx->dl, format, GX_TEXGEN_NONE, GX_TEXSIZE_S32, GX_TEXSIZE_T64, GX_TEXREPEAT_S, GX_TEXFLIP_S,
                      GX_TEXPLTTCOLOR0_TRNS, ctx->partAddr[4].texAddr);
    if (format != GX_TEXFMT_DIRECT) {
        G3C_TexPlttBase(&ctx->dl, ctx->partAddr[4].plttAddr, GX_TEXFMT_PLTT16);
    }
    G3C_PushMtx(&ctx->dl);
    position.x = ctx->anchors[0].x;
    position.y = ctx->anchors[0].y + y0;
    position.z = ctx->anchors[0].z + 0xA3;
    NFLi_MapVecFx32(obj, &translation, &position);
    G3C_Translate(&ctx->dl, translation.x, translation.y, translation.z);
    G3C_Scale(&ctx->dl, scale, scale, scale);
    for (i = 0; i < polygons; i++) {
        count = NFLi_ReadU8(&stream);
        func_02005124(&ctx->dl, NFLi_ReadPrimitive(&stream));
        for (j = 0; j < count; j++) {
            v = NFLi_ReadU8(&stream) * 3;
            n = NFLi_ReadU8(&stream) * 3;
            tc = NFLi_ReadU8(&stream) * 2;
            G3C_TexCoord(&ctx->dl, texCoords[tc] << 5, texCoords[tc + 1] << 6);
            NFLi_MapVecFx16(obj, &normal, (const VecFx16 *)&normals[n]);
            G3C_Normal(&ctx->dl, normal.x, normal.y, normal.z);
            NFLi_MapVecFx16(obj, &vertex, (const VecFx16 *)&vertices[v]);
            if (j > 0) {
                if (x == vertex.x) {
                    G3C_VtxYZ(&ctx->dl, vertex.y, vertex.z);
                } else if (y == vertex.y) {
                    G3C_VtxXZ(&ctx->dl, vertex.x, vertex.z);
                } else if (z == vertex.z) {
                    G3C_VtxXY(&ctx->dl, vertex.x, vertex.y);
                } else {
                    G3C_Vtx(&ctx->dl, vertex.x, vertex.y, vertex.z);
                }
            } else {
                G3C_Vtx(&ctx->dl, vertex.x, vertex.y, vertex.z);
            }
            x = vertex.x;
            y = vertex.y;
            z = vertex.z;
        }
        func_02005148(&ctx->dl);
    }
    G3C_PopMtx(&ctx->dl, 1);
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
    NFLFace *obj = ctx->face;
    const NFLCharData *mii = ctx->mii;
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
    format = obj->texFmt == 7 ? GX_TEXFMT_DIRECT : GX_TEXFMT_PLTT16;
    y0 = (mii->noseY - 8) * -114 + 32;
    scale = (mii->noseScale - 4) * 685 + 4377;
    if (NFLi_ReadVec16Array(&stream, vertices) == 0) {
        return 0;
    }
    NFLi_ReadVec16Array(&stream, normals);
    NFLi_ReadTexCoordArray(&stream, texCoords);
    polygons = NFLi_ReadU8(&stream);
    G3C_TexImageParam(&ctx->dl, format, GX_TEXGEN_NONE, GX_TEXSIZE_S32, GX_TEXSIZE_T64, GX_TEXREPEAT_S, GX_TEXFLIP_S,
                      GX_TEXPLTTCOLOR0_TRNS, ctx->partAddr[6].texAddr);
    if (format != GX_TEXFMT_DIRECT) {
        G3C_TexPlttBase(&ctx->dl, ctx->partAddr[6].plttAddr, GX_TEXFMT_PLTT16);
    }
    G3C_PushMtx(&ctx->dl);
    position.x = ctx->anchors[0].x;
    position.y = ctx->anchors[0].y + y0;
    position.z = ctx->anchors[0].z;
    NFLi_MapVecFx32(obj, &translation, &position);
    G3C_Translate(&ctx->dl, translation.x, translation.y, translation.z);
    G3C_Scale(&ctx->dl, scale, scale, scale);
    for (i = 0; i < polygons; i++) {
        count = NFLi_ReadU8(&stream);
        func_02005124(&ctx->dl, NFLi_ReadPrimitive(&stream));
        for (j = 0; j < count; j++) {
            v = NFLi_ReadU8(&stream) * 3;
            n = NFLi_ReadU8(&stream) * 3;
            tc = NFLi_ReadU8(&stream) * 2;
            G3C_TexCoord(&ctx->dl, texCoords[tc] << 5, texCoords[tc + 1] << 6);
            NFLi_MapVecFx16(obj, &normal, (const VecFx16 *)&normals[n]);
            G3C_Normal(&ctx->dl, normal.x, normal.y, normal.z);
            NFLi_MapVecFx16(obj, &vertex, (const VecFx16 *)&vertices[v]);
            if (j > 0) {
                if (x == vertex.x) {
                    G3C_VtxYZ(&ctx->dl, vertex.y, vertex.z);
                } else if (y == vertex.y) {
                    G3C_VtxXZ(&ctx->dl, vertex.x, vertex.z);
                } else if (z == vertex.z) {
                    G3C_VtxXY(&ctx->dl, vertex.x, vertex.y);
                } else {
                    G3C_Vtx(&ctx->dl, vertex.x, vertex.y, vertex.z);
                }
            } else {
                G3C_Vtx(&ctx->dl, vertex.x, vertex.y, vertex.z);
            }
            x = vertex.x;
            y = vertex.y;
            z = vertex.z;
        }
        func_02005148(&ctx->dl);
    }
    G3C_PopMtx(&ctx->dl, 1);
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
    NFLFace *obj = ctx->face;
    const NFLCharData *mii = ctx->mii;
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
    format = obj->texFmt == 7 ? GX_TEXFMT_DIRECT : GX_TEXFMT_PLTT4;
    y0 = (mii->noseY - 8) * -114 + 32;
    scale = (mii->noseScale - 4) * 685 + 4377;
    if (NFLi_ReadVec16Array(&stream, vertices) == 0) {
        return 0;
    }
    NFLi_ReadVec16Array(&stream, normals);
    polygons = NFLi_ReadU8(&stream);
    G3C_TexImageParam(&ctx->dl, format, GX_TEXGEN_NONE, GX_TEXSIZE_S8, GX_TEXSIZE_T8, GX_TEXREPEAT_NONE,
                      GX_TEXFLIP_NONE, GX_TEXPLTTCOLOR0_USE, obj->colorTexAddr);
    if (obj->texFmt != 7) {
        G3C_TexPlttBase(&ctx->dl, obj->colorPlttAddr, format);
    }
    G3C_PushMtx(&ctx->dl);
    position.x = ctx->anchors[0].x;
    position.y = ctx->anchors[0].y + y0;
    position.z = ctx->anchors[0].z;
    NFLi_MapVecFx32(obj, &translation, &position);
    G3C_Translate(&ctx->dl, translation.x, translation.y, translation.z);
    G3C_Scale(&ctx->dl, scale, scale, scale);
    for (i = 0; i < polygons; i++) {
        count = NFLi_ReadU8(&stream);
        func_02005124(&ctx->dl, NFLi_ReadPrimitive(&stream));
        G3C_TexCoord(&ctx->dl, 0x8000, 0);
        for (j = 0; j < count; j++) {
            v = NFLi_ReadU8(&stream) * 3;
            NFLi_MapVecFx16(obj, &normal, (const VecFx16 *)&normals[NFLi_ReadU8(&stream) * 3]);
            G3C_Normal(&ctx->dl, normal.x, normal.y, normal.z);
            NFLi_MapVecFx16(obj, &vertex, (const VecFx16 *)&vertices[v]);
            if (j > 0) {
                if (x == vertex.x) {
                    G3C_VtxYZ(&ctx->dl, vertex.y, vertex.z);
                } else if (y == vertex.y) {
                    G3C_VtxXZ(&ctx->dl, vertex.x, vertex.z);
                } else if (z == vertex.z) {
                    G3C_VtxXY(&ctx->dl, vertex.x, vertex.y);
                } else {
                    G3C_Vtx(&ctx->dl, vertex.x, vertex.y, vertex.z);
                }
            } else {
                G3C_Vtx(&ctx->dl, vertex.x, vertex.y, vertex.z);
            }
            x = vertex.x;
            y = vertex.y;
            z = vertex.z;
        }
        func_02005148(&ctx->dl);
    }
    G3C_PopMtx(&ctx->dl, 1);
    return 1;
}

extern const u16 sNFLFaceGXColors[];
extern const u16 sNFLHairGXColors[];
extern const u16 sNFLFavoriteGXColors[];

// The colour texture, which the untextured meshes take their colour from by their texture
// coordinate: an 8x8 image of 2-bit pixels in four 4x4 quadrants of colours 0 to 3
void NFLi_MakeColorTexture(u16 *palette) {
    int i;

    for (i = 0; i < 4; i++) {
        palette[i] = 0x5500;
        palette[i + 4] = 0xFFAA;
    }
    DC_FlushRange(palette, 0x10);
}

// Its palette: the hair colour (0), face colour (1), beard colour (2) and favourite colour (3) of
// the kept fields
void NFLi_MakeColorTexturePltt(u16 *palette, const s16 *params) {
    palette[0] = sNFLHairGXColors[params[4]];
    palette[1] = sNFLFaceGXColors[params[2]];
    palette[2] = sNFLHairGXColors[params[14]];
    palette[3] = sNFLFavoriteGXColors[params[0]];
    DC_FlushRange(palette, 0x10);
}

// The colour texture in mode 7: 8x8 direct colours, the same four by quadrant
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

// The colour texture's part function in the palette modes: its texture and palette streams
// (0) are to load when a colour changed, and it goes at the start of the face's VRAM
int NFLi_PrepareColorTexturePltt(NFLMakeCtx *ctx) {
    NFLFace *obj = ctx->face;
    const NFLCharData *mii = ctx->mii;
    s16 *params = obj->keptFields;

    if (params[4] != mii->hairColor || params[2] != mii->faceColor || params[14] != mii->beardColor ||
        params[0] != mii->favoriteColor || (obj->texStreams[0].state >= 1 && obj->texStreams[0].state <= 6) ||
        (obj->plttStreams[0].state >= 1 && obj->plttStreams[0].state <= 6)) {
        obj->texStreams[0].state = 0;
        obj->texStreams[0].value = mii->favoriteColor;
        obj->plttStreams[0].state = 0;
        obj->plttStreams[0].value = mii->favoriteColor;
    }
    obj->colorTexAddr = obj->texBase;
    obj->colorPlttAddr = obj->plttBase;
    return 0;
}

// The same in mode 7, which has no palette
int NFLi_PrepareColorTextureDirect(NFLMakeCtx *ctx) {
    NFLFace *obj = ctx->face;
    const NFLCharData *mii = ctx->mii;
    s16 *params = obj->keptFields;

    if (params[4] != mii->hairColor || params[2] != mii->faceColor || params[14] != mii->beardColor ||
        params[0] != mii->favoriteColor || (obj->texStreams[0].state >= 1 && obj->texStreams[0].state <= 6)) {
        obj->texStreams[0].state = 0;
        obj->texStreams[0].value = mii->favoriteColor;
    }
    obj->colorTexAddr = obj->texBase;
    obj->plttStreams[0].state = 7;
    return 0;
}

int NFLi_PrepareColorTexture(NFLMakeCtx *ctx) {
    if (ctx->face->texFmt == 7) {
        return NFLi_PrepareColorTextureDirect(ctx);
    }
    return NFLi_PrepareColorTexturePltt(ctx);
}

// The stream of item index of category in the library's resource
const u8 *NFLi_GetResourceItem(int category, u32 index) {
    const u8 *header = sNFLWork->resource;
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
    items = sNFLWork->resource + offsets[category];
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

// Forgets the kept fields (-1), so that every part is made again
void NFLi_ResetKeptFields(NFLFace *obj) {
    s16 *params = obj->keptFields;

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
void NFLi_KeepFields(NFLFace *obj, const NFLCharData *mii) {
    s16 *params = obj->keptFields;

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

// The axes of the library's coordinates (sNFLDefaultAxes), which NFL_MakeFace gives each face
typedef struct NFLAxes {
    VecFx32 negate;
    u8 yAxis;
    u8 zAxis;
    u8 xAxis;
} NFLAxes;

extern int (*const sNFLPartPreparers[])(NFLMakeCtx *ctx);
extern const NFLAxes sNFLDefaultAxes;

// Makes face index's display lists from a Mii's data, and sets its streams to load what changed. partDepth is added
// to the depth of the face parts
#pragma optimization_level 2
#pragma opt_common_subs off
int NFL_MakeFace(int index, const NFLCharData *mii, u32 partDepth) {
    OSIntrMode enabled;
    NFLMakeCtx ctx;
    NFLFace *obj;
    NFLStream *entries;
    NFLStream *entries2;
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
    if (sNFLWork->phase == 1 || sNFLWork->phase == 3) {
        return 6;
    }
    obj = NFLi_GetFace(index);
    if (obj == NULL) {
        return 9;
    }
    entries = obj->texStreams;
    entries2 = obj->plttStreams;
    if (!NFLi_IsFaceDataInRange(mii) || !NFLi_IsValidCharData(mii)) {
        return 7;
    }
    ctx.face = obj;
    ctx.mii = mii;
    obj->texFmt = sNFLWork->texFmt;
    obj->expression = 0;
    obj->exprFlags = sNFLWork->exprFlags;
    obj->dlSize = 0;
    obj->partsDlSize = 0;
    obj->partDepth = partDepth;
    obj->mii = mii;
    obj->loadedSize = 0;
    obj->copiedSize = 0;
    if (obj->state == 4) {
        if (obj->texFmt == 7) {
            obj->texBase = ctx.texBase = sNFLWork->faceTexBase + index * 0x6880;
            obj->plttBase = ctx.plttBase = sNFLWork->facePlttBase;
        } else {
            obj->texBase = ctx.texBase = sNFLWork->faceTexBase + index * 0x1A10;
            obj->plttBase = ctx.plttBase = sNFLWork->facePlttBase + index * 0x130;
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
        ctx.texBase = obj->texBase;
        ctx.plttBase = obj->plttBase;
    }
    obj->yAxis = sNFLDefaultAxes.yAxis;
    obj->zAxis = sNFLDefaultAxes.zAxis;
    obj->xAxis = sNFLDefaultAxes.xAxis;
    obj->negate.x = sNFLDefaultAxes.negate.x;
    obj->negate.y = sNFLDefaultAxes.negate.y;
    obj->negate.z = sNFLDefaultAxes.negate.z;
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
            obj->state = 0;
            found = TRUE;
            break;
        }
    }
    if (!found) {
        for (i = 0; i < 10; i++) {
            if (entries2[i].state == 0) {
                found = TRUE;
                obj->state = found;
                break;
            }
        }
    }
    if (found == TRUE) {
        NFLFace *p;

        if (sNFLWork->loadList == obj) {
            sNFLWork->loadList = obj->nextLoad;
        }
        p = sNFLWork->loadList;
        if (p == NULL) {
            sNFLWork->loadList = obj;
        } else {
            while (p->nextLoad != NULL) {
                if (p->nextLoad == obj) {
                    p->nextLoad = obj->nextLoad;
                    if (p->nextLoad == NULL) {
                        break;
                    }
                }
                p = p->nextLoad;
            }
            p->nextLoad = obj;
        }
    } else {
        obj->state = 2;
    }
    obj->nextLoad = NULL;
    if (sNFLWork->phase == 4) {
        sNFLWork->phase = 0;
    }
    OS_RestoreInterrupts(enabled);
    G3_BeginMakeDL(&ctx.dl, obj->dl, sizeof(obj->dl));
    NFLi_MakeFaceMesh(&ctx);
    NFLi_MakeBeardMesh(&ctx);
    NFLi_MakeNoseSkinMesh(&ctx);
    NFLi_MakeNoseMesh(&ctx);
    NFLi_MakeFavoriteColorHairMesh(&ctx);
    NFLi_MakeHairMesh(&ctx);
    NFLi_MakeHairSkinMesh(&ctx);
    NFLi_MakeGlassesMesh(&ctx);
    obj->dlSize = G3_EndMakeDL(&ctx.dl);
    G3_BeginMakeDL(&ctx.dl, obj->partsDl, sizeof(obj->partsDl));
    NFLi_MakeFacePartsDL(&ctx);
    obj->partsDlSize = G3_EndMakeDL(&ctx.dl);
    NFLi_KeepFields(obj, mii);
    NFL_SetExpression(index, 0);
    return 0;
}
#pragma opt_common_subs reset
#pragma optimization_level reset

// Sets the matrices NFL_DrawFace draws face index under: a position and a vector matrix
int NFL_SetFaceMatrices(int index, const MtxFx43 *mtx43, const MtxFx33 *mtx33) {
    NFLFace *obj;

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
    MI_Copy48B(mtx43, &obj->mtx);
    MI_Copy36B(mtx33, &obj->nrmMtx);
    return 0;
}

// Sets the polygon attributes of a face's display list (at the places its makers kept) and the material colours
int NFL_SetFaceDrawParam(int index, const NFLDrawParam *param) {
    NFLFace *obj;
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
    if (obj->polyPatches[0].cmd != NULL && obj->polyPatches[0].param != NULL) {
        info.curr_cmd = obj->polyPatches[0].cmd;
        info.curr_param = obj->polyPatches[0].param;
        G3CS_PolygonAttr(&info, param->light, param->polyMode, obj->polyPatches[0].flip ? GX_CULL_BACK : GX_CULL_FRONT,
                         param->polygonID, param->alpha, param->misc);
    }
    if (obj->polyPatches[1].cmd != NULL && obj->polyPatches[1].param != NULL) {
        info.curr_cmd = obj->polyPatches[1].cmd;
        info.curr_param = obj->polyPatches[1].param;
        G3CS_PolygonAttr(&info, param->light, param->polyMode, obj->polyPatches[1].flip ? GX_CULL_BACK : GX_CULL_FRONT,
                         param->polygonID, param->alpha, param->misc);
    }
    if (obj->polyPatches[2].cmd != NULL && obj->polyPatches[2].param != NULL) {
        info.curr_cmd = obj->polyPatches[2].cmd;
        info.curr_param = obj->polyPatches[2].param;
        G3CS_PolygonAttr(&info, param->light, param->polyMode, obj->polyPatches[2].flip ? GX_CULL_BACK : GX_CULL_FRONT,
                         param->polygonID, param->alpha, param->misc);
    }
    if (obj->polyPatches[3].cmd != NULL && obj->polyPatches[3].param != NULL) {
        info.curr_cmd = obj->polyPatches[3].cmd;
        info.curr_param = obj->polyPatches[3].param;
        G3CS_PolygonAttr(&info, param->light, param->polyMode, GX_CULL_NONE, param->polygonID, param->alpha,
                         param->misc);
    }
    return 0;
}

// Sends a face's display lists to the geometry engine, under its matrices, once it is loaded
int NFL_DrawFace(int index, BOOL withSecond) {
    NFLFace *obj;

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
    G3_LoadMtx43((const MtxFx43 *)&obj->nrmMtx);
    G3_MtxMode(GX_MTXMODE_POSITION);
    G3_LoadMtx43(&obj->mtx);
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    MI_WaitDma(GXi_DmaId);
    if (withSecond && obj->partsDlSize != 0) {
        MI_CpuSend32(obj->partsDl, &reg_G3X_GXFIFO, obj->partsDlSize);
    }
    if (obj->dlSize != 0) {
        MI_CpuSend32(obj->dl, &reg_G3X_GXFIFO, obj->dlSize);
    }
    G3_PopMtx(1);
    return 0;
}

// 0 when face index is loaded and can be drawn, 0xA when its loading failed, 0xF while it is not ready
int NFL_GetFaceStatus(int index) {
    NFLFace *obj;

    if (!NFLi_IsInitialized()) {
        return 4;
    }
    obj = NFLi_GetFace(index);
    if (obj == NULL) {
        return 9;
    }
    if (obj->state == 3) {
        return 0xA;
    }
    if (obj->state != 2) {
        return 0xF;
    }
    return 0;
}

extern const NFLColor sNFLFaceColors[];

// The face colour index as a GXRgb
GXRgb NFLi_GetFaceColor(int index) {
    return GX_RGB(sNFLFaceColors[index].r >> 3, sNFLFaceColors[index].g >> 3, sNFLFaceColors[index].b >> 3);
}

// The favourite colour index as a GXRgb
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
    NFLFace *obj = ctx->face;
    const NFLCharData *mii = ctx->mii;
    s16 *params = obj->keptFields;
    int i;

    obj->exprFields[0] = mii->eyeColor;
    obj->exprFields[1] = mii->mouthColor;
    obj->exprFields[2] = mii->eyebrowY;
    obj->exprFields[3] = mii->eyeRotation;
    obj->exprFields[4] = mii->eyeType;
    obj->exprFields[5] = mii->faceType;
    obj->exprFields[7] = mii->eyebrowType;
    obj->exprFields[6] = mii->eyebrowRotation;
    for (i = 0; i < 5; i++) {
        switch (i) {
        case 0:
            ctx->parts[0].texSizeS = 2;
            ctx->parts[1].texSizeS = 2;
            ctx->parts[0].texSizeT = 2;
            ctx->parts[1].texSizeT = 2;
            if (params[7] != mii->eyeType || (obj->texStreams[1].state >= 1 && obj->texStreams[1].state <= 6)) {
                obj->texStreams[1].state = 0;
                obj->texStreams[1].value = mii->eyeType;
            }
            if (params[8] != mii->eyeColor || (obj->plttStreams[1].state >= 1 && obj->plttStreams[1].state <= 6)) {
                if (obj->texFmt == 7) {
                    obj->texStreams[1].state = 0;
                    obj->plttStreams[1].state = 7;
                } else {
                    obj->plttStreams[1].state = 0;
                }
                obj->plttStreams[1].value = mii->eyeColor;
            }
            if (obj->texFmt == 7) {
                ctx->parts[0].texAddr = obj->texBase + 0x80;
                ctx->parts[1].texAddr = obj->texBase + 0x80;
                ctx->parts[0].plttAddr = obj->plttBase;
                ctx->parts[1].plttAddr = obj->plttBase;
                obj->eyeTexAddr = obj->texBase + 0x80;
            } else {
                ctx->parts[0].texAddr = obj->texBase + 0x10;
                ctx->parts[1].texAddr = obj->texBase + 0x10;
                ctx->parts[0].plttAddr = obj->plttBase + 0x10;
                ctx->parts[1].plttAddr = obj->plttBase + 0x10;
                obj->eyeTexAddr = obj->texBase + 0x10;
                obj->eyePlttAddr = obj->plttBase + 0x10;
            }
            break;
        case 1:
            ctx->parts[2].texSizeS = 2;
            ctx->parts[3].texSizeS = 2;
            ctx->parts[2].texSizeT = 2;
            ctx->parts[3].texSizeT = 2;
            if (params[5] != mii->eyebrowType || (obj->texStreams[2].state >= 1 && obj->texStreams[2].state <= 6)) {
                obj->texStreams[2].state = 0;
                obj->texStreams[2].value = mii->eyebrowType;
            }
            if (params[6] != mii->eyebrowColor || (obj->plttStreams[2].state >= 1 && obj->plttStreams[2].state <= 6)) {
                if (obj->texFmt == 7) {
                    obj->texStreams[2].state = 0;
                    obj->plttStreams[2].state = 7;
                } else {
                    obj->plttStreams[2].state = 0;
                }
                obj->plttStreams[2].value = mii->eyebrowColor;
            }
            if (obj->texFmt == 7) {
                ctx->parts[2].texAddr = obj->texBase + 0x880;
                ctx->parts[3].texAddr = obj->texBase + 0x880;
                ctx->parts[2].plttAddr = obj->plttBase;
                ctx->parts[3].plttAddr = obj->plttBase;
            } else {
                ctx->parts[2].texAddr = obj->texBase + 0x210;
                ctx->parts[3].texAddr = obj->texBase + 0x210;
                ctx->parts[2].plttAddr = obj->plttBase + 0x30;
                ctx->parts[3].plttAddr = obj->plttBase + 0x30;
            }
            break;
        case 2:
            ctx->parts[4].texSizeS = 2;
            ctx->parts[4].texSizeT = 2;
            if (params[11] != mii->mouthType || (obj->texStreams[3].state >= 1 && obj->texStreams[3].state <= 6)) {
                obj->texStreams[3].state = 0;
                obj->texStreams[3].value = mii->mouthType;
            }
            if (params[10] != mii->mouthColor || (obj->plttStreams[3].state >= 1 && obj->plttStreams[3].state <= 6)) {
                if (obj->texFmt == 7) {
                    obj->texStreams[3].state = 0;
                    obj->plttStreams[3].state = 7;
                } else {
                    obj->plttStreams[3].state = 0;
                }
                obj->plttStreams[3].value = mii->mouthColor;
            }
            if (obj->texFmt == 7) {
                ctx->parts[4].texAddr = obj->texBase + 0x1080;
                ctx->parts[4].plttAddr = obj->plttBase;
                obj->mouthTexAddr = obj->texBase + 0x1080;
            } else {
                ctx->parts[4].texAddr = obj->texBase + 0x410;
                ctx->parts[4].plttAddr = obj->plttBase + 0x50;
                obj->mouthTexAddr = obj->texBase + 0x410;
                obj->mouthPlttAddr = obj->plttBase + 0x50;
            }
            break;
        case 3:
            ctx->parts[5].texSizeS = 1;
            ctx->parts[5].texSizeT = 3;
            if (params[16] != mii->mustacheType || (obj->texStreams[4].state >= 1 && obj->texStreams[4].state <= 6)) {
                obj->texStreams[4].state = 0;
                obj->texStreams[4].value = mii->mustacheType;
            }
            if (params[14] != mii->beardColor || (obj->plttStreams[4].state >= 1 && obj->plttStreams[4].state <= 6)) {
                if (obj->texFmt != 7) {
                    obj->plttStreams[4].state = 0;
                } else {
                    obj->plttStreams[4].state = 7;
                    obj->texStreams[4].state = 0;
                }
                obj->plttStreams[4].value = mii->beardColor;
            }
            if (obj->texFmt == 7) {
                ctx->parts[5].texAddr = obj->texBase + 0x1880;
                ctx->parts[5].plttAddr = obj->plttBase;
            } else {
                ctx->parts[5].texAddr = obj->texBase + 0x610;
                ctx->parts[5].plttAddr = obj->plttBase + 0x70;
            }
            break;
        case 4:
            ctx->parts[6].texSizeS = 1;
            ctx->parts[6].texSizeT = 1;
            if (sNFLWork->sharedTex[5].allocated == 0) {
                ctx->parts[6].texAddr = sNFLWork->texAddr;
                ctx->parts[6].plttAddr = sNFLWork->plttAddr;
                sNFLWork->sharedTex[5].texAddr = sNFLWork->texAddr;
                sNFLWork->sharedTex[5].plttAddr = sNFLWork->plttAddr;
                sNFLWork->stage = 0;
                sNFLWork->streams[5].state = 0;
                if (obj->texFmt == 7) {
                    sNFLWork->texAddr += 0x200;
                    sNFLWork->unk12C = 7;
                } else {
                    sNFLWork->texAddr += 0x80;
                    sNFLWork->plttAddr += 0x20;
                }
                sNFLWork->sharedTex[5].allocated = 1;
                return 0;
            }
            ctx->parts[6].texAddr = sNFLWork->sharedTex[5].texAddr;
            ctx->parts[6].plttAddr = sNFLWork->sharedTex[5].plttAddr;
            return 0;
        }
    }
    return 0;
}

// The shared texture slots of the expressions, each given VRAM from the work area's texAddr once
// and its stream set to load: slot 0, the eyes of expression 5
int NFLi_AllocExpression5Texture(NFLFace *obj, u32 *address) {
    NFLWork *work = sNFLWork;

    if (work->sharedTex[0].allocated == 0) {
        work->sharedTex[0].texAddr = *address;
        if (obj->texFmt == 7) {
            *address += 0x800;
        } else {
            *address += 0x200;
            work->sharedTex[0].plttAddr = obj->eyePlttAddr;
        }
        sNFLWork->streams[0].state = 0;
        work->sharedTex[0].allocated = 1;
    }
    return 0;
}

// Slot 1, the mouth of expression 6
int NFLi_AllocExpression6Texture(NFLFace *obj, u32 *address) {
    NFLWork *work = sNFLWork;

    if (work->sharedTex[1].allocated == 0) {
        work->sharedTex[1].texAddr = *address;
        if (obj->texFmt == 7) {
            *address += 0x800;
        } else {
            *address += 0x200;
            work->sharedTex[1].plttAddr = obj->mouthPlttAddr;
        }
        sNFLWork->streams[1].state = 0;
        work->sharedTex[1].allocated = 1;
    }
    return 0;
}

// Slot 2, the eyes of expression 1
int NFLi_AllocExpression1Texture(NFLFace *obj, u32 *address) {
    NFLWork *work = sNFLWork;

    if (work->sharedTex[2].allocated == 0) {
        work->sharedTex[2].texAddr = *address;
        if (obj->texFmt == 7) {
            *address += 0x800;
        } else {
            *address += 0x200;
            work->sharedTex[2].plttAddr = obj->eyePlttAddr;
        }
        sNFLWork->streams[2].state = 0;
        work->sharedTex[2].allocated = 1;
    }
    return 0;
}

// Slot 3, the mouth of expression 2
int NFLi_AllocExpression2Texture(NFLFace *obj, u32 *address) {
    NFLWork *work = sNFLWork;

    if (work->sharedTex[3].allocated == 0) {
        work->sharedTex[3].texAddr = *address;
        if (obj->texFmt == 7) {
            *address += 0x800;
        } else {
            *address += 0x200;
            work->sharedTex[3].plttAddr = obj->eyePlttAddr;
        }
        sNFLWork->streams[3].state = 0;
        work->sharedTex[3].allocated = 1;
    }
    return 0;
}

// Slot 4, the mouth of expression 3
int NFLi_AllocExpression3Texture(NFLFace *obj, u32 *address) {
    NFLWork *work = sNFLWork;

    if (work->sharedTex[4].allocated == 0) {
        work->sharedTex[4].texAddr = *address;
        if (obj->texFmt == 7) {
            *address += 0x800;
        } else {
            *address += 0x200;
            work->sharedTex[4].plttAddr = obj->eyePlttAddr;
        }
        sNFLWork->streams[4].state = 0;
        work->sharedTex[4].allocated = 1;
    }
    return 0;
}

// Expression 4 has its eyes in the face's own VRAM: texture stream 5 loads eye type 0x31 (in mode
// 7 again when the eye colour changed)
int NFLi_PrepareExpression4Texture(NFLFace *obj, u32 *address) {
    s16 *params = obj->keptFields;
    const NFLCharData *mii = obj->mii;

    if (obj->exprFlags & 0x10) {
        if (obj->texStreams[5].value != 0x31 || (obj->texFmt == 7 && params[8] != mii->eyeColor) ||
            (obj->texStreams[5].state >= 1 && obj->texStreams[5].state <= 6)) {
            obj->texStreams[5].state = 0;
            obj->texStreams[5].value = 0x31;
        }
        if (obj->texFmt == 7) {
            obj->expr4EyeTexAddr = obj->texBase + 0x2080;
        } else {
            obj->expr4EyeTexAddr = obj->texBase + 0x810;
            obj->expr4EyePlttAddr = obj->eyePlttAddr;
        }
    }
    return 0;
}

// The textures of the expressions the face may take (bit n of exprFlags for kind n)
int NFLi_AllocExpressionTextures(NFLMakeCtx *ctx) {
    NFLFace *obj = ctx->face;
    int result;

    if (obj->exprFlags & 0x20) {
        result = NFLi_AllocExpression5Texture(obj, &sNFLWork->texAddr);
        if (result != 0) {
            return result;
        }
    }
    if (obj->exprFlags & 0x40) {
        result = NFLi_AllocExpression6Texture(obj, &sNFLWork->texAddr);
        if (result != 0) {
            return result;
        }
    }
    if (obj->exprFlags & 0x2) {
        result = NFLi_AllocExpression1Texture(obj, &sNFLWork->texAddr);
        if (result != 0) {
            return result;
        }
    }
    if (obj->exprFlags & 0x4) {
        result = NFLi_AllocExpression2Texture(obj, &sNFLWork->texAddr);
        if (result != 0) {
            return result;
        }
    }
    if (obj->exprFlags & 0x8) {
        result = NFLi_AllocExpression3Texture(obj, &sNFLWork->texAddr);
        if (result != 0) {
            return result;
        }
    }
    return NFLi_PrepareExpression4Texture(obj, &ctx->texBase);
}


extern const u8 sNFLEyebrowTypeRotations[];
extern const u8 sNFLEyeTypeRotations[];

// The places, sizes and rotations of the seven parts (NFLMakeCtx 0x78), from the Mii's data
void NFLi_PlaceFaceParts(NFLMakeCtx *ctx) {
    const NFLCharData *mii = ctx->mii;
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

    ctx->parts[0].place[0] = eyeX;
    ctx->parts[0].place[1] = eyeY;
    ctx->parts[0].place[2] = eyeScale;
    ctx->parts[0].place[3] = -eyeScale;
    ctx->parts[0].place[4] = eyeHeight;
    ctx->parts[0].place[5] = -eyeHeight;
    ctx->parts[0].place[6] = -(eyeRotation * 0x10000 / 32);
    ctx->parts[0].place[7] = 4;
    ctx->parts[1].place[0] = eyeX;
    ctx->parts[1].place[1] = eyeY;
    ctx->parts[1].place[2] = eyeScale;
    ctx->parts[1].place[3] = -eyeScale;
    ctx->parts[1].place[4] = eyeHeight;
    ctx->parts[1].place[5] = -eyeHeight;
    ctx->parts[1].place[6] = -(eyeRotation * 0x10000 / 32);
    ctx->parts[1].place[7] = 3;
    ctx->parts[2].place[0] = eyebrowX;
    ctx->parts[2].place[1] = eyebrowY;
    ctx->parts[2].place[2] = eyebrowScale;
    ctx->parts[2].place[3] = -eyebrowScale;
    ctx->parts[2].place[4] = eyebrowHeight;
    ctx->parts[2].place[5] = -eyebrowHeight;
    ctx->parts[2].place[6] = -(eyebrowRotation << 11);
    ctx->parts[2].place[7] = 4;
    ctx->parts[3].place[0] = eyebrowX;
    ctx->parts[3].place[1] = eyebrowY;
    ctx->parts[3].place[2] = eyebrowScale;
    ctx->parts[3].place[3] = -eyebrowScale;
    ctx->parts[3].place[4] = eyebrowHeight;
    ctx->parts[3].place[5] = -eyebrowHeight;
    ctx->parts[3].place[6] = -(eyebrowRotation << 11);
    ctx->parts[3].place[7] = 3;
    ctx->parts[4].place[0] = 0;
    ctx->parts[4].place[1] = mouthY;
    ctx->parts[4].place[2] = FX_Mul(mouthScale, 0x5180);
    ctx->parts[4].place[3] = -FX_Mul(mouthScale, 0x28C0);
    ctx->parts[4].place[4] = 0xDD94;
    ctx->parts[4].place[5] = -0x6ECA;
    ctx->parts[4].place[6] = 0;
    ctx->parts[4].place[7] = 1;
    ctx->parts[5].place[0] = 0;
    ctx->parts[5].place[1] = beardY;
    ctx->parts[5].place[2] = FX_Mul(beardScale, 0x7480);
    ctx->parts[5].place[3] = -FX_Mul(beardScale, 0x7480);
    ctx->parts[5].place[4] = beardHeight;
    ctx->parts[5].place[5] = -beardHeight;
    ctx->parts[5].place[6] = 0;
    ctx->parts[5].place[7] = 2;
    ctx->parts[6].place[0] = moleX;
    ctx->parts[6].place[1] = moleY;
    ctx->parts[6].place[2] = FX_Mul(moleScale, 0x1060);
    ctx->parts[6].place[3] = -FX_Mul(moleScale, 0x1060);
    ctx->parts[6].place[4] = moleHeight;
    ctx->parts[6].place[5] = -moleHeight;
    ctx->parts[6].place[6] = 0;
    ctx->parts[6].place[7] = 0;
}

// Four planes, z = -(x * a + y * b + d) / c, by the quadrant a point is in (NFLi_GetPartCorner)
typedef struct NFLPlane {
    fx32 a;
    fx32 b;
    fx32 c;
    fx32 d;
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
    out->z = -FX_Div(sNFLFacePlanes[j][i].d + (FX_Mul(sNFLFacePlanes[j][i].a, out->x) +
                                                  FX_Mul(sNFLFacePlanes[j][i].b, out->y)),
                     sNFLFacePlanes[j][i].c) *
             0x47 / 200;
}

// The state that NFLi_MakeMouthMolePart and NFLi_MakeEyeBrowPart share across calls (NFLi_MakeFacePartsDL clears set
// between pairs)
typedef struct NFLMakeState {
    fx32 a; // A plane, as NFLPlane's, that the first of a pair sets
    fx32 b;
    fx32 c;
    fx32 d;
    VecFx32 center; // The first's centre and size, which the second compares its own with
    fx32 size;
    int set;
} NFLMakeState;

void NFLi_MakeMouthMolePart(GXDLInfo *dl, NFLMakePart *part, NFLFace *obj, const NFLCharData *data, NFLPatch *patch,
                   NFLDLRef *ref, int n, NFLMakeState *state);
void NFLi_MakeEyeBrowPart(GXDLInfo *dl, NFLMakePart *part, NFLFace *obj, const NFLCharData *data, NFLPatch *patch,
                   NFLDLRef *ref, int n, NFLMakeState *state);

void NFLi_MakeRotZMtx(const NFLDrawContext *ctx, MtxFx43 *mtx, fx32 sinVal, fx32 cosVal);
void NFLi_DLTranslate(const NFLDrawContext *ctx, GXDLInfo *info, fx32 x, fx32 y, fx32 z);
void NFLi_DLScale(const NFLDrawContext *ctx, GXDLInfo *info, fx32 x, fx32 y, fx32 z);
void NFLi_DLVtx(const NFLDrawContext *ctx, GXDLInfo *info, fx16 x, fx16 y, fx16 z);
void NFLi_DLNormal(const NFLDrawContext *ctx, GXDLInfo *info, fx16 x, fx16 y, fx16 z);
// The absolute value of v
static inline fx32 NFLi_Abs(fx32 v) {
    return v < 0 ? -v : v;
}

void NFLi_MapVecFx32NoNegate(const NFLDrawContext *ctx, VecFx32 *out, const VecFx32 *in);
void NFLi_MapVecFx32Copy(const NFLDrawContext *ctx, VecFx32 *out, const VecFx32 *in);

extern const VecFx32 sNFLFaceGridPoints[3][3];
extern const VecFx32 sNFLFaceGridNormals[3][3];

// Parts 4 to 6 of NFLi_MakeFacePartsDL, the mouth (kind 1), the mustache (kind 2) and the mole (kind 0): placed as
// NFLi_MakeEyeBrowPart places its quads, but looked up at |x| (the face is symmetric), and drawn with the vertices
// transformed here (MTX_MultVec43, the normals by the rotation's 3x3 part) instead of by matrix commands, so the
// expressions have no rotation to patch (patch->mtx is cleared). The mouth is a strip of two quads whose texture runs
// from the right edge to the left, the mustache one whose texture is mirrored about its middle, the mole one quad.
void NFLi_MakeMouthMolePart(GXDLInfo *dl, NFLMakePart *part, NFLFace *obj, const NFLCharData *data, NFLPatch *patch,
                   NFLDLRef *ref, int n, NFLMakeState *state) {
    MtxFx43 mtx;
    MtxFx43 rot;
    MtxFx33 nrmMtx;
    VecFx32 grid[2][2];
    VecFx32 normals[2][2];
    VecFx32 axis;
    VecFx32 center;
    VecFx32 normal;
    VecFx32 up;
    VecFx32 point;
    VecFx32 point2;
    VecFx32 sum;
    VecFx32 trans;
    VecFx32 transOut;
    VecFx32 axisOut;
    VecFx32 scale;
    VecFx32 scaleOut;
    VecFx32 vtx;
    VecFx32 nrm;
    VecFx32 vtxOut;
    VecFx32 nrmOut;
    fx32 x;
    fx32 y;
    fx32 w;
    fx32 h;
    u32 kind;
    int fmt;
    int texW;
    int texH;
    int i;
    int j;
    fx32 zSum;
    int faceType;
    fx16 cos;
    fx16 sin;
    int scaleY;
    int scaleX;
    fx32 cross;
    fx32 dot;
    int a;
    int b;
    int k;
    fx32 size;
    fx32 u;
    fx32 v;
    fx32 t;
    fx32 s;
    fx32 rotation;
    fx32 cx;
    fx32 px;
    int pi;
    int pj;
    fx32 top;
    fx32 bottom;

    texW = 1;
    texH = 1;
    x = part->place[0];
    y = part->place[1];
    scaleX = part->place[2];
    scaleY = part->place[3];
    w = part->place[4];
    h = part->place[5];
    rotation = part->place[6];
    kind = part->place[7];
    fmt = obj->texFmt;
    G3C_PushMtx(dl);
    sin = (fx16)FX_CosIdx((u16)rotation);
    cos = (fx16)FX_SinIdx((u16)rotation);
    faceType = data->faceType;
    zSum = 0;
    cx = x * 5 / 64;
    center.y = y * 5 / 64;
    if (cx < 0) {
        cx = -cx;
    }
    center.x = cx;
    pi = cx >= 0x1B33 ? 1 : 0;
    pj = center.y <= 0x167B ? 1 : 0;
    center.z = -FX_Div(sNFLFacePlanes[pj][pi].d + (FX_Mul(sNFLFacePlanes[pj][pi].a, cx) + FX_Mul(sNFLFacePlanes[pj][pi].b, center.y)),
                       sNFLFacePlanes[pj][pi].c) * 0x47 / 200;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            grid[i][j].x = center.x + w * (i * 2 - 1) * 5 / 128;
            grid[i][j].y = center.y + h * (j * 2 - 1) * 5 / 128;
            px = NFLi_Abs(grid[i][j].x);
            pi = px >= 0x1B33 ? 1 : 0;
            pj = grid[i][j].y <= 0x167B ? 1 : 0;
            grid[i][j].z = -FX_Div(sNFLFacePlanes[pj][pi].d + (FX_Mul(sNFLFacePlanes[pj][pi].a, px) +
                                                                  FX_Mul(sNFLFacePlanes[pj][pi].b, grid[i][j].y)),
                                   sNFLFacePlanes[pj][pi].c) *
                           0x47 / 200;
            u = FX_Div(px - sNFLFaceGridPoints[pj][pi].x, sNFLFaceGridPoints[pj][pi + 1].x - sNFLFaceGridPoints[pj][pi].x);
            v = FX_Div(grid[i][j].y - sNFLFaceGridPoints[pj][pi].y,
                       sNFLFaceGridPoints[pj + 1][pi].y - sNFLFaceGridPoints[pj][pi].y);
            top = sNFLFaceGridNormals[pj][pi].x +
               FX_Mul(sNFLFaceGridNormals[pj][pi + 1].x - sNFLFaceGridNormals[pj][pi].x, u);
            bottom = sNFLFaceGridNormals[pj + 1][pi].x +
                  FX_Mul(sNFLFaceGridNormals[pj + 1][pi + 1].x - sNFLFaceGridNormals[pj + 1][pi].x, u);
            normals[i][j].x = top + FX_Mul(bottom - top, v);
            if (grid[i][j].x < 0) {
                normals[i][j].x = -normals[i][j].x;
            }
            top = sNFLFaceGridNormals[pj][pi].y +
               FX_Mul(sNFLFaceGridNormals[pj][pi + 1].y - sNFLFaceGridNormals[pj][pi].y, u);
            bottom = sNFLFaceGridNormals[pj + 1][pi].y +
                  FX_Mul(sNFLFaceGridNormals[pj + 1][pi + 1].y - sNFLFaceGridNormals[pj + 1][pi].y, u);
            normals[i][j].y = top + FX_Mul(bottom - top, v);
            top = sNFLFaceGridNormals[pj][pi].z +
               FX_Mul(sNFLFaceGridNormals[pj][pi + 1].z - sNFLFaceGridNormals[pj][pi].z, u);
            bottom = sNFLFaceGridNormals[pj + 1][pi].z +
                  FX_Mul(sNFLFaceGridNormals[pj + 1][pi + 1].z - sNFLFaceGridNormals[pj + 1][pi].z, u);
            normals[i][j].z = top + FX_Mul(bottom - top, v);
            VEC_Normalize(&normals[i][j], &normals[i][j]);
        }
    }
    normal.x = 0;
    normal.y = 0;
    normal.z = 0;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            normal.x += normals[i][j].x;
            normal.y += normals[i][j].y;
            normal.z += normals[i][j].z;
            zSum += grid[i][j].z;
        }
    }
    if (kind == 0) {
        center.z = (center.z + zSum / 4) / 2;
        if (x < 0) {
            normal.x = -normal.x;
        }
    }
    VEC_Normalize(&normal, &normal);
    if (state != NULL && state->set == 0) {
        state->center.x = 0;
        state->center.y = 0;
        state->center.z = 0;
        for (a = 0; a < 2; a++) {
            for (b = 0; b < 2; b++) {
                NFLi_GetPartCorner(&point, &center, kind, a, b, scaleX, scaleY, sin, cos);
                state->center.x += point.x;
                state->center.y += point.y;
            }
        }
        state->center.x /= 4;
        state->center.y /= 4;
        state->center.z = 0;
        state->size = (scaleX > scaleY ? scaleX : scaleY) * 5 / 128;
        state->a = normal.x < 0 ? -normal.x : normal.x;
        state->b = normal.y;
        state->c = normal.z * 200 / 0x47;
        state->d = -(FX_Mul(normal.z * 200 / 0x47, center.z) +
                     (FX_Mul(normal.x < 0 ? -normal.x : normal.x, NFLi_Abs(x / 32)) +
                      FX_Mul(normal.y, y / 32)));
    }
    if (state != NULL) {
        if (state->set == 1) {
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
            if (scaleX > scaleY) {
                size = scaleX * 5 / 128;
            } else {
                size = scaleY * 5 / 128;
            }
            if (VEC_Distance(&sum, &state->center) <= (size + state->size) / 3) {
                center.z = -FX_Div(state->d + (FX_Mul(state->a, NFLi_Abs(x / 32)) +
                                                  FX_Mul(state->b, y / 32)),
                                   state->c);
            } else {
                n = 0;
            }
        } else {
            state->set = 1;
        }
    }
    up.x = 0;
    up.y = 0;
    up.z = FX32_ONE;
    dot = VEC_DotProduct(&normal, &up);
    cross = FX_Sqrt(FX32_ONE - FX_Mul(dot, dot));
    VEC_CrossProduct(&up, &normal, &axis);
    if (faceType == 2 || faceType == 6 || faceType == 7) {
        y += 0x1E60;
    }
    if (ref != NULL) {
        ref->cmd = dl->curr_cmd;
        ref->param = dl->curr_param;
    }
    trans.x = x / 32;
    trans.y = y / 32;
    trans.z = obj->partDepth + (center.z + n);
    NFLi_MapVecFx32(obj, &transOut, &trans);
    MTX_Identity43(&mtx);
    MTX_TransApply43(&mtx, &mtx, transOut.x, transOut.y, transOut.z);
    NFLi_MapVecFx32(obj, &axisOut, &axis);
    axis.x = axisOut.x;
    axis.y = axisOut.y;
    axis.z = axisOut.z;
    MTX_RotAxis43(&rot, &axis, cross, dot);
    MTX_Copy43To33(&rot, &nrmMtx);
    MTX_Concat43(&rot, &mtx, &mtx);
    if (patch != NULL) {
        patch->mtx.cmd = NULL;
        patch->mtx.param = NULL;
    }
    scale.x = scaleX / 32;
    scale.y = scaleY / 32;
    scale.z = FX32_ONE;
    NFLi_MapVecFx32NoNegate(obj, &scaleOut, &scale);
    MTX_ScaleApply43(&mtx, &mtx, scaleOut.x, scaleOut.y, scaleOut.z);
    if (fmt == GX_TEXFMT_DIRECT) {
        if (patch != NULL) {
            patch->tex.cmd = dl->curr_cmd;
            patch->tex.param = dl->curr_param;
        }
        G3C_TexImageParam(dl, GX_TEXFMT_DIRECT, GX_TEXGEN_NONE, part->texSizeS, part->texSizeT, GX_TEXREPEAT_ST,
                          GX_TEXFLIP_ST, GX_TEXPLTTCOLOR0_TRNS, part->texAddr);
    } else {
        if (patch != NULL) {
            patch->tex.cmd = dl->curr_cmd;
            patch->tex.param = dl->curr_param;
        }
        G3C_TexImageParam(dl, GX_TEXFMT_PLTT16, GX_TEXGEN_NONE, part->texSizeS, part->texSizeT, GX_TEXREPEAT_ST,
                          GX_TEXFLIP_ST, GX_TEXPLTTCOLOR0_TRNS, part->texAddr);
        if (patch != NULL) {
            patch->pltt.cmd = dl->curr_cmd;
            patch->pltt.param = dl->curr_param;
        }
        G3C_TexPlttBase(dl, part->plttAddr, GX_TEXFMT_PLTT16);
    }
    switch (part->texSizeS) {
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
    switch (part->texSizeT) {
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
#define NFL_NORMAL(nx, ny, nz)                                                                                       \
    nrm.x = (nx);                                                                                                    \
    nrm.y = (ny);                                                                                                    \
    nrm.z = (nz);                                                                                                    \
    NFLi_MapVecFx32Copy(obj, &nrmOut, &nrm);                                                                         \
    MTX_MultVec33(&nrmOut, &nrmMtx, &nrm);                                                                           \
    G3C_Normal(dl, (fx16)nrm.x, (fx16)nrm.y, (fx16)nrm.z)
#define NFL_VTX(vx, vy, vz)                                                                                          \
    vtx.x = (vx);                                                                                                    \
    vtx.y = (vy);                                                                                                    \
    vtx.z = (vz);                                                                                                    \
    NFLi_MapVecFx32Copy(obj, &vtxOut, &vtx);                                                                         \
    MTX_MultVec43(&vtxOut, &mtx, &vtx);                                                                              \
    G3C_Vtx(dl, (fx16)vtx.x, (fx16)vtx.y, (fx16)vtx.z)
    s = texW << 12;
    if (kind == 1) {
        func_02005124(dl, GX_BEGIN_QUAD_STRIP);
        G3C_TexCoord(dl, s, 0);
        NFL_NORMAL(0x197, 0, 0xFEA);
        NFL_VTX(0x800, -0x800, -0xCC);
        t = texH << 12;
        G3C_TexCoord(dl, s, t);
        NFL_VTX(0x800, 0x800, -0xCC);
        G3C_TexCoord(dl, s / 2, 0);
        NFL_NORMAL(0, 0, 0xFFF);
        NFL_VTX(0, -0x800, 0);
        G3C_TexCoord(dl, s / 2, t);
        NFL_VTX(0, 0x800, 0);
        G3C_TexCoord(dl, 0, 0);
        NFL_NORMAL(-0x197, 0, 0xFEA);
        NFL_VTX(-0x800, -0x800, -0xCC);
        G3C_TexCoord(dl, 0, t);
        NFL_VTX(-0x800, 0x800, -0xCC);
    } else if (kind == 2) {
        func_02005124(dl, GX_BEGIN_QUAD_STRIP);
        G3C_TexCoord(dl, 0, 0);
        NFL_NORMAL(0x197, 0, 0xFEA);
        NFL_VTX(0x800, -0x800, -0xCC);
        t = texH << 12;
        G3C_TexCoord(dl, 0, t);
        NFL_VTX(0x800, 0x800, -0xCC);
        G3C_TexCoord(dl, s, 0);
        NFL_NORMAL(0, 0, 0xFFF);
        NFL_VTX(0, -0x800, 0);
        G3C_TexCoord(dl, s, t);
        NFL_VTX(0, 0x800, 0);
        G3C_TexCoord(dl, 0, 0);
        NFL_NORMAL(0, -0x197, 0xFEA);
        NFL_VTX(-0x800, -0x800, -0xCC);
        G3C_TexCoord(dl, 0, t);
        NFL_VTX(-0x800, 0x800, -0xCC);
    } else {
        func_02005124(dl, GX_BEGIN_QUADS);
        G3C_TexCoord(dl, 0, 0);
        NFL_NORMAL(0, 0, 0xFFF);
        NFL_VTX(-0x800, -0x800, 0);
        G3C_TexCoord(dl, s, 0);
        NFL_VTX(0x800, -0x800, 0);
        t = texH << 12;
        G3C_TexCoord(dl, s, t);
        NFL_VTX(0x800, 0x800, 0);
        G3C_TexCoord(dl, 0, t);
        NFL_VTX(-0x800, 0x800, 0);
    }
#undef NFL_NORMAL
#undef NFL_VTX
    func_02005148(dl);
    G3C_PopMtx(dl, 1);
}

// Parts 0 to 3 of NFLi_MakeFacePartsDL, the eyes and eyebrows: a part placed as a quad on the face, its 2x2 points'
// Z from the planes of sNFLFacePlanes, their normals interpolated over the 3x3 grids of points (sNFLFaceGridPoints)
// and normals (sNFLFaceGridNormals), the quad turned onto their sum by matrix commands that the expressions patch.
// Kind 4 is the mirrored side: its X is negated and its texture runs the other way.
void NFLi_MakeEyeBrowPart(GXDLInfo *dl, NFLMakePart *part, NFLFace *obj, const NFLCharData *data, NFLPatch *patch,
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
    fx32 x0;
    fx32 s1;
    fx32 s0;
    fx32 x;
    fx32 y;
    fx32 w;
    fx32 h;
    u32 kind;
    int fmt;
    int texW;
    int texH;
    int i;
    int j;
    int faceType;
    int scaleX;
    int scaleY;
    fx16 sin;
    fx16 cos;
    fx32 cross;
    fx32 dot;
    int k;
    int a;
    int b;
    fx32 size;
    fx32 u;
    fx32 v;
    fx32 rotation;
    fx32 cx;
    int pi;
    int pj;
    fx32 top;
    fx32 bottom;
    fx32 px;

    texW = 1;
    texH = 1;
    x = part->place[0];
    y = part->place[1];
    scaleX = part->place[2];
    scaleY = part->place[3];
    w = part->place[4];
    h = part->place[5];
    rotation = part->place[6];
    kind = part->place[7];
    fmt = obj->texFmt;
    G3C_PushMtx(dl);
    sin = (fx16)FX_CosIdx((u16)rotation);
    cos = (fx16)FX_SinIdx((u16)rotation);
    faceType = data->faceType;
    cx = x * 5 / 64;
    center.y = y * 5 / 64;
    if (cx < 0) {
        cx = -cx;
    }
    center.x = cx;
    pi = cx >= 0x1B33 ? 1 : 0;
    pj = center.y <= 0x167B ? 1 : 0;
    center.z = -FX_Div(sNFLFacePlanes[pj][pi].d + (FX_Mul(sNFLFacePlanes[pj][pi].a, cx) + FX_Mul(sNFLFacePlanes[pj][pi].b, center.y)),
                       sNFLFacePlanes[pj][pi].c) * 0x47 / 200;
    for (i = 0; i < 2; i++) {
        for (j = 0; j < 2; j++) {
            grid[i][j].x = center.x + FX_Mul(sin, i * w * 5 / 64) + FX_Mul(cos, h * (j * 2 - 1) * 5 / 128);
            grid[i][j].y = center.y - FX_Mul(cos, i * w * 5 / 64) + FX_Mul(sin, h * (j * 2 - 1) * 5 / 128);
            px = grid[i][j].x;
            pi = px >= 0x1B33 ? 1 : 0;
            pj = grid[i][j].y <= 0x167B ? 1 : 0;
            grid[i][j].z = -FX_Div(sNFLFacePlanes[pj][pi].d + (FX_Mul(sNFLFacePlanes[pj][pi].a, px) +
                                                                  FX_Mul(sNFLFacePlanes[pj][pi].b, grid[i][j].y)),
                                   sNFLFacePlanes[pj][pi].c) *
                           0x47 / 200;
            u = FX_Div(px - sNFLFaceGridPoints[pj][pi].x, sNFLFaceGridPoints[pj][pi + 1].x - sNFLFaceGridPoints[pj][pi].x);
            v = FX_Div(grid[i][j].y - sNFLFaceGridPoints[pj][pi].y, sNFLFaceGridPoints[pj + 1][pi].y - sNFLFaceGridPoints[pj][pi].y);
            top = sNFLFaceGridNormals[pj][pi].x +
               FX_Mul(sNFLFaceGridNormals[pj][pi + 1].x - sNFLFaceGridNormals[pj][pi].x, u);
            bottom = sNFLFaceGridNormals[pj + 1][pi].x +
                  FX_Mul(sNFLFaceGridNormals[pj + 1][pi + 1].x - sNFLFaceGridNormals[pj + 1][pi].x, u);
            normals[i][j].x = top + FX_Mul(bottom - top, v);
            top = sNFLFaceGridNormals[pj][pi].y +
               FX_Mul(sNFLFaceGridNormals[pj][pi + 1].y - sNFLFaceGridNormals[pj][pi].y, u);
            bottom = sNFLFaceGridNormals[pj + 1][pi].y +
                  FX_Mul(sNFLFaceGridNormals[pj + 1][pi + 1].y - sNFLFaceGridNormals[pj + 1][pi].y, u);
            normals[i][j].y = top + FX_Mul(bottom - top, v);
            top = sNFLFaceGridNormals[pj][pi].z +
               FX_Mul(sNFLFaceGridNormals[pj][pi + 1].z - sNFLFaceGridNormals[pj][pi].z, u);
            bottom = sNFLFaceGridNormals[pj + 1][pi].z +
                  FX_Mul(sNFLFaceGridNormals[pj + 1][pi + 1].z - sNFLFaceGridNormals[pj + 1][pi].z, u);
            normals[i][j].z = top + FX_Mul(bottom - top, v);
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
    if (state != NULL && state->set == 0) {
        state->center.x = 0;
        state->center.y = 0;
        state->center.z = 0;
        for (a = 0; a < 2; a++) {
            for (b = 0; b < 2; b++) {
                NFLi_GetPartCorner(&point, &center, kind, a, b, scaleX, scaleY, sin, cos);
                state->center.x += point.x;
                state->center.y += point.y;
            }
        }
        state->center.x /= 4;
        state->center.y /= 4;
        state->center.z = 0;
        state->size = (scaleX > scaleY ? scaleX : scaleY) * 5 / 128;
        state->a = normal.x < 0 ? -normal.x : normal.x;
        state->b = normal.y;
        state->c = normal.z * 200 / 0x47;
        state->d = -(FX_Mul(normal.z * 200 / 0x47, center.z) +
                     (FX_Mul(normal.x < 0 ? -normal.x : normal.x, NFLi_Abs(x / 32)) +
                      FX_Mul(normal.y, y / 32)));
    }
    if (state != NULL) {
        if (state->set == 1) {
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
            if (scaleX > scaleY) {
                size = scaleX * 5 / 128;
            } else {
                size = scaleY * 5 / 128;
            }
            if (VEC_Distance(&sum, &state->center) <= (size + state->size) * 3 / 5) {
                center.z = -FX_Div(state->d + (FX_Mul(state->a, NFLi_Abs(x / 32)) +
                                                  FX_Mul(state->b, y / 32)),
                                   state->c);
            } else {
                n = 0;
            }
        } else {
            state->set = 1;
        }
    }
    up.x = 0;
    up.y = 0;
    up.z = FX32_ONE;
    dot = VEC_DotProduct(&normal, &up);
    cross = FX_Sqrt(FX32_ONE - FX_Mul(dot, dot));
    VEC_CrossProduct(&up, &normal, &axis);
    if (faceType == 2 || faceType == 6 || faceType == 7) {
        y += 0x1E60;
    }
    if (ref != NULL) {
        ref->cmd = dl->curr_cmd;
        ref->param = dl->curr_param;
    }
    if (kind == 4) {
        NFLi_DLTranslate(obj, dl, -x / 32, y / 32, obj->partDepth + (center.z + n));
    } else {
        NFLi_DLTranslate(obj, dl, x / 32, y / 32, obj->partDepth + (center.z + n));
    }
    NFLi_MapVecFx32(obj, &rotated, &axis);
    axis.x = rotated.x;
    axis.y = rotated.y;
    axis.z = rotated.z;
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
        G3C_TexImageParam(dl, GX_TEXFMT_DIRECT, GX_TEXGEN_NONE, part->texSizeS, part->texSizeT, GX_TEXREPEAT_ST,
                          GX_TEXFLIP_ST, GX_TEXPLTTCOLOR0_TRNS, part->texAddr);
    } else {
        patch->tex.cmd = dl->curr_cmd;
        patch->tex.param = dl->curr_param;
        G3C_TexImageParam(dl, GX_TEXFMT_PLTT16, GX_TEXGEN_NONE, part->texSizeS, part->texSizeT, GX_TEXREPEAT_ST,
                          GX_TEXFLIP_ST, GX_TEXPLTTCOLOR0_TRNS, part->texAddr);
        patch->pltt.cmd = dl->curr_cmd;
        patch->pltt.param = dl->curr_param;
        G3C_TexPlttBase(dl, part->plttAddr, GX_TEXFMT_PLTT16);
    }
    switch (part->texSizeS) {
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
    switch (part->texSizeT) {
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
    switch (kind) {
    case 4:
        x0 = -FX32_ONE;
        s1 = texW << 12;
        s0 = 0;
        break;
    case 3:
        x0 = 0;
        s1 = 0;
        s0 = texW << 12;
        break;
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

// The second display list: the face parts, as quads on the face, in pairs that share a state: the
// mouth and mustache, one side's eye and eyebrow, the other's, then the mole if the Mii has one
void NFLi_MakeFacePartsDL(NFLMakeCtx *ctx) {
    NFLFace *obj = ctx->face;
    const NFLCharData *data = ctx->mii;
    NFLMakeState state;

    state.set = 0;
    NFLi_MakeMouthMolePart(&ctx->dl, &ctx->parts[4], obj, data, &obj->mouthPatch, NULL, 0, &state);
    NFLi_MakeMouthMolePart(&ctx->dl, &ctx->parts[5], obj, data, NULL, NULL, 0x20, &state);
    state.set = 0;
    NFLi_MakeEyeBrowPart(&ctx->dl, &ctx->parts[0], obj, data, &obj->eyeMirrorPatch, NULL, 0, &state);
    NFLi_MakeEyeBrowPart(&ctx->dl, &ctx->parts[2], obj, data, &obj->browMirrorPatch, &obj->browMirrorTrans, 0x20,
                         &state);
    state.set = 0;
    NFLi_MakeEyeBrowPart(&ctx->dl, &ctx->parts[1], obj, data, &obj->eyePatch, NULL, 0, &state);
    NFLi_MakeEyeBrowPart(&ctx->dl, &ctx->parts[3], obj, data, &obj->browPatch, &obj->browTrans, 0x20, &state);
    if (data->moleType == 1) {
        NFLi_MakeMouthMolePart(&ctx->dl, &ctx->parts[6], obj, data, NULL, NULL, 0, NULL);
    }
}


// Patches a face part's display list: its rotation, if it has a matrix command, and its texture and palette
void NFLi_PatchFacePart(NFLFace *obj, NFLPatch *patch, fx16 sin, fx16 cos, GXTexFmt fmt, GXTexSizeS s, GXTexSizeT t,
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

// The expressions, which NFL_SetExpression applies by rewriting the face parts' commands: kind 0,
// the Mii's own eyes, eyebrows and mouth
#pragma optimization_level 2
#pragma opt_common_subs off
void NFLi_ApplyExpression0(NFLFace *obj) {
    fx16 sin;
    fx16 cos;
    fx32 y;
    int type;
    u16 idx;
    fx32 v;

    idx = (u16)(((obj->exprFields[3] + sNFLEyeTypeRotations[obj->exprFields[4]] - 0x20) << 16) / 32);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    y = -(obj->exprFields[2] * 0x10E1 - 0x22490);
    type = obj->exprFields[5];
    NFLi_PatchFacePart(obj, &obj->eyePatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->eyeTexAddr, obj->eyePlttAddr);
    NFLi_PatchFacePart(obj, &obj->eyeMirrorPatch, -sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->eyeTexAddr, obj->eyePlttAddr);
    idx = (u16)((obj->exprFields[6] + sNFLEyebrowTypeRotations[obj->exprFields[7]] - 0x20) << 11);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->browPatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    NFLi_PatchFacePart(obj, &obj->browMirrorPatch, -sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    switch (type) {
    case 2:
    case 6:
    case 7:
        y += 0x1E60;
        break;
    }
    if (obj->negate.x != 0) {
        v = -y / 32;
    } else {
        v = y / 32;
    }
    obj->browTrans.param[obj->yAxis] = v;
    obj->browMirrorTrans.param[obj->yAxis] = v;
    sin = (fx16)FX_SinIdx(0);
    cos = (fx16)FX_CosIdx(0);
    NFLi_PatchFacePart(obj, &obj->mouthPatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->mouthTexAddr, obj->mouthPlttAddr);
}
#pragma opt_common_subs reset
#pragma optimization_level reset

// Kind 5: the eyes of shared slot 0 (eye type 26's texture), turned as the Mii's own: the rotation moved by
// sNFLEyeTypeRotations as in kinds 1 and 4 and kept to 0..7 goes unused, and the eyes take the Mii's rotation.
// The original keeps that clamp's compares and drops only its moves, which is what opt_dead_assignments off gives
// with the clamp written on a copy (newRot) and the else adding d again.
#pragma optimization_level 2
#pragma opt_common_subs off
#pragma opt_dead_assignments off
void NFLi_ApplyExpression5(NFLFace *obj) {
    fx16 sin;
    fx16 cos;
    fx32 y;
    int type;
    u16 idx;
    fx32 v;
    int rot;
    int d;
    int newRot;

    rot = obj->exprFields[3];
    d = sNFLEyeTypeRotations[rot] - sNFLEyeTypeRotations[26];
    newRot = rot;
    idx = (u16)(((rot + sNFLEyeTypeRotations[obj->exprFields[4]] - 0x20) << 16) / 32);
    sin = (fx16)FX_SinIdx(idx);
    y = -(obj->exprFields[2] * 0x10E1 - 0x22490);
    cos = (fx16)FX_CosIdx(idx);
    type = obj->exprFields[5];
    if (rot + d < 0) {
        newRot = 0;
    } else if (rot + d > 7) {
        newRot = 7;
    } else {
        newRot += d;
    }
    NFLi_PatchFacePart(obj, &obj->eyePatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, sNFLWork->sharedTex[0].texAddr, obj->eyePlttAddr);
    NFLi_PatchFacePart(obj, &obj->eyeMirrorPatch, -sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, sNFLWork->sharedTex[0].texAddr, obj->eyePlttAddr);
    idx = (u16)((obj->exprFields[6] + sNFLEyebrowTypeRotations[obj->exprFields[7]] - 0x20) << 11);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->browPatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    NFLi_PatchFacePart(obj, &obj->browMirrorPatch, -sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    switch (type) {
    case 2:
    case 6:
    case 7:
        y += 0x1E60;
        break;
    }
    if (obj->negate.x != 0) {
        v = -y / 32;
    } else {
        v = y / 32;
    }
    obj->browTrans.param[obj->yAxis] = v;
    obj->browMirrorTrans.param[obj->yAxis] = v;
    sin = (fx16)FX_SinIdx(0);
    cos = (fx16)FX_CosIdx(0);
    NFLi_PatchFacePart(obj, &obj->mouthPatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->mouthTexAddr, obj->mouthPlttAddr);
}
#pragma opt_dead_assignments reset
#pragma opt_common_subs reset
#pragma optimization_level reset

// Kind 6: the mouth of shared slot 1
#pragma optimization_level 2
#pragma opt_common_subs off
void NFLi_ApplyExpression6(NFLFace *obj) {
    fx16 sin;
    fx16 cos;
    fx32 y;
    int type;
    u16 idx;
    fx32 v;

    idx = (u16)(((obj->exprFields[3] + sNFLEyeTypeRotations[obj->exprFields[4]] - 0x20) << 16) / 32);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    y = -(obj->exprFields[2] * 0x10E1 - 0x22490);
    type = obj->exprFields[5];
    NFLi_PatchFacePart(obj, &obj->eyePatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->eyeTexAddr, obj->eyePlttAddr);
    NFLi_PatchFacePart(obj, &obj->eyeMirrorPatch, -sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->eyeTexAddr, obj->eyePlttAddr);
    idx = (u16)((obj->exprFields[6] + sNFLEyebrowTypeRotations[obj->exprFields[7]] - 0x20) << 11);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->browPatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    NFLi_PatchFacePart(obj, &obj->browMirrorPatch, -sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    switch (type) {
    case 2:
    case 6:
    case 7:
        y += 0x1E60;
        break;
    }
    if (obj->negate.x != 0) {
        v = -y / 32;
    } else {
        v = y / 32;
    }
    obj->browTrans.param[obj->yAxis] = v;
    obj->browMirrorTrans.param[obj->yAxis] = v;
    sin = (fx16)FX_SinIdx(0);
    cos = (fx16)FX_CosIdx(0);
    NFLi_PatchFacePart(obj, &obj->mouthPatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, sNFLWork->sharedTex[1].texAddr, obj->mouthPlttAddr);
}
#pragma opt_common_subs reset
#pragma optimization_level reset

// Kind 1: the eyes of shared slot 2 (eye type 48's texture), their rotation moved by sNFLEyeTypeRotations at the
// rotation less at 48 and kept to 0..7.
// Not matching yet: the code is the original's, but the clamped rotation is kept in r3 where the original reuses r4.
#pragma optimization_level 2
#pragma opt_common_subs off
#pragma opt_propagation off
void NFLi_ApplyExpression1(NFLFace *obj) {
    fx16 sin;
    fx16 cos;
    fx32 y;
    int type;
    u16 idx;
    fx32 v;
    int rot;

    rot = obj->exprFields[3] + (sNFLEyeTypeRotations[obj->exprFields[3]] - sNFLEyeTypeRotations[48]);
    y = -(obj->exprFields[2] * 0x10E1 - 0x22490);
    type = obj->exprFields[5];
    if (rot < 0) {
        rot = 0;
    } else if (rot > 7) {
        rot = 7;
    }
    idx = (u16)(((rot + sNFLEyeTypeRotations[obj->exprFields[4]] - 0x20) << 16) / 32);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->eyePatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, sNFLWork->sharedTex[2].texAddr, obj->eyePlttAddr);
    NFLi_PatchFacePart(obj, &obj->eyeMirrorPatch, -sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, sNFLWork->sharedTex[2].texAddr, obj->eyePlttAddr);
    idx = (u16)((obj->exprFields[6] + sNFLEyebrowTypeRotations[obj->exprFields[7]] - 0x20) << 11);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->browPatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    NFLi_PatchFacePart(obj, &obj->browMirrorPatch, -sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    switch (type) {
    case 2:
    case 6:
    case 7:
        y += 0x1E60;
        break;
    }
    if (obj->negate.x != 0) {
        v = -y / 32;
    } else {
        v = y / 32;
    }
    obj->browTrans.param[obj->yAxis] = v;
    obj->browMirrorTrans.param[obj->yAxis] = v;
    sin = (fx16)FX_SinIdx(0);
    cos = (fx16)FX_CosIdx(0);
    NFLi_PatchFacePart(obj, &obj->mouthPatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->mouthTexAddr, obj->mouthPlttAddr);
}
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma optimization_level reset

// Kind 2: the eyes turned 2 steps more (kept to 0..7), the eyebrows 2 more (to 0..11), the mouth of shared slot 3.
// The eyebrow's sum is tested on a copy of the rotation and added again in the else: the original adds d twice.
#pragma optimization_level 2
#pragma opt_common_subs off
#pragma opt_propagation off
void NFLi_ApplyExpression2(NFLFace *obj) {
    fx16 sin;
    fx16 cos;
    u16 idx;
    fx32 v;
    int rot;
    int brow;
    int d;
    int browRot;
    fx32 y;
    int type;

    browRot = obj->exprFields[6];
    type = obj->exprFields[5];
    brow = browRot;
    d = 2;
    rot = obj->exprFields[3] + d;
    y = -(obj->exprFields[2] * 0x10E1 - 0x22490);
    if (rot < 0) {
        rot = 0;
    } else if (rot > 7) {
        rot = 7;
    }
    idx = (u16)(((rot + sNFLEyeTypeRotations[obj->exprFields[4]] - 0x20) << 16) / 32);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->eyePatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->eyeTexAddr, obj->eyePlttAddr);
    NFLi_PatchFacePart(obj, &obj->eyeMirrorPatch, -sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->eyeTexAddr, obj->eyePlttAddr);
    if (browRot + d < 0) {
        brow = 0;
    } else if (browRot + d > 11) {
        brow = 11;
    } else {
        brow += d;
    }
    idx = (u16)((brow + sNFLEyebrowTypeRotations[obj->exprFields[7]] - 0x20) << 11);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->browPatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    NFLi_PatchFacePart(obj, &obj->browMirrorPatch, -sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    switch (type) {
    case 2:
    case 6:
    case 7:
        y += 0x1E60;
        break;
    }
    if (obj->negate.x != 0) {
        v = -y / 32;
    } else {
        v = y / 32;
    }
    obj->browTrans.param[obj->yAxis] = v;
    obj->browMirrorTrans.param[obj->yAxis] = v;
    sin = (fx16)FX_SinIdx(0);
    cos = (fx16)FX_CosIdx(0);
    NFLi_PatchFacePart(obj, &obj->mouthPatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, sNFLWork->sharedTex[3].texAddr, obj->mouthPlttAddr);
}
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma optimization_level reset

// Kind 3: the eyes and eyebrows turned 2 steps less, the mouth of shared slot 4; an eyebrow rotation past 11 is set
// to 0
#pragma optimization_level 2
#pragma opt_common_subs off
#pragma opt_propagation off
void NFLi_ApplyExpression3(NFLFace *obj) {
    fx16 sin;
    fx16 cos;
    u16 idx;
    fx32 v;
    int rot;
    int brow;
    int d;
    int browRot;
    fx32 y;
    int type;

    browRot = obj->exprFields[6];
    type = obj->exprFields[5];
    brow = browRot;
    d = -2;
    rot = obj->exprFields[3] + d;
    y = -(obj->exprFields[2] * 0x10E1 - 0x22490);
    if (rot < 0) {
        rot = 0;
    } else if (rot > 7) {
        rot = 7;
    }
    idx = (u16)(((rot + sNFLEyeTypeRotations[obj->exprFields[4]] - 0x20) << 16) / 32);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->eyePatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->eyeTexAddr, obj->eyePlttAddr);
    NFLi_PatchFacePart(obj, &obj->eyeMirrorPatch, -sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->eyeTexAddr, obj->eyePlttAddr);
    if (browRot + d < 0) {
        brow = 0;
    } else if (browRot + d > 11) {
        brow = 0;
    } else {
        brow += d;
    }
    idx = (u16)((brow + sNFLEyebrowTypeRotations[obj->exprFields[7]] - 0x20) << 11);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->browPatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    NFLi_PatchFacePart(obj, &obj->browMirrorPatch, -sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    switch (type) {
    case 2:
    case 6:
    case 7:
        y += 0x1E60;
        break;
    }
    if (obj->negate.x != 0) {
        v = -y / 32;
    } else {
        v = y / 32;
    }
    obj->browTrans.param[obj->yAxis] = v;
    obj->browMirrorTrans.param[obj->yAxis] = v;
    sin = (fx16)FX_SinIdx(0);
    cos = (fx16)FX_CosIdx(0);
    NFLi_PatchFacePart(obj, &obj->mouthPatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, sNFLWork->sharedTex[4].texAddr, obj->mouthPlttAddr);
}
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma optimization_level reset

// Kind 4: the face's own second eye texture (eye type 0x31), the rotation moved as kind 1's by the table at 49, the
// eyebrows placed as for eyebrowY - 2.
// Not matching yet: as NFLi_ApplyExpression1.
#pragma optimization_level 2
#pragma opt_common_subs off
#pragma opt_propagation off
void NFLi_ApplyExpression4(NFLFace *obj) {
    fx16 sin;
    fx16 cos;
    fx32 y;
    int type;
    u16 idx;
    fx32 v;
    int rot;

    rot = obj->exprFields[3] + (sNFLEyeTypeRotations[obj->exprFields[3]] - sNFLEyeTypeRotations[49]);
    y = -((obj->exprFields[2] - 2) * 0x10E1 - 0x22490);
    type = obj->exprFields[5];
    if (rot < 0) {
        rot = 0;
    } else if (rot > 7) {
        rot = 7;
    }
    idx = (u16)(((rot + sNFLEyeTypeRotations[obj->exprFields[4]] - 0x20) << 16) / 32);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->eyePatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->expr4EyeTexAddr, obj->expr4EyePlttAddr);
    NFLi_PatchFacePart(obj, &obj->eyeMirrorPatch, -sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->expr4EyeTexAddr, obj->expr4EyePlttAddr);
    idx = (u16)((obj->exprFields[6] + sNFLEyebrowTypeRotations[obj->exprFields[7]] - 0x20) << 11);
    sin = (fx16)FX_SinIdx(idx);
    cos = (fx16)FX_CosIdx(idx);
    NFLi_PatchFacePart(obj, &obj->browPatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    NFLi_PatchFacePart(obj, &obj->browMirrorPatch, -sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, 0, 0);
    switch (type) {
    case 2:
    case 6:
    case 7:
        y += 0x1E60;
        break;
    }
    if (obj->negate.x != 0) {
        v = -y / 32;
    } else {
        v = y / 32;
    }
    obj->browTrans.param[obj->yAxis] = v;
    obj->browMirrorTrans.param[obj->yAxis] = v;
    sin = (fx16)FX_SinIdx(0);
    cos = (fx16)FX_CosIdx(0);
    NFLi_PatchFacePart(obj, &obj->mouthPatch, sin, cos, obj->texFmt, GX_TEXSIZE_S32, GX_TEXSIZE_T32, obj->mouthTexAddr, obj->mouthPlttAddr);
}
#pragma opt_propagation reset
#pragma opt_common_subs reset
#pragma optimization_level reset

int NFLi_PrepareFacePartTextures(NFLMakeCtx *ctx);
int NFLi_AllocExpressionTextures(NFLMakeCtx *ctx);
void NFLi_PlaceFaceParts(NFLMakeCtx *ctx);

// The face parts' part function, the first of sNFLPartPreparers
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

NFLFace *NFLi_GetFace(int index);
void NFLi_ApplyExpression0(NFLFace *obj);
void NFLi_ApplyExpression5(NFLFace *obj);
void NFLi_ApplyExpression6(NFLFace *obj);
void NFLi_ApplyExpression1(NFLFace *obj);
void NFLi_ApplyExpression2(NFLFace *obj);
void NFLi_ApplyExpression3(NFLFace *obj);
void NFLi_ApplyExpression4(NFLFace *obj);

// Gives face index expression kind (1 to 6) if its bit of exprFlags is set, else the Mii's own
// (kind 0), and keeps the kind
int NFL_SetExpression(int index, int type) {
    NFLFace *obj;

    if (!NFLi_IsInitialized()) {
        return 4;
    }
    obj = NFLi_GetFace(index);
    if (obj == NULL) {
        return 9;
    }
    switch (type) {
    case 5:
        if (obj->exprFlags & 0x20) {
            NFLi_ApplyExpression5(obj);
            obj->expression = 5;
            return 0;
        }
        break;
    case 6:
        if (obj->exprFlags & 0x40) {
            NFLi_ApplyExpression6(obj);
            obj->expression = 6;
            return 0;
        }
        break;
    case 1:
        if (obj->exprFlags & 0x2) {
            NFLi_ApplyExpression1(obj);
            obj->expression = 1;
            return 0;
        }
        break;
    case 2:
        if (obj->exprFlags & 0x4) {
            NFLi_ApplyExpression2(obj);
            obj->expression = 2;
            return 0;
        }
        break;
    case 3:
        if (obj->exprFlags & 0x8) {
            NFLi_ApplyExpression3(obj);
            obj->expression = 3;
            return 0;
        }
        break;
    case 4:
        if (obj->exprFlags & 0x10) {
            NFLi_ApplyExpression4(obj);
            obj->expression = 4;
            return 0;
        }
        break;
    }
    NFLi_ApplyExpression0(obj);
    obj->expression = 0;
    return 0;
}

// The kind NFL_SetExpression kept for face index, -1 without one
int NFL_GetExpression(int index) {
    NFLFace *obj;

    if (!NFLi_IsInitialized()) {
        return -1;
    }
    obj = NFLi_GetFace(index);
    if (obj == NULL) {
        return -1;
    }
    return obj->expression;
}

// Where NFL_Init's VRAM goes: the texture and palette addresses the shared slots start at, then the faces', and the
// faces' texture format (GX_TEXFMT_DIRECT or a palette format: 3 is tested)
typedef struct NFLDesc {
    u32 texAddr;
    u32 plttAddr;
    u32 texFmt;
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
    NFLLoadState *sub;

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
    work->randState = seed;
    work->randMul = 0x5D588B65;
    work->randAdd = 0x269EC3;
    work->texAddr = desc->texAddr;
    work->plttAddr = desc->plttAddr;
    work->texFmt = desc->texFmt;
    work->exprFlags = flags;
    work->loadList = NULL;
    work->phase = 4;
    work->stage = 2;
    work->buf = buf;
    work->bufSize = size;
    work->readOffset = 0;
    work->writeOffset = 0;
    work->pending = 0;
    work->loadedSize = 0;
    work->copiedSize = 0;
    sub = (NFLLoadState *)&work->loadList;
    for (i = 0; i < 5; i++) {
        if (flags & sNFLSharedStreamFlags[i]) {
            sub->streams[i].state = 0;
            n++;
        } else {
            sub->streams[i].state = 7;
        }
    }
    work->maxFaces = max;
    work->faces = NULL;
    work->streams[0].state = 0;
    if (desc->texFmt == 7) {
        work->streams[6].state = 7;
    } else {
        work->streams[6].state = 0;
    }
    if (desc->texFmt == 3) {
        work->faceTexBase = desc->texAddr + ((n << 11) + 0x200) / 4;
        work->facePlttBase = desc->plttAddr + 0x20;
    } else {
        work->faceTexBase = desc->texAddr + (n << 11) + 0x200;
        work->facePlttBase = desc->plttAddr;
    }
    enabled = OS_DisableInterrupts();
    sNFLWork = work;
    NFLi_SetResource(res);
    OS_RestoreInterrupts(enabled);
    return 0;
}

// Whether NFL_Init has published the work area
BOOL NFLi_IsInitialized(void) {
    return sNFLWork != NULL;
}

// Marks the work area's six shared texture slots as not given VRAM yet
int NFLi_ResetSharedTextures(u8 *p) {
    *(u32 *)(p + 0x18) = 0;
    *(u32 *)(p + 0x24) = 0;
    *(u32 *)(p + 0x30) = 0;
    *(u32 *)(p + 0x3c) = 0;
    *(u32 *)(p + 0x48) = 0;
    *(u32 *)(p + 0x54) = 0;
    return 0;
}

// Appends a face to the work area's list, to be made (state 4): 0x10 when the list holds maxFaces, 0x11
// when the face is in it already
int NFL_AddFace(NFLFace *obj) {
    OSIntrMode enabled;
    NFLWork *work;
    NFLFace *last;
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
    if (work->phase == 1 || work->phase == 3) {
        OS_RestoreInterrupts(enabled);
        return 6;
    }
    if (work->maxFaces == 0) {
        OS_RestoreInterrupts(enabled);
        return 0x10;
    }
    last = work->faces;
    if (last == NULL) {
        work->faces = obj;
        obj->next = NULL;
        obj->state = 4;
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
        if (n >= work->maxFaces) {
            OS_RestoreInterrupts(enabled);
            return 0x10;
        }
        last->next = obj;
        obj->next = NULL;
        obj->state = 4;
    }
    OS_RestoreInterrupts(enabled);
    return 0;
}

// Allows one more face (maxFaces)
int NFL_AddFaceSlot(void) {
    if (!NFLi_IsInitialized()) {
        return 4;
    }
    sNFLWork->maxFaces++;
    return 0;
}

// Allows one face fewer, and unlinks the last face if the list is then too long (returned in
// removed)
int NFL_RemoveFaceSlot(NFLFace **removed) {
    OSIntrMode enabled;
    NFLFace *obj = NULL;
    NFLWork *work;
    NFLFace *prev;
    int n;
    NFLFace *cur;

    enabled = OS_DisableInterrupts();
    if (!NFLi_IsInitialized()) {
        OS_RestoreInterrupts(enabled);
        return 4;
    }
    work = sNFLWork;
    if (work->phase == 1 || work->phase == 3) {
        OS_RestoreInterrupts(enabled);
        return 6;
    }
    if (work->maxFaces != 0) {
        work->maxFaces--;
    }
    if (removed != NULL) {
        *removed = NULL;
    }
    prev = sNFLWork->faces;
    if (prev != NULL) {
        if (sNFLWork->maxFaces == 0) {
            prev->state = 4;
            obj = prev;
            sNFLWork->faces = NULL;
        } else {
            cur = prev->next;
            n = 1;
            while (cur != NULL) {
                if (cur->next == NULL) {
                    if (n >= sNFLWork->maxFaces) {
                        cur->state = 4;
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
        prev = work->loadList;
        if (prev != NULL) {
            if (prev == obj) {
                work->loadList = obj->nextLoad;
            } else {
                for (cur = prev->nextLoad; cur != NULL; cur = cur->nextLoad) {
                    if (cur == obj) {
                        prev->nextLoad = obj->nextLoad;
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

// Sets face index to be made again (state 4)
int NFL_ResetFace(int index) {
    OSIntrMode enabled = OS_DisableInterrupts();
    NFLFace *obj;

    if (!NFLi_IsInitialized()) {
        OS_RestoreInterrupts(enabled);
        return 4;
    }
    if (sNFLWork->phase == 1 || sNFLWork->phase == 3) {
        OS_RestoreInterrupts(enabled);
        return 6;
    }
    obj = NFLi_GetFace(index);
    if (obj == NULL) {
        OS_RestoreInterrupts(enabled);
        return 9;
    }
    obj->state = 4;
    OS_RestoreInterrupts(enabled);
    return 0;
}

// The index of the first face to be made (state 4), -1 if none
int NFL_FindResetFace(void) {
    int n;
    NFLFace *obj;
    NFLWork *work;
    OSIntrMode enabled = OS_DisableInterrupts();

    if (!NFLi_IsInitialized()) {
        OS_RestoreInterrupts(enabled);
        return -1;
    }
    work = sNFLWork;
    if (work->maxFaces == 0) {
        OS_RestoreInterrupts(enabled);
        return -1;
    }
    n = 0;
    for (obj = work->faces; obj != NULL; obj = obj->next) {
        if (obj->state == 4) {
            OS_RestoreInterrupts(enabled);
            return n;
        }
        n++;
    }
    OS_RestoreInterrupts(enabled);
    return -1;
}

// The face of the list at index, NULL if there is none
NFLFace *NFLi_GetFace(int index) {
    int i;
    NFLFace *obj;
    NFLWork *work;
    OSIntrMode enabled = OS_DisableInterrupts();

    if (!NFLi_IsInitialized()) {
        OS_RestoreInterrupts(enabled);
        return NULL;
    }
    work = sNFLWork;
    if (index >= work->maxFaces) {
        OS_RestoreInterrupts(enabled);
        return NULL;
    }
    i = 0;
    for (obj = work->faces; obj != NULL; obj = obj->next) {
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

// Gives char data the create ID 0xE0 then seven zeros: bit 5 of the first byte set, which
// NFL_IsCreateIdUnassigned tests and MiiData_AssignCreateId requires before it gives an ID
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

// Whether bit 7 of the first byte is clear
BOOL NFL_IsCreateIdBit7Clear(const NFLCreateId *id) {
    return !(id->data[0] & 0x80);
}

// Bit 6 of the first byte
u32 NFLi_GetCreateIdBit6(const NFLCreateId *id) {
    return id->data[0] & 0x40;
}

// Whether the create ID is all zero
BOOL NFL_IsCreateIdZero(const NFLCreateId *id) {
    return *(u32 *)&id->data[0] == 0 && *(u32 *)&id->data[4] == 0;
}

// Whether b has a's low nibble of byte 2 and bytes 3 to 7 (the six bytes MiiData_AssignCreateId
// takes from its source), and is neither zero nor unassigned
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

// Bit 5 of the first byte (see NFLi_SetUnassignedCreateId)
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

// Zeroes a create ID
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
    /* 0x38 */ u16 faceColor; // NFLi_GetFaceColor
} NFLCharInfo;

u16 NFLi_GetFaceColor(int faceColor);

// Fills info from valid char data: the names as strings, the create ID, the fields, and the face
// colour as a GXRgb; 7 for invalid data
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
    info->faceColor = NFLi_GetFaceColor(data->faceColor);
    return 0;
}

u16 NFLi_GetFavoriteColor(int index);

// The favourite colour index as a GXRgb
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

// Keeps the resource (NFL_Res.dat), unless the loading is busy
int NFLi_SetResource(void *p) {
    OSIntrMode enabled = OS_DisableInterrupts();
    NFLWork *work = sNFLWork;

    if (work == NULL) {
        OS_RestoreInterrupts(enabled);
        return 4;
    }
    if (work->phase == 1 || work->phase == 3) {
        OS_RestoreInterrupts(enabled);
        return 6;
    }
    work->resource = p;
    OS_RestoreInterrupts(enabled);
    return 0;
}

BOOL NFLi_HasResource(void) {
    return sNFLWork->resource != NULL;
}

BOOL NFLi_HasBuffer(void) {
    return sNFLWork->buf != NULL;
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

// Makes a random Mii: sex 0 or 1 (2 for either), b and c 0 to 2 (3 for any), and an unassigned create
// ID
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

// The face type of a random Mii: one of the ten choices of row sex * 9 + b * 3 + c
u8 NFLi_RandomFaceType(int sex, int b, int c) {
    return sNFLRandomFaceTypes[sex * 9 + b * 3 + c][NFL_Random(10)];
}

// The hair type of a random Mii: one of the choices of row sex * 9 + b * 3 + c. Its choices and counts, and those of
// the eye, eyebrow, nose and mouth pickers, are local arrays: the compiler copies them from .rodata objects of its own,
// which symbols.txt names after them (sNFLRandomHairTypeChoices, sNFLRandomHairTypeCounts, ...)
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

// The face texture of a random Mii: one of the twenty choices of row sex * 9 + b * 3 + c
u8 NFLi_RandomFaceTexture(int sex, int b, int c) {
    return sNFLRandomFaceTextures[sex * 9 + b * 3 + c][NFL_Random(20)];
}

// The glasses of a random Mii, by b: none for most
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

// The face colour of a random Mii: one of the ten choices of row sex * 3 + c
u8 NFLi_RandomFaceColor(int sex, int c) {
    return sNFLRandomFaceColors[sex * 3 + c][NFL_Random(10)];
}

extern const u8 sNFLRandomHairColors[][20];

// The hair colour of a random Mii: one of the twenty choices of row c * 3 + b
u8 NFLi_RandomHairColor(int b, int c) {
    return sNFLRandomHairColors[c * 3 + b][NFL_Random(20)];
}

extern const u8 sNFLRandomEyeColors[][10];

// The eye colour of a random Mii: one of the ten choices of row c
u8 NFLi_RandomEyeColor(int c) {
    return sNFLRandomEyeColors[c][NFL_Random(10)];
}

// A random number below max (a 16-bit one when max is 0), from the LCG in the work area
u16 NFL_Random(u16 max) {
    NFLWork *work = sNFLWork;

    u32 r;

    work->randState = work->randMul * work->randState + work->randAdd;
    if (max == 0) {
        r = work->randState;
    } else {
        r = (work->randState >> 16) * max;
    }
    return r >> 16;
}

// The coordinate helpers: a vector in the library's axes to the drawing context's, each component to the axis its
// index says, negated where its flag says
void NFLi_MapVecFx16(const NFLDrawContext *ctx, VecFx16 *out, const VecFx16 *in) {
    ((fx16 *)out)[ctx->xAxis] = ctx->negate.z ? -in->x : in->x;
    ((fx16 *)out)[ctx->yAxis] = ctx->negate.x ? -in->y : in->y;
    ((fx16 *)out)[ctx->zAxis] = ctx->negate.y ? -in->z : in->z;
}

void NFLi_MapVecFx32(const NFLDrawContext *ctx, VecFx32 *out, const VecFx32 *in) {
    ((fx32 *)out)[ctx->xAxis] = ctx->negate.z ? -in->x : in->x;
    ((fx32 *)out)[ctx->yAxis] = ctx->negate.x ? -in->y : in->y;
    ((fx32 *)out)[ctx->zAxis] = ctx->negate.y ? -in->z : in->z;
}

// The same without the negations
void NFLi_MapVecFx32NoNegate(const NFLDrawContext *ctx, VecFx32 *out, const VecFx32 *in) {
    ((fx32 *)out)[ctx->xAxis] = in->x;
    ((fx32 *)out)[ctx->yAxis] = in->y;
    ((fx32 *)out)[ctx->zAxis] = in->z;
}

// As NFLi_MapVecFx32, through a local array, so that out may be in
void NFLi_MapVecFx32Copy(const NFLDrawContext *ctx, VecFx32 *out, const VecFx32 *in) {
    fx32 v[3];

    v[ctx->xAxis] = ctx->negate.z ? -in->x : in->x;
    v[ctx->yAxis] = ctx->negate.x ? -in->y : in->y;
    v[ctx->zAxis] = ctx->negate.y ? -in->z : in->z;
    out->x = v[0];
    out->y = v[1];
    out->z = v[2];
}

// Rotation about the context's z axis
void NFLi_MakeRotZMtx(const NFLDrawContext *ctx, MtxFx43 *mtx, fx32 sinVal, fx32 cosVal) {
    fx32 a[3];
    VecFx32 axis;

    a[ctx->xAxis] = 0;
    a[ctx->yAxis] = 0;
    a[ctx->zAxis] = ctx->negate.y ? -FX32_ONE : FX32_ONE;
    axis.x = a[0];
    axis.y = a[1];
    axis.z = a[2];
    MTX_RotAxis43(mtx, &axis, sinVal, cosVal);
}

// The display-list commands of a vector in the library's axes: G3C_Translate
void NFLi_DLTranslate(const NFLDrawContext *ctx, GXDLInfo *info, fx32 x, fx32 y, fx32 z) {
    VecFx32 in;
    VecFx32 out;

    in.x = x;
    in.y = y;
    in.z = z;
    NFLi_MapVecFx32(ctx, &out, &in);
    G3C_Translate(info, out.x, out.y, out.z);
}

// G3C_Scale. Compiled as the hair meshes, without the IR optimiser's passes: the stores to in stay, and the
// arguments go through NFLi_PutScale last to first
#pragma push
#pragma opt_common_subs off
#pragma opt_propagation off
#pragma opt_loop_invariants off
#pragma opt_strength_reduction off
#pragma opt_rotateloops off
#pragma opt_scalarize off
#pragma opt_rebuildlogicals off
#pragma opt_collapselogicalselectionintobittest off
void NFLi_DLScale(const NFLDrawContext *ctx, GXDLInfo *info, fx32 x, fx32 y, fx32 z) {
    VecFx32 in;
    fx32 out[3];

    in.x = x;
    in.y = y;
    in.z = z;
    out[ctx->xAxis] = in.x;
    out[ctx->yAxis] = in.y;
    out[ctx->zAxis] = in.z;
    NFLi_PutScale(info, out[0], out[1], out[2]);
}
#pragma pop

// G3C_Vtx
void NFLi_DLVtx(const NFLDrawContext *ctx, GXDLInfo *info, fx16 x, fx16 y, fx16 z) {
    VecFx16 in;
    VecFx16 out;

    in.x = x;
    in.y = y;
    in.z = z;
    NFLi_MapVecFx16(ctx, &out, &in);
    G3C_Vtx(info, out.x, out.y, out.z);
}

// G3C_Normal
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
    u16 firstBase;
    u16 secondBase;
    u16 firstSize;
} NFLTexBank;

extern const NFLTexBank sNFLTexBankParts[];

// Copies size bytes of texture image to offset of the texture slots, through the LCDC, as GX_LoadTex does (and in
// its shape: the first part's base, the second's, the first's size, the copy split over the two parts)
// Not matching: the instructions and callee-saved registers are the original's, but the original loads the fields'
// addresses in the order second, first, size (its literal pool +2, +0, +4), which with any statement order that loads
// them so gives the size and first base the other two registers; here the size is read first (pool +2, +4, +0).
int NFLi_CopyToTexBank(int bank, const void *src, u32 offset, u32 size) {
    u32 szBlk1 = sNFLTexBankParts[bank].firstSize;
    u32 blk2 = sNFLTexBankParts[bank].secondBase << 12;
    u32 blk1 = sNFLTexBankParts[bank].firstBase;
    u32 end = offset + size;
    void *temp;
    int result;

    result = NFLi_CheckTexBank(bank, offset, end);
    if (result != 0) {
        return result;
    }
    if (blk2 == 0) {
        temp = (void *)(offset + (blk1 << 12));
    } else if (end < szBlk1 << 12) {
        temp = (void *)(offset + (blk1 << 12));
    } else if (offset >= szBlk1 << 12) {
        temp = (void *)(blk2 + offset - (szBlk1 << 12));
    } else {
        void *temp2 = (void *)blk2;
        u32 temp1 = (szBlk1 << 12) - offset;

        temp = (void *)(offset + (blk1 << 12));
        MIi_CpuCopy32(src, temp, temp1);
        DC_FlushRange(temp, temp1);
        size -= temp1;
        MIi_CpuCopy32((const u8 *)src + temp1, temp2, size);
        DC_FlushRange(temp2, size);
        return 0;
    }
    MIi_CpuCopy32(src, temp, size);
    DC_FlushRange(temp, size);
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

// Copies to texture VRAM, in the banks func_0200473c gives for textures
int NFLi_LoadTex(const void *src, u32 offset, u32 size) {
    int bank = func_0200473c();
    int result;

    GX_BeginLoadTex();
    result = NFLi_CopyToTexBank(bank, src, offset, size);
    GX_EndLoadTex();
    return result;
}

// Copies to texture palette VRAM, in the banks func_0200474c gives for palettes
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
BOOL NFLi_CopyFaceTextures(NFLFace *obj, u32 *arg);
BOOL NFLi_CopyFacePltts(NFLFace *obj, u32 *arg);
void NFLi_RemoveFromLoadList(NFLFace *obj);

// The library's work of a frame, which main calls every frame: 4 not set up, 0x14 and 0x13 busy, 3 nothing to do
int NFL_FillBuffer(void) {
    NFLWork *work = sNFLWork;
    u32 arg;

    if (work == NULL) {
        return 4;
    }
    arg = work->pending;
    if (work->phase == 3) {
        return 0x14;
    }
    if (work->phase == 1) {
        return 0x13;
    }
    if (work->phase == 4) {
        return 3;
    }
    work->phase = 1;
    work->started = 0;
    switch (work->stage) {
    case 0:
        NFLi_CopySharedTextures(&arg);
        work->phase = 2;
        break;
    case 1:
        NFLi_CopySharedPltt(&arg);
        work->phase = 2;
        break;
    case 2:
        if (work->loadList == NULL) {
            work->phase = 4;
            work->pending = 0;
            work->readOffset = 0;
            work->writeOffset = 0;
            return 3;
        }
        switch (work->loadList->state) {
        case 0:
            NFLi_CopyFaceTextures(work->loadList, &arg);
            work->phase = 2;
            break;
        case 1:
            NFLi_CopyFacePltts(work->loadList, &arg);
            work->phase = 2;
            break;
        case 2:
        case 4:
            NFLi_RemoveFromLoadList(work->loadList);
            work->phase = 0;
            break;
        }
        break;
    }
    work->pending = arg;
    return 0;
}



// Copies size bytes, or converts them through palette when there is one
void NFLi_CopyTexData(const void *src, void *dest, const u16 *palette, u32 size, BOOL transparent) {
    if (palette != NULL) {
        NFLi_Convert4BitToDirect(dest, src, palette, size, transparent);
    } else {
        MIi_CpuCopy32(src, dest, size);
    }
}

// What NFLi_CopySharedTextures copies to each of the six shared streams: a resource item (NFLi_GetResourceItem's
// category and index) and its size
typedef struct NFLSharedStreamSource {
    int category;
    int index;
    u32 size;
} NFLSharedStreamSource;

extern NFLSharedStreamSource sNFLSharedStreamSources[];



// Stage 0 of NFL_FillBuffer: the six shared streams (sNFLSharedStreamSources) into the ring buffer, converted to direct
// colours in mode 7, then on to the shared palette or, in mode 7, the faces.
// Not matching: register allocation only (the original has pal in r5, source in r7 and the zero of extra in r8)
BOOL NFLi_CopySharedTextures(u32 *pos) {
    NFLWork *work = (NFLWork *)sNFLWork;
    NFLSharedStreamSource *source = sNFLSharedStreamSources;
    NFLStream *stream = work->streams;
    int i;
    u16 palette[16];

    for (i = 0; i < 6; stream++, source++, i++) {
        u32 extra;
        u16 *pal;
        u8 *src;
        int size;
        BOOL overflowB;
        BOOL overflowA;

        if (*pos >= work->bufSize) {
            return TRUE;
        }
        if (!work->started) {
            if (stream->state >= 2 && stream->state <= 5) {
                work->started = TRUE;
            } else if (stream->state <= 1) {
                work->pending = 0;
                work->readOffset = 0;
                work->writeOffset = 0;
                *pos = 0;
                work->started = TRUE;
            }
        }
        if (stream->state == 0 || stream->state == 1 || stream->state == 3 || stream->state == 4) {
        extra = 0;
        overflowB = FALSE;
        overflowA = FALSE;
        if (sNFLWork->texFmt == 7) {
            size = source->size * 4;
            if (source->category == 2) {
                NFLi_MakeEyePltt(palette, 0);
                pal = palette;
            } else if (source->category == 13) {
                NFLi_MakeMouthPltt(palette, 0);
                pal = palette;
            } else {
                pal = NFLi_GetResourceItem(12, 0);
            }
        } else {
            size = source->size;
            pal = NULL;
        }
        if (stream->state == 0) {
            u32 offset;

            if (*pos + size > work->bufSize) {
                overflowA = TRUE;
                size = work->bufSize - *pos;
            }
            offset = work->writeOffset;
            if (offset + size > work->bufSize) {
                overflowB = TRUE;
            }
            if (!overflowB) {
                src = NFLi_GetResourceItem(source->category, source->index);
                NFLi_CopyTexData(src, work->buf + offset, pal, size, TRUE);
                DC_FlushRange(work->buf + work->writeOffset, size);
            } else {
                int over = work->writeOffset + size - work->bufSize;
                int n;

                src = NFLi_GetResourceItem(source->category, source->index);
                n = size - over;
                NFLi_CopyTexData(src, work->buf + work->writeOffset, pal, n, TRUE);
                DC_FlushRange(work->buf + work->writeOffset, n);
                if (sNFLWork->texFmt == 7) {
                    src += n / 4;
                } else {
                    src += n;
                }
                NFLi_CopyTexData(src, work->buf, pal, over, TRUE);
                DC_FlushRange(work->buf, over);
            }
            if (!overflowA) {
                stream->state = 2;
            } else {
                work->copiedSize = size;
                stream->state = 3;
            }
            stream->bufOffset = work->writeOffset;
        } else {
            u8 *p = NFLi_GetResourceItem(source->category, source->index);

            switch (stream->state) {
            case 3:
                if (sNFLWork->texFmt == 7) {
                    p += (int)work->copiedSize / 4;
                } else {
                    p += work->copiedSize;
                }
                size -= work->copiedSize;
                extra = 0;
                break;
            case 4:
                if (sNFLWork->texFmt == 7) {
                    p += (int)work->copiedSize / 4;
                } else {
                    p += work->copiedSize;
                }
                size -= work->copiedSize;
                extra = work->loadedSize;
                break;
            case 1:
                extra = work->loadedSize;
                if (sNFLWork->texFmt == 7) {
                    p += (int)extra / 4;
                } else {
                    p += extra;
                }
                size -= extra;
                stream->bufOffset = work->writeOffset;
                break;
            }
            if (*pos + size > work->bufSize) {
                overflowA = TRUE;
                size = work->bufSize - *pos;
            }
            if (work->writeOffset + size > work->bufSize) {
                overflowB = TRUE;
            }
            if (!overflowA) {
                stream->state = stream->state == 3 ? 2 : 5;
                work->copiedSize = 0;
            } else {
                switch (stream->state) {
                case 3:
                case 4:
                    work->copiedSize += size;
                    break;
                case 1:
                    work->copiedSize = work->loadedSize + size;
                    stream->state = 4;
                    break;
                }
            }
            if (!overflowB) {
                NFLi_CopyTexData(p, work->buf + work->writeOffset, palette, size, TRUE);
                DC_FlushRange(work->buf + work->writeOffset, size);
            } else {
                int over = work->writeOffset + size - work->bufSize;
                int n = size - over;

                NFLi_CopyTexData(p, work->buf + work->writeOffset, palette, n, TRUE);
                DC_FlushRange(work->buf + work->writeOffset, n);
                if (sNFLWork->texFmt == 7) {
                    p += n / 4;
                } else {
                    p += n;
                }
                NFLi_CopyTexData(p, work->buf, palette, over, TRUE);
                DC_FlushRange(work->buf, over);
            }
        }
        switch (i) {
        case 0:
            stream->vramAddr = sNFLWork->sharedTex[0].texAddr + extra;
            break;
        case 1:
            stream->vramAddr = sNFLWork->sharedTex[1].texAddr + extra;
            break;
        case 2:
            stream->vramAddr = sNFLWork->sharedTex[2].texAddr + extra;
            break;
        case 3:
            stream->vramAddr = sNFLWork->sharedTex[3].texAddr + extra;
            break;
        case 4:
            stream->vramAddr = sNFLWork->sharedTex[4].texAddr + extra;
            break;
        case 5:
            stream->vramAddr = sNFLWork->sharedTex[5].texAddr + extra;
            break;
        }
        work->writeOffset = (work->writeOffset + size) % work->bufSize;
        *pos += size;
        }
    }
    if (sNFLWork->texFmt != 7) {
        NFLi_CopySharedPltt(pos);
    } else {
        if (work->loadList == NULL) {
            return TRUE;
        }
        switch (work->loadList->state) {
        case 0:
            NFLi_CopyFaceTextures(work->loadList, pos);
            break;
        case 1:
            NFLi_CopyFacePltts(work->loadList, pos);
            break;
        }
    }
    return TRUE;
}

// Stage 1 of NFL_FillBuffer: the work area's seventh stream, the shared palette, 32 bytes of resource category 12
// item 0 (the palette NFLi_CopySharedTextures converts through in mode 7)
BOOL NFLi_CopySharedPltt(u32 *pos) {
    NFLWork *work = (NFLWork *)sNFLWork;

    if (work->texFmt != 7) {
        if (*pos >= work->bufSize) {
            return TRUE;
        }
        if (!work->started) {
            if (work->streams[6].state >= 2 && work->streams[6].state <= 5) {
                work->started = TRUE;
            } else if (work->streams[6].state <= 1) {
                work->pending = 0;
                work->readOffset = 0;
                work->writeOffset = 0;
                *pos = 0;
                work->started = TRUE;
            }
        }
        if (work->streams[6].state == 0 || work->streams[6].state == 1 || work->streams[6].state == 3 ||
            work->streams[6].state == 4) {
            u32 extra = 0;
            BOOL overflowB = FALSE;
            BOOL overflowA = FALSE;
            int size = 32;
            u8 *src;

            if (work->streams[6].state == 0) {
                u32 offset;

                if (*pos + size > work->bufSize) {
                    size = work->bufSize - *pos;
                    overflowA = TRUE;
                }
                offset = work->writeOffset;
                if (offset + size > work->bufSize) {
                    overflowB = TRUE;
                }
                if (!overflowB) {
                    u8 *buf = work->buf;

                    src = NFLi_GetResourceItem(12, 0);
                    MIi_CpuCopy32(src, buf + offset, size);
                    DC_FlushRange(work->buf + work->writeOffset, size);
                } else {
                    int over = offset + size - work->bufSize;
                    int n;

                    src = NFLi_GetResourceItem(12, 0);
                    n = size - over;
                    MIi_CpuCopy32(src, work->buf + work->writeOffset, n);
                    DC_FlushRange(work->buf + work->writeOffset, n);
                    MIi_CpuCopy32(src + n, work->buf, over);
                    DC_FlushRange(work->buf, over);
                }
                if (!overflowA) {
                    work->streams[6].state = 2;
                } else {
                    work->copiedSize = size;
                    work->streams[6].state = 3;
                }
                work->streams[6].bufOffset = work->writeOffset;
            } else {
                u8 *p = NFLi_GetResourceItem(12, 0);

                switch (work->streams[6].state) {
                case 3:
                    p += work->copiedSize;
                    size -= work->copiedSize;
                    break;
                case 4:
                    p += work->copiedSize;
                    extra = work->loadedSize;
                    size -= work->copiedSize;
                    break;
                case 1:
                    extra = work->loadedSize;
                    work->streams[6].bufOffset = work->writeOffset;
                    p += extra;
                    size -= extra;
                    break;
                }
                if (*pos + size > work->bufSize) {
                    size = work->bufSize - *pos;
                    overflowA = TRUE;
                }
                if (work->writeOffset + size > work->bufSize) {
                    overflowB = TRUE;
                }
                if (!overflowA) {
                    work->streams[6].state = work->streams[6].state == 3 ? 2 : 5;
                    work->copiedSize = 0;
                } else {
                    switch (work->streams[6].state) {
                    case 3:
                    case 4:
                        work->copiedSize += size;
                        break;
                    case 1:
                        work->copiedSize = work->loadedSize + size;
                        work->streams[6].state = 4;
                        break;
                    }
                }
                if (!overflowB) {
                    MIi_CpuCopy32(p, work->buf + work->writeOffset, size);
                    DC_FlushRange(work->buf + work->writeOffset, size);
                } else {
                    int over = work->writeOffset + size - work->bufSize;
                    int n = size - over;

                    MIi_CpuCopy32(p, work->buf + work->writeOffset, n);
                    DC_FlushRange(work->buf + work->writeOffset, n);
                    if (sNFLWork->texFmt == 7) {
                        p += n / 4;
                    } else {
                        p += n;
                    }
                    MIi_CpuCopy32(p, work->buf, over);
                    DC_FlushRange(work->buf, over);
                }
            }
            work->writeOffset = (work->writeOffset + size) % work->bufSize;
            work->streams[6].vramAddr = sNFLWork->sharedTex[5].plttAddr + extra;
            *pos += size;
        }
    }
    if (*pos >= work->bufSize) {
        return TRUE;
    }
    if (work->loadList == NULL) {
        return TRUE;
    }
    switch (work->loadList->state) {
    case 0:
        NFLi_CopyFaceTextures(work->loadList, pos);
        break;
    case 1:
        NFLi_CopyFacePltts(work->loadList, pos);
        break;
    }
    return TRUE;
}

// What NFLi_CopyFaceTextures copies to each of a face's eleven texture streams: a resource category
// (NFLi_GetResourceItem's, 21 the colour texture) and its size
typedef struct NFLFaceStreamSource {
    int category;
    u32 size;
} NFLFaceStreamSource;

extern NFLFaceStreamSource sNFLFaceStreamSources[];

void NFLi_MakeColorTextureDirect(u16 *palette, const s16 *params);
void NFLi_MakeColorTexture(u16 *palette);

// Stage 2 of NFL_FillBuffer, a face in state 0: its eleven texture streams, then its palettes or, in mode 7, the next
// face of the load list.
// transparent is a u32: as a BOOL it took the highest of the spill slots where the original has it in the lowest
BOOL NFLi_CopyFaceTextures(NFLFace *obj, u32 *pos) {
    NFLStream *stream;
    NFLWork *work = sNFLWork;
    s16 *params = obj->keptFields;
    int i;
    u16 palette[64];

    for (i = 0; i < 11; i++) {
        int kind;
        int index;
        u16 *pal;
        u8 *src;
        int size;
        BOOL overflowB;
        BOOL overflowA;
        u32 transparent;

        stream = &obj->texStreams[i];
        if (*pos >= work->bufSize) {
            return TRUE;
        }
        if (!work->started) {
            if (stream->state >= 2 && stream->state <= 5) {
                work->started = TRUE;
            } else if (stream->state <= 1) {
                work->pending = 0;
                work->readOffset = 0;
                work->writeOffset = 0;
                *pos = 0;
                work->started = TRUE;
            }
        }
        if (stream->state == 0 || stream->state == 1 || stream->state == 3 || stream->state == 4) {
            kind = sNFLFaceStreamSources[i].category;
            index = stream->vramAddr;
            overflowB = FALSE;
            overflowA = FALSE;
            transparent = TRUE;
            pal = sNFLWork->texFmt == 7 ? palette : NULL;
            if (sNFLWork->texFmt == 7) {
                if (kind == 21) {
                    size = 0x80;
                } else {
                    size = sNFLFaceStreamSources[i].size * 4;
                }
            } else {
                size = sNFLFaceStreamSources[i].size;
            }
            switch (kind) {
            case 21:
                if (sNFLWork->texFmt == 7) {
                    NFLi_MakeColorTextureDirect(palette, params);
                } else {
                    NFLi_MakeColorTexture(palette);
                }
                if (stream->state == 0) {
                    if (*pos + size > work->bufSize) {
                        overflowA = TRUE;
                        size = work->bufSize - *pos;
                    }
                    if (work->writeOffset + size > work->bufSize) {
                        overflowB = TRUE;
                    }
                    if (!overflowB) {
                        MIi_CpuCopy32(palette, work->buf + work->writeOffset, size);
                        DC_FlushRange(work->buf + work->writeOffset, size);
                    } else {
                        int over = work->writeOffset + size - work->bufSize;
                        int n = size - over;

                        MIi_CpuCopy32(palette, work->buf + work->writeOffset, n);
                        DC_FlushRange(work->buf + work->writeOffset, n);
                        MIi_CpuCopy32((u8 *)palette + over, work->buf, over);
                        DC_FlushRange(work->buf, over);
                    }
                    if (!overflowA) {
                        stream->state = 2;
                    } else {
                        obj->copiedSize = size;
                        stream->state = 3;
                    }
                    stream->bufOffset = work->writeOffset;
                } else {
                    u8 *p = (u8 *)palette;

                    switch (stream->state) {
                    case 3:
                    case 4:
                        size -= obj->copiedSize;
                        p += obj->copiedSize;
                        break;
                    case 1:
                        p += obj->loadedSize;
                        size -= obj->loadedSize;
                        stream->bufOffset = work->writeOffset;
                        break;
                    }
                    if (*pos + size > work->bufSize) {
                        overflowA = TRUE;
                        size = work->bufSize - *pos;
                    }
                    if (work->writeOffset + size > work->bufSize) {
                        overflowB = TRUE;
                    }
                    if (!overflowA) {
                        stream->state = stream->state == 3 ? 2 : 5;
                        obj->copiedSize = 0;
                    } else {
                        switch (stream->state) {
                        case 3:
                        case 4:
                            obj->copiedSize += size;
                            break;
                        case 1:
                            obj->copiedSize = obj->loadedSize + size;
                            stream->state = 4;
                            break;
                        }
                    }
                    if (!overflowB) {
                        MIi_CpuCopy32(p, work->buf + work->writeOffset, size);
                        DC_FlushRange(work->buf + work->writeOffset, size);
                    } else {
                        int over = work->writeOffset + size - work->bufSize;
                        int n = size - over;

                        MIi_CpuCopy32(p, work->buf + work->writeOffset, n);
                        DC_FlushRange(work->buf + work->writeOffset, n);
                        MIi_CpuCopy32(p + n, work->buf, over);
                        DC_FlushRange(work->buf, over);
                    }
                }
                work->writeOffset = (work->writeOffset + size) % work->bufSize;
                *pos += size;
                break;
            case 2:
                if (sNFLWork->texFmt == 7) {
                    NFLi_MakeEyePltt(pal, params[8]);
                }
                break;
            case 3:
                if (sNFLWork->texFmt == 7) {
                    NFLi_MakeHairPltt(pal, params[6]);
                }
                break;
            case 13:
                if (sNFLWork->texFmt == 7) {
                    NFLi_MakeMouthPltt(pal, params[10]);
                }
                break;
            case 14:
                if (sNFLWork->texFmt == 7) {
                    NFLi_MakeHairPltt(pal, params[14]);
                }
                break;
            case 8:
                if (sNFLWork->texFmt == 7) {
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
                if (sNFLWork->texFmt == 7) {
                    NFLi_MakeHairPltt(pal, params[14]);
                }
                break;
            case 5:
                if (sNFLWork->texFmt == 7) {
                    NFLi_MakeFacePltt(pal, params[2]);
                }
                transparent = FALSE;
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
                    if (sNFLWork->texFmt == 7) {
                        NFLi_MakeFavoriteColorPltt(pal, params[0]);
                    }
                    kind = 19;
                    transparent = FALSE;
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
                    if (sNFLWork->texFmt == 7) {
                        NFLi_MakeHairPltt(pal, params[4]);
                    }
                    if (params[3] == 29) {
                        transparent = TRUE;
                    } else {
                        transparent = FALSE;
                    }
                }
                break;
            case 17:
                if (sNFLWork->texFmt == 7) {
                    pal = NFLi_GetResourceItem(12, 0);
                }
                break;
            }
            if (kind != 21) {
                if (stream->state == 0) {
                    u32 offset;

                    stream->bufOffset = work->writeOffset;
                    if (*pos + size > work->bufSize) {
                        overflowA = TRUE;
                        size = work->bufSize - *pos;
                    }
                    offset = work->writeOffset;
                    if (offset + size > work->bufSize) {
                        overflowB = TRUE;
                    }
                    if (!overflowB) {
                        src = NFLi_GetResourceItem(kind, index);
                        NFLi_CopyTexData(src, work->buf + offset, pal, size, transparent);
                        DC_FlushRange(work->buf + work->writeOffset, size);
                    } else {
                        int over = work->writeOffset + size - work->bufSize;
                        int n;

                        src = NFLi_GetResourceItem(kind, index);
                        n = size - over;
                        NFLi_CopyTexData(src, work->buf + work->writeOffset, pal, n, transparent);
                        DC_FlushRange(work->buf + work->writeOffset, n);
                        if (sNFLWork->texFmt == 7) {
                            src += n / 4;
                        } else {
                            src += n;
                        }
                        NFLi_CopyTexData(src, work->buf, pal, over, transparent);
                        DC_FlushRange(work->buf, over);
                    }
                    if (!overflowA) {
                        stream->state = 2;
                    } else {
                        obj->copiedSize = size;
                        stream->state = 3;
                    }
                } else {
                    u8 *p2 = NFLi_GetResourceItem(kind, index);

                    switch (stream->state) {
                    case 3:
                    case 4:
                        if (sNFLWork->texFmt == 7) {
                            p2 += (int)obj->copiedSize / 4;
                        } else {
                            p2 += obj->copiedSize;
                        }
                        size -= obj->copiedSize;
                        break;
                    case 1:
                        if (sNFLWork->texFmt == 7) {
                            p2 += (int)obj->loadedSize / 4;
                        } else {
                            p2 += obj->loadedSize;
                        }
                        size -= obj->loadedSize;
                        stream->bufOffset = work->writeOffset;
                        break;
                    }
                    if (*pos + size > work->bufSize) {
                        overflowA = TRUE;
                        size = work->bufSize - *pos;
                    }
                    if (work->writeOffset + size > work->bufSize) {
                        overflowB = TRUE;
                    }
                    if (!overflowB) {
                        NFLi_CopyTexData(p2, work->buf + work->writeOffset, pal, size, transparent);
                        DC_FlushRange(work->buf + work->writeOffset, size);
                    } else {
                        int over = work->writeOffset + size - work->bufSize;
                        int n = size - over;

                        NFLi_CopyTexData(p2, work->buf + work->writeOffset, pal, n, transparent);
                        DC_FlushRange(work->buf + work->writeOffset, n);
                        if (sNFLWork->texFmt == 7) {
                            p2 += n / 4;
                        } else {
                            p2 += n;
                        }
                        NFLi_CopyTexData(p2, work->buf, pal, over, transparent);
                        DC_FlushRange(work->buf, over);
                    }
                    if (!overflowA) {
                        stream->state = stream->state == 3 ? 2 : 5;
                        obj->copiedSize = 0;
                    } else {
                        switch (stream->state) {
                        case 3:
                        case 4:
                            obj->copiedSize += size;
                            break;
                        case 1:
                            obj->copiedSize = obj->loadedSize + size;
                            stream->state = 4;
                            break;
                        }
                    }
                }
                work->writeOffset = (work->writeOffset + size) % work->bufSize;
                *pos += size;
            }
        }
    }
    if (sNFLWork->texFmt == 7) {
        if (obj->nextLoad != NULL) {
            NFLi_CopyFaceTextures(obj->nextLoad, pos);
        }
    } else {
        NFLi_CopyFacePltts(obj, pos);
    }
    return TRUE;
}

void NFLi_MakeColorTexturePltt(u16 *palette, const s16 *params);

// Stage 2 of NFL_FillBuffer, a face in state 1: the palettes of its palette streams (plttStreams), made from its kept
// fields
BOOL NFLi_CopyFacePltts(NFLFace *obj, u32 *pos) {
    NFLWork *work = (NFLWork *)sNFLWork;
    s16 *params = obj->keptFields;

    if (work->texFmt != 7) {
        u16 palette[16];
        NFLStream *stream = obj->plttStreams;
        u16 *pal = palette;
        int i;

        for (i = 0; i < 11; stream++, i++) {
            int size;
            BOOL overflowB;
            BOOL overflowA;
            u8 *p;

            if (*pos >= work->bufSize) {
                return TRUE;
            }
            if (!work->started) {
                if (stream->state >= 2 && stream->state <= 5) {
                    work->started = TRUE;
                } else if (stream->state <= 1) {
                    work->pending = 0;
                    work->readOffset = 0;
                    work->writeOffset = 0;
                    *pos = 0;
                    work->started = TRUE;
                }
            }
            if (stream->state == 0 || stream->state == 1 || stream->state == 3 || stream->state == 4) {
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
                if (stream->state == 0) {
                    if (*pos + size > work->bufSize) {
                        overflowA = TRUE;
                        size = work->bufSize - *pos;
                    }
                    if (work->writeOffset + size > work->bufSize) {
                        overflowB = TRUE;
                    }
                    if (!overflowB) {
                        MIi_CpuCopy32(pal, work->buf + work->writeOffset, size);
                        DC_FlushRange(work->buf + work->writeOffset, size);
                    } else {
                        int over = work->writeOffset + size - work->bufSize;
                        int n = size - over;

                        MIi_CpuCopy32(pal, work->buf + work->writeOffset, n);
                        DC_FlushRange(work->buf + work->writeOffset, n);
                        MIi_CpuCopy32((u8 *)pal + n, work->buf, over);
                        DC_FlushRange(work->buf, over);
                    }
                    if (!overflowA) {
                        stream->state = 2;
                    } else {
                        obj->copiedSize = size;
                        stream->state = 3;
                    }
                    stream->bufOffset = work->writeOffset;
                } else {
                    p = (u8 *)pal;
                    switch (stream->state) {
                    case 3:
                    case 4:
                        p += obj->copiedSize;
                        size -= obj->copiedSize;
                        break;
                    case 1:
                        p = (u8 *)pal + obj->loadedSize;
                        size -= obj->loadedSize;
                        stream->bufOffset = work->writeOffset;
                        break;
                    }
                    if (*pos + size > work->bufSize) {
                        overflowA = TRUE;
                        size = work->bufSize - *pos;
                    }
                    if (work->writeOffset + size > work->bufSize) {
                        overflowB = TRUE;
                    }
                    if (!overflowA) {
                        stream->state = stream->state == 3 ? 2 : 5;
                        obj->copiedSize = 0;
                    } else {
                        switch (stream->state) {
                        case 3:
                        case 4:
                            obj->copiedSize += size;
                            break;
                        case 1:
                            obj->copiedSize = obj->loadedSize + size;
                            stream->state = 5;
                            break;
                        }
                    }
                    if (!overflowB) {
                        MIi_CpuCopy32(p, work->buf + work->writeOffset, size);
                        DC_FlushRange(work->buf + work->writeOffset, size);
                    } else {
                        int over = work->writeOffset + size - work->bufSize;
                        int n = size - over;

                        MIi_CpuCopy32(p, work->buf + work->writeOffset, n);
                        DC_FlushRange(work->buf + work->writeOffset, n);
                        MIi_CpuCopy32(p + n, work->buf, over);
                        DC_FlushRange(work->buf, over);
                    }
                }
                work->writeOffset = (work->writeOffset + size) % work->bufSize;
                *pos += size;
            }
        }
    }
    if (obj->nextLoad != NULL) {
        NFLi_CopyFaceTextures(obj->nextLoad, pos);
    }
    return TRUE;
}

// Takes obj out of the work area's loadList
void NFLi_RemoveFromLoadList(NFLFace *obj) {
    NFLWork *work = sNFLWork;
    NFLFace *prev = work->loadList;
    NFLFace *cur;

    if (prev == obj) {
        work->loadList = obj->nextLoad;
        obj->nextLoad = NULL;
        return;
    }
    for (cur = prev->nextLoad; cur != NULL; cur = cur->nextLoad) {
        if (cur == obj) {
            prev->nextLoad = obj->nextLoad;
            obj->nextLoad = NULL;
            return;
        }
        prev = cur;
    }
}

int NFLi_LoadSharedTextures(u32 *size);
int NFLi_LoadSharedPltt(u32 *size);
int NFLi_LoadFaceTextures(NFLFace *obj, u32 *size);
int NFLi_LoadFacePltts(NFLFace *obj, u32 *size);

// The library's work after the frame's interrupt, which main calls with 0x4000: 8 a size not a multiple of 4, 4 not
// set up, 0x13 and 0x14 busy, 2 and 3 nothing to do
int NFL_LoadVram(u32 size) {
    u32 rest = size;
    NFLWork *work;
    NFLFace *obj;
    int result;

    if (rest & 3) {
        return 8;
    }
    work = sNFLWork;
    if (work == NULL) {
        return 4;
    }
    if (work->phase == 1) {
        return 0x13;
    }
    if (work->phase == 3) {
        return 0x14;
    }
    if (work->phase == 0) {
        return 2;
    }
    if (work->phase == 4) {
        return 3;
    }
    work->phase = 3;
    switch (work->stage) {
    case 0:
        result = NFLi_LoadSharedTextures(&rest);
        if (result != 0 && result != 2) {
            work->stage = 3;
            work->phase = 4;
            return result;
        }
        if (result == 2) {
            work->phase = 0;
        } else {
            work->phase = 2;
        }
        break;
    case 1:
        result = NFLi_LoadSharedPltt(&rest);
        if (result != 0 && result != 2) {
            work->stage = 3;
            work->phase = 4;
            return result;
        }
        if (result == 2) {
            work->phase = 0;
        } else {
            work->phase = 2;
        }
        break;
    case 2:
        obj = work->loadList;
        if (obj == NULL) {
            work->phase = 4;
            return 2;
        }
        switch (obj->state) {
        case 0:
            result = NFLi_LoadFaceTextures(obj, &rest);
            if (result != 0 && result != 2) {
                obj->state = 3;
                NFLi_RemoveFromLoadList(work->loadList);
                work->phase = 0;
                return result;
            }
            if (result == 2) {
                work->phase = 0;
            } else {
                work->phase = 2;
            }
            break;
        case 1:
            result = NFLi_LoadFacePltts(obj, &rest);
            if (result != 0 && result != 2) {
                obj->state = 3;
                NFLi_RemoveFromLoadList(work->loadList);
                work->phase = 0;
                return result;
            }
            if (result == 2) {
                work->phase = 0;
            } else {
                work->phase = 2;
            }
            break;
        case 2:
        case 4:
            NFLi_RemoveFromLoadList(obj);
            work->phase = 0;
            break;
        }
        break;
    }
    if (result != 0) {
        return result;
    }
    return 1;
}

// Stage 0 of NFL_LoadVram, the VRAM side of NFLi_CopySharedTextures: loads up to *rest bytes of what the six shared
// streams have put in the ring buffer into texture VRAM (NFLi_LoadTex), then hands on to the faces' streams
// (NFLi_LoadFaceTextures) or the shared palette (NFLi_LoadSharedPltt)
int NFLi_LoadSharedTextures(u32 *rest) {
    NFLWork *work = (NFLWork *)sNFLWork;
    u32 dest;
    NFLStream *stream = work->streams;
    NFLSharedStreamSource *source = sNFLSharedStreamSources;
    int i;

    for (i = 0; i < 6; stream++, source++, i++) {
        BOOL go = FALSE;
        u8 *src;
        u32 size;
        int result;

        switch (stream->state) {
        case 2:
            if (sNFLWork->texFmt == 7) {
                size = source->size * 4;
            } else {
                size = source->size;
            }
            src = (u8 *)stream->bufOffset + (u32)work->buf;
            dest = stream->vramAddr;
            DC_FlushRange(src, size);
            go = TRUE;
            break;
        case 3:
            size = work->copiedSize;
            src = (u8 *)stream->bufOffset + (u32)work->buf;
            dest = stream->vramAddr;
            DC_FlushRange(src, size);
            go = TRUE;
            break;
        case 4:
            size = work->copiedSize - work->loadedSize;
            src = (u8 *)stream->bufOffset + (u32)work->buf;
            dest = stream->vramAddr;
            DC_FlushRange(src, size);
            go = TRUE;
            break;
        case 5:
            if (sNFLWork->texFmt == 7) {
                size = source->size * 4;
            } else {
                size = source->size;
            }
            size -= work->loadedSize;
            src = (u8 *)stream->bufOffset + (u32)work->buf;
            dest = stream->vramAddr;
            DC_FlushRange(src, size);
            go = TRUE;
            break;
        case 0:
        case 1:
            work->pending = 0;
            work->readOffset = 0;
            work->writeOffset = 0;
            return 2;
        }
        if (*rest == 0) {
            return 0;
        }
        if (go == TRUE) {
            if (*rest >= size) {
                if (stream->bufOffset + size <= work->bufSize) {
                    result = NFLi_LoadTex(src, dest, size);
                } else {
                    int over = stream->bufOffset + size - work->bufSize;
                    int n = size - over;

                    result = NFLi_LoadTex(src, dest, n);
                    if (result == 0) {
                        result = NFLi_LoadTex(work->buf, dest + n, over);
                    }
                }
                if (result != 0) {
                    stream->state = 6;
                    return result;
                }
                work->readOffset = (stream->bufOffset + size) % work->bufSize;
                if (work->writeOffset >= work->readOffset) {
                    work->pending = work->writeOffset - work->readOffset;
                } else {
                    work->pending = work->bufSize + work->writeOffset - work->readOffset;
                }
                if (stream->state == 2) {
                    stream->state = 7;
                } else if (stream->state == 5) {
                    stream->state = 7;
                    work->loadedSize = 0;
                } else {
                    stream->state = 1;
                    work->loadedSize = work->copiedSize;
                    work->copiedSize = 0;
                }
                *rest -= size;
            } else {
                if (stream->bufOffset + *rest <= work->bufSize) {
                    result = NFLi_LoadTex(src, dest, *rest);
                } else {
                    int over = stream->bufOffset + *rest - work->bufSize;
                    int n = *rest - over;

                    result = NFLi_LoadTex(src, dest, n);
                    if (result == 0) {
                        result = NFLi_LoadTex(work->buf, dest + n, over);
                    }
                }
                if (result != 0) {
                    stream->state = 6;
                    return result;
                }
                work->readOffset = (stream->bufOffset + *rest) % work->bufSize;
                if (work->writeOffset >= work->readOffset) {
                    work->pending = work->writeOffset - work->readOffset;
                } else {
                    work->pending = work->bufSize + work->writeOffset - work->readOffset;
                }
                if (stream->state == 3 || stream->state == 4) {
                    stream->state = 4;
                } else {
                    stream->state = 5;
                }
                work->loadedSize += *rest;
                stream->bufOffset = (stream->bufOffset + *rest) % work->bufSize;
                stream->vramAddr += *rest;
                *rest = 0;
                return 0;
            }
        }
    }
    if (sNFLWork->texFmt == 7) {
        work->stage = 2;
        if (*rest == 0) {
            return 2;
        }
        if (work->loadList != NULL) {
            switch (work->loadList->state) {
            case 0:
                return NFLi_LoadFaceTextures(work->loadList, rest);
            case 2:
            case 4:
                NFLi_RemoveFromLoadList(work->loadList);
                break;
            }
        }
    } else {
        work->stage = 1;
        if (*rest == 0) {
            return 2;
        }
        return NFLi_LoadSharedPltt(rest);
    }
    return 2;
}

// Stage 1 of NFL_LoadVram, the VRAM side of NFLi_CopySharedPltt: loads the shared palette into texture palette VRAM
// (NFLi_LoadTexPltt), then hands on to the faces' streams
int NFLi_LoadSharedPltt(u32 *rest) {
    NFLWork *work = (NFLWork *)sNFLWork;
    BOOL go = FALSE;
    u32 dest;
    int size;
    u8 *src;
    int result;

    if (work->texFmt == 7) {
        return 0;
    }
    switch (work->streams[6].state) {
    case 2:
        dest = work->streams[6].vramAddr;
        src = (u8 *)work->streams[6].bufOffset + (u32)work->buf;
        size = 32;
        DC_FlushRange(src, size);
        go = TRUE;
        break;
    case 3:
        dest = work->streams[6].vramAddr;
        src = (u8 *)work->streams[6].bufOffset + (u32)work->buf;
        size = work->copiedSize;
        DC_FlushRange(src, size);
        go = TRUE;
        break;
    case 4:
        size = work->copiedSize - work->loadedSize;
        src = (u8 *)work->streams[6].bufOffset + (u32)work->buf;
        dest = work->streams[6].vramAddr;
        DC_FlushRange(src, size);
        go = TRUE;
        break;
    case 5:
        size = 32 - work->loadedSize;
        src = (u8 *)work->streams[6].bufOffset + (u32)work->buf;
        dest = work->streams[6].vramAddr;
        DC_FlushRange(src, size);
        go = TRUE;
        break;
    case 0:
    case 1:
        work->pending = go;
        work->readOffset = go;
        work->writeOffset = go;
        return 2;
    }
    if (*rest == 0) {
        return 0;
    }
    if (go == TRUE) {
        if (*rest >= size) {
            if (work->streams[6].bufOffset + size <= work->bufSize) {
                result = NFLi_LoadTexPltt(src, dest, size);
            } else {
                u32 over = work->streams[6].bufOffset + size - work->bufSize;
                u32 n = size - over;

                result = NFLi_LoadTexPltt(src, dest, n);
                if (result == 0) {
                    result = NFLi_LoadTexPltt(work->buf, dest + n, over);
                }
            }
            if (result != 0) {
                work->streams[6].state = 6;
                return result;
            }
            work->readOffset = (work->streams[6].bufOffset + size) % work->bufSize;
            if (work->writeOffset >= work->readOffset) {
                work->pending = work->writeOffset - work->readOffset;
            } else {
                work->pending = work->bufSize + work->writeOffset - work->readOffset;
            }
            if (work->streams[6].state == 2) {
                work->streams[6].state = 7;
            } else if (work->streams[6].state == 5) {
                work->streams[6].state = 7;
                work->loadedSize = 0;
            } else {
                work->streams[6].state = 1;
                work->loadedSize = work->copiedSize;
                work->copiedSize = 0;
            }
            *rest -= size;
        } else {
            if (work->streams[6].bufOffset + *rest <= (u32)(work->buf + work->bufSize)) {
                result = NFLi_LoadTexPltt(work->buf + work->streams[6].bufOffset, work->streams[6].vramAddr, *rest);
            } else {
                u32 over = work->streams[6].bufOffset + *rest - work->bufSize;
                u32 n = *rest - over;

                result = NFLi_LoadTexPltt(work->buf + work->streams[6].bufOffset, work->streams[6].vramAddr, n);
                if (result == 0) {
                    result = NFLi_LoadTexPltt(work->buf, work->streams[6].vramAddr + n, over);
                }
            }
            if (result != 0) {
                work->streams[6].state = 6;
                return result;
            }
            work->readOffset = (work->streams[6].bufOffset + *rest) % work->bufSize;
            if (work->writeOffset >= work->readOffset) {
                work->pending = work->writeOffset - work->readOffset;
            } else {
                work->pending = work->bufSize + work->writeOffset - work->readOffset;
            }
            if (work->streams[6].state == 3 || work->streams[6].state == 4) {
                work->streams[6].state = 4;
            } else {
                work->streams[6].state = 5;
            }
            work->loadedSize += *rest;
            work->streams[6].bufOffset = (work->streams[6].bufOffset + *rest) % work->bufSize;
            work->streams[6].vramAddr += *rest;
            *rest = 0;
            return 0;
        }
    }
    work->stage = 2;
    if (*rest == 0) {
        return 2;
    }
    if (work->loadList != NULL) {
        switch (work->loadList->state) {
        case 0:
            return NFLi_LoadFaceTextures(work->loadList, rest);
        case 1:
            return NFLi_LoadFacePltts(work->loadList, rest);
        case 2:
        case 4:
            NFLi_RemoveFromLoadList(work->loadList);
            break;
        }
    }
    return 2;
}

// Where each of a face's eleven texture streams goes in texture VRAM, from the face's base (texBase): the first offset
// in the other modes, the second in mode 7
typedef struct NFLFaceTexOffset {
    u32 offset;
    u32 directOffset;
} NFLFaceTexOffset;

extern const NFLFaceTexOffset sNFLFaceTexOffsets[];

// Stage 2 of NFL_LoadVram, a face in state 0: the VRAM side of its eleven texture streams (NFLi_CopyFaceTextures)
int NFLi_LoadFaceTextures(NFLFace *obj, u32 *rest) {
    NFLWork *work = sNFLWork;
    int i;
    u32 dest;
    NFLStream *stream = obj->texStreams;
    int result;

    for (i = 0; i < 11; stream++, i++) {
        BOOL go = FALSE;
        u8 *src;
        u32 size;

        switch (stream->state) {
        case 2:
            if (sNFLWork->texFmt == 7) {
                if (i == 0) {
                    size = 0x80;
                } else {
                    size = sNFLFaceStreamSources[i].size * 4;
                }
                dest = obj->texBase + sNFLFaceTexOffsets[i].directOffset;
            } else {
                size = sNFLFaceStreamSources[i].size;
                dest = obj->texBase + sNFLFaceTexOffsets[i].offset;
            }
            src = (u8 *)stream->bufOffset + (u32)work->buf;
            go = TRUE;
            break;
        case 3:
            size = obj->copiedSize;
            src = (u8 *)stream->bufOffset + (u32)work->buf;
            if (sNFLWork->texFmt == 7) {
                dest = obj->texBase + sNFLFaceTexOffsets[i].directOffset;
            } else {
                dest = obj->texBase + sNFLFaceTexOffsets[i].offset;
            }
            go = TRUE;
            break;
        case 4:
            size = obj->copiedSize - obj->loadedSize;
            src = (u8 *)stream->bufOffset + (u32)work->buf;
            if (sNFLWork->texFmt == 7) {
                dest = obj->loadedSize + (obj->texBase + sNFLFaceTexOffsets[i].directOffset);
            } else {
                dest = obj->loadedSize + (obj->texBase + sNFLFaceTexOffsets[i].offset);
            }
            go = TRUE;
            break;
        case 5:
            if (sNFLWork->texFmt == 7) {
                if (i == 0) {
                    size = 0x80;
                } else {
                    size = sNFLFaceStreamSources[i].size * 4;
                }
                dest = obj->loadedSize + (obj->texBase + sNFLFaceTexOffsets[i].directOffset);
            } else {
                size = sNFLFaceStreamSources[i].size;
                dest = obj->loadedSize + (obj->texBase + sNFLFaceTexOffsets[i].offset);
            }
            size -= obj->loadedSize;
            src = (u8 *)stream->bufOffset + (u32)work->buf;
            go = TRUE;
            break;
        case 0:
        case 1:
            work->pending = go;
            work->readOffset = go;
            work->writeOffset = go;
            return 2;
        }
        if (*rest == 0) {
            return 0;
        }
        if (go == TRUE) {
            if (*rest >= size) {
                if (stream->bufOffset + size <= work->bufSize) {
                    result = NFLi_LoadTex(src, dest, size);
                } else {
                    int over = stream->bufOffset + size - work->bufSize;
                    int n = size - over;

                    result = NFLi_LoadTex(src, dest, n);
                    if (result == 0) {
                        result = NFLi_LoadTex(work->buf, dest + n, over);
                    }
                }
                if (result != 0) {
                    stream->state = 6;
                    return result;
                }
                work->readOffset = (stream->bufOffset + size) % work->bufSize;
                if (work->writeOffset >= work->readOffset) {
                    work->pending = work->writeOffset - work->readOffset;
                } else {
                    work->pending = work->bufSize + work->writeOffset - work->readOffset;
                }
                if (stream->state == 2) {
                    stream->state = 7;
                } else if (stream->state == 5) {
                    stream->state = 7;
                    obj->loadedSize = 0;
                } else {
                    stream->state = 1;
                    obj->loadedSize = obj->copiedSize;
                    obj->copiedSize = 0;
                    *rest -= size;
                    return 2;
                }
                *rest -= size;
            } else {
                if (stream->bufOffset + *rest <= work->bufSize) {
                    result = NFLi_LoadTex(src, dest, *rest);
                } else {
                    int over = stream->bufOffset + *rest - work->bufSize;
                    int n = *rest - over;

                    result = NFLi_LoadTex(src, dest, n);
                    if (result == 0) {
                        result = NFLi_LoadTex(work->buf, dest + n, over);
                    }
                }
                if (result != 0) {
                    stream->state = 6;
                    return result;
                }
                work->readOffset = (stream->bufOffset + *rest) % work->bufSize;
                if (work->writeOffset >= work->readOffset) {
                    work->pending = work->writeOffset - work->readOffset;
                } else {
                    work->pending = work->bufSize + work->writeOffset - work->readOffset;
                }
                if (stream->state == 3 || stream->state == 4) {
                    stream->state = 4;
                } else {
                    stream->state = 5;
                }
                obj->loadedSize += *rest;
                stream->bufOffset = (stream->bufOffset + *rest) % work->bufSize;
                *rest = 0;
                return 0;
            }
        }
    }
    result = 2;
    if (sNFLWork->texFmt == 7) {
        if (obj->nextLoad != NULL && *rest != 0) {
            result = NFLi_LoadFaceTextures(obj->nextLoad, rest);
            if (result != 0 && result != 2) {
                return result;
            }
        }
        if (obj->state != 4) {
            obj->state = 2;
        }
        NFLi_RemoveFromLoadList(obj);
    } else {
        obj->state = 1;
        if (*rest == 0) {
            return result;
        }
        result = NFLi_LoadFacePltts(obj, rest);
        return result;
    }
    return result;
}

// Where each of a face's palette streams goes in texture palette VRAM, from the face's base (plttBase)
extern const u32 sNFLFacePlttOffsets[];

// Stage 2 of NFL_LoadVram, a face in state 1: the VRAM side of its palette streams (NFLi_CopyFacePltts), ten of them
// here, where NFLi_CopyFacePltts fills eleven
int NFLi_LoadFacePltts(NFLFace *obj, u32 *rest) {
    NFLWork *work = (NFLWork *)sNFLWork;
    int result;

    if (work->texFmt != 7) {
        int i;
        u8 *src;
        NFLStream *stream = obj->plttStreams;

        for (i = 0; i < 10; stream++, i++) {
            BOOL go = FALSE;
            u32 dest;
            u32 size;

            switch (stream->state) {
            case 2:
                dest = obj->plttBase + sNFLFacePlttOffsets[i];
                if (i == 0) {
                    size = 16;
                } else {
                    size = 32;
                }
                src = (u8 *)stream->bufOffset + (u32)work->buf;
                DC_FlushRange(src, size);
                go = TRUE;
                break;
            case 3:
                src = (u8 *)stream->bufOffset + (u32)work->buf;
                size = obj->copiedSize;
                dest = obj->plttBase + sNFLFacePlttOffsets[i];
                DC_FlushRange(src, size);
                go = TRUE;
                break;
            case 4:
                size = obj->copiedSize - obj->loadedSize;
                src = (u8 *)stream->bufOffset + (u32)work->buf;
                dest = obj->loadedSize + (obj->plttBase + sNFLFacePlttOffsets[i]);
                DC_FlushRange(src, size);
                go = TRUE;
                break;
            case 5:
                if (i == 0) {
                    size = 16 - obj->loadedSize;
                } else {
                    size = 32 - obj->loadedSize;
                }
                src = (u8 *)stream->bufOffset + (u32)work->buf;
                dest = obj->loadedSize + (obj->plttBase + sNFLFacePlttOffsets[i]);
                DC_FlushRange(src, size);
                go = TRUE;
                break;
            case 0:
            case 1:
                work->pending = go;
                work->readOffset = go;
                work->writeOffset = go;
                return 2;
            }
            if (*rest == 0) {
                return 0;
            }
            if (go == TRUE) {
                if (*rest >= size) {
                    if (stream->bufOffset + size <= work->bufSize) {
                        result = NFLi_LoadTexPltt(src, dest, size);
                    } else {
                        int over = stream->bufOffset + size - work->bufSize;
                        int n = size - over;

                        result = NFLi_LoadTexPltt(src, dest, n);
                        if (result == 0) {
                            result = NFLi_LoadTexPltt(work->buf, dest + n, over);
                        }
                    }
                    if (result != 0) {
                        stream->state = 6;
                        return result;
                    }
                    work->readOffset = (stream->bufOffset + size) % work->bufSize;
                    if (work->writeOffset >= work->readOffset) {
                        work->pending = work->writeOffset - work->readOffset;
                    } else {
                        work->pending = work->bufSize + work->writeOffset - work->readOffset;
                    }
                    if (stream->state == 2) {
                        stream->state = 7;
                    } else if (stream->state == 5) {
                        stream->state = 7;
                        obj->loadedSize = 0;
                    } else {
                        stream->state = 1;
                        obj->loadedSize = obj->copiedSize;
                        obj->copiedSize = 0;
                    }
                    *rest -= size;
                } else {
                    if (stream->bufOffset + *rest <= work->bufSize) {
                        result = NFLi_LoadTexPltt(src, dest, *rest);
                    } else {
                        int over = stream->bufOffset + *rest - work->bufSize;
                        int n = *rest - over;

                        result = NFLi_LoadTexPltt(src, dest, n);
                        if (result == 0) {
                            result = NFLi_LoadTexPltt(work->buf, dest + n, over);
                        }
                    }
                    if (result != 0) {
                        stream->state = 6;
                        return result;
                    }
                    work->readOffset = (stream->bufOffset + *rest) % work->bufSize;
                    if (work->writeOffset >= work->readOffset) {
                        work->pending = work->writeOffset - work->readOffset;
                    } else {
                        work->pending = work->bufSize + work->writeOffset - work->readOffset;
                    }
                    if (stream->state == 3 || stream->state == 4) {
                        stream->state = 4;
                    } else {
                        stream->state = 5;
                    }
                    obj->loadedSize += *rest;
                    stream->bufOffset = (stream->bufOffset + *rest) % work->bufSize;
                    *rest = 0;
                    return 0;
                }
            }
        }
    }
    result = 2;
    if (obj->nextLoad != NULL && *rest != 0) {
        result = NFLi_LoadFaceTextures(obj->nextLoad, rest);
        if (result != 0 && result != 2) {
            return result;
        }
    }
    if (obj->state != 4) {
        obj->state = 2;
    }
    NFLi_RemoveFromLoadList(obj);
    return result;
}

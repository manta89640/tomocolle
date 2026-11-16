// NFL, the Mii face library: the main program's code from the end of NitroSystem (0x02069BF4) to the Mii transfer
// (0x020776E4), with its data. This one file holds the library's whole range, which is not yet divided into the
// files it was built from (docs/binary.md, "The Mii library").

#include "NFL.h"

NFLWork *data_02145360;

// The reader of the library's resource streams: one byte, advancing the stream
u8 func_02069bf4(const u8 **stream) {
    const u8 *p = *stream;
    u8 value = *p++;
    *stream = p;
    return value;
}

u16 func_02069c08(const u8 **stream) {
    u8 lo = func_02069bf4(stream);
    u8 hi = func_02069bf4(stream);
    return lo | (hi << 8);
}

s16 func_02069c30(const u8 **stream) {
    u8 lo = func_02069bf4(stream);
    u8 hi = func_02069bf4(stream);
    return lo | (hi << 8);
}

u32 func_02069c58(const u8 **stream) {
    u8 b0 = func_02069bf4(stream);
    u8 b1 = func_02069bf4(stream);
    u8 b2 = func_02069bf4(stream);
    u8 b3 = func_02069bf4(stream);
    return b0 | ((b1 << 8) | ((b3 << 24) | (b2 << 16)));
}

s32 func_02069c9c(const u8 **stream) {
    u8 b0 = func_02069bf4(stream);
    u8 b1 = func_02069bf4(stream);
    u8 b2 = func_02069bf4(stream);
    u8 b3 = func_02069bf4(stream);
    return b0 | ((b1 << 8) | ((b3 << 24) | (b2 << 16)));
}

// A count, then three halfwords for each
u16 func_02069ce0(const u8 **stream, s16 *out) {
    int i;
    u16 count = func_02069c08(stream);
    int n = count * 3;

    for (i = 0; i < n; i++) {
        out[i] = func_02069c30(stream);
    }
    return count;
}

// A count, then two halfwords for each
u16 func_02069d28(const u8 **stream, s16 *out) {
    int i;
    u16 count = func_02069c08(stream);
    int n = count * 2;

    for (i = 0; i < n; i++) {
        out[i] = func_02069c30(stream);
    }
    return count;
}

int func_02069d70(const u8 **stream) {
    int type = 2;

    switch (func_02069bf4(stream)) {
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

extern const NFLColor data_020b6678[];
extern const NFLColor data_020b65e8[];
extern const NFLColor data_020b6630[];

#define NFL_RGB(r, g, b) GX_RGB((r) >> 3, (g) >> 3, (b) >> 3)

// A palette of 16: colour index of data_020b6678, then darker by 5% a step down to 60%
void func_02069de4(u16 *palette, int index) {
    u32 r = data_020b6678[index].r;
    u32 g = data_020b6678[index].g;
    u32 b = data_020b6678[index].b;

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

// A palette of 16: colour index of data_020b65e8, 70% and 30% of it, 30% and 70% of the way to white, then fixed
// colours
void func_0206a014(u16 *palette, int index) {
    u32 r = data_020b65e8[index].r;
    u32 g = data_020b65e8[index].g;
    u32 b = data_020b65e8[index].b;

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

// A palette of 16: the colours 2 * index and 2 * index + 1 of data_020b6630 and their mean, then fixed colours
void func_0206a1b4(u16 *palette, int index) {
    u32 r0 = data_020b6630[index * 2].r;
    u32 g0 = data_020b6630[index * 2].g;
    u32 b0 = data_020b6630[index * 2].b;
    u32 r1 = data_020b6630[index * 2 + 1].r;
    u32 g1 = data_020b6630[index * 2 + 1].g;
    u32 b1 = data_020b6630[index * 2 + 1].b;

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
u32 func_0206a2a4(u32 from, u32 to, u32 rest, u32 weight) {
    if (from < to) {
        return from + (to - from) * weight / (rest + weight);
    }
    return from - (from - to) * weight / (rest + weight);
}

extern const NFLColor data_020b65a0[];

#define NFL_LERP_RGB(r, g, b, r2, g2, b2, rest, weight)                                                              \
    NFL_RGB(func_0206a2a4(r, r2, rest, weight), func_0206a2a4(g, g2, rest, weight), func_0206a2a4(b, b2, rest, weight))

// A palette of 16: colour index of data_020b65a0, and four ramps from it to four colours made from it
void func_0206a2e4(u16 *palette, int index) {
    u32 r = data_020b65a0[index].r;
    u32 g = data_020b65a0[index].g;
    u32 b = data_020b65a0[index].b;
    u32 r1 = func_0206a2a4(r, 0, 3, 1);
    u32 g1 = func_0206a2a4(g, 0, 3, 1);
    u32 b1 = func_0206a2a4(b, 0, 3, 1);
    u32 r2 = func_0206a2a4(r, 0, 3, 1);
    u32 g2 = func_0206a2a4(g, 0xFF, 3, 1);
    u32 b2 = func_0206a2a4(b, 0xDE, 3, 1);
    u32 r3 = func_0206a2a4(r, 0xFF, 3, 1);
    u32 g3 = func_0206a2a4(g, 0x92, 3, 1);
    u32 b3 = func_0206a2a4(b, 0x88, 3, 1);
    u32 r4 = func_0206a2a4(r, 0x55, 3, 2);
    u32 g4 = func_0206a2a4(g, 0x6D, 3, 2);
    u32 b4 = func_0206a2a4(b, 0x66, 3, 2);

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

extern const NFLColor data_020b66d8[];

// A palette of 16: colour index of data_020b66d8, then 84%, 68% and 50% of it
void func_0206a894(u16 *palette, int index) {
    u32 r = data_020b66d8[index].r;
    u32 g = data_020b66d8[index].g;
    u32 b = data_020b66d8[index].b;

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

extern const NFLColor data_020b6558[];

// A palette of 16: colour index of data_020b6558, half of it, and 50% and 25% of the way to white
void func_0206a99c(u16 *palette, int index) {
    u32 r = data_020b6558[index].r;
    u32 g = data_020b6558[index].g;
    u32 b = data_020b6558[index].b;

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
int func_0206aaec(NFLMakeCtx *ctx) {
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
int func_0206abb4(NFLMakeCtx *ctx) {
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
int func_0206acc0(NFLMakeCtx *ctx) {
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
int func_0206ae14(NFLMakeCtx *ctx) {
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
int func_0206af1c(NFLMakeCtx *ctx) {
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
int func_0206b024(NFLMakeCtx *ctx) {
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
int func_0206b0d8(NFLMakeCtx *ctx) {
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

    stream = func_0206cf6c(0, type);
    s = type > 1 ? GX_TEXSIZE_S32 : GX_TEXSIZE_S8;
    t = type > 1 ? GX_TEXSIZE_T32 : GX_TEXSIZE_T8;
    if (func_02069ce0(&stream, vertices) == 0) {
        return 0;
    }
    func_02069ce0(&stream, normals);
    func_02069d28(&stream, texCoords);
    polygons = func_02069bf4(&stream);
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
    func_02074444(obj, &translation, &position);
    G3C_Translate(&ctx->unk64, translation.x, translation.y, translation.z);
    for (i = 0; i < polygons; i++) {
        count = func_02069bf4(&stream);
        func_02005124(&ctx->unk64, func_02069d70(&stream));
        if (type < 2) {
            G3C_TexCoord(&ctx->unk64, 0, 0x8000);
        }
        for (j = 0; j < count; j++) {
            v = func_02069bf4(&stream) * 3;
            n = func_02069bf4(&stream) * 3;
            tc = func_02069bf4(&stream) * 2;
            if (type > 1) {
                G3C_TexCoord(&ctx->unk64, texCoords[tc] << 5, texCoords[tc + 1] << 5);
            }
            func_020743e4(obj, &normal, &normals[n]);
            G3C_Normal(&ctx->unk64, normal.x, normal.y, normal.z);
            func_020743e4(obj, &vertex, &vertices[v]);
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
int func_0206b3f4(NFLMakeCtx *ctx) {
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

    stream = func_0206cf6c(4, mii->faceType);
    s = mii->faceTexture != 0 ? GX_TEXSIZE_S32 : GX_TEXSIZE_S8;
    t = mii->faceTexture != 0 ? GX_TEXSIZE_T64 : GX_TEXSIZE_T8;
    ctx->unk0[0].x = func_02069c9c(&stream);
    ctx->unk0[0].y = func_02069c9c(&stream);
    ctx->unk0[0].z = func_02069c9c(&stream);
    ctx->unk0[1].x = func_02069c9c(&stream);
    ctx->unk0[1].y = func_02069c9c(&stream);
    ctx->unk0[1].z = func_02069c9c(&stream);
    ctx->unk0[2].x = func_02069c9c(&stream);
    ctx->unk0[2].y = func_02069c9c(&stream);
    ctx->unk0[2].z = func_02069c9c(&stream);
    if (func_02069ce0(&stream, vertices) == 0) {
        return 0;
    }
    func_02069ce0(&stream, normals);
    func_02069d28(&stream, texCoords);
    polygons = func_02069bf4(&stream);
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
        count = func_02069bf4(&stream);
        func_02005124(&ctx->unk64, func_02069d70(&stream));
        if (mii->faceTexture == 0) {
            G3C_TexCoord(&ctx->unk64, 0x8000, 0);
        }
        for (j = 0; j < count; j++) {
            v = func_02069bf4(&stream) * 3;
            n = func_02069bf4(&stream) * 3;
            tc = func_02069bf4(&stream) * 2;
            if (mii->faceTexture != 0) {
                G3C_TexCoord(&ctx->unk64, texCoords[tc] << 5, texCoords[tc + 1] << 6);
            }
            func_020743e4(obj, &normal, &normals[n]);
            G3C_Normal(&ctx->unk64, normal.x, normal.y, normal.z);
            func_020743e4(obj, &vertex, &vertices[v]);
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
int func_0206b740(NFLMakeCtx *ctx) {
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

    stream = func_0206cf6c(0x12, mii->hairType);
    bald = mii->hairType == 0x22 || mii->hairType == 0x39;
    s = bald ? GX_TEXSIZE_S32 : GX_TEXSIZE_S8;
    t = bald ? GX_TEXSIZE_T32 : GX_TEXSIZE_T8;
    obj->unk94[1].cmd = NULL;
    obj->unk94[1].param = NULL;
    obj->unk94[1].unk8 = 0;
    if (func_02069ce0(&stream, vertices) == 0) {
        return 0;
    }
    func_02069ce0(&stream, normals);
    func_02069d28(&stream, texCoords);
    polygons = func_02069bf4(&stream);
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
    func_02074444(obj, &translation, &position);
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
        count = func_02069bf4(&stream);
        func_02005124(&ctx->unk64, func_02069d70(&stream));
        if (!bald) {
            G3C_TexCoord(&ctx->unk64, 0x8000, 0x8000);
        }
        for (j = 0; j < count; j++) {
            v = func_02069bf4(&stream) * 3;
            n = func_02069bf4(&stream) * 3;
            tc = func_02069bf4(&stream) * 2;
            if (bald) {
                G3C_TexCoord(&ctx->unk64, texCoords[tc] << 5, texCoords[tc + 1] << 5);
            }
            func_020743e4(obj, &normal, &normals[n]);
            G3C_Normal(&ctx->unk64, normal.x, normal.y, normal.z);
            func_020743e4(obj, &vertex, &vertices[v]);
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

extern const u16 data_020b6524[];
extern const u16 data_020b6530[];
extern const u16 data_020b6540[];

void func_0206cc30(u16 *palette) {
    int i;

    for (i = 0; i < 4; i++) {
        palette[i] = 0x5500;
        palette[i + 4] = 0xFFAA;
    }
    DC_FlushRange(palette, 0x10);
}

void func_0206cc6c(u16 *palette, const s16 *params) {
    palette[0] = data_020b6530[params[4]];
    palette[1] = data_020b6524[params[2]];
    palette[2] = data_020b6530[params[14]];
    palette[3] = data_020b6540[params[0]];
    DC_FlushRange(palette, 0x10);
}

void func_0206cd9c(u16 *palette, const s16 *params) {
    int i;
    int j;

    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
            palette[i * 8 + j] = data_020b6530[params[4]];
        }
        for (j = 4; j < 8; j++) {
            palette[i * 8 + j] = data_020b6524[params[2]];
        }
    }
    for (i = 4; i < 8; i++) {
        for (j = 0; j < 4; j++) {
            palette[i * 8 + j] = data_020b6530[params[14]];
        }
        for (j = 4; j < 8; j++) {
            palette[i * 8 + j] = data_020b6540[params[0]];
        }
    }
    DC_FlushRange(palette, 0x80);
}

int func_0206ccd4(NFLMakeCtx *ctx) {
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

int func_0206cea8(NFLMakeCtx *ctx) {
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

int func_0206cf48(NFLMakeCtx *ctx) {
    if (ctx->unk1C8->unk90 == 7) {
        return func_0206cea8(ctx);
    }
    return func_0206ccd4(ctx);
}

// The stream of item index of category in the library's resource
const u8 *func_0206cf6c(int category, u32 index) {
    const u8 *header = data_02145360->unk64;
    const u8 *items;
    u32 offsets[20];
    int i;
    u16 count;
    u32 offset;

    func_02069c08(&header);
    func_02069c08(&header);
    for (i = 0; i < 20; i++) {
        offsets[i] = func_02069c58(&header);
    }
    items = data_02145360->unk64 + offsets[category];
    count = func_02069c08(&items);
    func_02069c08(&items);
    offset = 0;
    if (count < index) {
        return NULL;
    }
    for (i = 0; i < count; i++) {
        u32 value = func_02069c58(&items);
        if (i == index) {
            offset = value;
        }
    }
    return items + offset;
}

// Whether every field of a Mii's data is in its range
BOOL func_0206d040(const NFLCharData *mii) {
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

void func_0206d374(NFLObj *obj) {
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
void func_0206d3cc(NFLObj *obj, const NFLCharData *mii) {
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

extern int (*const data_02112140[])(NFLMakeCtx *ctx);
extern const NFLAxes data_0211215c;

// Makes object index's display lists from a Mii's data
#pragma optimization_level 2
#pragma opt_common_subs off
int func_0206d4f8(int index, const NFLCharData *mii, u32 unk) {
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
    if (!func_02072c1c()) {
        return 4;
    }
    if (!func_020735dc() || !func_020735fc()) {
        return 4;
    }
    if (data_02145360->unk8C == 1 || data_02145360->unk8C == 3) {
        return 6;
    }
    obj = func_02073060(index);
    if (obj == NULL) {
        return 9;
    }
    entries = obj->unk1A4;
    entries2 = obj->unk228;
    if (!func_0206d040(mii) || !func_020731e0(mii)) {
        return 7;
    }
    ctx.unk1C8 = obj;
    ctx.unk1CC = mii;
    obj->unk90 = data_02145360->unk68;
    obj->unk168 = 0;
    obj->unk16C = data_02145360->unk71;
    obj->unk0 = 0;
    obj->unk4 = 0;
    obj->unk28 = unk;
    obj->unkDC = mii;
    obj->unk174 = 0;
    obj->unk178 = 0;
    if (obj->unk170 == 4) {
        if (obj->unk90 == 7) {
            obj->unkD4 = ctx.unk24 = data_02145360->unk8 + index * 0x6880;
            obj->unkD8 = ctx.unk28 = data_02145360->unkC;
        } else {
            obj->unkD4 = ctx.unk24 = data_02145360->unk8 + index * 0x1A10;
            obj->unkD8 = ctx.unk28 = data_02145360->unkC + index * 0x130;
        }
        for (i = 0; i < 11; i++) {
            entries[i].value = -1;
            entries[i].state = 7;
        }
        for (i = 0; i < 10; i++) {
            entries2[i].value = -1;
            entries2[i].state = 7;
        }
        func_0206d374(obj);
    } else {
        ctx.unk24 = obj->unkD4;
        ctx.unk28 = obj->unkD8;
    }
    obj->unkD0 = data_0211215c.unkC;
    obj->unkD1 = data_0211215c.unkD;
    obj->unkD2 = data_0211215c.unkE;
    obj->unkC4.x = data_0211215c.unk0.x;
    obj->unkC4.y = data_0211215c.unk0.y;
    obj->unkC4.z = data_0211215c.unk0.z;
    enabled = OS_DisableInterrupts();
    func_0206cf48(&ctx);
    for (i = 0; i < 7; i++) {
        result = data_02112140[i](&ctx);
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

        if (data_02145360->unk74 == obj) {
            data_02145360->unk74 = obj->unk17C;
        }
        p = data_02145360->unk74;
        if (p == NULL) {
            data_02145360->unk74 = obj;
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
    if (data_02145360->unk8C == 4) {
        data_02145360->unk8C = 0;
    }
    OS_RestoreInterrupts(enabled);
    G3_BeginMakeDL(&ctx.unk64, obj->unk2A4, sizeof(obj->unk2A4));
    func_0206b3f4(&ctx);
    func_0206b0d8(&ctx);
    func_0206c964(&ctx);
    func_0206c65c(&ctx);
    func_0206b740(&ctx);
    func_0206bb24(&ctx);
    func_0206bfa4(&ctx);
    func_0206c2f8(&ctx);
    obj->unk0 = G3_EndMakeDL(&ctx.unk64);
    G3_BeginMakeDL(&ctx.unk64, obj->unk1D84, sizeof(obj->unk1D84));
    func_020715ac(&ctx);
    obj->unk4 = G3_EndMakeDL(&ctx.unk64);
    func_0206d3cc(obj, mii);
    func_020728fc(index, 0);
    return 0;
}
#pragma opt_common_subs reset
#pragma optimization_level reset

int func_0206d924(int index, const MtxFx43 *mtx43, const MtxFx33 *mtx33) {
    NFLObj *obj;

    if (mtx33 == NULL || mtx43 == NULL) {
        return 8;
    }
    if (!func_02072c1c()) {
        return 4;
    }
    obj = func_02073060(index);
    if (obj == NULL) {
        return 9;
    }
    MI_Copy48B(mtx43, &obj->unk2C);
    MI_Copy36B(mtx33, &obj->unk5C);
    return 0;
}

// Sets the polygon attributes of an object's display list (at the places its makers kept) and the material colours
int func_0206d984(int index, const NFLDrawParam *param) {
    NFLObj *obj;
    GXDLInfo info;

    if (param == NULL) {
        return 8;
    }
    if (!func_02072c1c()) {
        return 4;
    }
    obj = func_02073060(index);
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
int func_0206db98(int index, BOOL withSecond) {
    NFLObj *obj;

    if (!func_02072c1c()) {
        return 4;
    }
    obj = func_02073060(index);
    if (obj == NULL) {
        return 9;
    }
    if (func_0206dc84(index) != 0) {
        return 0xF;
    }
    G3_PushMtx();
    G3_MtxMode(GX_MTXMODE_POSITION_VECTOR);
    G3_LoadMtx43(&obj->unk5C);
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

int func_0206dc84(int index) {
    NFLObj *obj;

    if (!func_02072c1c()) {
        return 4;
    }
    obj = func_02073060(index);
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

extern const NFLColor data_020b65a0[];

GXRgb func_0206dcd0(int index) {
    return GX_RGB(data_020b65a0[index].r >> 3, data_020b65a0[index].g >> 3, data_020b65a0[index].b >> 3);
}

GXRgb func_0206dd1c(int index) {
    return data_020b6540[index] & 0x7FFF;
}

// Expands count 4-bit pixels into direct colours from palette, colour 0 transparent if asked
void func_0206dd44(u16 *dst, const u8 *src, const u16 *palette, u32 count, BOOL transparent) {
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

int func_0206e38c(NFLObj *obj, u32 *address) {
    NFLWork *work = data_02145360;

    if (work->unk10[0].unk8 == 0) {
        work->unk10[0].unk0 = *address;
        if (obj->unk90 == 7) {
            *address += 0x800;
        } else {
            *address += 0x200;
            work->unk10[0].unk4 = obj->unk14;
        }
        data_02145360->unkA0[0].unk8 = 0;
        work->unk10[0].unk8 = 1;
    }
    return 0;
}

int func_0206e3fc(NFLObj *obj, u32 *address) {
    NFLWork *work = data_02145360;

    if (work->unk10[1].unk8 == 0) {
        work->unk10[1].unk0 = *address;
        if (obj->unk90 == 7) {
            *address += 0x800;
        } else {
            *address += 0x200;
            work->unk10[1].unk4 = obj->unk24;
        }
        data_02145360->unkA0[1].unk8 = 0;
        work->unk10[1].unk8 = 1;
    }
    return 0;
}

int func_0206e46c(NFLObj *obj, u32 *address) {
    NFLWork *work = data_02145360;

    if (work->unk10[2].unk8 == 0) {
        work->unk10[2].unk0 = *address;
        if (obj->unk90 == 7) {
            *address += 0x800;
        } else {
            *address += 0x200;
            work->unk10[2].unk4 = obj->unk14;
        }
        data_02145360->unkA0[2].unk8 = 0;
        work->unk10[2].unk8 = 1;
    }
    return 0;
}

int func_0206e4dc(NFLObj *obj, u32 *address) {
    NFLWork *work = data_02145360;

    if (work->unk10[3].unk8 == 0) {
        work->unk10[3].unk0 = *address;
        if (obj->unk90 == 7) {
            *address += 0x800;
        } else {
            *address += 0x200;
            work->unk10[3].unk4 = obj->unk14;
        }
        data_02145360->unkA0[3].unk8 = 0;
        work->unk10[3].unk8 = 1;
    }
    return 0;
}

int func_0206e54c(NFLObj *obj, u32 *address) {
    NFLWork *work = data_02145360;

    if (work->unk10[4].unk8 == 0) {
        work->unk10[4].unk0 = *address;
        if (obj->unk90 == 7) {
            *address += 0x800;
        } else {
            *address += 0x200;
            work->unk10[4].unk4 = obj->unk14;
        }
        data_02145360->unkA0[4].unk8 = 0;
        work->unk10[4].unk8 = 1;
    }
    return 0;
}

int func_0206e5bc(NFLObj *obj, u32 *address) {
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

int func_0206e650(NFLMakeCtx *ctx) {
    NFLObj *obj = ctx->unk1C8;
    int result;

    if (obj->unk16C & 0x20) {
        result = func_0206e38c(obj, &data_02145360->unk0);
        if (result != 0) {
            return result;
        }
    }
    if (obj->unk16C & 0x40) {
        result = func_0206e3fc(obj, &data_02145360->unk0);
        if (result != 0) {
            return result;
        }
    }
    if (obj->unk16C & 0x2) {
        result = func_0206e46c(obj, &data_02145360->unk0);
        if (result != 0) {
            return result;
        }
    }
    if (obj->unk16C & 0x4) {
        result = func_0206e4dc(obj, &data_02145360->unk0);
        if (result != 0) {
            return result;
        }
    }
    if (obj->unk16C & 0x8) {
        result = func_0206e54c(obj, &data_02145360->unk0);
        if (result != 0) {
            return result;
        }
    }
    return func_0206e5bc(obj, &ctx->unk24);
}


BOOL func_02072c1c(void) {
    return data_02145360 != NULL;
}

int func_02072c38(u8 *p) {
    *(u32 *)(p + 0x18) = 0;
    *(u32 *)(p + 0x24) = 0;
    *(u32 *)(p + 0x30) = 0;
    *(u32 *)(p + 0x3c) = 0;
    *(u32 *)(p + 0x48) = 0;
    *(u32 *)(p + 0x54) = 0;
    return 0;
}

int func_02072c5c(NFLObj *obj) {
    OSIntrMode enabled;
    NFLWork *work;
    NFLObj *last;
    int n;

    if (obj == NULL) {
        return 8;
    }
    enabled = OS_DisableInterrupts();
    if (!func_02072c1c()) {
        OS_RestoreInterrupts(enabled);
        return 4;
    }
    work = data_02145360;
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

void func_020731b8(u8 *p) {
    int i;

    p[0x18] = 0xE0;
    for (i = 1; i < 8; i++) {
        p[0x18 + i] = 0;
    }
}

BOOL func_02073264(const NFLCreateId *id) {
    return !(id->data[0] & 0x80);
}

u32 func_02073278(const NFLCreateId *id) {
    return id->data[0] & 0x40;
}

// Whether the create ID is all zero
BOOL func_02073284(const NFLCreateId *id) {
    return *(u32 *)&id->data[0] == 0 && *(u32 *)&id->data[4] == 0;
}

BOOL func_020732a0(const NFLCreateId *a, const NFLCreateId *b) {
    int i;

    if (func_02073284(b) || func_0207331c(b)) {
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

BOOL func_0207331c(const NFLCreateId *id) {
    return (id->data[0] & 0x20) ? TRUE : FALSE;
}

// Whether two create IDs are equal and neither is all zero
BOOL func_02073330(const NFLCreateId *a, const NFLCreateId *b) {
    const u8 *p = a->data;
    const u8 *q = b->data;
    int i;

    if (func_02073284(a) || func_02073284(b)) {
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

void func_02073394(NFLCreateId *id) {
    *(u32 *)&id->data[0] = 0;
    *(u32 *)&id->data[4] = 0;
}

int func_02073584(void *p) {
    OSIntrMode enabled = OS_DisableInterrupts();
    NFLWork *work = data_02145360;

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

BOOL func_020735dc(void) {
    return data_02145360->unk64 != NULL;
}

BOOL func_020735fc(void) {
    return data_02145360->unk78 != NULL;
}

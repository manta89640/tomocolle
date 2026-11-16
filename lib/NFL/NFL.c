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

// The state that func_0206ed94 and func_020705d4 share across calls (func_020715ac clears unk20 between pairs)
typedef struct NFLMakeState {
    u8 unk0[0x20];
    int unk20;
} NFLMakeState;

void func_0206ed94(GXDLInfo *dl, NFLMakePart *part, NFLObj *obj, const NFLCharData *data, NFLPatch *patch,
                   NFLDLRef *ref, int n, NFLMakeState *state);
void func_020705d4(GXDLInfo *dl, NFLMakePart *part, NFLObj *obj, const NFLCharData *data, NFLPatch *patch,
                   NFLDLRef *ref, int n, NFLMakeState *state);

void func_020715ac(NFLMakeCtx *ctx) {
    NFLObj *obj = ctx->unk1C8;
    const NFLCharData *data = ctx->unk1CC;
    NFLMakeState state;

    state.unk20 = 0;
    func_0206ed94(&ctx->unk64, &ctx->unk78[4], obj, data, &obj->unk140, NULL, 0, &state);
    func_0206ed94(&ctx->unk64, &ctx->unk78[5], obj, data, NULL, NULL, 0x20, &state);
    state.unk20 = 0;
    func_020705d4(&ctx->unk64, &ctx->unk78[0], obj, data, &obj->unk128, NULL, 0, &state);
    func_020705d4(&ctx->unk64, &ctx->unk78[2], obj, data, &obj->unkF8, &obj->unk160, 0x20, &state);
    state.unk20 = 0;
    func_020705d4(&ctx->unk64, &ctx->unk78[1], obj, data, &obj->unk110, NULL, 0, &state);
    func_020705d4(&ctx->unk64, &ctx->unk78[3], obj, data, &obj->unkE0, &obj->unk158, 0x20, &state);
    if (data->moleType == 1) {
        func_0206ed94(&ctx->unk64, &ctx->unk78[6], obj, data, NULL, NULL, 0, NULL);
    }
}

void func_02074548(NFLObj *obj, MtxFx43 *mtx, fx16 sin, fx16 cos);

// Patches a face part's display list: its rotation, if it has a matrix command, and its texture and palette
void func_02071730(NFLObj *obj, NFLPatch *patch, fx16 sin, fx16 cos, GXTexFmt fmt, GXTexSizeS s, GXTexSizeT t,
                   u32 texAddr, u32 plttAddr) {
    GXDLInfo dl;
    MtxFx43 mtx;

    if (patch->mtx.cmd != NULL && patch->mtx.param != NULL) {
        dl.curr_cmd = patch->mtx.cmd;
        dl.curr_param = patch->mtx.param;
        func_02074548(obj, &mtx, sin, cos);
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

int func_0206ddb8(NFLMakeCtx *ctx);
int func_0206e650(NFLMakeCtx *ctx);
void func_0206e724(NFLMakeCtx *ctx);

int func_02071578(NFLMakeCtx *ctx) {
    int result = func_0206ddb8(ctx);

    if (result != 0) {
        return result;
    }
    result = func_0206e650(ctx);
    if (result != 0) {
        return result;
    }
    func_0206e724(ctx);
    return result;
}

NFLObj *func_02073060(int index);
void func_0207183c(NFLObj *obj);
void func_02071a78(NFLObj *obj);
void func_02071cdc(NFLObj *obj);
void func_02071f24(NFLObj *obj);
void func_0207219c(NFLObj *obj);
void func_02072418(NFLObj *obj);
void func_02072694(NFLObj *obj);

int func_020728fc(int index, int type) {
    NFLObj *obj;

    if (!func_02072c1c()) {
        return 4;
    }
    obj = func_02073060(index);
    if (obj == NULL) {
        return 9;
    }
    switch (type) {
    case 5:
        if (obj->unk16C & 0x20) {
            func_02071a78(obj);
            obj->unk168 = 5;
            return 0;
        }
        break;
    case 6:
        if (obj->unk16C & 0x40) {
            func_02071cdc(obj);
            obj->unk168 = 6;
            return 0;
        }
        break;
    case 1:
        if (obj->unk16C & 0x2) {
            func_02071f24(obj);
            obj->unk168 = 1;
            return 0;
        }
        break;
    case 2:
        if (obj->unk16C & 0x4) {
            func_0207219c(obj);
            obj->unk168 = 2;
            return 0;
        }
        break;
    case 3:
        if (obj->unk16C & 0x8) {
            func_02072418(obj);
            obj->unk168 = 3;
            return 0;
        }
        break;
    case 4:
        if (obj->unk16C & 0x10) {
            func_02072694(obj);
            obj->unk168 = 4;
            return 0;
        }
        break;
    }
    func_0207183c(obj);
    obj->unk168 = 0;
    return 0;
}

int func_02072a28(int index) {
    NFLObj *obj;

    if (!func_02072c1c()) {
        return -1;
    }
    obj = func_02073060(index);
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

int func_02072c38(u8 *p);
int func_02073584(void *p);
extern const u8 data_020b68d0[];

// The library's initialisation: main passes NFL_Res.dat's data as res
int func_02072a58(NFLWork *work, void *res, void *buf, u32 size, const NFLDesc *desc, u8 flags, u8 max) {
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
    func_02072c38((u8 *)work);
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
        if (flags & data_020b68d0[i]) {
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
    data_02145360 = work;
    func_02073584(res);
    OS_RestoreInterrupts(enabled);
    return 0;
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

int func_02072d8c(void) {
    if (!func_02072c1c()) {
        return 4;
    }
    data_02145360->unk70++;
    return 0;
}

int func_02072dc0(NFLObj **removed) {
    OSIntrMode enabled;
    NFLObj *obj = NULL;
    NFLWork *work;
    NFLObj *prev;
    int n;
    NFLObj *cur;

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
    if (work->unk70 != 0) {
        work->unk70--;
    }
    if (removed != NULL) {
        *removed = NULL;
    }
    prev = data_02145360->unk6C;
    if (prev != NULL) {
        if (data_02145360->unk70 == 0) {
            prev->unk170 = 4;
            obj = prev;
            data_02145360->unk6C = NULL;
        } else {
            cur = prev->next;
            n = 1;
            while (cur != NULL) {
                if (cur->next == NULL) {
                    if (n >= data_02145360->unk70) {
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
        work = data_02145360;
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

int func_02072f34(int index) {
    OSIntrMode enabled = OS_DisableInterrupts();
    NFLObj *obj;

    if (!func_02072c1c()) {
        OS_RestoreInterrupts(enabled);
        return 4;
    }
    if (data_02145360->unk8C == 1 || data_02145360->unk8C == 3) {
        OS_RestoreInterrupts(enabled);
        return 6;
    }
    obj = func_02073060(index);
    if (obj == NULL) {
        OS_RestoreInterrupts(enabled);
        return 9;
    }
    obj->unk170 = 4;
    OS_RestoreInterrupts(enabled);
    return 0;
}

int func_02072fc4(void) {
    int n;
    NFLObj *obj;
    NFLWork *work;
    OSIntrMode enabled = OS_DisableInterrupts();

    if (!func_02072c1c()) {
        OS_RestoreInterrupts(enabled);
        return -1;
    }
    work = data_02145360;
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

NFLObj *func_02073060(int index) {
    int i;
    NFLObj *obj;
    NFLWork *work;
    OSIntrMode enabled = OS_DisableInterrupts();

    if (!func_02072c1c()) {
        OS_RestoreInterrupts(enabled);
        return NULL;
    }
    work = data_02145360;
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

u16 func_020743a0(int max);

// Makes a create ID from the time, a random number and the MAC address
BOOL func_020730fc(NFLCreateId *id) {
    u16 random;
    s64 seconds;
    u32 t;
    RTCDate date;
    RTCTime time;
    u8 mac[6];

    if (RTC_GetDateTime(&date, &time) == RTC_RESULT_SUCCESS) {
        random = func_020743a0(4);
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

void func_020731b8(u8 *p) {
    int i;

    p[0x18] = 0xE0;
    for (i = 1; i < 8; i++) {
        p[0x18 + i] = 0;
    }
}

extern const int data_020b68d8[];

// Whether a Mii's data is valid: height and build at most 128, a favourite colour of the 12, a date that exists, and
// a create ID
BOOL func_020731e0(const NFLCharData *data) {
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
    if (data->birthDay > data_020b68d8[data->birthMonth]) {
        return FALSE;
    }
    return !func_02073284(&data->createId);
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

// What func_020733a4 makes of a Mii's data
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

u16 func_0206dcd0(int faceColor);

int func_020733a4(NFLCharInfo *info, const NFLCharData *data) {
    u8 height;
    u8 build;

    if (!func_020731e0(data)) {
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
    info->unknown00Bit0 = data->unknown00Bit0;
    info->height = height;
    info->build = build;
    info->unk34Bits29 = (*(const u32 *)&data->createId & 0x80000000) ? 0 : 3;
    info->unk38 = func_0206dcd0(data->faceColor);
    return 0;
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

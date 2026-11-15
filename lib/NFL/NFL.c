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

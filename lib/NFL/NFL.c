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

u16 func_0206dd1c(int index);

u16 func_02073514(int index) {
    return func_0206dd1c(index);
}

// The create-ID helpers above, on a Mii's data
BOOL func_02073520(const NFLCharData *data) {
    return func_02073264(&data->createId);
}

u32 func_02073530(const NFLCharData *data) {
    return func_02073278(&data->createId);
}

BOOL func_02073540(const NFLCreateId *a, const NFLCharData *b) {
    return func_020732a0(a, &b->createId);
}

BOOL func_02073550(const NFLCharData *data) {
    return func_0207331c(&data->createId);
}

BOOL func_02073560(const NFLCharData *a, const NFLCharData *b) {
    return func_02073330(&a->createId, &b->createId);
}

void func_02073574(NFLCharData *data) {
    func_02073394(&data->createId);
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

extern const NFLCharData data_020b70c8[];

// Copies Mii index (0 to 5) of the library's table into data, and gives it a blank create ID
int func_0207361c(NFLCharData *data, u32 index) {
    if (index > 5) {
        return 8;
    }
    if (data == NULL) {
        return 8;
    }
    MI_CpuCopy8(&data_020b70c8[index], data, sizeof(NFLCharData));
    func_020731b8((u8 *)data);
    return 0;
}

void func_020736ec(NFLCharData *data, int sex, int b, int c);

int func_02073664(NFLCharData *data, int sex, int b, int c) {
    if (data_02145360 == NULL) {
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
    func_020736ec(data, sex, b, c);
    func_020731b8((u8 *)data);
    return 0;
}

u16 func_020743a0(u16 max);
void func_020737b0(NFLCharData *data, int sex, int b, int c);

// Picks at random what is 2 (sex) or 3 (b and c) on entry, then makes the Mii
void func_020736ec(NFLCharData *data, int sex, int b, int c) {
    int d;
    u16 r;

    if (sex == 2) {
        if (func_020743a0(2) == 0) {
            sex = 0;
        } else {
            sex = 1;
        }
    }
    if (b == 3) {
        r = func_020743a0(10);
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
        r = func_020743a0(10);
        if (r < 4) {
            d = 2;
        } else if (r < 8) {
            d = 1;
        } else {
            d = 0;
        }
        break;
    }
    func_020737b0(data, sex, b, d);
}


u8 func_02073eac(int sex, int b, int c);
u8 func_02073eec(int sex, int b, int c);
u8 func_02073f94(int sex, int b, int c);
u8 func_02074034(int sex, int b, int c);
u8 func_020740a8(int sex, int b, int c);
u8 func_02074148(int sex, int b, int c);
u8 func_020741e8(int sex, int b, int c);
u8 func_02074228(int b);
u8 func_02074310(int sex, int c);
u8 func_02074344(int b, int c);
u8 func_02074378(int c);

extern const u8 data_020b689c[];
extern const u8 data_020b6884[];

// Makes a Mii's data at random, for sex, b and c: the parts from the tables of the functions after it, the face's
// parts placed lower by up to 4 for sex 1 and for b 0, a beard on one man in 5 of b 1 and 2
void func_020737b0(NFLCharData *data, int sex, int b, int c) {
    u8 offset = 0;
    int i;

    if (sex == 1) {
        offset += func_020743a0(3);
    }
    if (b == 0) {
        offset += func_020743a0(3);
    }
    data->faceType = func_02073eac(sex, b, c);
    data->faceColor = func_02074310(sex, c);
    data->faceTexture = func_020741e8(sex, b, c);
    data->hairType = func_02073eec(sex, b, c);
    data->hairColor = func_02074344(b, c);
    data->hairFlip = func_020743a0(2);
    data->eyeType = func_02073f94(sex, b, c);
    data->eyeColor = func_02074378(c);
    data->eyeScale = 4;
    if (sex == 0) {
        data->eyeRotation = 4;
        data->eyeRotation += data_020b689c[2] - data_020b689c[data->eyeType];
    } else {
        data->eyeRotation = 3;
        data->eyeRotation += data_020b689c[4] - data_020b689c[data->eyeType];
    }
    data->eyeX = 2;
    data->eyeY = offset + 12;
    data->eyebrowType = func_02074034(sex, b, c);
    data->eyebrowColor = data->hairColor;
    data->eyebrowScale = 4;
    data->eyebrowRotation = 6;
    data->eyebrowX = 2;
    if (c == 2) {
        data->eyebrowY = offset + 9;
        data->eyebrowRotation += data_020b6884[6] - data_020b6884[data->eyebrowType];
    } else {
        data->eyebrowY = offset + 10;
        data->eyebrowRotation += data_020b6884[0] - data_020b6884[data->eyebrowType];
    }
    data->noseType = func_020740a8(sex, b, c);
    if (sex == 0) {
        data->noseScale = 4;
    } else {
        data->noseScale = 3;
    }
    data->noseY = offset + 9;
    data->mouthType = func_02074148(sex, b, c);
    if (sex == 0) {
        data->mouthColor = 0;
    } else {
        data->mouthColor = func_020743a0(3);
    }
    data->mouthScale = 4;
    data->mouthY = offset + 13;
    if (sex == 0 && (b == 1 || b == 2) && func_020743a0(10) < 2) {
        u16 r = func_020743a0(3);

        if (r == 0) {
            data->mustacheType = 0;
            data->beardType = func_020743a0(3) + 1;
        } else if (r == 1) {
            data->mustacheType = func_020743a0(3) + 1;
            data->beardType = 0;
        } else if (r == 2) {
            data->mustacheType = func_020743a0(3) + 1;
            data->beardType = func_020743a0(3) + 1;
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
    data->glassesType = func_02074228(b);
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
    data->favoriteColor = func_020743a0(12);
    data->unknown00Bit0 = 0;
    data->unknown20Bit2 = 0;
}

extern const u8 data_020b69c0[][10];

u8 func_02073eac(int sex, int b, int c) {
    return data_020b69c0[sex * 9 + b * 3 + c][func_020743a0(10)];
}

// The hair type of a random Mii: one of the choices of row sex * 9 + b * 3 + c
u8 func_02073eec(int sex, int b, int c) {
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

    return choices[index][func_020743a0(counts[index])];
}

// The eye type of a random Mii: one of the choices of row sex * 9 + b * 3 + c
u8 func_02073f94(int sex, int b, int c) {
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

    return choices[index][func_020743a0(counts[index])];
}

extern const u8 data_020b6f18[][24];

// The eyebrow type of a random Mii: one of the choices of row sex * 9 + b * 3 + c
u8 func_02074034(int sex, int b, int c) {
    u8 counts[18] = {18, 18, 18, 23, 23, 23, 21, 21, 21, 9, 9, 9, 11, 11, 11, 9, 9, 9};
    u8 index = sex * 9 + b * 3 + c;

    return data_020b6f18[index][func_020743a0(counts[index])];
}

// The nose type of a random Mii: one of the choices of row sex * 9 + b * 3 + c
u8 func_020740a8(int sex, int b, int c) {
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

    return choices[index][func_020743a0(counts[index])];
}

// The mouth type of a random Mii: one of the choices of row sex * 9 + b * 3 + c
u8 func_02074148(int sex, int b, int c) {
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

    return choices[index][func_020743a0(counts[index])];
}

extern const u8 data_020b6c00[][20];

u8 func_020741e8(int sex, int b, int c) {
    return data_020b6c00[sex * 9 + b * 3 + c][func_020743a0(20)];
}

u8 func_02074228(int b) {
    u16 r = func_020743a0(100);

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

extern const u8 data_020b6984[][10];

u8 func_02074310(int sex, int c) {
    return data_020b6984[sex * 3 + c][func_020743a0(10)];
}

extern const u8 data_020b6a74[][20];

u8 func_02074344(int b, int c) {
    return data_020b6a74[c * 3 + b][func_020743a0(20)];
}

extern const u8 data_020b6966[][10];

u8 func_02074378(int c) {
    return data_020b6966[c][func_020743a0(10)];
}

// A random number below max (a 16-bit one when max is 0), from the LCG in the work area
u16 func_020743a0(u16 max) {
    NFLWork *work = data_02145360;

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
void func_020743e4(const NFLDrawContext *ctx, VecFx16 *out, const VecFx16 *in) {
    ((fx16 *)out)[ctx->unkD2] = ctx->unkCC ? -in->x : in->x;
    ((fx16 *)out)[ctx->unkD0] = ctx->unkC4 ? -in->y : in->y;
    ((fx16 *)out)[ctx->unkD1] = ctx->unkC8 ? -in->z : in->z;
}

void func_02074444(const NFLDrawContext *ctx, VecFx32 *out, const VecFx32 *in) {
    ((fx32 *)out)[ctx->unkD2] = ctx->unkCC ? -in->x : in->x;
    ((fx32 *)out)[ctx->unkD0] = ctx->unkC4 ? -in->y : in->y;
    ((fx32 *)out)[ctx->unkD1] = ctx->unkC8 ? -in->z : in->z;
}

void func_02074498(const NFLDrawContext *ctx, VecFx32 *out, const VecFx32 *in) {
    ((fx32 *)out)[ctx->unkD2] = in->x;
    ((fx32 *)out)[ctx->unkD0] = in->y;
    ((fx32 *)out)[ctx->unkD1] = in->z;
}

void func_020744c0(const NFLDrawContext *ctx, VecFx32 *out, const VecFx32 *in) {
    fx32 v[3];

    v[ctx->unkD2] = ctx->unkCC ? -in->x : in->x;
    v[ctx->unkD0] = ctx->unkC4 ? -in->y : in->y;
    v[ctx->unkD1] = ctx->unkC8 ? -in->z : in->z;
    out->x = v[0];
    out->y = v[1];
    out->z = v[2];
}

// Rotation about the context's z axis
void func_02074548(const NFLDrawContext *ctx, MtxFx43 *mtx, fx32 sinVal, fx32 cosVal) {
    fx32 a[3];
    VecFx32 axis;

    a[ctx->unkD2] = 0;
    a[ctx->unkD0] = 0;
    a[ctx->unkD1] = ctx->unkC8 ? -FX32_ONE : FX32_ONE;
    axis.x = a[0];
    axis.y = a[1];
    axis.z = a[2];
    MTX_RotAxis43(mtx, &axis, sinVal, cosVal);
}

void func_020745b0(const NFLDrawContext *ctx, GXDLInfo *info, fx32 x, fx32 y, fx32 z) {
    VecFx32 in;
    VecFx32 out;

    in.x = x;
    in.y = y;
    in.z = z;
    func_02074444(ctx, &out, &in);
    G3C_Translate(info, out.x, out.y, out.z);
}

void func_020745f4(const NFLDrawContext *ctx, GXDLInfo *info, fx32 x, fx32 y, fx32 z) {
    fx32 out[3];
    VecFx32 in = {x, y, z};

    out[ctx->unkD2] = in.x;
    out[ctx->unkD0] = in.y;
    out[ctx->unkD1] = in.z;
    G3C_Scale(info, out[0], out[1], out[2]);
}

void func_02074644(const NFLDrawContext *ctx, GXDLInfo *info, fx16 x, fx16 y, fx16 z) {
    VecFx16 in;
    VecFx16 out;

    in.x = x;
    in.y = y;
    in.z = z;
    func_020743e4(ctx, &out, &in);
    G3C_Vtx(info, out.x, out.y, out.z);
}

void func_02074688(const NFLDrawContext *ctx, GXDLInfo *info, fx16 x, fx16 y, fx16 z) {
    VecFx16 in;
    VecFx16 out;

    in.x = x;
    in.y = y;
    in.z = z;
    func_020743e4(ctx, &out, &in);
    G3C_Normal(info, out.x, out.y, out.z);
}

// Whether end bytes fit in the texture palette slots of a GXVRamTexPltt: 14 an unknown one, 13 too many
int func_020746cc(int bank, u32 offset, u32 end) {
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
int func_02074778(int bank, u32 offset, u32 end) {
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

extern const NFLTexBank data_020b7b04[];

// Copies size bytes of texture image to offset of the texture slots, through the LCDC, as GX_LoadTex does
int func_02074814(int bank, const void *src, u32 offset, u32 size) {
    u32 n;
    void *dest;
    int result;
    u32 second = data_020b7b04[bank].unk2 << 12;
    u32 first = data_020b7b04[bank].unk0;
    u32 split = data_020b7b04[bank].unk4;
    u32 end = offset + size;

    result = func_02074778(bank, offset, end);
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

extern const u16 data_020b7af4[];

// Copies size bytes of palettes to offset of the texture palette slots, through the LCDC, as GX_LoadTexPltt does
int func_02074904(int bank, const void *src, u32 offset, u32 size) {
    u32 base = data_020b7af4[bank >> 4] << 12;
    int result = func_020746cc(bank, offset, offset + size);
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

int func_02074964(const void *src, u32 offset, u32 size) {
    int bank = func_0200473c();
    int result;

    GX_BeginLoadTex();
    result = func_02074814(bank, src, offset, size);
    GX_EndLoadTex();
    return result;
}

int func_020749a4(const void *src, u32 offset, u32 size) {
    int bank = func_0200474c();
    int result;

    GX_BeginLoadTexPltt();
    result = func_02074904(bank, src, offset, size);
    GX_EndLoadTexPltt();
    return result;
}

BOOL func_02074b68(u32 *arg);
BOOL func_0207516c(u32 *arg);
BOOL func_02075528(NFLObj *obj, u32 *arg);
BOOL func_02075f50(NFLObj *obj, u32 *arg);
void func_020763e0(NFLObj *obj);

// The library's work of a frame, which main calls every frame: 4 not set up, 0x14 and 0x13 busy, 3 nothing to do
int func_020749e4(void) {
    NFLWork *work = data_02145360;
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
        func_02074b68(&arg);
        work->unk8C = 2;
        break;
    case 1:
        func_0207516c(&arg);
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
            func_02075528(work->unk74, &arg);
            work->unk8C = 2;
            break;
        case 1:
            func_02075f50(work->unk74, &arg);
            work->unk8C = 2;
            break;
        case 2:
        case 4:
            func_020763e0(work->unk74);
            work->unk8C = 0;
            break;
        }
        break;
    }
    work->unk88 = arg;
    return 0;
}

void func_0206dd44(u16 *dest, const void *src, const u16 *palette, u32 size, BOOL arg4);

// Copies size bytes, or converts them through palette when there is one
void func_02074b34(const void *src, void *dest, const u16 *palette, u32 size, BOOL arg4) {
    if (palette != NULL) {
        func_0206dd44(dest, src, palette, size, arg4);
    } else {
        MIi_CpuCopy32(src, dest, size);
    }
}

// What func_02074b68 copies to each of its six streams: a resource (func_0206cf6c's kind and index) and its size
typedef struct NFLStreamSource {
    int unk0;
    int unk4;
    u32 unk8;
} NFLStreamSource;

extern NFLStreamSource data_0211216c[];

void *func_0206cf6c(int kind, int index);

BOOL func_02074b68(u32 *pos) {
    NFLWork *work = (NFLWork *)data_02145360;
    NFLWorkUnkA0 *stream = work->unkA0;
    NFLStreamSource *source = data_0211216c;
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
        if (data_02145360->unk68 == 7) {
            size = source->unk8 * 4;
            if (source->unk0 == 2) {
                func_0206a014(palette, 0);
                pal = palette;
            } else if (source->unk0 == 13) {
                func_0206a1b4(palette, 0);
                pal = palette;
            } else {
                pal = func_0206cf6c(12, 0);
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
                src = func_0206cf6c(source->unk0, source->unk4);
                func_02074b34(src, work->unk78 + offset, pal, size, TRUE);
                DC_FlushRange(work->unk78 + work->unk84, size);
            } else {
                int over = work->unk84 + size - work->unk7C;
                int n;

                src = func_0206cf6c(source->unk0, source->unk4);
                n = size - over;
                func_02074b34(src, work->unk78 + work->unk84, pal, n, TRUE);
                DC_FlushRange(work->unk78 + work->unk84, n);
                if (data_02145360->unk68 == 7) {
                    src += n / 4;
                } else {
                    src += n;
                }
                func_02074b34(src, work->unk78, pal, over, TRUE);
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
            src = func_0206cf6c(source->unk0, source->unk4);
            switch (stream->unk8) {
            case 3:
                if (data_02145360->unk68 == 7) {
                    src += (int)work->unk98 / 4;
                } else {
                    src += work->unk98;
                }
                size -= work->unk98;
                extra = 0;
                break;
            case 4:
                if (data_02145360->unk68 == 7) {
                    src += (int)work->unk98 / 4;
                } else {
                    src += work->unk98;
                }
                size -= work->unk98;
                extra = work->unk94;
                break;
            case 1:
                extra = work->unk94;
                if (data_02145360->unk68 == 7) {
                    src += (int)extra / 4;
                } else {
                    src += extra;
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
                func_02074b34(src, work->unk78 + work->unk84, palette, size, TRUE);
                DC_FlushRange(work->unk78 + work->unk84, size);
            } else {
                int over = work->unk84 + size - work->unk7C;
                int n = size - over;

                func_02074b34(src, work->unk78 + work->unk84, palette, n, TRUE);
                DC_FlushRange(work->unk78 + work->unk84, n);
                if (data_02145360->unk68 == 7) {
                    src += n / 4;
                } else {
                    src += n;
                }
                func_02074b34(src, work->unk78, palette, over, TRUE);
                DC_FlushRange(work->unk78, over);
            }
        }
        switch (i) {
        case 0:
            stream->unk4 = data_02145360->unk10[0].unk0 + extra;
            break;
        case 1:
            stream->unk4 = data_02145360->unk10[1].unk0 + extra;
            break;
        case 2:
            stream->unk4 = data_02145360->unk10[2].unk0 + extra;
            break;
        case 3:
            stream->unk4 = data_02145360->unk10[3].unk0 + extra;
            break;
        case 4:
            stream->unk4 = data_02145360->unk10[4].unk0 + extra;
            break;
        case 5:
            stream->unk4 = data_02145360->unk10[5].unk0 + extra;
            break;
        }
        work->unk84 = (work->unk84 + size) % work->unk7C;
        *pos += size;
        }
    }
    if (data_02145360->unk68 != 7) {
        func_0207516c(pos);
    } else {
        if (work->unk74 == NULL) {
            return TRUE;
        }
        switch (work->unk74->unk170) {
        case 0:
            func_02075528(work->unk74, pos);
            break;
        case 1:
            func_02075f50(work->unk74, pos);
            break;
        }
    }
    return TRUE;
}

// The seventh stream of the work area: 32 bytes of resource 12, 0 (the palette func_02074b68 uses)
BOOL func_0207516c(u32 *pos) {
    NFLWork *work = (NFLWork *)data_02145360;

    if (work->unk68 != 7) {
        NFLWorkUnkA0 *stream;

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

                    src = func_0206cf6c(12, 0);
                    MIi_CpuCopy32(src, buf + offset, size);
                    DC_FlushRange(work->unk78 + work->unk84, size);
                } else {
                    int over = offset + size - work->unk7C;
                    int n;

                    src = func_0206cf6c(12, 0);
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
                src = func_0206cf6c(12, 0);
                switch (work->unkA0[6].unk8) {
                case 3:
                    src += work->unk98;
                    size -= work->unk98;
                    break;
                case 4:
                    src += work->unk98;
                    extra = work->unk94;
                    size -= work->unk98;
                    break;
                case 1:
                    extra = work->unk94;
                    work->unkA0[6].unk0 = work->unk84;
                    src += extra;
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
                    MIi_CpuCopy32(src, work->unk78 + work->unk84, size);
                    DC_FlushRange(work->unk78 + work->unk84, size);
                } else {
                    int over = work->unk84 + size - work->unk7C;
                    int n = size - over;

                    MIi_CpuCopy32(src, work->unk78 + work->unk84, n);
                    DC_FlushRange(work->unk78 + work->unk84, n);
                    if (data_02145360->unk68 == 7) {
                        src += n / 4;
                    } else {
                        src += n;
                    }
                    MIi_CpuCopy32(src, work->unk78, over);
                    DC_FlushRange(work->unk78, over);
                }
            }
            work->unk84 = (work->unk84 + size) % work->unk7C;
            work->unkA0[6].unk4 = data_02145360->unk10[5].unk4 + extra;
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
        func_02075528(work->unk74, pos);
        break;
    case 1:
        func_02075f50(work->unk74, pos);
        break;
    }
    return TRUE;
}

// What func_02075528 copies to each of an object's eleven streams: a resource kind (func_0206cf6c's) and its size
typedef struct NFLObjSource {
    int unk0;
    u32 unk4;
} NFLObjSource;

extern NFLObjSource data_021121b4[];

void func_0206cd9c(u16 *palette, const s16 *params);
void func_0206cc30(u16 *palette);

BOOL func_02075528(NFLObj *obj, u32 *pos) {
    NFLWork *work = data_02145360;
    s16 *params = obj->unk180;
    NFLWorkUnkA0 *stream = obj->unk1A4;
    int i;
    u16 palette[64];

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
            kind = data_021121b4[i].unk0;
            index = stream->unk4;
            overflowB = FALSE;
            overflowA = FALSE;
            arg4 = TRUE;
            pal = data_02145360->unk68 == 7 ? palette : NULL;
            if (data_02145360->unk68 == 7) {
                if (kind == 21) {
                    size = 0x80;
                } else {
                    size = data_021121b4[i].unk4 * 4;
                }
            } else {
                size = data_021121b4[i].unk4;
            }
            switch (kind) {
            case 21:
                if (data_02145360->unk68 == 7) {
                    func_0206cd9c(palette, params);
                } else {
                    func_0206cc30(palette);
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
                if (data_02145360->unk68 == 7) {
                    func_0206a014(pal, params[8]);
                }
                break;
            case 3:
                if (data_02145360->unk68 == 7) {
                    func_02069de4(pal, params[6]);
                }
                break;
            case 13:
                if (data_02145360->unk68 == 7) {
                    func_0206a1b4(pal, params[10]);
                }
                break;
            case 14:
                if (data_02145360->unk68 == 7) {
                    func_02069de4(pal, params[14]);
                }
                break;
            case 8:
                if (data_02145360->unk68 == 7) {
                    func_0206a99c(pal, params[12]);
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
                if (data_02145360->unk68 == 7) {
                    func_02069de4(pal, params[14]);
                }
                break;
            case 5:
                if (data_02145360->unk68 == 7) {
                    func_0206a2e4(pal, params[2]);
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
                    if (data_02145360->unk68 == 7) {
                        func_0206a894(pal, params[0]);
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
                    if (data_02145360->unk68 == 7) {
                        func_02069de4(pal, params[4]);
                    }
                    if (params[3] == 29) {
                        arg4 = TRUE;
                    } else {
                        arg4 = FALSE;
                    }
                }
                break;
            case 17:
                if (data_02145360->unk68 == 7) {
                    pal = func_0206cf6c(12, 0);
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
                        src = func_0206cf6c(kind, index);
                        func_02074b34(src, work->unk78 + offset, pal, size, arg4);
                        DC_FlushRange(work->unk78 + work->unk84, size);
                    } else {
                        int over = work->unk84 + size - work->unk7C;
                        int n;

                        src = func_0206cf6c(kind, index);
                        n = size - over;
                        func_02074b34(src, work->unk78 + work->unk84, pal, n, arg4);
                        DC_FlushRange(work->unk78 + work->unk84, n);
                        if (data_02145360->unk68 == 7) {
                            src += n / 4;
                        } else {
                            src += n;
                        }
                        func_02074b34(src, work->unk78, pal, over, arg4);
                        DC_FlushRange(work->unk78, over);
                    }
                    if (!overflowA) {
                        stream->unk8 = 2;
                    } else {
                        obj->unk178 = size;
                        stream->unk8 = 3;
                    }
                } else {
                    src = func_0206cf6c(kind, index);
                    switch (stream->unk8) {
                    case 3:
                    case 4:
                        if (data_02145360->unk68 == 7) {
                            src += (int)obj->unk178 / 4;
                        } else {
                            src += obj->unk178;
                        }
                        size -= obj->unk178;
                        break;
                    case 1:
                        if (data_02145360->unk68 == 7) {
                            src += (int)obj->unk174 / 4;
                        } else {
                            src += obj->unk174;
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
                        func_02074b34(src, work->unk78 + work->unk84, pal, size, arg4);
                        DC_FlushRange(work->unk78 + work->unk84, size);
                    } else {
                        int over = work->unk84 + size - work->unk7C;
                        int n = size - over;

                        func_02074b34(src, work->unk78 + work->unk84, pal, n, arg4);
                        DC_FlushRange(work->unk78 + work->unk84, n);
                        if (data_02145360->unk68 == 7) {
                            src += n / 4;
                        } else {
                            src += n;
                        }
                        func_02074b34(src, work->unk78, pal, over, arg4);
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
    if (data_02145360->unk68 == 7) {
        if (obj->unk17C != NULL) {
            func_02075528(obj->unk17C, pos);
        }
    } else {
        func_02075f50(obj, pos);
    }
    return TRUE;
}

void func_0206cc6c(u16 *palette, const s16 *params);

// The palettes of an object's eleven palette streams (unk228), made from its parameters at unk180
BOOL func_02075f50(NFLObj *obj, u32 *pos) {
    NFLWork *work = (NFLWork *)data_02145360;
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
                    func_0206cc6c(pal, params);
                    break;
                case 1:
                    func_0206a014(pal, params[8]);
                    break;
                case 2:
                    func_02069de4(pal, params[6]);
                    break;
                case 3:
                    func_0206a1b4(pal, params[10]);
                    break;
                case 4:
                    func_02069de4(pal, params[14]);
                    break;
                case 5:
                    func_0206a99c(pal, params[12]);
                    break;
                case 6:
                    func_02069de4(pal, params[14]);
                    break;
                case 7:
                    func_0206a2e4(pal, params[2]);
                    break;
                case 8:
                    if (params[3] == 34 || params[3] == 57) {
                        func_0206a894(pal, params[0]);
                    } else {
                        func_02069de4(pal, params[4]);
                    }
                    break;
                case 9:
                    MIi_CpuCopy32(func_0206cf6c(12, 0), pal, size);
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
        func_02075528(obj->unk17C, pos);
    }
    return TRUE;
}

// Takes obj out of the list at unk74 of the work area
void func_020763e0(NFLObj *obj) {
    NFLWork *work = data_02145360;
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

int func_02076680(u32 *size);
int func_02076a98(u32 *size);
int func_02076e1c(NFLObj *obj, u32 *size);
int func_02077300(NFLObj *obj, u32 *size);

// The library's work after the frame's interrupt, which main calls with 0x4000: 8 a size not a multiple of 4, 4 not
// set up, 0x13 and 0x14 busy, 2 and 3 nothing to do
int func_02076448(u32 size) {
    u32 rest = size;
    NFLWork *work;
    NFLObj *obj;
    int result;

    if (rest & 3) {
        return 8;
    }
    work = data_02145360;
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
        result = func_02076680(&rest);
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
        result = func_02076a98(&rest);
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
            result = func_02076e1c(obj, &rest);
            if (result != 0 && result != 2) {
                obj->unk170 = 3;
                func_020763e0(work->unk74);
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
            result = func_02077300(obj, &rest);
            if (result != 0 && result != 2) {
                obj->unk170 = 3;
                func_020763e0(work->unk74);
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
            func_020763e0(obj);
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

// The VRAM side of func_02074b68's streams, after the frame's interrupt: loads up to *rest bytes of what the six
// streams have put in the ring buffer into texture VRAM (func_02074964), then hands on to the object streams
// (func_02076e1c) or the seventh stream (func_02076a98)
int func_02076680(u32 *rest) {
    NFLWork *work = (NFLWork *)data_02145360;
    u32 dest;
    NFLWorkUnkA0 *stream = work->unkA0;
    NFLStreamSource *source = data_0211216c;
    int i;

    for (i = 0; i < 6; stream++, source++, i++) {
        BOOL go = FALSE;
        u8 *src;
        u32 size;
        int result;

        switch (stream->unk8) {
        case 2:
            if (data_02145360->unk68 == 7) {
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
            if (data_02145360->unk68 == 7) {
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
                    result = func_02074964(src, dest, size);
                } else {
                    int over = stream->unk0 + size - work->unk7C;
                    int n = size - over;

                    result = func_02074964(src, dest, n);
                    if (result == 0) {
                        result = func_02074964(work->unk78, dest + n, over);
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
                    result = func_02074964(src, dest, *rest);
                } else {
                    int over = stream->unk0 + *rest - work->unk7C;
                    int n = *rest - over;

                    result = func_02074964(src, dest, n);
                    if (result == 0) {
                        result = func_02074964(work->unk78, dest + n, over);
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
    if (data_02145360->unk68 == 7) {
        work->unk90 = 2;
        if (*rest == 0) {
            return 2;
        }
        if (work->unk74 != NULL) {
            switch (work->unk74->unk170) {
            case 0:
                return func_02076e1c(work->unk74, rest);
            case 2:
            case 4:
                func_020763e0(work->unk74);
                break;
            }
        }
    } else {
        work->unk90 = 1;
        if (*rest == 0) {
            return 2;
        }
        return func_02076a98(rest);
    }
    return 2;
}

// The VRAM side of the seventh stream (func_0207516c): loads it into texture palette VRAM (func_020749a4), then
// hands on to the object streams
int func_02076a98(u32 *rest) {
    NFLWork *work = (NFLWork *)data_02145360;
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
                result = func_020749a4(src, dest, size);
            } else {
                u32 over = work->unkA0[6].unk0 + size - work->unk7C;
                u32 n = size - over;

                result = func_020749a4(src, dest, n);
                if (result == 0) {
                    result = func_020749a4(work->unk78, dest + n, over);
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
                result = func_020749a4(work->unk78 + work->unkA0[6].unk0, work->unkA0[6].unk4, *rest);
            } else {
                u32 over = work->unkA0[6].unk0 + *rest - work->unk7C;
                u32 n = *rest - over;

                result = func_020749a4(work->unk78 + work->unkA0[6].unk0, work->unkA0[6].unk4, n);
                if (result == 0) {
                    result = func_020749a4(work->unk78, work->unkA0[6].unk4 + n, over);
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
            return func_02076e1c(work->unk74, rest);
        case 1:
            return func_02077300(work->unk74, rest);
        case 2:
        case 4:
            func_020763e0(work->unk74);
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

extern const NFLObjDest data_020b7b8c[];

// The VRAM side of an object's eleven streams (func_02075528)
int func_02076e1c(NFLObj *obj, u32 *rest) {
    NFLWork *work = data_02145360;
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
            if (data_02145360->unk68 == 7) {
                if (i == 0) {
                    size = 0x80;
                } else {
                    size = data_021121b4[i].unk4 * 4;
                }
                dest = obj->unkD4 + data_020b7b8c[i].unk4;
            } else {
                size = data_021121b4[i].unk4;
                dest = obj->unkD4 + data_020b7b8c[i].unk0;
            }
            src = (u8 *)stream->unk0 + (u32)work->unk78;
            go = TRUE;
            break;
        case 3:
            src = (u8 *)stream->unk0 + (u32)work->unk78;
            dest = obj->unkD4 + (data_02145360->unk68 == 7 ? data_020b7b8c[i].unk4 : data_020b7b8c[i].unk0);
            size = obj->unk178;
            go = TRUE;
            break;
        case 4:
            size = obj->unk178 - obj->unk174;
            src = (u8 *)stream->unk0 + (u32)work->unk78;
            if (data_02145360->unk68 == 7) {
                dest = obj->unk174 + (obj->unkD4 + data_020b7b8c[i].unk4);
            } else {
                dest = obj->unk174 + (obj->unkD4 + data_020b7b8c[i].unk0);
            }
            go = TRUE;
            break;
        case 5:
            if (data_02145360->unk68 == 7) {
                if (i == 0) {
                    size = 0x80;
                } else {
                    size = data_021121b4[i].unk4 * 4;
                }
                dest = obj->unk174 + (obj->unkD4 + data_020b7b8c[i].unk4);
            } else {
                size = data_021121b4[i].unk4;
                dest = obj->unk174 + (obj->unkD4 + data_020b7b8c[i].unk0);
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
                    result = func_02074964(src, dest, size);
                } else {
                    int over = stream->unk0 + size - work->unk7C;
                    int n = size - over;

                    result = func_02074964(src, dest, n);
                    if (result == 0) {
                        result = func_02074964(work->unk78, dest + n, over);
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
                    result = func_02074964(src, dest, *rest);
                } else {
                    int over = stream->unk0 + *rest - work->unk7C;
                    int n = *rest - over;

                    result = func_02074964(src, dest, n);
                    if (result == 0) {
                        result = func_02074964(work->unk78, dest + n, over);
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
    if (data_02145360->unk68 == 7) {
        if (obj->unk17C != NULL && *rest != 0) {
            result = func_02076e1c(obj->unk17C, rest);
            if (result != 0 && result != 2) {
                return result;
            }
        }
        if (obj->unk170 != 4) {
            obj->unk170 = 2;
        }
        func_020763e0(obj);
    } else {
        obj->unk170 = 1;
        if (*rest == 0) {
            return result;
        }
        result = func_02077300(obj, rest);
        return result;
    }
    return result;
}

// Where each of an object's palette streams goes in texture palette VRAM, from the object's base (unkD8)
extern const u32 data_020b7b64[];

// The VRAM side of an object's palette streams (func_02075f50): ten of them here, where func_02075f50 fills eleven
int func_02077300(NFLObj *obj, u32 *rest) {
    NFLWork *work = (NFLWork *)data_02145360;
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
                dest = obj->unkD8 + data_020b7b64[i];
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
                dest = obj->unkD8 + data_020b7b64[i];
                DC_FlushRange(src, size);
                go = TRUE;
                break;
            case 4:
                size = obj->unk178 - obj->unk174;
                src = (u8 *)stream->unk0 + (u32)work->unk78;
                dest = obj->unk174 + (obj->unkD8 + data_020b7b64[i]);
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
                dest = obj->unk174 + (obj->unkD8 + data_020b7b64[i]);
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
                        result = func_020749a4(src, dest, size);
                    } else {
                        int over = stream->unk0 + size - work->unk7C;
                        int n = size - over;

                        result = func_020749a4(src, dest, n);
                        if (result == 0) {
                            result = func_020749a4(work->unk78, dest + n, over);
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
                        result = func_020749a4(src, dest, *rest);
                    } else {
                        int over = stream->unk0 + *rest - work->unk7C;
                        int n = *rest - over;

                        result = func_020749a4(src, dest, n);
                        if (result == 0) {
                            result = func_020749a4(work->unk78, dest + n, over);
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
        result = func_02076e1c(obj->unk17C, rest);
        if (result != 0 && result != 2) {
            return result;
        }
    }
    if (obj->unk170 != 4) {
        obj->unk170 = 2;
    }
    func_020763e0(obj);
    return result;
}

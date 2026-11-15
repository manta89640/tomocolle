// NFL, the Mii face library: the main program's code from the end of NitroSystem (0x02069BF4) to the Mii transfer
// (0x020776E4), with its data. This one file holds the library's whole range, which is not yet divided into the
// files it was built from (docs/binary.md, "The Mii library").

#include "NFL.h"

NFLWork *data_02145360;

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

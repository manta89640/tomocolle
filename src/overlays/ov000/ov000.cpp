// Overlay 0, the smallest in the game: one class of seven virtual functions, whose table is at 0x02149c68.
// Its only string is L"テスト12３" ("test 12 3"), so it is probably a test scene.
//
// The class it derives from is not decompiled yet, so its functions are written here as plain functions that take
// the object, and what they call is named by address.

#include "types.h"

struct Ov000 {
    /* 0x000 */ u8 unk_000[0x154];
    /* 0x154 */ u8 unk_154[0x2B8];
    /* 0x40C */ u8 unk_40C[0xB0];
};

extern "C" {

void func_0202e0bc(void *);
void func_0202e244(void *, s32);
void func_020267bc(void *);

void func_ov000_02149a40(Ov000 *self) {
}

void func_ov000_02149c0c(Ov000 *self) {
}

void func_ov000_02149c10(Ov000 *self) {
    func_0202e0bc(self->unk_154);
    func_0202e244(self->unk_40C, 0);
}

void func_ov000_02149c34(Ov000 *self) {
}

void func_ov000_02149c38(Ov000 *self) {
}

Ov000 *func_ov000_02149c3c(Ov000 *self) {
    func_020267bc(self);
    return self;
}

} // extern "C"

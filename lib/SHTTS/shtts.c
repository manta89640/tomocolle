#include "shtts.h"

// Messages for each value of Shtts.error
extern const char *data_020bc174[];

void func_02089cf4(Shtts *engine) {
    OS_LockMutex(&engine->mutex);
}

void func_02089d08(Shtts *engine) {
    OS_UnlockMutex(&engine->mutex);
}

// Whether the engine is synthesizing
int func_02089d1c(Shtts *engine) {
    int busy;

    OS_LockMutex(&engine->mutex);
    busy = engine->busy;
    OS_UnlockMutex(&engine->mutex);
    return busy;
}

// Sets the caller's voice settings back to their defaults
void func_0208c398(Shtts *engine) {
    engine->unk_0EF20.unk_00 = 0;
    engine->unk_0EF20.unk_02 = 0x13C2;
    engine->unk_0EF20.unk_04 = 100;
    engine->unk_0EF20.unk_06 = 100;
    engine->unk_0EF20.unk_0A = 100;
    engine->unk_0EF20.unk_08 = 0;
    engine->unk_0EF20.unk_24 = 100;
    engine->unk_0EF20.unk_28 = 0x400;
    engine->unk_0EF20.unk_2A = 10;
    engine->unk_0EF20.unk_14 = 0;
    engine->unk_0EF20.unk_10 = 0;
    engine->unk_0EF20.unk_0E = 0;
    engine->unk_0EF20.unk_0C = 0;
    engine->unk_0EF20.unk_1C = 0;
    engine->unk_0EF20.unk_16 = 0;
    engine->unk_0EF20.unk_20 = 0;
    engine->unk_0EF20.unk_18 = 0;
    engine->unk_0EF20.unk_2C = 0;
}

void func_0208d4f0(Shtts *engine, ShttsCallback callback, void *arg) {
    engine->callback = callback;
    engine->callbackArg = arg;
}

// Asks the engine to stop synthesizing, and waits until it has, unless called from the engine's own thread
int func_0208d878(Shtts *engine) {
    int stopping = 0;
    OSThread *current;

    func_02089cf4(engine);
    if (engine->busy == 1) {
        stopping = 1;
        engine->stopRequested = 1;
    }
    func_02089d08(engine);

    current = OS_GetCurrentThread();
    if (stopping == 1 && current != &engine->thread) {
        while (func_02089d1c(engine) == 1) {
            OS_Sleep(10);
        }
    }
    return 0;
}

// Sets up an engine in memory the caller provides.
// Not matching yet: the same instructions, but the original keeps the pointer of the loop below in r5 and the
// engine's address plus 0xB000 in lr, and this has them the other way round.
int func_0208d8fc(Shtts *engine) {
    int i;
    u8 *p;

    MIi_CpuClear32(0, engine, sizeof(Shtts));
    engine->unk_00000 = 11000;
    engine->unk_00004 = 0x37;
    engine->unk_00008 = 0;
    engine->unk_0000C = 0;
    engine->unk_0EF50.unk_00 = 0;
    engine->unk_0EF50.unk_02 = 0x13C2;
    engine->unk_0EF50.unk_04 = 100;
    engine->unk_0EF50.unk_06 = 100;
    engine->unk_0EF50.unk_0A = 120;
    engine->unk_0EF50.unk_08 = 0;
    engine->unk_01B21 = 0;
    engine->unk_0EF50.unk_0C = 30;
    engine->unk_0EF50.unk_0E = 0;
    engine->unk_0EF50.unk_14 = 0;
    engine->unk_08CE4 = 0;
    engine->unk_0EF50.unk_16 = 0;
    engine->unk_0EF50.unk_18 = 0;
    engine->unk_0EF50.unk_1C = 0;
    engine->unk_0EF50.unk_20 = 0;
    engine->unk_0B6FC = 0;
    engine->unk_08906 = 0;
    engine->unk_08CDC = 480;
    engine->unk_08CDE = 240;
    engine->unk_08CE0 = 0;
    engine->unk_0EF50.unk_14 = 0;
    engine->unk_08CE4 = 0;
    engine->unk_0EF50.unk_16 = 0;
    engine->unk_0EF50.unk_18 = 0;
    engine->unk_0EF50.unk_1C = 0;
    engine->unk_0EF50.unk_20 = 0;
    engine->unk_0B6FC = 0;
    p = engine->unk_0B730;
    engine->unk_0B720 = 0;
    engine->unk_08722 = 0;
    engine->unk_0B728 = 0;
    engine->unk_0B729 = 0;
    engine->unk_0B72A = 1;
    engine->unk_0B72B = 1;
    engine->unk_0B72C = 0;
    for (i = 0; i < 4; i++) {
        p[0] = 0;
        p[1] = 0;
        p[2] = 0;
        p[3] = 0;
        p += 4;
    }
    engine->unk_0E624 = 0;
    engine->unk_0EE00 = 0;
    engine->unk_0EE04 = 0;
    engine->unk_0EE08 = 0;
    engine->unk_0EE0C = 0;
    engine->busy = 0;
    engine->stopRequested = 0;
    engine->unk_0EE11 = 0;
    engine->unk_0EE14 = 0;
    engine->unk_0EF50.unk_24 = 100;
    engine->unk_0EF50.unk_28 = 0x400;
    engine->unk_0EF50.unk_2A = 10;
    engine->unk_0EE18 = 1;
    OS_InitMutex(&engine->mutex);
    OS_InitMutex(&engine->unk_0EF08);
    engine->error = 0;
    engine->unk_0B72D = 0;
    func_0208dab4(engine);
    return 0;
}

int func_0208dab4(Shtts *engine) {
    func_0208c398(engine);
    return 0;
}

// 0 when idle, 1 while synthesizing, 2 while synthesizing and asked to stop
int func_0208dac4(Shtts *engine) {
    int state;

    func_02089cf4(engine);
    state = engine->busy;
    if (state == 1 && engine->stopRequested == 1) {
        state = 2;
    }
    func_02089d08(engine);
    return state;
}

// The message for the engine's last error
const char *func_0208daf8(Shtts *engine) {
    if (engine->error < 0 || engine->error > 38) {
        return data_020bc174[7];
    }
    return data_020bc174[engine->error];
}

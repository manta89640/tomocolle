#include "MainHeap.h"

MainHeap gMainHeap;

void *operator new(size_t size) {
    return gMainHeap.Alloc(size);
}

void *operator new[](size_t size) {
    return gMainHeap.Alloc(size);
}

void operator delete(void *block) {
    gMainHeap.Free(block);
}

extern "C" void *AllocFromMainHeap(u32 size, int align) {
    return gMainHeap.Alloc(size, align);
}

extern "C" void FreeToMainHeap(void *block) {
    gMainHeap.Free(block);
}

extern "C" void CreateMainFrameHeap(u32 size) {
    gMainHeap.CreateFrameHeap(size);
}

extern "C" void *AllocFromMainFrameHeap(u32 size) {
    return gMainHeap.AllocFromFrameHeap(size);
}

extern "C" void DestroyMainFrameHeap(void) {
    gMainHeap.DestroyFrameHeap();
}

void MainHeap::Init() {
    OS_InitMutex(&mMutex);
    OS_InitMutex(&mFrameHeapMutex);
    void *lo = OS_GetArenaLo(OS_ARENA_MAIN);
    void *hi = OS_GetArenaHi(OS_ARENA_MAIN);
    u32 size = (u32)MATH_ROUNDDOWN((u32)hi, 16) - (u32)MATH_ROUNDUP((u32)lo, 16);
    mHeap = NNS_FndCreateExpHeap(OS_AllocFromArenaLo(OS_ARENA_MAIN, size, 16), size);
}

void *MainHeap::Alloc(u32 size) {
    OS_LockMutex(&mMutex);
    void *block = NNS_FndAllocFromExpHeapEx(mHeap, size, 4);
    OS_UnlockMutex(&mMutex);
    return block;
}

void *MainHeap::Alloc(u32 size, int align) {
    OS_LockMutex(&mMutex);
    void *block = NNS_FndAllocFromExpHeapEx(mHeap, size, align);
    OS_UnlockMutex(&mMutex);
    return block;
}

void MainHeap::Free(void *block) {
    OS_LockMutex(&mMutex);
    if (block != NULL) {
        NNS_FndFreeToExpHeap(mHeap, block);
    }
    OS_UnlockMutex(&mMutex);
}

void MainHeap::CreateFrameHeap(u32 size) {
    OS_LockMutex(&mFrameHeapMutex);
    mFrameHeapMemory = AllocFromMainHeap(size, 4);
    mFrameHeap = NNS_FndCreateFrmHeap(mFrameHeapMemory, size);
    OS_UnlockMutex(&mFrameHeapMutex);
}

void MainHeap::DestroyFrameHeap() {
    OS_LockMutex(&mFrameHeapMutex);
    if (mFrameHeap != NULL) {
        NNS_FndDestroyFrmHeap(mFrameHeap);
        mFrameHeap = NULL;
    }
    if (mFrameHeapMemory != NULL) {
        FreeToMainHeap(mFrameHeapMemory);
        mFrameHeapMemory = NULL;
    }
    OS_UnlockMutex(&mFrameHeapMutex);
}

void *MainHeap::AllocFromFrameHeap(u32 size) {
    OS_LockMutex(&mFrameHeapMutex);
    void *block = NNS_FndAllocFromFrmHeapEx(mFrameHeap, size, 4);
    OS_UnlockMutex(&mFrameHeapMutex);
    return block;
}

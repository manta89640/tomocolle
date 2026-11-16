#ifndef MAINHEAP_H
#define MAINHEAP_H

#include <nitro.h>
#include <nnsys.h>

// The heap that holds the whole main arena: an expanded heap, and a frame heap that can be made inside it, each
// behind a mutex. operator new and delete allocate from it. NitroStartUp initialises it before the static
// initialisers run.
//
// The name is descriptive, not the original: no name of this class survives in the ROM.
class MainHeap {
public:
    MainHeap() : mFrameHeap(NULL), mFrameHeapMemory(NULL) {}
    void Init();
    void *Alloc(u32 size);
    void *Alloc(u32 size, int align);
    void Free(void *block);
    void CreateFrameHeap(u32 size);
    void DestroyFrameHeap();
    void *AllocFromFrameHeap(u32 size);

    /* 0x00 */ NNSFndHeapHandle mHeap;            // The expanded heap over the whole main arena
    /* 0x04 */ OSMutex mMutex;
    /* 0x1C */ NNSFndHeapHandle mFrameHeap;       // A frame heap, while there is one
    /* 0x20 */ void *mFrameHeapMemory;            // Its memory, from mHeap
    /* 0x24 */ OSMutex mFrameHeapMutex;
}; // 0x3C

extern MainHeap gMainHeap;

extern "C" {
void *AllocFromMainHeap(u32 size, int align);
// C functions on gMainHeap, for the sources of the scenes and overlays
void FreeToMainHeap(void *block);  // Frees a block of the main heap
void CreateMainFrameHeap(u32 size);     // Makes the frame heap, of size bytes
void *AllocFromMainFrameHeap(u32 size);    // Allocates from the frame heap
void DestroyMainFrameHeap(void);         // Destroys the frame heap
}

#endif // MAINHEAP_H

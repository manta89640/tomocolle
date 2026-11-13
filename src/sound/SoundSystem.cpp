#include "sound/SoundSystem.h"

extern "C" {
// Allocates from the heap that holds the whole main arena (gMainHeap); not decompiled yet
void *AllocFromMainHeap(u32 size, int align);
}

void SoundSystem::Init(const char *archivePath, u32 heapSize) {
    NNS_SndInit();
    for (int i = 0; i < HEAP_STATE_COUNT; i++) {
        mHeapStates[i] = 0;
    }
    mHeapMemory = AllocFromMainHeap(heapSize, 4);
    mHeap = NNS_SndHeapCreate(mHeapMemory, heapSize);
    NNS_SndArcInit(&mArchive, archivePath, mHeap, FALSE);
    NNS_SndArcPlayerSetup(mHeap);
    NNS_SndArcStrmInit(STRM_THREAD_PRIORITY, mHeap);
    for (int i = 0; i < SEQ_HANDLE_COUNT; i++) {
        NNS_SndHandleInit(&mSeqHandles[i]);
    }
    for (int i = 0; i < SEQARC_HANDLE_COUNT; i++) {
        NNS_SndHandleInit(&mSeqArcHandles[i]);
    }
    NNS_SndStrmHandleInit(&mStrmHandles[0]);
}

BOOL SoundSystem::StartSeqIfNotPlaying(int handle, int seqNo) {
    if (NNS_SndPlayerCountPlayingSeqBySeqNo(seqNo) != 0) {
        return FALSE;
    }
    NNS_SndArcPlayerStartSeq(&mSeqHandles[handle], seqNo);
    return TRUE;
}

void SoundSystem::StopAllSeq(int fadeFrames) {
    for (int i = 0; i < SEQ_HANDLE_COUNT; i++) {
        NNS_SndPlayerStopSeq(&mSeqHandles[i], fadeFrames);
    }
}

// Unmutes every track, then sets the mute of the given ones
void SoundSystem::SetSeqTrackMuteOnly(int handle, u16 trackBitMask, BOOL mute) {
    NNS_SndPlayerSetTrackMute(&mSeqHandles[handle], 0xFFFF, FALSE);
    NNS_SndPlayerSetTrackMute(&mSeqHandles[handle], trackBitMask, mute);
}

BOOL SoundSystem::StartSeqArcIfNotPlaying(int handle, int seqArcNo, int index) {
    return StartSeqArcLimited(handle, seqArcNo, index, 1);
}

// Starts the entry unless `maxPlaying` of it are playing already
BOOL SoundSystem::StartSeqArcLimited(int handle, int seqArcNo, int index, int maxPlaying) {
    if (NNS_SndPlayerCountPlayingSeqBySeqArcIdx(seqArcNo, index) >= maxPlaying) {
        return FALSE;
    }
    NNS_SndArcPlayerStartSeqArc(&mSeqArcHandles[handle], seqArcNo, index);
    return TRUE;
}

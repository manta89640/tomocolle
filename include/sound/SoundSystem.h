#ifndef SOUNDSYSTEM_H
#define SOUNDSYSTEM_H

#include <nitro.h>
#include <nnsys.h>

// The sound library's setup and what it plays with: the sound heap, the sound archive, and the handles of the
// sequences (played by sequence number), of the sequence archive entries, and of the archive's stream.
// SoundManager (SoundManager.h) is built on it and is what the game uses.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.
class SoundSystem {
public:
    enum {
        SEQ_HANDLE_COUNT = 2,
        SEQARC_HANDLE_COUNT = 6,
        STRM_HANDLE_COUNT = 1,
        HEAP_STATE_COUNT = 3,
        STRM_THREAD_PRIORITY = 10,
    };

    void Init(const char *archivePath, u32 heapSize);
    BOOL StartSeqIfNotPlaying(int handle, int seqNo);
    void StopAllSeq(int fadeFrames);
    void SetSeqTrackMuteOnly(int handle, u16 trackBitMask, BOOL mute);
    BOOL StartSeqArcIfNotPlaying(int handle, int seqArcNo, int index);
    BOOL StartSeqArcLimited(int handle, int seqArcNo, int index, int maxPlaying);

    /* 0x00 */ void *mHeapMemory;
    /* 0x04 */ NNSSndHeapHandle mHeap;
    /* 0x08 */ NNSSndArc mArchive;
    /* 0x9C */ NNSSndHandle mSeqHandles[SEQ_HANDLE_COUNT];       // For NNS_SndArcPlayerStartSeq
    /* 0xA4 */ NNSSndHandle mSeqArcHandles[SEQARC_HANDLE_COUNT]; // For NNS_SndArcPlayerStartSeqArc
    /* 0xBC */ NNSSndStrmHandle mStrmHandles[STRM_HANDLE_COUNT]; // For NNS_SndArcStrmStart
    /* 0xC0 */ int mHeapStates[HEAP_STATE_COUNT];                // Levels from NNS_SndHeapSaveState, by slot
};

#endif // SOUNDSYSTEM_H

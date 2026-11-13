#ifndef SOUNDMANAGER_H
#define SOUNDMANAGER_H

#include "sound/SoundSystem.h"

// The game's sound manager (gSoundManager): SoundSystem with a mutex around every use of the sound
// library, and with the volumes of the player's settings applied to whatever it starts.
//
// The names are descriptive, not the original: no name of this class survives in the ROM.
class SoundManager : public SoundSystem {
public:
    void Init(const char *archivePath, u32 heapSize);
    BOOL LoadGroup(int groupNo);
    void ApplyOutputSetting();
    void SetMono(BOOL mono);
    int GetVoiceStrmVolume();
    int GetVoiceVolume();

    void StopSeqArcEntry(int seqArcNo, int index, int fadeFrames);
    int CountPlayingSeq(int seqNo);
    int CountPlayingSeqArcEntry(int seqArcNo, int index);

    BOOL StartSeq(int handle, int seqNo);
    void StartSeqIfNotPlaying(int handle, int seqNo);
    void StopSeq(int handle, int fadeFrames);
    void StopAllSeq(int fadeFrames);
    BOOL ReadSeqVariable(int handle, int varNo, s16 *var);
    BOOL WriteSeqVariable(int handle, int varNo, s16 var);
    u32 GetSeqTick(int handle);
    void SetSeqTempoRatio(int handle, int ratio);
    void SetSeqTrackPitch(int handle, u16 trackBitMask, int pitch);
    void SetSeqTrackMute(int handle, u16 trackBitMask, BOOL mute);
    void SetSeqTrackMuteOnly(int handle, u16 trackBitMask, BOOL mute);
    void SetSeqTrackVolume(int handle, u16 trackBitMask, int volume);
    void SetSeqTrackModDepth(int handle, u16 trackBitMask, int depth);
    void SetSeqVolume(int handle, int volume);
    void MoveSeqVolume(int handle, int targetVolume, int frames);
    void ApplySeqInitialVolume(int handle, int seqNo);
    int GetSeqInitialVolume(int seqNo);

    void StartSeqArc(int handle, int seqArcNo, int index);
    void StartSeqArcIfNotPlaying(int handle, int seqArcNo, int index);
    void StartSeqArcLimited(int handle, int seqArcNo, int index, int maxPlaying);
    void StopSeqArc(int handle, int fadeFrames);
    void SetSeqArcTempoRatio(int handle, int ratio);
    void SetSeqArcTrackPitch(int handle, u16 trackBitMask, int pitch);
    void SetSeqArcTrackMute(int handle, u16 trackBitMask, BOOL mute);
    void SetSeqArcTrackPan(int handle, u16 trackBitMask, int pan);
    void SetSeqArcVolume(int handle, int volume);
    int GetSeqArcInitialVolume(int seqArcNo, int index);

    BOOL StartStrm(int handle, int strmNo, u32 offset);
    void StopStrm(int handle, int fadeFrames);
    u32 GetStrmPlayingPos(int handle);
    int GetStrmVolume(int strmNo);

    void SaveHeapState(int slot);
    void LoadHeapState(int slot);
    int GetHeapLevel();

    /* 0xCC */ OSMutex mMutex; // Guards the sound library; TtsPlayer locks it too
};

extern SoundManager gSoundManager;

#endif // SOUNDMANAGER_H

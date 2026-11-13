#include "sound/SoundManager.h"

// The part of the game's settings (gSettings) that these functions use. The object is not this file's; only
// these fields of it are known here.
struct SoundSettings {
    /* 0x00 */ u8 unknown00[8];
    /* 0x08 */ int unknownState;  // Init sets the defaults below when this is 3
    /* 0x0C */ u8 unknown0C[0xBC];
    /* 0xC8 */ u8 stereo;         // Not zero: stereo output (default 1)
    /* 0xC9 */ u8 musicVolume;    // 0 to 3, for sequences and streams (default 3)
    /* 0xCA */ u8 effectVolume;   // 0 to 3, for sequence archive entries (default 3)
    /* 0xCB */ u8 voiceVolume;    // 0 to 3, for the speech synthesizer (default 3)
};

extern "C" {
extern SoundSettings gSettings;
}

// What a volume setting (0 to 3) means. The order is not the ROM's: the first two land the other way round there.
static const s8 sVolumeBySetting[4] = { 0, 42, 84, 127 };          // A volume, for a sound with none of its own
static const s8 sVolumeScaleBySetting[4] = { 0, 11, 21, 32 };      // A scale in 32nds for a sound's own volume
static const s16 sStrmVolumeBySetting[4] = { -10000, -180, -60, 0 }; // The voice stream's channel volume

void SoundManager::Init(const char *archivePath, u32 heapSize) {
    OS_InitMutex(&mMutex);
    if (gSettings.unknownState == 3) {
        gSettings.stereo = 1;
        gSettings.musicVolume = 3;
        gSettings.effectVolume = 3;
        gSettings.voiceVolume = 3;
    }
    OS_LockMutex(&mMutex);
    SoundSystem::Init(archivePath, heapSize);
    OS_UnlockMutex(&mMutex);
    ApplyOutputSetting();
}

BOOL SoundManager::LoadGroup(int groupNo) {
    OS_LockMutex(&mMutex);
    BOOL result = NNS_SndArcLoadGroup(groupNo, mHeap);
    OS_UnlockMutex(&mMutex);
    return result;
}

// Sets mono or stereo output from the settings
void SoundManager::ApplyOutputSetting() {
    SetMono(gSettings.stereo == 0);
}

void SoundManager::SetMono(BOOL mono) {
    OS_LockMutex(&mMutex);
    NNS_SndSetMonoFlag(mono);
    OS_UnlockMutex(&mMutex);
}

// The voice volume setting as a stream channel's volume (TtsPlayer gives it to NNS_SndStrmSetChannelVolume)
int SoundManager::GetVoiceStrmVolume() {
    return sStrmVolumeBySetting[gSettings.voiceVolume];
}

// The voice volume setting as a volume from 0 to 127 (TtsPlayer gives it to NNS_SndWaveOutStart)
int SoundManager::GetVoiceVolume() {
    return sVolumeBySetting[gSettings.voiceVolume];
}

void SoundManager::StopSeqArcEntry(int seqArcNo, int index, int fadeFrames) {
    OS_LockMutex(&mMutex);
    NNS_SndPlayerStopSeqBySeqArcIdx(seqArcNo, index, fadeFrames);
    OS_UnlockMutex(&mMutex);
}

int SoundManager::CountPlayingSeq(int seqNo) {
    OS_LockMutex(&mMutex);
    int count = NNS_SndPlayerCountPlayingSeqBySeqNo(seqNo);
    OS_UnlockMutex(&mMutex);
    return count;
}

int SoundManager::CountPlayingSeqArcEntry(int seqArcNo, int index) {
    OS_LockMutex(&mMutex);
    int count = NNS_SndPlayerCountPlayingSeqBySeqArcIdx(seqArcNo, index);
    OS_UnlockMutex(&mMutex);
    return count;
}

BOOL SoundManager::StartSeq(int handle, int seqNo) {
    OS_LockMutex(&mMutex);
    BOOL result = NNS_SndArcPlayerStartSeq(&mSeqHandles[handle], seqNo);
    NNS_SndPlayerSetInitialVolume(&mSeqHandles[handle], GetSeqInitialVolume(seqNo));
    OS_UnlockMutex(&mMutex);
    return result;
}

void SoundManager::StartSeqIfNotPlaying(int handle, int seqNo) {
    OS_LockMutex(&mMutex);
    if (SoundSystem::StartSeqIfNotPlaying(handle, seqNo)) {
        NNS_SndPlayerSetInitialVolume(&mSeqHandles[handle], GetSeqInitialVolume(seqNo));
    }
    OS_UnlockMutex(&mMutex);
}

void SoundManager::StopSeq(int handle, int fadeFrames) {
    OS_LockMutex(&mMutex);
    NNS_SndPlayerStopSeq(&mSeqHandles[handle], fadeFrames);
    OS_UnlockMutex(&mMutex);
}

void SoundManager::StopAllSeq(int fadeFrames) {
    OS_LockMutex(&mMutex);
    SoundSystem::StopAllSeq(fadeFrames);
    OS_UnlockMutex(&mMutex);
}

BOOL SoundManager::ReadSeqVariable(int handle, int varNo, s16 *var) {
    OS_LockMutex(&mMutex);
    BOOL result = NNS_SndPlayerReadVariable(&mSeqHandles[handle], varNo, var);
    OS_UnlockMutex(&mMutex);
    return result;
}

BOOL SoundManager::WriteSeqVariable(int handle, int varNo, s16 var) {
    OS_LockMutex(&mMutex);
    BOOL result = NNS_SndPlayerWriteVariable(&mSeqHandles[handle], varNo, var);
    OS_UnlockMutex(&mMutex);
    return result;
}

u32 SoundManager::GetSeqTick(int handle) {
    OS_LockMutex(&mMutex);
    u32 tick = NNS_SndPlayerGetTick(&mSeqHandles[handle]);
    OS_UnlockMutex(&mMutex);
    return tick;
}

void SoundManager::SetSeqTempoRatio(int handle, int ratio) {
    OS_LockMutex(&mMutex);
    NNS_SndPlayerSetTempoRatio(&mSeqHandles[handle], ratio);
    OS_UnlockMutex(&mMutex);
}

void SoundManager::SetSeqTrackPitch(int handle, u16 trackBitMask, int pitch) {
    OS_LockMutex(&mMutex);
    NNS_SndPlayerSetTrackPitch(&mSeqHandles[handle], trackBitMask, pitch);
    OS_UnlockMutex(&mMutex);
}

void SoundManager::SetSeqTrackMute(int handle, u16 trackBitMask, BOOL mute) {
    OS_LockMutex(&mMutex);
    NNS_SndPlayerSetTrackMute(&mSeqHandles[handle], trackBitMask, mute);
    OS_UnlockMutex(&mMutex);
}

void SoundManager::SetSeqTrackMuteOnly(int handle, u16 trackBitMask, BOOL mute) {
    OS_LockMutex(&mMutex);
    SoundSystem::SetSeqTrackMuteOnly(handle, trackBitMask, mute);
    OS_UnlockMutex(&mMutex);
}

void SoundManager::SetSeqTrackVolume(int handle, u16 trackBitMask, int volume) {
    OS_LockMutex(&mMutex);
    NNS_SndPlayerSetTrackVolume(&mSeqHandles[handle], trackBitMask, volume);
    OS_UnlockMutex(&mMutex);
}

void SoundManager::SetSeqTrackModDepth(int handle, u16 trackBitMask, int depth) {
    OS_LockMutex(&mMutex);
    NNS_SndPlayerSetTrackModDepth(&mSeqHandles[handle], trackBitMask, depth);
    OS_UnlockMutex(&mMutex);
}

void SoundManager::SetSeqVolume(int handle, int volume) {
    OS_LockMutex(&mMutex);
    NNS_SndPlayerSetVolume(&mSeqHandles[handle], volume);
    OS_UnlockMutex(&mMutex);
}

void SoundManager::MoveSeqVolume(int handle, int targetVolume, int frames) {
    OS_LockMutex(&mMutex);
    NNS_SndPlayerMoveVolume(&mSeqHandles[handle], targetVolume, frames);
    OS_UnlockMutex(&mMutex);
}

// Sets the handle's initial volume to what `seqNo` gets under the music volume setting
void SoundManager::ApplySeqInitialVolume(int handle, int seqNo) {
    OS_LockMutex(&mMutex);
    NNS_SndPlayerSetInitialVolume(&mSeqHandles[handle], GetSeqInitialVolume(seqNo));
    OS_UnlockMutex(&mMutex);
}

// The sequence's own volume scaled by the music volume setting
int SoundManager::GetSeqInitialVolume(int seqNo) {
    const NNSSndSeqParam *param = NNS_SndArcGetSeqParam(seqNo);
    u8 setting = gSettings.musicVolume;
    if (param == NULL) {
        return sVolumeBySetting[setting];
    }
    return (param->volume * sVolumeScaleBySetting[setting]) >> 5;
}

void SoundManager::StartSeqArc(int handle, int seqArcNo, int index) {
    OS_LockMutex(&mMutex);
    NNS_SndArcPlayerStartSeqArc(&mSeqArcHandles[handle], seqArcNo, index);
    NNS_SndPlayerSetInitialVolume(&mSeqArcHandles[handle], GetSeqArcInitialVolume(seqArcNo, index));
    OS_UnlockMutex(&mMutex);
}

void SoundManager::StartSeqArcIfNotPlaying(int handle, int seqArcNo, int index) {
    OS_LockMutex(&mMutex);
    if (SoundSystem::StartSeqArcIfNotPlaying(handle, seqArcNo, index)) {
        NNS_SndPlayerSetInitialVolume(&mSeqArcHandles[handle], GetSeqArcInitialVolume(seqArcNo, index));
    }
    OS_UnlockMutex(&mMutex);
}

void SoundManager::StartSeqArcLimited(int handle, int seqArcNo, int index, int maxPlaying) {
    OS_LockMutex(&mMutex);
    if (SoundSystem::StartSeqArcLimited(handle, seqArcNo, index, maxPlaying)) {
        NNS_SndPlayerSetInitialVolume(&mSeqArcHandles[handle], GetSeqArcInitialVolume(seqArcNo, index));
    }
    OS_UnlockMutex(&mMutex);
}

void SoundManager::StopSeqArc(int handle, int fadeFrames) {
    OS_LockMutex(&mMutex);
    NNS_SndPlayerStopSeq(&mSeqArcHandles[handle], fadeFrames);
    OS_UnlockMutex(&mMutex);
}

void SoundManager::SetSeqArcTempoRatio(int handle, int ratio) {
    OS_LockMutex(&mMutex);
    NNS_SndPlayerSetTempoRatio(&mSeqArcHandles[handle], ratio);
    OS_UnlockMutex(&mMutex);
}

void SoundManager::SetSeqArcTrackPitch(int handle, u16 trackBitMask, int pitch) {
    OS_LockMutex(&mMutex);
    NNS_SndPlayerSetTrackPitch(&mSeqArcHandles[handle], trackBitMask, pitch);
    OS_UnlockMutex(&mMutex);
}

void SoundManager::SetSeqArcTrackMute(int handle, u16 trackBitMask, BOOL mute) {
    OS_LockMutex(&mMutex);
    NNS_SndPlayerSetTrackMute(&mSeqArcHandles[handle], trackBitMask, mute);
    OS_UnlockMutex(&mMutex);
}

void SoundManager::SetSeqArcTrackPan(int handle, u16 trackBitMask, int pan) {
    OS_LockMutex(&mMutex);
    NNS_SndPlayerSetTrackPan(&mSeqArcHandles[handle], trackBitMask, pan);
    OS_UnlockMutex(&mMutex);
}

void SoundManager::SetSeqArcVolume(int handle, int volume) {
    OS_LockMutex(&mMutex);
    NNS_SndPlayerSetVolume(&mSeqArcHandles[handle], volume);
    OS_UnlockMutex(&mMutex);
}

// The entry's own volume scaled by the effect volume setting
int SoundManager::GetSeqArcInitialVolume(int seqArcNo, int index) {
    const NNSSndSeqParam *param = NNS_SndArcGetSeqArcParam(seqArcNo, index);
    u8 setting = gSettings.effectVolume;
    if (param == NULL) {
        return sVolumeBySetting[setting];
    }
    return (param->volume * sVolumeScaleBySetting[setting]) >> 5;
}

BOOL SoundManager::StartStrm(int handle, int strmNo, u32 offset) {
    OS_LockMutex(&mMutex);
    BOOL result = NNS_SndArcStrmStart(&mStrmHandles[handle], strmNo, offset);
    NNS_SndArcStrmSetVolume(&mStrmHandles[handle], GetStrmVolume(strmNo));
    OS_UnlockMutex(&mMutex);
    return result;
}

void SoundManager::StopStrm(int handle, int fadeFrames) {
    OS_LockMutex(&mMutex);
    NNS_SndArcStrmStop(&mStrmHandles[handle], fadeFrames);
    OS_UnlockMutex(&mMutex);
}

u32 SoundManager::GetStrmPlayingPos(int handle) {
    OS_LockMutex(&mMutex);
    u32 pos = NNS_SndArcStrmGetCurrentPlayingPos(&mStrmHandles[handle]);
    OS_UnlockMutex(&mMutex);
    return pos;
}

// The stream's own volume scaled by the music volume setting
int SoundManager::GetStrmVolume(int strmNo) {
    const NNSSndArcStrmInfo *info = NNS_SndArcGetStrmInfo(strmNo);
    u8 setting = gSettings.musicVolume;
    if (info == NULL) {
        return sVolumeBySetting[setting];
    }
    return (info->volume * sVolumeScaleBySetting[setting]) >> 5;
}

void SoundManager::SaveHeapState(int slot) {
    OS_LockMutex(&mMutex);
    mHeapStates[slot] = NNS_SndHeapSaveState(mHeap);
    OS_UnlockMutex(&mMutex);
}

void SoundManager::LoadHeapState(int slot) {
    OS_LockMutex(&mMutex);
    NNS_SndHeapLoadState(mHeap, mHeapStates[slot]);
    OS_UnlockMutex(&mMutex);
}

int SoundManager::GetHeapLevel() {
    OS_LockMutex(&mMutex);
    int level = NNS_SndHeapGetCurrentLevel(mHeap);
    OS_UnlockMutex(&mMutex);
    return level;
}

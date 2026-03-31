#include "ps3_audio.h"
#include <cell/audio.h>
#include <cell/mp3dec.h>
#include <sys/ppu_thread.h>
#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AUDIO_PORT_CHANNELS 2
#define AUDIO_BLOCK_SAMPLES 256
#define AUDIO_BUFFER_SIZE (AUDIO_BLOCK_SAMPLES * AUDIO_PORT_CHANNELS * sizeof(float))

static uint32_t audioPort;
static bool audioRunning = false;
static sys_ppu_thread_t audioThread;
static float* audioMixBuffer;

// MultiStream state
#define MAX_STREAMS 8
typedef struct {
    float* pcmData;
    size_t sampleCount;
    size_t currentSample;
    bool active;
    float volume;
} AudioStream;

static AudioStream streams[MAX_STREAMS];

static void audioThreadFunc(uint64_t arg) {
    (void)arg;

    while (audioRunning) {
        memset(audioMixBuffer, 0, AUDIO_BUFFER_SIZE);

        for (int i = 0; i < MAX_STREAMS; i++) {
            if (streams[i].active) {
                for (int s = 0; s < AUDIO_BLOCK_SAMPLES * AUDIO_PORT_CHANNELS; s++) {
                    if (streams[i].currentSample < streams[i].sampleCount) {
                        audioMixBuffer[s] += streams[i].pcmData[streams[i].currentSample++] * streams[i].volume;
                    } else {
                        streams[i].active = false;
                        break;
                    }
                }
            }
        }

        // Wait for port to be ready and write
        cellAudioAdd64byteBoundary(0); // Dummy for alignment if needed
        cellAudioPortWrite(audioPort, audioMixBuffer);
    }

    sys_ppu_thread_exit(0);
}

void PS3Audio_Init() {
    cellAudioInit();

    CellAudioPortParam portParam;
    portParam.nChannel = AUDIO_PORT_CHANNELS;
    portParam.nBlock = 8;
    portParam.attr = 0;

    cellAudioPortOpen(&portParam, &audioPort);

    audioMixBuffer = (float*)memalign(64, AUDIO_BUFFER_SIZE);
    memset(streams, 0, sizeof(streams));

    audioRunning = true;
    sys_ppu_thread_create(&audioThread, audioThreadFunc, 0, 1000, 16 * 1024, SYS_PPU_THREAD_CREATE_JOINABLE, "AudioThread");

    cellAudioPortStart(audioPort);
}

void PS3Audio_Exit() {
    audioRunning = false;
    uint64_t exitVal;
    sys_ppu_thread_join(audioThread, &exitVal);

    cellAudioPortStop(audioPort);
    cellAudioPortClose(audioPort);
    cellAudioQuit();

    free(audioMixBuffer);
}

// MP3 Decoding helper using cellMp3Dec
static float* decodeMP3(const void* data, size_t size, size_t* outSamples) {
    CellMp3Handle handle;
    CellMp3ThreadInParam threadIn;
    CellMp3ThreadOutParam threadOut;
    CellMp3Info info;

    threadIn.enable = 0;
    threadOut.enable = 0;

    if (cellMp3Init() != CELL_OK) return NULL;
    if (cellMp3Create(&handle, &threadIn, &threadOut) != CELL_OK) return NULL;

    CellMp3Source src;
    src.srcSelect = CELL_MP3_SRC_SELECT_MEMORY;
    src.u.memSrc.addr = (void*)data;
    src.u.memSrc.size = size;

    if (cellMp3Open(handle, &src, &info) != CELL_OK) {
        cellMp3Destroy(handle);
        return NULL;
    }

    // Estimate sample count (simplified)
    *outSamples = info.totalSamples * info.numChannels;
    float* pcmData = (float*)malloc(*outSamples * sizeof(float));
    if (!pcmData) {
        cellMp3Close(handle);
        cellMp3Destroy(handle);
        return NULL;
    }

    CellMp3DecodeInParam inParam;
    CellMp3DecodeOutParam outParam;
    inParam.command = CELL_MP3_COMMAND_ALL;

    size_t samplesDecoded = 0;
    while (samplesDecoded < *outSamples) {
        outParam.addr = pcmData + samplesDecoded;
        if (cellMp3Read(handle, &inParam, &outParam) != CELL_OK) {
            break;
        }
        if (outParam.numSamples == 0) break;
        samplesDecoded += outParam.numSamples * info.numChannels;
    }
    *outSamples = samplesDecoded;

    cellMp3Close(handle);
    cellMp3Destroy(handle);

    return pcmData;
}

void PS3Audio_PlayMusic(const void* data, size_t size) {
    size_t samples;
    float* pcm = decodeMP3(data, size, &samples);
    if (!pcm) return;

    // Usually music is in stream 0
    if (streams[0].pcmData) free(streams[0].pcmData);
    streams[0].pcmData = pcm;
    streams[0].sampleCount = samples;
    streams[0].currentSample = 0;
    streams[0].volume = 1.0f;
    streams[0].active = true;
}

void PS3Audio_StopMusic() {
    streams[0].active = false;
}

bool PS3Audio_IsMusicPlaying() {
    return streams[0].active;
}

void PS3Audio_SetMusicVolume(int volume) {
    streams[0].volume = volume / 255.0f;
}

void PS3Audio_PlaySFX(const void* data, size_t size) {
    size_t samples;
    float* pcm = decodeMP3(data, size, &samples);
    if (!pcm) return;

    // Find free stream starting from 1
    for (int i = 1; i < MAX_STREAMS; i++) {
        if (!streams[i].active) {
            if (streams[i].pcmData) free(streams[i].pcmData);
            streams[i].pcmData = pcm;
            streams[i].sampleCount = samples;
            streams[i].currentSample = 0;
            streams[i].volume = 1.0f;
            streams[i].active = true;
            return;
        }
    }
    free(pcm); // No free streams
}

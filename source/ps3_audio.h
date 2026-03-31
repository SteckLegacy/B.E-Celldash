#ifndef PS3_AUDIO_H
#define PS3_AUDIO_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void PS3Audio_Init();
void PS3Audio_Exit();

// Music functions (Streaming)
void PS3Audio_PlayMusic(const void* data, size_t size);
void PS3Audio_StopMusic();
bool PS3Audio_IsMusicPlaying();
void PS3Audio_SetMusicVolume(int volume);

// SFX functions (One-shot)
void PS3Audio_PlaySFX(const void* data, size_t size);

#ifdef __cplusplus
}
#endif

#endif // PS3_AUDIO_H

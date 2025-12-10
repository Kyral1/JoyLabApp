#pragma once

#include <stdint.h>
#include <stdbool.h>

void speaker_init(void);
void speaker_set_volume(uint8_t level);
uint8_t speaker_get_volume(void);
void speaker_mute(void);
void speaker_stop(void);

// Blocking beep
void speaker_beep_blocking(uint16_t freq_hz, uint32_t duration_ms);

// File-based full buffer load (works but heavy)
void speaker_play_mp3_file(const char *path);

// Safe streaming MP3
void speaker_play_mp3_stream_safe(const char *path);



/*#pragma once
#include <stdint.h>

// Extern linker symbols created by target_add_binary_data
//extern const uint8_t _binary_AlarmSound_wav_start[];
//extern const uint8_t _binary_AlarmSound_wav_end[];

void speaker_init(void);
void speaker_set_volume(uint8_t level);   // level: 0–100%
uint8_t speaker_get_volume(void);
void speaker_mute(void);
//void speaker_play_wav_mem(const uint8_t *data_start, const uint8_t *data_end);
void speaker_beep_blocking(uint16_t freq_hz, uint32_t duration_ms);
void speaker_mute(void);
void speaker_stop(void);
void speaker_play_mp3_file(const char *path);
void speaker_play_mp3_stream(const char *path);
void speaker_stop(void);
//void mp3_play_task(void *path_ptr);
void speaker_play_mp3_stream_safe(const char *path);*/
#pragma once
#include <stdint.h>

// Extern linker symbols created by target_add_binary_data
extern const uint8_t _binary_AlarmSound_wav_start[];
extern const uint8_t _binary_AlarmSound_wav_end[];

void speaker_init(void);
void speaker_set_volume(uint8_t level);   // level: 0–100%
uint8_t speaker_get_volume(void);
void speaker_mute(void);
void speaker_play_wav_mem(const uint8_t *data_start, const uint8_t *data_end);
void speaker_beep_blocking(uint16_t freq_hz, uint32_t duration_ms);

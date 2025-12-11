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
void speaker_play_wav(const char *path);
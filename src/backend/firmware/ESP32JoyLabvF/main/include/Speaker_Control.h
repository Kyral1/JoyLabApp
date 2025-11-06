#pragma once
#include <stdint.h>

void speaker_init(void);
void speaker_set_volume(uint8_t level);   // level: 0–100%
uint8_t speaker_get_volume(void);
void speaker_mute(void);
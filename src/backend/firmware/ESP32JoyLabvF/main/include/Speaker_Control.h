#pragma once
#include <stdint.h>
#include <stdbool.h>

//Playing audio: play tones (e.g., simple beeps) because a tone is just a waveform (sine, square, whatever) within the 250 Hz–20 kHz range
#define SPEAKER_GPIO 19 //Can change

void speaker_init(void);
void speaker_set_state(bool on);
bool speaker_get_state(void);
void speaker_set_volume(uint8_t level); //0-100%
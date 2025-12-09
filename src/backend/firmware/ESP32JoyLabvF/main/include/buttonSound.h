#pragma once
#include <stdint.h>

#define NUM_BUTTONS 4
#define MAX_SOUND_NAME_LEN 32 

void load_button_sounds_from_nvs(void);
void save_button_sound_persistent(int button, const char *filename);
void save_button_sound(int button, const char *filename);
const char* get_button_sound(int button);
void play_button_sound(int button);
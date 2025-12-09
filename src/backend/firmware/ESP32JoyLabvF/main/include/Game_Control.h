#pragma once

#include <stdbool.h>

#define NUM_BUTTONS 4  // match Button_Control.h

void start_whackamole_game(void);
void stop_whackamole_game(void);

void stop_led_reg_game(void);
void start_led_reg_game(void);

void start_sound_game(void);
void stop_sound_game(void);
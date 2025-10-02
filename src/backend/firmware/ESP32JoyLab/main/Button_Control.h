#ifndef BUTTON_CONTROL_H
#define BUTTON_CONTROL_H

#include <stdbool.h>

#define NUM_BUTTONS 5 //can change later
extern const int button_pins[NUM_BUTTONS]; // Define which GPIO pins are used for the buttons

//D6 Series PushButton
//mechanical momentary pushbutton switch - only connects when you touch it down
//when pressed, legs connected, when released disconnected

//functions
void button_init_all(void);
bool button_is_pressed(int id);

#endif
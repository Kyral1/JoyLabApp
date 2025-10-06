#ifndef LED_CONTROL_H
#define LED_CONTROL_H

//DotStarLED (addressable)
//strip has 2 wires (data + clock) and uses digital protocol 
    //LEDs share the same data line, each has an index (first is 0)
    //send colors (R G B values) through the data line
    //use ESP-IDF LED driver to handle component 
    //brightness ranges from 0 (OFF) to 255

//declaring functions
void led_init(void);
void led_set_color_brightness(int index, uint8_t r, uint8_t g, uint8_t b, float brightness);
void led_clear_one(int index);
void led_clear(void);
void led_set_global_brightness(float brightness)

#endif
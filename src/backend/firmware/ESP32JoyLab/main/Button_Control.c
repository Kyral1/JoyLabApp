//pulls in ESP-IDF’s GPIO driver library
#include "driver/gpio.h"
#include "button_control.h"

//Button Pins (change when ESP is configured)
#define BUTTON1_PIN 12
#define BUTTON2_PIN 13
#define BUTTON3_PIN 14

//resets each pin before config
void button_init(void){
    gpio_reset_pin(BUTTON1_PIN)
    gpio_set_direction(BUTTON1_PIN, GPIO_MODE_INPUT);
    gpio_pullup_en(BUTTON1_PIN); //Internal pull-up resistor, when button is not pressed HIGH, pressed LOW
}

//checks if button is pressed
int button_read(int id){
    int pin = (id == 1) ? BUTTON1_PIN : (id == 2) ? BUTTON2_PIN : BUTTON3_PIN; //ternary op.
    return gpio_get_level(pin) == 0;
}
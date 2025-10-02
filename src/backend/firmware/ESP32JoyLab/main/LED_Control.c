//pulls in ESP-IDF’s GPIO driver library
#include "driver/gpio.h"
#include "led_control.h"

//LED Pins (change when ESP is configured)
#define LED1_PIN 2
#define LED2_PIN 4
#define LED3_PIN 5

//prepares all LEDs for use (reset + set as outputs)
void led_init(void){
    gpio_reset_pin(LED1_PIN);
    gpio_set_direction(LED1_PIN, GPIO_MODE_OUTPUT);

    gpio_reset_pin(LED2_PIN);
    gpio_set_direction(LED2_PIN, GPIO_MODE_OUTPUT);

    gpio_reset_pin(LED3_PIN);
    gpio_set_direction(LED3_PIN, GPIO_MODE_OUTPUT);
}

//LED ON
void led_on(int id){
    int pin = (id == 1) ? LED1_PIN : (id == 2) ? LED2_PIN : LED3_PIN; //ternary op.
    gpio_set_level(pin, 1) //set pin to high
}

//LED OFF
void led_off(int id){
    int pin = (id == 1) ? LED1_PIN : (id == 2) ? LED2_PIN : LED3_PIN; //ternary op.
    gpio_set_level(pin, 0) //set pin to low
}
#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;   
alarm_id_t long_press_alarm = -1;  

int64_t long_press_callback(alarm_id_t id, void *user_data) {
    flag_f_r = 1;  
    return 0;  
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == GPIO_IRQ_EDGE_FALL) {
        long_press_alarm = add_alarm_in_ms(500, long_press_callback, NULL, false);
    } else if (events == GPIO_IRQ_EDGE_RISE) {
        if (long_press_alarm >= 0) {
            cancel_alarm(long_press_alarm);
        }
    }
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);  

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);  

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, &btn_callback);

    bool estado_led = false;

    while (true) {
        if (flag_f_r) {
            flag_f_r = 0; 

            estado_led = !estado_led; 
            gpio_put(LED_PIN_R, estado_led);  
        }
    }
}
 
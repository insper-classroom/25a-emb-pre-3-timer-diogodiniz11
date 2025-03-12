#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_r = 0;
volatile int flag_g = 0;
volatile int g_timer_r = 0;
volatile int g_timer_g = 0;


void btn_callback(uint gpio, uint32_t events) {
    if (events == GPIO_IRQ_EDGE_FALL) {
        if (gpio == BTN_PIN_R)
            flag_r = 1;
        else if (gpio == BTN_PIN_G)
            flag_g = 1;
    }
}

bool timer_r_callback(repeating_timer_t *rt) {
    g_timer_r = 1;
    return true;
}

bool timer_g_callback(repeating_timer_t *rt) {
    g_timer_g = 1;
    return true;
}

int main() {
    stdio_init_all();

    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);
    gpio_put(LED_PIN_R, 0);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);
    gpio_put(LED_PIN_G, 0);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true, &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    
    repeating_timer_t timer_r;
    repeating_timer_t timer_g;

    int estado_r = 0;
    int estado_g = 0;

    while (true) {
        if (flag_r) {
            flag_r = 0;
            estado_r = !estado_r;

            if (estado_r) {
                if (!add_repeating_timer_ms(500, timer_r_callback, NULL, &timer_r)) {
                    printf("Falha ao iniciar o timer do LED vermelho!\n");
                }
            } else {
                cancel_repeating_timer(&timer_r);
                gpio_put(LED_PIN_R, 0);
            }
        }

        if (flag_g) {
            flag_g = 0;
            estado_g = !estado_g;

            if (estado_g) {
                if (!add_repeating_timer_ms(250, timer_g_callback, NULL, &timer_g)) {
                    printf("Falha ao iniciar o timer do LED verde!\n");
                }
            } else {
                cancel_repeating_timer(&timer_g);
                gpio_put(LED_PIN_G, 0);
            }
        }

        if (g_timer_r && estado_r) {
            g_timer_r = 0;
            gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R));
        }

        if (g_timer_g && estado_g) {
            g_timer_g = 0;
            gpio_put(LED_PIN_G, !gpio_get(LED_PIN_G));
        }
    }
}

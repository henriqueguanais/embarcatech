#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "led_rgb.h"

uint16_t led_level = 100;

void setup_pwm()
{
    uint slice;
    led_level = 100;
    gpio_set_function(LED, GPIO_FUNC_PWM); // Configura o pino do LED para função PWM
    slice = pwm_gpio_to_slice_num(LED);    // Obtém o slice do PWM associado ao pino do LED
    pwm_set_clkdiv(slice, DIVIDER_PWM);    // Define o divisor de clock do PWM
    pwm_set_wrap(slice, LED_PERIOD);           // Configura o valor máximo do contador (período do PWM)
    pwm_set_gpio_level(LED, led_level);    // Define o nível inicial do PWM para o pino do LED
    pwm_set_enabled(slice, true);          // Habilita o PWM no slice correspondente
}

void init_led_rgb(uint *up_down) {
    pwm_set_gpio_level(LED, led_level); // Define o nível atual do PWM (duty cycle)
    sleep_ms(70);                     // Atraso de 1 segundo
        if (*up_down)
        {
            led_level += LED_STEP; // Incrementa o nível do LED
            if (led_level >= LED_PERIOD)
                *up_down = 0; // Muda direção para diminuir quando atingir o período máximo
        }
        else
        {
            led_level -= LED_STEP; // Decrementa o nível do LED
            if (led_level <= LED_STEP)
                *up_down = 1; // Muda direção para aumentar quando atingir o mínimo
        }
}


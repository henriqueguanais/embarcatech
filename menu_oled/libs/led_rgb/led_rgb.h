#ifndef LED_RGB_H
#define LED_RGB_H

#include <stdint.h>

#define LED 12           // Pino do LED conectado
#define LED_PERIOD 1500   // Período do PWM (valor máximo do contador)
#define DIVIDER_PWM 16.0 // Divisor fracional do clock para o PWM
#define LED_STEP 50 // Passo de incremento/decremento para o duty cycle do LED
extern uint16_t led_level;      // Nível inicial do PWM (duty cycle)

void setup_pwm();
void init_led_rgb(uint *up_down);

#endif
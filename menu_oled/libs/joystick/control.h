#ifndef CONTROL_H
#define CONTROL_H

#include <stdint.h>  // Para usar uint8_t, uint16_t, etc.

// Definição dos pinos usados para o joystick e LEDs
#define VRX 26          // Pino de leitura do eixo X do joystick (conectado ao ADC)
#define VRY 27          // Pino de leitura do eixo Y do joystick (conectado ao ADC)
#define ADC_CHANNEL_0 0 // Canal ADC para o eixo X do joystick
#define ADC_CHANNEL_1 1 // Canal ADC para o eixo Y do joystick
#define SW 22          // Pino de leitura do botão do joystick

#define LED_B 13                   // Pino para controle do LED azul via PWM
#define LED_R 11                    // Pino para controle do LED vermelho via PWM
#define DIVIDER_PWM 16.0   // Divisor fracional do clock para o PWM
#define PERIOD 4096        // Período do PWM (valor máximo do contador)

// Variáveis globais (declaradas como extern para evitar múltiplas definições)
extern uint16_t led_b_level, led_r_level;
extern uint slice_led_b, slice_led_r;

void setup_joystick();
void setup_pwm_led(uint led, uint *slice, uint16_t level);
void setup();
void joystick_read_axis(uint16_t *vrx_value, uint16_t *vry_value, uint16_t *sw_value);


#endif
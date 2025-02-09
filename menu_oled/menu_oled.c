#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"

#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#include "ssd1306.h"
#include "menu.h"
#include "control.h"
#include "buzzer.h"
#include "led_rgb.h"

// Variáveis globais para sinalizar a saída dos modos
volatile bool exit_joystick_mode = false;
volatile bool exit_buzzer_mode = false;
volatile bool exit_led_rgb_mode = false;
// Funções
void gpio_callback(uint gpio, uint32_t events);
void joystick_mode(uint16_t vrx_value, uint16_t vry_value, uint16_t sw_value);
void buzzer_mode();
void led_rgb_mode(uint8_t *ssd, struct render_area frame_area);

int main() {
    stdio_init_all();

    // Inicializa o display OLED
    uint8_t ssd[ssd1306_buffer_length];
    struct render_area frame_area;
    init_display(ssd, &frame_area);
    
    // Inicializa o bitmap do display OLED
    ssd1306_t ssd_bm;
    ssd1306_init_bm(&ssd_bm, 128, 64, true, 0x3C, i2c1);
    ssd1306_config(&ssd_bm);

    ssd1306_draw_bitmap(&ssd_bm, menu_1);

    setup_joystick();

    int user_option = 0;
    uint16_t vrx_value, vry_value, sw_value;
    bool is_btn_pressed = false;

    // Configura a interrupção para o botão do joystick
    gpio_set_irq_enabled_with_callback(SW, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);

    while (true) {
        // Lê os valores dos eixos e botão do joystick
        joystick_read_axis(&vrx_value, &vry_value, &sw_value);
        // printf("VRX: %d\n", vrx_value);
        // printf("Botao: %d\n", sw_value);

        // Verifica se o botão foi pressionado ou solto
        if (sw_value == 0 && !is_btn_pressed) {
            is_btn_pressed = true;
        }
        else if (sw_value == 1 && is_btn_pressed) {
            is_btn_pressed = false;
        }

        // Verifica se o joystick foi movido para cima ou para baixo para alterar a opção do menu
        if (vrx_value < 100 && user_option <= 2) {
            user_option++;
        }
        else if (vrx_value > 4000 && user_option >= 0) {
            user_option--;
        }
        
        // Verifica se a opção do menu ultrapassou os limites
        if (user_option > 2) {
            user_option = 0;
        }
        else if (user_option < 0) {
            user_option = 2;
        }

        // Verifica a opção do menu selecionada
        switch (user_option)
        {
        case 0:
            ssd1306_draw_bitmap(&ssd_bm, menu_1);
            if (is_btn_pressed) {
                sleep_ms(50);

                ssd1306_draw_bitmap(&ssd_bm, menu_joystick);
                joystick_mode(vrx_value, vry_value, sw_value);

                sleep_ms(50);
            }
            break;
        case 1:
            ssd1306_draw_bitmap(&ssd_bm, menu_2);
            if (is_btn_pressed) {
                sleep_ms(50);

                ssd1306_draw_bitmap(&ssd_bm, menu_buzzer);
                buzzer_mode();

                sleep_ms(50);
            }
            break;
        case 2:
            ssd1306_draw_bitmap(&ssd_bm, menu_3);
            if (is_btn_pressed) {
                sleep_ms(50);

                ssd1306_draw_bitmap(&ssd_bm, menu_led);
                led_rgb_mode(ssd, frame_area);

                sleep_ms(50);
            }
            break;
        default:
            break;
        }
        sleep_ms(100);
    }
}

// Função callback para o botão do joystick
void gpio_callback(uint gpio, uint32_t events) {
    if (gpio == SW) { 
        exit_joystick_mode = true; // Sinaliza para sair do joystick_mode
        exit_buzzer_mode = true; // Sinaliza para sair do buzzer_mode
        exit_led_rgb_mode = true; // Sinaliza para sair do led_rgb_mode
    }
}

// Função para ajustar o nível PWM dos LEDs a partir dos valores dos eixos do joystick
void joystick_mode(uint16_t vrx_value, uint16_t vry_value, uint16_t sw_value) {
    setup();
    exit_joystick_mode = false;

    while (!exit_joystick_mode) {
        joystick_read_axis(&vrx_value, &vry_value, &sw_value); // Lê os valores dos eixos do joystick
        // Ajusta os níveis PWM dos LEDs de acordo com os valores do joystick
        pwm_set_gpio_level(LED_B, vrx_value); // Ajusta o brilho do LED azul com o valor do eixo X
        pwm_set_gpio_level(LED_R, vry_value); // Ajusta o brilho do LED vermelho com o valor do eixo Y

        // Pequeno delay antes da próxima leitura
        sleep_ms(100); // Espera 100 ms antes de repetir o ciclo

    }
    // Desliga os LEDs ao sair do modo joystick
    pwm_set_gpio_level(LED_B, 0); 
    pwm_set_gpio_level(LED_R, 0); 

}

// Função para tocar uma música no buzzer
void buzzer_mode() {
    pwm_init_buzzer(BUZZER_PIN);
    exit_buzzer_mode = false;

    while (!exit_buzzer_mode) {
        play_music(BUZZER_PIN);
    }
}

// Função para ajustar o nível PWM do LED RGB
void led_rgb_mode(uint8_t *ssd, struct render_area frame_area) {
    setup_pwm();
    exit_led_rgb_mode = false;
    uint up_down = 1; // Variável para controlar o sentido de incremento/decremento do PWM
   
    while (!exit_led_rgb_mode) {
        init_led_rgb(&up_down);
    }

    pwm_set_gpio_level(LED, 0);
}
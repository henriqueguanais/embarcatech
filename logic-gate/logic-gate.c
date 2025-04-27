#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "ssd1306.h"
#include "menu.h"
#include "control.h"

#define LED_R_PIN 13
#define LED_G_PIN 11

#define BTN_A_PIN 5
#define BTN_B_PIN 6

int main() {
    stdio_init_all();

    // inicializa o botao
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);

    // inicializa o botao
    gpio_init(BTN_B_PIN);
    gpio_set_dir(BTN_B_PIN, GPIO_IN);
    gpio_pull_up(BTN_B_PIN);

    // inicializa os leds
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);

    // Inicializa o display OLED
    uint8_t ssd[ssd1306_buffer_length];
    struct render_area frame_area;
    init_display(ssd, &frame_area);
    
    // Inicializa o bitmap do display OLED
    ssd1306_t ssd_bm;
    ssd1306_init_bm(&ssd_bm, 128, 64, true, 0x3C, i2c1);
    ssd1306_config(&ssd_bm);

    ssd1306_draw_bitmap(&ssd_bm, not_gate);

    setup_joystick();

    int user_option = 0;
    uint16_t vrx_value, vry_value;

    while (true) {
        // Lê os valores dos eixos e botão do joystick
        joystick_read_axis(&vrx_value, &vry_value);

        // Verifica se o joystick foi movido para cima ou para baixo para alterar a opção do menu
        if (vrx_value < 100 && user_option <= 6) {
            user_option++;
        }
        else if (vrx_value > 4000 && user_option >= 0) {
            user_option--;
        }
        // Verifica se a opção do menu ultrapassou os limites
        if (user_option > 6) {
            user_option = 0;
        }
        else if (user_option < 0) {
            user_option = 6;
        }

        // Verifica a opção do menu selecionada
        switch (user_option)
        {
        case 0:
            ssd1306_draw_bitmap(&ssd_bm, not_gate);
            if (gpio_get(BTN_A_PIN) == 0) {
                // se o botao A for pressionado, a saida do NOT é 0
                gpio_put(LED_R_PIN, 1);
                gpio_put(LED_G_PIN, 0);
            }
            else {
                gpio_put(LED_R_PIN, 0);
                gpio_put(LED_G_PIN, 1);
            }
        
            break;
        case 1:
            ssd1306_draw_bitmap(&ssd_bm, and_gate);
            if (gpio_get(BTN_A_PIN) == 0 && gpio_get(BTN_B_PIN) == 0) {
                // se o botao A e B forem pressionados, a saida do AND é 1
                gpio_put(LED_R_PIN, 0);
                gpio_put(LED_G_PIN, 1);
            }
            else {
                gpio_put(LED_R_PIN, 1);
                gpio_put(LED_G_PIN, 0);
            }
            break;
        case 2:
            ssd1306_draw_bitmap(&ssd_bm, nand_gate);
            if (gpio_get(BTN_A_PIN) == 0 && gpio_get(BTN_B_PIN) == 0) {
                // se o botao A e B forem pressionados, a saida do NAND é 0
                gpio_put(LED_R_PIN, 1);
                gpio_put(LED_G_PIN, 0);
            }
            else {
                gpio_put(LED_R_PIN, 0);
                gpio_put(LED_G_PIN, 1);
            }
            break;
        case 3:
            ssd1306_draw_bitmap(&ssd_bm, or_gate);
            if (gpio_get(BTN_A_PIN) == 0 || gpio_get(BTN_B_PIN) == 0) {
                // se o botao A ou B forem pressionados, a saida do OR é 1
                gpio_put(LED_R_PIN, 0);
                gpio_put(LED_G_PIN, 1);
            }
            else {
                gpio_put(LED_R_PIN, 1);
                gpio_put(LED_G_PIN, 0);
            }
            break;
        case 4:
            ssd1306_draw_bitmap(&ssd_bm, nor_gate);
            if (gpio_get(BTN_A_PIN) == 0 || gpio_get(BTN_B_PIN) == 0) {
                // se o botao A ou B forem pressionados, a saida do NOR é 0
                gpio_put(LED_R_PIN, 1);
                gpio_put(LED_G_PIN, 0);
            }
            else {
                gpio_put(LED_R_PIN, 0);
                gpio_put(LED_G_PIN, 1);
            }
            break;
        case 5: 
            ssd1306_draw_bitmap(&ssd_bm, xor_gate);
            if (gpio_get(BTN_A_PIN) != gpio_get(BTN_B_PIN)) {
                // se o botao A ou B forem pressionados, a saida do XOR é 1
                gpio_put(LED_R_PIN, 0);
                gpio_put(LED_G_PIN, 1);
            }
            else {
                gpio_put(LED_R_PIN, 1);
                gpio_put(LED_G_PIN, 0);
            }
            break;
        case 6:
            ssd1306_draw_bitmap(&ssd_bm, xnor_gate);
            if (gpio_get(BTN_A_PIN) != gpio_get(BTN_B_PIN)) {
                // se o botao A ou B forem pressionados, a saida do XNOR é 0
                gpio_put(LED_R_PIN, 1);
                gpio_put(LED_G_PIN, 0);
            }
            else {
                gpio_put(LED_R_PIN, 0);
                gpio_put(LED_G_PIN, 1);
            }
            break;
        default:
            break;
        }
        sleep_ms(100);
    }
}



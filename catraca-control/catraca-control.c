#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "ssd1306.h"
#include "menu.h"
#include "control_joy.h"
#include "neopixel.h"

#define LED_R 13
#define LED_G 11
int main() {
    stdio_init_all();

    gpio_init(LED_R);
    gpio_init(LED_G);
    gpio_set_dir(LED_R, GPIO_OUT);
    gpio_set_dir(LED_G, GPIO_OUT);
    gpio_put(LED_R, 1);
    gpio_put(LED_G, 0);

    // Inicializa o display OLED
    uint8_t ssd[ssd1306_buffer_length];
    struct render_area frame_area;
    init_display(ssd, &frame_area);

    // Inicializa matriz de LEDs NeoPixel.
    npInit(LED_PIN);
    npClear();
    npSetLED(getIndex(4, 0), 2, 0, 0);
    npSetLED(getIndex(3, 0), 2, 0, 0);
    npSetLED(getIndex(2, 0), 2, 0, 0);
    npSetLED(getIndex(1, 0), 2, 0, 0);
    npWrite();
    
    // Inicializa o bitmap do display OLED
    ssd1306_t ssd_bm;
    ssd1306_init_bm(&ssd_bm, 128, 64, true, 0x3C, i2c1);
    ssd1306_config(&ssd_bm);

    ssd1306_draw_bitmap(&ssd_bm, menu_1);

    setup_joystick(); 

    int8_t user_option = 0;
    uint16_t vrx_value, vry_value, sw_value;
    bool is_btn_pressed = false;
    bool GR, HO, DI, PT= false;

    while (true) {
        // Lê os valores dos eixos e botão do joystick
        joystick_read_axis(&vrx_value, &vry_value, &sw_value);
        
        printf("Botao: %d\n", sw_value);
        printf("btn: %d\n", is_btn_pressed);

        // liberação da catraca
        if (!PT || (GR && HO && DI)) {
            gpio_put(LED_R, 0);
            gpio_put(LED_G, 1);
        }
        else {
            gpio_put(LED_R, 1);
            gpio_put(LED_G, 0);
        }

        // Verifica se o botão foi pressionado ou solto
        if (sw_value == 0 && !is_btn_pressed) {
            is_btn_pressed = true;
        }
        else if (sw_value == 1 && is_btn_pressed) {
            is_btn_pressed = false;
        }

        // Verifica se o joystick foi movido para cima ou para baixo para alterar a opção do menu
        if (vrx_value < 100) {
            user_option++;
        }
        else if (vrx_value > 4000) {
            user_option--;
        }
        
        // Verifica se a opção do menu ultrapassou os limites
        if (user_option > 3) {
            user_option = 0;
        }
        else if (user_option < 0) {
            user_option = 3;
        }

        // Verifica a opção do menu selecionada
        switch (user_option)
        {
        case 0:
            ssd1306_draw_bitmap(&ssd_bm, menu_1);
            if (is_btn_pressed && !GR) {
                sleep_ms(50);
                // GR
                npSetLED(getIndex(4, 0), 0, 2, 0);
                npWrite();
                GR = true;
                sleep_ms(50);
            }
            else if (is_btn_pressed && GR) {
                sleep_ms(50);
                // GR
                npSetLED(getIndex(4, 0), 2, 0, 0);
                npWrite();
                GR = false;
                sleep_ms(50);
            }
            break;
        case 1:
            ssd1306_draw_bitmap(&ssd_bm, menu_2);
            if (is_btn_pressed && !HO) {
                sleep_ms(50);
                // HO
                npSetLED(getIndex(3, 0), 0, 2, 0);
                npWrite();
                HO = true;
                sleep_ms(50);
            }
            else if (is_btn_pressed && HO) {
                sleep_ms(50);
                // HO
                npSetLED(getIndex(3, 0), 2, 0, 0);
                npWrite();
                HO = false;
                sleep_ms(50);
            }
            break;
        case 2:
            ssd1306_draw_bitmap(&ssd_bm, menu_3);
            if (is_btn_pressed && !DI) {
                sleep_ms(50);
                // DI
                npSetLED(getIndex(2, 0), 0, 2, 0);
                npWrite();
                DI = true;
                sleep_ms(50);
            }
            else if (is_btn_pressed && DI) {
                sleep_ms(50);
                // DI
                npSetLED(getIndex(2, 0), 2, 0, 0);
                npWrite();
                DI = false;
                sleep_ms(50);
            }
            break;
        case 3:
            ssd1306_draw_bitmap(&ssd_bm, menu_4);
            if (is_btn_pressed && !PT) {
                sleep_ms(50);
                // PT
                npSetLED(getIndex(1, 0), 0, 2, 0);
                npWrite();
                PT = true;
                sleep_ms(50);
            }
            else if (is_btn_pressed && PT) {
                sleep_ms(50);
                // PT
                npSetLED(getIndex(1, 0), 2, 0, 0);
                npWrite();
                PT = false;
                sleep_ms(50);
            }
            break;
        default:
            break;
        }
        sleep_ms(100);
    }
}

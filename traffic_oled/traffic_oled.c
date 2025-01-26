#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "lib/ssd1306.h"
#include "hardware/i2c.h"

#define I2C_SDA 14
#define I2C_SCL 15

#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12

#define BTN_A_PIN 5

int A_state = 0;    //Botao A está pressionado?
bool is_red_high = true;    // O sinal está vermelho?

// LEDS do sinal verde
void SinalAberto(){
    gpio_put(LED_R_PIN, 0);
    gpio_put(LED_G_PIN, 1);
    gpio_put(LED_B_PIN, 0); 
}
// LEDS do sinal amarelo
void SinalAtencao(){
    gpio_put(LED_R_PIN, 1);
    gpio_put(LED_G_PIN, 1);
    gpio_put(LED_B_PIN, 0);
}
// LEDS do sinal vermelho
void SinalFechado(){
    gpio_put(LED_R_PIN, 1);
    gpio_put(LED_G_PIN, 0);
    gpio_put(LED_B_PIN, 0);
}
// Função para aguardar um tempo e verificar se o botão A foi pressionado
int WaitWithRead(int timeMS){
    for(int i = 0; i < timeMS; i = i+100){
        A_state = !gpio_get(BTN_A_PIN);
        if(A_state == 1){
            return 1;
        }
        sleep_ms(100);
    }
    return 0;
}

// Inicializa o display OLED
void init_display(uint8_t *ssd, struct render_area *frame_area) {
    // Inicialização do i2c
    i2c_init(i2c1, ssd1306_i2c_clock * 1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);

    // Processo de inicialização completo do OLED SSD1306
    ssd1306_init();

    // Preparar área de renderização para o display (ssd1306_width pixels por ssd1306_n_pages páginas)
    *frame_area = (struct render_area) {
        .start_column = 0,
        .end_column = ssd1306_width - 1,
        .start_page = 0,
        .end_page = ssd1306_n_pages - 1
    };

    calculate_render_area_buffer_length(frame_area);

    // zera o display inteiro
    memset(ssd, 0, ssd1306_buffer_length);
    render_on_display(ssd, frame_area);
}

// Desenha o texto no display
void draw_text(uint8_t *ssd, struct render_area frame_area, char *text[], int count_of) {
    memset(ssd, 0, ssd1306_buffer_length);
    int y = 0;
    for (uint i = 0; i < count_of; i++)
    {
        ssd1306_draw_string(ssd, 0, y, text[i]);
        y += 8;
    }
    render_on_display(ssd, &frame_area);
}
// Desenha um contador regressivo no display
void draw_count(uint8_t *ssd, struct render_area frame_area, char *text[], int time, int count_of) {
    char temp[16];  // Buffer temporário para a linha de contagem
    for (int x = time; x > 0; x--) {
        // Formata a linha de contagem regressiva
        snprintf(temp, sizeof(temp), "Tempo %d s", x);
        text[count_of - 1] = temp;  
        draw_text(ssd, frame_area, text, count_of);  // Atualiza o display
        
        // se estiver no sinal vermelho, verifica se o botão foi pressionado
        if (is_red_high) {
            A_state = WaitWithRead(1000);
            if (A_state == 1) {
                break;
            }
        }
        else {
            sleep_ms(1000);
        }
    }
}

int main() {
    stdio_init_all(); 

    // inicializa o botao
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);

    // inicializa os leds
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);

    // Inicializa o display OLED
    uint8_t ssd[ssd1306_buffer_length];
    struct render_area frame_area;
    init_display(ssd, &frame_area);     

    while(true) {
        SinalFechado();
        is_red_high = true;
        char *text_vermelho[] = {"SINAL FECHADO", "AGUARDE", "", ""};
        // desenha o texto no OLED e aguarda 10 segundos caso o botao nao seja pressionado
        draw_count(ssd, frame_area, text_vermelho, 10, count_of(text_vermelho));
        is_red_high = false;

        if (A_state == 1) {
            // botao de pedestres pressionado
            SinalAtencao();
            char *text_amarelo[] = {"SINAL DE", "ATENCAO", "PREPARE SE", "", ""};
            // desenha o texto no OLED e aguarda 5 segundos
            draw_count(ssd, frame_area, text_amarelo, 5, count_of(text_amarelo));

            SinalAberto();
            char *text_verde[] = {"SINAL ABERTO", "ATRAVESSAR COM", "CUIDADO", "", ""};
            // desenha o texto no OLED e aguarda 10 segundos
            draw_count(ssd, frame_area, text_verde, 10, count_of(text_verde));

        }
        else {
            // botao de pedestres nao pressionado
            SinalAtencao();
            char *text_amarelo[] = {"SINAL DE", "ATENCAO", "PREPARE SE", "", ""};
            // desenha o texto no OLED e aguarda 2 segundos
            draw_count(ssd, frame_area, text_amarelo, 2, count_of(text_amarelo));

            SinalAberto();
            char *text_verde[] = {"SINAL ABERTO", "ATRAVESSAR COM", "CUIDADO", "", ""};
            // desenha o texto no OLED e aguarda 8 segundos
            draw_count(ssd, frame_area, text_verde, 8, count_of(text_verde));
        }
    }

    return 0;
}
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

#define led_red 18
#define led_yellow 19
#define led_green 20
#define led_pedestre 21
#define botao 28
#define buzzer 16

#define buzzer_frequency 1100

// funcao que inicializa o buzzer
void pwm_init_buzzer(uint pin) {
    // Configurar o pino como saída de PWM
    gpio_set_function(pin, GPIO_FUNC_PWM);

    // Obter o slice do PWM associado ao pino
    uint slice_num = pwm_gpio_to_slice_num(pin);

    // Configurar o PWM com frequência desejada
    pwm_config config = pwm_get_default_config();
    pwm_config_set_clkdiv(&config, clock_get_hz(clk_sys) / (buzzer_frequency * 4096)); // Divisor de clock
    pwm_init(slice_num, &config, true);

    // Iniciar o PWM no nível baixo
    pwm_set_gpio_level(pin, 0);
}

// funcao que define os estados dos 3 leds de semaforo
void leds_put(bool red, bool yellow, bool green) {
    gpio_put(led_red, red);
    gpio_put(led_yellow, yellow);
    gpio_put(led_green, green);
}

// funcao chamada durante a interrupcao do botao, executando o algoritmo da travessia
void travessia_pedestre() {
    // liga o led amarelo por 5 segundos
    leds_put(false, true, false);
    sleep_ms(5000);
    printf("Passagem de pedestres...");
    // liga o led de pedestre e o led vermelho por 15 segundos
    gpio_put(led_pedestre, true);
    leds_put(true, false, false);
    // Configurar o duty cycle para 50% (ativo)
    pwm_set_gpio_level(buzzer, 1024);

    sleep_ms(15000);
    // desliga led de pedestre e o buzzer
    gpio_put(led_pedestre, false);
    pwm_set_gpio_level(buzzer, 0);
    
}

int main() {
    // inicializa os pinos
    stdio_init_all();
    gpio_init(led_red);
    gpio_init(led_yellow);
    gpio_init(led_green);
    gpio_init(led_pedestre);
    gpio_init(botao);
    gpio_init(buzzer);

    // define os pinos como entrada ou saida
    gpio_set_dir(led_red, true);
    gpio_set_dir(led_yellow, true);
    gpio_set_dir(led_green, true);
    gpio_set_dir(led_pedestre, true);
    gpio_set_dir(botao, false);
    gpio_set_dir(buzzer, true);

    // chama a funcao de inicializacao do buzzer
    pwm_init_buzzer(buzzer);
    // define o botao como pull-up
    gpio_pull_up(botao);
    // cria a interrupcao, que ao ser pressionado ativa a interrupcao, indo para a funcao travessia_pedestre()
    gpio_set_irq_enabled_with_callback(botao, GPIO_IRQ_EDGE_RISE, true, &travessia_pedestre);

    while (true) {
        /* 
        funcionamento padrao do semaforo:
        Verde ---> 8 s
        Amarelo ---> 2 s
        Vermelho ---> 10 s
        */
        leds_put(false, false, true);
        sleep_ms(8000);
        leds_put(false, true, false);
        sleep_ms(2000);
        leds_put(true, false, false);
        sleep_ms(10000);
    }
}
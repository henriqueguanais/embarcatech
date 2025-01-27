#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "neopixel.h"

// Biblioteca gerada pelo arquivo .pio durante compilação.
#include "ws2818b.pio.h"


// Definição do número de LEDs e pino.
#define LED_COUNT 25
#define LED_PIN 7

int main() {
    // Inicializa o sistema.
    stdio_init_all();
    npInit(LED_PIN);
    npClear(); // Garante que todos os LEDs comecem apagados.

    draw_square();

    npSetLED(17, 255, 0, 0);
    npSetLED(12, 255, 0, 0); // Define o LED de índice 12 (centro) para amarelo
    npSetLED(7, 255, 255, 0); // Define o LED de índice 7 (centro) para verde

    // Passo 2: Envia os valores do buffer para os LEDs físicos.
    npWrite(); // Agora os LEDs acendem com as cores especificadas.

    // Loop infinito (nenhuma atualização adicional).
    while (true) {
        sleep_ms(1000);
    }
}
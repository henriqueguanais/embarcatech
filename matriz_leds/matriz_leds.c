#include <stdio.h>
#include "pico/stdlib.h"
#include "neopixel.h"

int main() {
  // Inicializa entradas e saídas.
  stdio_init_all();

  // Inicializa matriz de LEDs NeoPixel.
  npInit(LED_PIN);
  npClear();
  
  int x = 4, y = 0;
  int start_pointx = 4, start_pointy = 0;
  npSetLED(getIndex(x, y), 0, 2, 0);
  npWrite();
  while (true) {
    for (x = start_pointx; x >= start_pointy; x--) {
        npSetLED(getIndex(x, y), 0, 2, 0);
        npWrite();
        sleep_ms(100);
    }
    for (y = start_pointy; y <= start_pointx; y++) {
        npSetLED(getIndex(x, y), 0, 2, 0);
        npWrite();
        sleep_ms(100);
    }
    for (x = start_pointy; x <= start_pointx; x++) {
        npSetLED(getIndex(x, y), 0, 2, 0);
        npWrite();
        sleep_ms(100);
    }
    start_pointx--;
    start_pointy++;
    sleep_ms(100);
    
  }
}
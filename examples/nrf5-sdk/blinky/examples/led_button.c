#include <stdbool.h>
#include <stdint.h>
#include <nrf_gpio.h>
#include "nrf52840.h"
#include "nrf_delay.h"

void led_on(void) {
    NRF_P0->DIRSET = (1 << 22);
}

void led_off(void) {
    NRF_P0->DIRCLR = (1 << 22);
}

void button_init(void) {
    NRF_P1->PIN_CNF[0] = (NRF_GPIO_PIN_PULLUP << NRF_GPIO_PIN_PULLUP);
}

int main(void) {
    button_init();
    while (1) {
        if (!NRF_P1->IN) {
            led_on();
            nrf_delay_ms(1000);
            led_off();
        }
    }
}

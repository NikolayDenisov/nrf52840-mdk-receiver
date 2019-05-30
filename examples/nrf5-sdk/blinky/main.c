
#include <stdbool.h>
#include <stdint.h>
#include <nrf_gpio.h>
#include "nrf52840.h"
#include "nrf_delay.h"

#define BNT1 NRF_GPIO_PIN_MAP(1, 0)

void led_on(void) {
    NRF_P0->DIRSET = (1 << 22);
}

void led_off(void) {
    NRF_P0->DIRCLR = (1 << 22);
}

void button_init(void) {
    NRF_P1->PIN_CNF[0] = (NRF_GPIO_PIN_PULLUP << NRF_GPIO_PIN_PULLUP);
}

void clock_initialization() {
    NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART = 1;

    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {
        // Do nothing.
    }
}

int main(void) {
    clock_initialization();
    button_init();
    while (1) {
        if (! NRF_P1->IN & (1 << 0)) {
            led_on();
            nrf_delay_ms(1000);
            led_off();
        }
    }
}


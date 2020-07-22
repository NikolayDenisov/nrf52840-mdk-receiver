#include <stdbool.h>
#include <stdint.h>
#include "nrf52840.h"
#include "nrf_delay.h"

void led_on(void) {
    NRF_P0->DIRSET = (1 << 22);
}

void led_off(void) {
    NRF_P0->DIRCLR = (1 << 22);
}


int main(void) {
    while (1) {
        led_on();
        nrf_delay_ms(1000);
        led_off();
        nrf_delay_ms(1000);
    }
}

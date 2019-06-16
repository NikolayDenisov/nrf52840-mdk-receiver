#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "nrf52840.h"
#include "nrf52840_bitfields.h"
#include "nrf_delay.h"


void clock_initialization() {
    /* Start 32 MHz crystal oscillator */
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;

    /* Wait for the external oscillator to start up */
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {
        // Do nothing.
    }
}

int main(void) {
    clock_initialization();
    NRF_P0->DIRSET = (1<<23);
}
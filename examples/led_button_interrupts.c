#include <nrf_gpio.h>
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
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {
    }
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
}

void led_on(void) {
    NRF_P0->DIRSET = (1 << 22);
}

void led_off(void) {
    NRF_P0->DIRCLR = (1 << 22);
}

void GPIOTE_IRQHandler(void) {
    led_on();
    nrf_delay_ms(1000);
    led_off();
    NRF_GPIOTE->EVENTS_PORT = 0;

}

void button_init(void) {
    NRF_P1->PIN_CNF[0] = (NRF_GPIO_PIN_PULLUP << NRF_GPIO_PIN_PULLUP)
                         | (NRF_GPIO_PIN_SENSE_LOW << GPIO_PIN_CNF_SENSE_Pos);
    NRF_GPIOTE->CONFIG[0] = (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos)
                            | (0 << GPIOTE_CONFIG_PSEL_Pos)
                            | (1 << GPIOTE_CONFIG_PORT_Pos)
                            | (GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos);
    NRF_GPIOTE->INTENSET = (0 << GPIOTE_INTENSET_IN0_Set) | (1 << GPIOTE_INTENSET_PORT_Pos);
    NVIC_ClearPendingIRQ(GPIOTE_IRQn);
    NVIC_EnableIRQ(GPIOTE_IRQn);

}

int main(void) {
    clock_initialization();
    button_init();
    while (1) {
        __WFI();
    }
}
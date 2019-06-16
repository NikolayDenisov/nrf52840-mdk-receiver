
#include <stdbool.h>
#include <stdint.h>
#include <nrf_gpio.h>
#include "nrf52840.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_gpiote.h"


void push_button_handler(void) {
    nrf_gpio_port_dir_input_set(NRF_P1, 1 << 0);
    NRF_P1->PIN_CNF[0] = (NRF_GPIO_PIN_PULLUP << NRF_GPIO_PIN_PULLUP);
    NRF_GPIOTE->INTENSET |= 0x1UL<<1;
    NVIC_SetPriority(GPIOTE_IRQn, APP_IRQ_PRIORITY_HIGH);
    NVIC_ClearPendingIRQ(GPIOTE_IRQn);
    NVIC_EnableIRQ(GPIOTE_IRQn);
    NRF_GPIOTE->CONFIG[0] = (GPIOTE_CONFIG_MODE_Event << GPIOTE_CONFIG_MODE_Pos) |
                            (0 << GPIOTE_CONFIG_PSEL_Pos) | (1) |
                            (GPIOTE_CONFIG_POLARITY_HiToLo << GPIOTE_CONFIG_POLARITY_Pos);
}

void led_on(void) {
    nrf_gpio_pin_dir_set(22, NRF_GPIO_PIN_DIR_OUTPUT);
}

void led_off(void) {
    nrf_gpio_pin_dir_set(22, NRF_GPIO_PIN_DIR_INPUT);
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
    push_button_handler();
    nrf_delay_ms(1000);
    while (1) {
    }
}


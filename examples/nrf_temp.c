#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "nrf52840.h"
#include "nrf52840_bitfields.h"
#include "nrf_delay.h"


/**@cond NO_DOXYGEN */
#define MASK_SIGN           (0x00000200UL)
#define MASK_SIGN_EXTENSION (0xFFFFFC00UL)

void clock_initialization() {
    /* Start 32 MHz crystal oscillator */
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;

    /* Wait for the external oscillator to start up */
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {
        // Do nothing.
    }
}

void uart_init(void) {
    /*Initialize the UART peripheral */
    NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud115200;
    NRF_P0->DIRSET = (1 << 19); /*! RXD Input */
    NRF_P0->DIRCLR = (1 << 20); /*! TXD Output */
    NRF_UART0->PSEL.RXD = 19;
    NRF_UART0->PSEL.TXD = 20;
    NRF_UART0->PSEL.CTS = 0xFFFFFFFF; /**< Value indicating that no pin is connected to this UART register. */
    NRF_UART0->PSEL.RTS = 0xFFFFFFFF;/**< Value indicating that no pin is connected to this UART register. */
    NRF_UART0->CONFIG = (UART_CONFIG_PARITY_Excluded << UART_CONFIG_PARITY_Pos |
                         UART_CONFIG_HWFC_Disabled << UART_CONFIG_HWFC_Pos |
                         UART_CONFIG_STOP_One << UART_CONFIG_STOP_Pos);
    NRF_UART0->ENABLE = (UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos);
    NRF_UART0->TASKS_STARTTX = 1;
}


void simple_uart_put(uint8_t cr) {
    NRF_UART0->TXD = (uint8_t) cr;
    while (NRF_UART0->EVENTS_TXDRDY != 1) {
        // Wait for TXD data to be sent
    }
    NRF_UART0->EVENTS_TXDRDY = 0;
}

void uart_put_string(const uint8_t *str) {
    uint_fast8_t i = 0;
    uint8_t ch = str[i++];
    while (ch != '\0') {
        simple_uart_put(ch);
        ch = str[i++];
    }
}

uint32_t temp_measurement(void) {
    uint32_t temp_value;
    NRF_TEMP->TASKS_START = 1;
    while (NRF_TEMP->EVENTS_DATARDY == 0) {}
    temp_value = ((NRF_TEMP->TEMP & MASK_SIGN) != 0) ? (NRF_TEMP->TEMP | MASK_SIGN_EXTENSION) : (NRF_TEMP->TEMP);
    NRF_TEMP->TASKS_STOP = 1;
    return temp_value;

}

int main(void) {
    clock_initialization();
    nrf_delay_ms(300);
    uart_init();
    while (1) {
        uint32_t value = temp_measurement() / 4 - 9;
        char s[40];
        sprintf(s, "Temperatute=%ld\r\n", value);
        uart_put_string((const uint8_t *) s);
        nrf_delay_ms(2000);
        uart_put_string((const uint8_t *) "\033[2J");
        nrf_delay_ms(1000);
    }
}

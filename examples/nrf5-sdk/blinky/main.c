
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "nrf52840.h"
#include "nrf52840_bitfields.h"
#include "nrf_delay.h"

void clock_initialization() {
    /* Start 16 MHz crystal oscillator */
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;

    /* Wait for the external oscillator to start up */
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {
        // Do nothing.
    }

    /* Start low frequency crystal oscillator for app_timer(used by bsp)*/
    NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART = 1;

    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {
        // Do nothing.
    }
}

void UART_init(void) {
    /*Initialize the UART peripheral */
    NRF_UART0->ENABLE = 0;
    NRF_UART0->INTENCLR = 0xFFFFFFFF;
    NRF_P0->DIRSET = (1 << 19); /*! RXD Input */
    NRF_P0->DIRCLR = (1 << 20); /*! TXD Output */
    NRF_UART0->PSEL.RXD = (1 << 19);
    NRF_UART0->PSEL.TXD = (1 << 20);
    NRF_UART0->PSEL.CTS = 0xFFFFFFFF;
    NRF_UART0->PSEL.RTS = 0xFFFFFFFF;
    NRF_UART0->BAUDRATE = (UART_BAUDRATE_BAUDRATE_Baud115200 << UART_BAUDRATE_BAUDRATE_Pos);
    NRF_UART0->CONFIG = (UART_CONFIG_HWFC_Msk << UART_CONFIG_HWFC_Pos);
    NRF_UART0->ENABLE = (UART_ENABLE_ENABLE_Enabled << UART_ENABLE_ENABLE_Pos);
    NRF_UART0->CONFIG |= (UART_CONFIG_PARITY_Msk << UART_CONFIG_PARITY_Pos);
    NRF_UART0->TASKS_STARTTX = 1;
    NRF_UART0->TASKS_STARTRX = 1;
    return;
}


void simple_uart_put(uint8_t cr) {
    NRF_UART0->TXD = (uint8_t) cr;

    while (NRF_UART0->EVENTS_TXDRDY != 1) {
        // Wait for TXD data to be sent
    }

    NRF_UART0->EVENTS_TXDRDY = 0;
}

void simple_uart_putstring(const uint8_t *str) {
    uint_fast8_t i = 0;
    uint8_t ch = str[i++];
    while (ch != '\0') {
        simple_uart_put(ch);
        ch = str[i++];
    }
}


int main(void) {
    clock_initialization();
    nrf_delay_ms(100);
    UART_init();
    while (1) {
        nrf_delay_ms(1000);
        simple_uart_putstring((const uint8_t *)"\n\rMPU9150 Initialize success\n\r");
        printf("ACC:  %d	%d	%d	", 25, 255, 28);
        NRF_P0->DIRSET = (1 << 22);
        nrf_delay_ms(1000);
        NRF_P0->DIRCLR = (1 << 22);
    }
}


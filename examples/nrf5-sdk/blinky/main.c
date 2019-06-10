
#include <stdbool.h>
#include <stdint.h>
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
    NRF_UART0->BAUDRATE = UART_BAUDRATE_BAUDRATE_Baud115200 ;
    NRF_UART0->CONFIG = UART_CONFIG_HWFC_Msk;
    NRF_UART0->ENABLE = UART_ENABLE_ENABLE_Enabled;
    NRF_UART0->CONFIG |= UART_CONFIG_PARITY_Msk;
    NRF_UART0->TASKS_STARTTX = 1;
    NRF_UART0->TASKS_STARTRX = 1;
    return;
}


void send_byte(char c) {
    NRF_UART0->TXD = (uint8_t) c;
    while (NRF_UART0->EVENTS_TXDRDY != 1) {}
    NRF_UART0->EVENTS_TXDRDY = 0;
}

int puts(const char *__str) {
    int i = 0;
    while (__str[i]) {
        send_byte(__str[i]);
        i++;
    }
    return 1;
}


int main(void) {
    clock_initialization();
    nrf_delay_ms(100);
    UART_init();
    while (1) {
        nrf_delay_ms(1000);
        puts("Hello");
        nrf_delay_ms(1000);
    }
}


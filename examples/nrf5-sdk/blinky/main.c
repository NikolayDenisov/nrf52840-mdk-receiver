#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
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

void GPIOTE_IRQHandler(void) {
    uart_put_string((const uint8_t *) "IRQHandler\r\n");
    led_on();
    nrf_delay_ms(1000);
    led_off();
    NRF_GPIOTE->INTENCLR = 0x20;
}

void button_init(void) {
    uart_put_string((const uint8_t *) "BUTTON init\r\n");
    NRF_P1->PIN_CNF[0] = (GPIO_PIN_CNF_INPUT_Connect << GPIO_PIN_CNF_INPUT_Pos) |
                         (GPIO_PIN_CNF_SENSE_Low << GPIO_PIN_CNF_SENSE_Pos) |
                         (GPIO_PIN_CNF_PULL_Pullup << GPIO_PIN_CNF_PULL_Pos);
    NRF_GPIOTE->INTENSET = 0x20;
    NVIC_EnableIRQ(GPIOTE_IRQn);

}

int main(void) {
    clock_initialization();
    uart_init();
    button_init();
    uart_put_string((const uint8_t *) "Wait interrupt\r\n");
    while (1) {
        uart_put_string((const uint8_t *) "BEFORE interrupt\r\n");
        __WFI();
        uart_put_string((const uint8_t *) "AFTER interrupt\r\n");
    }
}
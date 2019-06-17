#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <nrf_gpio.h>
#include "nrf52840.h"
#include "nrf52840_bitfields.h"
#include "nrf_delay.h"


#define LFCLK_FREQUENCY           (32768UL)                               /**< LFCLK*/
#define RTC_FREQUENCY             (8UL)                                   /**<RTC_FREQUENCY */
#define COMPARE_COUNTERTIME       (3UL)                                   /**< Get Compare event COMPARE_TIME seconds after the counter starts from 0. */
#define COUNTER_PRESCALER ((LFCLK_FREQUENCY/RTC_FREQUENCY) - 1) /** f = LFCLK/(prescaler + 1) **/


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

void RTC0_IRQHandler() {
    uart_put_string((const uint8_t *) "IRQ debug\r\n");
    if ((NRF_RTC0->EVENTS_TICK != 0) &&
        ((NRF_RTC0->INTENSET & RTC_INTENSET_TICK_Msk) != 0)) {
        NRF_RTC0->EVENTS_TICK = 0;
    }

    if ((NRF_RTC0->EVENTS_COMPARE[0] != 0) &&
        ((NRF_RTC0->INTENSET & RTC_INTENSET_COMPARE0_Msk) != 0)) {
        NRF_RTC0->EVENTS_COMPARE[0] = 0;
    }
}

int main(void) {
    uint32_t counter;
    uint32_t cur_cc;
    clock_initialization();
    uart_init();
    uart_put_string((const uint8_t *) "Start RTC example\r\n");
    NVIC_EnableIRQ(RTC0_IRQn); // Enable Interrupt for the RTC in the core.
    NRF_RTC0->PRESCALER = COUNTER_PRESCALER; /**12 bit prescaler for COUNTER frequency**/
    NRF_RTC0->CC[0] = COMPARE_COUNTERTIME * RTC_FREQUENCY;
    NRF_RTC0->EVTENSET = RTC_EVTENSET_TICK_Msk; /**Enable event routing**/
    NRF_RTC0->INTENSET = RTC_INTENSET_TICK_Msk; /**Enable interrupt**/
    NRF_RTC0->EVENTS_COMPARE[0] = 0; /**Compare register 0**/
    NRF_RTC0->TASKS_START = 1; /**Start RTC COUNTER**/
    while (true) {
        counter = NRF_RTC0->COUNTER;
        cur_cc = NRF_RTC0->CC[0];
        uart_put_string((const uint8_t *) "Read Counter\r\n");
        nrf_delay_ms(2000);
        __WFI();
        char s1[40];
        char s2[40];
        sprintf(s1, "Counter value=%ld\r\n", counter);
        sprintf(s2, "CC value=%ld\r\n", cur_cc);
        uart_put_string((const uint8_t *) s1);
        uart_put_string((const uint8_t *) s2);
//        nrf_delay_ms(2000);
    }
}
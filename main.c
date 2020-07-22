#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "nrf_drv_qspi.h"
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "boards.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "sdk_config.h"
#include "nrf52840.h"
#include "nrf52840_bitfields.h"
#include "bsp.h"
#include "radio_config.h"

static uint32_t packet;

void send_packet() {
    NRF_RADIO->EVENTS_READY = 0U;
    NRF_RADIO->TASKS_TXEN = 1;
    while (NRF_RADIO->EVENTS_READY == 0U) {
        // wait
    }
    NRF_RADIO->EVENTS_END = 0U;
    NRF_RADIO->TASKS_START = 1U;
    while (NRF_RADIO->EVENTS_END == 0U) {
        // wait
    }

    bsp_board_led_invert(0);
    nrf_delay_ms(10);
    NRF_LOG_INFO("The packet was sent");
    NRF_RADIO->EVENTS_DISABLED = 0U;
    NRF_RADIO->TASKS_DISABLE = 1U;
    while (NRF_RADIO->EVENTS_DISABLED == 0U) {
        // wait
    }
}

void clock_initialization() {
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {
        // wait
    }
    NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {
        // wait
    }
}

int main(void) {
    uint32_t err_code = NRF_SUCCESS;
    packet = 1;
    clock_initialization();
    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
    NRF_LOG_INFO("TRANSMITTER\n");
    bsp_board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);
    radio_configure();
    NRF_RADIO->PACKETPTR = (uint32_t) &packet;
    while (true) {
        send_packet();
        NRF_LOG_INFO("The contents of the package was %u", (unsigned int) packet);
        nrf_delay_ms(1000);
    }
}

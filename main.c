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

uint32_t read_packet() {
    uint32_t result = 0;
    NRF_RADIO->EVENTS_READY = 0U;
    // Enable radio and wait for ready
    NRF_RADIO->TASKS_RXEN = 1U;
    while (NRF_RADIO->EVENTS_READY == 0U) {
        // wait
    }
    NRF_RADIO->EVENTS_END = 0U;
    // Start listening and wait for address received event
    NRF_RADIO->TASKS_START = 1U;
    // Wait for end of packet or buttons state changed
    while (NRF_RADIO->EVENTS_END == 0U) {
        // wait
    }
    if (NRF_RADIO->CRCSTATUS == 1U) {
        result = packet;
    }
    NRF_RADIO->EVENTS_DISABLED = 0U;
    // Disable radio
    NRF_RADIO->TASKS_DISABLE = 1U;
    while (NRF_RADIO->EVENTS_DISABLED == 0U) {
        // wait
    }
    return result;
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
    NRF_LOG_INFO("receiver\n");
    bsp_board_init(BSP_INIT_LEDS);
    radio_configure();
    NRF_RADIO->PACKETPTR = (uint32_t) &packet;
    NRF_LOG_INFO("Radio receiver example started.");
    NRF_LOG_INFO("Wait for first packet");
    NRF_LOG_FLUSH();
    while (true) {
        uint32_t received = read_packet();
        NRF_LOG_INFO("Packet was received");
        NRF_LOG_INFO("The contents of the package is %u", (unsigned int) received);
        NRF_LOG_FLUSH();
    }
}

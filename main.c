#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "bsp.h"
#include "nrf_drv_qspi.h"
#include "nrf_delay.h"
#include "app_util_platform.h"
#include "app_error.h"
#include "boards.h"

#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "sdk_config.h"
#include "radio_config.h"
#include "app_timer.h"
#include "bsp_config.h"
#include "app_button.h"

static uint32_t packet;

void clock_initialization() {
    NRF_CLOCK->EVENTS_HFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_HFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_HFCLKSTARTED == 0) {
        // Wait.
    }
    NRF_CLOCK->LFCLKSRC = (CLOCK_LFCLKSRC_SRC_Xtal << CLOCK_LFCLKSRC_SRC_Pos);
    NRF_CLOCK->EVENTS_LFCLKSTARTED = 0;
    NRF_CLOCK->TASKS_LFCLKSTART = 1;
    while (NRF_CLOCK->EVENTS_LFCLKSTARTED == 0) {
        // Wait.
    }
}


uint32_t read_packet() {
    uint32_t result = 0;
    NRF_RADIO->EVENTS_READY = 0U;
    NRF_RADIO->TASKS_RXEN = 1U;
    while (NRF_RADIO->EVENTS_READY == 0U) {
        // Wait
    }
    NRF_RADIO->EVENTS_END = 0U;
    NRF_RADIO->TASKS_START = 1U;
    while (NRF_RADIO->EVENTS_END == 0U) {
        // Wait
    }
    if (NRF_RADIO->CRCSTATUS == 1U) {
        result = packet;
    }
    NRF_RADIO->EVENTS_DISABLED = 0U;
    NRF_RADIO->TASKS_DISABLE = 1U;
    while (NRF_RADIO->EVENTS_DISABLED == 0U) {
        // Wait
    }
    return result;
}


int main(void) {
    uint32_t err_code = NRF_SUCCESS;
    clock_initialization();
    err_code = app_timer_init();
    APP_ERROR_CHECK(err_code);
    err_code = NRF_LOG_INIT(NULL);
    APP_ERROR_CHECK(err_code);
    NRF_LOG_DEFAULT_BACKENDS_INIT();
    err_code = bsp_init(BSP_INIT_LEDS, NULL);
    APP_ERROR_CHECK(err_code);
    radio_configure();
    NRF_RADIO->PACKETPTR = (uint32_t) &packet;
    err_code = bsp_indication_set(BSP_INDICATE_USER_STATE_OFF);
    NRF_LOG_INFO("Radio receiver example started.");
    NRF_LOG_INFO("Wait for first packet");
    APP_ERROR_CHECK(err_code);
    NRF_LOG_FLUSH();
    while (true) {
        uint32_t received = read_packet();
        err_code = bsp_indication_set(BSP_INDICATE_RCV_OK);
        NRF_LOG_INFO("Packet was received");
        APP_ERROR_CHECK(err_code);
        NRF_LOG_INFO("The contents of the package is %u", (unsigned int) received);
        NRF_LOG_FLUSH();
    }
}

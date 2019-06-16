void delay_us(uint16_t us) {
    NRF_TIMER1->TASKS_CLEAR = 1; /**<set Timer COUNTER>**/

    NRF_TIMER1->MODE = 0x0000;  /**<set Timer COUNTER>**/
    NRF_TIMER1->PRESCALER = 0x0004; /**<16Mhz / 2**PRESCALER = 1Mhz>**/
    NRF_TIMER1->BITMODE = 0x0000; /**<16Bit use>**/
    NRF_TIMER1->CC[0] = us; /**<set Timer COUNTER>**/
    NRF_TIMER1->SHORTS = 0x0001;
    NRF_TIMER1->TASKS_START = 1;


    while (NRF_TIMER1->EVENTS_COMPARE[0] == 0);
    NRF_TIMER1->EVENTS_COMPARE[0] = 0;
    NRF_TIMER1->TASKS_STOP = 1;
}

void delay_ms(uint32_t ms) {
    // delay based on 16 bit TIMER1, max delay 2^32 ms
    uint32_t k;
    for (k = 0; k < ms; k++) {
        delay_us(1000);
    }
}
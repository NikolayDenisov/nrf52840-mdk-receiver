#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
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
}

uint16_t pwm_seq[4] = {5000, 8000, 12000, 15000};


void init_pwm(void) {
    NRF_PWM0->PSEL.OUT[0] = (22 << PWM_PSEL_OUT_PIN_Pos) | (PWM_PSEL_OUT_CONNECT_Connected
            << PWM_PSEL_OUT_CONNECT_Pos); /**Output pin select for PWM channel 0**/
    NRF_PWM0->ENABLE = 1;
    NRF_PWM0->MODE = (PWM_MODE_UPDOWN_Up << PWM_MODE_UPDOWN_Pos); /**Selects operating mode UpAndDown**/
    NRF_PWM0->PRESCALER = (PWM_PRESCALER_PRESCALER_DIV_1
            << PWM_PRESCALER_PRESCALER_Pos); /**Configuration for PWM_CLK 16 MHz**/
    NRF_PWM0->COUNTERTOP = (16000<< PWM_COUNTERTOP_COUNTERTOP_Pos); /** Value up to which the pulse generator counter counts 1ms**/
    NRF_PWM0->LOOP = (1 << PWM_LOOP_CNT_Pos); /**Number of playbacks of a loop**/
    NRF_PWM0->DECODER = (PWM_DECODER_LOAD_Individual << PWM_DECODER_LOAD_Pos |
                         PWM_DECODER_MODE_RefreshCount << PWM_DECODER_LOAD_Pos);
    NRF_PWM0->SEQ[0].PTR = ((uint32_t) (pwm_seq)
            << PWM_SEQ_PTR_PTR_Pos); /**Beginning address in RAM of this sequence**/
    NRF_PWM0->SEQ[0].CNT = ((sizeof(pwm_seq) / sizeof(uint16_t))
            << PWM_SEQ_CNT_CNT_Pos); /**Number of values (duty cycles) in this sequence**/
    NRF_PWM0->SEQ[0].REFRESH = 1000; /**Number of additional PWM periods between samples loaded into compare register**/
    NRF_PWM0->SEQ[0].ENDDELAY = 0; /**Time added after the sequence**/
    NRF_PWM0->TASKS_SEQSTART[0] = 1; /**Loads the first PWM value on all enabled channels from sequence 0,**/
}


int main(void) {
    clock_initialization();
    init_pwm();
}
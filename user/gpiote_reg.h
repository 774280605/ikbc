#ifndef GPIOTE_REG__H__
#define GPIOTE_REG__H__

#include "nrf_gpio.h"
#define GPIOTE_REG_MAX_CHANNEL 8


typedef struct{
    uint8_t index;
    uint8_t pin[8];
    uint8_t reset_pin[8];
    uint8_t count;
}GPIOTE_REG_Context; 





void gpiote_reg_init();

void gpiote_reg_in_event(uint8_t pin);

void gpiote_reg_in_uninit();

void gpiote_reg_in_reset();







#endif
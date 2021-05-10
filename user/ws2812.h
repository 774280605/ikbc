#ifndef WS2812__H__
#define WS2812__H__
#include <stdint.h>
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"
#include "app_timer.h"



typedef struct rgb{
  uint8_t g;
  uint8_t r;
  uint8_t b; 
}RGB_t;


typedef struct{
 unsigned char R;
 unsigned char G;
 unsigned char B;
}COLOR_RGB;

 

typedef struct{
 float H;
 float S;
 float V;
}COLOR_HSV;


void ws2812_init();



//��ɫ
void ws2812_set_close();
//��ɫ
void ws2812_set_red();
//��ɫ
void ws2812_set_green();
//��ɫ
void set_color_blue();

//�Զ�����ɫ1
void set_custom_color_1();
void set_custom_color_2();
void set_custom_color_3();
void set_custom_color_4();

//
void led_flash_init();
void led_flash_start();
void led_flash_stop();


void send_color(RGB_t* grb);

void setup_pretty_led();
void increase_light();
void decrease_light();

#endif
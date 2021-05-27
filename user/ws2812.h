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

void ws2812_macro_mode_setup();
void ws2812_macro_put_point(int key);
int kbd_key_convert_pos(char key);

void ws2812_macro_timer_setup();

//白色
void ws2812_set_close();
//红色
void ws2812_set_red();
//绿色
void ws2812_set_green();
//蓝色
void ws2812_color_blue();

//自定义颜色1
void set_custom_color_1();
void set_custom_color_2();
void set_custom_color_3();
void set_custom_color_4();

//
void led_flash_init();
void led_flash_start();
void led_flash_stop();


void ws2812_send_color(RGB_t*rgb);

void setup_pretty_led();
void increase_light();
void decrease_light();

#endif
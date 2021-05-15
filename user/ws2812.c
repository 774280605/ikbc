#include "ws2812.h"
#include "nrf_delay.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
APP_TIMER_DEF(m_led_timer);
APP_TIMER_DEF(m_flash_timer);
APP_TIMER_DEF(m_macro_timer);


RGB_t m_rgb;

int m_lists_index = 0;
RGB_t m_lists[8] = {
    {.r = 155, .g = 48, .b = 255},  //紫色
    {.r = 255, .g = 48, .b = 48},   //红色
    {.r = 0, .g = 250, .b = 154},   //红色青色
    {.r = 0, .g = 245, .b = 255},   //红色青色
    {.r = 0, .g = 245, .b = 255},   //红色青色
    {.r = 0, .g = 255, .b = 127},   //红色青色
    {.r = 255, .g = 106, .b = 106}, //红色青色
    {.r = 255, .g = 20, .b = 147},  //红色青色

};

enum {
    F12_Pos=0,
    F11_Pos=1,
    F10_Pos=2,
    F9_Pos=3,
    F8_Pos=4,
    F7_Pos=5,
    F6_Pos=6,
    F5_Pos=7,
    F4_Pos=8,
    F3_Pos=9,
    F2_Pos=10,
    F1_Pos=11,
    ESC_Pos=12,

    PAGEUP_Pos=13,


};






char m_ws2812_table[86]={0};






bool m_prettyLedRun = false;

void RGB_TO_HSV(const COLOR_RGB *input, COLOR_HSV *output)
{
    float r, g, b, minRGB, maxRGB, deltaRGB;

    r = input->R / 255.0f;
    g = input->G / 255.0f;
    b = input->B / 255.0f;
    minRGB = MIN(r, MIN(g, b));
    maxRGB = MAX(r, MAX(g, b));
    deltaRGB = maxRGB - minRGB;

    output->V = maxRGB;
    if (maxRGB != 0.0)
        output->S = deltaRGB / maxRGB;
    else
        output->S = 0.0;
    if (output->S <= 0.0)
    {
        output->H = -1.0f;
    }
    else
    {
        if (r == maxRGB)
        {
            output->H = (g - b) / deltaRGB;
        }
        else
        {
            if (g == maxRGB)
            {
                output->H = 2.0 + (b - r) / deltaRGB;
            }
            else
            {
                if (b == maxRGB)
                {
                    output->H = 4.0 + (r - g) / deltaRGB;
                }
            }
        }
        output->H = output->H * 60.0;
        if (output->H < 0.0)
        {
            output->H += 360;
        }
        output->H /= 360;
    }
}

void HSV_TO_RGB(COLOR_HSV *input, COLOR_RGB *output)
{
    float R, G, B;
    int k;
    float aa, bb, cc, f;
    if (input->S <= 0.0)
        R = G = B = input->V;
    else
    {
        if (input->H == 1.0)
            input->H = 0.0;
        input->H *= 6.0;
        k = floor(input->H);
        f = input->H - k;
        aa = input->V * (1.0 - input->S);
        bb = input->V * (1.0 - input->S * f);
        cc = input->V * (1.0 - (input->S * (1.0 - f)));
        switch (k)
        {
        case 0:
            R = input->V;
            G = cc;
            B = aa;
            break;
        case 1:
            R = bb;
            G = input->V;
            B = aa;
            break;
        case 2:
            R = aa;
            G = input->V;
            B = cc;
            break;
        case 3:
            R = aa;
            G = bb;
            B = input->V;
            break;
        case 4:
            R = cc;
            G = aa;
            B = input->V;
            break;
        case 5:
            R = input->V;
            G = aa;
            B = bb;
            break;
        }
    }
    output->R = R * 255;
    output->G = G * 255;
    output->B = B * 255;
}

#define SPI_INSTANCE 0
static const nrf_drv_spi_t spi = NRF_DRV_SPI_INSTANCE(SPI_INSTANCE);

//#define SPI1_INSTANCE  1
// static const nrf_drv_spi_t spi1 = NRF_DRV_SPI_INSTANCE(SPI1_INSTANCE);
//#define SPI2_INSTANCE  2
// static const nrf_drv_spi_t spi2 = NRF_DRV_SPI_INSTANCE(SPI2_INSTANCE);

#define WS_HIGH 0x7C
#define WS_LOW 0x70

#define WS2812_DATA_LEN(array) (sizeof(array) / sizeof(array[0]))

uint8_t rest_code[] = {
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
    0x00,
};

uint8_t rx_buffer[2] = {0};
int rest_len = WS2812_DATA_LEN(rest_code);

void spi_event_handler(nrf_drv_spi_evt_t const *p_event, void *p_context)
{
    nrf_delay_us(10);
}

void ws2812_init()
{
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.frequency = NRF_DRV_SPI_FREQ_8M;
    spi_config.ss_pin = NRF_GPIO_PIN_MAP(0, 28);
    spi_config.miso_pin = NRF_GPIO_PIN_MAP(0, 3);
    spi_config.mosi_pin = NRF_GPIO_PIN_MAP(1, 2);
    spi_config.sck_pin = NRF_GPIO_PIN_MAP(1, 11);
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, NULL, NULL));
    memset(m_ws2812_table,1,86); 
    //set_color_green();
}

void ws2812_set_close()
{
    memset(m_ws2812_table,1,86); 
    RGB_t rgb;
    rgb.b = 0;
    rgb.r = 0;
    rgb.g = 0;
    ws2812_send_color(&rgb);
    m_rgb = rgb;
}
void ws2812_set_red()
{
    memset(m_ws2812_table,1,86); 
    RGB_t rgb = {.b = 0, .g = 0, .r = 0xff};
    ws2812_send_color(&rgb);
    m_rgb = rgb;
}
void ws2812_set_green()
{
    memset(m_ws2812_table,1,86); 
    RGB_t rgb;
    rgb.b = 0;
    rgb.r = 0;
    rgb.g = 0xff;
    ws2812_send_color(&rgb);
    m_rgb = rgb;
}
void ws2812_color_blue()
{
    memset(m_ws2812_table,1,86); 
    RGB_t rgb;

    rgb.r = 0;
    rgb.g = 0;
    rgb.b = 0xff;
    ws2812_send_color(&rgb);
    m_rgb = rgb;
}

void set_custom_color_1()
{
    memset(m_ws2812_table,1,86); 
    RGB_t rgb;
    rgb.r = 155;
    rgb.g = 48;
    rgb.b = 255;
    ws2812_send_color(&rgb);
    m_rgb = rgb;
}

void set_custom_color_2()
{
     memset(m_ws2812_table,1,86); 
    RGB_t rgb;
    rgb.g = 0xff;
    rgb.r = 0;
    rgb.b = 0xff;
    ws2812_send_color(&rgb);
    m_rgb = rgb;
}

void set_custom_color_3()
{
     memset(m_ws2812_table,1,86); 
    RGB_t rgb;
    rgb.g = 0;
    rgb.r = 0xff;
    rgb.b = 0xff;
    ws2812_send_color(&rgb);
    m_rgb = rgb;
}

void set_custom_color_4()
{
    memset(m_ws2812_table,1,86); 
    RGB_t rgb;
    rgb.g = 0;
    rgb.r = 0x0f;
    rgb.b = 0xff;
    ws2812_send_color(&rgb);
    m_rgb = rgb;
}

uint32_t m_flashCount = 0;
static void led_flash_handler(void *p_context)
{
    ++m_flashCount;
    if (m_flashCount % 2 == 1)
        ws2812_set_green();
    else
        ws2812_color_blue();
}

static void led_mode_handler(void *ctx)
{
    RGB_t rgb;
    rgb = m_lists[m_lists_index];
    ws2812_send_color(&rgb);
    m_lists_index++;
    m_lists_index %= 8;
}

static void ws2812_macro_timer_handler(void*ctx)
{
    RGB_t rgb={.b=0,.g=0,.r=0xff};
    ws2812_send_color(&rgb);
    
}

void led_flash_init()
{
    ret_code_t ret;
    //ret = app_timer_create(&m_led_timer, APP_TIMER_MODE_REPEATED, led_flash_handler);
    //APP_ERROR_CHECK(ret);
    ret = app_timer_create(&m_flash_timer, APP_TIMER_MODE_REPEATED, led_mode_handler);
    APP_ERROR_CHECK(ret);

     ret = app_timer_create(&m_macro_timer, APP_TIMER_MODE_SINGLE_SHOT, ws2812_macro_timer_handler);
    APP_ERROR_CHECK(ret);
}

void pretty_led_start()
{
    app_timer_start(m_flash_timer, APP_TIMER_TICKS(10000), NULL);
}
void pretty_led_stop()
{
    app_timer_stop(m_flash_timer);
}

void setup_pretty_led()
{
    if (m_prettyLedRun == false)
    {
        pretty_led_start();
        m_prettyLedRun = true;
    }
    else
    {
        pretty_led_stop();
        m_prettyLedRun = false;
    }
}

void led_flash_start()
{
    app_timer_start(m_led_timer, APP_TIMER_TICKS(200), NULL);
}

void led_flash_stop()
{
    app_timer_stop(m_led_timer);
    ws2812_set_green();
}

void ws2812_send_color(RGB_t *grb)
{
    char rgb_val[24] = {0};
    int index = 0;
    for (int i = 7; i >= 0; --i)
    {
        if ((grb->g >> i) & 1 == 1)
        {
            rgb_val[index++] = WS_HIGH;
        }
        else
        {
            rgb_val[index++] = WS_LOW;
        }
    }

    for (int i = 7; i >= 0; --i)
    {
        if ((grb->r >> i) & 1 == 1)
        {
            rgb_val[index++] = WS_HIGH;
        }
        else
        {
            rgb_val[index++] = WS_LOW;
        }
    }
    for (int i = 7; i >= 0; --i)
    {
        if ((grb->b >> i) & 1 == 1)
        {
            rgb_val[index++] = WS_HIGH;
        }
        else
        {
            rgb_val[index++] = WS_LOW;
        }
    }

    //开启27个灯
    // for(int i=0;i<84;++i)
    for (int i = 0; i < 86; ++i)
    {
        if(m_ws2812_table[i])
            APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, rgb_val, 24, NULL, 0));
          else
          {
             char closeCode[24]={0};
             for(int j=0;j<24;++j)
             {
                closeCode[j]=WS_LOW;
             }
             APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, closeCode, 24, NULL, 0));
          }
    }

    APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, rest_code, rest_len, NULL, 0));
}

void increase_light()
{
    COLOR_RGB c_rgb;
    COLOR_HSV c_hsv;
    c_rgb.R = m_rgb.r;
    c_rgb.B = m_rgb.b;
    c_rgb.G = m_rgb.g;

    RGB_TO_HSV(&c_rgb, &c_hsv);
    c_hsv.V = c_hsv.V + 0.1;
    if (c_hsv.V >= 1)
        return;
    HSV_TO_RGB(&c_hsv, &c_rgb);

    RGB_t rgb;
    rgb.b = c_rgb.B;
    rgb.r = c_rgb.R;
    rgb.g = c_rgb.G;
    ws2812_send_color(&rgb);
    m_rgb = rgb;
}
void decrease_light()
{
    COLOR_RGB c_rgb;
    COLOR_HSV c_hsv;
    c_rgb.R = m_rgb.r;
    c_rgb.B = m_rgb.b;
    c_rgb.G = m_rgb.g;

    RGB_TO_HSV(&c_rgb, &c_hsv);
    c_hsv.V = c_hsv.V - 0.1;
    if (c_hsv.V <= 0.0)
        return;
    HSV_TO_RGB(&c_hsv, &c_rgb);

    RGB_t rgb;
    rgb.b = c_rgb.B;
    rgb.r = c_rgb.R;
    rgb.g = c_rgb.G;
    ws2812_send_color(&rgb);
    m_rgb = rgb;
}


void ws2812_macro_mode_setup()
{
    memset(m_ws2812_table,0,86);
}

void ws2812_macro_put_point(int pos)
{
    if(pos<0 || pos >85)
        return ;

    m_ws2812_table[pos]=1;
}
int kbd_key_convert_pos(char key)
{
    switch(key)
    {
        case F12_Pos:
            
            break;

        default:
            break;
    }

    return 0;
}

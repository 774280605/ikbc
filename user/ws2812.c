#include "ws2812.h"
#include "nrf_delay.h"
#include <math.h>
#include <stdlib.h>
APP_TIMER_DEF(m_led_timer);
APP_TIMER_DEF(m_flash_timer);

RGB_t m_rgb;

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
    spi_config.ss_pin = NRF_GPIO_PIN_MAP(0, 24);
    spi_config.miso_pin = NRF_GPIO_PIN_MAP(1, 0);
    spi_config.mosi_pin = NRF_GPIO_PIN_MAP(0, 20);
    spi_config.sck_pin = NRF_GPIO_PIN_MAP(1, 2);
    APP_ERROR_CHECK(nrf_drv_spi_init(&spi, &spi_config, NULL, NULL));
}

void ws2812_set_white()
{
    RGB_t rgb;
    rgb.b = 0xff;
    rgb.r = 0xff;
    rgb.g = 0xff;
    send_color(&rgb);
    m_rgb = rgb;
}
void ws2812_set_red()
{
    RGB_t rgb = {.b = 0, .g = 0, .r = 0xff};
    send_color(&rgb);
    m_rgb = rgb;
}
void ws2812_set_green()
{
    RGB_t rgb;
    rgb.b = 0;
    rgb.r = 0;
    rgb.g = 0xff;
    send_color(&rgb);
    m_rgb = rgb;
}
void ws2812_set_blue()
{
    RGB_t rgb;
    rgb.b = 0xff;
    rgb.r = 0;
    rgb.g = 0;
    send_color(&rgb);
    m_rgb = rgb;
}

void set_custom_color_1()
{
    RGB_t rgb;
    rgb.g = 0xff;
    rgb.r = 0xff;
    rgb.b = 0;
    send_color(&rgb);
    m_rgb = rgb;
}

void set_custom_color_2()
{
    RGB_t rgb;
    rgb.g = 0xff;
    rgb.r = 0;
    rgb.b = 0xff;
    send_color(&rgb);
    m_rgb = rgb;
}

void set_custom_color_3()
{
    RGB_t rgb;
    rgb.g = 0;
    rgb.r = 0xff;
    rgb.b = 0xff;
    send_color(&rgb);
    m_rgb = rgb;
}

void set_custom_color_4()
{
    RGB_t rgb;
    rgb.g = 0;
    rgb.r = 0x0f;
    rgb.b = 0xff;
    send_color(&rgb);
    m_rgb = rgb;
}

uint32_t m_flashCount = 0;
static void
led_flash_handler(void *p_context)
{
    ++m_flashCount;
    if (m_flashCount % 2 == 1)
        ws2812_set_green();
    else
        ws2812_set_blue();
}

static void
led_mode_handler(void *ctx)
{
    RGB_t rgb;
    rgb.g = rand() % 255;
    rgb.r = rand() % 255;
    rgb.b = rand() % 255;

    send_color(&rgb);
}

void led_flash_init()
{
    ret_code_t ret;
    ret = app_timer_create(
        &m_led_timer, APP_TIMER_MODE_REPEATED, led_flash_handler);
    APP_ERROR_CHECK(ret);
    ret = app_timer_create(
        &m_flash_timer, APP_TIMER_MODE_REPEATED, led_mode_handler);
    APP_ERROR_CHECK(ret);
}

void pretty_led_start()
{
    app_timer_start(m_flash_timer, APP_TIMER_TICKS(200), NULL);
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

void send_color(RGB_t *grb)
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

    //����27����
    // for(int i=0;i<84;++i)
    for (int i = 0; i < 84; ++i)
    {
        APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi, rgb_val, 24, NULL, 0));
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
    send_color(&rgb);
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
    send_color(&rgb);
    m_rgb = rgb;
}
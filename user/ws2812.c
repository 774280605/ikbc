#include "ws2812.h"
#include "nrf_delay.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include "kbd.h"
APP_TIMER_DEF(m_led_timer);
APP_TIMER_DEF(m_flash_timer);
APP_TIMER_DEF(m_macro_timer);


RGB_t m_rgb={.g=0xff,.r=0,.b=0};

int m_current_point=0;



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
    HOME_Pos=14,
    INSERT_Pos=15,
    BACKSPACE_Pos=16, 
    ADD_Pos=17,
    SUB_Pos=18,
    ZERO_Pos=19,
    NINE_Pos=20,
    EIGHT_Pos=21,
    SEVEN_Pos=22,
    SIX_Pos=23,
    FIVE_Pos=24,
    FOUR_Pos=25,
    THREE_Pos=26,
    TWO_Pos=27,
    ONE_Pos=28,
    BOLANXIAN_Pos=29,
    PAGEDOWN_Pos=30,
    END_Pos=31,
    DELETE_Pos=32,
    SLASH_Pos=33,
    CLOSEBRACKET_Pos=34,
    OPENBRACKET_Pos=35,
    P_Pos=36,
    O_Pos=37,
    I_Pos=38,
    U_Pos=39,
    Y_Pos=40,
    T_Pos=41,
    R_Pos=42,
    E_Pos=43,
    W_Pos=44,
    Q_Pos=45,
    TAB_Pos=46,
    ENTER_Pos=47,
    YINHAO_Pos=48,
    MAOHAO_Pos=49,
    L_Pos=50,
    K_Pos=51,
    J_Pos=52,
    H_Pos=53,
    G_Pos=54,
    F_Pos=55,
    D_Pos=56,
    S_Pos=57,
    A_Pos=58,
    CAPS_Pos=59,
    UP_Pos=60,
    RSHIFT_Pos=61,
    WHY_Pos=62,
    JUHAO_Pos=63,
    DOUHAO_Pos=64,
    M_Pos=65,
    N_Pos=66,
    B_Pos=67,
    V_Pos=68,
    C_Pos=69,
    X_Pos=70,
    Z_Pos=71,
    LSHIFT_Pos=72,
    RIGHT_Pos=73,
    DOWN_Pos=74,
    LEFT_Pos=75,
    RCTRL_Pos=76,
    MENU_Pos=77,
    RFN_Pos=78,
    RALT_Pos=79,
    SPACE3_Pos=80,
    SPACE2_Pos=81,
    SPACE1_Pos=82,
    LALT_Pos=83,
    WIN_Pos=84,
    LFN_Pos=85

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
    memset(m_ws2812_table,0,86);
    m_ws2812_table[m_current_point]=1;
    ws2812_send_color(&m_rgb);
    ++m_current_point;
    m_current_point%=86;
}

static void ws2812_macro_timer_handler(void*ctx)
{
    RGB_t rgb={.b=0,.g=0xff,.r=0};
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

void ws2812_macro_timer_setup()
{
            app_timer_start(m_macro_timer, APP_TIMER_TICKS(500), NULL);

}



void pretty_led_start()
{
    app_timer_start(m_flash_timer, APP_TIMER_TICKS(500), NULL);
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

void ws2812_macro_put_point(int key)
{
    int pos=kbd_key_convert_pos(key);

    if(pos<0 || pos >85)
        return ;

    m_ws2812_table[pos]=1;
    RGB_t rgb={.b=0,.g=0xff,.r=0};
    ws2812_send_color(&rgb);
}
int kbd_key_convert_pos(char key)
{
    switch(key)
    {
       case  _A  :     
                return A_Pos;
       case   _B :    
                return B_Pos;
       case   _C :     
                return C_Pos;
       case   _D :     
                return D_Pos;
       case   _E :     
                return E_Pos;
       case   _F :     
                return F_Pos;
       case   _G :     
                return G_Pos;
       case   _H :     
                return H_Pos;
       case   _I :     
                return I_Pos;
       case   _J :     
                return J_Pos;
       case   _K :     
                return K_Pos;
       case   _L :     
                return L_Pos;
       case   _M :     
                return M_Pos;
       case   _N :     
                return N_Pos;
       case   _O :     
                return O_Pos;

       case   _P :     
                return P_Pos;
       case   _Q :     
                return Q_Pos;
       case   _R :     
                return R_Pos;
       case   _S :     
                return S_Pos;
       case   _T :     
                return T_Pos;
       case   _U :     
                return U_Pos;
       case   _V :     
                return V_Pos;
       case   _W :     
                return W_Pos;
       case   _X :     
                return X_Pos;
       case   _Y :     
                return Y_Pos;
       case   _Z :     
                return Z_Pos;
       case   _1 :     
                return ONE_Pos;
       case   _2 :     
                return TWO_Pos;
       case   _3 :     
                return THREE_Pos;
       case   _4 :     
                return FOUR_Pos;
       case   _5 :     
                return FIVE_Pos;
       case   _6 :     
                return SIX_Pos;
       case   _7 :     
                return SEVEN_Pos;
       case   _8 :     
                return EIGHT_Pos;
       case   _9 :     
                return NINE_Pos;
       case   _0 :     
                return ZERO_Pos;
       case   _ENTER             :
       case   _ESC               :
       case   _BACKSPACE         :
       case   _TAB               :
       case   _SPACEBAR          :
       case   _UNDERSCORE        :
       case   _PLUS              :
       case   _OPEN_BRACKET      :
       case   _CLOSE_BRACKET     :
       case   _BACKSLASH         :
       case   _ASH               :
       case   _COLON             :
       case   _QUOTE             :
       case   _TILDE             :
       case   _COMMA             :
       case   _DOT               :
       case   _SLASH             :
       case   _CAPS_LOCK         :
       case   _F1   :
       case   _F2   :
       case   _F3   :
       case   _F4   :
       case   _F5   :
       case   _F6   :
       case   _F7   :
       case   _F8   :
       case   _F9   :
       case   _F10  :
       case   _F11  :
       case   _F12  :
       case   _PRINTSCREEN   :  
       case   _SCROLL_LOCK   :  
       case   _PAUSE   :        
       case   _INSERT  :        
       case   _HOME    :        
       case   _PAGEUP  :        
       case   _DELETE  :        
       case   _END      :       
       case   _PAGEDOWN :       
       case   _RIGHT:           
       case   _LEFT :           
       case   _DOWN :           
       case   _UP   :           

        default:
            break;
    }

    return 0;
}

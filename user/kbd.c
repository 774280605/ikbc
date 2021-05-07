#include "kbd.h"
#include "SEGGER_RTT.h"
#include "app_usbd_hid_kbd_internal.h"
#include "low_energy_timer.h"
#include "macro.h"
#include "nrf_delay.h"
#include "nrf_drv_gpiote.h"
#include "nrf_gpiote.h"
#include "nrf_gzll.h"
#include "ws2812.h"
#include <stdbool.h>
#include <string.h>
KBD_Context_t m_kbdContext;

#define KEY_A 4
#define KEY_B 5
#define KEY_C 6
#define KEY_D 7
#define KEY_E 8
#define KEY_F 9
#define KEY_G 10
#define KEY_H 11
#define KEY_I 12
#define KEY_J 13
#define KEY_K 14
#define KEY_L 15
#define KEY_M 16
#define KEY_N 17
#define KEY_O 18
#define KEY_P 19
#define KEY_Q 20
#define KEY_R 21
#define KEY_S 22
#define KEY_T 23
#define KEY_U 24
#define KEY_V 25
#define KEY_W 26
#define KEY_X 27
#define KEY_Y 28
#define KEY_Z 29

#define KEY_1 30
#define KEY_2 31
#define KEY_3 32
#define KEY_4 33
#define KEY_5 34
#define KEY_6 35
#define KEY_7 36
#define KEY_8 37
#define KEY_9 38
#define KEY_0 39

#define KEY_RETURN 40
#define KEY_ESC 41
#define KEY_BACKSPACE 42
#define KEY_TAB 43
#define KEY_SPACEBAR 44
#define KEY_SUB 45           //-_
#define KEY_ADD 46           //+=
#define KEY_LEFT_BRACKET 47  //[{
#define KEY_RIGHT_BRACKET 48 //]}
#define KEY_SLASH 49         // \|
#define KEY_SEM 51           //·ÖºÅ
#define KEY_YINHAO 52        //'" ÒýºÅ
#define KEY_TILDE 53         //`~ ²¨ÀËÏß
#define KEY_COMMA 54         //,¶ººÅ
#define KEY_PERIOD 55        //.¾äºÅ
#define KEY_WHY 56           //ÎÊºÅ

#define KEY_CAPS 57

#define KEY_F1 58
#define KEY_F2 59
#define KEY_F3 60
#define KEY_F4 61
#define KEY_F5 62
#define KEY_F6 63
#define KEY_F7 64
#define KEY_F8 65
#define KEY_F9 66
#define KEY_F10 67
#define KEY_F11 68
#define KEY_F12 69

#define KEY_PRINTSCREEN 70
#define KEY_SCROLLLOCK 71
#define KEY_PAUSE 72
#define KEY_INSERT 73
#define KEY_HOME 74
#define KEY_PAGEUP 75
#define KEY_DELETE 76
#define KEY_END 77
#define KEY_PAGEDOWN 78
#define KEY_RIGHTARROW 79
#define KEY_LEFTARROW 80
#define KEY_DOWNARROW 81
#define KEY_UPARROW 82
#define KEY_WIN 0xE3
#define KEY_MENU 0
#define KEY_FN 0

#define KEY_L_CTRL 0xE0
#define KEY_L_SHIFT 0xE1
#define KEY_L_ALT 0xE2
#define KEY_R_CTRL 0xE4
#define KEY_R_SHIFT 0xE5
#define KEY_R_ALT 0xE6

uint32_t m_row[6] = {
    NRF_GPIO_PIN_MAP(1, 11), // row0
    NRF_GPIO_PIN_MAP(1, 10), // row1
    NRF_GPIO_PIN_MAP(0, 3),  // row2
    NRF_GPIO_PIN_MAP(0, 28), // row3
    NRF_GPIO_PIN_MAP(1, 13), // row4
    NRF_GPIO_PIN_MAP(0, 2)}; // row5

uint32_t m_col[15] = {
    NRF_GPIO_PIN_MAP(0, 29), // 0
    NRF_GPIO_PIN_MAP(0, 31), // 1
    NRF_GPIO_PIN_MAP(0, 30), // 2
    NRF_GPIO_PIN_MAP(0, 0),  // 3w
    NRF_GPIO_PIN_MAP(0, 26), // 4
    NRF_GPIO_PIN_MAP(0, 1),  // 5
    NRF_GPIO_PIN_MAP(0, 6),  // 6
    NRF_GPIO_PIN_MAP(0, 5),  // 7
    NRF_GPIO_PIN_MAP(0, 8),  // 8
    NRF_GPIO_PIN_MAP(1, 9),  // 9
    NRF_GPIO_PIN_MAP(0, 4),  // 10
    NRF_GPIO_PIN_MAP(0, 12), // 11
    NRF_GPIO_PIN_MAP(0, 7),  // 12
    NRF_GPIO_PIN_MAP(0, 15), // 13
    NRF_GPIO_PIN_MAP(0, 17), // 14
};

uint32_t m_row_col_0[15] = {
    NRF_GPIO_PIN_MAP(0, 29), // 0
    NRF_GPIO_PIN_MAP(0, 31), // 1
    NRF_GPIO_PIN_MAP(0, 30), // 2
    NRF_GPIO_PIN_MAP(0, 0),  // 3
    NRF_GPIO_PIN_MAP(0, 26), // 4
    NRF_GPIO_PIN_MAP(0, 1),  // 5
    NRF_GPIO_PIN_MAP(0, 6),  // 6
    NRF_GPIO_PIN_MAP(0, 5),  // 7
    NRF_GPIO_PIN_MAP(0, 8),  // 8
    NRF_GPIO_PIN_MAP(1, 9),  // 9
    NRF_GPIO_PIN_MAP(0, 4),  // 10
    NRF_GPIO_PIN_MAP(0, 12), // 11
    NRF_GPIO_PIN_MAP(0, 7),  // 12
    NRF_GPIO_PIN_MAP(0, 15), // 13
    NRF_GPIO_PIN_MAP(0, 17), // 14
};

uint32_t m_row_col_1[15] = {
    NRF_GPIO_PIN_MAP(0, 29), // 0
    NRF_GPIO_PIN_MAP(0, 31), // 1
    NRF_GPIO_PIN_MAP(0, 30), // 2
    NRF_GPIO_PIN_MAP(0, 0),  // 3
    NRF_GPIO_PIN_MAP(0, 26), // 4
    NRF_GPIO_PIN_MAP(0, 1),  // 5
    NRF_GPIO_PIN_MAP(0, 6),  // 6
    NRF_GPIO_PIN_MAP(0, 5),  // 7
    NRF_GPIO_PIN_MAP(0, 8),  // 8
    NRF_GPIO_PIN_MAP(1, 9),  // 9
    NRF_GPIO_PIN_MAP(0, 4),  // 10
    NRF_GPIO_PIN_MAP(0, 12), // 11
    NRF_GPIO_PIN_MAP(0, 7),  // 12
    NRF_GPIO_PIN_MAP(0, 15), // 13
    NRF_GPIO_PIN_MAP(0, 17), // 14

};
uint32_t m_row_col_2[15] = {
    NRF_GPIO_PIN_MAP(0, 29), // 0
    NRF_GPIO_PIN_MAP(0, 31), // 1
    NRF_GPIO_PIN_MAP(0, 30), // 2
    NRF_GPIO_PIN_MAP(0, 0),  // 3
    NRF_GPIO_PIN_MAP(0, 26), // 4
    NRF_GPIO_PIN_MAP(0, 1),  // 5
    NRF_GPIO_PIN_MAP(0, 6),  // 6
    NRF_GPIO_PIN_MAP(0, 5),  // 7
    NRF_GPIO_PIN_MAP(0, 8),  // 8
    NRF_GPIO_PIN_MAP(1, 9),  // 9
    NRF_GPIO_PIN_MAP(0, 4),  // 10
    NRF_GPIO_PIN_MAP(0, 12), // 11
    NRF_GPIO_PIN_MAP(0, 7),  // 12
    NRF_GPIO_PIN_MAP(0, 15), // 13
    NRF_GPIO_PIN_MAP(0, 17), // 14

};
uint32_t m_row_col_3[15] = {
    NRF_GPIO_PIN_MAP(0, 29), // 0
    NRF_GPIO_PIN_MAP(0, 31), // 1
    NRF_GPIO_PIN_MAP(0, 30), // 2
    NRF_GPIO_PIN_MAP(0, 0),  // 3
    NRF_GPIO_PIN_MAP(0, 26), // 4
    NRF_GPIO_PIN_MAP(0, 1),  // 5
    NRF_GPIO_PIN_MAP(0, 6),  // 6
    NRF_GPIO_PIN_MAP(0, 5),  // 7
    NRF_GPIO_PIN_MAP(0, 8),  // 8
    NRF_GPIO_PIN_MAP(1, 9),  // 9
    NRF_GPIO_PIN_MAP(0, 4),  // 10
    NRF_GPIO_PIN_MAP(0, 12), // 11
    NRF_GPIO_PIN_MAP(0, 7),  // 12
    NRF_GPIO_PIN_MAP(0, 15), // 13
    NRF_GPIO_PIN_MAP(0, 17), // 14

};

uint32_t m_row_col_4[13] = {
    NRF_GPIO_PIN_MAP(0, 29), // 0
    NRF_GPIO_PIN_MAP(0, 31), // 1
    NRF_GPIO_PIN_MAP(0, 30), // 2
    NRF_GPIO_PIN_MAP(0, 0),  // 3
    NRF_GPIO_PIN_MAP(0, 26), // 4
    NRF_GPIO_PIN_MAP(0, 1),  // 5
    NRF_GPIO_PIN_MAP(0, 6),  // 6
    NRF_GPIO_PIN_MAP(0, 5),  // 7
    NRF_GPIO_PIN_MAP(0, 8),  // 8
    NRF_GPIO_PIN_MAP(1, 9),  // 9
    NRF_GPIO_PIN_MAP(0, 4),  // 10
    NRF_GPIO_PIN_MAP(0, 12), // 11
    NRF_GPIO_PIN_MAP(0, 7),  // 12

};

uint32_t m_row_col_5[11] = {
    NRF_GPIO_PIN_MAP(0, 29), // 0
    NRF_GPIO_PIN_MAP(0, 31), // 1
    NRF_GPIO_PIN_MAP(0, 30), // 2
    NRF_GPIO_PIN_MAP(0, 0),  // 3
    NRF_GPIO_PIN_MAP(0, 26), // 4
    NRF_GPIO_PIN_MAP(0, 1),  // 5
    NRF_GPIO_PIN_MAP(0, 6),  // 6
    NRF_GPIO_PIN_MAP(0, 5),  // 7
    NRF_GPIO_PIN_MAP(0, 8),  // 8
    NRF_GPIO_PIN_MAP(1, 9),  // 9
    NRF_GPIO_PIN_MAP(0, 4),  // 10

};

uint32_t m_unuse_pin[5] = {
    NRF_GPIO_PIN_MAP(0, 24),
    NRF_GPIO_PIN_MAP(1, 0),
    NRF_GPIO_PIN_MAP(1, 2),
    NRF_GPIO_PIN_MAP(1, 4),
    NRF_GPIO_PIN_MAP(1, 6),
};

unsigned char m_KBD_TABLE[6][15] = {
    {KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10, KEY_F11, KEY_F12,
        KEY_PAGEUP, KEY_HOME},
    {KEY_TILDE, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_SUB, KEY_ADD, KEY_BACKSPACE,
        KEY_INSERT},
    {KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, KEY_LEFT_BRACKET, KEY_RIGHT_BRACKET,
        KEY_SLASH, KEY_DELETE},
    {KEY_L_CTRL, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_SEM, KEY_YINHAO, KEY_RETURN,
        KEY_END, KEY_PAGEDOWN},
    {KEY_L_SHIFT, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M, KEY_COMMA, KEY_PERIOD, KEY_WHY,
        KEY_R_SHIFT, KEY_UPARROW, 0, 0},
    {KEY_CAPS, KEY_WIN, KEY_L_ALT, KEY_SPACEBAR, KEY_R_ALT, KEY_MENU, KEY_FN, KEY_R_CTRL,
        KEY_LEFTARROW, KEY_DOWNARROW, KEY_RIGHTARROW, 0, 0, 0, 0}};

KEY_Point LCTRL_KEY = {3, 0};
KEY_Point LSHIFT_KEY = {4, 0};
KEY_Point LALT_KEY = {5, 2};

KEY_Point RCTRL_KEY = {5, 7};
KEY_Point RSHIFT_KEY = {4, 11};
KEY_Point RALT_KEY = {5, 4};
KEY_Point WIN_KEY = {5, 0x01};

KEY_Point FN_KEY = {5, 5};
KEY_Point MENU_KEY = {5, 6};

KEY_Point UP_KEY = {4, 12};
KEY_Point LEFT_KEY = {5, 8};
KEY_Point DOWN_KEY = {5, 9};
KEY_Point RIGTH_KEY = {5, 10};

KEY_Point A_KEY = {3, 1};
KEY_Point Q_KEY = {2, 1};
KEY_Point W_KEY = {2, 2};
KEY_Point E_KEY = {2, 3};
KEY_Point CAPS_KEY = {5, 0};
KEY_Point BOLANGXIAN_KEY = {1, 0};
KEY_Point J_KEY = {3, 7};
KEY_Point K_KEY = {3, 8};
KEY_Point L_KEY = {3, 9};
KEY_Point I_KEY = {2, 8};
KEY_Point BACKSPACE_KEY = {1, 13};

KEY_Point K_1_KEY = {1, 1};
KEY_Point K_2_KEY = {1, 2};
KEY_Point K_3_KEY = {1, 3};
KEY_Point K_4_KEY = {1, 4};
KEY_Point K_5_KEY = {1, 5};
KEY_Point K_6_KEY = {1, 6};
KEY_Point K_7_KEY = {1, 7};
KEY_Point K_8_KEY = {1, 8};
KEY_Point K_9_KEY = {1, 9};
KEY_Point K_0_KEY = {1, 10};

void reset_kbd_status()
{
    for (uint8_t i = 0; i < 6; ++i)
    {
        for (uint8_t j = 0; j < 15; ++j)
        {
            m_kbdContext.m_kbdStatus[i][j] = m_kbdContext.m_kbdLastTimeStatus[i][j] = 0;
        }
    }
}

void kbd_fn_key_handle()
{
    if (m_kbdContext.m_kbdStatus[FN_KEY.row][FN_KEY.col])
    {
        if (m_kbdContext.m_kbdStatus[LEFT_KEY.row][LEFT_KEY.col])
        {
            m_kbdContext.m_connMode = WIRE_MODE;
            reset_kbd_status();
        }
        if (m_kbdContext.m_kbdStatus[RIGTH_KEY.row][RIGTH_KEY.col])
        {
            m_kbdContext.m_connMode = WIRELESS_MODE;
            reset_kbd_status();
        }

        if (m_kbdContext.m_kbdStatus[MENU_KEY.row][MENU_KEY.col])
        {
            reset_macro_table();

            reset_kbd_status();
        }

        // 1
        if (m_kbdContext.m_kbdStatus[K_1_KEY.row][K_1_KEY.col])
        {
            if (m_kbdContext.m_connMode == WIRE_MODE)
            {
                ws2812_set_blue();
                nrf_delay_ms(200);
            }
            reset_kbd_status();
        }
        if (m_kbdContext.m_kbdStatus[K_2_KEY.row][K_2_KEY.col])
        {
            if (m_kbdContext.m_connMode == WIRE_MODE)
            {
                ws2812_set_green();
                nrf_delay_ms(200);
            }
            reset_kbd_status();
        }
        if (m_kbdContext.m_kbdStatus[K_3_KEY.row][K_3_KEY.col])
        {
            if (m_kbdContext.m_connMode == WIRE_MODE)
            {
                ws2812_set_red();
                nrf_delay_ms(200);
            }
            reset_kbd_status();
        }
        if (m_kbdContext.m_kbdStatus[K_4_KEY.row][K_4_KEY.col])
        {
            if (m_kbdContext.m_connMode == WIRE_MODE)
            {
                ws2812_set_blue();
                nrf_delay_ms(200);
            }
            reset_kbd_status();
        }
        if (m_kbdContext.m_kbdStatus[K_5_KEY.row][K_5_KEY.col])
        {
            if (m_kbdContext.m_connMode == WIRE_MODE)
            {
                //set_custom_color_1();
                nrf_delay_ms(200);
            }

            reset_kbd_status();
        }
        if (m_kbdContext.m_kbdStatus[K_6_KEY.row][K_6_KEY.col])
        {
            if (m_kbdContext.m_connMode == WIRE_MODE)
            {
                //set_custom_color_2();
                nrf_delay_ms(200);
            }

            reset_kbd_status();
        }
        if (m_kbdContext.m_kbdStatus[K_7_KEY.row][K_7_KEY.col])
        {
            if (m_kbdContext.m_connMode == WIRE_MODE)
            {
                //set_custom_color_3();
                nrf_delay_ms(200);
            }
            reset_kbd_status();
        }
        if (m_kbdContext.m_kbdStatus[K_8_KEY.row][K_8_KEY.col])
        {
            if (m_kbdContext.m_connMode == WIRE_MODE)
            {
                //set_custom_color_4();
                nrf_delay_ms(200);
            }
            reset_kbd_status();
        }

        if (m_kbdContext.m_kbdStatus[K_0_KEY.row][K_0_KEY.col])
        {
            if (m_kbdContext.m_connMode == WIRE_MODE)
            {
                //setup_pretty_led();
                nrf_delay_ms(200);
            }
            reset_kbd_status();
        }

        if (m_kbdContext.m_kbdStatus[UP_KEY.row][UP_KEY.col])
        {
            if (m_kbdContext.m_connMode == WIRE_MODE)
            {
                increase_light();
                nrf_delay_ms(100);
            }

            reset_kbd_status();
        }
        if (m_kbdContext.m_kbdStatus[DOWN_KEY.row][DOWN_KEY.col])
        {
            if (m_kbdContext.m_connMode == WIRE_MODE)
            {
                decrease_light();
                nrf_delay_ms(100);
            }

            reset_kbd_status();
        }
    }
}

void kbd_init()
{
    memset(&m_kbdContext, 0, sizeof(m_kbdContext));
    m_kbdContext.m_connMode = WIRELESS_MODE;

    for (uint8_t i = 0; i < 5; ++i)
    {
        nrf_gpio_cfg_input(m_unuse_pin[i], NRF_GPIO_PIN_PULLDOWN);
        nrf_delay_us(2);
    }

    kbd_gpio_init();

    //led_flash_init();

    // low_energy_timer_init();
    // kbd_setup_low_energy_tiemr();
    // gpiote_reg_init();
    // kbd_gpiote_enable();
}

void kbd_gpio_init()
{
    for (int col = 0; col < 15; ++col)
    {
        nrf_gpio_cfg_input(m_col[col], NRF_GPIO_PIN_PULLDOWN);
        // nrf_gpio_cfg_output(m_col[col]);

        // nrf_gpio_pin_clear(m_col[col]);
        // nrf_gpio_pin_set(m_col[col]);
    }

    // end test
    for (int row = 0; row < 6; ++row)
    {
        nrf_gpio_cfg_input(m_row[row], NRF_GPIO_PIN_PULLDOWN);
        /*nrf_drv_gpiote_in_config_t in_config=GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);

      in_config.pull=NRF_GPIO_PIN_PULLDOWN;
      ret_code_t ret;
      ret = nrf_drv_gpiote_in_init(m_row[row], &in_config, row_in_event_handler);
      APP_ERROR_CHECK(ret);*/
        nrf_delay_ms(1);
    }
}

void kbd_scan()
{
    for (int i = 0; i < 15; ++i)
    {
        nrf_gpio_cfg_input(m_col[i], NRF_GPIO_PIN_PULLDOWN);
        // nrf_gpio_pin_clear(m_col[i]);
    }

    // 0 row
    for (int i = 0; i < 15; ++i)
    {
        nrf_gpio_cfg_output(m_row_col_0[i]);
        nrf_delay_us(40);
        nrf_gpio_pin_set(m_row_col_0[i]);
        nrf_delay_us(40);
        if (nrf_gpio_pin_read(m_row[0]))
        {
            m_kbdContext.m_kbdStatus[0][i] = 1;
        }
        else
        {
            m_kbdContext.m_kbdStatus[0][i] = 0;
        }
        nrf_gpio_pin_clear(m_row_col_0[i]);
        nrf_delay_us(10);
        nrf_gpio_cfg_input(m_row_col_0[i], NRF_GPIO_PIN_PULLDOWN);
        nrf_delay_us(10);
    }

    // 1 row
    for (int i = 0; i < 15; ++i)
    {
        nrf_gpio_cfg_output(m_row_col_1[i]);
        nrf_delay_us(40);
        nrf_gpio_pin_set(m_row_col_1[i]);
        nrf_delay_us(40);
        if (nrf_gpio_pin_read(m_row[1]))
        {
            m_kbdContext.m_kbdStatus[1][i] = 1;
        }
        else
        {
            m_kbdContext.m_kbdStatus[1][i] = 0;
        }
        nrf_gpio_pin_clear(m_row_col_1[i]);
        nrf_delay_us(10);
        nrf_gpio_cfg_input(m_row_col_1[i], NRF_GPIO_PIN_PULLDOWN);
        nrf_delay_us(10);
    }

    // 2 row
    for (int i = 0; i < 15; ++i)
    {
        nrf_gpio_cfg_output(m_row_col_2[i]);
        nrf_delay_us(40);
        nrf_gpio_pin_set(m_row_col_2[i]);
        nrf_delay_us(40);
        if (nrf_gpio_pin_read(m_row[2]))
        {
            m_kbdContext.m_kbdStatus[2][i] = 1;
        }
        else
        {
            m_kbdContext.m_kbdStatus[2][i] = 0;
        }
        nrf_gpio_pin_clear(m_row_col_2[i]);
        nrf_delay_us(10);
        nrf_gpio_cfg_input(m_row_col_2[i], NRF_GPIO_PIN_PULLDOWN);
        nrf_delay_us(10);
    }

    // 3 row
    for (int i = 0; i < 15; ++i)
    {
        nrf_gpio_cfg_output(m_row_col_3[i]);
        nrf_delay_us(40);
        nrf_gpio_pin_set(m_row_col_3[i]);
        nrf_delay_us(40);
        if (nrf_gpio_pin_read(m_row[3]))
        {
            m_kbdContext.m_kbdStatus[3][i] = 1;
        }
        else
        {
            m_kbdContext.m_kbdStatus[3][i] = 0;
        }
        nrf_gpio_pin_clear(m_row_col_3[i]);
        nrf_delay_us(10);
        nrf_gpio_cfg_input(m_row_col_3[i], NRF_GPIO_PIN_PULLDOWN);
        nrf_delay_us(10);
    }

    // 4 row
    for (int i = 0; i < 13; ++i)
    {
        nrf_gpio_cfg_output(m_row_col_4[i]);
        nrf_delay_us(40);
        nrf_gpio_pin_set(m_row_col_4[i]);

        nrf_delay_us(40);
        if (nrf_gpio_pin_read(m_row[4]))
        {
            m_kbdContext.m_kbdStatus[4][i] = 1;
        }
        else
        {
            m_kbdContext.m_kbdStatus[4][i] = 0;
        }
        nrf_gpio_pin_clear(m_row_col_4[i]);
        nrf_delay_us(10);
        nrf_gpio_cfg_input(m_row_col_4[i], NRF_GPIO_PIN_PULLDOWN);
        nrf_delay_us(10);
    }

    // 5 row
    for (int i = 0; i < 11; ++i)
    {
        nrf_gpio_cfg_output(m_row_col_5[i]);
        nrf_delay_us(40);
        nrf_gpio_pin_set(m_row_col_5[i]);
        nrf_delay_us(40);
        if (nrf_gpio_pin_read(m_row[5]))
        {
            m_kbdContext.m_kbdStatus[5][i] = 1;
        }
        else
        {
            m_kbdContext.m_kbdStatus[5][i] = 0;
        }
        nrf_gpio_pin_clear(m_row_col_5[i]);
        nrf_delay_us(10);
        nrf_gpio_cfg_input(m_row_col_5[i], NRF_GPIO_PIN_PULLDOWN);
        nrf_delay_us(10);
    }
}

void kbd_saveLastStatus()
{
    for (int row = 0; row < 6; ++row)
    {
        for (int col = 0; col < 15; ++col)
        {
            m_kbdContext.m_kbdLastTimeStatus[row][col] = m_kbdContext.m_kbdStatus[row][col];
        }
    }
}

uint8_t kbd_StatusIsChanged()
{
    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 15; ++j)
        {
            if (m_kbdContext.m_kbdLastTimeStatus[i][j] != m_kbdContext.m_kbdStatus[i][j])
            {
                kbd_saveLastStatus();

                return 1;
            }
        }
    }

    return 0;
}

void kbd_updateHidData()
{
    memset(m_kbdContext.m_table, 0, 6);
    m_kbdContext.m_modifier = KBD_MODIFIER_NONE;

    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 15; ++j)
        {
            if (m_kbdContext.m_kbdStatus[i][j])
            {
                if (kbd_modifier(i, j) == 0) // common key
                {
                    if (is_macro_key(m_KBD_TABLE[i][j]))
                    {
                        put_macro_key_to_pkt(m_KBD_TABLE[i][j]);
                        // } else if (kbd_composite_key_handle(i, j)) {
                    }
                    else
                    {
                        uint8_t key = m_KBD_TABLE[i][j];
                        uint8_t index = kbd_availableIndex();
                        m_kbdContext.m_table[index] = key;
                    }
                }
            }
        }
    }
    // m_hidData[0] = m_modifier;
}

uint8_t kbd_availableIndex()
{
    for (int i = 0; i < 6; i++)
    {
        if (m_kbdContext.m_table[i] == 0)
        {
            return i;
        }
    }

    return 5;
}

uint8_t kbd_modifier(uint8_t row, uint8_t col)
{
    if (LSHIFT_KEY.row == row && LSHIFT_KEY.col == col)
    {
        m_kbdContext.m_modifier |= KBD_MODIFIER_LEFT_SHIFT;
    }
    else if (RSHIFT_KEY.row == row && RSHIFT_KEY.col == col)
    {
        m_kbdContext.m_modifier |= KBD_MODIFIER_RIGHT_SHIFT;
    }
    else if (LCTRL_KEY.row == row && LCTRL_KEY.col == col)
    {
        m_kbdContext.m_modifier |= KBD_MODIFIER_LEFT_CTRL;
    }
    else if (RCTRL_KEY.row == row && RCTRL_KEY.col == col)
    {
        m_kbdContext.m_modifier |= KBD_MODIFIER_RIGHT_CTRL;
    }
    else if (LALT_KEY.row == row && LALT_KEY.col == col)
    {
        m_kbdContext.m_modifier |= KBD_MODIFIER_LEFT_ALT;
    }
    else if (RALT_KEY.row == row && RALT_KEY.col == col)
    {
        m_kbdContext.m_modifier |= KBD_MODIFIER_RIGHT_ALT;
    }
    else if (WIN_KEY.row == row && WIN_KEY.col == col)
    {
        m_kbdContext.m_modifier |= KBD_MODIFIER_LEFT_UI;
    }
    else
    {
        return 0;
    }
    return 1;
}

//
uint8_t is_create_macroo()
{
    if (m_kbdContext.m_kbdStatus[FN_KEY.row][FN_KEY.col])
    {
        if (m_kbdContext.m_kbdStatus[Q_KEY.row][Q_KEY.col])
        {
            m_kbdContext.m_currentRecordMacro = m_KBD_TABLE[Q_KEY.row][Q_KEY.col];
            return 1;
        }
        if (m_kbdContext.m_kbdStatus[W_KEY.row][W_KEY.col])
        {
            m_kbdContext.m_currentRecordMacro = m_KBD_TABLE[W_KEY.row][W_KEY.col];
            return 1;
        }
        if (m_kbdContext.m_kbdStatus[E_KEY.row][E_KEY.col])
        {
            m_kbdContext.m_currentRecordMacro = m_KBD_TABLE[E_KEY.row][E_KEY.col];
            return 1;
        }
        if (m_kbdContext.m_kbdStatus[A_KEY.row][A_KEY.col])
        {
            m_kbdContext.m_currentRecordMacro = m_KBD_TABLE[A_KEY.row][A_KEY.col];
            return 1;
        }
    }

    return 0;
}

uint8_t extract_macro_key()
{
    for (uint8_t i = 0; i < 6; ++i)
    {
        for (uint8_t j = 0; j < 15; ++j)
        {
            if (m_kbdContext.m_kbdStatus[i][j])
            {
                return m_KBD_TABLE[i][j];
            }
        }
    }

    return 0;
}

bool is_fn_macro()
{
    if (m_kbdContext.m_kbdStatus[FN_KEY.row][FN_KEY.col])
    {
        return true;
    }

    return false;
}

void start_record_macro()
{
    if (is_create_macroo())
    {
        // if (m_kbdContext.m_connMode == WIRE_MODE)
        //  led_flash_start();
        uint8_t is_create = 0;
        uint8_t recordKey = m_kbdContext.m_currentRecordMacro;
        uint8_t alreadyInMacroMount = 0;
        put_macro_id(recordKey);
        m_kbdContext.m_currentRecordMacro = 0;

        do
        {
            kbd_scan();
            if (kbd_StatusIsChanged() == 0)
            {
                continue;
            }
            if (is_create_macroo() && (is_create > 0))
            {
                if (m_kbdContext.m_currentRecordMacro == recordKey)
                {
                    break;
                }
            }

            if (is_create_macroo() && is_create == 0)
            {
                continue;
            }

            // fn key end
            if (is_fn_macro() && is_create > 0)
            {
                break;
            }

            uint8_t tmp = extract_macro_key();
            if (tmp != 0)
            {
                put_macro_key(recordKey, tmp);
                nrf_delay_ms(100);
                ++alreadyInMacroMount;
                is_create = 1;
            }

            if (alreadyInMacroMount == 3)
            {
                break;
            }
        } while (1);
        reset_kbd_status();

        nrf_delay_ms(200);
        // if (m_kbdContext.m_connMode == WIRE_MODE)
        //  led_flash_stop();
    }
}

void kbd_loop()
{
    kbd_scan();
    kbd_fn_key_handle();
    start_record_macro();
}

void kbd_send_hid_report()
{
    if (m_kbdContext.m_connMode == WIRELESS_MODE)
    {
        nrf_gzll_add_packet_to_tx_fifo(0, (uint8_t *)&m_kbdContext, 8);
    }
    else
    {
        send_hid_data(m_kbdContext.m_usbHandle, (uint8_t *)&m_kbdContext);
    }
}

uint8_t kbd_composite_key_handle(uint8_t row, uint8_t col)
{
    /*if(m_kbdContext.m_kbdStatus[BOLANGXIAN_KEY.row][BOLANGXIAN_KEY.col] &&
     row == BOLANGXIAN_KEY.row && col == BOLANGXIAN_KEY.col)
    {

        if(m_kbdContext.m_kbdStatus[CAPS_KEY.row][CAPS_KEY.col])
        {
            uint8_t index= kbd_availableIndex();
            m_kbdContext.m_table[index]=KEY_CAPS;
            return MULTIPLE_COMPOSITE_KEY;
        }

    }
    //left
    if(m_kbdContext.m_kbdStatus[J_KEY.row][J_KEY.col] &&
    row == J_KEY.row && col == J_KEY.col)
    {
        if(m_kbdContext.m_kbdStatus[CAPS_KEY.row][CAPS_KEY.col])
        {
            uint8_t index= kbd_availableIndex();
            m_kbdContext.m_table[index]=KEY_LEFTARROW;
            return MULTIPLE_COMPOSITE_KEY;
        }

    }
    //down
    if(m_kbdContext.m_kbdStatus[K_KEY.row][K_KEY.col] &&
    row == K_KEY.row && col == K_KEY.col)
    {
        if(m_kbdContext.m_kbdStatus[CAPS_KEY.row][CAPS_KEY.col])
        {
            uint8_t index= kbd_availableIndex();
            m_kbdContext.m_table[index]=KEY_DOWNARROW;
            return MULTIPLE_COMPOSITE_KEY;
        }

    }
    //right
    if( m_kbdContext.m_kbdStatus[L_KEY.row][L_KEY.col] &&
    row== L_KEY.row&& col == L_KEY.col)
    {
        if(m_kbdContext.m_kbdStatus[CAPS_KEY.row][CAPS_KEY.col])
        {
            uint8_t index= kbd_availableIndex();
            m_kbdContext.m_table[index]=KEY_RIGHTARROW;
            return MULTIPLE_COMPOSITE_KEY;
        }

    }
    //up
    if(m_kbdContext.m_kbdStatus[I_KEY.row][I_KEY.col] &&
    row == I_KEY.row && col == I_KEY.col)
    {
        if(m_kbdContext.m_kbdStatus[CAPS_KEY.row][CAPS_KEY.col])
        {
            uint8_t index= kbd_availableIndex();
            m_kbdContext.m_table[index]=KEY_UPARROW;
            return MULTIPLE_COMPOSITE_KEY;
        }

    }

    //delete
    if(m_kbdContext.m_kbdStatus[BACKSPACE_KEY.row][BACKSPACE_KEY.col]&&
    row ==BACKSPACE_KEY.row && col == BACKSPACE_KEY.col)
    {
        if(m_kbdContext.m_kbdStatus[CAPS_KEY.row][CAPS_KEY.col])
        {
           uint8_t index= kbd_availableIndex();
            m_kbdContext.m_table[index]=KEY_DELETE;
            return MULTIPLE_COMPOSITE_KEY;
        }
    }*/

    return SINGLE_COMPOSITE_KEY;
}
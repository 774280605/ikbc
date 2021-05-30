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

typedef struct
{
    uint8_t row;
    uint8_t col;

} KEY_Point;

KEY_Point LCTRL_KEY = {3, 0};
KEY_Point LSHIFT_KEY = {4, 0};
KEY_Point LALT_KEY = {5, 2};

KEY_Point RCTRL_KEY = {5, 9};
KEY_Point RSHIFT_KEY = {4, 11};
KEY_Point RALT_KEY = {5, 6};
KEY_Point WIN_KEY = {5, 1};

KEY_Point FN_KEY = {5, 7};
KEY_Point LEFT_FN_KEY = {5, 0};

KEY_Point MENU_KEY = {5, 8};

KEY_Point UP_KEY = {4, 12};
KEY_Point LEFT_KEY = {5, 10};
KEY_Point DOWN_KEY = {5, 11};
KEY_Point RIGTH_KEY = {5, 12};

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

KEY_Point K_SUB_KEY={1,11};
KEY_Point K_PLUS_KEY={1,12};




KBD_Context_t m_kbdContext;

#define KEY_FN 0
unsigned char m_Matrix[6][17] = {
    {_ESC, _F1, _F2, _F3, _F4, _F5, _F6, _F7, _F8, _F9, _F10, _F11, _F12, 0, 0, 0, 0},
    {_TILDE, _1, _2, _3, _4, _5, _6, _7, _8, _9, _0, _UNDERSCORE, _PLUS, _BACKSPACE, _INSERT, _HOME, _PAGEUP},
    {_TAB, _Q, _W, _E, _R, _T, _Y, _U, _I, _O, _P, _OPEN_BRACKET, _CLOSE_BRACKET, _BACKSLASH, _DELETE, _END, _PAGEDOWN},
    {_LEFT_CTRL, _A, _S, _D, _F, _G, _H, _J, _K, _L, _COLON, _QUOTE, _ENTER, 0, 0, 0, 0},
    {_LEFT_SHIFT, _Z, _X, _C, _V, _B, _N, _M, _COMMA, _DOT, _SLASH, _RIGHT_SHIFT, _UP, 0, 0, 0, 0},
    {0, _LEFT_UI, _LEFT_ALT, _SPACEBAR, _CAPS_LOCK, _SPACEBAR, _RIGHT_ALT, KEY_FN, 0, _RIGHT_CTRL,
        _LEFT, _DOWN, _RIGHT, 0, 0, 0, 0}};

int m_Matrix_Row_Raw[6] = {
    NRF_GPIO_PIN_MAP(0, 10), // row0
    NRF_GPIO_PIN_MAP(1, 6),  // row1
    NRF_GPIO_PIN_MAP(0, 9),  // row2
    NRF_GPIO_PIN_MAP(1, 4),  // row3
    NRF_GPIO_PIN_MAP(0, 2),  // row4
    NRF_GPIO_PIN_MAP(1, 13), // row5

};

int m_Matrix_Col_Raw[13] = {
    NRF_GPIO_PIN_MAP(1, 0),  // col0
    NRF_GPIO_PIN_MAP(0, 24), // col1
    NRF_GPIO_PIN_MAP(0, 22), // col2
    NRF_GPIO_PIN_MAP(0, 13), // col3
    NRF_GPIO_PIN_MAP(0, 20), // col4
    NRF_GPIO_PIN_MAP(0, 15), // col5
    NRF_GPIO_PIN_MAP(0, 4),  // col6
    NRF_GPIO_PIN_MAP(1, 9),  // col7
    NRF_GPIO_PIN_MAP(0, 8),  // col8
    NRF_GPIO_PIN_MAP(0, 5),  // col9
    NRF_GPIO_PIN_MAP(0, 6),  // col10
    NRF_GPIO_PIN_MAP(0, 1),  // col11
    NRF_GPIO_PIN_MAP(0, 26), // col12
};

int m_Matrix_Col_Raw_2_3[17] = {
    NRF_GPIO_PIN_MAP(1, 0),  // col0
    NRF_GPIO_PIN_MAP(0, 24), // col1
    NRF_GPIO_PIN_MAP(0, 22), // col2
    NRF_GPIO_PIN_MAP(0, 13), // col3
    NRF_GPIO_PIN_MAP(0, 20), // col4
    NRF_GPIO_PIN_MAP(0, 15), // col5
    NRF_GPIO_PIN_MAP(0, 4),  // col6
    NRF_GPIO_PIN_MAP(1, 9),  // col7
    NRF_GPIO_PIN_MAP(0, 8),  // col8
    NRF_GPIO_PIN_MAP(0, 5),  // col9
    NRF_GPIO_PIN_MAP(0, 6),  // col10
    NRF_GPIO_PIN_MAP(0, 1),  // col11
    NRF_GPIO_PIN_MAP(0, 26), // col12
    NRF_GPIO_PIN_MAP(0, 0),  // col13
    NRF_GPIO_PIN_MAP(0, 30), // col14
    NRF_GPIO_PIN_MAP(0, 31), // col15
    NRF_GPIO_PIN_MAP(0, 29), // col16
};

uint32_t m_unuse_pin[5] = {
    NRF_GPIO_PIN_MAP(0, 3),
    NRF_GPIO_PIN_MAP(0, 12),
    NRF_GPIO_PIN_MAP(0, 28),
    NRF_GPIO_PIN_MAP(1, 10),
    NRF_GPIO_PIN_MAP(1, 11),
};

void reset_kbd_status()
{
    for (uint8_t i = 0; i < 6; ++i)
    {
        for (uint8_t j = 0; j < 17; ++j)
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
            if (m_kbdContext.m_connMode == WIRE_MODE)
                    ws2812_set_close();
            
             nrf_delay_ms(200);
            reset_kbd_status();
        }

        // 1
        if (m_kbdContext.m_kbdStatus[K_1_KEY.row][K_1_KEY.col])
        {
            if (m_kbdContext.m_connMode == WIRE_MODE)
            {
                ws2812_set_close();
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
                ws2812_color_blue();
                nrf_delay_ms(200);
            }
            reset_kbd_status();
        }
        if (m_kbdContext.m_kbdStatus[K_5_KEY.row][K_5_KEY.col])
        {
            if (m_kbdContext.m_connMode == WIRE_MODE)
            {
                set_custom_color_1();
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
                setup_pretty_led();
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

    /* for (uint8_t i = 0; i < 5; ++i) {
    nrf_gpio_cfg_input(m_unuse_pin[i], NRF_GPIO_PIN_PULLDOWN);
    nrf_delay_us(2);
  }*/

    kbd_gpio_init();

    led_flash_init();

    // low_energy_timer_init();
    // kbd_setup_low_energy_tiemr();
    // gpiote_reg_init();
    // kbd_gpiote_enable();
}

void kbd_gpio_init()
{
    for (int i = 0; i < 17; ++i)
    {
        nrf_gpio_cfg_input(m_Matrix_Col_Raw_2_3[i], NRF_GPIO_PIN_PULLDOWN);
    }

    // end test
    for (int i = 0; i < 6; ++i)
    {
        nrf_gpio_cfg_input(m_Matrix_Row_Raw[i], NRF_GPIO_PIN_PULLDOWN);
        nrf_delay_ms(1);
    }
}

void kbd_scan()
{
    for (int i = 0; i < 17; ++i)
    {
        nrf_gpio_cfg_input(m_Matrix_Col_Raw_2_3[i], NRF_GPIO_PIN_PULLDOWN);
        // nrf_gpio_pin_clear(m_col[i]);
    }

    // 0 row 13 col
    for (int i = 0; i < 13; ++i)
    {
        nrf_gpio_cfg_output(m_Matrix_Col_Raw[i]);
        nrf_delay_us(40);
        nrf_gpio_pin_set(m_Matrix_Col_Raw[i]);
        nrf_delay_us(40);
        if (nrf_gpio_pin_read(m_Matrix_Row_Raw[0]))
        {
            m_kbdContext.m_kbdStatus[0][i] = 1;
        }
        else
        {
            m_kbdContext.m_kbdStatus[0][i] = 0;
        }
        nrf_gpio_pin_clear(m_Matrix_Col_Raw[i]);
        nrf_delay_us(10);
        nrf_gpio_cfg_input(m_Matrix_Col_Raw[i], NRF_GPIO_PIN_PULLDOWN);
        nrf_delay_us(10);
    }

    // 1 row
    for (int i = 0; i < 17; ++i)
    {
        nrf_gpio_cfg_output(m_Matrix_Col_Raw_2_3[i]);
        nrf_delay_us(40);
        nrf_gpio_pin_set(m_Matrix_Col_Raw_2_3[i]);
        nrf_delay_us(40);
        if (nrf_gpio_pin_read(m_Matrix_Row_Raw[1]))
        {
            m_kbdContext.m_kbdStatus[1][i] = 1;
        }
        else
        {
            m_kbdContext.m_kbdStatus[1][i] = 0;
        }
        nrf_gpio_pin_clear(m_Matrix_Col_Raw_2_3[i]);
        nrf_delay_us(10);
        nrf_gpio_cfg_input(m_Matrix_Col_Raw_2_3[i], NRF_GPIO_PIN_PULLDOWN);
        nrf_delay_us(10);
    }

    // 2 row
    for (int i = 0; i < 17; ++i)
    {
        nrf_gpio_cfg_output(m_Matrix_Col_Raw_2_3[i]);
        nrf_delay_us(40);
        nrf_gpio_pin_set(m_Matrix_Col_Raw_2_3[i]);
        nrf_delay_us(40);
        if (nrf_gpio_pin_read(m_Matrix_Row_Raw[2]))
        {
            m_kbdContext.m_kbdStatus[2][i] = 1;
        }
        else
        {
            m_kbdContext.m_kbdStatus[2][i] = 0;
        }
        nrf_gpio_pin_clear(m_Matrix_Col_Raw_2_3[i]);
        nrf_delay_us(10);
        nrf_gpio_cfg_input(m_Matrix_Col_Raw_2_3[i], NRF_GPIO_PIN_PULLDOWN);
        nrf_delay_us(10);
    }

    // 3 row
    for (int i = 0; i < 13; ++i)
    {
        nrf_gpio_cfg_output(m_Matrix_Col_Raw[i]);
        nrf_delay_us(40);
        nrf_gpio_pin_set(m_Matrix_Col_Raw[i]);
        nrf_delay_us(40);
        if (nrf_gpio_pin_read(m_Matrix_Row_Raw[3]))
        {
            m_kbdContext.m_kbdStatus[3][i] = 1;
        }
        else
        {
            m_kbdContext.m_kbdStatus[3][i] = 0;
        }
        nrf_gpio_pin_clear(m_Matrix_Col_Raw[i]);
        nrf_delay_us(10);
        nrf_gpio_cfg_input(m_Matrix_Col_Raw[i], NRF_GPIO_PIN_PULLDOWN);
        nrf_delay_us(10);
    }

    // 4 row
    for (int i = 0; i < 13; ++i)
    {
        nrf_gpio_cfg_output(m_Matrix_Col_Raw[i]);
        nrf_delay_us(40);
        nrf_gpio_pin_set(m_Matrix_Col_Raw[i]);

        nrf_delay_us(40);
        if (nrf_gpio_pin_read(m_Matrix_Row_Raw[4]))
        {
            m_kbdContext.m_kbdStatus[4][i] = 1;
        }
        else
        {
            m_kbdContext.m_kbdStatus[4][i] = 0;
        }
        nrf_gpio_pin_clear(m_Matrix_Col_Raw[i]);
        nrf_delay_us(10);
        nrf_gpio_cfg_input(m_Matrix_Col_Raw[i], NRF_GPIO_PIN_PULLDOWN);
        nrf_delay_us(10);
    }

    // 5 row
    for (int i = 0; i < 13; ++i)
    {
        nrf_gpio_cfg_output(m_Matrix_Col_Raw[i]);
        nrf_delay_us(40);
        nrf_gpio_pin_set(m_Matrix_Col_Raw[i]);
        nrf_delay_us(40);
        if (nrf_gpio_pin_read(m_Matrix_Row_Raw[5]))
        {
            m_kbdContext.m_kbdStatus[5][i] = 1;
        }
        else
        {
            m_kbdContext.m_kbdStatus[5][i] = 0;
        }
        nrf_gpio_pin_clear(m_Matrix_Col_Raw[i]);
        nrf_delay_us(10);
        nrf_gpio_cfg_input(m_Matrix_Col_Raw[i], NRF_GPIO_PIN_PULLDOWN);
        nrf_delay_us(10);
    }
}

void kbd_saveLastStatus()
{
    for (int row = 0; row < 6; ++row)
    {
        for (int col = 0; col < 17; ++col)
        {
            m_kbdContext.m_kbdLastTimeStatus[row][col] = m_kbdContext.m_kbdStatus[row][col];
        }
    }
}

bool kbd_status_is_changed()
{
    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 17; ++j)
        {
            if (m_kbdContext.m_kbdLastTimeStatus[i][j] != m_kbdContext.m_kbdStatus[i][j])
            {
                kbd_saveLastStatus();

                return true;
            }
        }
    }

    return false;
}

void kbd_update_hid_data()
{
    memset(m_kbdContext.m_table, 0, 6);
    m_kbdContext.m_modifier = _NONE;

    for (int i = 0; i < 6; ++i)
    {
        for (int j = 0; j < 17; ++j)
        {
            if (m_kbdContext.m_kbdStatus[i][j])
            {
                if (kbd_modifier(i, j) == false) // common key
                {
                    if (is_macro_key(m_Matrix[i][j]))
                    {
                        put_macro_key_to_pkt(m_Matrix[i][j]);
                    }
                    else if (kbd_composite_key_handle(i, j))
                    {
                    }
                    else
                    {
                        uint8_t key = m_Matrix[i][j];
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

bool kbd_modifier(uint8_t row, uint8_t col)
{
    if (LSHIFT_KEY.row == row && LSHIFT_KEY.col == col)
    {
        m_kbdContext.m_modifier |= _LEFT_SHIFT;
    }
    else if (RSHIFT_KEY.row == row && RSHIFT_KEY.col == col)
    {
        m_kbdContext.m_modifier |= _RIGHT_SHIFT;
    }
    else if (LCTRL_KEY.row == row && LCTRL_KEY.col == col)
    {
        m_kbdContext.m_modifier |= _LEFT_CTRL;
    }
    else if (RCTRL_KEY.row == row && RCTRL_KEY.col == col)
    {
        m_kbdContext.m_modifier |= _RIGHT_CTRL;
    }
    else if (LALT_KEY.row == row && LALT_KEY.col == col)
    {
        m_kbdContext.m_modifier |= _LEFT_ALT;
    }
    else if (RALT_KEY.row == row && RALT_KEY.col == col)
    {
        m_kbdContext.m_modifier |= _RIGHT_ALT;
    }
    else if (WIN_KEY.row == row && WIN_KEY.col == col)
    {
        m_kbdContext.m_modifier |= _LEFT_UI;
    }
    else
    {
        return false;
    }
    return true;
}

//
uint8_t is_create_macro()
{
    if (m_kbdContext.m_kbdStatus[FN_KEY.row][FN_KEY.col])
    {
        if (m_kbdContext.m_kbdStatus[Q_KEY.row][Q_KEY.col])
        {
            m_kbdContext.m_currentRecordMacro = m_Matrix[Q_KEY.row][Q_KEY.col];
            return 1;
        }
        if (m_kbdContext.m_kbdStatus[W_KEY.row][W_KEY.col])
        {
            m_kbdContext.m_currentRecordMacro = m_Matrix[W_KEY.row][W_KEY.col];
            return 1;
        }
        if (m_kbdContext.m_kbdStatus[E_KEY.row][E_KEY.col])
        {
            m_kbdContext.m_currentRecordMacro = m_Matrix[E_KEY.row][E_KEY.col];
            return 1;
        }
        if (m_kbdContext.m_kbdStatus[A_KEY.row][A_KEY.col])
        {
            m_kbdContext.m_currentRecordMacro = m_Matrix[A_KEY.row][A_KEY.col];
            return 1;
        }
    }

    return 0;
}

uint8_t extract_macro_key()
{
    for (uint8_t i = 0; i < 6; ++i)
    {
        for (uint8_t j = 0; j < 17; ++j)
        {
            if (m_kbdContext.m_kbdStatus[i][j])
            {
                return m_Matrix[i][j];
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
    if (is_create_macro())
    {
        if (m_kbdContext.m_connMode == WIRE_MODE)
             ws2812_macro_mode_setup();

        uint8_t is_create = 0;
        uint8_t recordKey = m_kbdContext.m_currentRecordMacro;
        uint8_t alreadyInMacroMount = 0;
        put_macro_id(recordKey);
        m_kbdContext.m_currentRecordMacro = 0;

        do
        {
            kbd_scan();
            if (kbd_status_is_changed() == false)
            {
                continue;
            }
            if (is_create_macro() && (is_create > 0))
            {
                if (m_kbdContext.m_currentRecordMacro == recordKey)
                {
                    break;
                }
            }

            if (is_create_macro() && is_create == 0)
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
                ws2812_macro_put_point(tmp);
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
         //if (m_kbdContext.m_connMode == WIRE_MODE)
         //   ws2812_macro_timer_setup();
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
   /* if(m_kbdContext.m_kbdStatus[BOLANGXIAN_KEY.row][BOLANGXIAN_KEY.col] &&
     row == BOLANGXIAN_KEY.row && col == BOLANGXIAN_KEY.col)
    {

        if(m_kbdContext.m_kbdStatus[CAPS_KEY.row][CAPS_KEY.col])
        {
            uint8_t index= kbd_availableIndex();
            m_kbdContext.m_table[index]=KEY_CAPS;
            return MULTIPLE_COMPOSITE_KEY;
        }
       
    }*/
    //left
    if(m_kbdContext.m_kbdStatus[J_KEY.row][J_KEY.col] &&
    row == J_KEY.row && col == J_KEY.col)
    {
        if(m_kbdContext.m_kbdStatus[LEFT_FN_KEY.row][LEFT_FN_KEY.col])
        {
            uint8_t index= kbd_availableIndex();
            m_kbdContext.m_table[index]=_LEFT;
            return MULTIPLE_COMPOSITE_KEY;
        }
       
    }
    //down
    if(m_kbdContext.m_kbdStatus[K_KEY.row][K_KEY.col] &&
    row == K_KEY.row && col == K_KEY.col)
    {
        if(m_kbdContext.m_kbdStatus[LEFT_FN_KEY.row][LEFT_FN_KEY.col])
        {
            uint8_t index= kbd_availableIndex();
            m_kbdContext.m_table[index]=_DOWN;
            return MULTIPLE_COMPOSITE_KEY;
        }
       
    }
    //right
    if( m_kbdContext.m_kbdStatus[L_KEY.row][L_KEY.col] &&
    row== L_KEY.row&& col == L_KEY.col)
    {
        if(m_kbdContext.m_kbdStatus[LEFT_FN_KEY.row][LEFT_FN_KEY.col])
        {
            uint8_t index= kbd_availableIndex();
            m_kbdContext.m_table[index]=_RIGHT;
            return MULTIPLE_COMPOSITE_KEY;
        }
        
    }
    //up
    if(m_kbdContext.m_kbdStatus[I_KEY.row][I_KEY.col] &&
    row == I_KEY.row && col == I_KEY.col)
    {
        if(m_kbdContext.m_kbdStatus[LEFT_FN_KEY.row][LEFT_FN_KEY.col])
        {
            uint8_t index= kbd_availableIndex();
            m_kbdContext.m_table[index]=_UP;
            return MULTIPLE_COMPOSITE_KEY;
        }
        
    }

    //delete
    if(m_kbdContext.m_kbdStatus[BACKSPACE_KEY.row][BACKSPACE_KEY.col]&&
    row ==BACKSPACE_KEY.row && col == BACKSPACE_KEY.col)
    {
        if(m_kbdContext.m_kbdStatus[LEFT_FN_KEY.row][LEFT_FN_KEY.col])
        {
           uint8_t index= kbd_availableIndex();
            m_kbdContext.m_table[index]=_DELETE;
            return MULTIPLE_COMPOSITE_KEY;
        }
    }
    //volume down
     if(m_kbdContext.m_kbdStatus[K_SUB_KEY.row][K_SUB_KEY.col]&&
    row ==K_SUB_KEY.row && col == K_SUB_KEY.col)
    {
        if(m_kbdContext.m_kbdStatus[LEFT_FN_KEY.row][LEFT_FN_KEY.col])
        {
           uint8_t index= kbd_availableIndex();
            m_kbdContext.m_table[index]=_VOLUME_DOWN;
            return MULTIPLE_COMPOSITE_KEY;
        }
    }
    
     //volume up
     if(m_kbdContext.m_kbdStatus[K_PLUS_KEY.row][K_PLUS_KEY.col]&&
    row ==K_PLUS_KEY.row && col == K_PLUS_KEY.col)
    {
        if(m_kbdContext.m_kbdStatus[LEFT_FN_KEY.row][LEFT_FN_KEY.col])
        {
           uint8_t index= kbd_availableIndex();
            m_kbdContext.m_table[index]=_VOLUME_UP;
            return MULTIPLE_COMPOSITE_KEY;
        }
    }
    return SINGLE_COMPOSITE_KEY;
}
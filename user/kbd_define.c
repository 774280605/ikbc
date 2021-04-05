/*
这是RGB键盘红色的PCB哪款





*/

#include "kbd_define.h"
#include "SEGGER_RTT.h"
#include "nrf_gpio.h"
#include "kbd.h"

uint8_t m_hidData[8] = {0};

uint8_t m_kbdTable[6][15] = {0};
uint8_t m_lastTable[6][15] = {0};
uint8_t m_modifier = 0;
uint8_t m_mode = WIRELESS_MODE;

const app_usbd_hid_kbd_t * m_usbHandle;


uint8_t m_macro_key;


uint8_t m_per_row_col[6] = {15, 15, 15, 15, 13, 11};


uint32_t m_row[6] = {
    NRF_GPIO_PIN_MAP(1, 11), // row0
    NRF_GPIO_PIN_MAP(1, 10), // row1
    NRF_GPIO_PIN_MAP(0, 3),  // row2
    NRF_GPIO_PIN_MAP(0, 28), // row3
    NRF_GPIO_PIN_MAP(1, 13), // row4
    NRF_GPIO_PIN_MAP(0, 2)};// row5


uint32_t m_col[15] = {
    NRF_GPIO_PIN_MAP(0, 29), // 0
    NRF_GPIO_PIN_MAP(0, 31),  // 1
    NRF_GPIO_PIN_MAP(0, 30),  // 2
    NRF_GPIO_PIN_MAP(0, 0),  // 3w
    NRF_GPIO_PIN_MAP(0, 26),  // 4
    NRF_GPIO_PIN_MAP(0, 1),  // 5
    NRF_GPIO_PIN_MAP(0, 6), // 6
    NRF_GPIO_PIN_MAP(0, 5), // 7
    NRF_GPIO_PIN_MAP(0, 8), // 8
    NRF_GPIO_PIN_MAP(1, 9), // 9
    NRF_GPIO_PIN_MAP(0, 4), // 10
    NRF_GPIO_PIN_MAP(0, 12), // 11
    NRF_GPIO_PIN_MAP(0, 7), // 12
    NRF_GPIO_PIN_MAP(0, 15), // 13
    NRF_GPIO_PIN_MAP(0, 17),  // 14
};


uint32_t m_row_col_0[15] = {
    NRF_GPIO_PIN_MAP(0, 29), // 0
    NRF_GPIO_PIN_MAP(0, 31),  // 1
    NRF_GPIO_PIN_MAP(0, 30),  // 2
    NRF_GPIO_PIN_MAP(0, 0),  // 3
    NRF_GPIO_PIN_MAP(0, 26),  // 4
    NRF_GPIO_PIN_MAP(0, 1),  // 5
    NRF_GPIO_PIN_MAP(0, 6), // 6
    NRF_GPIO_PIN_MAP(0, 5), // 7
    NRF_GPIO_PIN_MAP(0, 8), // 8
    NRF_GPIO_PIN_MAP(1, 9), // 9
    NRF_GPIO_PIN_MAP(0, 4), // 10
    NRF_GPIO_PIN_MAP(0, 12), // 11
    NRF_GPIO_PIN_MAP(0, 7), // 12
    NRF_GPIO_PIN_MAP(0, 15), // 13
    NRF_GPIO_PIN_MAP(0, 17),  // 14
};


uint32_t m_row_col_1[15] = {
   NRF_GPIO_PIN_MAP(0, 29), // 0
    NRF_GPIO_PIN_MAP(0, 31),  // 1
    NRF_GPIO_PIN_MAP(0, 30),  // 2
    NRF_GPIO_PIN_MAP(0, 0),  // 3
    NRF_GPIO_PIN_MAP(0, 26),  // 4
    NRF_GPIO_PIN_MAP(0, 1),  // 5
    NRF_GPIO_PIN_MAP(0, 6), // 6
    NRF_GPIO_PIN_MAP(0, 5), // 7
    NRF_GPIO_PIN_MAP(0, 8), // 8
    NRF_GPIO_PIN_MAP(1, 9), // 9
    NRF_GPIO_PIN_MAP(0, 4), // 10
    NRF_GPIO_PIN_MAP(0, 12), // 11
    NRF_GPIO_PIN_MAP(0, 7), // 12
    NRF_GPIO_PIN_MAP(0, 15), // 13
    NRF_GPIO_PIN_MAP(0, 17),  // 14

};
uint32_t m_row_col_2[15] = {
    NRF_GPIO_PIN_MAP(0, 29), // 0
    NRF_GPIO_PIN_MAP(0, 31),  // 1
    NRF_GPIO_PIN_MAP(0, 30),  // 2
    NRF_GPIO_PIN_MAP(0, 0),  // 3
    NRF_GPIO_PIN_MAP(0, 26),  // 4
    NRF_GPIO_PIN_MAP(0, 1),  // 5
    NRF_GPIO_PIN_MAP(0, 6), // 6
    NRF_GPIO_PIN_MAP(0, 5), // 7
    NRF_GPIO_PIN_MAP(0, 8), // 8
    NRF_GPIO_PIN_MAP(1, 9), // 9
    NRF_GPIO_PIN_MAP(0, 4), // 10
    NRF_GPIO_PIN_MAP(0, 12), // 11
    NRF_GPIO_PIN_MAP(0, 7), // 12
    NRF_GPIO_PIN_MAP(0, 15), // 13
    NRF_GPIO_PIN_MAP(0, 17),  // 14

};
uint32_t m_row_col_3[15] = {
    NRF_GPIO_PIN_MAP(0, 29), // 0
    NRF_GPIO_PIN_MAP(0, 31),  // 1
    NRF_GPIO_PIN_MAP(0, 30),  // 2
    NRF_GPIO_PIN_MAP(0, 0),  // 3
    NRF_GPIO_PIN_MAP(0, 26),  // 4
    NRF_GPIO_PIN_MAP(0, 1),  // 5
    NRF_GPIO_PIN_MAP(0, 6), // 6
    NRF_GPIO_PIN_MAP(0, 5), // 7
    NRF_GPIO_PIN_MAP(0, 8), // 8
    NRF_GPIO_PIN_MAP(1, 9), // 9
    NRF_GPIO_PIN_MAP(0, 4), // 10
    NRF_GPIO_PIN_MAP(0, 12), // 11
    NRF_GPIO_PIN_MAP(0, 7), // 12
    NRF_GPIO_PIN_MAP(0, 15), // 13
    NRF_GPIO_PIN_MAP(0, 17),  // 14

};

uint32_t m_row_col_4[13] = {
   NRF_GPIO_PIN_MAP(0, 29), // 0
    NRF_GPIO_PIN_MAP(0, 31),  // 1
    NRF_GPIO_PIN_MAP(0, 30),  // 2
    NRF_GPIO_PIN_MAP(0, 0),  // 3
    NRF_GPIO_PIN_MAP(0, 26),  // 4
    NRF_GPIO_PIN_MAP(0, 1),  // 5
    NRF_GPIO_PIN_MAP(0, 6), // 6
    NRF_GPIO_PIN_MAP(0, 5), // 7
    NRF_GPIO_PIN_MAP(0, 8), // 8
    NRF_GPIO_PIN_MAP(1, 9), // 9
    NRF_GPIO_PIN_MAP(0, 4), // 10
    NRF_GPIO_PIN_MAP(0, 12), // 11
    NRF_GPIO_PIN_MAP(0, 7), // 12
   
};


uint32_t m_row_col_5[11] = {
    NRF_GPIO_PIN_MAP(0, 29), // 0
    NRF_GPIO_PIN_MAP(0, 31),  // 1
    NRF_GPIO_PIN_MAP(0, 30),  // 2
    NRF_GPIO_PIN_MAP(0, 0),  // 3
    NRF_GPIO_PIN_MAP(0, 26),  // 4
    NRF_GPIO_PIN_MAP(0, 1),  // 5
    NRF_GPIO_PIN_MAP(0, 6), // 6
    NRF_GPIO_PIN_MAP(0, 5), // 7
    NRF_GPIO_PIN_MAP(0, 8), // 8
    NRF_GPIO_PIN_MAP(1, 9), // 9
    NRF_GPIO_PIN_MAP(0, 4), // 10

};


unsigned char m_KBD_TABLE[6][15] = {
    {KEY_ESC, KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9, KEY_F10,KEY_F11, KEY_F12, 
              KEY_PAGEUP, KEY_HOME},
    {KEY_TILDE, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9, KEY_0, KEY_SUB,KEY_ADD, KEY_BACKSPACE, 
              KEY_INSERT},
    {KEY_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P,KEY_LEFT_BRACKET, KEY_RIGHT_BRACKET, 
              KEY_SLASH,KEY_DELETE},
    {KEY_L_CTRL, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_SEM, KEY_YINHAO,KEY_RETURN, 
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
KEY_Point CAPS_KEY={5,0};
KEY_Point BOLANGXIAN_KEY={1,0};
KEY_Point J_KEY=  {3,7};
KEY_Point K_KEY=  {3,8};
KEY_Point L_KEY=  {3,9};
KEY_Point I_KEY=  {2,8};
KEY_Point BACKSPACE_KEY={1,13};


KEY_Point K_1_KEY={1,1};
KEY_Point K_2_KEY={1,2};
KEY_Point K_3_KEY={1,3};
KEY_Point K_4_KEY={1,4};
KEY_Point K_5_KEY={1,5};
KEY_Point K_6_KEY={1,6};
KEY_Point K_7_KEY={1,7};
KEY_Point K_8_KEY={1,8};
KEY_Point K_9_KEY={1,9};
KEY_Point K_0_KEY={1,10};


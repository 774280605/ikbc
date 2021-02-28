#ifndef __USER_KBD__
#define __USER_KBD__

#include "kbd_define.h"
#include "nrf_gpio.h"
#include "app_usbd_hid_kbd.h"

#include <stdint.h>
#include <stdbool.h>

typedef enum {
    KBD_A               = 4,  /**<KBD_A               code*/
    KBD_B               = 5,  /**<KBD_B               code*/
    KBD_C               = 6,  /**<KBD_C               code*/
    KBD_D               = 7,  /**<KBD_D               code*/
    KBD_E               = 8,  /**<KBD_E               code*/
    KBD_F               = 9,  /**<KBD_F               code*/
    KBD_G               = 10, /**<KBD_G               code*/
    KBD_H               = 11, /**<KBD_H               code*/
    KBD_I               = 12, /**<KBD_I               code*/
    KBD_J               = 13, /**<KBD_J               code*/
    KBD_K               = 14, /**<KBD_K               code*/
    KBD_L               = 15, /**<KBD_L               code*/
    KBD_M               = 16, /**<KBD_M               code*/
    KBD_N               = 17, /**<KBD_N               code*/
    KBD_O               = 18, /**<KBD_O               code*/
    KBD_P               = 19, /**<KBD_P               code*/
    KBD_Q               = 20, /**<KBD_Q               code*/
    KBD_R               = 21, /**<KBD_R               code*/
    KBD_S               = 22, /**<KBD_S               code*/
    KBD_T               = 23, /**<KBD_T               code*/
    KBD_U               = 24, /**<KBD_U               code*/
    KBD_V               = 25, /**<KBD_V               code*/
    KBD_W               = 26, /**<KBD_W               code*/
    KBD_X               = 27, /**<KBD_X               code*/
    KBD_Y               = 28, /**<KBD_Y               code*/
    KBD_Z               = 29, /**<KBD_Z               code*/
    KBD_1               = 30, /**<KBD_1               code*/
    KBD_2               = 31, /**<KBD_2               code*/
    KBD_3               = 32, /**<KBD_3               code*/
    KBD_4               = 33, /**<KBD_4               code*/
    KBD_5               = 34, /**<KBD_5               code*/
    KBD_6               = 35, /**<KBD_6               code*/
    KBD_7               = 36, /**<KBD_7               code*/
    KBD_8               = 37, /**<KBD_8               code*/
    KBD_9               = 38, /**<KBD_9               code*/
    KBD_0               = 39, /**<KBD_0               code*/
    KBD_ENTER           = 40, /**<KBD_ENTER           code*/
    KBD_ESCAPE          = 41, /**<KBD_ESCAPE          code*/
    KBD_BACKSPACE       = 42, /**<KBD_BACKSPACE       code*/
    KBD_TAB             = 43, /**<KBD_TAB             code*/
    KBD_SPACEBAR        = 44, /**<KBD_SPACEBAR        code*/
    KBD_UNDERSCORE      = 45, /**<KBD_UNDERSCORE      code*/
    KBD_PLUS            = 46, /**<KBD_PLUS            code*/
    KBD_OPEN_BRACKET    = 47, /**<KBD_OPEN_BRACKET    code*/
    KBD_CLOSE_BRACKET   = 48, /**<KBD_CLOSE_BRACKET   code*/
    KBD_BACKSLASH       = 49, /**<KBD_BACKSLASH       code*/
    KBD_ASH             = 50, /**<KBD_ASH             code*/
    KBD_COLON           = 51, /**<KBD_COLON           code*/
    KBD_QUOTE           = 52, /**<KBD_QUOTE           code*/
    KBD_TILDE           = 53, /**<KBD_TILDE           code*/
    KBD_COMMA           = 54, /**<KBD_COMMA           code*/
    KBD_DOT             = 55, /**<KBD_DOT             code*/
    KBD_SLASH           = 56, /**<KBD_SLASH           code*/
    KBD_CAPS_LOCK       = 57, /**<KBD_CAPS_LOCK       code*/
    KBD_F1              = 58, /**<KBD_F1              code*/
    KBD_F2              = 59, /**<KBD_F2              code*/
    KBD_F3              = 60, /**<KBD_F3              code*/
    KBD_F4              = 61, /**<KBD_F4              code*/
    KBD_F5              = 62, /**<KBD_F5              code*/
    KBD_F6              = 63, /**<KBD_F6              code*/
    KBD_F7              = 64, /**<KBD_F7              code*/
    KBD_F8              = 65, /**<KBD_F8              code*/
    KBD_F9              = 66, /**<KBD_F9              code*/
    KBD_F10             = 67, /**<KBD_F10             code*/
    KBD_F11             = 68, /**<KBD_F11             code*/
    KBD_F12             = 69, /**<KBD_F12             code*/
    KBD_PRINTSCREEN     = 70, /**<KBD_PRINTSCREEN     code*/
    KBD_SCROLL_LOCK     = 71, /**<KBD_SCROLL_LOCK     code*/
    KBD_PAUSE           = 72, /**<KBD_PAUSE           code*/
    KBD_INSERT          = 73, /**<KBD_INSERT          code*/
    KBD_HOME            = 74, /**<KBD_HOME            code*/
    KBD_PAGEUP          = 75, /**<KBD_PAGEUP          code*/
    KBD_DELETE          = 76, /**<KBD_DELETE          code*/
    KBD_END             = 77, /**<KBD_END             code*/
    KBD_PAGEDOWN        = 78, /**<KBD_PAGEDOWN        code*/
    KBD_RIGHT           = 79, /**<KBD_RIGHT           code*/
    KBD_LEFT            = 80, /**<KBD_LEFT            code*/
    KBD_DOWN            = 81, /**<KBD_DOWN            code*/
    KBD_UP              = 82, /**<KBD_UP              code*/
    KBD_KEYPAD_NUM_LOCK = 83, /**<KBD_KEYPAD_NUM_LOCK code*/
    KBD_KEYPAD_DIVIDE   = 84, /**<KBD_KEYPAD_DIVIDE   code*/
    KBD_KEYPAD_AT       = 85, /**<KBD_KEYPAD_AT       code*/
    KBD_KEYPAD_MULTIPLY = 85, /**<KBD_KEYPAD_MULTIPLY code*/
    KBD_KEYPAD_MINUS    = 86, /**<KBD_KEYPAD_MINUS    code*/
    KBD_KEYPAD_PLUS     = 87, /**<KBD_KEYPAD_PLUS     code*/
    KBD_KEYPAD_ENTER    = 88, /**<KBD_KEYPAD_ENTER    code*/
    KBD_KEYPAD_1        = 89, /**<KBD_KEYPAD_1        code*/
    KBD_KEYPAD_2        = 90, /**<KBD_KEYPAD_2        code*/
    KBD_KEYPAD_3        = 91, /**<KBD_KEYPAD_3        code*/
    KBD_KEYPAD_4        = 92, /**<KBD_KEYPAD_4        code*/
    KBD_KEYPAD_5        = 93, /**<KBD_KEYPAD_5        code*/
    KBD_KEYPAD_6        = 94, /**<KBD_KEYPAD_6        code*/
    KBD_KEYPAD_7        = 95, /**<KBD_KEYPAD_7        code*/
    KBD_KEYPAD_8        = 96, /**<KBD_KEYPAD_8        code*/
    KBD_KEYPAD_9        = 97, /**<KBD_KEYPAD_9        code*/
    KBD_KEYPAD_0        = 98, /**<KBD_KEYPAD_0        code*/
}kbd_codes_t;


enum{
  KBD_MODIFIER_NONE	      =0x00,
  KBD_MODIFIER_LEFT_CTRL      =0x01,
  KBD_MODIFIER_LEFT_SHIFT     =0x02,
  KBD_MODIFIER_LEFT_ALT	      =0x04,
  KBD_MODIFIER_LEFT_UI	      =0x08,
  KBD_MODIFIER_RIGHT_CTRL     =0x10,
  KBD_MODIFIER_RIGHT_SHIFT    =0x20,
  KBD_MODIFIER_RIGHT_ALT      =0x40,
  KBD_MODIFIER_RIGTH_UI	      =0x80,
};


enum
{
    SINGLE_COMPOSITE_KEY      =0,
    MULTIPLE_COMPOSITE_KEY    =1,
    
};

enum{
  ENERGY_SLEEP,
  ENERGY_NORMAL,

};



typedef struct
{
    uint8_t m_modifier;
    uint8_t m_reserve;
    uint8_t m_table[6];


    uint8_t m_kbdStatus[6][15];
    uint8_t m_kbdLastTimeStatus[6][15];
    uint8_t m_currentRecordMacro;
    uint8_t m_connMode;
    const app_usbd_hid_kbd_t * m_usbHandle;
    uint8_t m_energyMode;
    bool m_spi_inited;
    uint8_t m_pipe;


} KBD_Context_t;



extern KBD_Context_t m_kbdContext;

extern bool m_lowEnergyTimeout;


uint8_t kbd_availableIndex();

void kbd_saveLastStatus();


//逻辑
// 1.初始化
void kbd_init();

void kbd_gpio_init();


// 2.扫描
void kbd_scan();
// 3.键盘状态是否改变
uint8_t kbd_StatusIsChanged();
// 4.update hid data
void kbd_updateHidData();

uint8_t kbd_modifier(uint8_t row, uint8_t col);

void kbd_fn_key_handle();

void start_record_macro();
//扫描循环
void kbd_loop();
//发送报文
void kbd_send_hid_report();
//组合键
uint8_t kbd_composite_key_handle(uint8_t row,uint8_t col);

void kbd_capslock_on();
void kbd_capslock_off();


void send_hid_data(app_usbd_hid_kbd_t const * p_kbd,uint8_t* buf);






#endif
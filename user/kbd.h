#ifndef __USER_KBD__
#define __USER_KBD__

#include "app_usbd_hid_kbd.h"

#include "nrf_gpio.h"

#include <stdbool.h>
#include <stdint.h>

typedef enum {
  _A               = 4,  /**<KBD_A               code*/
  _B               = 5,  /**<KBD_B               code*/
  _C               = 6,  /**<KBD_C               code*/
  _D               = 7,  /**<KBD_D               code*/
  _E               = 8,  /**<KBD_E               code*/
  _F               = 9,  /**<KBD_F               code*/
  _G               = 10, /**<KBD_G               code*/
  _H               = 11, /**<KBD_H               code*/
  _I               = 12, /**<KBD_I               code*/
  _J               = 13, /**<KBD_J               code*/
  _K               = 14, /**<KBD_K               code*/
  _L               = 15, /**<KBD_L               code*/
  _M               = 16, /**<KBD_M               code*/
  _N               = 17, /**<KBD_N               code*/
  _O               = 18, /**<KBD_O               code*/
  _P               = 19, /**<KBD_P               code*/
  _Q               = 20, /**<KBD_Q               code*/
  _R               = 21, /**<KBD_R               code*/
  _S               = 22, /**<KBD_S               code*/
  _T               = 23, /**<KBD_T               code*/
  _U               = 24, /**<KBD_U               code*/
  _V               = 25, /**<KBD_V               code*/
  _W               = 26, /**<KBD_W               code*/
  _X               = 27, /**<KBD_X               code*/
  _Y               = 28, /**<KBD_Y               code*/
  _Z               = 29, /**<KBD_Z               code*/
  _1               = 30, /**<KBD_1               code*/
  _2               = 31, /**<KBD_2               code*/
  _3               = 32, /**<KBD_3               code*/
  _4               = 33, /**<KBD_4               code*/
  _5               = 34, /**<KBD_5               code*/
  _6               = 35, /**<KBD_6               code*/
  _7               = 36, /**<KBD_7               code*/
  _8               = 37, /**<KBD_8               code*/
  _9               = 38, /**<KBD_9               code*/
  _0               = 39, /**<KBD_0               code*/
  _ENTER           = 40, /**<KBD_ENTER           code*/
  _ESC             = 41, /**<KBD_ESCAPE          code*/
  _BACKSPACE       = 42, /**<KBD_BACKSPACE       code*/
  _TAB             = 43, /**<KBD_TAB             code*/
  _SPACEBAR        = 44, /**<KBD_SPACEBAR        code*/
  _UNDERSCORE      = 45, /**<KBD_UNDERSCORE      code*/
  _PLUS            = 46, /**<KBD_PLUS            code*/
  _OPEN_BRACKET    = 47, /**<KBD_OPEN_BRACKET    code*/
  _CLOSE_BRACKET   = 48, /**<KBD_CLOSE_BRACKET   code*/
  _BACKSLASH       = 49, /**<KBD_BACKSLASH       code*/
  _ASH             = 50, /**<KBD_ASH             code*/
  _COLON           = 51, /**<KBD_COLON           code*/
  _QUOTE           = 52, /**<KBD_QUOTE           code*/
  _TILDE           = 53, /**<KBD_TILDE           code*/
  _COMMA           = 54, /**<KBD_COMMA           code*/
  _DOT             = 55, /**<KBD_DOT             code*/
  _SLASH           = 56, /**<KBD_SLASH           code*/
  _CAPS_LOCK       = 57, /**<KBD_CAPS_LOCK       code*/
  _F1              = 58, /**<KBD_F1              code*/
  _F2              = 59, /**<KBD_F2              code*/
  _F3              = 60, /**<KBD_F3              code*/
  _F4              = 61, /**<KBD_F4              code*/
  _F5              = 62, /**<KBD_F5              code*/
  _F6              = 63, /**<KBD_F6              code*/
  _F7              = 64, /**<KBD_F7              code*/
  _F8              = 65, /**<KBD_F8              code*/
  _F9              = 66, /**<KBD_F9              code*/
  _F10             = 67, /**<KBD_F10             code*/
  _F11             = 68, /**<KBD_F11             code*/
  _F12             = 69, /**<KBD_F12             code*/
  _PRINTSCREEN     = 70, /**<KBD_PRINTSCREEN     code*/
  _SCROLL_LOCK     = 71, /**<KBD_SCROLL_LOCK     code*/
  _PAUSE           = 72, /**<KBD_PAUSE           code*/
  _INSERT          = 73, /**<KBD_INSERT          code*/
  _HOME            = 74, /**<KBD_HOME            code*/
  _PAGEUP          = 75, /**<KBD_PAGEUP          code*/
  _DELETE          = 76, /**<KBD_DELETE          code*/
  _END             = 77, /**<KBD_END             code*/
  _PAGEDOWN        = 78, /**<KBD_PAGEDOWN        code*/
  _RIGHT           = 79, /**<KBD_RIGHT           code*/
  _LEFT            = 80, /**<KBD_LEFT            code*/
  _DOWN            = 81, /**<KBD_DOWN            code*/
  _UP              = 82, /**<KBD_UP              code*/
  _KEYPAD_NUM_LOCK = 83, /**<KBD_KEYPAD_NUM_LOCK code*/
  _KEYPAD_DIVIDE   = 84, /**<KBD_KEYPAD_DIVIDE   code*/
  _KEYPAD_AT       = 85, /**<KBD_KEYPAD_AT       code*/
  _KEYPAD_MULTIPLY = 85, /**<KBD_KEYPAD_MULTIPLY code*/
  _KEYPAD_MINUS    = 86, /**<KBD_KEYPAD_MINUS    code*/
  _KEYPAD_PLUS     = 87, /**<KBD_KEYPAD_PLUS     code*/
  _KEYPAD_ENTER    = 88, /**<KBD_KEYPAD_ENTER    code*/
  _KEYPAD_1        = 89, /**<KBD_KEYPAD_1        code*/
  _KEYPAD_2        = 90, /**<KBD_KEYPAD_2        code*/
  _KEYPAD_3        = 91, /**<KBD_KEYPAD_3        code*/
  _KEYPAD_4        = 92, /**<KBD_KEYPAD_4        code*/
  _KEYPAD_5        = 93, /**<KBD_KEYPAD_5        code*/
  _KEYPAD_6        = 94, /**<KBD_KEYPAD_6        code*/
  _KEYPAD_7        = 95, /**<KBD_KEYPAD_7        code*/
  _KEYPAD_8        = 96, /**<KBD_KEYPAD_8        code*/
  _KEYPAD_9        = 97, /**<KBD_KEYPAD_9        code*/
  _KEYPAD_0        = 98, /**<KBD_KEYPAD_0        code*/
} kbd_codes_t;

enum enMODIFIER {
  _NONE        = 0x00,
  _LEFT_CTRL   = 0x01,
  _LEFT_SHIFT  = 0x02,
  _LEFT_ALT    = 0x04,
  _LEFT_UI     = 0x08,
  _RIGHT_CTRL  = 0x10,
  _RIGHT_SHIFT = 0x20,
  _RIGHT_ALT   = 0x40,
  _RIGTH_UI    = 0x80,
};

enum {
  SINGLE_COMPOSITE_KEY   = 0,
  MULTIPLE_COMPOSITE_KEY = 1,

};

enum _ConnType{
  WIRE_MODE,
  WIRELESS_MODE,

};


typedef struct {
  uint8_t m_modifier;
  uint8_t m_reserve;
  uint8_t m_table[6];

  uint8_t m_kbdStatus[6][17];
  uint8_t m_kbdLastTimeStatus[6][17];
  uint8_t m_currentRecordMacro;
  uint8_t m_connMode;
  const app_usbd_hid_kbd_t* m_usbHandle;

} KBD_Context_t;

extern KBD_Context_t m_kbdContext;


uint8_t kbd_availableIndex();

void kbd_saveLastStatus();

//Âß¼­
// 1.³õÊ¼»¯
void kbd_init();

void kbd_gpio_init();

// 2.É¨Ãè
void kbd_scan();
// 3.¼üÅÌ×´Ì¬ÊÇ·ñ¸Ä±ä
uint8_t kbd_status_is_changed();
// 4.update hid data
void kbd_update_hid_data();

uint8_t kbd_modifier(uint8_t row, uint8_t col);

void kbd_fn_key_handle();

void start_record_macro();
//É¨ÃèÑ­»·
void kbd_loop();
//·¢ËÍ±¨ÎÄ
void kbd_send_hid_report();

void send_hid_data(app_usbd_hid_kbd_t const* p_kbd, uint8_t* buf);

uint8_t kbd_composite_key_handle(uint8_t row,uint8_t col);
#endif
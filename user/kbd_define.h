#ifndef __USER_KBD_DEFINE__
#define __USER_KBD_DEFINE__
#include "app_usbd_hid_kbd.h"
#include "nrf_esb.h"
#include <stdint.h>

//#define KBD_SCAN_INTERRUPT

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
#define KEY_SUB 45            //-_
#define KEY_ADD 46            //+=
#define KEY_LEFT_BRACKET 47   //[{
#define KEY_RIGHT_BRACKET 48  //]}
#define KEY_SLASH 49          // \|
#define KEY_SEM 51            //·ÖºÅ
#define KEY_YINHAO 52         //'" ÒýºÅ
#define KEY_TILDE 53          //`~ ²¨ÀËÏß
#define KEY_COMMA 54          //,¶ººÅ
#define KEY_PERIOD 55         //.¾äºÅ
#define KEY_WHY 56            //ÎÊºÅ

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

#define KEY_NULL 0
#define KEY_XXX 0

//¿ØÖÆ¼üµÄÎ¨Ò»²Ù×÷
#define LEFT_SHIFT_BIT 1
#define RIGHT_SHIFT_BIT 5
#define LEFT_CTRL_BIT 0
#define RIGHT_CRTL_BIT 4
#define LEFT_ALT_BIT 2
#define RIGHT_ALT_BIT 6
#define WIN_KEY_BIT 3

enum {
  WIRE_MODE,
  WIRELESS_MODE,

};

typedef struct
{
  uint8_t row;
  uint8_t col;

} KEY_Point;

extern uint8_t m_modifier;
extern uint8_t m_macro_key;

extern uint8_t m_hidData[8];

extern uint32_t m_row[6];

extern uint32_t m_col[15];

extern uint8_t m_kbdTable[6][15];
extern uint8_t m_lastTable[6][15];

extern uint8_t m_per_row_col[6];
extern uint32_t m_row_col_0[15];
extern uint32_t m_row_col_1[15];
extern uint32_t m_row_col_2[15];
extern uint32_t m_row_col_3[15];
extern uint32_t m_row_col_4[13];
extern uint32_t m_row_col_5[11];

extern unsigned char m_KBD_TABLE[6][15];

extern KEY_Point LCTRL_KEY;
extern KEY_Point LSHIFT_KEY;
extern KEY_Point LALT_KEY;

extern KEY_Point RCTRL_KEY;
extern KEY_Point RSHIFT_KEY;
extern KEY_Point RALT_KEY;
extern KEY_Point WIN_KEY;
extern KEY_Point FN_KEY;
extern KEY_Point MENU_KEY;
extern KEY_Point UP_KEY;
extern KEY_Point LEFT_KEY;
extern KEY_Point DOWN_KEY;
extern KEY_Point RIGTH_KEY;
extern KEY_Point A_KEY;
extern KEY_Point Q_KEY;
extern KEY_Point W_KEY;
extern KEY_Point E_KEY;
extern KEY_Point CAPS_KEY;
extern KEY_Point BOLANGXIAN_KEY;
extern KEY_Point J_KEY;
extern KEY_Point K_KEY;
extern KEY_Point L_KEY;
extern KEY_Point I_KEY;
extern KEY_Point BACKSPACE_KEY;

extern KEY_Point K_1_KEY;
extern KEY_Point K_2_KEY;
extern KEY_Point K_3_KEY;
extern KEY_Point K_4_KEY;
extern KEY_Point K_5_KEY;
extern KEY_Point K_6_KEY;
extern KEY_Point K_7_KEY;
extern KEY_Point K_8_KEY;
extern KEY_Point K_9_KEY;
extern KEY_Point K_0_KEY;

extern const app_usbd_hid_kbd_t* m_usbHandle;
extern uint8_t m_mode;
void kbd_debug();

void transmite_data();

#endif
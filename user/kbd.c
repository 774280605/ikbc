#include "kbd.h"
#include "SEGGER_RTT.h"
#include "macro.h"
#include "nrf_delay.h"
#include "nrf_drv_gpiote.h"
#include "nrf_gpiote.h"
#include "low_energy_timer.h"
#include "ws2812.h"
#include <string.h>
#include <stdbool.h>
#include "gpiote_reg.h"
#include "app_usbd_hid_kbd_internal.h"
#include "nrf_gzll.h"
KBD_Context_t m_kbdContext;
uint32_t m_lowEnergyCount=0;

bool m_lowEnergyTimeout=false;
#define PIPE_NUM_0 0
#define PIPE_NUM_1 1

uint32_t m_unuse_pin[9]={
NRF_GPIO_PIN_MAP(0,15),
NRF_GPIO_PIN_MAP(0,17),
NRF_GPIO_PIN_MAP(0,20),
NRF_GPIO_PIN_MAP(0,22),
NRF_GPIO_PIN_MAP(1,0),
NRF_GPIO_PIN_MAP(1,2),
NRF_GPIO_PIN_MAP(1,4),
NRF_GPIO_PIN_MAP(1,6),
NRF_GPIO_PIN_MAP(0,10),
};



void reset_kbd_status()
{
    for (uint8_t i = 0; i < 6; ++i)
    {
        for (uint8_t j = 0; j < 15; ++j)
        {
            
            m_kbdContext.m_kbdStatus[i][j]=m_kbdContext.m_kbdLastTimeStatus[i][j]=0;
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
        
        //1
        if(m_kbdContext.m_kbdStatus[K_1_KEY.row][K_1_KEY.col])
        {
            if(m_kbdContext.m_connMode==WIRE_MODE)
            {
              //set_color_white();
              nrf_delay_ms(200);
              m_kbdContext.m_pipe=PIPE_NUM_0;
            }
            reset_kbd_status();
        }
         if(m_kbdContext.m_kbdStatus[K_2_KEY.row][K_2_KEY.col])
        {
            if(m_kbdContext.m_connMode==WIRE_MODE)
            {
              //set_color_green();
              nrf_delay_ms(200);
              m_kbdContext.m_pipe=PIPE_NUM_1;
            }
            reset_kbd_status();
        }
         if(m_kbdContext.m_kbdStatus[K_3_KEY.row][K_3_KEY.col])
        {
            if(m_kbdContext.m_connMode==WIRE_MODE)
            {
              //set_color_red();
              nrf_delay_ms(200);
            }
            reset_kbd_status();
        }
         if(m_kbdContext.m_kbdStatus[K_4_KEY.row][K_4_KEY.col])
        {
            if(m_kbdContext.m_connMode==WIRE_MODE)
            {
              //set_color_blue();
              nrf_delay_ms(200);
            }
            reset_kbd_status();
        }
         if(m_kbdContext.m_kbdStatus[K_5_KEY.row][K_5_KEY.col])
        {
            if(m_kbdContext.m_connMode==WIRE_MODE)
            {
              //set_custom_color_1();
              nrf_delay_ms(200);
            }
              
            reset_kbd_status();
        }
         if(m_kbdContext.m_kbdStatus[K_6_KEY.row][K_6_KEY.col])
        {
            if(m_kbdContext.m_connMode==WIRE_MODE)
            {
              //set_custom_color_2();
              nrf_delay_ms(200);
            }
              
            reset_kbd_status();
        }
        if(m_kbdContext.m_kbdStatus[K_7_KEY.row][K_7_KEY.col])
        {
            if(m_kbdContext.m_connMode==WIRE_MODE)
            { 

              //set_custom_color_3();
              nrf_delay_ms(200);
            }
            reset_kbd_status();
        }
        if(m_kbdContext.m_kbdStatus[K_8_KEY.row][K_8_KEY.col])
        {
            if(m_kbdContext.m_connMode==WIRE_MODE)
            { 
             //set_custom_color_4();
             nrf_delay_ms(200);
            }
            reset_kbd_status();
        }
        
         if(m_kbdContext.m_kbdStatus[K_0_KEY.row][K_0_KEY.col])
        {
            if(m_kbdContext.m_connMode==WIRE_MODE)
            { 
              //setup_pretty_led();
              nrf_delay_ms(200);
            }
            reset_kbd_status();
        }

        if(m_kbdContext.m_kbdStatus[UP_KEY.row][UP_KEY.col])
        {
            if(m_kbdContext.m_connMode==WIRE_MODE)
            {
              //increase_light();
              nrf_delay_ms(100);
            }
              
            reset_kbd_status();
        }
        if(m_kbdContext.m_kbdStatus[DOWN_KEY.row][DOWN_KEY.col])
        {
            if(m_kbdContext.m_connMode==WIRE_MODE)
            {
             // decrease_light();
              nrf_delay_ms(100);
            }
              
            reset_kbd_status();
        }
        
        
    }
}



void kbd_init()
{
    memset(&m_kbdContext,0,sizeof(m_kbdContext));
    m_kbdContext.m_connMode=WIRELESS_MODE;
    m_kbdContext.m_pipe=0;
    
    for(uint8_t i=0;i<9;++i)
    {
      nrf_gpio_cfg_input(m_unuse_pin[i],NRF_GPIO_PIN_PULLDOWN);
      nrf_delay_us(2);
    }
    //capslock
    nrf_gpio_cfg_output(NRF_GPIO_PIN_MAP(0,9));
    kbd_capslock_off();
    kbd_gpio_init();

    //led_flash_init();

    //low_energy_timer_init();
    //kbd_setup_low_energy_tiemr();
    //gpiote_reg_init();
    //kbd_gpiote_enable();
    

}



void kbd_gpio_init()
{
    for (int col = 0; col < 15; ++col)
    {
        nrf_gpio_cfg_input(m_col[col],NRF_GPIO_PIN_PULLDOWN);
        //nrf_gpio_cfg_output(m_col[col]);
       
        //nrf_gpio_pin_clear(m_col[col]);
        //nrf_gpio_pin_set(m_col[col]);
    }


    
    //end test
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
        nrf_gpio_cfg_input(m_col[i],NRF_GPIO_PIN_PULLDOWN);
        //nrf_gpio_pin_clear(m_col[i]);
    }
        


    // 0 row
    for (uint8_t col = 0; col < m_per_row_col[0]; ++col)
    {
        nrf_gpio_cfg_output(m_row_col_0[col]);
        nrf_delay_us(40);
        nrf_gpio_pin_set(m_row_col_0[col]);
        nrf_delay_us(40);
        if (nrf_gpio_pin_read(m_row[0]))
        {
            m_kbdContext.m_kbdStatus[0][col] = 1;
        }
        else
        {
            m_kbdContext.m_kbdStatus[0][col] = 0;
        }
        nrf_gpio_pin_clear(m_row_col_0[col]);
        nrf_delay_us(10);
        nrf_gpio_cfg_input(m_row_col_0[col],NRF_GPIO_PIN_PULLDOWN);
        nrf_delay_us(10);
    }

    // 1 row
    for (uint8_t col = 0; col < m_per_row_col[1]; ++col)
    {

        nrf_gpio_cfg_output(m_row_col_1[col]);
        nrf_delay_us(40);
        nrf_gpio_pin_set(m_row_col_1[col]);
        nrf_delay_us(40);
        if (nrf_gpio_pin_read(m_row[1]))
        {
            m_kbdContext.m_kbdStatus[1][col] = 1;
        }
        else
        {
            m_kbdContext.m_kbdStatus[1][col] = 0;
        }
        nrf_gpio_pin_clear(m_row_col_1[col]);
        nrf_delay_us(10);
        nrf_gpio_cfg_input(m_row_col_1[col],NRF_GPIO_PIN_PULLDOWN);
        nrf_delay_us(10);
    }

    // 2 row
    for (uint8_t col = 0; col < m_per_row_col[2]; ++col)
    {
        nrf_gpio_cfg_output(m_row_col_2[col]);
        nrf_delay_us(40);
        nrf_gpio_pin_set(m_row_col_2[col]);
        nrf_delay_us(40);
        if (nrf_gpio_pin_read(m_row[2]))
        {
            m_kbdContext.m_kbdStatus[2][col] = 1;
        }
        else
        {
            m_kbdContext.m_kbdStatus[2][col] = 0;
        }
        nrf_gpio_pin_clear(m_row_col_2[col]);
        nrf_delay_us(10);
        nrf_gpio_cfg_input(m_row_col_2[col],NRF_GPIO_PIN_PULLDOWN);
        nrf_delay_us(10);
    }

    // 3 row
    for (uint8_t col = 0; col < m_per_row_col[3]; ++col)
    {
        nrf_gpio_cfg_output(m_row_col_3[col]);
        nrf_delay_us(40);
        nrf_gpio_pin_set(m_row_col_3[col]);
        nrf_delay_us(40);
        if (nrf_gpio_pin_read(m_row[3]))
        {
            m_kbdContext.m_kbdStatus[3][col] = 1;
        }
        else
        {
            m_kbdContext.m_kbdStatus[3][col] = 0;
        }
        nrf_gpio_pin_clear(m_row_col_3[col]);
        nrf_delay_us(10);
        nrf_gpio_cfg_input(m_row_col_3[col],NRF_GPIO_PIN_PULLDOWN);
        nrf_delay_us(10);
    }

    // 4 row
    for (uint8_t col = 0; col < m_per_row_col[4]; ++col)
    {
        nrf_gpio_cfg_output(m_row_col_4[col]);
        nrf_delay_us(40);
        nrf_gpio_pin_set(m_row_col_4[col]);

        nrf_delay_us(40);
        if (nrf_gpio_pin_read(m_row[4]))
        {
            m_kbdContext.m_kbdStatus[4][col] = 1;
        }
        else
        {
            m_kbdContext.m_kbdStatus[4][col] = 0;
        }
        nrf_gpio_pin_clear(m_row_col_4[col]);
        nrf_delay_us(10);
        nrf_gpio_cfg_input(m_row_col_4[col],NRF_GPIO_PIN_PULLDOWN);
        nrf_delay_us(10);
    }

    // 5 row
    for (uint8_t col = 0; col < m_per_row_col[5]; ++col)
    {
        nrf_gpio_cfg_output(m_row_col_5[col]);
        nrf_delay_us(40);
        nrf_gpio_pin_set(m_row_col_5[col]);
        nrf_delay_us(40);
        if (nrf_gpio_pin_read(m_row[5]))
        {
            m_kbdContext.m_kbdStatus[5][col] = 1;
        }
        else
        {
            m_kbdContext.m_kbdStatus[5][col] = 0;
        }
        nrf_gpio_pin_clear(m_row_col_5[col]);
        nrf_delay_us(10);
        nrf_gpio_cfg_input(m_row_col_5[col],NRF_GPIO_PIN_PULLDOWN);
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
    m_kbdContext.m_modifier=KBD_MODIFIER_NONE;
    
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
                    }
                    else if(kbd_composite_key_handle(i,j))
                    {
                        
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
    //m_hidData[0] = m_modifier;
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
	m_kbdContext.m_modifier|=KBD_MODIFIER_LEFT_SHIFT;
	//m_modifier |= (1 << LEFT_SHIFT_BIT);
    }
    else if (RSHIFT_KEY.row == row && RSHIFT_KEY.col == col)
    {
        //m_modifier |= (1 << RIGHT_SHIFT_BIT);
	m_kbdContext.m_modifier|=KBD_MODIFIER_RIGHT_SHIFT;
    }
    else if (LCTRL_KEY.row == row && LCTRL_KEY.col == col)
    {
        //m_modifier |= (1 << LEFT_CTRL_BIT);
	m_kbdContext.m_modifier|=KBD_MODIFIER_LEFT_CTRL;
    }
    else if (RCTRL_KEY.row == row && RCTRL_KEY.col == col)
    {
        //m_modifier |= (1 << RIGHT_CRTL_BIT);
        m_kbdContext.m_modifier|=KBD_MODIFIER_RIGHT_CTRL;
    }
    else if (LALT_KEY.row == row && LALT_KEY.col == col)
    {
        m_kbdContext.m_modifier|=KBD_MODIFIER_LEFT_ALT;
    }
    else if (RALT_KEY.row == row && RALT_KEY.col == col)
    {
         m_kbdContext.m_modifier|=KBD_MODIFIER_RIGHT_ALT;
    }
    else if (WIN_KEY.row == row && WIN_KEY.col == col)
    {
         m_kbdContext.m_modifier|=KBD_MODIFIER_LEFT_UI;
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
        //if(m_kbdContext.m_connMode==WIRE_MODE)
         // led_flash_start();
        uint8_t is_create = 0;
        uint8_t recordKey = m_kbdContext.m_currentRecordMacro;
        uint8_t alreadyInMacroMount = 0;
        put_macro_id(recordKey);
        m_kbdContext.m_currentRecordMacro = 0;

        do
        {
            kbd_scan();
            if(kbd_StatusIsChanged()==0)
            {
                continue;
            }
            /*if (is_create_macroo() && (is_create >0))
            {
                if (m_kbdContext.m_currentRecordMacro == recordKey)
                {
                    break;
                }
            }*/

            if (is_create_macroo() && is_create == 0)
            {
                continue;
            }

            //fn key end
            if(is_fn_macro() && is_create>0)
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
        //if(m_kbdContext.m_connMode==WIRE_MODE)
         // led_flash_stop();
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
        nrf_gzll_add_packet_to_tx_fifo(m_kbdContext.m_pipe, 
        (uint8_t*)&m_kbdContext, 8);
    }
    else
    {
        send_hid_data(m_kbdContext.m_usbHandle, (uint8_t*)&m_kbdContext);
    }

}

uint8_t kbd_composite_key_handle(uint8_t row,uint8_t col)
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

void kbd_capslock_on()
{   
    nrf_gpio_pin_clear(NRF_GPIO_PIN_MAP(0,9));
}
void kbd_capslock_off()
{
    nrf_gpio_pin_set(NRF_GPIO_PIN_MAP(0,9));
}

#include "kbd_reactor.h"

#include "nrf_gpio.h"
#include  "nrf_gpiote.h"
#include "kbd_define.h"
#include "nrf_drv_gpiote.h"
#include "SEGGER_RTT.h"
#include "nrf_delay.h"
#include<string.h>
struct Kbd_Context_t{
  uint8_t m_usbHidData[8];
  uint8_t m_kbdTable[6][16];
  uint8_t m_lastTable[6][16];
  uint8_t m_modifier;//modifier key


  
};


static void row_in_pin_handler(nrf_drv_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
    KBD_Reactor_loop();
}






void KBD_Reactor_init(){
     for(int col=0;col<16;++col){
      nrf_gpio_cfg_output(m_col[col]);
     nrf_gpio_pin_set(m_col[col]);
  }

  ret_code_t err_code;


  for(int row=0;row<6;++row){
      nrf_drv_gpiote_in_config_t in_config=NRFX_GPIOTE_CONFIG_IN_SENSE_TOGGLE(true);
      in_config.sense=NRF_GPIOTE_POLARITY_TOGGLE;//NRF_GPIOTE_POLARITY_TOGGLE;
      in_config.pull=NRF_GPIO_PIN_PULLDOWN;
  
      err_code = nrf_drv_gpiote_in_init(m_row[row], &in_config, row_in_pin_handler);
      APP_ERROR_CHECK(err_code);
 
      nrf_drv_gpiote_in_event_enable(m_row[row], true);
  }


}



void KBD_Reactor_row_enable_in_event(){
   for(int i=0;i<6;++i){
   nrf_delay_us(10);
        nrf_drv_gpiote_in_event_enable(m_row[i], true);
      
    }

}

void KBD_Reactor_row_disable_in_event(){
  
    for(int i=0;i<6;++i){
        nrf_drv_gpiote_in_event_disable(m_row[i]);
      
    }

}



void KBD_Reactor_loop(){
    KBD_Reactor_row_disable_in_event();

     for(int i=0;i<16;++i)
  {
      nrf_gpio_pin_set(m_col[i]);
      
      nrf_delay_us(10);
      for(int j=0;j<6;++j)
      {
        if(nrf_gpio_pin_read(m_row[j]))
        {
            m_kbdTable[j][i]=1;
            SEGGER_RTT_printf(0,"m_kbdTable[%d][%d] is down\n",j,i);
        
        }
        else
        {
           m_kbdTable[j][i]=0;
        }
          
      }
      

      nrf_gpio_pin_clear(m_col[i]);
      nrf_delay_us(10);
  }

    if(KBD_Reactor_status()){
        KBD_Reactor_updatebuf();
        transmite_data(m_hidData,m_mode);
    
    }
    
    KBD_Reactor_row_enable_in_event();
}

uint8_t KBD_Reactor_status(){
   for(int i=0;i<6;++i){
    for(int j=0;j<16;++j){
        if(m_lastTable[i][j]!= m_kbdTable[i][j]){
          KBD_Reactor_save_last();
        
          return 1;
        }
    }
  
  }
  
  return 0;


}


void KBD_Reactor_updatebuf(){
     memset(m_hidData,0,8);
  m_modifier=0;
  for(int i=0;i<6;++i)
  {
    for(int j=0;j<16;++j)
    {
      if(m_kbdTable[i][j])
      {
        if(KBD_Reactor_modifier(i,j)==0)
        {
          uint8_t key= m_KBD_TABLE[i][j];
          uint8_t index= KBD_availableIndex();
          m_hidData[index]=key;
        
        }
      }
    
    }
  
  }
  m_hidData[0]=m_modifier;
  


}


void KBD_Reactor_save_last(){
     for(int row=0;row<6;++row){
      for(int col=0;col<16;++col){
         m_lastTable[row][col]=m_kbdTable[row][col];
              
              
      }
    }
    kbd_debug();


}

 uint8_t KBD_Reactor_modifier(uint8_t row,uint8_t col){
    if(LSHIFT_KEY.row== row && LSHIFT_KEY.col==col){
		m_modifier |= (1 << LEFT_SHIFT_BIT);
	}
	else if(RSHIFT_KEY.row==row && RSHIFT_KEY.col==col){
		m_modifier |= (1 << RIGHT_SHIFT_BIT);
	}
	else if(LCTRL_KEY.row== row && LCTRL_KEY.col==col){
		m_modifier |= (1 << LEFT_CTRL_BIT);
	}
	else if(RCTRL_KEY.row==row && RCTRL_KEY.col==col){
		m_modifier |= (1 << RIGHT_CRTL_BIT);
	}
	else if(LALT_KEY.row==row && LALT_KEY.col==col){
		m_modifier |= (1 << LEFT_ALT_BIT);
	}
	else if(RALT_KEY.row==row && RALT_KEY.col==col){
		m_modifier |= (1 << RIGHT_ALT_BIT);
	}
	else if(WIN_KEY.row==row && WIN_KEY.col==col){
		m_modifier |= (1 << WIN_KEY_BIT);
	}else{
          return 0;
        }
    return 1;
 }


 uint8_t KBD_availableIndex(){
 
   for(int i=2;i<8;i++){
    if(m_hidData[i]==0){
        return i;
    }
  }
  return 7; 
 }
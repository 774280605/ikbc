#include "gpiote_reg.h"
#include "nrf_gpiote.h"
#include <string.h>
GPIOTE_REG_Context m_ctx;
void gpiote_reg_init()
{

  
  memset(&m_ctx,0,sizeof(m_ctx));
  
  NVIC_SetPriority(GPIOTE_IRQn,5);
  NVIC_EnableIRQ(GPIOTE_IRQn);

}

void gpiote_reg_in_event(uint8_t pin)
{
   uint8_t port = (pin>>5)&0x01;
   uint8_t p= pin&0x1f;
  

   nrf_gpio_cfg(pin,
        NRF_GPIO_PIN_DIR_INPUT,
        NRF_GPIO_PIN_INPUT_CONNECT,
        NRF_GPIO_PIN_PULLDOWN,
        NRF_GPIO_PIN_S0S1,
        NRF_GPIO_PIN_SENSE_HIGH);


  
  uint32_t config=0;
  config|=0x01; //event;
  config|=(p<<8);//gpio 2
  config|= (port<<13);
  config|= (0x01<<16);//low to high
  

  NRF_GPIOTE->CONFIG[m_ctx.index]=config;
  //enable interrupt
  NRF_GPIOTE->INTENSET+=(m_ctx.index+1);

  m_ctx.pin[m_ctx.index] =m_ctx.reset_pin[m_ctx.index]=pin;

  ++m_ctx.index;
  ++m_ctx.count;
}


void gpiote_reg_in_uninit()
{
  NRF_GPIOTE->INTENCLR=0xFF;
  NRF_GPIOTE->CONFIG[0]=0;
  NRF_GPIOTE->CONFIG[1]=0;
  NRF_GPIOTE->CONFIG[2]=0;
  NRF_GPIOTE->CONFIG[3]=0;
  NRF_GPIOTE->CONFIG[4]=0;
  NRF_GPIOTE->CONFIG[5]=0;
  NRF_GPIOTE->CONFIG[6]=0;
  NRF_GPIOTE->CONFIG[7]=0;
  m_ctx.index=0;
}

void gpiote_reg_in_reset()
{
  for(uint8_t i=0;i<m_ctx.count;++i)
  {
     uint8_t tmp=  m_ctx.reset_pin[i];
     uint8_t port = (tmp>>5)&0x01;
     uint8_t p= tmp&0x1f;
  

     nrf_gpio_cfg(tmp,
          NRF_GPIO_PIN_DIR_INPUT,
          NRF_GPIO_PIN_INPUT_CONNECT,
          NRF_GPIO_PIN_PULLDOWN,
          NRF_GPIO_PIN_S0S1,
          NRF_GPIO_PIN_SENSE_HIGH);


  
    uint32_t config=0;
    config|=0x01; //event;
    config|=(p<<8);//gpio 2
    config|= (port<<13);
    config|= (0x01<<16);//low to high
  

    NRF_GPIOTE->CONFIG[m_ctx.index]=config;
    //enable interrupt
    NRF_GPIOTE->INTENSET+=m_ctx.index;

    

    ++m_ctx.index;
    
  }
}
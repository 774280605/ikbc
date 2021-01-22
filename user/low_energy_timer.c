#include "low_energy_timer.h"
#include "kbd.h"
APP_TIMER_DEF(m_low_energy_timer);


extern void low_energy_handler(void * p_context);



void low_energy_timer_init()
{
     ret_code_t ret;
     ret = app_timer_create(&m_low_energy_timer, APP_TIMER_MODE_REPEATED, 
                            low_energy_handler);
      APP_ERROR_CHECK(ret);

}

void low_energy_timer_stop()
{
      app_timer_stop(m_low_energy_timer);
  
}

void low_energy_timer_start()
{
     app_timer_start(m_low_energy_timer, APP_TIMER_TICKS(1000), NULL);
}
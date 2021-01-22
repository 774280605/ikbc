#ifndef __USER_KBD_REACTOR__
#define __USER_KBD_REACTOR__
#include <stdint.h>

typedef struct Kbd_Context_t KBD_Context; 


void KBD_Reactor_init();


void KBD_Reactor_loop();

uint8_t KBD_Reactor_status();


void KBD_Reactor_updatebuf();


void KBD_Reactor_save_last();



 uint8_t KBD_Reactor_modifier(uint8_t row,uint8_t col);


uint8_t KBD_availableIndex();


#endif



/*
 * macro.c
 *
 *  Created on: 2020��6�
 *      Author: demon
 */

#include "macro.h"
#include "kbd_define.h"
#include "kbd.h"


uint8_t macro_index=0;

uint8_t macro_table[MACRO_TABLE_ROW][MACRO_TABLE_COL]={0};



uint8_t is_macro_key(uint8_t key){
	for(uint8_t i=0;i<MACRO_TABLE_ROW;++i){
            if(macro_table[i][0]==key){
                    return 1;
            }
	}
	return 0;
}

void reset_macro_table(){
	for(uint8_t i=0;i<MACRO_TABLE_ROW;++i)
        {
		for(uint8_t j=0;j<MACRO_TABLE_COL;++j)
                {
			macro_table[i][j]=0x00;
		}
	}
	macro_index=0;
}


void put_macro_key(uint8_t id,uint8_t key){
    for(uint8_t i=0;i<MACRO_TABLE_ROW;++i)
    {
        if(macro_table[i][0]==id)
        {
            for(uint8_t col=1;col<MACRO_TABLE_COL;++col)
            {
                    if(macro_table[i][col]==0x00)
                    {
                            macro_table[i][col]=key;
                            return;
                    }
            }

        }
    }
}

void put_macro_id(uint8_t id){
	for(uint8_t i=0;i<MACRO_TABLE_ROW;++i)
        {
		if(macro_table[i][0]==0x00)
                {
			macro_table[i][0]=id;
			break;
		}
	}
	macro_index++;

	macro_index%= MACRO_TABLE_ROW;
}

//判断是宏才进入此函数
void put_macro_key_to_pkt(uint8_t key){
    for(uint8_t i=0;i<MACRO_TABLE_ROW;++i)
    {
        if(macro_table[i][0]==key)
        {
            for(uint8_t col=1;col<MACRO_TABLE_COL;++col)
            {
              if(macro_table[i][col]==0)
                      break;
              for(uint8_t j=0;j<6;++j)
              {
                  if(m_kbdContext.m_table[j]==0x00){
                        m_kbdContext.m_table[j]=macro_table[i][col];
                        break;
                  }
              }
            }
            return;
        }
    }
}

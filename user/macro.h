/*
 * macro.h
 *
 *  Created on: 2020å¹æœ6æ—
 *      Author: demon
 */

#ifndef SRC_MACRO_H_
#define SRC_MACRO_H_
#include <stdint.h>



#define MACRO_TABLE_ROW 6
#define MACRO_TABLE_COL 4







/*
 * 
 * 0	1 2   3
 * ID KEY,KEY,KEY
 *
 */

extern uint8_t macro_table[MACRO_TABLE_ROW][MACRO_TABLE_COL];





uint8_t is_macro_key(uint8_t key);

void reset_macro_table();

void put_macro_key(uint8_t id, uint8_t key);

void put_macro_id(uint8_t id);

void put_macro_key_to_pkt(uint8_t key);



#endif /* SRC_MACRO_H_ */

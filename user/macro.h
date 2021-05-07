/*
 * macro.h
 *
 *  Created on: 2020å¹æœ6æ—
 *      Author: demon
 */

#ifndef SRC_MACRO_H_
#define SRC_MACRO_H_
#include <stdint.h>




typedef struct {
  char key;
  char value[6];
}macro_matrix_t; 


uint8_t is_macro_key(uint8_t key);

void reset_macro_table();

void put_macro_key(uint8_t id, uint8_t key);

void put_macro_id(uint8_t id);

void put_macro_key_to_pkt(uint8_t key);



#endif /* SRC_MACRO_H_ */

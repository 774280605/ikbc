/*
 * macro.c
 *
 *  Created on: 2020��6�
 *      Author: demon
 */

#include "macro.h"

#include "kbd.h"

macro_matrix_t m_macro[3];

uint8_t macro_index = 0;

uint8_t is_macro_key(uint8_t key)
{
    for (int i = 0; i < 3; ++i)
    {
        if (m_macro[i].key == key)
        {
            return 1;
        }
    }
    return 0;
}

void reset_macro_table()
{
    memset(m_macro, 0, sizeof(macro_matrix_t) * 3);
    macro_index = 0;
}

void put_macro_key(uint8_t id, uint8_t key)
{
    for (int i = 0; i < 3; ++i)
    {
        if (m_macro[i].key == id)
        {
            for (int j = 0; j < 6; ++j)
            {
                if (m_macro[i].value[j] == 0)
                {
                    m_macro[i].value[j] = key;
                }
            }
        }
    }

    return;
}

void put_macro_id(uint8_t id)
{
    for (int i = 0; i < 3; ++i)
    {
        if (m_macro[i].key == 0x00)
        {
            m_macro[i].key = id;
            break;
        }
    }
    macro_index++;

    macro_index %= 3;
}

//判断是宏才进入此函数
void put_macro_key_to_pkt(uint8_t key)
{
    for (int i = 0; i < 3; ++i)
    {
        if (m_macro[i].key == key)
        {
            for (int j = 0; j < 6; ++j)
            {
                if (m_macro[i].value[j] != 0)
                {
                    for (int x = 0; x < 6; ++x)
                    {
                        if (m_kbdContext.m_table[x] == 0x00)
                        {
                            m_kbdContext.m_table[x] = m_macro[i].value[j];
                            break;
                        }
                    }
                }
            }
            return;
        }
    }
}
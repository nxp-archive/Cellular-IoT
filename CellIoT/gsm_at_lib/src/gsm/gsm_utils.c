/**
 * \file            gsm_utils.c
 * \brief           Utilities
 */

/*
 * Copyright (c) 2019 Tilen MAJERLE
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of GSM-AT library.
 *
 * Author:          Tilen MAJERLE <tilen@majerle.eu>
 * Version:         v0.6.0
 */
#include "gsm_private.h"
#include "gsm_utils.h"
#include "gsm_mem.h"
#include <stdint.h>

/**
 * \brief           Convert `unsigned 32-bit` number to string
 * \param[in]       num: Number to convert
 * \param[out]      out: Output variable to save string
 * \param[in]       is_hex: Set to `1` to output hex, 0 otherwise
 * \param[in]       width: Width of output string.
 *                      When number is shorter than width, leading `0` characters will apply.
 *                      This parameter is valid only when formatting hex numbers
 * \return          Pointer to output variable
 */
char *
gsm_u32_to_gen_str(uint32_t num, char* out, uint8_t is_hex, uint8_t width) {
    char* tmp = out;
    uint8_t i, y;

    /* Convert number to string */
    i = 0;
    tmp[0] = '0';
    if (num == 0) {
        i++;
    } else {
        if (is_hex) {
            uint8_t mod;
            while (num > 0) {
                mod = num & 0x0F;
                if (mod < 10) {
                    tmp[i] = mod + '0';
                } else {
                    tmp[i] = mod - 10 + 'A';
                }
                num >>= 4;
                i++;
            }
        } else {
            while (num > 0) {
                tmp[i] = (num % 10) + '0';
                num /= 10;
                i++;
            }
        }
    }
    if (is_hex) {
        while (i < width) {
            tmp[i] = '0';
            i++;
        }
    }
    tmp[i] = 0;

    /* Rotate string */
    y = 0;
    while (y < ((i + 1) / 2)) {
        char t = out[i - y - 1];
        out[i - y - 1] = tmp[y];
        tmp[y] = t;
        y++;
    }
    out[i] = 0;
    return out;
}

/**
 * \brief           Convert `signed 32-bit` number to string
 * \param[in]       num: Number to convert
 * \param[out]      out: Output variable to save string
 * \return          Pointer to output variable
 */
char *
gsm_i32_to_gen_str(int32_t num, char* out) {
    if (num < 0) {
        *out++ = '-';
        return gsm_u32_to_gen_str(GSM_U32(-num), out, 0, 0) - 1;
    } else {
        return gsm_u32_to_gen_str(GSM_U32(num), out, 0, 0);
    }
}

/**
 * \brief           Create a list to keep trace of the '+SRQNSRING' received
 * \return          Pointer to the list that keep trace of the '+SRQNSRING' received
 */
st_NewRingList * gsm_ring_list_init(void)
{
	st_NewRingList *list = malloc(sizeof(st_NewRingList));

    if (list == NULL)
    {
        exit(EXIT_FAILURE);
    }
    list->first_ring = NULL;
    list->is_at_sqnsrecv_ongoing = 0;

    return list;
}

/**
 * \brief           Insert a new ring elem in the list at a given position
 * \param[in]       list: list that keep trace of the '+SRQNSRING' received
 * \param[in]       byte_pending: byte pending to read given by the ring event
 * \param[in]       conn_id: connection id given by the ring event
 * \param[in]       pos: position of the new elem in the list (0=top/-1=bottom/[1-..]specified position)
 * \return          void
 */
void gsm_ring_list_insert_elem(st_NewRingList *list, uint32_t byte_pending, uint32_t conn_id, int8_t pos)
{
    /* generation of a new element */
	st_RingElem *new = malloc(sizeof(st_RingElem));

    if (list == NULL || new == NULL)
    {
        exit(EXIT_FAILURE);
    }
    new->BytesPending = byte_pending;
    new->connid = conn_id;

    /* Insertion of the new elem in the list */
    if (0 == pos)
    {
    	/* Insert the elem at the top of the list */
    	new->next_ring = list->first_ring;
    	list->first_ring = new;
    }
    else
	{
		/* Insert the elem at a given position in the list */
		st_RingElem *cur_elem = list->first_ring;
    	st_RingElem *prev_elem = NULL;
		uint8_t unsigned_pos = (uint8_t)pos, cnt = 0;

		/* Check if at least one elem is in the list */
		if (NULL == cur_elem)
		{
			list->first_ring = new;		/* No elem in the list so add one */
			new->next_ring = NULL;
		}
		else
		{
			while( cur_elem->next_ring != NULL && cnt < unsigned_pos )
			{
				prev_elem = cur_elem;
				cur_elem = cur_elem->next_ring;
			}

			if (cur_elem->next_ring == NULL)
			{
				cur_elem->next_ring = new;
				new->next_ring = NULL;
			}
			else
			{
				prev_elem->next_ring = new;
				new->next_ring = cur_elem;
			}
		}
	}
}

/**
 * \brief           Delete a ring elem in the list at a given position
 * \param[in]       list: list that keep trace of the '+SRQNSRING' received
 * \param[in]       pos: position of the elem in the list (0=top/-1=bottom)
 * \return          void
 */
void gsm_ring_list_delete_elem(st_NewRingList *list, int8_t pos)
{
    if (list == NULL)
    {
        exit(EXIT_FAILURE);
    }

    /* Supression of the elem in the list */
    if (!pos)
    {
    	if(list->first_ring != NULL)
    	{
    		/* Delete the elem at the top of the list */
    		st_RingElem *del_elem = list->first_ring;
			list->first_ring = del_elem->next_ring;
			free(del_elem);
    	}
    }
    else
    {
    	/* Delete the elem at the bottom of the list */
    	st_RingElem *cur_elem = list->first_ring;
    	st_RingElem *prev_elem = NULL;

    	while( cur_elem->next_ring != NULL )
    	{
    		prev_elem = cur_elem;
    		cur_elem = cur_elem->next_ring;
    	}

		free(cur_elem);
		if (prev_elem != NULL)
		{
			prev_elem->next_ring = NULL;
		}
		else
		{
			list->first_ring = NULL;
		}
    }
}

/**
 * @file fifo.h
 * Classical First In First Out buffer implementation
 */

#ifndef FIFO_H
#define FIFO_H

#ifdef __cplusplus
extern "C" {
#endif


/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"

#if USE_FIFO != 0
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "dyn_mem.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**
 * Descriptor of a fifo 
 */
typedef struct
{
    uint32_t item_size;
    uint32_t item_num;
    uint32_t wp;
    uint32_t rp;
    uint8_t empty :1;
    uint8_t full  :1;
    uint8_t * buf;
}fifo_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Initialize a fifo.
 * @param fifo_p pointer to a fifo_t variable
 * @param buf_p pointer to the buffer of the fifo (item_size * item_num bytes size )
 * @param item_size the of an item stored by the fifo (in bytes)
 * @param item_num number of elements stored by the fifo
 */
void fifo_init(fifo_t * fifo_p, void * buf_p, uint32_t item_size, uint32_t item_num);

/**
 * Add item to the fifo
 * @param fifo_p pointer to fifo_t variable
 * @param data_p pointer to a data to add
 * @return true: if the data is pushed, false: if the fifo was full
 */
bool fifo_push(fifo_t * fifo_p, const void * data_p);

/**
 * Add multiple data to the fifo
 * @param fifo_p pointer to a fifo_t variable
 * @param data_p pointer to the data to push
 * @param num number of items to push
 * @return number of pushed items (<= 'num')
 */
uint32_t fifo_push_mult(fifo_t * fifo_p, const void * data_p, uint32_t num);

/**
 * Read data from a fifo
 * @param fifo_p pointer to a fifo_t variable
 * @param data_p pointer to buffer to store the read data
 * @return true: if the data is popped, false: if the fifo was empty
 */
bool fifo_pop(fifo_t * fifo_p, void * data_p);

/**
 * Read multiple data from a fifo
 * @param fifo_p pointer to fifo_t variable
 * @param data_p buffer to store the read data
 * @param num number of items to read
 * @return  number of popped items (<= 'num')
 */
uint32_t fifo_pop_mult(fifo_t * fifo_p, void * data_p, uint32_t num);

/**
 * Make the fifo empty
 * @param fifo_p pointer to a fifo_t variable
 */
void fifo_clear(fifo_t * fifo_p);

/**
 * Returns with the number of free spaces in the fifo (max. 'item_num')
 * @param fifo_p pointer to a fifo_t variable
 * @return Free spaces is the fifo. (0 means the fifo is full)
 */
uint32_t fifo_get_free(fifo_t * fifo_p);

/**********************
 *      MACROS
 **********************/

#endif  /*USE_FIFO*/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif

/**
 * @file fifo.c
 * Classical First In First Out buffer implementation
 */

/*********************
 *      INCLUDES
 *********************/
#include "misc_conf.h"
#if USE_FIFO != 0

#include "fifo.h"
#include <stddef.h>
#include <string.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Initialize a fifo.
 * @param fifo_p pointer to a fifo_t variable
 * @param buf_p pointer to the buffer of the fifo (item_size * item_num bytes size )
 * @param item_size the of an item stored by the fifo (in bytes) 
 * @param item_num number of elements stored by the fifo
 */
void fifo_init(fifo_t * fifo_p, void * buf_p, uint32_t item_size, uint32_t item_num)
{   
    fifo_p->buf = buf_p;
    fifo_p->item_num = item_num;
    fifo_p->item_size = item_size;
    fifo_p->rp = 0;
    fifo_p->wp = 0;
    fifo_p->body.empty = 1;
    fifo_p->full = 0;
}

/**
 * Add item to the fifo
 * @param fifo_p pointer to fifo_t variable
 * @param data_p pointer to a data to add 
 * @return true: if the data is pushed, false: if the fifo was full
 */
bool fifo_push(fifo_t * fifo_p, const void * data_p)
{
    if(fifo_p->full != 0)  return false;
 
/* Copy data into the fifo */
    uint8_t * wr_begin = fifo_p->buf;
    wr_begin += (uint32_t) fifo_p->wp * fifo_p->item_size;        
    memcpy(wr_begin, data_p, fifo_p->item_size);

    /*Increase the write pointer*/
    fifo_p->wp ++;

    if(fifo_p->wp >= fifo_p->item_num) fifo_p->wp = 0;
    
    /*Set the fifo full if necessary*/
    if(fifo_p->rp == fifo_p->wp) fifo_p->full = 1;
    
    /*The fifo surely will not be empty after that*/
    fifo_p->body.empty = 0;
    
    return true;
}

/**
 * Add multiple data to the fifo
 * @param fifo_p pointer to a fifo_t variable
 * @param data_p pointer to the data to push
 * @param num number of items to push
 * @return number of pushed items (<= 'num')
 */
uint32_t fifo_push_mult(fifo_t * fifo_p, const void * data_p, uint32_t num)
{
    uint32_t push_num = 0;
    const uint8_t * data_u8_p = data_p;
    uint8_t suc;
    
    for(push_num = 0; push_num < num; push_num ++) {
        /* Push data*/
        suc = fifo_push(fifo_p, data_u8_p);
        
        /* Check the result and break on fail*/
        if(suc == 0) {
            break;
        } else {
            data_u8_p += fifo_p->item_size;
        }
    }
    
    return push_num;
}

/**
 * Read data from a fifo
 * @param fifo_p pointer to a fifo_t variable
 * @param data_p pointer to buffer to store the read data
 * @return true: if the data is popped, false: if the fifo was empty
 */
bool fifo_pop(fifo_t * fifo_p, void * data_p)
{
    if(fifo_p->body.empty != 0) return false;
    
    /* Copy data into the fifo */
    uint8_t  * rd_begin = fifo_p->buf;
    rd_begin += (uint32_t)fifo_p->rp * fifo_p->item_size;        
    memcpy(data_p, rd_begin, fifo_p->item_size);

    /*Increase the read pointer*/
    fifo_p->rp ++;

    if(fifo_p->rp >= fifo_p->item_num)  fifo_p->rp = 0;
    
    /*Set the fifo empty if necessary*/
    if(fifo_p->rp == fifo_p->wp) fifo_p->body.empty = 1;
    
    /*The fifo surely will not be full after that*/
    fifo_p->full = 0;

    return true;
}
/**
 * Read multiple data from a fifo
 * @param fifo_p pointer to fifo_t variable
 * @param data_p buffer to store the read data
 * @param num number of items to read
 * @return  number of popped items (<= 'num')
 */
uint32_t fifo_pop_mult(fifo_t * fifo_p, void * data_p, uint32_t num)
{
    uint32_t pop_num = 0;
    uint8_t * data_u8_p = data_p;
    uint8_t suc;
    
    for(pop_num = 0; pop_num < num; pop_num ++) {
        /* Pop data*/
        suc = fifo_pop(fifo_p, data_u8_p);
        
        /* Check the result and break on fail*/
        if(suc == 0) {
            break;
        }
        else {
            data_u8_p += fifo_p->item_size;
        }
    }
    
    return pop_num;
}

/**
 * Make the fifo empty
 * @param fifo_p pointer to a fifo_t variable
 */
void fifo_clear(fifo_t * fifo_p)
{
    fifo_p->rp = 0;
    fifo_p->wp = 0;
    fifo_p->body.empty = 1;
    fifo_p->full = 0;
}

/**
 * Returns with the number of free spaces in the fifo (max. 'item_num')
 * @param fifo_p pointer to a fifo_t variable
 * @return Free spaces is the fifo. (0 means the fifo is full)
 */
uint32_t fifo_get_free(fifo_t * fifo_p)
{
    uint32_t free_num;
    
    if(fifo_p->rp > fifo_p->wp) {
        free_num = fifo_p->rp - fifo_p->wp;
    } else if (fifo_p->rp < fifo_p->wp) {
        free_num = fifo_p->item_num - 
                   (fifo_p->wp - fifo_p->rp);
    } else {
        if(fifo_p->body.empty != 0) {
            free_num = fifo_p->item_num;
        } else {
            free_num = 0;
        }
    }
    
    return free_num;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif

/**
 * @file lv_disp.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#include "lv_disp.h"

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
 * Get the number of areas in the buffer
 * @return number of invalid areas
 */
uint16_t lv_disp_get_inv_buf_size(lv_disp_t * disp)
{
    return disp->inv_p;
}

/**
 * Pop (delete) the last 'num' invalidated areas from the buffer
 * @param num number of areas to delete
 */
void lv_disp_pop_from_inv_buf(lv_disp_t * disp, uint16_t num)
{

    if(disp->inv_p < num) disp->inv_p = 0;
    else disp->inv_p -= num;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

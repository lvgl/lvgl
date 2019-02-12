/**
 * @file lv_disp.h
 *
 */

#ifndef LV_DISP_H
#define LV_DISP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_hal/lv_hal.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Get the number of areas in the buffer
 * @return number of invalid areas
 */
uint16_t lv_disp_get_inv_buf_size(lv_disp_t * disp);

/**
 * Pop (delete) the last 'num' invalidated areas from the buffer
 * @param num number of areas to delete
 */
void lv_disp_pop_from_inv_buf(lv_disp_t * disp, uint16_t num);

/**********************
 *      MACROS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TEMPL_H*/

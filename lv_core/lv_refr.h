/**
 * @file lv_refr.h
 *
 */

#ifndef LV_REFR_H
#define LV_REFR_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "lv_obj.h"
#include <stdbool.h>

/*********************
 *      DEFINES
 *********************/
#ifndef LV_ATTRIBUTE_FLUSH_READY
#  define LV_ATTRIBUTE_FLUSH_READY
#endif

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
 * Initialize the screen refresh subsystem
 */
void lv_refr_init(void);

/**
 * Call in the display driver's  'disp_flush' function when the flushing is finished
 */
LV_ATTRIBUTE_FLUSH_READY void lv_flush_ready(lv_disp_t * disp);

/**
 * Redraw the invalidated areas now.
 * Normally the redrawing is periodically executed in `lv_task_handler` but a long blocking process can
 * prevent the call of `lv_task_handler`. In this case if the the GUI is updated in the process (e.g. progress bar)
 * this function can be called when the screen should be updated.
 */
void lv_refr_now(void);

/**
 * Invalidate an area on display to redraw it
 * @param area_p pointer to area which should be invalidated (NULL: delete the invalidated areas)
 * @param disp pointer to display where the area should be invalidated (NULL can be used if there is only one display)
 */
void lv_inv_area(lv_disp_t * disp, const lv_area_t * area_p);


/**
 * Set a function to call after every refresh to announce the refresh time and the number of refreshed pixels
 * @param cb pointer to a callback function (void my_refr_cb(uint32_t time_ms, uint32_t px_num))
 */
void lv_refr_set_monitor_cb(void (*cb)(uint32_t, uint32_t));

/**
 * Called when an area is invalidated to modify the coordinates of the area.
 * Special display controllers may require special coordinate rounding
 * @param cb pointer to the a function which will modify the area
 */
void lv_refr_set_round_cb(void(*cb)(lv_area_t*));

/**
 * Get the number of areas in the buffer
 * @return number of invalid areas
 */
uint16_t lv_refr_get_buf_size(void);

/**
 * Pop (delete) the last 'num' invalidated areas from the buffer
 * @param num number of areas to delete
 */
void lv_refr_pop_from_buf(uint16_t num);

/**
 * Get the display which is being refreshed
 * @return the display being refreshed
 */
lv_disp_t * lv_refr_get_disp_refreshing(void);

/**********************
 *   STATIC FUNCTIONS
 **********************/


#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_REFR_H*/

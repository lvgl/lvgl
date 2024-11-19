/**
 * @file lv_buttonmatrix_private.h
 *
 */

#ifndef LV_BUTTONMATRIX_PRIVATE_H
#define LV_BUTTONMATRIX_PRIVATE_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/

#include "../../core/lv_obj_private.h"
#include "lv_buttonmatrix.h"

#if LV_USE_BUTTONMATRIX != 0

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/** Data of button matrix */
struct _lv_buttonmatrix_t {
    lv_obj_t obj;
    const char * const * map_p;          /**< Pointer to the current map */
    lv_area_t * button_areas;            /**< Array of areas of buttons */
    lv_buttonmatrix_ctrl_t * ctrl_bits;  /**< Array of control bytes */
    uint32_t btn_cnt;                    /**< Number of button in 'map_p'(Handled by the library) */
    uint32_t row_cnt;                    /**< Number of rows in 'map_p'(Handled by the library) */
    uint32_t btn_id_sel;                 /**< Index of the active button (being pressed/released etc) or LV_BUTTONMATRIX_BUTTON_NONE */
    uint32_t one_check : 1;              /**< Single button toggled at once */
};


/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**********************
 *      MACROS
 **********************/

#endif /* LV_USE_BUTTONMATRIX != 0 */

#ifdef __cplusplus
} /*extern "C"*/
#endif

#endif /*LV_BUTTONMATRIX_PRIVATE_H*/

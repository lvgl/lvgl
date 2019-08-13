/**
 * @file lv_mask.h
 *
 */

#ifndef LV_MASK_H
#define LV_MASK_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include <stdbool.h>
#include "../lv_misc/lv_area.h"
#include "../lv_misc/lv_color.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    lv_point_t origo;
    lv_coord_t steep;
    uint8_t flat :1;
}lv_mask_line_param_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_mask_apply(lv_color_t * dest_buf, lv_color_t * src_buf, lv_opa_t * mask_buf, lv_coord_t len);
void lv_mask_line_left(lv_opa_t * mask_buf, lv_coord_t abs_x, lv_coord_t abs_y, lv_coord_t len, bool inner, void * param);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_MASK_H*/

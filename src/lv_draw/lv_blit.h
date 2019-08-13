/**
 * @file lv_blit.h
 *
 */

#ifndef LV_BLIT_H
#define LV_BLIT_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#include "../lv_misc/lv_color.h"
#include "../lv_misc/lv_area.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/
typedef enum {
    LV_BLIT_MODE_NORMAL,
    LV_BLIT_MODE_ADDITIVE,
}lv_blit_mode_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void lv_blit_color(lv_color_t * dest_buf, lv_color_t * bg_buf, lv_coord_t len, lv_color_t color, lv_blit_mode_t mode);

/**********************
 *      MACROS
 **********************/

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_BLIT_H*/

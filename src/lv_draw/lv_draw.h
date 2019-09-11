/**
 * @file lv_draw.h
 *
 */

#ifndef LV_DRAW_H
#define LV_DRAW_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************
 *      INCLUDES
 *********************/
#ifdef LV_CONF_INCLUDE_SIMPLE
#include "lv_conf.h"
#else
#include "../../../lv_conf.h"
#endif

#include "../lv_core/lv_style.h"
#include "../lv_misc/lv_txt.h"
#include "lv_img_decoder.h"

/*********************
 *      DEFINES
 *********************/
#define LV_DRAW_BUF_MAX_NUM    6

/**********************
 *      TYPEDEFS
 **********************/
typedef struct {
    void * p;
    uint16_t size;
    uint8_t used    :1;
}lv_draw_buf_t;

/**********************
 * GLOBAL PROTOTYPES
 **********************/

/**
 * Give a buffer with the given to use during drawing.
 * Be careful to not use the buffer while other processes are using it.
 * @param size the required size
 */
void * lv_draw_buf_get(uint32_t size);

/**
 * Release the draw buffer
 * @param p buffer to release
 */
void lv_draw_buf_release(void * p);

/**
 * Free all draw buffers
 */
void lv_draw_buf_free_all(void);


/**********************
 *  GLOBAL VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   POST INCLUDES
 *********************/
#include "lv_draw_rect.h"
#include "lv_draw_label.h"
#include "lv_draw_img.h"
#include "lv_draw_line.h"
#include "lv_draw_triangle.h"
#include "lv_draw_arc.h"
#include "lv_draw_blend.h"
#include "lv_draw_mask.h"

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_DRAW_H*/

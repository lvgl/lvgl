/**
 * @file lv_draw_buf_vg_lite.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_vg_lite.h"

#if LV_USE_DRAW_VG_LITE

#include "../lv_draw_buf_private.h"
#include "lv_vg_lite_utils.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void init_handlers(lv_draw_buf_handlers_t * handlers);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_buf_vg_lite_init_handlers(void)
{
    init_handlers(lv_draw_buf_get_handlers());
    init_handlers(lv_draw_buf_get_font_handlers());
    init_handlers(lv_draw_buf_get_image_handlers());
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static uint32_t width_to_stride(uint32_t w, lv_color_format_t color_format)
{
    return lv_vg_lite_width_to_stride(w, lv_vg_lite_vg_fmt(color_format));
}

static void init_handlers(lv_draw_buf_handlers_t * handlers)
{
    LV_ASSERT_NULL(handlers);
    handlers->width_to_stride_cb = width_to_stride;
}

#endif /*LV_USE_DRAW_VG_LITE*/

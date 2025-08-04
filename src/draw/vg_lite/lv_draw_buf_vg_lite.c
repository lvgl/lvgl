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

static void * align_pointer_cb(void * buf, lv_color_format_t color_format)
{
    if(!buf) {
        return NULL;
    }

    uint32_t align = LV_DRAW_BUF_ALIGN;

    /* To avoid warnings when constructing draw_buf for static bitmap fonts */
    if(color_format == LV_COLOR_FORMAT_A8) {
        align = 16;
    }

    return (void *)LV_ROUND_UP((lv_uintptr_t)buf, align);
}

static uint32_t width_to_stride_cb(uint32_t w, lv_color_format_t color_format)
{
    return lv_vg_lite_width_to_stride(w, lv_vg_lite_vg_fmt(color_format));
}

static void init_handlers(lv_draw_buf_handlers_t * handlers)
{
    LV_ASSERT_NULL(handlers);
    handlers->align_pointer_cb = align_pointer_cb;
    handlers->width_to_stride_cb = width_to_stride_cb;
}

#endif /*LV_USE_DRAW_VG_LITE*/

/**
 * @file lv_nuttx_cache.c
 *
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_nuttx_cache.h"
#include "../../../lvgl.h"

#if LV_USE_NUTTX

#include <nuttx/cache.h>

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void invalidate_cache(void * buf, uint32_t stride, lv_color_format_t color_format, const lv_area_t * area);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_nuttx_cache_init(void)
{
    lv_draw_buf_handlers_t * handlers = lv_draw_buf_get_handlers();
    handlers->invalidate_cache_cb = invalidate_cache;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void invalidate_cache(void * buf, uint32_t stride, lv_color_format_t color_format, const lv_area_t * area)
{
    LV_UNUSED(color_format);
    LV_ASSERT_NULL(buf);

    lv_uintptr_t start;
    lv_uintptr_t end;

    if(area) {
        int32_t h = lv_area_get_height(area);
        start = (lv_uintptr_t)buf + area->y1 * stride;
        end = start + h * stride;
    }
    else {
        /* If area is empty, use stride as length */
        start = (lv_uintptr_t)buf;
        end = start + stride;
    }

    LV_UNUSED(start);
    LV_UNUSED(end);
    up_invalidate_dcache(start, end);
}

#endif /* LV_USE_NUTTX */

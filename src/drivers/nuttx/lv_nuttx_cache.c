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

static void invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area);

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

static void invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area)
{
    LV_ASSERT_NULL(draw_buf);
    void * buf = draw_buf->data;
    uint32_t stride = draw_buf->header.stride;

    lv_uintptr_t start;
    lv_uintptr_t end;

    int32_t h = lv_area_get_height(area);
    start = (lv_uintptr_t)buf + area->y1 * stride;
    end = start + h * stride;

    LV_UNUSED(start);
    LV_UNUSED(end);
    up_invalidate_dcache(start, end);
}

#endif /* LV_USE_NUTTX */

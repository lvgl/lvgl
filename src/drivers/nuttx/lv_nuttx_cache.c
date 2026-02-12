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

#include "../../draw/lv_draw_buf_private.h"
#include "../../core/lv_global.h"

#ifdef __NuttX__
    #include <nuttx/cache.h>
#else
    #include "mock/nuttx_cache.h"
#endif

/*********************
 *      DEFINES
 *********************/
#define image_cache_draw_buf_handlers &(LV_GLOBAL_DEFAULT()->image_cache_draw_buf_handlers)
#define font_draw_buf_handlers &(LV_GLOBAL_DEFAULT()->font_draw_buf_handlers)

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area);
static void flush_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area);

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
    handlers->flush_cache_cb = flush_cache;

    handlers = image_cache_draw_buf_handlers;
    handlers->invalidate_cache_cb = invalidate_cache;
    handlers->flush_cache_cb = flush_cache;

    handlers = font_draw_buf_handlers;
    handlers->invalidate_cache_cb = invalidate_cache;
    handlers->flush_cache_cb = flush_cache;
}

void lv_nuttx_cache_deinit(void)
{
    lv_draw_buf_handlers_t * handlers = lv_draw_buf_get_handlers();
    handlers->invalidate_cache_cb = NULL;
    handlers->flush_cache_cb = NULL;

    handlers = image_cache_draw_buf_handlers;
    handlers->invalidate_cache_cb = NULL;
    handlers->flush_cache_cb = NULL;

    handlers = font_draw_buf_handlers;
    handlers->invalidate_cache_cb = NULL;
    handlers->flush_cache_cb = NULL;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void draw_buf_to_region(
    const lv_draw_buf_t * draw_buf, const lv_area_t * area,
    lv_uintptr_t * start, lv_uintptr_t * end)
{
    LV_ASSERT_NULL(draw_buf);
    LV_ASSERT_NULL(area);
    LV_ASSERT_NULL(start);
    LV_ASSERT_NULL(end);

    void * buf = draw_buf->data;
    uint32_t stride = draw_buf->header.stride;

    int32_t h = lv_area_get_height(area);
    *start = (lv_uintptr_t)buf + area->y1 * stride;
    *end = *start + h * stride;
}

static void invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area)
{
    lv_uintptr_t start;
    lv_uintptr_t end;
    draw_buf_to_region(draw_buf, area, &start, &end);
    up_invalidate_dcache(start, end);
}

static void flush_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area)
{
    lv_uintptr_t start;
    lv_uintptr_t end;
    draw_buf_to_region(draw_buf, area, &start, &end);
    up_flush_dcache(start, end);
}

#endif /* LV_USE_NUTTX */

/**
 * @file lv_draw_buf_g2d.c
 *
 */

/**
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: MIT
 */

/*********************
 *      INCLUDES
 *********************/

#include "lv_draw_g2d.h"

#if LV_USE_DRAW_G2D
#include "../../lv_draw_buf_private.h"
#include "g2d.h"
#include "lv_g2d_buf_map.h"
#include "lv_g2d_utils.h"


/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void * _buf_malloc(size_t size_bytes, lv_color_format_t color_format);

static void _buf_free(void * buf);

static void _invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area);

/**********************
 *  STATIC VARIABLES
 **********************/

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_draw_buf_g2d_init_handlers(void)
{
    lv_draw_buf_handlers_t * handlers = lv_draw_buf_get_handlers();

    handlers->buf_malloc_cb = _buf_malloc;
    handlers->buf_free_cb = _buf_free;
    handlers->invalidate_cache_cb = _invalidate_cache;
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void * _buf_malloc(size_t size_bytes, lv_color_format_t color_format)
{
    LV_UNUSED(color_format);

    size_bytes += LV_DRAW_BUF_ALIGN - 1;
    struct g2d_buf * buf = g2d_alloc(size_bytes, 1);
    G2D_ASSERT_MSG(buf, "Failed to alloc buffer.");
    g2d_insert_buf_map(buf->buf_vaddr, buf);
    return buf->buf_vaddr;
}

static void _buf_free(void * buf)
{
    // make sure items are in the hash table
    struct g2d_buf * g2d_buf = g2d_search_buf_map(buf);
    if(g2d_buf) {
        g2d_free_item(buf);
    }
}

static void _invalidate_cache(const lv_draw_buf_t * draw_buf, const lv_area_t * area)
{

    LV_UNUSED(area);
    struct g2d_buf * buf = g2d_search_buf_map(draw_buf->data);
    G2D_ASSERT_MSG(buf, "Failed to find buffer in map.");
    g2d_cache_op(buf, G2D_CACHE_FLUSH);
}

#endif /*LV_USE_DRAW_G2D*/
